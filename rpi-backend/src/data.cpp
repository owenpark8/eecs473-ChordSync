#ifdef DEBUG
#include <iostream>
#endif
#include <filesystem>
#include <stdexcept>

#include <fmt/format.h>

#include "data.hpp"

namespace data {
    std::string const data_directory = std::string(USER_HOME_DIR) + ".local/share/vsguitar/";
    std::string const db_filename = data_directory + "vsguitar.db";


    auto init() -> bool {
        namespace fs = std::filesystem;
#ifdef DEBUG
        std::cout << "Initializing app data...\n";
        std::cout << "Checking if data directory \"" << data::data_directory << "\" exists...\n";
#endif
        if (!fs::exists(data::data_directory)) {
#ifdef DEBUG
            std::cout << "Data directory does not exist! Creating directory...\n";
#endif
            if (!data::create_directory_if_not_exists(data::data_directory)) {
#ifdef DEBUG
                std::cerr << "Critical error: Data directory could not be created. Exiting.\n";
#endif
                return false;
            }
        }
#ifdef DEBUG
        else {
            std::cout << "Data directory found!\n";
        }
#endif

        try {
            SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            data::songs::create_table_if_not_exists(db);
        } catch (std::exception& e) {
#ifdef DEBUG
            std::cerr << "SQLite exception: " << e.what() << std::endl;
#endif
            return false;
        }

        return true;
    }

    auto create_directory_if_not_exists(std::string const& directory) -> bool {
        namespace fs = std::filesystem;
        if (!fs::exists(directory)) {
            try {
                return fs::create_directories(directory);
            } catch (std::filesystem::filesystem_error const& e) {
                return false;
            } catch (std::bad_alloc const& e) {
                return false;
            }
        }

        return true;
    }

    namespace songs {
        std::string const song_table_name = "songs";
        std::string const song_table_schema = "(id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT NOT NULL, artist TEXT NOT NULL, length INTEGER NOT "
                                              "NULL, bpm INTEGER NOT NULL, notes BLOB)";

        auto serialize_notes(std::vector<Note> const& notes) -> std::vector<uint8_t> {
            std::vector<uint8_t> blob;
            blob.reserve(sizeof(Note) * notes.size());

            for (auto const& note: notes) {
                uint32_t start_timestamp = note.start_timestamp_ms;
                uint16_t length = note.length_ms;
                uint8_t midi = note.midi_note;
                uint8_t fret_and_string = (note.fret & 0x0F) | ((note.string & 0x0F) << 4); // Combine fret and string

                blob.insert(blob.end(), reinterpret_cast<uint8_t*>(&start_timestamp),
                            reinterpret_cast<uint8_t*>(&start_timestamp) + sizeof(start_timestamp));
                blob.insert(blob.end(), reinterpret_cast<uint8_t*>(&length), reinterpret_cast<uint8_t*>(&length) + sizeof(length));
                blob.push_back(midi);
                blob.push_back(fret_and_string);
            }
            return blob;
        }

        auto deserialize_notes(std::vector<uint8_t> const& blob) -> std::vector<Note> {
            std::vector<Note> notes;
            size_t offset = 0;

            while (offset < blob.size()) {
                Note note{};

                note.start_timestamp_ms = *reinterpret_cast<uint32_t const*>(&blob[offset]);
                offset += sizeof(note.start_timestamp_ms);

                note.length_ms = *reinterpret_cast<uint16_t const*>(&blob[offset]);
                offset += sizeof(note.length_ms);

                note.midi_note = blob[offset++];

                std::uint8_t fret_and_string = blob[offset++];
                note.fret = fret_and_string & 0x0F;
                note.string = (fret_and_string >> 4) & 0x0F;


                notes.push_back(note);
            }
            return notes;
        }

        auto populate_song_info(SQLite::Statement& query) -> SongInfo {
            SongInfo song;
            song.id = query.getColumn(0).getUInt();
            song.title = query.getColumn(1).getString();
            song.artist = query.getColumn(2).getString();
            song.bpm = query.getColumn(3).getUInt();
            song.length = std::chrono::seconds(query.getColumn(4).getUInt());

            void const* blob = query.getColumn(5).getBlob();
            int blob_size = query.getColumn(5).getBytes();
            std::vector<uint8_t> notes_blob(static_cast<uint8_t const*>(blob), static_cast<uint8_t const*>(blob) + blob_size);
            song.notes = deserialize_notes(notes_blob);

            return song;
        }

        auto create_table_if_not_exists(SQLite::Database& db) -> void {
            bool const table_exists = db.tableExists(data::songs::song_table_name);

            if (!table_exists) {
#ifdef DEBUG
                std::cout << "Songs table does not exist. Creating table...\n";
#endif
                db.exec("CREATE TABLE " + data::songs::song_table_name + " " + data::songs::song_table_schema);
            }
        }

        auto insert_new_song(SQLite::Database& db, SongInfo const& song) -> void {
            SQLite::Statement query(db,
                                    "INSERT INTO " + data::songs::song_table_name + " (title, artist, bpm, length, notes) VALUES (?, ?, ?, ?, ?)");
            query.bind(1, song.title);
            query.bind(2, song.artist);
            query.bind(3, song.length.count());
            query.bind(4, song.bpm);
            auto notes_blob = serialize_notes(song.notes);
            query.bind(5, notes_blob.data(), static_cast<int>(notes_blob.size()));

            query.exec();
        }

        auto get_all_songs(SQLite::Database& db) -> std::vector<SongInfo> {
            std::vector<SongInfo> songs;

            SQLite::Statement query(db, "SELECT id, title, artist, bpm, length, notes FROM " + data::songs::song_table_name);

            while (query.executeStep()) {
                songs.push_back(populate_song_info(query));
            }

            return songs;
        }

        auto song_id_exists(SQLite::Database& db, std::uint8_t song_id) -> bool {
#ifdef DEBUG
            std::cout << "Querying database for song id " << static_cast<int>(song_id) << "\n";
#endif
            SQLite::Statement query(db, "SELECT id, title, artist, bpm, length, notes FROM " + data::songs::song_table_name + " WHERE id = ?");
            query.bind(1, song_id);

            if (query.executeStep()) {
                return true;
            }
            return false;
        }

        auto get_song_by_id(SQLite::Database& db, std::uint8_t song_id) -> SongInfo {
#ifdef DEBUG
            std::cout << "Querying database for song id " << static_cast<int>(song_id) << "\n";
#endif
            SQLite::Statement query(db, "SELECT id, title, artist, bpm, length, notes FROM " + data::songs::song_table_name + " WHERE id = ?");
            query.bind(1, song_id);

            if (query.executeStep()) {
                return populate_song_info(query);
            }
            throw std::runtime_error(fmt::format("No song with id {} found!", song_id));
        }

    } // namespace songs

} // namespace data

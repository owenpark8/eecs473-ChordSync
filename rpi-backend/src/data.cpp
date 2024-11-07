#ifdef DEBUG
#include <iostream>
#endif
#include <filesystem>

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
        std::string const song_table_schema = "(id INTEGER PRIMARY KEY, title TEXT NOT NULL, artist TEXT NOT NULL, length INTEGER NOT NULL)";

        auto create_table_if_not_exists(SQLite::Database& db) -> void {
            bool const table_exists = db.tableExists(data::songs::song_table_name);

            if (!table_exists) {
                db.exec("CREATE TABLE " + data::songs::song_table_name + " " + data::songs::song_table_schema);
            }
        }

        auto insert_new_song(SQLite::Database& db, SongInfo const& song) -> void {
            SQLite::Statement query(db, "INSERT INTO " + data::songs::song_table_name + " (title, artist, length) VALUES (?, ?, ?)");
            query.bind(1, song.title);
            query.bind(2, song.artist);
            query.bind(3, static_cast<uint32_t>(song.length.count()));
            query.exec();
        }

        auto get_all_songs(SQLite::Database& db) -> std::vector<SongInfo> {
            std::vector<SongInfo> songs;

            SQLite::Statement query(db, "SELECT id, title, artist, length FROM " + data::songs::song_table_name);

            while (query.executeStep()) {
                SongInfo song;
                song.id = query.getColumn(0).getUInt();
                song.title = query.getColumn(1).getString();
                song.artist = query.getColumn(2).getString();
                song.length = std::chrono::milliseconds(query.getColumn(3).getUInt());

                songs.push_back(song);
            }

            return songs;
        }

    } // namespace songs

} // namespace data

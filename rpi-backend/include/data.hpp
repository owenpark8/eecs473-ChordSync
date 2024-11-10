#pragma once

#ifdef DEBUG
#include <iostream>
#endif
#include <chrono>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

namespace data {
    extern std::string const data_directory;
    extern std::string const db_filename;

    auto init() -> bool;

    /**
     * @brief Checks if a directory exists and creates it if it does not exist
     *
     * @param directory the full pathname of the directory to check
     */
    auto create_directory_if_not_exists(std::string const& directory) -> bool;

    namespace songs {

        struct __attribute__((packed)) Note {
            std::uint32_t start_timestamp_ms;
            std::uint16_t length_ms;
            std::uint8_t midi_note;
            std::uint8_t fret : 4;
            std::uint8_t string : 4;

#ifdef DEBUG
            auto print() const -> void {
                std::cout << "start timestamp (ms): " << static_cast<int>(start_timestamp_ms) << " | length (ms): " << static_cast<int>(length_ms)
                          << " | midi note: " << static_cast<int>(midi_note) << " | fret: " << static_cast<int>(fret)
                          << " | string: " << static_cast<int>(string) << "\n";
            }
#endif
        };

        struct SongInfo {
            std::uint8_t id;
            std::string title;
            std::string artist;
            std::chrono::seconds length = std::chrono::seconds(0);
            std::uint16_t bpm;
            std::vector<Note> notes;
        };

        extern std::string const song_table_name;
        extern std::string const song_table_schema;

        auto create_table_if_not_exists(SQLite::Database& db) -> void;
        auto insert_new_song(SQLite::Database& db, SongInfo const& song) -> void;
        auto get_all_songs(SQLite::Database& db) -> std::vector<SongInfo>;
        auto get_song_by_id(SQLite::Database& db, std::uint8_t song_id) -> SongInfo;

    } // namespace songs

} // namespace data

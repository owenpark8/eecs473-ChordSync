#pragma once

#ifdef DEBUG
#include <iostream>
#endif
#include <chrono>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include "guitar.hpp"

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
        auto song_id_exists(SQLite::Database& db, std::uint8_t song_id) -> bool;
        auto get_song_by_id(SQLite::Database& db, std::uint8_t song_id) -> SongInfo;

#ifdef DEBUG
        static SongInfo const baby_shark = {.title = "baby shark",
                                            .artist = "doodoodoodoodoo",
                                            .length = std::chrono::seconds(137),
                                            .bpm = 115,
                                            .notes = {
                                                    data::songs::Note{.start_timestamp_ms = 0,
                                                                      .length_ms = 417,
                                                                      .midi_note = 0,
                                                                      .fret = 0,
                                                                      .string = static_cast<uint8_t>(string_e::HIGH_E)},
                                            }};
#endif

        static SongInfo const ode_to_joy =
                {
                        .title = "Ode To Joy",
                        .artist = "ChordSync Basics",
                        .length = std::chrono::seconds(13),
                        .bpm = 144,
                        .notes =
                                {
                                        Note{.start_timestamp_ms = 0,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 417,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 834,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 1251,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 1668,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 2085,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 2502,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 2919,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 3336,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 3753,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 4170,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 4587,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 5004,
                                             .length_ms = 625,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 5629,
                                             .length_ms = 208,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 5837,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 6254,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 6671,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 7088,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 7505,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 7922,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 8339,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 8756,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 9173,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 9590,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 10007,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 10424,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 10841,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 0,
                                             .string = static_cast<std::uint8_t>(string_e::HIGH_E)},
                                        Note{.start_timestamp_ms = 11258,
                                             .length_ms = 682,
                                             .midi_note = 0,
                                             .fret = 3,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 11940,
                                             .length_ms = 208,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                        Note{.start_timestamp_ms = 12148,
                                             .length_ms = 417,
                                             .midi_note = 0,
                                             .fret = 1,
                                             .string = static_cast<std::uint8_t>(string_e::B)},
                                },
        };

        namespace chords {
            static SongInfo const a_major_chord = {
                    .title = "A Major Chord",
                    .artist = "Chords",
                    .length = std::chrono::seconds(10),
                    .bpm = 1,
                    .notes = {Note{

                    }},
            };
        }


    } // namespace songs

} // namespace data

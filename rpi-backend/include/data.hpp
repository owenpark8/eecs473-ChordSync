#include <chrono>
#include <string>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

namespace data {
    extern std::string const data_directory;
    extern std::string const db_filename;

    /**
     * @brief Checks if a directory exists and creates it if it does not exist
     *
     * @param directory the full pathname of the directory to check
     */
    auto create_directory_if_not_exists(std::string const& directory) -> bool;

    namespace songs {
        struct SongInfo {
            int id;
            std::string title;
            std::string artist;
            std::chrono::milliseconds length = std::chrono::milliseconds(0);
        };

        extern std::string const song_table_name;
        extern std::string const song_table_schema;

        auto create_table_if_not_exists(SQLite::Database& db) -> void;
        auto insert_new_song(SQLite::Database& db, SongInfo const& song) -> void;
        auto get_all_songs(SQLite::Database& db) -> std::vector<SongInfo>;

    } // namespace songs

} // namespace data

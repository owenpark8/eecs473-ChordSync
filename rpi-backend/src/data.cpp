#include <filesystem>
#include <fstream>
#include <optional>

#include "data.hpp"


namespace data {

    std::string const data_directory = std::string(USER_HOME_DIR) + "/.local/share/vsguitar";

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
        std::string const song_directory = data_directory + "/songs";
        std::string const song_info_filename = "info.txt";

        std::shared_mutex m_song_directory_mutex{};
        std::unordered_map<std::string, SongInfo> m_song_id_to_info = get_all_songs();

        auto generate_timestamp_id() -> std::string {
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            return std::to_string(now_ms);
        }
        auto get_all_songs() -> std::unordered_map<std::string, SongInfo> {
            std::unordered_map<std::string, SongInfo> songs_map;
            namespace fs = std::filesystem;

            std::shared_lock lock(m_song_directory_mutex);
            for (auto const& entry: fs::directory_iterator(song_directory)) {
                if (entry.is_directory()) {
                    std::string const song_dir = entry.path().string();
                    std::string const song_id = entry.path().filename().string();

                    auto song_info = SongInfo::read_from_file(song_dir);
                    if (song_info) {
                        songs_map[song_id] = *song_info;
                    }
                }
            }

            return songs_map;
        }

        auto write_song_info_to_file(std::string const& id, SongInfo const& info) -> bool {
            namespace fs = std::filesystem;
            std::string const song_dir = song_directory + "/" + id;

            std::unique_lock lock(m_song_directory_mutex);
            try {
                if (!fs::exists(song_dir)) {
                    fs::create_directories(song_dir);
                }

                if (info.write_to_file(song_dir)) {
                    return true;
                } else {
                    return false;
                }
            } catch (fs::filesystem_error const& e) {
                return false;
            }
        }


        auto SongInfo::write_to_file(std::string const& dir) const -> bool {
            std::ofstream out_file(dir + "/" + song_info_filename);
            if (!out_file) {
                return false;
            }
            out_file << title << '\n' << artist << '\n' << length.count() << '\n';
            return true;
        }

        auto SongInfo::read_from_file(std::string const& dir) -> std::optional<SongInfo> {
            SongInfo song_info;
            std::ifstream in_file(dir + "/" + song_info_filename);
            if (in_file) {
                std::getline(in_file, song_info.title);
                std::getline(in_file, song_info.artist);

                unsigned long long milliseconds;
                in_file >> milliseconds;
                song_info.length = std::chrono::milliseconds(milliseconds);

                if (in_file.fail()) {
                    return std::nullopt;
                }

            } else {
                return std::nullopt;
            }
            return song_info;
        }
    } // namespace songs

} // namespace data

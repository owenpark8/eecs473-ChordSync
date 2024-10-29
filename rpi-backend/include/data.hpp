#include <chrono>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace data {
    extern std::string const data_directory;

    auto create_directory_if_not_exists(std::string const& directory) -> bool;

    namespace songs {
        struct SongInfo {
            std::string title;
            std::string artist;
            std::chrono::milliseconds length = std::chrono::milliseconds(0);

            [[nodiscard]] auto write_to_file(std::string const& dir) const -> bool;

            static auto read_from_file(std::string const& dir) -> std::optional<SongInfo>;
        };

        extern std::string const song_info_filename;
        extern std::string const song_directory;
        extern std::shared_mutex m_song_directory_mutex;
        extern std::unordered_map<std::string, SongInfo> m_song_id_to_info;

        auto generate_timestamp_id() -> std::string;

        auto get_all_songs() -> std::unordered_map<std::string, SongInfo>;

        auto write_song_info_to_file(std::string const& id, SongInfo const& info) -> bool;

    } // namespace songs

} // namespace data

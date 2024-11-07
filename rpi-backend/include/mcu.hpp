#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>

#include "guitar.hpp"

namespace mcu {
    enum class song_status_e {
        UNKNOWN,
        LOADING,
        STOPPED,
        PLAYING,
    };

    using song_id_t = std::uint8_t;

    auto start_song_loading(std::uint8_t id) -> void;
    auto send_note(std::chrono::milliseconds timestamp, fret_t fret, string_e string) -> void;
    auto get_and_update_loaded_song_id() -> void;
    auto play_loaded_song() -> void;

    extern std::mutex mut;
    extern song_id_t current_song_id;
    extern song_status_e current_song_status;
} // namespace mcu

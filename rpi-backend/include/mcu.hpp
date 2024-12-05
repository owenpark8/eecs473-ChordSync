#pragma once

#include <cstdint>
#include <mutex>
#include <string>

#include <fmt/format.h>

#include "data.hpp"
#include "messaging.hpp"

namespace mcu {
    using song_id_t = std::uint8_t;

    constexpr std::chrono::seconds ACK_TIMEOUT = std::chrono::seconds(5);

    class NoACKException : public std::exception {
    private:
        std::string m_message;

    public:
        explicit NoACKException(std::string const& details) : m_message(fmt::format("Did not receive ACK! {}", details)) {}

        [[nodiscard]] auto what() const noexcept -> char const* override { return m_message.c_str(); }
    };

    class NoMsgException : public std::exception {
    private:
        std::string m_message;

    public:
        explicit NoMsgException(std::string const& details) : m_message(fmt::format("Did not receive message! {}", details)) {}

        [[nodiscard]] auto what() const noexcept -> char const* override { return m_message.c_str(); }
    };

    class UnexpectedMsgException : public std::exception {
    private:
        std::string m_message;

    public:
        explicit UnexpectedMsgException(std::string const& details) : m_message(fmt::format("Received Unexpected message! {}", details)) {}

        [[nodiscard]] auto what() const noexcept -> char const* override { return m_message.c_str(); }
    };

    auto send_song(data::songs::SongInfo const& song) -> void;
    [[nodiscard]] auto get_loaded_song_id() -> std::uint8_t;
    auto play_loaded_song() -> void;
    auto end_loaded_song() -> void;

    // Backdoor
    auto send_control_message(ControlMessage const& message) -> void;

    extern std::mutex mut;
    extern song_id_t current_song_id;
    extern bool playing;
} // namespace mcu

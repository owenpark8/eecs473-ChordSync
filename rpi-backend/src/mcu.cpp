#include <array>
#include <cstdint>

#include "messaging.hpp"
#include "serial.hpp"

#include "mcu.hpp"

namespace mcu {
    auto send_control_message(ControlMessage const& message) -> void { serial::send(message.data(), sizeof(ControlMessage)); }

    auto send_data_message(DataMessage const& message) -> void {
        std::visit([](auto const& msg) { serial::send(reinterpret_cast<uint8_t const*>(&msg), sizeof(msg)); }, message);
    }

    [[nodiscard]] auto receive_ack() -> bool {
        ControlMessage msg{};
        serial::receive(msg.data(), msg.size());

        return (msg == ACK_MESSAGE);
    }

    auto start_song_loading(std::uint8_t id) -> void {
        send_control_message(START_SONG_LOADING_MESSAGE);
        // receive_ack();
        send_data_message(StartSongLoadingDataMessage{id});
        // receive_ack();
    }
    auto send_note(std::uint32_t timestamp_ms, std::uint16_t length_ms, std::uint8_t fret, std::uint8_t string) -> void {
        NoteDataMessage msg{
                .timestamp_ms = timestamp_ms,
                .length_ms = length_ms,
                .fret = static_cast<std::uint8_t>(fret),
                .string = static_cast<std::uint8_t>(string),
        };
        send_control_message(NOTE_MESSAGE);
        // receive_ack();
        send_data_message(msg);
        // receive_ack();
    }

    auto get_and_update_loaded_song_id() -> void { send_control_message(REQUEST_SONG_ID_MESSAGE); }

    auto play_loaded_song() -> void { send_control_message(START_SONG_MESSAGE); }

    std::mutex mut{};
    std::uint8_t current_song_id = 0x00;
    song_status_e current_song_status = song_status_e::UNKNOWN;
} // namespace mcu

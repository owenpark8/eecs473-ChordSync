#include <array>
#include <chrono>
#include <cstdint>
#include <stdexcept>

#include "messaging.hpp"
#include "serial.hpp"

#include "mcu.hpp"

namespace mcu {
    auto send_control_message(ControlMessage const& message) -> void { serial::send(message.data(), sizeof(ControlMessage)); }

    template<typename T>
    struct is_valid_data_message : std::false_type {};
    template<>
    struct is_valid_data_message<StartSongLoadingDataMessage> : std::true_type {};
    template<>
    struct is_valid_data_message<NoteDataMessage> : std::true_type {};
    template<>
    struct is_valid_data_message<LoadedSongDataMessage> : std::true_type {};

    template<typename MessageType>
    auto send_data_message(MessageType const& message) -> typename std::enable_if<is_valid_data_message<MessageType>::value, void>::type {
        serial::send(MESSAGE_HEADER);
        serial::send(reinterpret_cast<std::uint8_t const*>(&message), sizeof(MessageType));
    }

    [[nodiscard]] auto receive_ack() -> bool {
        using namespace std::chrono_literals;

        ControlMessage msg{};
        serial::receive(msg.data(), msg.size(), 5s);

        return (msg == ACK_MESSAGE);
    }

    auto start_song_loading(std::uint8_t id) -> void {
        send_control_message(START_SONG_LOADING_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not start song loading: did not receive ACK!");
        }
        send_data_message(StartSongLoadingDataMessage{id});
        if (!receive_ack()) {
            throw std::runtime_error("Could not start song loading: did not receive ACK!");
        }
    }
    auto send_note(std::uint32_t timestamp_ms, std::uint16_t length_ms, std::uint8_t fret, std::uint8_t string) -> void {
        NoteDataMessage msg{
                .timestamp_ms = timestamp_ms,
                .length_ms = length_ms,
                .fret = static_cast<std::uint8_t>(fret),
                .string = static_cast<std::uint8_t>(string),
        };

        send_control_message(NOTE_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not send note: did not receive ACK!");
        }
        send_data_message(msg);
        if (!receive_ack()) {
            throw std::runtime_error("Could not send note: did not receive ACK!");
        }
    }

    auto end_song_loading() -> void {
        send_control_message(END_SONG_LOADING_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not end song loading: did not receive ACK!");
        }
    }

    auto get_and_update_loaded_song_id() -> void {
        send_control_message(REQUEST_SONG_ID_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not request song ID: did not receive ACK!");
        }
        std::uint8_t buf[2] = {0};
        serial::receive(buf, 2);
        if (buf[0] != 0x01) {
            throw std::runtime_error("Could not request song ID: received malformed data");
        }
        send_control_message(ACK_MESSAGE);
        current_song_id = buf[1];
    }

    auto play_loaded_song() -> void {
        send_control_message(START_SONG_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not play loaded song: did not receive ACK!");
        }
    }

    auto end_loaded_song() -> void {
        send_control_message(END_SONG_MESSAGE);
        if (!receive_ack()) {
            throw std::runtime_error("Could not play loaded song: did not receive ACK!");
        }
    }
    std::mutex mut{};

    std::mutex song_info_mut{};
    std::uint8_t current_song_id = 0x00;
    song_status_e current_song_status = song_status_e::UNKNOWN;

} // namespace mcu

#include <array>
#include <cstdint>
#include <stdexcept>

#include "data.hpp"
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
        ControlMessage msg{};
        serial::receive(msg.data(), msg.size(), ACK_TIMEOUT);
        return (msg == ACK_MESSAGE);
    }
    auto send_reset() -> void {
        send_control_message(CLEAR_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not clear screens");
        }
    }

    auto send_clear() -> void {
        send_control_message(CLEAR_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not clear screens");
        }
    }

    auto send_song(data::songs::SongInfo const& song) -> void {
        send_control_message(START_SONG_LOADING_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not start song loading");
        }
        send_data_message(StartSongLoadingDataMessage{song.id});
        if (!receive_ack()) {
            throw NoACKException("Could not start song loading");
        }

        for (auto const& note: song.notes) {
            NoteDataMessage msg{
                    .timestamp_ms = note.start_timestamp_ms,
                    .length_ms = note.length_ms,
                    .fret = static_cast<std::uint8_t>(note.fret),
                    .string = static_cast<std::uint8_t>(note.string),
            };
            send_control_message(NOTE_MESSAGE);
            if (!receive_ack()) {
                throw NoACKException("Could not send note");
            }
            send_data_message(msg);
            if (!receive_ack()) {
                throw NoACKException("Could not send note");
            }
        }

        send_control_message(END_SONG_LOADING_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not end song loading");
        }
    }

    [[nodiscard]] auto get_loaded_song_id() -> std::uint8_t {
        send_control_message(REQUEST_SONG_ID_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not request song ID");
        }
        std::array<std::uint8_t, 2> id_msg{};
        if (!serial::receive(id_msg.data(), id_msg.size())) {
            throw NoACKException("Could not request song ID");
        }
        if (id_msg[0] != 0x01) {
            throw NoACKException("Could not request song ID");
        }

        return id_msg[1];
    }

    auto play_loaded_song() -> void {
        send_control_message(START_SONG_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not play loaded song");
        }
    }

    auto end_loaded_song() -> void {
        send_control_message(END_SONG_MESSAGE);
        if (!receive_ack()) {
            throw NoACKException("Could not end loaded song");
        }
    }

    auto hold_major_chord(MessageType chord_message_type) -> void {
        switch (chord_message_type) {
            case MessageType::HoldAMajorChord: {
                send_control_message(HOLD_A_MAJOR_CHORD_MESSAGE);
                break;
            }
            case MessageType::HoldCMajorChord: {
                send_control_message(HOLD_C_MAJOR_CHORD_MESSAGE);
                break;
            }
            case MessageType::HoldDMajorChord: {
                send_control_message(HOLD_D_MAJOR_CHORD_MESSAGE);
                break;
            }
            case MessageType::HoldGMajorChord: {
                send_control_message(HOLD_G_MAJOR_CHORD_MESSAGE);
                break;
            }
            case MessageType::HoldEMajorChord: {
                send_control_message(HOLD_E_MAJOR_CHORD_MESSAGE);
                break;
            }
            case MessageType::HoldFMajorChord: {
                send_control_message(HOLD_F_MAJOR_CHORD_MESSAGE);
                break;
            }
            default: {
                return;
            }
        }
        if (!receive_ack()) {
            throw NoACKException("Could not hold major chord");
        }
    }

    std::mutex mut{};

    std::mutex song_info_mut{};
    std::uint8_t current_song_id = 0x00;
    bool playing = false;

} // namespace mcu

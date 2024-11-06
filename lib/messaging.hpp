#pragma once

#include <array>
#include <cstdint>

#include "guitar.hpp"

// Maximum data size for the message payload.
constexpr std::size_t MAX_DATA_SIZE = 6;

// Represents a timestamp in milliseconds.
using timestamp_t = std::uint32_t;

/**
 * @enum MessageType
 * @brief Defines various message types for communication.
 */
enum class MessageType : std::uint8_t {
    None = 0x00,             ///< Default
    Reset = 0x41,            ///< Reset the device
    StartSongLoading = 0x42, ///< Start loading a song with a Song ID
    EndSongLoading = 0x43,   ///< End loading of the song
    Note = 0x44,             ///< Send a note with timestamp, fret, and string
    StartLoadedSong = 0x45,  ///< Start playing the loaded song
    EndLoadedSong = 0x46,    ///< End playing the loaded song
    RequestSongID = 0x47,    ///< Request the song ID
    LoadedSongID = 0x48,     ///< Send the loaded song ID
    ACK = 0x06               ///< Acknowledgment message
};


/**
 * @struct Message
 * @brief Represents a message structure for encoding and sending messages.
 */
struct Message {
    static constexpr std::uint8_t header = 0x01;    ///< Fixed header byte
    MessageType type;                               ///< Type of message
    std::array<std::uint8_t, MAX_DATA_SIZE> data{}; ///< Data payload (fixed-size for embedded compatibility)
    std::size_t data_len = 0;                       ///< Actual length of data payload

    /**
     * @brief Encodes the message into a byte array for transmission.
     * @param buffer Output buffer with size at least MAX_DATA_SIZE + 2 bytes.
     * @return Number of bytes written into the buffer.
     */
    auto encode(std::uint8_t* buffer) const -> std::size_t {
        buffer[0] = header;
        buffer[1] = static_cast<std::uint8_t>(type);
        for (std::size_t i = 0; i < data_len; ++i) {
            buffer[i + 2] = data[i];
        }
        return 2 + data_len;
    }

    [[nodiscard]] auto size() const -> std::size_t { return data_len + 2; }
};

// Helper functions to build specific message types

constexpr inline auto create_reset_message() -> Message { return {MessageType::Reset, {}, 0}; }

constexpr inline auto create_start_song_loading_message(std::uint8_t const song_id) -> Message {
    Message msg{MessageType::StartSongLoading, {}, 1};
    msg.data[0] = song_id;
    return msg;
}

constexpr inline auto create_end_song_loading_message() -> Message { return {MessageType::EndSongLoading, {}, 0}; }

constexpr inline auto create_note_message(timestamp_t const timestamp, fret_t const fret, string_e const string) -> Message {
    Message msg{MessageType::Note, {}, 5};
    msg.data[0] = (timestamp >> 24) & 0xFF;
    msg.data[1] = (timestamp >> 16) & 0xFF;
    msg.data[2] = (timestamp >> 8) & 0xFF;
    msg.data[3] = timestamp & 0xFF;
    msg.data[4] = static_cast<std::uint8_t>((fret & 0x0F) << 4 | (static_cast<std::uint8_t>(string) & 0x0F));
    return msg;
}

constexpr inline auto create_start_loaded_song_message() -> Message { return {MessageType::StartLoadedSong, {}, 0}; }

constexpr inline auto create_end_loaded_song_message() -> Message { return {MessageType::EndLoadedSong, {}, 0}; }

constexpr inline auto create_request_song_id_message() -> Message { return {MessageType::RequestSongID, {}, 0}; }

constexpr inline auto create_loaded_song_id_message(std::uint8_t const song_id) -> Message {
    Message msg{MessageType::LoadedSongID, {}, 1};
    msg.data[0] = song_id;
    return msg;
}

constexpr inline auto create_ack_message() -> Message { return {MessageType::ACK, {}, 0}; }

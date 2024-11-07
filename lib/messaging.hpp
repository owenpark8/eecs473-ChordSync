#pragma once

#include <array>
#include <cstdint>
#include <variant>

// Maximum data size for the message payload.
constexpr std::size_t MAX_DATA_SIZE = 6;

// Represents a timestamp in milliseconds.
using timestamp_t = std::uint32_t;

/**
 * @enum MessageType
 * @brief Defines various message types for communication.
 */
enum class MessageType : std::uint8_t {
    // clang-format off
    None             = 0x00, ///< Default
    Reset            = 0x41, ///< Reset the device
    StartSongLoading = 0x42, ///< Start loading a song with a Song ID
    EndSongLoading   = 0x43, ///< End loading of the song
    Note             = 0x44, ///< Send a note with timestamp, fret, and string
    StartSong        = 0x45, ///< Start playing the loaded song
    EndSong          = 0x46, ///< End playing the loaded song
    RequestSongID    = 0x47, ///< Request the song ID
    LoadedSongID     = 0x48, ///< Send the loaded song ID
    ACK              = 0x06  ///< Acknowledgment message
    // clang-format on
};

using ControlMessage = std::array<std::uint8_t, 2>;

constexpr std::uint8_t MESSAGE_HEADER = 0x01;

// clang-format off
constexpr ControlMessage RESET_MESSAGE              = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Reset)};
constexpr ControlMessage START_SONG_LOADING_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Reset)};
constexpr ControlMessage END_SONG_LOADING_MESSAGE   = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::EndSongLoading)};
constexpr ControlMessage NOTE_MESSAGE               = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Note)};
constexpr ControlMessage START_SONG_MESSAGE         = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::StartSong)};
constexpr ControlMessage END_SONG_MESSAGE           = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::EndSong)};
constexpr ControlMessage REQUEST_SONG_ID_MESSAGE    = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::RequestSongID)};
constexpr ControlMessage LOADED_SONG_ID_MESSAGE     = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::RequestSongID)};
constexpr ControlMessage ACK_MESSAGE                = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::ACK)};
// clang-format on

struct StartSongLoadingDataMessage {
    std::uint8_t song_id;
};

struct NoteDataMessage {
    std::uint32_t timestamp;
    std::uint8_t fret : 4;
    std::uint8_t string : 4;
};

struct LoadedSongDataMessage {
    std::uint8_t song_id;
};

using DataMessage = std::variant<StartSongLoadingDataMessage, NoteDataMessage, LoadedSongDataMessage>;

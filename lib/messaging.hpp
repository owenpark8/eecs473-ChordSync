#pragma once

#include <array>
#include <cstdint>

// Maximum data size for the message payload.
constexpr std::size_t MAX_DATA_SIZE = 6;

// Maximum number of notes in a song
constexpr std::size_t MAX_NOTES_IN_SONG = 50;

/**
 * @enum MessageType
 * @brief Defines various message types for communication.
 */
enum class MessageType : std::uint8_t {
    // clang-format off
    None             = 0x00, ///< Default
    Reset            = 0x41, ///< Reset the device
    Clear            = 0x42, ///< Clear the screens

    StartSongLoading = 0x52, ///< Start loading a song with a Song ID
    EndSongLoading   = 0x53, ///< End loading of the song
    Note             = 0x54, ///< Send a note with timestamp, fret, and string
    StartSong        = 0x55, ///< Start playing the loaded song
    EndSong          = 0x56, ///< End playing the loaded song
    RequestSongID    = 0x57, ///< Request the song ID
    LoadedSongID     = 0x58, ///< Send the loaded song ID

    DarkMode         = 0x60, ///< Set Dark Mode
    LightMode        = 0x61, ///< Set Light Mode (default)



	HoldAMajorChord  = 0x80, ///< Display A Major Chord indefinitely
	HoldCMajorChord  = 0x81, ///< Display C Major Chord indefinitely
	HoldDMajorChord  = 0x82, ///< Display D Major Chord indefinitely
	HoldEMajorChord  = 0x83, ///< Display E Major Chord indefinitely
	HoldFMajorChord  = 0x84, ///< Display F Major Chord indefinitely
	HoldGMajorChord  = 0x85, ///< Display G Major Chord indefinitely

    ACK              = 0x06  ///< Acknowledgment message
    // clang-format on
};

using ControlMessage = std::array<std::uint8_t, 2>;

constexpr std::uint8_t MESSAGE_HEADER = 0x01;

// clang-format off
constexpr ControlMessage RESET_MESSAGE              = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Reset)};
constexpr ControlMessage CLEAR_MESSAGE              = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Clear)};
constexpr ControlMessage START_SONG_LOADING_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::StartSongLoading)};
constexpr ControlMessage END_SONG_LOADING_MESSAGE   = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::EndSongLoading)};
constexpr ControlMessage NOTE_MESSAGE               = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::Note)};
constexpr ControlMessage START_SONG_MESSAGE         = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::StartSong)};
constexpr ControlMessage END_SONG_MESSAGE           = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::EndSong)};
constexpr ControlMessage REQUEST_SONG_ID_MESSAGE    = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::RequestSongID)};
constexpr ControlMessage LOADED_SONG_ID_MESSAGE     = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::LoadedSongID)};
constexpr ControlMessage DARK_MODE_MESSAGE          = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::DarkMode)};
constexpr ControlMessage LIGHT_MODE_MESSAGE         = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::LightMode)};
constexpr ControlMessage HOLD_A_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldAMajorChord)};
constexpr ControlMessage HOLD_C_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldCMajorChord)};
constexpr ControlMessage HOLD_D_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldDMajorChord)};
constexpr ControlMessage HOLD_E_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldEMajorChord)};
constexpr ControlMessage HOLD_F_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldFMajorChord)};
constexpr ControlMessage HOLD_G_MAJOR_CHORD_MESSAGE = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::HoldGMajorChord)};
constexpr ControlMessage ACK_MESSAGE                = {MESSAGE_HEADER, static_cast<uint8_t>(MessageType::ACK)};
// clang-format on

struct StartSongLoadingDataMessage {
    std::uint8_t song_id;
};

struct __attribute__((packed)) NoteDataMessage {
    std::uint32_t timestamp_ms;
    std::uint16_t length_ms;
    std::uint8_t fret : 4;
    std::uint8_t string : 4;
};

struct LoadedSongDataMessage {
    std::uint8_t song_id;
};

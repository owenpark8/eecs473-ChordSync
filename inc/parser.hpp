#pragma once

#include <string>
#include <unordered_map>

namespace parser {

    double timestamp;                                      // elasped time in song TODO: figure out units
    // TODO: This is assuming we are storing TAB files on MCU, subject to change depending on where files are located
    std::string current_song;                              // name of current song playing
    std::unordered_map<std::string, std::string> song_map; // map of song names to directory names

    /**
     * @brief Sets the current playing song
     * @param song_name name of song to be set as current_song
     */
    void set_song(std::string song_name);

    /**
     * @brief Parses TAB file song
     */
    void parse_song();

    /**
     * @brief Rewinds the current song, starts playing from beginning
     */
    void rewind_song();

    /**
     * @brief Pauses the current playing song
     */
    void pause_song();

    /**
     * @brief Resumes the current song
     */
    void resume_song();

    /**
     * @brief listen for commands from RPI (commands to synchronize timestamp with RPI)
     */
    void listen_to_rpi();
}; // namespace parser
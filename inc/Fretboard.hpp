#pragma once

#include <array>
#include <cstdint>

#include "lcd.hpp"

/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens.
 * Interfaced with coordinates on both "fretboard grid" or "pixel grid" as continous grids across LCD screens
 */

constexpr size_t num_lcds = 5;

class Fretboard {
    enum class string_e { LOW_E, A, D, G, B, HIGH_E }; // guitar string
    using fret_t = uint8_t;                            // guitar fret number

    /**
     * Coordinates/Finger positions of fretboard
     * (0, 0) at F on low E string
     */
    struct fretboard_location {
        fret_t x;
        string_e y;
    };

public:
    /**
     * @brief Default constructor for Fretboard
     *
     */
    Fretboard() = default;

    /**
     * @brief Initializes LCD screens on the fretboard
     *
     */
    void fretboard_init();

    /**
     * @brief Writes a circle to a specific location on the fretboard
     * @param fretboard_location coordinates of note on fretboard grid
     * @param radius radius of circle to be written
     * @param color color of circle to be written
     */
    void draw_note(fretboard_location fretboard_location, int radius, color color);

    /**
     * @brief draws indicator to play open string  TODO: determine what kind of indicator
     * @param string string to play
     * @param color color of indicator
     */
    void draw_string(string_e string, color color);

    /*----------------------BACKDOOR FUNCTIONS-------------------------- */
    /**
     * @brief Writes a pixel to a specific location on an LCD screen
     * @param pixel_location coordinates of pixel to be written to screen
     * @param color color of pixel to be written
     */
    void draw_pixel(pixel_location pixel_location, color color);

    /**
     * @brief converts fretboard location to pixel location
     * @param fretboard_location fretboard location to be converted
     * @return pixel coordinate of fret
     */
    auto convert_fret_to_pixels(fretboard_location fretboard_location) -> pixel_location;

    /**
     * @brief Clears the fretboard LCDs
     */
    void clear_fretboard();

private:
    std::array<LCD, num_lcds> m_lcds;
};

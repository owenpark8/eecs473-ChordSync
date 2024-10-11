#pragma once

#include <array>
#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>
#include "lcd.hpp"

#ifdef __cplusplus
}
#endif

/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens.
 * Interfaced with coordinates on both "fretboard grid" or "pixel grid" as continous grids across LCD screens
 */

constexpr std::size_t num_lcds = 5;
constexpr std::size_t num_frets = 24; // TODO: Check this number
constexpr std::size_t num_strings = 6;

class Fretboard {
    enum class string_e { LOW_E, A, D, G, B, HIGH_E }; // guitar string
    using fret_t = uint8_t;                            // guitar fret number

    /**
     * Coordinates/Finger positions of fretboard
     * (0, 0) at F on low E string
     */
    struct fretboard_location_t {
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
    void draw_note(fretboard_location_t fretboard_location, int radius, color_t color);

    /**
     * @brief draws indicator to play open string  TODO: determine what kind of indicator
     * @param string string to play
     * @param color color of indicator
     */
    void draw_string(string_e string, color_t color);

    /*----------------------BACKDOOR FUNCTIONS-------------------------- */
    /**
     * @brief Writes a pixel to a specific location on an LCD screen
     * @param pixel_location coordinates of pixel to be written to screen
     * @param color color of pixel to be written
     */
    void draw_pixel(pixel_location_t pixel_location, color_t color);

    /**
     * @brief converts fretboard location to pixel location
     * @param fretboard_location fretboard location to be converted
     * @return pixel coordinate of fret
     */
    auto convert_fret_to_pixels(fretboard_location_t fretboard_location) -> pixel_location_t;

    /**
     * @brief Clears the fretboard LCDs
     */
    void clear_fretboard();

private:
    std::array<LCD, num_lcds> m_lcds;
    // TODO: replace these numbers with actual pixels after measuring with guitar
    const std::array<uint16_t, num_frets> fret_pixel_array = { // keeps track of fret to pixel_location.x
        0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 
        1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300
    };
};

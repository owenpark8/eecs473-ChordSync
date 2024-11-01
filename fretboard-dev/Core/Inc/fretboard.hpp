#pragma once

#include <array>
#include <cstdint>

#include "lcd.hpp"

/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens.
 * Interfaced with coordinates on both "fretboard grid" or "pixel grid" as continous grids across LCD screens
 */

constexpr std::size_t num_lcds = 6;
constexpr std::size_t num_frets = 24; // TODO: Check this number
constexpr std::size_t num_strings = 6;
constexpr uint16_t total_pixel_width = num_lcds*480;

class Fretboard {
    enum class string_e { LOW_E, A, D, G, B, HIGH_E }; // guitar string
    using fret_t = uint8_t;                            // guitar fret number

    /**
     * Coordinates/Finger positions of fretboard
     * (0, 0) at F on high E string
     */
    struct fretboard_location_t {
        fret_t x;
        string_e y;
    };

    /**
     * 
     */
    struct fret_loc_pixel_dim_t {
        pixel_location_t pixel_loc;
        uint16_t w;
        uint16_t h;
    };
public:
    /**
     * @brief Default constructor for Fretboard
     *
     */
    Fretboard() = default;

    /**
     * @brief Constructor for Fretboard with 6 LCDs
     *
     */
    Fretboard(LCD const &lcd_1, LCD const &lcd_2, LCD const &lcd_3, LCD const &lcd_4, LCD const &lcd_5, LCD const &lcd_6)
        : m_lcds{lcd_1, lcd_2, lcd_3, lcd_4, lcd_5, lcd_6} {}

    /**
     * @brief Initializes LCD screens on the fretboard
     *
     */
    auto fretboard_init() -> void;

    /**
     * @brief Writes a circle to a specific location on the fretboard
     * @param fretboard_location coordinates of note on fretboard grid
     * @param color color of circle to be written
     */
    auto draw_note(fretboard_location_t fretboard_location, uint16_t color) -> void;

    /**
     * @brief draws indicator to play open string  TODO: determine what kind of indicator
     * @param string string to play
     * @param color color of indicator
     */
    auto draw_string(string_e string, uint16_t color) -> void;

    /*----------------------BACKDOOR FUNCTIONS-------------------------- */

    /**
     * @brief converts fretboard location to pixel location
     * @param fretboard_location fretboard location to be converted
     * @return pixel coordinate of fret
     */
    auto convert_fret_to_pixels(fretboard_location_t fretboard_location) -> fret_loc_pixel_dim_t;

    /**
     * @brief Clears the fretboard LCDs
     */
    auto clear_fretboard() -> void;

    /*
     * @brief converts string to height and pixel_location.y
     * @param y string to be converted
     * @param pixel_y changes in this function, starting pixel location y
     * @param height changes in this function, height to next pixel
     */
    auto convert_fret_y(string_e y, uint16_t &pixel_y, uint16_t &height) -> void;

private:
    std::array<LCD, num_lcds> m_lcds;
    // TODO: replace these numbers with actual pixels after measuring with guitar
    const std::array<uint16_t, num_frets> fret_pixel_array = { // keeps track of fret to pixel_location.x
        0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 
        1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300
    };
};

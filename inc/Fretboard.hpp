#pragma once

#include "hardware.hpp"
#include <cstdint>

/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens
 * Treat both "fretboard grid" and "pixel grid" as continous grids across LCD screens
 */

class Fretboard {

    SPI_HandleTypeDef* hspi; // SPI handler
    Pin LCD_RS;              // LCD register select (command/data)
    Pin LCD_CS;              // LCD chip select
    Pin LCD_RST;             // LCD reset


    /**
     * 3 bits for 8 different colors
     */
    struct color {
        unsigned int r : 1;
        unsigned int g : 1;
        unsigned int b : 1;
    };

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
    /**
     * Coordinates of Pixels on LCD array
     * (0, 0) at corner pixel closest to (0, 0) fretboard location
     */
    struct pixel_location {
        uint16_t x;
        uint16_t y;
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
     * @brief draws indicator to play open string TODO: determine what kind of indicator
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
};
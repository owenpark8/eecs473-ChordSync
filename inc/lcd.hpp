#pragma once

#include "hardware.hpp"

/**
 * 3 bits for 8 different colors
 */
struct color_t {
    unsigned int r : 1;
    unsigned int g : 1;
    unsigned int b : 1;
};

/**
 * Coordinates of pixels on LCD array
 */
struct pixel_location_t {
    uint16_t x;
    uint16_t y;
};

/**
 * Interface for the ILI9488 LCD controller
 */
class LCD {
public:
    LCD() = default;

    void init();

    /**
     * @brief Draws a single pixel
     * @param pos position of pixel
     * @param color pixel color
     */
    void draw_pixel(pixel_location_t pos, color_t color);

    /**
     * @brief Fills the entire screen with a color
     * @param color fill color of screen
     */
    void fill_screen(color_t color);

    /**
     * @brief Draws a vertical line
     * @param pos starting position of line (top)
     * @param h height of the line in pixels
     * @param color fill color of rectangle
     */
    void draw_vertical_line(pixel_location_t pos, uint16_t h, uint16_t color);

    /**
     * @brief Draws a horizontal line
     * @param pos starting position of line (left)
     * @param w width of the line in pixels
     * @param color fill color of rectangle
     */
    void draw_horizontal_line(pixel_location_t pos, uint16_t w, uint16_t color);

    /**
     * @brief Draws a filled color rectangle
     * @param pos top left position of rectangle
     * @param w horizontal width of rectangle in pixels
     * @param h vertical height of rectangle in pixels
     * @param color fill color of rectangle
     */
    void draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, uint16_t color);

    // backdoors
    void send_data(uint8_t const* data, uint16_t size);

private:
    SPI m_spi{};
    Pin m_reg_sel{}; // LCD register select (command/data)
    Pin m_reset{};   // LCD reset
};

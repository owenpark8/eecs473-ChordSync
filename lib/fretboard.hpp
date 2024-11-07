#pragma once

#include <array>
#include <cstdint>

#include "lcd.hpp"
#include "guitar.hpp"


/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens.
 * Interfaced with coordinates on both "fretboard grid" or "pixel grid" as continous grids across LCD screens
 */
class Fretboard {
    static constexpr std::size_t NUM_LCDS = 6;
    static constexpr std::size_t NUM_FRETS = 23;
    static constexpr std::size_t NUM_STRINGS = 6;
    static constexpr uint16_t TOTAL_PIXEL_WIDTH = NUM_LCDS * ILI9486_TFTHEIGHT;

    struct note_location_rectangle_t {
        pixel_location_t pixel_loc;
        uint16_t w;
        uint16_t h;
    };

    enum class uart_state {
        REC_NEW_MSG,       // Receive a new header and message (2 bytes)
        REC_SONG_ID,
        REC_NOTE
    };

    std::array<LCD, NUM_LCDS> m_lcds{};
    uint8_t uart_buf = {0};
    uart_state m_uart_state;


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
    Fretboard(LCD const& lcd_1, LCD const& lcd_2, LCD const& lcd_3, LCD const& lcd_4, LCD const& lcd_5, LCD const& lcd_6)
        : m_lcds{lcd_1, lcd_2, lcd_3, lcd_4, lcd_5, lcd_6} {}


    /**
     * @brief Initializes LCD screens on the fretboard
     *
     */
    auto init() -> void {
        m_lcds[0].reset_lcd(); // only need to call it on an arbitrary one since they are all tied together
        for (auto& lcd: m_lcds) {
            lcd.init();
        }
        m_uart_state = uart_state::REC_NEW_MSG;
        // clear();
    }

    /**
     * @brief Writes a circle to a specific location on the fretboard
     * @param fretboard_location coordinates of note on fretboard grid
     * @param color color of circle to be written
     */
    auto draw_note(note_location_t note_location, uint16_t color) -> void {
        note_location_rectangle_t note_location_rectangle = convert_note_to_pixels(note_location);
        // Find which lcd(s) will be selected to draw this note
        uint16_t lcd_index_1 = note_location_rectangle.pixel_loc.x / 480; // 480 is width of LCD screen in pixels
        uint16_t lcd_index_2 = (note_location_rectangle.pixel_loc.x + note_location_rectangle.w) / 480; // 480 is width of LCD screen in pixels
        // In both cases, we want to draw a rectangle on the first LCD, our
        // if condition will confine the set_addr_window to not go beyond the bounds of the screen

        m_lcds[lcd_index_1].draw_rectangle({note_location_rectangle.pixel_loc.x % 480, note_location_rectangle.pixel_loc.y}, note_location_rectangle.w, note_location_rectangle.h, color);
        if (lcd_index_1 != lcd_index_2) {
            // Note goes across screens
            m_lcds[lcd_index_2].draw_rectangle({0, note_location_rectangle.pixel_loc.y}, (note_location_rectangle.pixel_loc.x+note_location_rectangle.w) % 480, note_location_rectangle.h, color);
        }
    }


    /**
     * @brief draws indicator to play open string  TODO: determine what kind of indicator
     * @param string string to play
     * @param color color of indicator
     */
    auto draw_string(string_e string, uint16_t color) -> void {
        uint16_t pixel_y;
        uint16_t height;
        convert_string_to_y(string, pixel_y, height); // Gives values to pixel_y and height
        for (auto& lcd: m_lcds) {
            lcd.draw_horizontal_line({0, pixel_y}, height, color);
        }
    }


    /**
     * @brief Handles a uart message, called on interrupt
     */
    auto handle_uart_message(UART_HandleTypeDef *huart) -> void {
        switch (m_uart_state) {
            case uart_state::REC_NEW_MSG:
                // HAL_UART_Receive_IT(huart, uart_buf, 2);
                break;
            case uart_state::REC_SONG_ID:
                break;
            case uart_state::REC_NOTE:
                break;
        }
    }

private:
    /*----------------------BACKDOOR FUNCTIONS-------------------------- */

    /**
     * @brief converts note location to note pixel location
     * @param note_location note location to be converted
     * @return pixel coordinate of note
     */
    auto convert_note_to_pixels(note_location_t note_location) -> note_location_rectangle_t {
        // TODO: replace these numbers with actual pixels after measuring with guitar
        static const std::array<uint16_t, NUM_FRETS> fret_pixel_array = {
                // keeps track of fret to pixel_location.x
                0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100,
                1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200
        };

        uint16_t pixel_x = fret_pixel_array[note_location.fret];
        uint16_t pixel_y = 0;
        uint16_t height = 0;
        convert_string_to_y(note_location.string, pixel_y, height); // Gives values to pixel_y and height
        // Find width and height of rectangle to be drawn
        uint16_t width = (note_location.fret == NUM_FRETS - 1) ? TOTAL_PIXEL_WIDTH - pixel_x : fret_pixel_array[note_location.fret + 1] - pixel_x;
        return {{pixel_x, pixel_y}, width, height};
    }

    /**
     * @brief Clears the fretboard LCDs
     */
    auto clear() -> void {
        for (auto& lcds: m_lcds) {
            lcds.clear_screen();
        }
    }

    /*
     * @brief converts string to height and pixel_location.y
     * @param y string to be converted
     * @param pixel_y changes in this function, starting pixel location y
     * @param height changes in this function, height to next pixel
     */
    auto convert_string_to_y(string_e y, uint16_t& pixel_y, uint16_t& height) -> void {
        switch (y) {
            // TODO: Check if these conversions are correct with guitar
            case string_e::HIGH_E:
                pixel_y = 0;
                height = 52;
                break;
            case string_e::A:
                pixel_y = 53;
                height = 53;
                break;
            case string_e::D:
                pixel_y = 107;
                height = 53;
                break;
            case string_e::G:
                pixel_y = 160;
                height = 52;
                break;
            case string_e::B:
                pixel_y = 213;
                height = 53;
                break;
            case string_e::LOW_E:
                pixel_y = 267;
                height = 52;
                break;
            default:
                break;
        }
    }

};

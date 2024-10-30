#pragma once

#include <cstdint>
#include <vector>

#include "hardware.hpp"


/*--------------TODO: Update pins with correct numbers------------------------*/
// #define TFT_CS_Pin GPIO_PIN_0
// #define TFT_CS_GPIO_Port GPIOD
#define TFT_RST_Pin GPIO_PIN_7
#define TFT_RST_GPIO_Port GPIOD
#define TFT_DC_Pin GPIO_PIN_6
#define TFT_DC_GPIO_Port GPIOD

/*---------------Various other control signals---------------------*/
#define ILI9486_SLPOUT 0x11 // Sleep mode off
#define ILI9486_DISPON 0x29 // Turn display on
#define ILI9486_CASET 0x2A  // Coloumn Address Set
#define ILI9486_PASET 0x2B  // 
#define ILI9486_RAMWR 0x2C

/*------------------LCD Screen pixel dimensions--------------------*/
#define ILI9486_TFTWIDTH  320
#define ILI9486_TFTHEIGHT 480
#define ILI9486_PIXEL_COUNT	ILI9486_TFTWIDTH * ILI9486_TFTHEIGHT

/*----------------------Color Definitions--------------------------*/
// 16 Bits for Color
constexpr uint16_t WHITE = 0xFFFF;
constexpr uint16_t BLACK = 0x0000;
constexpr uint16_t RED = 0xF800;
constexpr uint16_t GREEN = 0x07E0;
constexpr uint16_t BLUE = 0x001F;
constexpr uint16_t YELLOW = 0xFFE0;

/**
 * Coordinates of pixels on LCD array
 */
struct pixel_location_t {
    uint16_t x;
    uint16_t y;
};

/**
 * Interface for the ILI9486 LCD controller
 */
class LCD {
public:
    LCD(SPI const &spi, Pin const &reg_sel, Pin const &reset)
        : m_spi(spi), m_reg_sel(reg_sel), m_reset(reset) {}

    auto init() const -> void;

    /**
     * @brief Draws a single pixel
     * @param pos position of pixel
     * @param color pixel color
     */
    auto draw_pixel(pixel_location_t pos, uint16_t color) const -> void;

    /**
     * @brief Fills the entire screen with a color
     * @param color fill color of screen
     */
    auto fill_screen(uint16_t color) const -> void;

    /**
     * @brief Clears the screen to be all white
     */
    auto clear_screen() const -> void;

    /**
     * @brief Draws a vertical line
     * @param pos starting position of line (top)
     * @param h height of the line in pixels
     * @param color fill color of rectangle
     */
    auto draw_vertical_line(pixel_location_t pos, uint16_t h, uint16_t color) const -> void;

    /**
     * @brief Draws a horizontal line
     * @param pos starting position of line (left)
     * @param w width of the line in pixels
     * @param color fill color of rectangle
     */
    auto draw_horizontal_line(pixel_location_t pos, uint16_t w, uint16_t color) const -> void;

    /**
     * @brief Draws a filled color rectangle
     * @param pos top left position of rectangle
     * @param w horizontal width of rectangle in pixels
     * @param h vertical height of rectangle in pixels
     * @param color fill color of rectangle
     */
    auto draw_rectangle(pixel_location_t pos, int16_t w, int16_t h, uint16_t color) const -> void;
    
    /**
     * @brief Draws a filled color rectangle
     * @param pos top left position of bitmap
     * @param w horizontal width of bitmap in pixels
     * @param h vertical height of bitmap in pixels
     */
    auto draw_bitmap(pixel_location_t pos, int16_t w, int16_t h, const std::vector<uint8_t> &bitmap) const -> void;

    /*------------BACKDOOR FUNCTIONS---------------*/
    auto send_command(uint8_t command) const -> void;
    auto send_data(uint8_t data) const -> void;
    auto send_data_long(uint8_t const *data, std::size_t size) const -> void;
    // set_addr_window requires that no other non image SPI data is sent after this, until transaction is over
    auto set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) const -> void;

    auto start_reset() const -> void;
    auto end_reset() const -> void;
    auto set_command() const -> void;
    auto set_data() const -> void;
    auto noop() const -> void;

private:
    SPI m_spi{};
    Pin m_reg_sel{}; // LCD register select (command/data)
    Pin m_reset{};   // LCD reset
    const uint16_t width = ILI9486_TFTHEIGHT;  // width in pixels of LCD screen
    const uint16_t height = ILI9486_TFTWIDTH; // height in pixels of LCD screen
};

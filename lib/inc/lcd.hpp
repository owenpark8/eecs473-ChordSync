#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "hardware.hpp"
#include <cstdint>
#ifdef __cplusplus
}
#endif

/*--------------TODO: Update pins with correct numbers------------------------*/
// #define TFT_CS_Pin GPIO_PIN_0
// #define TFT_CS_GPIO_Port GPIOD
#define TFT_RST_Pin GPIO_PIN_7
#define TFT_RST_GPIO_Port GPIOD
#define TFT_DC_Pin GPIO_PIN_6
#define TFT_DC_GPIO_Port GPIOD

/*---------------Various other control signals---------------------*/
#define ILI9488_SLPOUT 0x11
#define ILI9488_DISPON 0x29
#define ILI9488_CASET 0x2A
#define ILI9488_PASET 0x2B
#define ILI9488_RAMWR 0x2C

/*------------------LCD Screen pixel dimensions--------------------*/
#define ILI9488_TFTWIDTH  320
#define ILI9488_TFTHEIGHT 480
#define ILI9488_PIXEL_COUNT	ILI9488_TFTWIDTH * ILI9488_TFTHEIGHT

/*----------------------Color Definitions--------------------------*/
#define WHITE 0xFFFF
#define BLACK 0x0000
#define RED   0xF800
#define YELLOW 0xFFE0
#define GREEN 0x07E0

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
 * Interface for the ILI9486 LCD controller
 */
class LCD {
public:
    LCD(SPI spi, Pin reg_sel, Pin reset)
        : m_spi(spi), m_reg_sel(reg_sel), m_reset(reset) {}

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
    void fill_screen(/*color_t color*/ uint16_t color); // TODO: Consider changing this back to color_t

    /**
     * @brief Clears the screen to be all white
     */
    void clear_screen();

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

    /*------------BACKDOOR FUNCTIONS---------------*/
    void send_data(uint8_t data);
    void send_command(uint8_t com);
    void send_data_multi(uint8_t *buff, std::size_t buff_size);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void CS_A();
    void CS_D();
    void RST_A();
    void RST_D();
    void DC_COMMAND();
    void DC_DATA();

private:
    SPI m_spi{};
    Pin m_reg_sel{}; // LCD register select (command/data)
    Pin m_reset{};   // LCD reset
    const uint16_t width = ILI9488_TFTWIDTH;  // width in pixels of LCD screen
    const uint16_t height = ILI9488_TFTHEIGHT; // height in pixels of LCD screen
};

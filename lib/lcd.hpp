#pragma once

#include <cstdint>
#include <vector>

#include "hardware.hpp"

/*---------------Various other control signals---------------------*/
#define ILI9486_SLPOUT 0x11 // Sleep mode off
#define ILI9486_DISPON 0x29 // Turn display on
#define ILI9486_CASET 0x2A  // Coloumn Address Set
#define ILI9486_PASET 0x2B  //
#define ILI9486_RAMWR 0x2C


/*------------------LCD Screen pixel dimensions--------------------*/
#define ILI9486_TFTWIDTH  320
#define ILI9486_TFTHEIGHT 480

/*----------------------Color Definitions--------------------------*/
// 16 Bits for Color
constexpr uint16_t WHITE = 0xFFFF;
constexpr uint16_t BLACK = 0x0000;
constexpr uint16_t GRAY = 0x18c3;
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
	LCD() = default;

    LCD(SPI const &spi, Pin const &reg_sel, Pin const &reset)
        : m_spi(spi), m_reg_sel(reg_sel), m_reset(reset) {}

    auto init() const -> void {
		// Send initialization commands
		send_command(0xE0); // Positive Gamma control (sets pixels luminance)
		send_data(0x00); // 1
		send_data(0x03); // 2
		send_data(0x09); // 3
		send_data(0x08); // 4
		send_data(0x16); // 5
		send_data(0x0A); // 6
		send_data(0x3F); // 7
		send_data(0x78); // 8
		send_data(0x4C); // 9
		send_data(0x09); // 10
		send_data(0x0A); // 11
		send_data(0x08); // 12
		send_data(0x16); // 13
		send_data(0x1A); // 14
		send_data(0x0F); // 15
		send_command(0XE1); // Negative Gamma control
		send_data(0x00); // 1
		send_data(0x16); // 2
		send_data(0x19); // 3
		send_data(0x03); // 4
		send_data(0x0F); // 5
		send_data(0x05); // 6
		send_data(0x32); // 7
		send_data(0x45); // 8
		send_data(0x46); // 9
		send_data(0x04); // 10
		send_data(0x0E); // 11
		send_data(0x0D); // 12
		send_data(0x35); // 13
		send_data(0x37); // 14
		send_data(0x0F); // 15

		send_command(0XC0); //Power Control 1
		send_data(0x17); // Vreg1out (sets positive gamma voltage)
		send_data(0x15); // Verg2out (sets negative gamma voltage)

		send_command(0xC1); //Power Control 2
		send_data(0x41); //VGH,VGL
		// missing configuration for VCI1 regulator output voltage, default 0b110 = 5
		send_command(0xC5); //VCOM Control this is not correct
		send_data(0x00);
		send_data(0x12); //Vcom
		send_data(0x80);

		// ------END QUALITY SETTINGS

		send_command(0x3A); // Interface Pixel Control
		send_data(0x66); // 16 bit DIFFERENT

		send_command(0XB0); // Interface Mode Control, SPI clock config
		send_data(0x80); // SDO NOT USE

		send_command(0xB1); // Frame rate
		send_data(0xA0); // 62Hz, 0 Display clock division factor (LCD clk not reduced)
		// send_data(0x10); // RTNA = 0b10000 = 16 clocks/frame (fastest frame rate) DIFFERENT

		send_command(0xB4); // Display Inversion Control, sets polarity of pixels on LCD, affects quality and lifetime of screen
		send_data(0x02);

		send_command(0XB6); // Display Function Control  RGB/MCU Interface Control, configure for MCU
		send_data(0x02); // MCU
		send_data(0x02); // Source, Gate scan dieection

		send_command(0XE9);      // Set Image Function
		send_data(0x00);    // Disable 24 bit data

		send_command(0xF7);      // Adjust Control
		send_data(0xA9);
		send_data(0x51);
		send_data(0x2C);
		send_data(0x82);    // D7 stream, loose

		send_command(ILI9486_SLPOUT); // Exit Sleep mode
		busyWaitMs(120);

		send_command(ILI9486_DISPON); // Turn Display on

		send_command(0x36); // Memory Access Control, determines orientation of how LCD is updated
		send_data(0xE8);
	}

    auto reset_lcd() const -> void {
		start_reset();
    	busyWaitMs(10);
    	end_reset();
	}

    /**
     * @brief Fills the entire screen with a color
     * @param color fill color of screen
     */
    auto fill_screen(uint16_t color) const -> void {
		draw_rectangle({0, 0}, width, height, color);
	}

    /**
     * @brief Clears the screen to be all white
     */
    auto clear_screen(bool dark_mode) const -> void {
		fill_screen(dark_mode ? BLACK : WHITE);
	}

    /**
     * @brief Draws a horizontal line that spans across the full width of the screen
     * @param pos starting position of line (left)
     * @param h height of the line in pixels
     * @param color fill color of rectangle
     */
    auto draw_horizontal_line(pixel_location_t pos, uint16_t h, uint16_t color) const -> void {
		draw_rectangle(pos, width, h, color);
	}

    /**
     * @brief Draws a filled color rectangle
     * @param pos top left position of rectangle
     * @param w horizontal width of rectangle in pixels
     * @param h vertical height of rectangle in pixels
     * @param color fill color of rectangle
     */
    auto draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, uint16_t color) const -> void {
		if ((pos.x >= width) || (pos.y >= height)) return;
		if ((pos.x + w - 1) >= width) w = width - pos.x; // if our rectangle extends past the horizontal dimensions of the screen
		if ((pos.y + h - 1) >= height) h = height - pos.y;
		set_addr_window(pos.x, pos.y, pos.x + w - 1, pos.y + h - 1);

		uint8_t r = (color & 0xF800) >> 11;
		uint8_t g = (color & 0x07E0) >> 5;
		uint8_t b = color & 0x001F;

		r = (r * 255) / 31;
		g = (g * 255) / 63;
		b = (b * 255) / 31;
		set_data();
		for (int i = 0; i < w * h; ++i) {
			m_spi.spi_write_stay_selected(&r, 1);
			m_spi.spi_write_stay_selected(&g, 1);
			m_spi.spi_write_stay_selected(&b, 1);
		}
		noop(); // NOOP, reset csx, and end data stream
	}

    /**
     * @brief Draws a filled color rectangle
     * @param pos top left position of bitmap
     * @param w horizontal width of bitmap in pixels
     * @param h vertical height of bitmap in pixels
     */
    auto draw_bitmap(pixel_location_t pos, uint16_t w, uint16_t h, const std::vector<uint8_t> &bitmap) const -> void {
		if ((pos.x >= width) || (pos.y >= height)) return;
		if ((pos.x + w - 1) >= width) w = width - pos.x; // if our rectangle extends past the horizontal dimensions of the screen
		if ((pos.y + h - 1) >= height) h = height - pos.y;
		set_addr_window(pos.x, pos.y, pos.x + w - 1, pos.y + h - 1);
		
		set_data();
		for(auto &elm : bitmap) {
			m_spi.spi_write_stay_selected(&elm, 1);
		}
		noop(); // NOOP, reset csx, and end data stream
	}


    /*------------BACKDOOR FUNCTIONS---------------*/
    auto send_command(uint8_t command) const -> void {
		const uint8_t temp_command = command;
    	set_command();
    	m_spi.spi_write(&temp_command, 1);
	}
    auto send_data(uint8_t data) const -> void {
		const uint8_t temp_data = data;
    	set_data();
    	m_spi.spi_write(&temp_data, 1);
	}
    auto send_data_long(uint8_t const *data, std::size_t size) const -> void {
		set_data();
    	m_spi.spi_write_long(data, size);
	}
    // set_addr_window requires that no other non image SPI data is sent after this, until transaction is over
    // Bounds of coloumn and row are inclusive
    auto set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) const -> void {
		send_command(ILI9486_CASET); // Column addr set
		send_data(x0 >> 8);
		send_data(x0 & 0xFF);
		send_data(x1 >> 8);
		send_data(x1 & 0xFF);
		send_command(ILI9486_PASET); // Set rows
		send_data(y0 >> 8);
		send_data(y0 & 0xFF);
		send_data(y1 >> 8);
		send_data(y1 & 0xFF);
		send_command(ILI9486_RAMWR); // write to RAM
	}

	auto start_reset() const -> void { m_reset.reset(); }
	auto end_reset() const -> void { m_reset.set(); }
	auto set_command() const -> void { m_reg_sel.reset(); }
	auto set_data() const -> void { m_reg_sel.set(); }
	auto noop() const -> void {send_command(0x00); }

	auto busyWaitMs(uint32_t delay_ms) const -> void {
		// Calculate the number of iterations per millisecond
		uint32_t cycles_per_ms = SystemCoreClock / 1000;

		// Outer loop for milliseconds
		for (uint32_t i = 0; i < delay_ms; i++) {
			// Inner loop to waste cycles for one millisecond
			for (uint32_t j = 0; j < cycles_per_ms; j++) {
				__NOP(); // No operation, just waste cycles
			}
		}
	}


private:
    SPI m_spi{};
    Pin m_reg_sel{}; // LCD register select (command/data)
    Pin m_reset{};   // LCD reset
    static constexpr uint16_t width = ILI9486_TFTHEIGHT;  // width in pixels of LCD screen
    static constexpr uint16_t height = ILI9486_TFTWIDTH; // height in pixels of LCD screen

};

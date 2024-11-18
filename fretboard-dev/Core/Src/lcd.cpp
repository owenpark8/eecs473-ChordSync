#include "lcd.hpp"


auto LCD::init() const -> void {
    // Send initialization commands
    // send_command(0x01);  // Software reset TODO: Is this necessary?
    // HAL_Delay(50);       // Necessary to wait at least 5 ms after software reset

    // TODO: Fix LCD quality settings
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

    // send_command(0x36); // Memory Access Control, determines orientation of how LCD is updated
    // send_data(0x28);

    send_command(0x3A); // Interface Pixel Control
    send_data(0x66); // 16 bit DIFFERENT

    send_command(0XB0); // Interface Mode Control, SPI clock config
    send_data(0x80); // SDO NOT USE

    send_command(0xB1); // Frame rate
    send_data(0xA0); // 62Hz, 0 Display clock division factor (LCD clk not reduced)
    // send_data(0x10); // RTNA = 0b10000 = 16 clocks/frame (fastest frame rate) DIFFERENT

    send_command(0xB4); // Display Inversion Control, sets polarity of pixels on LCD, affects quality and lifetime of screen
    // send_data(0x12); // 2-dot DIFFERENT
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

auto LCD::reset_lcd() const -> void {
    start_reset();
    busyWaitMs(10);
    end_reset();
}

auto LCD::fill_screen(uint16_t color) const -> void {
    draw_rectangle({0, 0}, width, height, color);
}

auto LCD::clear_screen() const -> void { fill_screen(WHITE); }

auto LCD::draw_horizontal_line(pixel_location_t pos, uint16_t h, uint16_t color) const -> void {
    draw_rectangle(pos, width, h, color);
}

auto LCD::draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, uint16_t color) const -> void {
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

auto LCD::draw_bitmap(pixel_location_t pos, uint16_t w, uint16_t h, const std::vector<uint8_t> &bitmap) const -> void {
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

auto LCD::send_command(uint8_t const command) const -> void {
    const uint8_t temp_command = command;
    set_command();
    m_spi.spi_write(&temp_command, 1);
}

auto LCD::send_data(uint8_t const data) const -> void {
    const uint8_t temp_data = data;
    set_data();
    m_spi.spi_write(&temp_data, 1);
}

auto LCD::send_data_long(uint8_t const* data, std::size_t const size) const -> void {
    set_data();
    m_spi.spi_write_long(data, size);
}

auto LCD::set_addr_window(uint16_t const x0, uint16_t const y0, uint16_t const x1, uint16_t const y1) const -> void {
    // assert(x0 <= x1 && y0 <= y1);
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

void LCD::drawCharTest(pixel_location_t pos, unsigned char c, uint16_t color, uint16_t bg, uint8_t size){
//	if(rotationNum == 1 || rotationNum ==3)
//		{
//	if((x >= ILI9488_TFTWIDTH)            || // Clip right
//	     (y >= ILI9488_TFTHEIGHT)           || // Clip bottom
//	     ((x + 6 * size - 1) < 0) || // Clip left
//	     ((y + 8 * size - 1) < 0))   // Clip top
//	    return;
//		}
//		else
//		{
//			if((y >= ILI9488_TFTWIDTH)            || // Clip right
//	     (x >= ILI9488_TFTHEIGHT)           || // Clip bottom
//	     ((y + 6 * size - 1) < 0) || // Clip left
//	     ((x + 8 * size - 1) < 0))   // Clip top
//	    return;
//		}


//	  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

	  //6
	  for (int8_t i=0; i<6; i++ ) {
	    uint8_t line;
	    if (i == 5)
	      line = 0x0;
	    else
	      line = pgm_read_byte(font1+(c*5)+i);
	    for (int8_t j = 0; j<8; j++) {
	      if (line & 0x1) {
	        if (size == 1) // default size
	        	drawPixel({pos.x+i, pos.y+j}, color);
	        else {  // big size
	        	draw_rectangle({pos.x+(i*size), pos.y+(j*size)}, (size + pos.x+(i*size))/10, (size+1 + pos.y+(j*size))/10, color);
	        	//fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
	        }

	        drawPixelSize({pos.x + i, pos.y + i}, color, size);
	      } else if (bg != color) {
	        if (size == 1) // default size
	        	drawPixel({pos.x+i, pos.y+j}, bg);
	        else {  // big size
	        	draw_rectangle({pos.x+(i*size), pos.y+(j*size)}, (size + pos.x+(i*size))/10, (size+1 + pos.y+(j*size))/10, bg);
	        }
	      }
	      line >>= 1;
	    }
	  }

}

void LCD::drawPixel(pixel_location_t pos, uint16_t color)
{
    draw_rectangle(pos, 1, 1, color);
}

void LCD::drawPixelSize(pixel_location_t pos, uint16_t color, uint16_t size)
{
    draw_rectangle(pos, size, size, color);
}

auto LCD::start_reset() const -> void { m_reset.reset(); }
auto LCD::end_reset() const -> void { m_reset.set(); }
auto LCD::set_command() const -> void { m_reg_sel.reset(); }
auto LCD::set_data() const -> void { m_reg_sel.set(); }
auto LCD::noop() const -> void {send_command(0x00); }

auto LCD::busyWaitMs(uint32_t delay_ms) const -> void{
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

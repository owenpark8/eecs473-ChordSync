#include "lcd.hpp"

auto LCD::init() const -> void {
    start_reset();
    HAL_Delay(10);
    end_reset();
    HAL_Delay(120);

    // Send initialization commands
    send_command(0x01);  // Software reset TODO: Is this necessary?
    HAL_Delay(10);

    /*// TODO: Fix LCD quality settings
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
    /*send_command(0xC1); //Power Control 2
    send_data(0x41); //VGH,VGL 
    // missing configuration for VCI1 regulator output voltage, default 0b110 = 5
    send_command(0xC5); //VCOM Control this is not correct
    send_data(0x00);
    send_data(0x12); //Vcom
    send_data(0x80);*/

    // ------END QUALITY SETTINGS

    send_command(0x36); // Memory Access Control, determines orientation of how LCD is updated
    send_data(0x48);

    send_command(0x3A); // Interface Pixel Control
    send_data(0x55); // 16 bit

    send_command(0XB0); // Interface Mode Control, SPI clock config
    send_data(0x80); // SDO NOT USE

    send_command(0xB1); // Frame rate
    send_data(0xA0); // 62Hz, 0 Display clock division factor (LCD clk not reduced)
    send_data(0x10); // RTNA = 0b10000 = 16 clocks/frame (fastest frame rate)

    send_command(0xB4); // Display Inversion Control, sets polarity of pixels on LCD, affects quality and lifetime of screen
    send_data(0x12); // 2-dot

    send_command(0XB6); // Display Function Control  RGB/MCU Interface Control, configure for MCU
    send_data(0x02); // MCU
    send_data(0x02); // Source, Gate scan dieection

    send_command(ILI9486_SLPOUT); // Exit Sleep mode
    HAL_Delay(120);

    send_command(ILI9486_DISPON); // Turn Display on
}

auto LCD::fill_screen(color_t color) const -> void {
    draw_rectangle({0, 0}, width - 1, height - 1, color);
}

auto LCD::clear_screen() const -> void { fill_screen(WHITE); }

auto LCD::draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, color_t color) const -> void {
    uint32_t i, n, cnt, buf_size;
    if ((pos.x >= width) || (pos.y >= height)) return;
    if ((pos.x + w - 1) >= width) w = width - pos.x; // if our rectangle extends past the horizontal dimensions of the screen
    if ((pos.y + h - 1) >= height) h = height - pos.y;
    set_addr_window(pos.x, pos.y, pos.x + w - 1, pos.y + h - 1);

    n = w * h;
    uint8_t rgb_buf = 0x0;
    rgb |= ((color.r << 5) | (color.r << 2)) & 0xFF; // set bit 2 and 5 to r
    rgb |= ((color.g << 4) | (color.g << 1)) & 0xFF; // set bit 1 and 4 to g
    rgb |= ((color.b << 3) | (color.b)) & 0xFF; // set bit 0 and 3 to b

    while (n > 1) {
        send_command(rgb_buf);
        n -= 2; // 2 pixels per 8 bit buffer
    }
    if (n == 1) {
        rgb_buf &= 0b111; // send the last pixel
        send_command(rgb_buf);
    }
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
    send_command(ILI9488_CASET); // Column addr set
    {
        uint8_t const data[] = {static_cast<uint8_t>((x0 >> 8) & 0xFF),
                                static_cast<uint8_t>(x0 & 0xFF),
                                static_cast<uint8_t>((x1 >> 8) & 0xFF),
                                static_cast<uint8_t>(x1 & 0xFF)};
        send_data_long(data, sizeof(data));
    }
    send_command(ILI9488_PASET);
    {
        uint8_t const data[] = {static_cast<uint8_t>((y0 >> 8) & 0xFF),
                                static_cast<uint8_t>(y0 & 0xFF),
                                static_cast<uint8_t>((y1 >> 8) & 0xFF),
                                static_cast<uint8_t>(y1 & 0xFF)};
        send_data_long(data, sizeof(data));
    }
    send_command(ILI9488_RAMWR); // write to RAM
}

auto LCD::start_reset() const -> void { m_reset.reset(); }
auto LCD::end_reset() const -> void { m_reset.set(); }
auto LCD::set_command() const -> void { m_reg_sel.reset(); }
auto LCD::set_data() const -> void { m_reg_sel.set(); }
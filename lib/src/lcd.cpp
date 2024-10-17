#include "lcd.hpp"

auto LCD::init() const -> void {
    start_reset();
    HAL_Delay(10);
    end_reset();

    send_command(0xE0);
    send_data(0x00);
    send_data(0x03);
    send_data(0x09);
    send_data(0x08);
    send_data(0x16);
    send_data(0x0A);
    send_data(0x3F);
    send_data(0x78);
    send_data(0x4C);
    send_data(0x09);
    send_data(0x0A);
    send_data(0x08);
    send_data(0x16);
    send_data(0x1A);
    send_data(0x0F);
    send_command(0XE1);
    send_data(0x00);
    send_data(0x16);
    send_data(0x19);
    send_data(0x03);
    send_data(0x0F);
    send_data(0x05);
    send_data(0x32);
    send_data(0x45);
    send_data(0x46);
    send_data(0x04);
    send_data(0x0E);
    send_data(0x0D);
    send_data(0x35);
    send_data(0x37);
    send_data(0x0F);
    send_command(0XC0); //Power Control 1
    send_data(0x17); //Vreg1out
    send_data(0x15); //Verg2out
    send_command(0xC1); //Power Control 2
    send_data(0x41); //VGH,VGL
    send_command(0xC5); //Power Control 3
    send_data(0x00);
    send_data(0x12); //Vcom
    send_data(0x80);

    send_command(0x36); //Memory Access
    send_data(0x48);

    send_command(0x3A); // Interface Pixel Format
    send_data(0x66); //18 bit

    send_command(0XB0); // Interface Mode Control
    send_data(0x80); //SDO NOT USE

    send_command(0xB1); //Frame rate
    send_data(0xA0); //60Hz

    send_command(0xB4); //Display Inversion Control
    send_data(0x02); //2-dot

    send_command(0XB6); //Display Function Control  RGB/MCU Interface Control

    send_data(0x02); //MCU
    send_data(0x02); //Source,Gate scan dieection

    send_command(0XE9); // Set Image Functio
    send_data(0x00); // Disable 24 bit data

    send_command(0xF7); // Adjust Control
    send_data(0xA9);
    send_data(0x51);
    send_data(0x2C);
    send_data(0x82); // D7 stream, loose

    send_command(ILI9488_SLPOUT); //Exit Sleep

    HAL_Delay(120);

    send_command(ILI9488_DISPON); //Display on
}

auto LCD::fill_screen(/*color_t color*/ uint16_t const color) const -> void {
    draw_rectangle({0, 0}, width - 1, height - 1, color);
}

auto LCD::clear_screen() const -> void { fill_screen(WHITE); }

auto LCD::draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, uint16_t color) const -> void {
    uint32_t i, n, cnt, buf_size;
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

    n = w * h * 3;
    if (n <= 65535) {
        cnt = 1;
        buf_size = n;
    } else {
        cnt = n / 3;
        buf_size = 3;
        uint8_t min_cnt = n / 65535 + 1;
        for (i = min_cnt; i < n / 3; i++) {
            if (n % i == 0) {
                cnt = i;
                buf_size = n / i;
                break;
            }
        }
    }
    uint8_t frm_buf[buf_size];
    for (i = 0; i < buf_size / 3; i++) {
        frm_buf[i * 3] = r;
        frm_buf[i * 3 + 1] = g;
        frm_buf[i * 3 + 2] = b;
    }
    while (cnt > 0) {
        send_data_long(frm_buf, buf_size);

        cnt -= 1;
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
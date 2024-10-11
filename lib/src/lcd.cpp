#ifdef __cplusplus
extern "C" {
#endif
    #include "lcd.hpp"
}


void LCD::init() {
    CS_D();
    RST_A();
    HAL_Delay(10);
    RST_D();

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
    send_data(0x17);    //Vreg1out
    send_data(0x15);    //Verg2out
    send_command(0xC1); //Power Control 2
    send_data(0x41);    //VGH,VGL
    send_command(0xC5); //Power Control 3
    send_data(0x00);
    send_data(0x12); //Vcom
    send_data(0x80);

    send_command(0x36); //Memory Access
    send_data(0x48);

    send_command(0x3A); // Interface Pixel Format
    send_data(0x66);    //18 bit

    send_command(0XB0); // Interface Mode Control
    send_data(0x80);    //SDO NOT USE

    send_command(0xB1); //Frame rate
    send_data(0xA0);    //60Hz

    send_command(0xB4); //Display Inversion Control
    send_data(0x02);    //2-dot

    send_command(0XB6); //Display Function Control  RGB/MCU Interface Control

    send_data(0x02); //MCU
    send_data(0x02); //Source,Gate scan dieection

    send_command(0XE9); // Set Image Functio
    send_data(0x00);    // Disable 24 bit data

    send_command(0xF7); // Adjust Control
    send_data(0xA9);
    send_data(0x51);
    send_data(0x2C);
    send_data(0x82); // D7 stream, loose

    send_command(ILI9488_SLPOUT); //Exit Sleep

    HAL_Delay(120);

    send_command(ILI9488_DISPON); //Display on
}

void LCD::fill_screen(/*color_t color*/ uint16_t color) { draw_rectangle({0, 0}, width - 1, height - 1, color); }

void LCD::clear_screen() { fill_screen(WHITE); }

void LCD::draw_rectangle(pixel_location_t pos, uint16_t w, uint16_t h, uint16_t color) {
    uint32_t i, n, cnt, buf_size;
    if ((pos.x >= width) || (pos.y >= height)) return;
    if ((pos.x + w - 1) >= width) w = width - pos.x; // if our rectangle extends past the horizontal dimensions of the screen
    if ((pos.y + h - 1) >= height) h = height - pos.y;
    setAddrWindow(pos.x, pos.y, pos.x + w - 1, pos.y + h - 1);
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
    DC_DATA();
    CS_A();
    while (cnt > 0) {
        HAL_SPI_Transmit(m_spi.m_hpi, frm_buf, buf_size, HAL_MAX_DELAY);

        cnt -= 1;
    }
    CS_D();
}

void LCD::send_command(uint8_t com) {
    uint8_t tmpCmd = com;
    DC_COMMAND();
    CS_A();
    HAL_SPI_Transmit(m_spi.m_hpi, &tmpCmd, 1, 1);
    CS_D();
}

void LCD::send_data(uint8_t data) {
    uint8_t tmpCmd = data;
    DC_DATA();
    CS_A();
    HAL_SPI_Transmit(m_spi.m_hpi, &tmpCmd, 1, 1);
    CS_D();
}

void LCD::send_data_multi(uint8_t* buff, std::size_t buff_size) {
    DC_DATA();
    CS_A();
    while (buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(m_spi.m_hpi, buff, chunk_size, HAL_MAX_DELAY);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
    CS_D();
}

void LCD::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    send_command(ILI9488_CASET); // Column addr set
    {
        uint8_t data[] = {(x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF};
        send_data_multi(data, sizeof(data));
    }
    send_command(ILI9488_PASET);
    {
        uint8_t data[] = {(y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF};
        send_data_multi(data, sizeof(data));
    }
    send_command(ILI9488_RAMWR); // write to RAM
}

void LCD::CS_A() { HAL_GPIO_WritePin(m_spi.m_csx.m_port, m_spi.m_csx.m_pin, GPIO_PIN_RESET); }

void LCD::CS_D() { HAL_GPIO_WritePin(m_spi.m_csx.m_port, m_spi.m_csx.m_pin, GPIO_PIN_SET); }

void LCD::RST_A() { HAL_GPIO_WritePin(m_reset.m_port, m_reset.m_pin, GPIO_PIN_RESET); }
void LCD::RST_D() { HAL_GPIO_WritePin(m_reset.m_port, m_reset.m_pin, GPIO_PIN_SET); }
void LCD::DC_COMMAND() { HAL_GPIO_WritePin(m_reg_sel.m_port, m_reg_sel.m_pin, GPIO_PIN_RESET); }
void LCD::DC_DATA() { HAL_GPIO_WritePin(m_reg_sel.m_port, m_reg_sel.m_pin, GPIO_PIN_SET); }

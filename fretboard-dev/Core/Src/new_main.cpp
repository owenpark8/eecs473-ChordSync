#include "fretboard.hpp"

#include "new_main.h"

extern SPI_HandleTypeDef hspi1;

void new_main() {
    auto lcd_1 = LCD(SPI{&hspi1, Pin{CSX_A_GPIO_Port, CSX_A_Pin}}, Pin{LCD_RS_GPIO_Port, LCD_RS_Pin}, Pin{LCD_RST_GPIO_Port, LCD_RST_Pin});

    lcd_1.init();
    lcd_1.clear_screen();

    while (1) {}
}
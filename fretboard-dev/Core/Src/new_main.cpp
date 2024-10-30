#include "fretboard.hpp"

#include "new_main.h"


void new_main(SPI_HandleTypeDef* hspi1){
	Pin CS_in = Pin(CSX_A_GPIO_Port, CSX_A_Pin);
	SPI SPI_in = SPI(hspi1, CS_in);
	Pin reg_sel_in = Pin(LCD_RS_GPIO_Port, LCD_RS_Pin);
	Pin reset_in = Pin(LCD_RST_GPIO_Port, LCD_RST_Pin);
	LCD lcd_1 = LCD(SPI_in, reg_sel_in, reset_in);

	lcd_1.init();
	lcd_1.fill_screen(WHITE);


	  lcd_1.drawCharTest({50, 100}, 'h', BLACK, WHITE, 10);

}

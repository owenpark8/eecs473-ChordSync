#include "fretboard.hpp"

#include "new_main.h"


void new_main(SPI_HandleTypeDef* hspi1){
	Pin CS_in = Pin(GPIOC, CSX_A_Pin);
	SPI SPI_in = SPI(hspi1, CS_in);
	Pin reg_sel_in = Pin(GPIOA, LCD_RS_Pin);
	Pin reset_in = Pin(GPIOC, LCD_RST_Pin);
	LCD lcd_1 = LCD(SPI_in, reg_sel_in, reset_in);

	lcd_1.init();
	//lcd_1.fill_screen(0x07E0);

  while(1){
	  lcd_1.fill_screen(0x07E0);
	  HAL_Delay(20);
	  lcd_1.fill_screen(0xFFFF);
  }
}

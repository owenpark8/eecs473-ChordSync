#include "fretboard.hpp"

#include "new_main.h"


void new_main(SPI_HandleTypeDef* hspi1){
	//5 different chip selects.
	Pin CS_in = Pin(CSX_A_GPIO_Port, CSX_A_Pin);
	Pin CS_in2 = Pin(CSX_B_GPIO_Port, CSX_B_Pin);
	Pin CS_in3 = Pin(CSX_C_GPIO_Port, CSX_C_Pin);
	Pin CS_in4 = Pin(CSX_D_GPIO_Port, CSX_D_Pin);
	Pin CS_in5 = Pin(CSX_E_GPIO_Port, CSX_E_Pin);


	SPI SPI_in = SPI(hspi1, CS_in);
	SPI SPI_in2 = SPI(hspi1, CS_in2);
	SPI SPI_in3 = SPI(hspi1, CS_in3);
	SPI SPI_in4 = SPI(hspi1, CS_in4);
	SPI SPI_in5 = SPI(hspi1, CS_in5);



	Pin reg_sel_in = Pin(LCD_RS_GPIO_Port, LCD_RS_Pin);
	Pin reset_in = Pin(LCD_RST_GPIO_Port, LCD_RST_Pin);

	LCD lcd_1 = LCD(SPI_in, reg_sel_in, reset_in);
	LCD lcd_2 = LCD(SPI_in2, reg_sel_in, reset_in);
	LCD lcd_3 = LCD(SPI_in3, reg_sel_in, reset_in);
	LCD lcd_4 = LCD(SPI_in4, reg_sel_in, reset_in);
	LCD lcd_5 = LCD(SPI_in5, reg_sel_in, reset_in);

	lcd_1.init();
	lcd_2.init();
	lcd_3.init();
	lcd_4.init();
	lcd_5.init();

	lcd_1.clear_screen();
	lcd_2.clear_screen();
	lcd_3.clear_screen();
	lcd_4.clear_screen();
	lcd_5.clear_screen();

	lcd_1.drawCharTest({50, 100}, 'h', BLACK, WHITE, 10);
	lcd_2.drawCharTest({50, 100}, 'e', BLACK, WHITE, 10);
	lcd_3.drawCharTest({50, 100}, 'l', BLACK, WHITE, 10);
	lcd_4.drawCharTest({50, 100}, 'l', BLACK, WHITE, 10);
	lcd_5.drawCharTest({50, 100}, 'o', BLACK, WHITE, 10);

	HAL_Delay(2000);


	lcd_1.drawCharTest({50, 100}, 'w', BLACK, WHITE, 10);
	lcd_2.drawCharTest({50, 100}, 'o', BLACK, WHITE, 10);
	lcd_3.drawCharTest({50, 100}, 'r', BLACK, WHITE, 10);
	lcd_4.drawCharTest({50, 100}, 'l', BLACK, WHITE, 10);
	lcd_5.drawCharTest({50, 100}, 'd', BLACK, WHITE, 10);
}

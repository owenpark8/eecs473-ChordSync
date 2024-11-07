#include "fretboard.hpp"

#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

Fretboard fretboard;

auto init() -> void {
	Pin CS_A = Pin(CSX_A_GPIO_Port, CSX_A_Pin);
	Pin CS_B = Pin(CSX_B_GPIO_Port, CSX_B_Pin);
	Pin CS_C = Pin(CSX_C_GPIO_Port, CSX_C_Pin);
	Pin CS_D = Pin(CSX_D_GPIO_Port, CSX_D_Pin);
	Pin CS_E = Pin(CSX_E_GPIO_Port, CSX_E_Pin);
	Pin CS_F = Pin(CSX_F_GPIO_Port, CSX_F_Pin);

	SPI SPI_A = SPI(&hspi1, CS_A);
	SPI SPI_B = SPI(&hspi1, CS_B);
	SPI SPI_C = SPI(&hspi2, CS_C);
	SPI SPI_D = SPI(&hspi2, CS_D);
	SPI SPI_E = SPI(&hspi3, CS_E);
	SPI SPI_F = SPI(&hspi3, CS_F);


	Pin reg_sel_in = Pin(LCD_RS_GPIO_Port, LCD_RS_Pin);
	Pin reset_in = Pin(LCD_RST_GPIO_Port, LCD_RST_Pin);

	LCD lcd_1 = LCD(SPI_A, reg_sel_in, reset_in);
	LCD lcd_2 = LCD(SPI_B, reg_sel_in, reset_in);
	LCD lcd_3 = LCD(SPI_C, reg_sel_in, reset_in);
	LCD lcd_4 = LCD(SPI_D, reg_sel_in, reset_in);
	LCD lcd_5 = LCD(SPI_E, reg_sel_in, reset_in);
	LCD lcd_6 = LCD(SPI_F, reg_sel_in, reset_in);


	fretboard = Fretboard{lcd_1, lcd_2, lcd_3, lcd_4, lcd_5, lcd_6};
	fretboard.init();

	lcd_1.fill_screen(GREEN);
	HAL_Delay(20);
	lcd_2.fill_screen(GREEN);
	HAL_Delay(20);
	lcd_3.fill_screen(GREEN);
	HAL_Delay(20);
	lcd_4.fill_screen(GREEN);
	HAL_Delay(20);
	lcd_5.fill_screen(GREEN);
	HAL_Delay(20);
	while(true) {
		for(int i = 0; i < 23; ++i) {
			fretboard.draw_note({i, string_e::HIGH_E}, (i % 2 == 0) ? RED : BLUE);
			HAL_Delay(20);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	fretboard.handle_uart_message(huart);
}


extern "C" {
	void HAL_Post_Init() {
		init();
	}
}

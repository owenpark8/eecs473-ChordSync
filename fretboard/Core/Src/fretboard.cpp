#include "fretboard.hpp"
#include "timing.hpp"
#include "amp.hpp"

#include "main.h"


extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim6;

#define SONG_TIMER &htim6

Fretboard fretboard;
Amplifier amp;

auto init() -> void {
	Pin AMP(AMP_AMP_GPIO_Port, AMP_AMP_Pin);
	Pin MOD(AMP_MOD_GPIO_Port, AMP_MOD_Pin);
	Pin DLY(AMP_DLY_GPIO_Port, AMP_DLY_Pin);
	Pin RVB(AMP_RVB_GPIO_Port, AMP_RVB_Pin);
	Pin AMP_PWR(AMP_PWR_GPIO_Port, AMP_PWR_Pin);

	amp = Amplifier{AMP, MOD, DLY, RVB, AMP_PWR};
	amp.power_on();

    Pin CS_A = Pin(CSX_A_GPIO_Port, CSX_A_Pin);
    Pin CS_B = Pin(CSX_B_GPIO_Port, CSX_B_Pin);
    Pin CS_C = Pin(CSX_C_GPIO_Port, CSX_C_Pin);
    Pin CS_D = Pin(CSX_D_GPIO_Port, CSX_D_Pin);
    Pin CS_E = Pin(CSX_E_GPIO_Port, CSX_E_Pin);
    Pin CS_F = Pin(CSX_F_GPIO_Port, CSX_F_Pin);

    SPI SPI_A = SPI(&hspi3, CS_A);
    SPI SPI_B = SPI(&hspi3, CS_B);
    SPI SPI_C = SPI(&hspi2, CS_C);
    SPI SPI_D = SPI(&hspi2, CS_D);
    SPI SPI_E = SPI(&hspi1, CS_E);
    SPI SPI_F = SPI(&hspi1, CS_F);


    Pin reg_sel_in = Pin(LCD_RS_GPIO_Port, LCD_RS_Pin);
    Pin reset_in = Pin(LCD_RST_GPIO_Port, LCD_RST_Pin);

    LCD lcd_1 = LCD(SPI_A, reg_sel_in, reset_in);
    LCD lcd_2 = LCD(SPI_B, reg_sel_in, reset_in);
    LCD lcd_3 = LCD(SPI_C, reg_sel_in, reset_in);
    LCD lcd_4 = LCD(SPI_D, reg_sel_in, reset_in);
    LCD lcd_5 = LCD(SPI_E, reg_sel_in, reset_in);
    LCD lcd_6 = LCD(SPI_F, reg_sel_in, reset_in);

    HAL_Delay(timing::LCD_BOOTUP_TIME.count());

    fretboard = Fretboard{lcd_6, lcd_5, lcd_4, lcd_3, lcd_2, lcd_1, &huart1};
    fretboard.init();

    HAL_TIM_Base_Start_IT(SONG_TIMER);

    // Checkerboard debug code
    // while (true) {
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::HIGH_E}, (i % 2 == 0) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::A}, (i % 2 == 1) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::D}, (i % 2 == 0) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::G}, (i % 2 == 1) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::B}, (i % 2 == 0) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     for (int i = 0; i < 23; ++i) {
    //         fretboard.draw_note({i, string_e::LOW_E}, (i % 2 == 1) ? GREEN : WHITE);
    //         HAL_Delay(20);
    //     }
    //     fretboard.draw_string(string_e::HIGH_E, BLACK);
    //     fretboard.draw_string(string_e::A, BLACK);
    //     fretboard.draw_string(string_e::D, BLACK);
    //     fretboard.draw_string(string_e::G, BLACK);
    //     fretboard.draw_string(string_e::B, BLACK);
    //     fretboard.draw_string(string_e::LOW_E, BLACK);

    //     HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_SET);
    //     HAL_Delay(500);
    //     HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_RESET);
    //     HAL_Delay(500);
    // }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) { fretboard.handle_uart_message(); }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim == SONG_TIMER) {
        fretboard.handle_song_time();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) { fretboard.handle_uart_error(); }


extern "C" {
void HAL_Post_Init() { init(); }
}

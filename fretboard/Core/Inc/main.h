/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void HAL_Post_Init();
// void MX_USART1_UART_Init();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_4
#define LCD_RS_GPIO_Port GPIOA
#define CSX_A_Pin GPIO_PIN_4
#define CSX_A_GPIO_Port GPIOC
#define CSX_B_Pin GPIO_PIN_5
#define CSX_B_GPIO_Port GPIOC
#define CSX_C_Pin GPIO_PIN_0
#define CSX_C_GPIO_Port GPIOB
#define CSX_D_Pin GPIO_PIN_1
#define CSX_D_GPIO_Port GPIOB
#define CSX_E_Pin GPIO_PIN_2
#define CSX_E_GPIO_Port GPIOB
#define CSX_F_Pin GPIO_PIN_10
#define CSX_F_GPIO_Port GPIOB
#define DEBUG_LED_Pin GPIO_PIN_12
#define DEBUG_LED_GPIO_Port GPIOB
#define AMP_PWR_Pin GPIO_PIN_8
#define AMP_PWR_GPIO_Port GPIOA
#define AMP_RVB_Pin GPIO_PIN_9
#define AMP_RVB_GPIO_Port GPIOA
#define AMP_DLY_Pin GPIO_PIN_10
#define AMP_DLY_GPIO_Port GPIOA
#define AMP_MOD_Pin GPIO_PIN_11
#define AMP_MOD_GPIO_Port GPIOA
#define AMP_AMP_Pin GPIO_PIN_12
#define AMP_AMP_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define areset_n_Pin GPIO_PIN_0
#define areset_n_GPIO_Port GPIOD
#define adrdy_n_Pin GPIO_PIN_8
#define adrdy_n_GPIO_Port GPIOA
#define adrdy_n_EXTI_IRQn EXTI9_5_IRQn
#define bt_reset_Pin GPIO_PIN_9
#define bt_reset_GPIO_Port GPIOF
#define power_Pin GPIO_PIN_12
#define power_GPIO_Port GPIOD
#define bt_disct_Pin GPIO_PIN_15
#define bt_disct_GPIO_Port GPIOF
#define astart_Pin GPIO_PIN_1
#define astart_GPIO_Port GPIOB
#define bt_mute_Pin GPIO_PIN_13
#define bt_mute_GPIO_Port GPIOF
#define uart_rts_Pin GPIO_PIN_0
#define uart_rts_GPIO_Port GPIOA
#define ncs_Pin GPIO_PIN_4
#define ncs_GPIO_Port GPIOA
#define uart_cts_Pin GPIO_PIN_1
#define uart_cts_GPIO_Port GPIOA
#define bt_status_Pin GPIO_PIN_14
#define bt_status_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "circular_buffer.h"
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "utils.h"
#include "tps.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

Ads1292R adc;
CircularBuffer<int> adc_buf(100);
CircularBuffer<uint8_t> uart_buf(100);
Tps65070 tps;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  SEGGER_RTT_printf(0, "Starting cardiocase\n");
  MX_GPIO_Init();
  MX_I2C1_Init();
  // MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  // MX_TIM7_Init();

  HAL_TIM_Base_Start(&htim2);
  // HAL_GPIO_WritePin(bt_reset_GPIO_Port, bt_reset_Pin, GPIO_PIN_RESET);
  // circ_buf = CircularBuffer<int>(100);
  // MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_GPIO_WritePin(power_GPIO_Port, power_Pin, GPIO_PIN_RESET);
  
  // TPS
  tps = Tps65070(hi2c1);
  tps.init();

  HAL_GPIO_WritePin(bt_reset_GPIO_Port, bt_reset_Pin, GPIO_PIN_SET);
  // HAL_Delay(500);

  // HAL_Delay(10000);
  // ADC
  adc = Ads1292R(hspi1, htim2);
  adc.init();
  HAL_Delay(5000);
  HAL_GPIO_WritePin(power_GPIO_Port, power_Pin, GPIO_PIN_RESET);
  // NVIC_SystemReset();

  /* USER CODE END 2 */

  /* Infinite loop */
  // while (1);
  /* USER CODE BEGIN WHILE */
  
  //Timer start
  // __HAL_TIM_CLEAR_FLAG(&htim7, TIM_SR_UIF);
  // HAL_TIM_Base_Start_IT(&htim7);
  // send_command(&huart2, (uint8_t *)"AT\r\n", 4, uart_buf);
  // while (1);
  send_command(&huart2, (uint8_t *)"AT+BAUD=57600\r\n", 15, uart_buf);
  send_command(&huart2, (uint8_t *)"AT+LPM=1\r\n", 10, uart_buf);
  send_command(&huart2, (uint8_t *)"AT+TPMODE=0\r\n", 13, uart_buf);
  // HAL_UART_Transmit(&huart2, (uint8_t *)"AT+LPM=1", 8, HAL_MAX_DELAY);
  
  recieve_command((uint8_t *)"+GATTDATA=5,AdcOn", 17, uart_buf);
  // send_command(&huart2, (uint8_t *)"AT+LESEND=3,100\r\n", 19, uart_buf);

  int voltage = tps.voltage_measurment();
  send_voltage(&huart2, voltage, uart_buf);
  recieve_command((uint8_t *)"OK", 2, uart_buf);

  int i = 0, buf[40], error_cnt = 0;
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    if (!adc_buf.empty())
    {
      int result = adc_buf.get();
      buf[i] = result;
      i++;
      if (i >= 40)
      {
        error_cnt += send_data(&huart2, buf, 40, uart_buf);
        i = 0;

        if (error_cnt > 5)
        {
          HAL_GPIO_WritePin(power_GPIO_Port, power_Pin, GPIO_PIN_RESET);
        }
      }
      // SEGGER_RTT_printf(0, "RESULT: %d\n", result);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */

// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Check which version of the timer triggered this callback and toggle LED
  if (htim == &htim7)
  {
    // SEGGER_RTT_printf(0, "Hello from timer\n");
    int voltage = tps.voltage_measurment();
    send_voltage(&huart2, voltage, uart_buf);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

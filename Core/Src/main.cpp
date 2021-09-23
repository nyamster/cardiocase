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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();

  HAL_TIM_Base_Start(&htim2);
  // circ_buf = CircularBuffer<int>(100);
  // MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_GPIO_WritePin(power_GPIO_Port, power_Pin, GPIO_PIN_RESET);
  
  // TPS
  int status;
  status = HAL_I2C_IsDeviceReady(&hi2c1, 0x48<<1, 10, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  uint8_t buf[] = {0x16, 0x47};
  status = HAL_I2C_Master_Transmit(&hi2c1, 0x48<<1, buf, 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  uint8_t buf1[] = {0x10, 0x33}; //0x25 - 1.8V 0x33 - 2.5V
  status = HAL_I2C_Master_Transmit(&hi2c1, 0x48<<1, buf1, 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  uint8_t buf2[1] = {1};
  status = HAL_I2C_Mem_Read(&hi2c1, 0x48<<1, 0x10, I2C_MEMADD_SIZE_8BIT, buf2, 1, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  SEGGER_RTT_printf(0, "I2c: %x\n", buf2[0]);

  // ADC
  adc = Ads1292R(hspi1, htim2);

  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(astart_GPIO_Port, astart_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(areset_n_GPIO_Port, areset_n_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(areset_n_GPIO_Port, areset_n_Pin, GPIO_PIN_SET);
  HAL_Delay(200);

  // Reset ADC
  adc.write_command(0x06);
  HAL_Delay(100);

  // Set command mode
  adc.write_command(0x11);
  HAL_Delay(100);

  // Read ADC id
  uint8_t reg = adc.read_reg(0x00);
  SEGGER_RTT_printf(0, "Adc: %x\n", reg);
  HAL_Delay(100);

  // Writing into CONFIG 1 register
  adc.write_reg(0x01, 0x01);
  HAL_Delay(100);

  reg = adc.read_reg(0x01);
  SEGGER_RTT_printf(0, "CONFIG 1: %x, expected: 0x01\n", reg);

  // Writing into CONFIG 2 register
  //1010_0011
  adc.write_reg(0x02, 0xA3);
  HAL_Delay(100);

  reg = adc.read_reg(0x02);
  SEGGER_RTT_printf(0, "CONFIG 2: %x, expected: 0xE3\n", reg);

  // Writing into LOFF register
  // adc.write_reg(0x03, 0x10);
  // HAL_Delay(100);

  // reg = adc.read_reg(0x03);
  // SEGGER_RTT_printf(0, "LOFF: %x, expected: 0x10\n", reg);

  // Writing into CH1SET register
  //01010101
  adc.write_reg(0x04, 0x55);
  HAL_Delay(100);

  // Read register
  reg = adc.read_reg(0x01);
  SEGGER_RTT_printf(0, "CH1SET: %x, expected: 0x55\n", reg);
  HAL_Delay(100);

  // Writing into CH2SET register
  adc.write_reg(0x05, 0x55);
  HAL_Delay(100);

  // Read register
  reg = adc.read_reg(0x05);
  SEGGER_RTT_printf(0, "CH2SET: %x, expected: 0x55\n", reg);
  HAL_Delay(100);

  // Set continuous mode
  adc.write_command(0x10);
  HAL_Delay(100);

  HAL_GPIO_WritePin(astart_GPIO_Port, astart_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  
  // Ads1292R adc (hspi1);
  // adc.init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t data;
  // HAL_UART_Receive_IT(&huart2, uart, 1);
  // send_command(&huart2, (uint8_t *)"AT+TPMODE=0\r\n", 13, uart_buf);
  // send_command(&huart2, (uint8_t *)"AT+BAUD=460800\r\n", 16, uart_buf);
  // send_command(&huart2, (uint8_t *)"AT+LPM=1\r\n", 10, uart_buf);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+LPM=1", 8, HAL_MAX_DELAY);
  
  // recieve_command((uint8_t *)"+GATTDATA=5,AdcOn", 17, uart_buf);
  while (1)
  {
    // status = HAL_UART_Receive(&huart2, &data, 1, HAL_MAX_DELAY);
    // if (status != HAL_OK)
    // {
    //   SEGGER_RTT_printf(0, "ERROR: %d\n", status);
    // }
    // if (data != 0) {
    //   SEGGER_RTT_printf(0, "Uart data: %d\n", data);
    // }
    /* USER CODE END WHILE */
    // if (!adc_buf.empty())
    // {
    //   int result = adc_buf.get();
    //   // SEGGER_RTT_printf(0, "RESULT: %d\n", result);
    // }
    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 2;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

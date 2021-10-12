#include "adc.h"
#include "usart.h"
#include "utils.h"
#include "circular_buffer.h"

void Ads1292R::delay_us(int delay)
{
  // DWT->CYCCNT = 0; // сброс счётчика
  // while(DWT->CYCCNT < 64*delay) {} // задержка 5 мкс (5 * 72 )
  TIM2->CNT = 0;
  while(TIM2->CNT < delay) {}
}

void Ads1292R::write_reg(uint8_t reg, uint8_t value)
{
    uint8_t cmd = 0x40 + reg;
    uint8_t buf[] = {cmd, 0, value};
    int status;

    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    delay_us(20);
    status = HAL_SPI_Transmit(&spi, &buf[0], 1, HAL_MAX_DELAY);
    delay_us(10);
    status = HAL_SPI_Transmit(&spi, &buf[1], 1, HAL_MAX_DELAY);
    delay_us(10);
    status = HAL_SPI_Transmit(&spi, &buf[2], 1, HAL_MAX_DELAY);
    delay_us(20);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
    }
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
}

uint8_t Ads1292R::read_reg(uint8_t reg)
{
  uint8_t cmd = 0x20 + reg;
  uint8_t buf_trans[] = {cmd, 0, 0};
  uint8_t buf_rec[] = {0, 0, 0};
  int status;

  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
  delay_us(20);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[0], &buf_rec[0], 1, HAL_MAX_DELAY);
  delay_us(10);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[1], &buf_rec[1], 1, HAL_MAX_DELAY);
  delay_us(10);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[2], &buf_rec[2], 1, HAL_MAX_DELAY);
  delay_us(20);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }
  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
  return buf_rec[2];
}

void Ads1292R::write_command(uint8_t command)
{
    int status;

    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    delay_us(20);
    status = HAL_SPI_Transmit(&spi, &command, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
    }
    delay_us(20);
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
}

int Ads1292R::read_data()
{
    int status;
    uint8_t buf[] = {0x0};
    uint8_t data_buf[4] = {0, 0, 0, 0};
    uint8_t status_buf[4] = {0, 0, 0, 0};
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    this->delay_us(10);

    // Read status word
    for (int i = 0; i < 3; i++) {
      status = HAL_SPI_TransmitReceive(&spi, &buf[0], &status_buf[i], 1, HAL_MAX_DELAY);
      if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d \n", status);
      }
    }

    // Read data
    for (int i = 0; i < 3; i++) {
      status = HAL_SPI_TransmitReceive(&spi, &buf[0], &data_buf[i], 1, HAL_MAX_DELAY);
      if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
      }
    }

    for (int i = 0; i < 3; i++) {
      status = HAL_SPI_TransmitReceive(&spi, &buf[0], &data_buf[i], 1, HAL_MAX_DELAY);
      if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
      }
    }

    int status_word = (status_buf[0] << 8*2) | (status_buf[1] << 8*1) | (status_buf[2] << 8*0);
    // SEGGER_RTT_printf(0, "status word: %x\n", status_word);
    if (status_buf[0] != 0xC0) {
      SEGGER_RTT_printf(0, "Wrong status word: %x\n", status_word);
      while(1);
    }

    int result = (data_buf[0] << 8*2) | (data_buf[1] << 8*1) | (data_buf[2] << 8*0);
    this->delay_us(10);
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
    this->delay_us(10);

    return result;
}

void Ads1292R::init()
{
  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
  // HAL_Delay(200);
  HAL_GPIO_WritePin(astart_GPIO_Port, astart_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(areset_n_GPIO_Port, areset_n_Pin, GPIO_PIN_RESET);
  // HAL_Delay(1000);
  HAL_GPIO_WritePin(areset_n_GPIO_Port, areset_n_Pin, GPIO_PIN_SET);
  // HAL_Delay(200);

  // Reset ADC
  // write_command(0x06);
  // HAL_Delay(200);

  // Set command mode
  write_command(0x11);
  HAL_Delay(100);

  // Read ADC id
  uint8_t reg = read_reg(0x00);
  SEGGER_RTT_printf(0, "Adc: %x\n", reg);
  HAL_Delay(100);

  // Writing into CONFIG 2 register
  //1010_0011
  write_reg(0x02, 0xA3);
  HAL_Delay(100);

  reg = read_reg(0x02);
  SEGGER_RTT_printf(0, "CONFIG 2: %x, expected: 0xA3\n", reg);
  // if (reg != 0xA3)
  // {
  //   send_command(&huart2, (uint8_t *)"AT+LESEND=2,NO\r\n", 16, uart_buf);
  // }
  // else
  // {
  //   send_command(&huart2, (uint8_t *)"AT+LESEND=3,YES\r\n", 17, uart_buf);
  // }

  // Writing into CONFIG 1 register
  write_reg(0x01, 0x01);
  HAL_Delay(100);

  reg = read_reg(0x01);
  SEGGER_RTT_printf(0, "CONFIG 1: %x, expected: 0x01\n", reg);


  // Writing into LOFF register
  // write_reg(0x03, 0x10);
  // HAL_Delay(100);

  // reg = read_reg(0x03);
  // SEGGER_RTT_printf(0, "LOFF: %x, expected: 0x10\n", reg);

  // Writing into CH1SET register
  //01010101
  write_reg(0x04, 0x55);
  HAL_Delay(100);

  // Read register
  reg = read_reg(0x04);
  SEGGER_RTT_printf(0, "CH1SET: %x, expected: 0x55\n", reg);
  HAL_Delay(100);

  // Writing into CH2SET register
  write_reg(0x05, 0x55);
  HAL_Delay(100);

  // Read register
  reg = read_reg(0x05);
  SEGGER_RTT_printf(0, "CH2SET: %x, expected: 0x55\n", reg);
  HAL_Delay(100);

  // // Writing into LOFF_SENS register
  // write_reg(0x07, 0x3C);
  // HAL_Delay(100);

  // // Read register
  // reg = read_reg(0x07);
  // SEGGER_RTT_printf(0, "LOFF_SENS: %x, expected: 0x3C\n", reg);
  // HAL_Delay(100);

  // Set continuous mode
  // write_command(0x10);
  // HAL_Delay(100);

  // HAL_GPIO_WritePin(astart_GPIO_Port, astart_Pin, GPIO_PIN_SET);
  // HAL_Delay(100);
}
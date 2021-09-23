#include "adc.h"

void Ads1292R::delay_us(int delay) {
  // DWT->CYCCNT = 0; // сброс счётчика
  // while(DWT->CYCCNT < 64*delay) {} // задержка 5 мкс (5 * 72 )
  TIM2->CNT = 0;
  while(TIM2->CNT < delay) {}
}

void Ads1292R::write_reg(uint8_t reg, uint8_t value) {
    uint8_t cmd = 0x40 + reg;
    uint8_t buf[] = {cmd, 0, value};
    int status;

    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    status = HAL_SPI_Transmit(&spi, &buf[0], 1, HAL_MAX_DELAY);
    status = HAL_SPI_Transmit(&spi, &buf[1], 1, HAL_MAX_DELAY);
    status = HAL_SPI_Transmit(&spi, &buf[2], 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
    }
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
}

uint8_t Ads1292R::read_reg(uint8_t reg) {
  uint8_t cmd = 0x20 + reg;
  uint8_t buf_trans[] = {cmd, 0, 0};
  uint8_t buf_rec[] = {0, 0, 0};
  int status;

  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[0], &buf_rec[0], 1, HAL_MAX_DELAY);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[1], &buf_rec[1], 1, HAL_MAX_DELAY);
  status = HAL_SPI_TransmitReceive(&spi, &buf_trans[2], &buf_rec[2], 1, HAL_MAX_DELAY);
  HAL_Delay(1);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }
  HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
  return buf_rec[2];
}

void Ads1292R::write_command(uint8_t command) {
    int status;

    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    status = HAL_SPI_Transmit(&spi, &command, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: %d\n", status);
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
}

int Ads1292R::read_data() {
    int status;
    uint8_t buf[] = {0x0};
    uint8_t data_buf[4] = {0, 0, 0, 0};
    uint8_t status_buf[4] = {0, 0, 0, 0};
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_RESET);
    this->delay_us(40);

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
    this->delay_us(40);
    HAL_GPIO_WritePin(ncs_GPIO_Port, ncs_Pin, GPIO_PIN_SET);
    this->delay_us(20);

    return result;
}
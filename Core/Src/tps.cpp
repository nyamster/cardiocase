#include "tps.h"

void Tps65070::init()
{
  // TPS
  int status;
  status = HAL_I2C_IsDeviceReady(&i2c, 0x48<<1, 10, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  constexpr uint8_t tmp[] = {0x16, 0x47};
  status = HAL_I2C_Master_Transmit(&i2c, 0x48<<1, const_cast<uint8_t *>(tmp), 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  constexpr uint8_t tmp1[] = {0x10, 0x33}; //0x25 - 1.8V 0x33 - 2.5V
  status = HAL_I2C_Master_Transmit(&i2c, 0x48<<1, const_cast<uint8_t *>(tmp1), 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  constexpr uint8_t tmp2[1] = {1};
  status = HAL_I2C_Mem_Read(&i2c, 0x48<<1, 0x10, I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t *>(tmp2), 1, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  // Voltage measurment, Voltage values: 3,0V - 4,2V
  constexpr uint8_t tmp3[] = {0x07, 0xd9};
  status = HAL_I2C_Master_Transmit(&i2c, 0x48<<1, const_cast<uint8_t *>(tmp3), 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  SEGGER_RTT_printf(0, "I2c: %x\n", tmp2[0]);
}

int Tps65070::voltage_measurment()
{
  constexpr uint8_t tmp[] = {0x07, 0xd9};
  int status = 0;

  status = HAL_I2C_Master_Transmit(&i2c, 0x48<<1, (uint8_t *)tmp, 2, HAL_MAX_DELAY);
  if (status != HAL_OK) {
    SEGGER_RTT_printf(0, "ERROR: %d\n", status);
  }

  uint8_t recieve[] = {0x09, 0x0a};
  status = HAL_I2C_Master_Receive(&i2c, 0x48<<1, &recieve[0], 1, HAL_MAX_DELAY);
  status = HAL_I2C_Master_Receive(&i2c, 0x48<<1, &recieve[1], 1, HAL_MAX_DELAY);

  int battery_charge = (recieve[1] << 8) | recieve[0];
  battery_charge = battery_charge * 6000 / 1024;
  battery_charge = ((battery_charge - 3000) / 12) % 100;
  // SEGGER_RTT_printf(0, "Voltage: %d", battery_charge);
  return battery_charge;
}
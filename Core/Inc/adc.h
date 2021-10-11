#ifndef ADC_H
#define ADC_H

#include "main.h"

#define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC

class Ads1292R {
    private:
    SPI_HandleTypeDef spi;
    TIM_HandleTypeDef timer;

    public:
    Ads1292R(SPI_HandleTypeDef _spi, TIM_HandleTypeDef _timer) : spi(_spi), timer(_timer) {
        SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешение счётчика
        DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запуск счётчика
    }
    Ads1292R(){}

    int read_data();
    void write_reg(uint8_t reg, uint8_t value);
    uint8_t read_reg(uint8_t reg);
    void write_command(uint8_t command);
    void delay_us(int delay);
    void init();
};

extern Ads1292R adc;

#endif
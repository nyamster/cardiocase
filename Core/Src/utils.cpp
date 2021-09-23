#include "utils.h"
#include <cstring>

void send_command(UART_HandleTypeDef *huart, uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf)
{
    uint8_t buf[10];
    int cnt = 0;

    HAL_UART_Transmit(huart, command, len, HAL_MAX_DELAY);
    while (1)
    {
        if (!uart_buf.empty()) {
            buf[cnt] = uart_buf.get();
            // SEGGER_RTT_printf(0, "Uart data: %c\n", buf[cnt]);
            cnt += 1;
        }
        if (buf[0] == 'O' && buf[1] == 'K') {
            SEGGER_RTT_printf(0, "OK\n");
            return;
        }
    }
}

void recieve_command(uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf)
{
    uint8_t buf[len];
    int cnt = 0;

    while (1)
    {
        if (!uart_buf.empty()) {
            buf[cnt] = uart_buf.get();
            // SEGGER_RTT_printf(0, "Uart data: %c\n", buf[cnt]);
            cnt += 1;
        }
        if (!strcmp((char*)buf, (char*)command)) {
            SEGGER_RTT_printf(0, "Recieved\n");
            return;
        }
    }
}
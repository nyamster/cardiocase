#include "utils.h"
#include <cstring>

int send_command(UART_HandleTypeDef *huart, uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf)
{
    uint8_t buf[] = {0,0,0,0,0,0,0,0,0,0};
    int cnt = 0;

    // while (1)
    // {
        HAL_UART_Transmit(huart, command, len, HAL_MAX_DELAY);
    //     HAL_Delay(1000);
    // }
    while (1)
    {
        if (!uart_buf.empty())
        {
            buf[cnt] = uart_buf.get();
            // SEGGER_RTT_printf(0, "Uart data: %c\n", buf[cnt]);
            cnt += 1;
        }
        if (buf[0] == 'O' && buf[1] == 'K')
        {
            SEGGER_RTT_printf(0, "OK\n");
            return 0;
        }
        if (buf[0] == 'E' &&
            buf[1] == 'R' &&
            buf[2] == 'R' &&
            buf[3] == 'O' &&
            buf[4] == 'R')
        {
            SEGGER_RTT_printf(0, "ERROR\n");
            return 1;
        }
        if (buf[0] == '+' &&
            buf[1] == 'G' &&
            buf[2] == 'A' &&
            buf[3] == 'T' &&
            buf[4] == 'T')
        {
            SEGGER_RTT_printf(0, "Turning off\n");
            HAL_GPIO_WritePin(power_GPIO_Port, power_Pin, GPIO_PIN_RESET);
            return 1;
        }
    }
}

int send_voltage(UART_HandleTypeDef *huart, int voltage, CircularBuffer<uint8_t> &uart_buf)
{
    int const str_len = 17;
    char *data = (char *)malloc(str_len * sizeof(char));
    // char *msg = (char *)malloc(len*6*sizeof(char));
    char *pointer = data;
    pointer += sprintf(pointer, "AT+LESEND=%d,", 3);
    pointer += sprintf(pointer, "%03d", voltage);
    pointer += sprintf(pointer, "\r\n");
    SEGGER_RTT_printf(0, "STRIMG: %s\n", data);
    return HAL_UART_Transmit(huart, (uint8_t *)data, str_len, HAL_MAX_DELAY);
}

void recieve_command(uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf)
{
    uint8_t buf[len];
    int cnt = 0;

    while (1)
    {
        if (!uart_buf.empty()) {
            buf[cnt] = uart_buf.get();
            SEGGER_RTT_printf(0, "Uart data: %c\n", buf[cnt]);
            cnt += 1;
        }
        if (!strcmp((char*)buf, (char*)command)) {
            SEGGER_RTT_printf(0, "Recieved\n");
            return;
        }
    }
}

int send_data(UART_HandleTypeDef *huart, int *buf, int len, CircularBuffer<uint8_t> &uart_buf)
{
    int const str_len = 13 + len*6 + 3;
    char *data = (char *)malloc(str_len * sizeof(char));
    char *pointer = data;
    __uint8_t num[len*3];
    int count = 0;
    pointer += sprintf(pointer, "AT+LESEND=%d,", len*6);
    for (int i = 0; i < len; i++)
    {
        // uint8_t bytes[4];
        buf[i] >>= 5;
        // SEGGER_RTT_printf(0, "RESULT: %d\n", buf[i]);
        // bytes[0] = (buf[i] >> 24) & 0xFF;
        num[count] = (buf[i] >> 16) & 0xFF;
        pointer += sprintf(pointer, "%02x", num[count]);
        count += 1;
        num[count] = (buf[i] >> 8) & 0xFF;
        pointer += sprintf(pointer, "%02x", num[count]);
        count += 1;
        num[count] = buf[i] & 0xFF;
        pointer += sprintf(pointer, "%02x", num[count]);
        count += 1;

    }
    pointer += sprintf(pointer, "\r\n");
    // sprintf(data, "%d", num);
    // SEGGER_RTT_printf(0, "STRIMG: %s\n", data);
    return send_command(huart, (uint8_t *)data, str_len, uart_buf);
}
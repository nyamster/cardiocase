#include "main.h"
#include "circular_buffer.h"

int send_command(UART_HandleTypeDef *huart, uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf);
void recieve_command(uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf);
int send_data(UART_HandleTypeDef *huart, int *buf, int len, CircularBuffer<uint8_t> &uart_buf);
int send_voltage(UART_HandleTypeDef *huart, int voltage, CircularBuffer<uint8_t> &uart_buf);
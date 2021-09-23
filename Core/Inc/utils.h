#include "main.h"
#include "circular_buffer.h"

void send_command(UART_HandleTypeDef *huart, uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf);
void recieve_command(uint8_t *command, int len, CircularBuffer<uint8_t> &uart_buf);
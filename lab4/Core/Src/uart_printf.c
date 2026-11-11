#include "uart_printf.h"

UART_HandleTypeDef *huart;

void init_uart_printf(UART_HandleTypeDef *uart) {
	huart = uart;
}

PUTCHAR_PROTOTYPE
{

 HAL_UART_Transmit(huart, (uint8_t *)&ch, 1, 0xFFFF);

return ch;
}

#ifndef BSP_USART_H
#define BSP_USART_H

#include "driver/uart.h"
#include "driver/gpio.h"

#define USART_UX            UART_NUM_2
#define USART_TX_GPIO_PIN   GPIO_NUM_7
#define USART_RX_GPIO_PIN   GPIO_NUM_6
#define RX_BUF_SIZE         1024

void Usart_Init(uint32_t baudrate);

#endif //BSP_USART_H
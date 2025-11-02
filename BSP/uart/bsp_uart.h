#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm8s.h"
#include "stdio.h"

void uart_hw_init(u32 baudrate);
void uart_set_rx_callback(void (*rx_callback)(uint8_t data));
void uart_send_byte(uint8_t data);
void uart_send_bytes(const uint8_t *data, uint16_t len);

#endif

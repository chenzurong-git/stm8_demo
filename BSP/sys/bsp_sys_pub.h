#ifndef __BSP_SYS_PUB_H
#define __BSP_SYS_PUB_H
#include "stm8s.h"
#include <stdio.h>
#include <string.h>
#include "math.h"

#include "bsp_clk.h"
#include "bsp_uart.h"
#include "sys_timer.h"

void delay_us(u16 nCount);
void delay_ms(u16 nCount);
u16 Get_decimal(double dt, u8 deci);
void bsp_sys_init(void);
void assert_failed(uint8_t *file, uint32_t line);

#endif

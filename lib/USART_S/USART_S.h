#ifndef __USART_S_H
#define __USART_S_H

#include <driverlib.h>
#include <stdio.h>
#include "msp.h"

void UART_Init(void);

// UART接收中断处理程序
void EUSCIA0_IRQHandler(void);

// 定义__putchar函数以适配printf
int __putchar(int character);

#endif //__USART_S_H
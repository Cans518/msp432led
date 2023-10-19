#ifndef __DELAY_H
#define __DELAY_H
#include <driverlib.h>
#include <stdio.h>
#include "msp.h"

void delay_init(void);
void delay_ms(uint32_t nms);
void delay_us(uint32_t nus);

#endif

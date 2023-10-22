#ifndef __DELAY_H
#define __DELAY_H
#include <driverlib.h>
#include <stdio.h>
#include "msp.h"

void delay_init(void);
void delay_ms(uint16_t ms);
void delay_us(uint32_t us);;
void delay_s(uint16_t s) ;
#endif

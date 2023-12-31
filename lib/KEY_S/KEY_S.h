#ifndef __KEY_S_H
#define __KEY_S_H

#include<driverlib.h>

#define KEY1 MAP_GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN1)
#define KEY2 MAP_GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN4)
#define KEY_ON 0
#define KEY_OFF 1

void KEY_Init(void);
void KEY1_Init(void);
void KEY2_Init(void);
void KEY1_antishake(void);
void KEY2_antishake(void);

#endif //__KEY_S_H
#include <driverlib.h>
#include <KEY_s.h>

void KEY_Init(){
    MAP_GPIO_setAsInputPin(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);
}

void KEY1_Init(){
    MAP_GPIO_setAsInputPin(GPIO_PORT_P1,GPIO_PIN1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1);
}

void KEY2_Init(){
    MAP_GPIO_setAsInputPin(GPIO_PORT_P1,GPIO_PIN4);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN4);
}

void KEY1_antishake(){
    for (uint16_t i  = 0;i <= 500; i++){}
	for (uint16_t i  = 0;; i++){if(KEY1 == KEY_OFF)break;}
}

void KEY2_antishake(){
    for (uint16_t i  = 0;i <= 500; i++){}
    for (uint16_t i  = 0;; i++){if(KEY2 == KEY_OFF)break;}
}

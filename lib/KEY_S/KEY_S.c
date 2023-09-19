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

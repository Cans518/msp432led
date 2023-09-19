#ifndef __LED_S_H
#define __LED_S_H
#include <driverlib.h>

//LED1_红色 P1.0
#define LED1_RED BITBAND_PERI(P1OUT,0)
//LED2_红色 P2.0
#define LED2_RED BITBAND_PERI(P2OUT,0)
//LED2_绿色 P2.1
#define LED2_GREEN BITBAND_PERI(P2OUT,1)
//LED2_蓝色 P2.2
#define LED2_BLUE BITBAND_PERI(P2OUT,2)

//LED初始化
//初始化LED1,LED2
void LED_Init(void);
//初始化LED1
void LED1_Init(void);
//初始化LED2
void LED2_Init(void);

//对于LED1的操作 LED1为单色红灯 P1.0
//打开LED1的红色灯
void LED1_RED_ON(void);
//关闭LED1的红色灯
void LED1_RED_OFF(void);
//翻转LED1的红色灯
void LED1_RED_TOGGLE(void);

/*
    LED2是三色灯，三个接口对应P2.0(红)、P2.1(绿)、P2.2(蓝)
    根据排练组合LED2一共可以实现7种颜色：
        1. 红色
        2. 绿色
        3. 蓝色
        4. 黄色  （红色+绿色）
        5. 青色  （绿色+蓝色）
        6. 紫色  （红色+蓝色）
        7. 白色  （红色+绿色+蓝色）
*/

//对于LED2-红色的操作，LED2为三色灯 P2.0
//打开LED2的红色灯
void LED2_RED_ON(void);
//关闭LED2的红色灯
void LED2_RED_OFF(void);
//翻转LED2的红色灯
void LED2_RED_TOGGLE(void);


//对于LED2-绿色的操作，LED2为三色灯 P2.1
//打开LED2的绿色灯
void LED2_GREEN_ON(void);
//关闭LED2的绿色灯
void LED2_GREEN_OFF(void);
//翻转LED2的绿色灯
void LED2_GREEN_TOGGLE(void);

//对于LED2-蓝色的操作，LED2为三色灯 P2.2
//打开LED2的蓝色灯
void LED2_BLUE_ON(void);
//关闭LED2的蓝色灯
void LED2_BLUE_OFF(void);
//翻转LED2的蓝色灯
void LED2_BLUE_TOGGLE(void);

//对于LED2-黄色的操作，黄色=红色+绿色
//打开LED2的黄色灯
void LED2_YELLOW_ON(void);
//关闭LED2的黄色灯
void LED2_YELLOW_OFF(void);
//翻转LED2的黄色灯
void LED2_YELLOW_TOGGLE(void);

//对于LED2-青色的操作，青色=绿色+蓝色
//打开LED2的青色灯
void LED2_CYAN_ON(void);
//关闭LED2的青色灯
void LED2_CYAN_OFF(void);
//翻转LED2的青色灯
void LED2_CYAN_TOGGLE(void);

//对于LED2-紫色的操作，紫色=红色+蓝色
//打开LED2的紫色灯
void LED2_PURPLE_ON(void);
//关闭LED2的紫色灯
void LED2_PURPLE_OFF(void);
//翻转LED2的紫色灯
void LED2_PURPLE_TOGGLE(void);

//对于LED2-白色的操作，白色=红色+绿色+蓝色
//打开LED2的白色灯
void LED2_WHITE_ON(void);
//关闭LED2的白色灯
void LED2_WHITE_OFF(void);
//翻转LED2的白色灯
void LED2_WHITE_TOGGLE(void);

#endif //__LED_S_H
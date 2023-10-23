#include <driverlib.h>
#include <stdlib.h>
#include <KEY_S.h>
#include <LED_S.h>

#define TIMER_PERIODA 60000 // 周期 20ms——控制舵机的PWM波
#define DUTY_CYCLE 4500 // 2.5ms的工作时间，对应sg90舵机的90°位置

uint32_t duty_cycle = DUTY_CYCLE;
uint8_t cout_1 = 0, cout_2 = 0;

/* Timer_A2 Up Configuration Parameter */   //使用Timer_A2.1，对应引脚5.6
const Timer_A_UpModeConfig upConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz, 主频默认为3MHz，分频为1
        TIMER_PERIODA,                          // 60000 tick period, 20ms PWM周期
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value

};

/* Timer_A Compare Configuration Parameter  (PWM1) */  //对应引脚5.6
Timer_A_CompareModeConfig compareConfig_PWM =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1  TAx1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        DUTY_CYCLE                                  // 4500 Duty Cycle
};

void M_Delay(volatile uint32_t i){
    for ( i ; i > 0; i--){}
}

int main()
{
    MAP_WDT_A_holdTimer(); // 关闭看门狗

	KEY_Init(); // 初始化KEY
        
	LED2_Init();
	LED2_RED_ON(); // 初始化P2.0,并点亮

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化P5.6为PWM输出口

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfigA2); // 初始化Timer_A2.1

    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE); // 开启Timer_A2.1为向上计数模式

    MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 初始化Timer_A2.1输出的PWM波

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);  // 清空按键中断标识位

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4,GPIO_HIGH_TO_LOW_TRANSITION);   // 设置中断边沿

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4); // 打开按键中断

    MAP_Interrupt_enableInterrupt(INT_PORT1); // 使能按键中断

    MAP_Interrupt_enableSleepOnIsrExit(); // 允许未中断时进入睡眠低功耗模式

    MAP_Interrupt_enableMaster(); // 使能总中断	

	while(1)
		MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒

    return 0;
}

void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,status);	//清除标志位

	// 当KEY1(P1.1)被按下时
    if(KEY1 == KEY_ON){
        KEY1_antishake(); // 按键防抖
        LED2_RED_OFF(); // 关闭红灯
        LED2_BLUE_ON(); // 打开蓝灯,提示正在转动
        duty_cycle += 333; // 顺时针转动10°
        cout_1++;
        if(abs(cout_1 - cout_2) >= 10){
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
        M_Delay(100000); // 等待一段时间，等待转动完成
        LED2_BLUE_OFF(); // 关闭蓝灯
        LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
	}
    // 当KEY2(P1.4)被按下时
	if(KEY2 == KEY_ON){
        KEY2_antishake(); // 按键防抖
        LED2_GREEN_ON(); // 打开绿灯
        LED2_RED_OFF(); // 关闭红灯
        duty_cycle -= 333; // 逆时针转动10°
        cout_2++;
        if(abs(cout_1 - cout_2) >= 10){
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
        M_Delay(100000); // 等待一段时间，等待转动完成
        LED2_GREEN_OFF(); // 关闭绿灯
        LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
	}
}

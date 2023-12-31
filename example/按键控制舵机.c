#include <driverlib.h>
#include <stdlib.h>
#include <KEY_S.h>
#include <LED_S.h>
#include <DELAY_S.h>

#define TIMER_PERIODA 60000 // 周期 20ms——控制舵机的PWM波
#define DUTY_CYCLE 4500 // 1.5ms的工作时间，对应sg90舵机的90°位置

uint32_t duty_cycle = DUTY_CYCLE;
uint8_t cout_1 = 0, cout_2 = 0;

/*配置TIMERA为向上计数模式的参数结构体*/           //使用Timer_A2
const Timer_A_UpModeConfig upConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // 使用时钟源SMCLK
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz, 主频默认为3MHz，分频为1
        TIMER_PERIODA,                          // 设置CCR0值, 20ms PWM周期
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // 禁止计时器中断
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // 禁止CCR0触发中断
        TIMER_A_DO_CLEAR                        // 清空CCR0

};

/*配置TIMERA为比较模式输出PWM方波的参数结构体*/        //对应引脚5.6
Timer_A_CompareModeConfig compareConfig_PWM =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // 使用CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // 禁止CCR触发中断
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // 变换电平
        DUTY_CYCLE                                  // 设置CCR1的值，对应90°
};

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
        cout_1++; // 使用cout_1作为计数，计算顺时针选择次数
        if(abs(cout_1 - cout_2) >= 10){
            // 当cout_1和cout_2的差值大于等于10时，将cout_1清零，cout_2清零
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 初始化Timer_A2.1输出的PWM波
        delay_ms(35); // 等待一段时间，等待转动完成
        LED2_BLUE_OFF(); // 关闭蓝灯
        LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
	}
    // 当KEY2(P1.4)被按下时
	if(KEY2 == KEY_ON){
        KEY2_antishake(); // 按键防抖
        LED2_GREEN_ON(); // 打开绿灯
        LED2_RED_OFF(); // 关闭红灯
        duty_cycle -= 333; // 逆时针转动10°
        cout_2++; // 使用cout_2作为计数，计算逆时针选择次数
        if(abs(cout_1 - cout_2) >= 10){
            // 当cout_1和cout_2的差值大于等于10时，将cout_1清零，cout_2清零
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 初始化Timer_A2.1输出的PWM波
        delay_ms(35); // 等待一段时间，等待转动完成
        LED2_GREEN_OFF(); // 关闭绿灯
        LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
	}
}

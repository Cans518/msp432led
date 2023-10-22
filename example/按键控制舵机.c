#include <driverlib.h>
#include <stdlib.h>
#include <KEY_S.h>
#include <LED_S.h>

#define TIMER_PERIODA 60000 // 周期 20ms——控制舵机的PWM波
#define DUTY_CYCLE 4500 // 舵机转动90°（对应停车牌下落）

uint32_t duty_cycle = DUTY_CYCLE;
uint8_t cout_1 = 0, cout_2 = 0;

/* Timer_A2 Up Configuration Parameter */   //使用Timer_A2.1，对应引脚5.6
const Timer_A_UpModeConfig upConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz
        TIMER_PERIODA,                           // 60000 tick period
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
        DUTY_CYCLE                                 // 3000 Duty Cycle
};

void M_Delay(volatile uint32_t i){
    for ( i ; i > 0; i--){}
}

int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

	KEY_Init();

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);

    //初始化P2.0,并点亮
	LED2_Init();
	LED2_RED_ON();

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfigA2);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);


    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4); 

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);  //清空中断标识位

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4,GPIO_HIGH_TO_LOW_TRANSITION);   //edge

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);

    //使能中断
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();	

	while(1)
		MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒

    return 0;
}

void PORT1_IRQHandler(void)
{

    //中断服务
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,status);	//清除标志位

	if(KEY1 == KEY_ON){
		for (uint16_t i  = 0;i <= 500; i++){}
		for (uint16_t i  = 0;; i++){if(KEY1 == KEY_OFF)break;}
        LED2_RED_OFF();
        LED2_BLUE_ON();
        duty_cycle += 333;
        cout_1++;
        if(abs(cout_1 - cout_2) >= 10){
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
        M_Delay(100000);
        LED2_BLUE_OFF();
        LED2_RED_ON();
	}
	if(KEY2 == KEY_ON){
		for (uint16_t i  = 0;i <= 500; i++){}
		for (uint16_t i  = 0;; i++){if(KEY2 == KEY_OFF)break;}
        LED2_GREEN_ON();
        LED2_RED_OFF();
        duty_cycle -= 333;
        cout_2++;
        if(abs(cout_1 - cout_2) >= 10){
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
        }
        compareConfig_PWM.compareValue = duty_cycle;
        MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
        M_Delay(100000);
        LED2_GREEN_OFF();
        LED2_RED_ON();
	}
}

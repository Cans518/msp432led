#include <driverlib.h>

volatile uint32_t Timer32_Cnt = 0;
/* Timer32 ISR */

int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

    //初始化P2.0,并点亮
    P2DIR |= BIT0 | BIT1;
    P2OUT |= BIT0 | BIT1;
    
	//配置timer32
	MAP_Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_PERIODIC_MODE);
	//开启中断
	MAP_Interrupt_enableInterrupt(INT_T32_INT1);
	//开启总中断
	MAP_Interrupt_enableMaster();
	//开启定时器
	MAP_Timer32_setCount(TIMER32_BASE,750000);
    MAP_Timer32_enableInterrupt(TIMER32_BASE);
    MAP_Timer32_startTimer(TIMER32_BASE, true);

    
    return 0;
}


void T32_INT1_IRQHandler(void)
{
	MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
	Timer32_Cnt +=10;
	if(Timer32_Cnt == 100)
	{
		Timer32_Cnt = 0;
		P2OUT ^= BIT0 | BIT1;
	}
    MAP_Timer32_setCount(TIMER32_BASE,750000);
	MAP_Timer32_startTimer(TIMER32_BASE, true);
}
#include <driverlib.h>

#define TIMER_PERIODA0 15000
#define DUTY_CYCLE0 300

int compare_light = 300;
int flag = 1;

//结构体A0设定
const Timer_A_UpModeConfig upConfigA0 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz
        TIMER_PERIODA0,                           // 6000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

//配置PWM波
Timer_A_CompareModeConfig compareConfig_PWM0 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        DUTY_CYCLE0                                // 300 Duty Cycle
};

const uint8_t port_mapping[]=
{
	PMAP_TA0CCR1A,PMAP_TA0CCR1A,
	PMAP_NONE,PMAP_NONE,PMAP_NONE,PMAP_NONE,PMAP_NONE
};


int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

	//映射引脚
	MAP_PMAP_configurePorts((const uint8_t*)port_mapping,PMAP_P2MAP,1,PMAP_DISABLE_RECONFIGURATION);
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);

	//配置定时器A0
	MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfigA0);
	MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
	
	//开启PWM波
	MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM0);

	while (1)
	{
		compare_light += flag * 100;
		if(compare_light >= TIMER_PERIODA0)
			flag = -1;
		if (compare_light <= 0)
			flag = 1;
		compareConfig_PWM0.compareValue = compare_light;
		MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM0);
		for(int i=0;i<10000;i++){};
	}
	

    
    return 0;
}
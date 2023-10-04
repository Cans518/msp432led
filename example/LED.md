## LED灯说明

### LED1

1. LED1接入口为`P1.0`
2. LED1为单色LED灯，颜色为`红色`

### LED2

1. LED2的接入口为 `P2.0~P2.2`
2. LED2为三原色LED：红，绿，蓝
    |接入口|颜色|
    |:--:|:--:|
    |P2.0|红色|
    |P2.1|绿色|
    |P2.2|蓝色|

## 点亮测试

```c
#include <msp432p401r.h>
int main()
{
    //点亮LED1，颜色为单色红色
    P1DIR |= BIT0;//设置P1.0为输出
    P1OUT |= BIT0;//设置P1.0为高电平
    
    //点亮LED2，颜色为单色绿色
    P2DIR |= BIT1;//设置P2.1为输出
    P2OUT |= BIT1;//设置P2.1为高电平

    //关闭LED1，LED2
    P1OUT &= ~BIT0;
    P2OUT &= ~BIT1;

    //点亮LED2，黄色（红色+绿色）
    P2DIR |= BIT0 | BIT1;//设置P2.2为输出
    P2OUT |= BIT0 | BIT1;//设置P2.2为高电平
    return 0;
}
```

## 闪烁测试

### for循环闪烁
```c
#include <msp432p401r.h>
#include <driverlib.h>
void M_Delay(volatile uint32_t i){
    for ( i ; i > 0; i--){}
}
int main()
{
    volatile uint32_t c=0;
    MAP_WDT_A_holdTimer();//挂起看门狗计时器
    while (1)
    {
        P2DIR |= BIT0;
        P2OUT |= BIT0;
        M_Delay(500000);
        P2OUT ^= BIT0;
        P2DIR |= BIT1;
        P2OUT |= BIT1;
        M_Delay(500000);
        P2OUT ^= BIT1;
        P2DIR |= BIT2;
        P2OUT |= BIT2;
        M_Delay(500000);
        P2OUT |= BIT0 | BIT1;
        M_Delay(500000);
        P2OUT ^= BIT0 | BIT1;
        P2OUT |= BIT0 | BIT2;
        M_Delay(500000);
        P2OUT ^= BIT0 | BIT2;
        P2OUT |= BIT1 | BIT2;
        M_Delay(500000);
        P2OUT ^= BIT1 | BIT2;
        P2DIR |= BIT0 | BIT1 | BIT2;
        while (c<=100){
            P2OUT ^= BIT0 | BIT1 | BIT2;
            c++;
            M_Delay(50000);
        }
        P2OUT ^= BIT0 | BIT1 | BIT2;
    }
    
    return 0;
}
```

### 定时器闪烁
```c
#include <msp432p401r.h>
#include <driverlib.h>
#include <msp.h>

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
	MAP_Timer32_setCount(TIMER32_BASE,750000);//2.5s
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
```

### 呼吸灯测试

```c
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
```
#include <driverlib.h>
#include <LED_S.h>
#include <KEY_S.h>
#include <DELAY_S.h>
#include <stdio.h>

char str[100];

// UART串口输出的配置结构体，对应UART模块
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	1,//BRDIV = 6
	10,//UCxBRF = 8
	1,//UCxBRS = 0x20
	//这里在12MHz下配置波特率为115200bps,数据来源为用户手册
	EUSCI_A_UART_NO_PARITY,//无校验
	EUSCI_A_UART_LSB_FIRST,//低位先行
	EUSCI_A_UART_ONE_STOP_BIT,//一个停止位
	EUSCI_A_UART_MODE,//UART模式
	EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION//滤波
};

void UART_Printf(char *str)
{
	while(*str)
		MAP_UART_transmitData(EUSCI_A0_BASE, (uint8_t)*(str++));
}

const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/1 = 12MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void HCSR04Init(void)
{

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);//tring
	MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN6); 
	
    MAP_GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN6);
    
	MAP_GPIO_disableInterrupt(GPIO_PORT_P2, GPIO_PIN4);
    MAP_GPIO_disableInterrupt(GPIO_PORT_P5, GPIO_PIN6);

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);

    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA1_0);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    
    MAP_Interrupt_enableMaster();  

}
void TA1_0_IRQHandler(void)
{
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

float Distance(void)
{
	int i=0;
	float distance=0,sum = 0,count;
	while(i!=5)
	{
	    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);//上拉
		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);//上拉
		delay_us(20);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);//上拉
		while(MAP_GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN6) == 0 );	//
		TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
		while(MAP_GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN6) == 1); //
		count=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
		//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
		//s = vt/2 = t*0.034/2 = t*0.017 = t/5.8	
		distance=(count / 5.8);	
		i++;
		sum = sum + distance;
	}
	return sum / 5;
}

int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

    // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	// 设置DCO频率为12Hz
	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
	
	// 初始化UART模块
	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

	// 开启UART模块
	MAP_UART_enableModule(EUSCI_A0_BASE);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1 | GPIO_PIN4); 

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,GPIO_PIN1 | GPIO_PIN4);  

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4,GPIO_HIGH_TO_LOW_TRANSITION);

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);

    //使能中断
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    LED_Init();
    KEY_Init();
    LED1_RED_ON();
    HCSR04Init();

    while(1){
    }
    return 0;
}

void PORT1_IRQHandler(void)
{

    //中断服务
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,status);	//清除标志位

    if(status & GPIO_PIN1)
    {
        for (uint16_t i  = 0;i <= 500; i++){}
		for (uint16_t i  = 0;; i++){if(KEY1 == KEY_OFF)break;}
        LED2_GREEN_ON();
        float Distance1= Distance();
        sprintf(str,"Distance = %.2fmm\r\n",Distance1); 
        UART_Printf(str);   
        delay_ms(50);
        LED2_GREEN_OFF();
    }
}

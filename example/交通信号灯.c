#include <driverlib.h>
#include <LED_S.h>
#include <KEY_S.h>

int8_t a[10],i = 0;
int16_t TimeNow = 0;

//配置结构体
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	78,//BRDIV = 6
	2,//UCxBRF = 8
	0,//UCxBRS = 0x20
	//这里配置波特率为115200bps,数据来源为用户手册
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

int main(void)
{
	//关闭开门狗
	MAP_WDT_A_holdTimer();

	LED_Init();
	KEY_Init();
	
	// 初始化GPIO
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
	
	// 初始化LED1，并保持默认关闭
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

	// 设置DCO频率为12Hz
	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	
	// 初始化UART模块
	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

	// 开启UART模块
	MAP_UART_enableModule(EUSCI_A0_BASE);
	
	// 打开UART接收中断
	MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
	
	// 使能UART中断
	MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

	MAP_SysTick_enableModule();//使能系统滴答定时器模块
    MAP_SysTick_setPeriod(1200000);//设置装载初值，0.1秒
    MAP_Interrupt_enableSleepOnIsrExit();//使能退出时进入中断睡眠模式
    MAP_SysTick_enableInterrupt();//使能系统滴答定时器中断
	
	// 允许在 ISR 退出时进入睡眠模式
	MAP_Interrupt_enableSleepOnIsrExit();
	
	// 使能总中断
	MAP_Interrupt_enableMaster();
	
	while (1)
		MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒
	
}

void EUSCIA0_IRQHandler(void){
}

void SysTick_Handler(void){

	if (KEY1 == KEY_ON){
		LED_Init();
		LED2_RED_ON();
		UART_Printf("Emergency vehicles passing, no traffic allowed\n");
		while (KEY2 == KEY_OFF);
		LED2_RED_OFF();
		UART_Printf("Passing completed, normal traffic\n");
		TimeNow =0;
	}
	else{
		switch (TimeNow){
			case 10:LED2_GREEN_ON(); UART_Printf("6\n");break;
			case 20: UART_Printf("5\n");break;
			case 30: UART_Printf("4\n");break;
			case 40: UART_Printf("3\n");break;
			case 50: UART_Printf("2\n");break;
			case 60: UART_Printf("1\n");break;
			case 70: UART_Printf("3\n");break;
			case 71 ... 79: 	LED2_GREEN_TOGGLE();break;
			case 80: UART_Printf("2\n");break;
			case 81 ... 89: 	LED2_GREEN_TOGGLE();break;
			case 90: UART_Printf("1\n");break;
			case 91 ... 99: 	LED2_GREEN_TOGGLE();break;
			case 100:LED2_GREEN_OFF(); LED2_YELLOW_ON(); UART_Printf("3\n");break;
			case 110:UART_Printf("2\n");break;
			case 120:UART_Printf("1\n");break;
			case 130:LED2_YELLOW_OFF(); LED2_RED_ON(); UART_Printf("5\n");break;
			case 140:UART_Printf("4\n");break;
			case 150:UART_Printf("3\n");break;
			case 160:UART_Printf("2\n");break;
			case 170:UART_Printf("1\n");break;
			case 180:UART_Printf("3\n");break;
			case 181 ... 189: 	LED2_RED_TOGGLE();break;
			case 190:UART_Printf("2\n");break;
			case 191 ... 199: 	LED2_RED_TOGGLE();break;
			case 200:UART_Printf("1\n");break;
			case 201 ... 209: 	LED2_RED_TOGGLE();break;
			case 210:LED2_RED_OFF(); TimeNow = 9;break;
			default:break;
		}
		TimeNow++;
	}
	

}
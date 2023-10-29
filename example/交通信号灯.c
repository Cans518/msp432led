#include <driverlib.h>
#include <LED_S.h>
#include <KEY_S.h>

int16_t TimeNow = 0; // 计时器，用于观察实际的时间

//配置结构体
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	78,//BRDIV = 78
	2,//UCxBRF = 2
	0,//UCxBRS = 0
	//这里配置波特率为12MHz下的9600bps,数据来源为用户手册
	//使用9600bps，是因为传输数据到arduino时，使用arduino的软串口，软串口的波特率建议不超过57600bps
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
	MAP_WDT_A_holdTimer(); // 关闭开门狗

	LED_Init(); // 初始化LED
	KEY_Init(); // 初始化KEY

    LED1_RED_ON(); // 打开LED1的红色灯，表示通电使用中

	MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4); // 初始化GPIO P2.4作为蜂鸣器
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN4); // 默认为高电平不触发
	
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // 设置DCO频率为12Hz

	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig); // 初始化UART模块

	MAP_UART_enableModule(EUSCI_A0_BASE); // 开启UART模块

	MAP_SysTick_enableModule(); // 使能系统滴答定时器模块

    MAP_SysTick_setPeriod(1200000); // 设置装载初值，0.1秒

    MAP_Interrupt_enableSleepOnIsrExit(); // 使能退出时进入中断睡眠模式

    MAP_SysTick_enableInterrupt(); // 使能系统滴答定时器中断
	
	MAP_Interrupt_enableSleepOnIsrExit(); // 允许在 ISR 退出时进入睡眠模式

	MAP_Interrupt_enableMaster(); // 使能总中断
	
	while (1)
		MAP_PCM_gotoLPM0();//低功耗模式0，中断唤醒
	
}

void SysTick_Handler(void){

	if (KEY1 == KEY_ON){
		KEY1_antishake(); // 按键防抖
		LED_Init();
		LED2_RED_ON();
		UART_Printf("Emergency vehicles passing, no traffic allowed\n");
		MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN4); // 打开蜂鸣器
		while (KEY2 == KEY_OFF);
		MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN4); // 关闭蜂鸣器
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
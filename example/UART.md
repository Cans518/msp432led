#include <driverlib.h>

int8_t a[10],i = 0;

//配置结构体
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	6,//BRDIV = 6
	8,//UCxBRF = 8
	0x20,//UCxBRS = 0x20
	//这里配置波特率为115200bps,数据来源为用户手册
	EUSCI_A_UART_NO_PARITY,//无校验
	EUSCI_A_UART_LSB_FIRST,//低位先行
	EUSCI_A_UART_ONE_STOP_BIT,//一个停止位
	EUSCI_A_UART_MODE,//UART模式
	EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION//滤波
};

void UAPT_Printf(char *str)
{
	while(*str)
		MAP_UART_transmitData(EUSCI_A0_BASE, (uint8_t)*(str++));
}

int main(void)
{
	//关闭开门狗
	MAP_WDT_A_holdTimer();
	
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
	
	// 允许在 ISR 退出时进入睡眠模式
	MAP_Interrupt_enableSleepOnIsrExit();
	
	// 使能总中断
	MAP_Interrupt_enableMaster();
	
	while (1)
		MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒
	
}

void EUSCIA0_IRQHandler(void)
{
	uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
		a[i++] = MAP_UART_receiveData(EUSCI_A0_BASE);
		if (i == 2 && a[0] == (uint8_t)'o' && a[1] == (uint8_t)'n'){
			i = 0;
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
			UAPT_Printf("LED1 is on\n");
		}
		if (i == 3 && a[0] == (uint8_t)'o' && a[1] == (uint8_t)'f' && a[2] == (uint8_t)'f'){
			i = 0;
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
			UAPT_Printf("LED1 is off\n");
		}
	}
	//清除中断标志
	MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);
}
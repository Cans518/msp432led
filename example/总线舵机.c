#include <driverlib.h>
#include <LED_S.h>
#include <DELAY_S.h>
#include <stdio.h>

char str[100]; // str字符串用于储存串口输出的数据

// UART串口输出的配置结构体，对应UART模块
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	6,//BRDIV = 6
	8,//UCxBRF = 8
	0x20,//UCxBRS = 0x20
	//这里在12Hz下配置波特率为115200bps,数据来源为用户手册
	EUSCI_A_UART_NO_PARITY,//无校验
	EUSCI_A_UART_LSB_FIRST,//低位先行
	EUSCI_A_UART_ONE_STOP_BIT,//一个停止位
	EUSCI_A_UART_MODE,//UART模式
	EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION//滤波
};

void UART_Printf(char *str)
{
	while(*str)
		MAP_UART_transmitData(EUSCI_A2_BASE, (uint8_t)*(str++));
}

int main(void)
{
	MAP_WDT_A_holdTimer(); // 关闭看门狗
    
    LED_Init(); // 初始化LED
    
    LED1_RED_ON(); // 打开LED1的红色灯，表示通电使用中

	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // 设置DCO频率为12Hz
	
	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig); // 初始化UART模块
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig); // 初始化UART模块

	MAP_UART_enableModule(EUSCI_A0_BASE); // 开启UART模块
    MAP_UART_enableModule(EUSCI_A2_BASE); // 开启UART模块
	
	MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 打开UART接收中断
	
	MAP_Interrupt_enableInterrupt(INT_EUSCIA0); // 使能UART中断
	
	MAP_Interrupt_enableSleepOnIsrExit(); // 允许在 ISR 退出时进入睡眠模式
	
	MAP_Interrupt_enableMaster(); // 使能总中断
	
	while (1)
		MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒
}

void EUSCIA0_IRQHandler(void)
{
	uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    //串口接收中断
	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
        // 串口接收到数据'1'时
        if( MAP_UART_receiveData(EUSCI_A0_BASE) == '1' ){
            LED2_BLUE_ON(); // 打开蓝灯，提示正在转动
            UART_Printf("#001P1600T1000!");
            delay_s(2);
            LED2_BLUE_OFF(); // 关闭蓝灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
		}
        // 串口接收到数据'2'时
		if( MAP_UART_receiveData(EUSCI_A0_BASE) == '2' ){
            LED2_GREEN_ON(); // 打开绿灯，提示正在转动
            UART_Printf("#001P2000T1000!");
            delay_s(2);
            LED2_GREEN_OFF(); // 关闭绿灯
		}
    }
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status); // 清除中断标志
}
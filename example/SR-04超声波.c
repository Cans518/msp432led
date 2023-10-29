#include <driverlib.h>
#include <LED_S.h>
#include <KEY_S.h>
#include <DELAY_S.h>

char str[100]; // str字符串用于储存串口输出的数据

// UART串口输出的配置结构体，对应UART模块
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	1,//BRDIV = 1
	10,//UCxBRF = 10
	1,//UCxBRS = 1
	//这里在3MHz下配置波特率为115200bps,数据来源为用户手册
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
        TIMER_A_CLOCKSOURCE_SMCLK,              // 设置是时钟源为SMCLK
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
        50000,                                  // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // 禁止TA中断
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // 禁止CCR0触发中断
        TIMER_A_DO_CLEAR                        // 清空值
};


float Distance(void)
{
	int i=0;
	float distance=0,sum = 0,count;
	while(i!=5)
	{
	    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4); // P2.4 Trig 输出低电平,准备触发
		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);// P2.4 Trig 输出高电平
		delay_us(10); // 延时10us, 高电平持续10us激发测距
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);// P2.4 Trig 输出低电平，结束触发
		while(MAP_GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN6) == 0 );	// 当P5.6变为高电平时，测距开始
		    MAP_Timer_A_clearTimer(TIMER_A2_BASE);	 // TA2.1清空计数
		while(MAP_GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN6) == 1); // 当P5.6变为低电平时,测距完成
		    count= MAP_Timer_A_getCounterValue(TIMER_A2_BASE); // 读取TA2.1计数
		//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
		//s = vt/2 = t*0.034/2 = t*0.017 = t/5.8 mm
		distance=(count / 5.8);	// 计算距离
		i++; 
		sum = sum + distance;
	}
	return sum / 5; // 返回平均距离
}

int main()
{
    MAP_WDT_A_holdTimer(); // 关闭看门狗

    LED_Init();
    LED1_RED_ON(); // 打开LED1的红色灯，表示通电使用中

    KEY_Init(); // 初始化按键

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4); // 初始化P2.4为输出，Trig激发引脚
	MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN6);  // 初始化P5.6为输入，echo回波引脚
	
    MAP_GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN6); // 初始化P5.6为输入，电平默认为低

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig); // 配置Timer_A2.1，对应引脚为P5.6

    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE); // 开启Timer_A2.1为向上计数模式

	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出
	
	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig); // 初始化UART模块

	MAP_UART_enableModule(EUSCI_A0_BASE); // 开启UART模块

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,GPIO_PIN1 | GPIO_PIN4);  // 清除按键中断标志位

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4,GPIO_HIGH_TO_LOW_TRANSITION); // 设置中断边沿

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4); // 打开按键中断

    MAP_Interrupt_enableInterrupt(INT_PORT1); // 使能按键中断

    MAP_Interrupt_enableSleepOnIsrExit();// 允许未中断时进入睡眠低功耗模式

    MAP_Interrupt_enableMaster(); // 使能总中断

    while(1)
        MAP_PCM_gotoLPM0();//低功耗模式0，串口中断唤醒

    return 0;
}

void PORT1_IRQHandler(void)
{
    //中断服务
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,status);	//清除标志位

    if(KEY1 == KEY_ON || KEY2 == KEY_ON) 
    {
        KEY1_antishake(); // 按键防抖
        KEY2_antishake(); // 按键防抖
        LED2_GREEN_ON(); // 打开绿色LED，表示正在测距
        float Distance1= Distance(); // 测距
        sprintf(str,"Distance = %.2fmm\r\n",Distance1); // 将测距结果转换为字符串 
        UART_Printf(str); // 打印测距结果
        delay_ms(50); // 延时50ms,等待下一次测距
        LED2_GREEN_OFF(); // 关闭绿色LED，表示测距结束
    }
}

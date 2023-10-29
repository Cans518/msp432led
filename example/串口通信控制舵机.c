#include <driverlib.h>
#include <stdlib.h>
#include <LED_S.h>
#include <DELAY_S.h>

#define TIMER_PERIODA 240000 // 周期 20ms——控制舵机的PWM波 
#define DUTY_CYCLE 18000 // 1.5ms的工作时间，对应sg90舵机的90°位置

uint32_t duty_cycle = DUTY_CYCLE;
uint8_t cout_1 = 0, cout_2 = 0; // 用于计数，判断是否转动到边界
char str[100]; // str字符串用于储存串口输出的数据

/*配置TIMERA为向上计数模式的参数结构体*/           //使用Timer_A2.1，对应引脚5.6
const Timer_A_UpModeConfig upConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // 使用时钟源SMCLK
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 12MHz 调整主频为12MHz后依旧不做分频
        TIMER_PERIODA,                          // 240000 tick period，20ms的PWM周期
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // 禁止计时器中断
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // 禁止CCR0触发中断
        TIMER_A_DO_CLEAR                        // 清空值
};

/*配置TIMERA为比较模式输出PWM方波的参数结构体*/        //使用的Timer_A2.1，对应引脚为P5.6
Timer_A_CompareModeConfig compareConfig_PWM =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // 使用CCR1  TAx1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // 禁止CCR触发中断
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // 变换电平
        DUTY_CYCLE                                  // 设置CCR1的值，对应90°
};

// UART串口输出的配置结构体，对应UART模块
const eUSCI_UART_Config uartConfig =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,//时钟源
	6,//BRDIV = 6
	8,//UCxBRF = 8
	32,//UCxBRS = 32，0x20
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
		MAP_UART_transmitData(EUSCI_A0_BASE, (uint8_t)*(str++));
}

int main(void)
{
	MAP_WDT_A_holdTimer(); // 关闭看门狗

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);  // 初始化P5.6为PWM输出口

	LED2_Init();
	LED2_RED_ON(); // 初始化P2.0,并点亮
    
    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfigA2); // 初始化Timer_A2.1，对应引脚为P5.6
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE); // 开启Timer_A2.1为向上计数模式

    MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);  // 初始化Timer_A2.1输出的PWM波，默认的2.5ms为sg90舵机的90°
	
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2|GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); // 初始化GPIO P1.2 和 P1.3口作为UART接收和输出，根据用户手册，P1.2接收，P1.3输出

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // 设置DCO频率为12Hz
	
	MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig); // 初始化UART模块

	MAP_UART_enableModule(EUSCI_A0_BASE); // 开启UART模块
	
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
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status); // 清除中断标志
    //串口接收中断
	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
        // 串口接收到数据'1'时
        if( MAP_UART_receiveData(EUSCI_A0_BASE) == '1' ){
            LED2_RED_OFF(); // 关闭红灯
            LED2_BLUE_ON(); // 打开蓝灯，提示正在转动
            duty_cycle += 1333; // 顺时针转动10°
            cout_1++; // 使用cout_1作为计数，计算顺时针选择次数
            if(abs(cout_1 - cout_2) >= 10){
                // 判断是否到达边界
                // 当顺时针转动次数和逆时针转动次数差值的绝对值为10时，说明到达边界
                cout_1 = 0;
                cout_2 = 0;
                duty_cycle = DUTY_CYCLE;
                UART_Printf("\nTurned on 180 degree now. Turn back to 90 degree\n"); // 提示转动到边界需要回到初始位置
            }
            else{
                sprintf(str,"\nClockwise select 10 degree\nNow it's %d degree.\n",90 + cout_1*10 - cout_2*10); // 提示顺时针转动10°，并输出转动后的角度
                UART_Printf(str);
            }
            compareConfig_PWM.compareValue = duty_cycle;
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 再次配置Timer_A2.1输出的PWM波
            delay_ms(35); // 等待一段时间，等待转动到位
            LED2_BLUE_OFF(); // 关闭蓝灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
		}
        // 串口接收到数据'2'时
		else if( MAP_UART_receiveData(EUSCI_A0_BASE) == '2' ){
            LED2_GREEN_ON(); // 打开绿灯
            LED2_RED_OFF(); // 关闭红灯，提示正在转动
			duty_cycle -= 1333; // 逆时针转动10°
            cout_2++; // 使用cout_2作为计数，计算逆时针选择次数
            if(abs(cout_1 - cout_2) >= 10){
                // 判断是否到达边界
                // 当逆时针转动次数和顺时针转动次数差值的绝对值为10时，说明到达边界
                cout_1 = 0;
                cout_2 = 0;
                duty_cycle = DUTY_CYCLE;
                UART_Printf("Turned on 0 degree now. Turn back to 90 degree\n"); // 提示转动到边界需要回到初始位置
            }
            else{
                sprintf(str,"Counterclockwise select 10 degree\nNow it's %d degree.\n",90 + cout_1*10 - cout_2*10);
                UART_Printf(str);
            }
            compareConfig_PWM.compareValue = duty_cycle;
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 再次配置Timer_A2.1输出的PWM波
            delay_ms(35); // 等待一段时间，等待转动到位
            LED2_GREEN_OFF(); // 关闭绿灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
		}
        // 串口接收到其它数据时
        else {
            LED2_RED_OFF(); // 关闭红灯
            LED2_WHITE_ON(); // 打开白灯，提示正在转动
            cout_1 = 0;
            cout_2 = 0;
            duty_cycle = DUTY_CYCLE;
            compareConfig_PWM.compareValue = duty_cycle; 
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 再次配置Timer_A2.1输出的PWM波
            UART_Printf("Turn to 90 degree.\n"); // 提示回到初始位置
            delay_ms(35); // 等待一段时间
            LED2_WHITE_OFF(); // 关闭白灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
        }
	}
}
#include <driverlib.h>
#include <LED_S.h>
#include <stdio.h>

#define TIMER_PERIODA 240000 // 周期 20ms——控制舵机的PWM波 
#define DUTY_CYCLE 18000 // 2.5ms的工作时间，对应sg90舵机的90°位置

uint32_t duty_cycle = DUTY_CYCLE;
uint8_t cout_1 = 0, cout_2 = 0; // 用于计数，判断是否转动到边界
char str[100]; // str字符串用于储存串口输出的数据

/* Timer_A2 Up Configuration Parameter */   //使用Timer_A2.1，对应引脚5.6
const Timer_A_UpModeConfig upConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 12MHz 调整主频为12MHz后依旧不做分频
        TIMER_PERIODA,                          // 60000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

/*配置TIMERA为比较模式输出PWM方波的参数结构体*/  //使用的Timer_A2.1，对应引脚为P5.6
Timer_A_CompareModeConfig compareConfig_PWM =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1  TAx1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        DUTY_CYCLE                                  // 3000 Duty Cycle
};

void M_Delay(volatile uint32_t i){
    for ( i ; i > 0; i--){}
}

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
    //串口接收中断
	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
        // 串口接收到数据'1'时
        if( MAP_UART_receiveData(EUSCI_A0_BASE) == '1' ){
            LED2_RED_OFF(); // 关闭红灯
            LED2_BLUE_ON(); // 打开蓝灯，提示正在转动
            duty_cycle += 1333; // 顺时针转动10°
            cout_1++;
            if(cout_2 > 0) cout_2--;
            if(cout_1 >= 10){
                cout_1 = 0;
                cout_2 = 0;
                duty_cycle = DUTY_CYCLE;
                UART_Printf("\nTruned on 180 degree now. Trun back to 90 degree\n"); // 提示转动到边界需要回到初始位置
            }
            else{
                sprintf(str,"\nClockwise select 10 degree\nNow it's %d degree.\n",90 + cout_1*10 - cout_2*10);
                UART_Printf(str);
            }
            compareConfig_PWM.compareValue = duty_cycle;
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 再次配置Timer_A2.1输出的PWM波
            M_Delay(400000); // 等待一段时间，等待转动到位
            LED2_BLUE_OFF(); // 关闭蓝灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
		}
        // 串口接收到数据'2'时
		else if( MAP_UART_receiveData(EUSCI_A0_BASE) == '2' ){
            LED2_GREEN_ON(); // 打开绿灯
            LED2_RED_OFF(); // 关闭红灯，提示正在转动
			duty_cycle -= 1333; // 逆时针转动10°
            cout_2++;
            if(cout_1 > 0) cout_1--;
            if(cout_2 >= 10){
                cout_1 = 0;
                cout_2 = 0;
                duty_cycle = DUTY_CYCLE;
                UART_Printf("Truned on 0 degree now. Trun back to 90 degree\n"); // 提示转动到边界需要回到初始位置
            }
            else{
                sprintf(str,"Counterclockwise select 10 degree\nNow it's %d degree.\n",90 + cout_1*10 - cout_2*10);
                UART_Printf(str);
            }
            compareConfig_PWM.compareValue = duty_cycle;
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM); // 再次配置Timer_A2.1输出的PWM波
            M_Delay(400000); // 等待一段时间，等待转动到位
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
            MAP_Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
            UART_Printf("Trun to 90 degree.\n"); // 提示回到初始位置
            M_Delay(400000); // 等待一段时间
            LED2_WHITE_OFF(); // 关闭白灯
            LED2_RED_ON(); // 打开红灯，提示转动完成，等待下一次命令
        }
	}
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status); // 清除中断标志
}
#include <driverlib.h>
#include <KEY_S.h>
#include <LED_S.h>
#include <DELAY_S.h>
#include <USART_S.h>

#define TRIG_PIN GPIO_PORT_P5, GPIO_PIN6 // 配置P5.6为触发TRIG引脚
#define ECHO_PIN GPIO_PORT_P2, GPIO_PIN4 // 配置P2.4为回波Echo引脚

volatile uint32_t start_time = 0; // 初始化回波开始时间
volatile uint32_t end_time = 0; // 初始化回波结束时间

void main(void) {
    MAP_WDT_A_holdTimer(); // 停用看门狗定时器

    KEY1_Init(); // 初始化KEY1
    LED1_Init(); // 初始化LED1

    UART_Init(); // 初始化串口

    // 初始化MSP432系统时钟
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // 设置DCO频率为12 MHz
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // 配置MCLK为DCOCLK（12 MHz）
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // 配置SMCLK为DCOCLK（12 MHz）
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // 配置HSMCLK为DCOCLK（12 MHz）


    // 配置Trig引脚（发送触发脉冲）
    GPIO_setAsOutputPin(TRIG_PIN); // 将TRIG引脚设置为输出
    GPIO_setOutputLowOnPin(TRIG_PIN); // 将TRIG引脚设置为低电平，等待拉高输出

    // 配置Echo引脚（接收回波信号）
    GPIO_setAsInputPinWithPullUpResistor(ECHO_PIN); // 将Echo引脚设置为输入
    GPIO_selectInterruptEdge(ECHO_PIN, GPIO_HIGH_TO_LOW_TRANSITION); // 设置Echo引脚为上升沿触发
    GPIO_clearInterruptFlag(ECHO_PIN); // 清除Echo引脚中断标志
    GPIO_enableInterrupt(ECHO_PIN); // 使能Echo引脚中断

    // 启用Systick定时器
    SysTick->LOAD = 0xFFFFFF; // 设置重装载寄存器
    SysTick->VAL = 0; // 重置计数寄存器
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // 使能Systick

    __enable_irq(); // 启用全局中断

    while (1) {
        //当按键按下
        if (KEY1 == KEY_ON) {

            // 按键防抖
            KEY1_antishake();

            // 发送触发脉冲
            GPIO_setOutputHighOnPin(TRIG_PIN); // 发送触发脉冲(10微秒)
            delay_us(10); // 等待10微秒
            GPIO_setOutputLowOnPin(TRIG_PIN); // 发送完成后将TRIG引脚设置为低电平，等待下一次触发

            LED1_RED_ON(); // 点亮红色LED，指示开始测距离

            while (end_time == 0); // 等待回波信号到达Echo引脚

            LED1_RED_OFF(); // 关闭红色LED，指示测量结束

            // 计算距离
             uint32_t distance = end_time - start_time;
            distance = (distance * 340) / 20000; // 声音传播速度为340 m/s

            // 将距离打印到串口终端
            printf("Distance: %d cm\n", distance);

            end_time = 0; // 重置end_time
        }
    }
}

// Echo引脚的中断处理程序
void PORT2_IRQHandler(void) {
    if (GPIO_getInterruptStatus(ECHO_PIN) & ECHO_PIN) {// 中断发生
        if (GPIO_getInputPinValue(ECHO_PIN)) // 上升沿记录开始时间
            start_time = SysTick->VAL; // 记录开始时间 ，发射波的时间
        else // 下降沿记录结束时间
            end_time = SysTick->VAL; // 记录结束时间，收到回波时间

        GPIO_clearInterruptFlag(ECHO_PIN); // 清除中断标志
    }
}

#include "USART_S.h"

void UART_Init(void) {
    // 配置UART引脚
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // 配置UART参数
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST; // 将UART模块复位

    // 配置波特率，时钟源选择为SMCLK
    EUSCI_A0->BRW = 78; // 12 MHz / 9600 = 1250，BRW设置为1250-1
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRS_OFS) | EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK; // 清除复位位并配置时钟源

    // 启用UART模块并启用接收中断
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;
}

// UART接收中断处理程序
void EUSCIA0_IRQHandler(void) {
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) {
        // 从UART接收缓冲区读取数据
        uint8_t receivedData = EUSCI_A0->RXBUF;
        // 在这里执行接收到数据后的操作
    }
}

// 定义__putchar函数以适配printf
int __putchar(int character) {
    // 等待UART发送缓冲区准备就绪
    while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

    // 发送字符到UART
    EUSCI_A0->TXBUF = (uint8_t)character;

    return character;
}
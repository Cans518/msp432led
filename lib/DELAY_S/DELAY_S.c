#include "DELAY.h"
static uint8_t fac_us = 0;	//us延时倍乘数

//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
	fac_us = CS_getMCLK() / 1000000;		 //系统时钟
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //内部时钟源
}
void delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->LOAD = nus * fac_us;			  //时间加载
	SysTick->VAL = 0x00;					  //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); //等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		//关闭计数器
	SysTick->VAL = 0X00;							//清空计数器
}
//延时nms
void delay_ms(uint32_t nms)
{
	while(nms)
	{
		delay_us(1000);
		--nms;
	}
}

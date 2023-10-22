#include "DELAY_S.h"

#define clock_source 12    //时钟频率填写到这里（单位MHz）    

void delay_us(uint32_t us){ //uS微秒级延时程序
	MAP_SysTick_disableInterrupt();   //禁用时钟中断
	MAP_SysTick_setPeriod(us*clock_source);	//加载计数值
	SysTick->VAL=0x00;           //清空计数器
	MAP_SysTick_enableModule();   //使能系统时钟
	while(MAP_SysTick_getValue()&(us*clock_source));
	SysTick->VAL=0x00;        //清空计数器
	MAP_SysTick_disableModule();//关闭计数
}

void delay_ms(uint16_t ms){ //mS毫秒级延时程序 		  	  
	while( ms-- != 0){
		delay_us(1000);	
	}
}

void delay_s(uint16_t s){ //S秒级延时程序 		  	  
	while( s-- != 0){
		delay_ms(1000);	
	}
} 

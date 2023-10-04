#include <driverlib.h>
#include <KEY_S.h>
#include <LED_S.h>

int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

	KEY_Init();

    //初始化P2.0,并点亮
	LED2_Init();
	LED2_YELLOW_ON();
	LED2_BLUE_OFF();

	//当key1或者key2按下时，开关LED
	while(1){
		if(KEY1 == KEY_ON){
			for (uint16_t i  = 0;i <= 50000; i++){}
			for (uint16_t i  = 0;; i++){if(KEY1 == KEY_OFF)break;}
			LED2_YELLOW_TOGGLE();
		}
		if(KEY2 == KEY_ON){
			for (uint16_t i  = 0;i <= 50000; i++){}
			for (uint16_t i  = 0;; i++){if(KEY2 == KEY_OFF)break;}
			LED2_BLUE_TOGGLE();
		}
	}


    return 0;
}
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

	//当key1或者key2按下时，开关LED
	while(1){
		for (uint16_t i  = 0; i < 3000; i++){}
		if(KEY1 == KEY_ON || KEY2 == KEY_ON){
			for (uint16_t i  = 0; i < 3000; i++){}
			LED2_YELLOW_TOGGLE();
		}
	}


    return 0;
}
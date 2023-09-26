```c
#include <driverlib.h>
#include <KEY_S.h>
#include <LED_S.h>

int main()
{
    //关闭看门狗
    MAP_WDT_A_holdTimer();

	//初始化P1.0和P1.4为输入
	MAP_GPIO_setAsInputPin(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);

    //初始化P2.0,并点亮
	LED2_Init();
	LED2_YELLOW_ON();

	//将P1.0和P1.4输入上拉
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1|GPIO_PIN4);

	//当key1或者key2按下时，开关LED
	while(1){
		if(KEY1 == KEY_ON || KEY2 == KEY_ON){
			for (uint16_t i  = 0; i < 3000; i++){}
			LED2_YELLOW_TOGGLE();
		}
	}


    return 0;
}
```

```c
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
			for (uint16_t i  = 0;i <= 500; i++){}
			for (uint16_t i  = 0;; i++){if(KEY1 == KEY_OFF)break;}
			LED2_YELLOW_TOGGLE();
		}
		if(KEY2 == KEY_ON){
			for (uint16_t i  = 0;i <= 500; i++){}
			for (uint16_t i  = 0;; i++){if(KEY2 == KEY_OFF)break;}
			LED2_BLUE_TOGGLE();
		}
	}


    return 0;
}
```


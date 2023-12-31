#include<LED_S.h>

void LED_Init(void){LED1_Init();LED2_Init();}
void LED1_Init(void){P1DIR |= BIT0; LED1_RED=0;}
void LED2_Init(void){P2DIR |= BIT0 | BIT1 | BIT2;LED2_BLUE=0;LED2_RED=0;LED2_GREEN=0;}


void LED1_RED_ON(void){LED1_RED = 1;}
void LED1_RED_OFF(void){LED1_RED = 0;}
void LED1_RED_TOGGLE(void){LED1_RED ^= 1;}

void LED2_RED_ON(void){LED2_RED = 1;}
void LED2_RED_OFF(void){LED2_RED = 0;}
void LED2_RED_TOGGLE(void){LED2_RED ^= 1;}

void LED2_GREEN_ON(void){LED2_GREEN = 1;}
void LED2_GREEN_OFF(void){LED2_GREEN = 0;}
void LED2_GREEN_TOGGLE(void){LED2_GREEN ^= 1;}

void LED2_BLUE_ON(void){LED2_BLUE = 1;}
void LED2_BLUE_OFF(void){LED2_BLUE = 0;}
void LED2_BLUE_TOGGLE(void){LED2_BLUE ^= 1;}

void LED2_YELLOW_ON(void){LED2_RED = 1;LED2_GREEN = 1;}
void LED2_YELLOW_OFF(void){LED2_RED = 0;LED2_GREEN = 0;}
void LED2_YELLOW_TOGGLE(void){LED2_RED ^= 1;LED2_GREEN ^= 1;}

void LED2_CYAN_ON(void){LED2_GREEN = 1;LED2_BLUE = 1;}
void LED2_CYAN_OFF(void){LED2_GREEN = 0;LED2_BLUE = 0;}
void LED2_CYAN_TOGGLE(void){LED2_GREEN ^= 1;LED2_BLUE ^= 1;}

void LED2_PURPLE_ON(void){LED2_RED = 1;LED2_BLUE = 1;}
void LED2_PURPLE_OFF(void){LED2_RED = 0;LED2_BLUE = 0;}
void LED2_PURPLE_TOGGLE(void){LED2_RED ^= 1;LED2_BLUE ^= 1;}

void LED2_WHITE_ON(void){LED2_RED = 1;LED2_GREEN = 1;LED2_BLUE = 1;}
void LED2_WHITE_OFF(void){LED2_RED = 0;LED2_GREEN = 0;LED2_BLUE = 0;}
void LED2_WHITE_TOGGLE(void){LED2_RED ^= 1;LED2_GREEN ^= 1;LED2_BLUE ^= 1;}
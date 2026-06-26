#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"


int test = 0;

int main(void)
{
	OLED_Init();						// OLED初始化
	LED_Init();							// LED初始化
	Key_Init();							// 按键初始化
	Timer_Init();						// 定时器初始化（ISR自动驱动Key_Tick）

	while (1)
	{
		Key_ID_t key;					// 当前按下的按键ID

		key = Key_AnyPress();			// 有键按下返回其ID，无事件返回0
		if (key) test = key;			// key值1/2/3刚好对应当前test值

		if (Key_AnyLongPress())	LED_Turn();

		OLED_ShowNum(0, 0, test, 1, OLED_8X16);
		OLED_Update();
	}
}

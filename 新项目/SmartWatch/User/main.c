#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"

/**************************** 按键事件处理（覆盖 Key.c 中的 __weak 默认实现） ****************************/
void Key_EventHandler(key_t *key, key_event_t event)
{
	/* 先判断哪个键 */
	if (key == &key1) {
		switch (event) {
		case KEY_PRESS: 
				OLED_Clear();
				OLED_ShowString(0,0,"Key1_PRESS",OLED_6X8);
				OLED_Update();
				break;
		case KEY_RELEASE:  /* KEY1 短按释放 */ break;
		case KEY_LONGPRESS:
				OLED_Clear();
				OLED_ShowString(0,0,"Key1_longPRESS",OLED_6X8);
				OLED_Update();
				break;
		}
	} else if (key == &key2) {
		switch (event) {
		case KEY_PRESS:   
				OLED_Clear();
				OLED_ShowString(0,8,"Key2_PRESS",OLED_6X8);
				OLED_Update();
				break;
		case KEY_RELEASE:  /* KEY2 短按释放 */ break;
		case KEY_LONGPRESS:
				OLED_Clear();
				OLED_ShowString(0,8,"Key2_longPRESS",OLED_6X8);
				OLED_Update();
				break;
		}
	} else if (key == &key3) {
		switch (event) {
		case KEY_PRESS:    
				OLED_Clear();
				OLED_ShowString(0,16,"Key3_PRESS",OLED_6X8);
				OLED_Update();
				break;
		case KEY_RELEASE:  /* KEY3 短按释放 */ break;
		case KEY_LONGPRESS:
				OLED_Clear();	
				OLED_ShowString(0,16,"Key3_longPRESS",OLED_6X8);
				OLED_Update();
				break;
		}
	}
}

int main(void)
{
	OLED_Init();
	Timer_Init();
	Key_Init();


	while (1)
	{
		Key_Scan();
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

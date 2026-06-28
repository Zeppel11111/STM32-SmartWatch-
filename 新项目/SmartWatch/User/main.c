#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "UI_ct.h"
#include "Key.h"
#include "MyRTC.h"

/**************************** 按键事件 → UI 分发 ****************************/
void Key_EventHandler(key_t *key, key_event_t event)
{
	UI_KeyDispatch(key, event);		// 一键路由到当前页面
}

int main(void)
{
	OLED_Init();
	Timer_Init();
	Key_Init();
	MyRTC_Init();
	while (1)
	{
		UI_Run();		// 绘制当前页 + 刷新屏幕
	}
}

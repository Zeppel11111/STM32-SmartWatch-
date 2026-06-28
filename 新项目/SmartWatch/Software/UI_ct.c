#include "UI_ct.h"
#include "UI_home.h"
#include "UI_setting.h"
#include "OLED.h"

/* 当前页面 */
static UI_state current_page = home_page;

/**************************** 页面切换 ****************************/
void UI_SwitchPage(UI_state target)
{
	current_page = target;
	OLED_Clear();		// 切页时清屏
}

/**************************** 按键事件分发到当前页面 ****************************/
void UI_KeyDispatch(key_t *key, key_event_t event)
{
	switch (current_page)
	{
		case home_page:
			UI_home_OnKey(key, event);
			break;

		case app_page:
			/* TODO: UI_app_OnKey(key, event); */
			break;

		case setting_page:
			UI_setting_OnKey(key, event);
			break;
	}
}

/**************************** 主循环：绘制当前页 + 刷新屏幕 ****************************/
void UI_Run(void)
{
	switch (current_page)
	{
		case home_page:
			UI_home_Draw();
			break;

		case app_page:
			/* TODO: UI_app_Draw(); */
			break;

		case setting_page:
			UI_setting_Draw();
			break;
	}
	OLED_Update();		// 统一刷新，各页面只写显存
}

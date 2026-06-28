#include "UI_home.h"
#include "MyRTC.h"
#include "OLED.h"
#include "UI_ct.h"

/* 底部控件选中索引：0="菜单", 1="设置" */
static uint8_t selected = 0;

/* 控件坐标 */
#define ITEM1_X     0
#define ITEM2_X     96
#define ITEM_Y      48
#define ITEM_W      32      // 2个中文字符 × 16px（8X16 模式下中文为 16x16）
#define ITEM_H      16

/**************************** 绘制主页 ****************************/
void UI_home_Draw(void)
{
	MyRTC_ReadTime();
	OLED_Clear();

	/* 日期 */
	OLED_Printf(0, 0, OLED_6X8, "%4d-%2d-%2d",
	            MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]);

	/* 时间 */
	OLED_Printf(32, 16, OLED_8X16, "%2d:%2d:%2d",
	            MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);

	/* 底部控件 */
	OLED_ShowString(ITEM1_X, ITEM_Y, "菜单", OLED_8X16);
	OLED_ShowString(ITEM2_X, ITEM_Y, "设置", OLED_8X16);

	/* 选中项反显 */
	if (selected == 0)
		OLED_ReverseArea(ITEM1_X, ITEM_Y, ITEM_W, ITEM_H);
	else
		OLED_ReverseArea(ITEM2_X, ITEM_Y, ITEM_W, ITEM_H);
}

/**************************** 主页按键处理 ****************************/
void UI_home_OnKey(key_t *key, key_event_t event)
{
	if (event != KEY_PRESS) return;		// 仅响应按下

	if (key == &key3)
	{
		/* KEY3(PA6) 左移光标，单向循环 */
		selected = (selected > 0) ? selected - 1 : 1;
	}
	else if (key == &key1)
	{
		/* KEY1(PA2) 右移光标，单向循环 */
		selected = (selected < 1) ? selected + 1 : 0;
	}
	else if (key == &key2)
	{
		/* KEY2(PA4) 确认进入 */
		if (selected == 0)
			UI_SwitchPage(app_page);
		else
			UI_SwitchPage(setting_page);
	}
}

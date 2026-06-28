#include "UI_setting.h"
#include "OLED.h"
#include "UI_ct.h"

/* 设置选项表 */
typedef struct {
	char   *name;			// 选项名（UTF-8）
	uint8_t id;				// 选项ID（预留）
} setting_item_t;

static const setting_item_t setting_items[] = {
	{"亮度",   0},
	{"音量",   1},
	{"蓝牙",   2},
	{"关于",   3},
	{"重置",   4},
	{"时间",   5},
	{"显示",   6},
	{"返回",   7},
};
#define ITEM_COUNT   8
#define ROW_H        16		// 行高（8X16 模式下中文为 16px）
#define ROWS_PER_PG  4		// 每屏行数（64 / 16 = 4）

static uint8_t cursor = 0;	// 当前选中表项索引
static uint8_t scroll = 0;	// 屏幕顶部对应表项的索引

/**************************** 绘制设置页 ****************************/
void UI_setting_Draw(void)
{
	uint8_t i, idx;

	OLED_Clear();

	/* 只绘制当前可见的 4 行 */
	for (i = 0; i < ROWS_PER_PG; i++)
	{
		idx = scroll + i;
		if (idx >= ITEM_COUNT) break;

		OLED_ShowString(8, i * ROW_H, setting_items[idx].name, OLED_8X16);

		/* 选中行反显 */
		if (idx == cursor)
			OLED_ReverseArea(0, i * ROW_H, 128, ROW_H);
	}

	/* 右侧滚动条（总项数超过一屏时显示） */
	if (ITEM_COUNT > ROWS_PER_PG)
	{
		uint8_t thumb_y = (uint8_t)((uint32_t)scroll * (64 - 8) / (ITEM_COUNT - 1));
		OLED_DrawRectangle(126, thumb_y, 2, 8, OLED_FILLED);
	}
}

/**************************** 设置页按键处理 ****************************/
void UI_setting_OnKey(key_t *key, key_event_t event)
{
	if (event != KEY_PRESS) return;

	if (key == &key3)
	{
		/* KEY3(PA6) 上移 */
		if (cursor > 0)
			cursor--;
		else
			cursor = ITEM_COUNT - 1;		// 到顶回绕到底
	}
	else if (key == &key1)
	{
		/* KEY1(PA2) 下移 */
		if (cursor < ITEM_COUNT - 1)
			cursor++;
		else
			cursor = 0;						// 到底回绕到顶
	}
	else if (key == &key2)
	{
		switch (cursor)
		{
			case (7):UI_SwitchPage(home_page);
			break;
		
		
		}
	}

	/* 滚动跟随：确保 cursor 在可见范围内 */
	if (cursor < scroll)
		scroll = cursor;
	else if (cursor > scroll + ROWS_PER_PG - 1)
		scroll = cursor - ROWS_PER_PG + 1;
}

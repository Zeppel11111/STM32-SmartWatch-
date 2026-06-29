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
static uint8_t prev_cursor = 0;	// 上一帧的 cursor，用于检测回绕

/* 弹簧动画参数 —————— 可调：K 越小越慢，D 越大越弹 */
static float   anim_y   = 0;		// 高亮条当前 Y（屏幕坐标，带小数）
static float   anim_vel = 0;		// 速度
#define SPRING_K  0.3f				// 刚度（0=死，1=瞬移）
#define SPRING_D  0.7f				// 阻尼（越接近1弹得越久，<0.7 不弹）

/**************************** 初始化设置页状态 ****************************/
void UI_setting_Init(void)
{
	cursor = 0;
	scroll = 0;
	prev_cursor = 0;
	anim_y   = 0;
	anim_vel = 0;
}

/**************************** 绘制设置页 ****************************/
void UI_setting_Draw(void)
{
	uint8_t i, idx;
	float   target_y, diff, vel_abs;
	uint8_t wrap_dist;

	OLED_Clear();

	/* 1. 绘制可见行文字（不画高亮） */
	for (i = 0; i < ROWS_PER_PG; i++)
	{
		idx = scroll + i;
		if (idx >= ITEM_COUNT) break;
		OLED_ShowString(8, i * ROW_H, setting_items[idx].name, OLED_8X16);
	}

	/* 2. 弹簧动画：计算高亮条目标位置 */
	target_y = (float)((int)(cursor - scroll)) * ROW_H;

	/* 检测回绕（cursor 从 0→7 或 7→0），直接瞬移不平滑 */
	wrap_dist = (cursor > prev_cursor) ? (cursor - prev_cursor)
	                                   : (prev_cursor - cursor);
	if (wrap_dist > 1)
	{
		anim_y   = target_y;
		anim_vel = 0;
	}
	else
	{
		/* 弹簧物理：加速度 = (目标 - 当前位置) × 刚度，每帧加阻尼 */
		anim_vel += (target_y - anim_y) * SPRING_K;
		anim_vel *= SPRING_D;
		anim_y   += anim_vel;

		/* 够近就吸住，避免永不停歇的微颤 */
		diff = target_y - anim_y;
		if (diff < 0) diff = -diff;
		vel_abs = anim_vel;
		if (vel_abs < 0) vel_abs = -vel_abs;
		if (diff < 0.3f && vel_abs < 0.3f)
		{
			anim_y   = target_y;
			anim_vel = 0;
		}
	}
	prev_cursor = cursor;

	/* 3. 画高亮条（用动画后的 Y） */
	OLED_ReverseArea(0, (int)anim_y, 128, ROW_H);

	/* 4. 右侧滚动条（总项数超过一屏时显示） */
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
		/* KEY2(PA4) 确认：进入子功能 或 返回主页 */
		switch (cursor)
		{
			case 0: UI_SetSubPage(SUB_BRIGHTNESS); UI_PushPage(sub_page); break;
			case 1: UI_SetSubPage(SUB_VOLUME);     UI_PushPage(sub_page); break;
			case 2: UI_SetSubPage(SUB_BLUETOOTH);  UI_PushPage(sub_page); break;
			case 3: UI_SetSubPage(SUB_ABOUT);      UI_PushPage(sub_page); break;
			case 4: UI_SetSubPage(SUB_RESET);      UI_PushPage(sub_page); break;
			case 5: UI_SetSubPage(SUB_TIME);       UI_PushPage(sub_page); break;
			case 6: UI_SetSubPage(SUB_DISPLAY);    UI_PushPage(sub_page); break;
			case 7: UI_PopPage(); break;           /* "返回" → 回到主页 */
		}
	}

	/* 滚动跟随：确保 cursor 在可见范围内 */
	if (cursor < scroll)
		scroll = cursor;
	else if (cursor > scroll + ROWS_PER_PG - 1)
		scroll = cursor - ROWS_PER_PG + 1;
}

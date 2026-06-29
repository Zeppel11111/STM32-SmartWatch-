#include "UI_subpage.h"
#include "UI_ct.h"
#include "OLED.h"

/* ── 亮度调节 ── */
static uint8_t  brightness = 0xCF;	// 默认值，范围 0x00~0xFF
static int8_t   hold_dir   = 0;		// 长按连续方向：1=变亮, -1=变暗, 0=停止
static uint8_t  hold_tick  = 0;		// 连续调速计数器

/* ── 时间调整数据 ── */
static uint16_t RTC_Setting[] = {2026, 5, 20, 30, 30, 30};

/* 可编辑字段：{ X, Y, W, *值, min, max } */
typedef struct {
	uint8_t  x, y, w;
	uint16_t *val;
	uint16_t min, max;
} time_field_t;

/* 格式: 2026-05-20 / 12:30（不显示秒） */
static time_field_t fields[] = {
	{0,  24, 32, &RTC_Setting[0], 2000, 2099},	// 0: 年
	{40, 24, 16, &RTC_Setting[1],    1,   12},	// 1: 月
	{64, 24, 16, &RTC_Setting[2],    1,   31},	// 2: 日
	{0,  40, 16, &RTC_Setting[3],    0,   23},	// 3: 时
	{24, 40, 16, &RTC_Setting[4],    0,   59},	// 4: 分
};
#define FIELD_COUNT  5
#define RET_X     96
#define RET_Y     48
#define RET_W     32

static uint8_t field_cursor = 0;	// 导航光标：0~4=字段, 5="返回"
static uint8_t edit_mode   = 0;		// 0=导航态, 1=编辑态

/* ── 子功能页标题表 ── */
static const char *sub_titles[] = {
	"亮度", "音量", "蓝牙", "关于", "重置", "时间", "显示",
};

#define TITLE_Y   0
#define BODY_Y    24

/**************************** 初始化子页状态 ****************************/
void UI_subpage_Init(void)
{
	SubPage id = UI_GetSubPage();
	switch (id)
	{
		case SUB_TIME:
			field_cursor = 0;
			edit_mode    = 0;
			RTC_Setting[5] = 0;
			break;
		default: break;
	}
}

/**************************** 绘制子功能页 ****************************/
void UI_subpage_Draw(void)
{
	SubPage id = UI_GetSubPage();

	OLED_Clear();

	/* 标题行 */
	OLED_ShowString(0, TITLE_Y, ">", OLED_8X16);
	OLED_ShowString(16, TITLE_Y, (char *)sub_titles[id], OLED_8X16);
	OLED_DrawLine(0, TITLE_Y + 15, 127, TITLE_Y + 15);

	switch (id)
	{
		case SUB_BRIGHTNESS:
			/* 长按连续调节 */
			if (hold_dir != 0)
			{
				/* 检测松手：对应按键弹起则停止 */
				uint8_t still_held;
				if (hold_dir > 0)
					still_held = (GPIO_ReadInputDataBit(KEY_PORT, KEY1_PIN) == 0);
				else
					still_held = (GPIO_ReadInputDataBit(KEY_PORT, KEY3_PIN) == 0);
				if (!still_held)
				{
					hold_dir  = 0;
					hold_tick = 0;
				}
				else
				{
					hold_tick++;
					if (hold_tick >= 4)		// 每 4 帧调一步（~40ms/步 @100fps）
					{
						hold_tick = 0;
						if (hold_dir > 0 && brightness < 239)
							brightness += 16;
						else if (hold_dir < 0 && brightness > 16)
							brightness -= 16;
						OLED_SetContrast(brightness);
					}
				}
			}
			/* 亮度百分比 + 进度条 */
			{
				uint8_t pct = (uint8_t)((uint16_t)brightness * 100 / 255);
				uint8_t bar_w = (uint8_t)((uint16_t)brightness * 112 / 255);
				OLED_Printf(0, 24, OLED_8X16, "%d%%", pct);
				OLED_DrawRectangle(8, 44, 112, 8, OLED_UNFILLED);
				if (bar_w > 0)
					OLED_DrawRectangle(8, 44, bar_w, 8, OLED_FILLED);
			}
			break;
		case SUB_VOLUME:
			OLED_ShowString(0, BODY_Y, "音量调节", OLED_8X16);
			break;
		case SUB_BLUETOOTH:
			OLED_ShowString(0, BODY_Y, "蓝牙设置", OLED_8X16);
			break;
		case SUB_ABOUT:
			OLED_ShowString(0, BODY_Y, "关于设备", OLED_8X16);
			break;
		case SUB_RESET:
			OLED_ShowString(0, BODY_Y, "重置选项", OLED_8X16);
			break;

		case SUB_TIME:
			/* 日期行: 2026-05-20 */
			OLED_Printf(0,  24, OLED_8X16, "%04d", RTC_Setting[0]);
			OLED_ShowString(32, 24, "-", OLED_8X16);
			OLED_Printf(40, 24, OLED_8X16, "%02d", RTC_Setting[1]);
			OLED_ShowString(56, 24, "-", OLED_8X16);
			OLED_Printf(64, 24, OLED_8X16, "%02d", RTC_Setting[2]);

			/* 时间行: 12:30（不显示秒） */
			OLED_Printf(0,  40, OLED_8X16, "%02d", RTC_Setting[3]);
			OLED_ShowString(16, 40, ":", OLED_8X16);
			OLED_Printf(24, 40, OLED_8X16, "%02d", RTC_Setting[4]);

			/* 返回（右下角） */
			OLED_ShowString(RET_X, RET_Y, "返回", OLED_8X16);

			/* 光标高亮 */
			if (field_cursor < FIELD_COUNT)
			{
				time_field_t *f = &fields[field_cursor];
				OLED_ReverseArea(f->x, f->y, f->w, 16);
			}
			else
			{
				OLED_ReverseArea(RET_X, RET_Y, RET_W, 16);
			}
			break;

		case SUB_DISPLAY:
			OLED_ShowString(0, BODY_Y, "显示设置", OLED_8X16);
			break;
	}
}

/**************************** 子功能页按键处理 ****************************/
void UI_subpage_OnKey(key_t *key, key_event_t event)
{
	SubPage id = UI_GetSubPage();

	/* ── 时间设置页 ── */
	if (id == SUB_TIME)
	{
		/* KEY2 长按（导航态）：保存并返回 */
		if (key == &key2 && event == KEY_LONGPRESS && !edit_mode)
		{
			/* TODO: MyRTC_Write(RTC_Setting); */
			UI_PopPage();
			return;
		}

		if (event != KEY_PRESS) return;

		if (edit_mode)
		{
			/* ═══ 编辑态 ═══ */
			if (field_cursor < FIELD_COUNT)
			{
				time_field_t *f = &fields[field_cursor];

				if (key == &key2)
				{
					edit_mode = 0;
				}
				else if (key == &key3)
				{
					if (*f->val > f->min)
						(*f->val)--;
					else
						*f->val = f->max;
				}
				else if (key == &key1)
				{
					if (*f->val < f->max)
						(*f->val)++;
					else
						*f->val = f->min;
				}
			}
			else
			{
				if (key == &key2)
					edit_mode = 0;
			}
		}
		else
		{
			/* ═══ 导航态 ═══ */
			if (key == &key2)
			{
				if (field_cursor < FIELD_COUNT)
					edit_mode = 1;
				else
				{
					/* 选"返回"：退出 */
					/* TODO: MyRTC_Write(RTC_Setting); */
					UI_PopPage();
				}
			}
			else if (key == &key3)
			{
				if (field_cursor > 0)
					field_cursor--;
				else
					field_cursor = FIELD_COUNT;
			}
			else if (key == &key1)
			{
				if (field_cursor < FIELD_COUNT)
					field_cursor++;
				else
					field_cursor = 0;
			}
		}
		return;
	}

	/* ── 亮度调节页 ── */
	if (id == SUB_BRIGHTNESS)
	{
		/* 任意按键按下 → 停止连续调节 */
		if (event == KEY_PRESS && hold_dir != 0)
		{
			hold_dir  = 0;
			hold_tick = 0;
		}

		if (event == KEY_LONGPRESS)
		{
			/* 长按启动连续调节 */
			if (key == &key1)      hold_dir =  1;
			else if (key == &key3) hold_dir = -1;
			hold_tick = 0;
			return;
		}

		if (event != KEY_PRESS) return;

		if (key == &key3)
		{
			/* KEY3(PA6) 短按变暗一步 */
			if (brightness > 16)
				brightness -= 16;
			else
				brightness = 0;
			OLED_SetContrast(brightness);
		}
		else if (key == &key1)
		{
			/* KEY1(PA2) 短按变亮一步 */
			if (brightness < 239)
				brightness += 16;
			else
				brightness = 0xFF;
			OLED_SetContrast(brightness);
		}
		else if (key == &key2)
		{
			/* KEY2(PA4) 返回 */
			UI_PopPage();
		}
		return;
	}

	/* ── 其他子页 ── */
	if (event != KEY_PRESS) return;

	if (key == &key3)
	{
		UI_PopPage();
		return;
	}

	switch (id)
	{
		case SUB_VOLUME:     /* TODO */ break;
		case SUB_BLUETOOTH:  /* TODO */ break;
		case SUB_ABOUT:      break;
		case SUB_RESET:      /* TODO */ break;
		case SUB_DISPLAY:    /* TODO */ break;
		default: break;
	}
}

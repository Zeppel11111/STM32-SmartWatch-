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
#define ITEM_W      32      // 2个中文字符 × 16px
#define ITEM_H      16
#define ITEM1_CX    (ITEM1_X + ITEM_W / 2)   // 菜单中心 X = 16
#define ITEM2_CX    (ITEM2_X + ITEM_W / 2)   // 设置中心 X = 112

/* ── 水滴动画状态 ── */
static float   anim_cx  = ITEM1_CX;    // 高亮条中心 X（带小数，弹簧驱动）
static float   anim_vel = 0;           // 水平速度
static float   anim_w   = ITEM_W;      // 当前宽度（速度大时变窄）

#define K_POS    0.10f      // 位置刚度（越小滑动越慢，0.1≈96px/120ms）
#define D_POS    0.60f      // 阻尼（<0.6 弹过头，>0.7 太粘稠）
#define W_SCALE  0.07f      // 宽度-速度系数（越大越容易被"吸细"）
#define W_MIN    6.0f       // 最窄宽度

/**************************** 初始化主页状态 ****************************/
void UI_home_Init(void)
{
	selected = 0;
	anim_cx  = ITEM1_CX;
	anim_vel = 0;
	anim_w   = ITEM_W;
}

/**************************** 绘制主页 ****************************/
void UI_home_Draw(void)
{
	float target_cx, speed, left;
	uint8_t w;

	MyRTC_ReadTime();
	OLED_Clear();

	/* 日期 */
	OLED_Printf(0, 0, OLED_6X8, "%4d-%02d-%02d",
	            MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]);

	/* 时间 */
	OLED_Printf(32, 16, OLED_8X16, "%02d:%02d:%02d",
	            MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);

	/* 底部控件文字 */
	OLED_ShowString(ITEM1_X, ITEM_Y, "菜单", OLED_8X16);
	OLED_ShowString(ITEM2_X, ITEM_Y, "设置", OLED_8X16);

	/* ── 水滴弹簧动画 ── */
	target_cx = (selected == 0) ? (float)ITEM1_CX : (float)ITEM2_CX;

	/* 弹簧物理驱动水平位置 */
	anim_vel += (target_cx - anim_cx) * K_POS;
	anim_vel *= D_POS;
	anim_cx  += anim_vel;

	/* 吸住：够近且够慢就归位 */
	{
		float diff = target_cx - anim_cx;
		float av   = anim_vel;
		if (diff < 0) diff = -diff;
		if (av   < 0) av   = -av;
		if (diff < 0.3f && av < 0.3f) {
			anim_cx  = target_cx;
			anim_vel = 0;
		}
	}

	/* 宽度由速度决定：动越快越细 → "被抽吸"的效果 */
	speed = anim_vel;
	if (speed < 0) speed = -speed;
	anim_w = (float)ITEM_W * (1.0f - speed * W_SCALE);
	if (anim_w < W_MIN) anim_w = W_MIN;
	if (anim_w > ITEM_W) anim_w = ITEM_W;

	/* 绘高亮条：居中于 anim_cx */
	w    = (uint8_t)(anim_w + 0.5f);
	left = anim_cx - anim_w * 0.5f;
	if (left < 0) left = 0;
	OLED_ReverseArea((uint8_t)left, ITEM_Y, w, ITEM_H);
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
			UI_PushPage(app_page);
		else
			UI_PushPage(setting_page);
	}
}

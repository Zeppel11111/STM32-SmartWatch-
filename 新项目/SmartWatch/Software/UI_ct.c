#include "UI_ct.h"
#include "UI_home.h"
#include "UI_setting.h"
#include "UI_subpage.h"
#include "OLED.h"

/* 导航栈 */
#define NAV_CAPACITY  4                     // 栈容量（主页→设置→子页→调值，4 层够用）
static UI_state nav_stack[NAV_CAPACITY];    // 栈数组：每格存一个页面编号
static uint8_t  nav_top  = 0;              // 栈顶索引（永远指向"当前页面"）
static SubPage   current_sub = 0;          // 当前子功能 ID（仅 nav_stack[top]==sub_page 时有效）

/**************************** 根据页面编号调对应 Init ****************************/
static void page_init(UI_state page)
{
	switch (page)
	{
		case home_page:    UI_home_Init();    break;
		case setting_page: UI_setting_Init(); break;
		case sub_page:     UI_subpage_Init(); break;
		default: break;
	}
}

/**************************** 开机初始化 ****************************/
void UI_Init(void)
{
	nav_stack[0] = home_page;
	nav_top = 0;
	page_init(home_page);		// 主页复位
	OLED_Clear();
}

/**************************** 压栈切页 ****************************/
void UI_PushPage(UI_state page)
{
	if (nav_top < NAV_CAPACITY - 1)
	{
		nav_top++;
		nav_stack[nav_top] = page;
	}
	page_init(page);		// 新页面复位状态
	OLED_Clear();
}

/**************************** 弹栈返回 ****************************/
void UI_PopPage(void)
{
	if (nav_top > 0)
	{
		nav_top--;
	}
	page_init(nav_stack[nav_top]);	// 回到的页面也复位
	OLED_Clear();
}

/**************************** 子页 ID 存取 ****************************/
void UI_SetSubPage(SubPage id)
{
	current_sub = id;
}

SubPage UI_GetSubPage(void)
{
	return current_sub;
}

/**************************** 按键事件 → 栈顶页面 ****************************/
void UI_KeyDispatch(key_t *key, key_event_t event)
{
	switch (nav_stack[nav_top])
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

		case sub_page:
			UI_subpage_OnKey(key, event);
			break;
	}
}

/**************************** 主循环：栈顶查表 → 绘制 + 刷新 ****************************/
void UI_Run(void)
{
	switch (nav_stack[nav_top])
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

		case sub_page:
			UI_subpage_Draw();
			break;
	}
	OLED_Update();
}

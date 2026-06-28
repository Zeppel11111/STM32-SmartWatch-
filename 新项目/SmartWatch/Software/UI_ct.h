#ifndef		__UI_CT_H
#define 	__UI_CT_H

#include "Key.h"

/* 页面状态枚举 */
typedef enum
{
	home_page = 0,      // 主页面
	app_page,           // 图形应用界面
	setting_page        // 设置选项界面
} UI_state;

/* 页面控制 API */
void UI_Run(void);                                  // 主循环调用：绘制当前页 + 刷新屏幕
void UI_KeyDispatch(key_t *key, key_event_t event); // 按键分发到当前页
void UI_SwitchPage(UI_state target);                // 页面切换

#endif

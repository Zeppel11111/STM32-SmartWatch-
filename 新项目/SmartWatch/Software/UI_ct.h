#ifndef		__UI_CT_H
#define 	__UI_CT_H

#include "Key.h"

/* 页面状态枚举（栈中每层存储的"谁在显示"编号） */
typedef enum
{
	home_page = 0,      // 主页面
	app_page,           // 图形应用界面
	setting_page,       // 设置选项界面
	sub_page,           // 子功能页（由 sub_page_id 区分具体功能）
} UI_state;

/* 子功能页 ID（进入 sub_page 前设置，告诉绘制/按键函数"哪个子功能"） */
typedef enum
{
	SUB_BRIGHTNESS = 0, // 亮度
	SUB_VOLUME,         // 音量
	SUB_BLUETOOTH,      // 蓝牙
	SUB_ABOUT,          // 关于
	SUB_RESET,          // 重置
	SUB_TIME,           // 时间
	SUB_DISPLAY,        // 显示
} SubPage;

/* 导航栈 API */
void UI_Init(void);                                 // 开机初始化：设栈底为主页 + 调主页 Init
void UI_Run(void);                                  // 主循环调用：查栈顶 → 绘制 + 刷新
void UI_KeyDispatch(key_t *key, key_event_t event); // 按键分发：查栈顶 → 对应 OnKey
void UI_PushPage(UI_state page);                    // 压栈并切页（进入新页面，自动调 Init）
void UI_PopPage(void);                              // 弹栈返回上一页（自动调 Init）
void UI_SetSubPage(SubPage id);                     // 设子页 ID（PushPage(sub_page) 前调用）
SubPage UI_GetSubPage(void);                        // 获取当前子页 ID

#endif

#ifndef		__UI_HOME_H
#define 	__UI_HOME_H

#include "Key.h"

void UI_home_Init(void);                            // 初始化主页状态（重置选中项）
void UI_home_Draw(void);                            // 绘制主页
void UI_home_OnKey(key_t *key, key_event_t event);  // 主页按键处理

#endif

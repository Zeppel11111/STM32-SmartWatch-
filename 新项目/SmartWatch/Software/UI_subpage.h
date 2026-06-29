#ifndef		__UI_SUBPAGE_H
#define 	__UI_SUBPAGE_H

#include "Key.h"

void UI_subpage_Init(void);                          // 初始化子页状态
void UI_subpage_Draw(void);                            // 绘制子功能页（根据 UI_GetSubPage() 区分）
void UI_subpage_OnKey(key_t *key, key_event_t event);  // 子功能页按键处理

#endif

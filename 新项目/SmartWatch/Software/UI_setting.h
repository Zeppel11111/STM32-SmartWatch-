#ifndef		__UI_SETTING_H
#define 	__UI_SETTING_H

#include "Key.h"

void UI_setting_Draw(void);                            // 绘制设置页
void UI_setting_OnKey(key_t *key, key_event_t event);  // 设置页按键处理

#endif

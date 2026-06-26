#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

/*********************参数宏定义*********************/

/*用户配置区域*/
/*添加按键：修改KEY_COUNT，在Key_ID_t枚举中增加ID，在Key.c配置表中增加一行*/
#define KEY_COUNT				3			// 按键数量
#define KEY_DEBOUNCE_MS			20			// 消抖时间（ms）
#define KEY_LONG_PRESS_MS		1000		// 长按判定时间（ms）

/*********************参数宏定义*********************/


/*********************类型定义*********************/

/*按键ID枚举*/
/*添加新按键时在此增加枚举值，值从1开始*/
typedef enum
{
	KEY_ID_1 = 1,
	KEY_ID_2 = 2,
	KEY_ID_3 = 3,
	// KEY_ID_4 = 4,		// 示例：添加第4个按键
} Key_ID_t;

/*********************类型定义*********************/


/*函数声明*********************/

/*初始化函数*/
void Key_Init(void);

/*定时扫描函数（需由1ms定时中断调用）*/
void Key_Tick(void);

/*===== 任意键查询（推荐，一行搞定）=====*/

/*任意按键按下 → 返回其ID，无事件返回0*/
Key_ID_t Key_AnyPress(void);

/*任意按键释放 → 返回其ID，无事件返回0*/
Key_ID_t Key_AnyRelease(void);

/*任意按键长按 → 返回其ID，无事件返回0*/
Key_ID_t Key_AnyLongPress(void);

/*********************函数声明*/

#endif


#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

/* 按键引脚定义 */
#define KEY1_PIN			GPIO_Pin_2
#define KEY2_PIN			GPIO_Pin_4
#define KEY3_PIN			GPIO_Pin_6
#define KEY_PORT			GPIOA

#define DEBOUNCE_TIME		20		// 消抖时间 (ms)
#define LONGPRESS_TIME		1000	// 长按阈值 (ms)

/* 按键事件类型 */
typedef enum {
	KEY_NONE = 0,
	KEY_PRESS,		// 按下（消抖确认后立即触发）
	KEY_RELEASE,	// 短按释放（按下时长 < 长按阈值）
	KEY_LONGPRESS	// 长按（按下时长 >= 长按阈值，不等释放即触发）
} key_event_t;

/* 按键状态机 */
typedef enum {
	KEY_STATE_FREE = 0,
	KEY_STATE_DEBOUNCE,
	KEY_STATE_PRESSED,
	KEY_STATE_LONGPRESSED
} key_state_t;

/* 按键结构体（每个按键一个实例） */
typedef struct {
	key_state_t     state;
	uint16_t        pin;
	uint16_t        press_tick;		// 按下时刻的 tick
	uint16_t        debounce_tick;	// 消抖起始 tick
} key_t;



/* 全局按键实例 */
extern key_t key1, key2, key3;

void Key_Init(void);
void Key_Tick(void);					// 1ms 定时中断中调用
void Key_Read(key_t *key);				// 单个按键状态机（ISR 中调用）
void Key_EventHandler(key_t *key, key_event_t event);	// 按键事件处理（由 UI 层实现）

#endif

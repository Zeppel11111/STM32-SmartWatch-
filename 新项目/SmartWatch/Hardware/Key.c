#include "stm32f10x.h"
#include "Key.h"

/* 全局按键实例 */
key_t key1 = { .pin = KEY1_PIN };
key_t key2 = { .pin = KEY2_PIN };
key_t key3 = { .pin = KEY3_PIN };

/* 1ms 定时器 tick，在中断中递增 */
static volatile uint16_t key_tick = 0;

/**************************** 定时 tick ****************************/
void Key_Tick(void)
{
	if (key_tick < 65535)
		key_tick++;
	else
		key_tick = 0;
}

/**************************** 初始化 ****************************/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   = KEY1_PIN | KEY2_PIN | KEY3_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);

	/* 默认绑定事件处理回调 */
	Key_BindCallback(&key1, Key_EventHandler);
	Key_BindCallback(&key2, Key_EventHandler);
	Key_BindCallback(&key3, Key_EventHandler);
}

/**************************** 绑定回调 ****************************/
void Key_BindCallback(key_t *key, key_callback_t cb)
{
	key->callback = cb;
}

/**************************** 默认事件处理（__weak：main 重写后自动覆盖） ****************************/
__weak void Key_EventHandler(key_t *key, key_event_t event)
{
	(void)key;
	(void)event;
	/* 默认空实现，由 main.c 重写 */
}

/**************************** 单按键状态机 ****************************/
void Key_Read(key_t *key)
{
	uint8_t pressed = (GPIO_ReadInputDataBit(KEY_PORT, key->pin) == 0);
	uint16_t elapsed;

	switch (key->state) {

	case KEY_STATE_FREE:
		if (pressed) {
			key->state = KEY_STATE_DEBOUNCE;
			key->debounce_tick = key_tick;
		}
		break;

	case KEY_STATE_DEBOUNCE:
		if (!pressed) {
			key->state = KEY_STATE_FREE;
		} else {
			elapsed = (uint16_t)(key_tick - key->debounce_tick);
			if (elapsed >= DEBOUNCE_TIME) {
				key->state = KEY_STATE_PRESSED;
				key->press_tick = key_tick;
				if (key->callback)
					key->callback(key, KEY_PRESS);
			}
		}
		break;

	case KEY_STATE_PRESSED:
		if (!pressed) {
			key->state = KEY_STATE_FREE;
			if (key->callback)
					key->callback(key, KEY_RELEASE);
		} else {
			elapsed = (uint16_t)(key_tick - key->press_tick);
			if (elapsed >= LONGPRESS_TIME) {
				key->state = KEY_STATE_LONGPRESSED;
				if (key->callback)
					key->callback(key, KEY_LONGPRESS);
			}
		}
		break;

	case KEY_STATE_LONGPRESSED:
		if (!pressed) {
			key->state = KEY_STATE_FREE;
		}
		break;
	}
}

/**************************** 扫描全部按键 ****************************/
void Key_Scan(void)
{
	Key_Read(&key1);
	Key_Read(&key2);
	Key_Read(&key3);
}

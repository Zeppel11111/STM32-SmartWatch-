#include "stm32f10x.h"
#include "Key.h"

/* 全局按键实例 */
key_t key1 = { .pin = KEY1_PIN };
key_t key2 = { .pin = KEY2_PIN };
key_t key3 = { .pin = KEY3_PIN };

/* 1ms 定时器 tick，在中断中递增 */
static volatile uint16_t key_tick = 0;

/**************************** 定时 tick（TIM1 中断中调用） ****************************/
void Key_Tick(void)
{
	if (key_tick < 65535)
		key_tick++;
	else
		key_tick = 0;

	/* 周期性扫描：消抖计时 & 长按检测 */
	if (key1.state == KEY_STATE_DEBOUNCE || key1.state == KEY_STATE_PRESSED) Key_Read(&key1);
	if (key2.state == KEY_STATE_DEBOUNCE || key2.state == KEY_STATE_PRESSED) Key_Read(&key2);
	if (key3.state == KEY_STATE_DEBOUNCE || key3.state == KEY_STATE_PRESSED) Key_Read(&key3);
}

/**************************** 初始化 ****************************/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   = KEY1_PIN | KEY2_PIN | KEY3_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);

	/* 映射 GPIOA 引脚到 EXTI 线 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);	// KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);	// KEY2
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);	// KEY3

	/* EXTI 配置：双边沿触发 */
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line    = EXTI_Line2 | EXTI_Line4 | EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NVIC 配置 */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}




/**************************** 单按键状态机（ISR 中调用） ****************************/
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
				Key_EventHandler(key, KEY_PRESS);
			}
		}
		break;

	case KEY_STATE_PRESSED:
		if (!pressed) {
			key->state = KEY_STATE_FREE;
			Key_EventHandler(key, KEY_RELEASE);
		} else {
			elapsed = (uint16_t)(key_tick - key->press_tick);
			if (elapsed >= LONGPRESS_TIME) {
				key->state = KEY_STATE_LONGPRESSED;
				Key_EventHandler(key, KEY_LONGPRESS);
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

/**************************** EXTI 中断服务 ****************************/
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) == SET) {
		Key_Read(&key1);
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) == SET) {
		Key_Read(&key2);
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line6) == SET) {
		Key_Read(&key3);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

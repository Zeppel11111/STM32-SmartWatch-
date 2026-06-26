#include "stm32f10x.h"                  // Device header
#include "Key.h"

/*********************内部事件类型（模块内使用）*********************/

#define KEY_EVT_PRESS			0x01
#define KEY_EVT_RELEASE			0x02
#define KEY_EVT_LONG_PRESS		0x03

/*********************内部事件类型*********************/


/*********************硬件配置表*********************/

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t	 pin;
} Key_HW_Config_t;

static const Key_HW_Config_t Key_Config[KEY_COUNT] =
{
	{GPIOA, GPIO_Pin_2},				// KEY1：PA2
	{GPIOA, GPIO_Pin_4},				// KEY2：PA4
	{GPIOA, GPIO_Pin_6},				// KEY3：PA6
	// {GPIOB, GPIO_Pin_0},			// 示例：添加KEY4
};

/*********************硬件配置表*********************/


/*********************状态机定义*********************/

typedef enum
{
	KEY_STATE_IDLE,
	KEY_STATE_DEBOUNCE_DOWN,
	KEY_STATE_PRESSED,
	KEY_STATE_LONG_PRESS,
	KEY_STATE_DEBOUNCE_UP,
} Key_State_t;

typedef struct
{
	Key_State_t	state;
	uint16_t	timer;
} Key_Runtime_t;

static Key_Runtime_t Key_Runtime[KEY_COUNT];

/*********************状态机定义*********************/


/*********************事件标志位*********************/

static uint8_t Key_Flags_Press[KEY_COUNT];
static uint8_t Key_Flags_Release[KEY_COUNT];
static uint8_t Key_Flags_LongPress[KEY_COUNT];

/*********************事件标志位*********************/


/*********************全局变量*********************/

static uint16_t TickCount = 0;

/*********************全局变量*********************/


/*********************内部函数*********************/

/**
  * 函    数：触发按键事件（内部）
  * 说    明：设置对应按键的事件标志位，供查询API读取
  */
static void Key_FireEvent(Key_ID_t id, uint8_t evt)
{
	uint8_t i = id - 1;
	if (i >= KEY_COUNT) return;

	switch (evt)
	{
		case KEY_EVT_PRESS:		Key_Flags_Press[i] = 1;		break;
		case KEY_EVT_RELEASE:	Key_Flags_Release[i] = 1;	break;
		case KEY_EVT_LONG_PRESS:Key_Flags_LongPress[i] = 1;	break;
	}
}

/**
  * 函    数：扫描任意标志位并清零（内部）
  */
static Key_ID_t Key_AnyFlag(uint8_t *flags)
{
	uint8_t i;
	for (i = 0; i < KEY_COUNT; i ++)
	{
		if (flags[i])
		{
			flags[i] = 0;
			return (Key_ID_t)(i + 1);
		}
	}
	return 0;
}

/*********************内部函数*********************/


/*********************公开函数*********************/

void Key_Init(void)
{
	uint8_t i;
	uint32_t periph_mask = 0;
	uint16_t pin_gpioa = 0;
	uint16_t pin_gpiob = 0;

	for (i = 0; i < KEY_COUNT; i ++)
	{
		if (Key_Config[i].port == GPIOA)
		{
			periph_mask |= RCC_APB2Periph_GPIOA;
			pin_gpioa |= Key_Config[i].pin;
		}
		else if (Key_Config[i].port == GPIOB)
		{
			periph_mask |= RCC_APB2Periph_GPIOB;
			pin_gpiob |= Key_Config[i].pin;
		}
	}

	if (periph_mask) RCC_APB2PeriphClockCmd(periph_mask, ENABLE);

	GPIO_InitTypeDef cfg;
	cfg.GPIO_Mode = GPIO_Mode_IPU;
	cfg.GPIO_Speed = GPIO_Speed_50MHz;

	if (pin_gpioa)
	{
		cfg.GPIO_Pin = pin_gpioa;
		GPIO_Init(GPIOA, &cfg);
	}
	if (pin_gpiob)
	{
		cfg.GPIO_Pin = pin_gpiob;
		GPIO_Init(GPIOB, &cfg);
	}

	for (i = 0; i < KEY_COUNT; i ++)
	{
		Key_Runtime[i].state = KEY_STATE_IDLE;
		Key_Runtime[i].timer = 0;
		Key_Flags_Press[i] = 0;
		Key_Flags_Release[i] = 0;
		Key_Flags_LongPress[i] = 0;
	}

	TickCount = 0;
}

/*===== 任意键查询 =====*/

Key_ID_t Key_AnyPress(void)     { return Key_AnyFlag(Key_Flags_Press); }
Key_ID_t Key_AnyRelease(void)   { return Key_AnyFlag(Key_Flags_Release); }
Key_ID_t Key_AnyLongPress(void) { return Key_AnyFlag(Key_Flags_LongPress); }

/*===== 状态机驱动 =====*/

void Key_Tick(void)
{
	uint8_t i;
	uint8_t raw;

	TickCount ++;
	if (TickCount < KEY_DEBOUNCE_MS) return;
	TickCount = 0;

	for (i = 0; i < KEY_COUNT; i ++)
	{
		raw = !GPIO_ReadInputDataBit(Key_Config[i].port, Key_Config[i].pin);
		Key_Runtime_t *p = &Key_Runtime[i];

		switch (p->state)
		{
			case KEY_STATE_IDLE:
				if (raw)
				{
					p->state = KEY_STATE_DEBOUNCE_DOWN;
					p->timer = 0;
				}
				break;

			case KEY_STATE_DEBOUNCE_DOWN:
				p->timer += KEY_DEBOUNCE_MS;
				if (p->timer >= KEY_DEBOUNCE_MS)
				{
					if (raw)
					{
						p->state = KEY_STATE_PRESSED;
						p->timer = 0;
						Key_FireEvent((Key_ID_t)(i + 1), KEY_EVT_PRESS);
					}
					else
					{
						p->state = KEY_STATE_IDLE;
						p->timer = 0;
					}
				}
				break;

			case KEY_STATE_PRESSED:
				p->timer += KEY_DEBOUNCE_MS;
				if (!raw)
				{
					p->state = KEY_STATE_DEBOUNCE_UP;
					p->timer = 0;
				}
				else if (p->timer >= KEY_LONG_PRESS_MS)
				{
					p->state = KEY_STATE_LONG_PRESS;
					Key_FireEvent((Key_ID_t)(i + 1), KEY_EVT_LONG_PRESS);
				}
				break;

			case KEY_STATE_LONG_PRESS:
				if (!raw)
				{
					p->state = KEY_STATE_DEBOUNCE_UP;
					p->timer = 0;
				}
				break;

			case KEY_STATE_DEBOUNCE_UP:
				p->timer += KEY_DEBOUNCE_MS;
				if (p->timer >= KEY_DEBOUNCE_MS)
				{
					if (!raw)
					{
						p->state = KEY_STATE_IDLE;
						p->timer = 0;
						Key_FireEvent((Key_ID_t)(i + 1), KEY_EVT_RELEASE);
					}
					else
					{
						p->state = KEY_STATE_PRESSED;
						p->timer = 0;
					}
				}
				break;
		}
	}
}

/*********************公开函数*/


/*****************江协科技|版权所有****************/
/*****************jiangxiekeji.com*****************/

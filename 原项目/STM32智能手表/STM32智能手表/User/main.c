#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "menu.h"
#include "Timer.h"
#include "Timer3.h"
#include "Key.h"
#include "Function.h"

//2026.3.5日志
//项目进度：100%
//手表1.0代码完工
/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

int main(void)
{
	OLED_Clear();
	Peripheral_Init();

	int clkflag1;
	
	while (1)
	{
		clkflag1=First_Page_Clock();
		if(clkflag1==1){Menu_Control();}//菜单
		else if(clkflag1==2){Setting_Page();}//设置
	}
}

// 定时器中断函数，可以复制到使用它的地方
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		Key3_Tick();
		Dino_Core_Tick();//100ms
		Meme_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM3_IRQHandler(void)
{//1ms
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		Timer_Tick();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}



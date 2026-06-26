#include "stm32f10x.h"                  // Device header
#include "MyRTC.h"
#include "OLED.h"
#include "Key.h"

extern uint8_t KeyNum;

void Show_SetTime_FirstUI(void)
{
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_Printf(0, 16, OLED_8X16, "年:%4d",MyRTC_Time[0]);
	OLED_Printf(0, 32, OLED_8X16, "月:%2d",MyRTC_Time[1]);
	OLED_Printf(0, 48, OLED_8X16, "日:%2d",MyRTC_Time[2]);
}

void Show_SetTime_SecondUI(void)
{
	OLED_Clear();
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_Printf(0, 16, OLED_8X16, "时:%2d",MyRTC_Time[3]);
	OLED_Printf(0, 32, OLED_8X16, "分:%2d",MyRTC_Time[4]);
	OLED_Printf(0, 48, OLED_8X16, "更改时间");
}

void Change_Time(void)
{
	uint16_t Year=2000;
	uint8_t Month=1;
	uint8_t Day=1;
	uint8_t Hour=1;
	int8_t Minute=1;
	//年循环
	while(1)
	{
		OLED_Clear();
		KeyNum=Key_GetNum();
		if(KeyNum==1)//上一项
		{
			Year--;
			if(Year<=1900)Year=1900;
		}
		else if(KeyNum==2)//下一项
		{
			Year++;
			if(Year>=2500)Year=2500;
		}
		else if(KeyNum==3)//确认
		{
			MyRTC_Time[0]=Year;
			break;
		}
		OLED_ShowString(0, 0,"年", OLED_8X16);
		OLED_Printf(32,16,OLED_12X24,"%02d",Year);
		OLED_Update();
	}
	//月循环
	while(1)
	{
		OLED_Clear();
		KeyNum=Key_GetNum();
		if(KeyNum==1)//上一项
		{
			Month--;
			if(Month<=0)Month=12;
		}
		else if(KeyNum==2)//下一项
		{
			Month++;
			if(Month>=13)Month=1;
		}
		else if(KeyNum==3)//确认
		{
			MyRTC_Time[1]=Month;
			break;
		}
		OLED_ShowString(0, 0,"月", OLED_8X16);
		OLED_Printf(48,16,OLED_12X24,"%02d",Month);
		OLED_Update();
	}
	//日循环
	while(1)
	{
		OLED_Clear();
		KeyNum=Key_GetNum();
		if(KeyNum==1)//上一项
		{
			Day--;
			if(Day<=0)Day=31;
		}
		else if(KeyNum==2)//下一项
		{
			Day++;
			if(Day>=32)Day=1;
		}
		else if(KeyNum==3)//确认
		{
			MyRTC_Time[2]=Day;
			break;
		}
		OLED_ShowString(0, 0,"日", OLED_8X16);
		OLED_Printf(48,16,OLED_12X24,"%02d",Day);
		OLED_Update();
	}
	//时循环
	while(1)
	{
		OLED_Clear();
		KeyNum=Key_GetNum();
		if(KeyNum==1)//上一项
		{
			Hour--;
			if(Hour<=0)Hour=24;
		}
		else if(KeyNum==2)//下一项
		{
			Hour++;
			if(Hour>=25)Hour=1;
		}
		else if(KeyNum==3)//确认
		{
			MyRTC_Time[3]=Hour;
			break;
		}
		OLED_ShowString(0, 0,"时", OLED_8X16);
		OLED_Printf(48,16,OLED_12X24,"%02d",Hour);
		OLED_Update();
	}
	//分循环
	while(1)
	{
		OLED_Clear();
		KeyNum=Key_GetNum();
		if(KeyNum==1)//上一项
		{
			Minute--;
			if(Minute<0)Minute=59;
		}
		else if(KeyNum==2)//下一项
		{
			Minute++;
			if(Minute>=60)Minute=0;
		}
		else if(KeyNum==3)//确认
		{
			MyRTC_Time[4]=Minute;
			break;
		}
		OLED_ShowString(0, 0,"分", OLED_8X16);
		OLED_Printf(48,16,OLED_12X24,"%02d",Minute);
		OLED_Update();
	}
	MyRTC_SetTime();
	OLED_Clear();
}

int settimeflag=1;
int Set_Time(void)
{
	OLED_Clear();
	while(1)
	{
		KeyNum=Key_GetNum();
		int settimeflag_temp=0;
		if(KeyNum==1)//上一项
		{
			settimeflag--;
			if(settimeflag<=0)settimeflag=7;
		}
		else if(KeyNum==2)//下一项
		{
			settimeflag++;
			if(settimeflag>=8)settimeflag=1;
		}
		else if(KeyNum==3)//确认
		{
			settimeflag_temp=settimeflag;
			OLED_Clear();
			OLED_Update();
		}
		//进入时间更改
		if(settimeflag_temp==1)return 0;
		else if(settimeflag_temp==7)Change_Time();
		
		switch(settimeflag)
		{
			case 1:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 2:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			case 3:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
			case 4:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,48,16,16);
				OLED_Update();
				break;
			case 5:
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			case 6:
				OLED_Clear();
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
			case 7:
				OLED_Clear();
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,48,64,16);
				OLED_Update();
				break;
		}
	}
}
#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"                                                                                                                                                                                                     
#include "Key.h"
#include "LED.h"
#include "SetTime.h"
#include "menu.h"
#include "Function.h" 
#include "MPU6050.h" 
#include <math.h>  // 包含数学库
#include "AD.h" 

uint8_t KeyNum;
int Menu_t[7]={-96,-48,0,48,96,144,192};
void Peripheral_Init(void)
{ 
	OLED_Init();
	MyRTC_Init();
	Key_Init();
	Lighter_Init();
	Timer_Init();
	Timer3_Init();
	MPU6050_Init();
	AD_Init();
}



/*----------------------------------首页时钟-------------------------------------*/
uint16_t ADvalue;
float VBTA;
int Battery_Capacity;

void Show_Battery(void)
{
	int sum;

	for(int i=0;i<3000;i++)
	{
		ADvalue=AD_GetValue();
		sum+=ADvalue;
	}
	
	ADvalue=sum/3000;
	VBTA=(float)ADvalue/4095*3.3;
	Battery_Capacity=(ADvalue-3276)*100/819;
	if(Battery_Capacity<0)Battery_Capacity=0;
	
//	OLED_ShowNum(64,0,ADvalue,4,OLED_6X8);
//	OLED_Printf(64, 8, OLED_6X8,"VBTA:%.2f",VBTA);
	OLED_ShowNum(85,4,Battery_Capacity,3,OLED_6X8);
	OLED_ShowChar(103,4,'%',OLED_6X8);
	
	if(Battery_Capacity==100)OLED_ShowImage(110,0,16,16,Battery);
	else if(Battery_Capacity>=10&&Battery_Capacity<100)
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea((112+Battery_Capacity/10),5,(10-Battery_Capacity/10),6);
		OLED_ClearArea(85,4,6,8);
	}
	
	else 
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea(112,5,10,6);
		OLED_ClearArea(85,4,12,8);
	}
}


void Show_Clock_UI(void)
{
	Show_Battery();
	MyRTC_ReadTime();
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);
}

int clkflag=1;

int First_Page_Clock(void)
{
	OLED_Clear();
	while(1)
	{
		KeyNum=Key_GetNum();
		
		if(KeyNum==1)//上一项
		{
			clkflag--;
			if(clkflag<=0)clkflag=2;
		}
		else if(KeyNum==2)//下一项
		{
			clkflag++;
			if(clkflag>=3)clkflag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			return clkflag;
		}
		else if(KeyNum==4)//确认
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);	
			GPIO_SetBits(GPIOB, GPIO_Pin_12);	
		}
		
		switch(clkflag)
		{
			case 1:
				Show_Clock_UI();
				OLED_ReverseArea(0,48,32,16);
				OLED_Update();
				break;
			
			case 2:
				Show_Clock_UI();
				OLED_ReverseArea(96,48,32,16);
				OLED_Update();
				break;
		}
	}
}

/*----------------------------------设置界面UI-------------------------------------*/

void Show_Setting_UI(void)
{
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_ShowString(0,16,"日期时间设置",OLED_8X16);
}

int setflag=1;
int menuflag=3;//数组对齐

int Setting_Page(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t setflag_temp=0;
		if(KeyNum==1)//上一项
		{
			setflag--;
			if(setflag<=0)setflag=2;
		}
		else if(KeyNum==2)//下一项
		{
			setflag++;
			if(setflag>=3)setflag=1;
		}
		else if(KeyNum==3)//确认
		{
			setflag_temp=setflag;
			if(setflag_temp==1)return 0;
			else if(setflag_temp==2)Set_Time();
		}
		
		switch(setflag)
		{
			case 1:
				Show_Setting_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			
			case 2:
				Show_Setting_UI();
				OLED_ReverseArea(0,16,96,16);
				OLED_Update();
				break;
		}
	}
}

/*----------------------------------滑动菜单-------------------------------------*/

//直接显示一整组
void Moving_MenuUI(void)
{
	OLED_Clear();
	OLED_ShowImage(42,10,44,44,Chosen);
	OLED_ShowImage(Menu_t[0],Menu_Y,32,32,Menu[0]);
	OLED_ShowImage(Menu_t[1],Menu_Y,32,32,Menu[1]);
	OLED_ShowImage(Menu_t[2],Menu_Y,32,32,Menu[2]);
	OLED_ShowImage(Menu_t[3],Menu_Y,32,32,Menu[3]);
	OLED_ShowImage(Menu_t[4],Menu_Y,32,32,Menu[4]);
	OLED_ShowImage(Menu_t[5],Menu_Y,32,32,Menu[5]);
	OLED_ShowImage(Menu_t[6],Menu_Y,32,32,Menu[6]);
	OLED_Update();
}

void Menu_Move(int* menu, int step)
{*menu+=step;}
//0 1 2 3 4 5 6数组角标
//菜单滑动，扫描按键然后遍历自增x
void Menu_Control(void)
{
	while(1)
	{
		Moving_MenuUI();
		KeyNum=Key_GetNum();
		uint8_t menuflag_temp=7;
		//向左移动
		if(KeyNum==1)//上一项
		{
			if(menuflag>0)
			{
			for(int y=1;y<=12;y++)
			{
				int step=4;
				for(int i=0;i<=6;i++)
				{	
					Menu_Move(&Menu_t[i],step);
				}
				Moving_MenuUI();
			}
			}
			menuflag--;
			if(menuflag<0)menuflag=0;	
		}
		else if(KeyNum==2)//下一项
		{
			if(menuflag<6)
			{
				for(int y=1;y<=12;y++)
			{
				int step=4;
				for(int i=0;i<=6;i++)
				{	
					Menu_Move(&Menu_t[i],-step);
				}
				Moving_MenuUI();
			}
				
			}
			menuflag++;
			if(menuflag>=7)menuflag=6;
		}
		else if(KeyNum==3)//确认
		{
			menuflag_temp=menuflag;
		}
		//显示功能
		switch(menuflag_temp)
		{
			case 0:
				Function_Game();
				break;
			case 1:
				Function_MPU6050();
				break;
			case 2:
				Function_Timer3();
				break;
			case 3:
				Function_Lighter();
				break;
			case 4:
				Function_Gradienter();
				break;
			case 5:
				Function_Meme();
				break;
			case 6:
				return;
				break;
		}
	}

}




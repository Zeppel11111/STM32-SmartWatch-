#include "stm32f10x.h"                  // Device header
#include "Function.h"  
#include "OLED.h"
#include "Key.h"
#include "math.h"
#include <stdlib.h>  // 提供 rand() 和 srand()
#include <time.h>     // 提供 time() 作为随机种子
#include "Timer3.h"
#include "Delay.h"
#include "MPU6050.h"

extern uint8_t KeyNum;
//功能菜单模板
void Function_Template(void)
{
//	int func_flag=1;
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t setflag_temp=0;
		if(KeyNum==1)//上一项
		{
		}
		else if(KeyNum==2)//下一项
		{
		}
		else if(KeyNum==3)//确认
		{
			return;
		}
	OLED_Clear();
	OLED_ShowString(32,24,"敬请期待",OLED_8X16);
	OLED_Update();
	}
}

uint8_t game_setflag=1;

/*--------------------------游戏UI-------------------------------*/

void Function_GameUI(void)
{
	OLED_Clear();
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_ShowString(0,16,"小恐龙游戏",OLED_8X16);
}
/*-----------------------------小恐龙模块-----------------------------------*/
//用于得分计算
int Dino_core;
int Dino_core_temp;
int Dino_ground[2];
int Dino_ground_tick;
int Dino_enemy[2];
int jump_tick;
int Jump_flag=0;
int rand_1;
int death_flag=0;
float Dino_y;

void Dino_Core_Tick(void)
{
	static uint8_t tick;
	tick++;
	jump_tick++;
		if(jump_tick>=55555)jump_tick=55554;
	if(tick>=100)
	{
	tick=0;	
	Dino_core+=1;
		if(Dino_core>=55555)Dino_core=55554;
	Dino_ground_tick+=4;
		if(Dino_ground_tick>128)
		{
			Dino_ground_tick=0;
		}
	}	
}

void Dino_Iint(void)
{
	Dino_y=0;
	srand(Dino_core);
	Dino_core=0;
	Dino_core_temp=0;
	jump_tick=0;
	Dino_ground_tick=0;
	death_flag=0;
	Dino_ground[0]=0;
	Dino_ground[1]=128;
	OLED_Clear();
	OLED_ShowImage(0,60,128,6,Ground);
	OLED_Update();
}

void Dino_Jump(void)
{  
	float Dino_yy;
    if(Jump_flag == 1)  // 只在刚按下按键时重置 tick
    {
        jump_tick = 0;
        Jump_flag = 2;  // 使用2表示正在跳跃中
    }
    
    // 限制跳跃时间，例如30个tick完成跳跃
    if(jump_tick <= 1500)
    {
        Dino_yy = 36 * sin(3.14 * jump_tick / 1500);  // 调整跳跃曲线
		Dino_y=42-Dino_yy;
        OLED_ShowImage(5, Dino_y, 16, 18, Diom[0]);
    }
    else
    {
        Jump_flag = 0;  // 跳跃结束
    }
}

void Dino_Run(void)
{
	Dino_y=42;
	if(0<=(Dino_ground_tick%8)&&(Dino_ground_tick%8)<=3)
		OLED_ShowImage(5,Dino_y,16,18,Diom[0]);
	else 
		OLED_ShowImage(5,Dino_y,16,18,Diom[1]);  	
}
//碰撞检测，直接读取全局数据，没有分离模块
int Collision_Detection(void)
{
	if(((Dino_y+18)>=44&&((Dino_enemy[0])<=21)&&((Dino_enemy[0]+14)>=21))||((Dino_y+18)>=44&&((Dino_enemy[0])<=5)&&((Dino_enemy[0]+14)>=5)))
	{
		death_flag=1;
		return death_flag;
	}
	else death_flag=0;
}

void Function_Game_Dino(void)
{
	Dino_Iint();
	
	while(1)
	{	
		//碰撞检测	
		if(!Collision_Detection())
		{
		KeyNum=Key_GetNum();
		if(KeyNum==1&&Jump_flag==0)//恐龙跳跃
		{
			Jump_flag=1;	
		}
		else if(KeyNum==2)//下一项
		{
		}
		else if(KeyNum==3)//确认
		{
			return;
		}
		
		OLED_Clear();
		//分数8像素
		//云朵 10像素    
		//恐龙 18
		//地面3像素
		Dino_ground[0]=0-Dino_ground_tick;
		Dino_ground[1]=128-Dino_ground_tick;	
		Dino_core_temp=Dino_core;
		OLED_ShowNum(98,0,Dino_core_temp,5,OLED_6X8);
		//地面显示
		OLED_ShowImage(Dino_ground[0],58,128,6,Ground);
		OLED_ShowImage(Dino_ground[1],58,128,6,Ground+1);

		//云朵显示（绑定地面）
		OLED_ShowImage(Dino_ground[0],8,8,6,Cloud);
		OLED_ShowImage(Dino_ground[0]+64,12,8,6,Cloud);
		OLED_ShowImage(Dino_ground[1],8,8,6,Cloud);
		OLED_ShowImage(Dino_ground[1]+64,12,8,6,Cloud);
		//仙人掌显示
		if(Dino_ground_tick==128)rand_1=rand()%3; 		
		Dino_enemy[0]=Dino_ground[0]+114;
		Dino_enemy[1]=Dino_ground[1]+114;
		OLED_ShowImage(Dino_enemy[0],44,14,16,Enemy[rand_1]);
		OLED_ShowImage(Dino_enemy[1],44,14,16,Enemy[rand_1]);
		// 恐龙显示
		if(Jump_flag != 0)  // 正在跳跃
			Dino_Jump();
		else 
			{
			Dino_Run();      
			}		
		}
		else 
		{
				KeyNum=Key_GetNum();
			if(KeyNum==1&&Jump_flag==0)//恐龙跳跃
			{
				Dino_Iint();
				return;	
			}
			else if(KeyNum==2)//下一项
			{
				Dino_Iint();
				return;
			}
			else if(KeyNum==3)//确认
			{
				Dino_Iint();
				return;
			}
			if(0<=(Dino_ground_tick%40)&&(Dino_ground_tick%40)<=19)
			{
			OLED_ShowString(32,24,"GAME OVER!!!",OLED_8X16);
			OLED_ShowNum(32,40,Dino_core_temp,5,OLED_8X16);
			}
			else
			OLED_Clear();
		}
			OLED_Update();
	}
}

/*-----------------------------小恐龙模块-----------------------------------*/


//这是进入“游戏”界面的菜单
void Function_Game(void)
{
	Function_GameUI();
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t game_setflag_temp=0;
		if(KeyNum==1)//上一项
		{
			game_setflag--;
			if(game_setflag<=0)game_setflag=2;
		}
		else if(KeyNum==2)//下一项
		{
			game_setflag++;
			if(game_setflag>=3)game_setflag=1;			
		}
		else if(KeyNum==3)//确认
		{
			game_setflag_temp=game_setflag;
		}
		
		switch(game_setflag)
		{
			case(1):
				Function_GameUI();
				OLED_ReverseArea(0,0,16,16);	
				OLED_Update();
				break;
			case(2):
				Function_GameUI();
				OLED_ReverseArea(0,16,16*5,16);	
				OLED_Update();
				break;
		}
		switch(game_setflag_temp)
		{
			case(1):
				return;
				break;
			//游戏1：小恐龙跳跃
			case(2):
				Function_Game_Dino();
				break;
		}
		

	}
}

/*---------------------------------小灯区----------------------------------------*/
//PB15

void Lighter_On(void);
void Lighter_Off(void);

void Lighter_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_12|GPIO_Pin_13;				//GPIO引脚，赋值为第0号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOB的初始化
	Lighter_Off();
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);	
	GPIO_SetBits(GPIOB, GPIO_Pin_13);	
}

void Lighter_On(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);	
}

void Lighter_Off(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);	
}

/*------------------------------------小灯UI-------------------------------------------*/

uint8_t lighter_falg=1;

void Lighter_UI(void)
{
	OLED_Clear();
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_ShowString(32,24,"ON",OLED_8X16);
	OLED_ShowString(80,24,"OFF",OLED_8X16);
}


void Function_Lighter(void)
{
	Lighter_UI();
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t lighter_falg_temp=0;
		if(KeyNum==1)//上一项
		{
			lighter_falg--;
			if(lighter_falg<=0)lighter_falg=3;
		}
		else if(KeyNum==2)//下一项
		{
			lighter_falg++;
			if(lighter_falg>=4)lighter_falg=1;			
		}
		else if(KeyNum==3)//确认
		{
			lighter_falg_temp=lighter_falg;
		}
		
		switch(lighter_falg)
		{
			case(1):
				Lighter_UI();
				OLED_ReverseArea(0,0,16,16);	
				OLED_Update();
				break;
			case(2):
				Lighter_UI();
				OLED_ReverseArea(32,24,8*2,16);	
				OLED_Update();
				break;
			case(3):
				Lighter_UI();
				OLED_ReverseArea(80,24,8*3,16);	
				OLED_Update();
				break;
		}
		switch(lighter_falg_temp)
		{
			case(1):
				return;
				break;
			case(2):
				Lighter_On();
				break;
			case(3):
				Lighter_Off();
				break;
		}
		

	}
}

/*----------------------------------------表情包meme区--------------------------------------------*/
//点进来直接播放无需UI
static uint8_t meme_tick=0;

void Meme_Tick(void)
{
	meme_tick++;
	if(meme_tick>=65535)meme_tick=0;
}

void Function_Meme(void)
{
	OLED_Clear();
	int count=0;
	meme_tick=0;
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)//确认
		{
			return;
		}
		if(meme_tick%250==0)
		{
			OLED_ShowImage(32,0,64,64,Love[count]);
			OLED_Update();
			count++;
			if(count>=4)count=0;
		}
	}

}

/*----------------------------------------计时器--------------------------------------------*/
//时分秒
static int Count_Timer[3]={0,0,0};
//用于读定时器
uint16_t count3;

void Function_Timer3_Init(void)
{
	Count_Timer[0]=0;
	Count_Timer[1]=0;
	Count_Timer[2]=0;
	count3=0;
}

void Timer_Tick(void)
{
	count3++;
}

void Function_Timer3_Add(void)
{
	Count_Timer[2]=count3;
	
	if(Count_Timer[2]==60)
	{
		Count_Timer[2]=0;
		Count_Timer[1]++;
	}
	
	if(Count_Timer[1]==60)
	{
		Count_Timer[1]=0;
		Count_Timer[0]++;
	}
}
	
void Function_TimerUI(void)
{
	OLED_Clear();
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_ShowImage(16,16,16, 16, Timer[0]);
	OLED_ShowImage(48,16,16, 16, Timer[1]);
	OLED_ShowImage(80,16,16, 16, Timer[2]);
	OLED_Printf(16,32,OLED_12X24,"%02d:%02d:%02d",Count_Timer[0],Count_Timer[1],Count_Timer[2]);
}

uint8_t timer3_flag=1;

void Function_Timer3(void)
{
	Function_Timer3_Init();
	Function_TimerUI();
	while(1)
	{
		Function_Timer3_Add();
		
		KeyNum=Key_GetNum();
		uint8_t timer3_flag_temp=0;
		if(KeyNum==1)//上一项
		{
			timer3_flag--;
			if(timer3_flag<=0)timer3_flag=4;
		}
		else if(KeyNum==2)//下一项
		{
			timer3_flag++;
			if(timer3_flag>=5)timer3_flag=1;			
		}
		else if(KeyNum==3)//确认
		{
			timer3_flag_temp=timer3_flag;
		}
		
		switch(timer3_flag)
		{
			case(1):
				Function_TimerUI();
				OLED_ReverseArea(0,0,16,16);	
				OLED_Update();
				break;
			case(2):
				Function_TimerUI();
				OLED_ReverseArea(16,16,8*2,16);	
				OLED_Update();
				break;
			case(3):
				Function_TimerUI();
				OLED_ReverseArea(48,16,8*2,16);	
				OLED_Update();
				break;
			case(4):
				Function_TimerUI();
				OLED_ReverseArea(80,16,8*2,16);	
				OLED_Update();
				break;
		}
		switch(timer3_flag_temp)
		{
			case(1):
				Timer3_Stop();
				return;
				break;
			case(2):
				//继续计时
				Timer3_On();
				break;
			case(3):
				Timer3_Stop();
				break;
			case(4):
				Timer3_Reset();
				break;
		}
	
	}
}


/*----------------------------------MPU6050-------------------------------------*/

int16_t ax,ay,az,gx,gy,gz;//MPU6050测得的三轴加速度和角速度
float roll_g,pitch_g,yaw_g;//陀螺仪解算的欧拉角
float roll_a,pitch_a;//加速度计解算的欧拉角
float Roll,Pitch,Yaw;//互补滤波后的欧拉角
float a=0.9;//互补滤波器系数
float Delta_t=0.005;//采样周期
double pi=3.1415927;
 

void MPU6050_Calculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	
	//通过陀螺仪解算欧拉角
	roll_g=Roll+(float)gx*Delta_t;
	pitch_g=Pitch+(float)gy*Delta_t;
	yaw_g=Yaw+(float)gz*Delta_t;
	
	//通过加速度计解算欧拉角
	pitch_a=atan2((-1)*ax,az)*180/pi;
	roll_a=atan2(ay,az)*180/pi;
	
	//通过互补滤波器进行数据融合
	Roll=a*roll_g+(1-a)*roll_a;
	Pitch=a*pitch_g+(1-a)*pitch_a;
	Yaw=a*yaw_g;
}

void MPU6050_UI(void)
{
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_Printf(0, 16, OLED_8X16, "Roll:%0.2f", Roll);
	OLED_Printf(0, 32, OLED_8X16, "Pitch:%0.2f", Pitch);
	OLED_Printf(0, 48, OLED_8X16, "Yaw:%0.2f", Yaw);
}

void Function_MPU6050(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)//确认
		{
			return;
		}
		OLED_Clear();
		MPU6050_Calculation();
		MPU6050_UI();
		OLED_ReverseArea(0,0,16,16);
		OLED_Update();
	}

}

/*----------------------------------水平仪-------------------------------------*/

void Gradienter_UI(void)
{
	MPU6050_Calculation();
	OLED_Clear();
	OLED_ShowImage(0,0,16, 16, Return);
	OLED_DrawCircle(64,32,30,0);
	OLED_DrawCircle(64-Roll,32+Pitch,4,1);
}

void Function_Gradienter(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)//确认
		{
			return;
		}
		Gradienter_UI();
		OLED_ReverseArea(0,0,16,16);
		OLED_Update();
	}


}


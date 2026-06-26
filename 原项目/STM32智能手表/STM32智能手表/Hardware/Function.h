#ifndef __FUNCTION_H
#define __FUNCTION_H

void Function_Template(void);
//游戏区
void Function_GameUI(void);
void Function_Game(void);
void Function_Game_Dino(void);
void Dino_Core_Tick(void);
void Dino_Iint(void);
void Dino_Jump(void);
void Dino_Run(void);
int Collision_Detection(void);
//小灯区
void Lighter_Init(void);
void Lighter_On(void);
void Lighter_Off(void);

void Lighter_UI(void);
void Function_Lighter(void);
//表情包区 进行取模图片的循环播放
void Function_Meme(void);

//计时器区
void Function_Timer3_Init(void);
void Timer_Tick(void);
void Function_Timer3_Add(void);
void Function_TimerUI(void);

void Function_Timer3(void);

//MPU6050区
void MPU6050_Calculation(void);
void MPU6050_UI(void);
void Function_MPU6050(void);

//水平仪
void Gradienter_UI(void);
void Function_Gradienter(void);

#endif

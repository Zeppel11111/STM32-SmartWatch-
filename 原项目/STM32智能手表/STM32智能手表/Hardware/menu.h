#ifndef __MENU_H
#define __MENU_H
/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
		32	16	|32  16  32  16  32  16  32
  * 
  * 		      0             X轴           127 
  * 		     .------------------------------->
  * 		   0 |
  * 		     |
  * 		     |
  * 		     |
  * 		 Y轴 |
  * 		     |
  * 		     |
  * 		    |
  *  		 63 |
  * 		     v
  * 
  */
#define Menu_Y		16

void Show_Battery(void);
void Menu_Move(int* menu, int step);
void Menu_Control(void);
void Peripheral_Init(void);
void Show_Clock_UI(void);
void Show_Setting_UI(void);
int First_Page_Clock(void);
int Setting_Page(void);


#endif

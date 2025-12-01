#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "key.h"
#include "stepmotor.h"
#include "led.h"
#define MAX_FAN_SPEED 300
uint16_t AD0,AD1;
u16 vol_1 = 0;
int now_dir = 0;
unsigned int fanSpeed = 0; //风速
unsigned int fanDir= 0;//风向
//旋转标志
//static int has_rotated = 0;
// 需要在全局或静态变量中保存上一次的风向值
//static unsigned int last_fanDir = 0;
int Key_Num=0;
int Key_Num1=0;
int mode=0;
int jiaodu;//角度差
int diff;//判断顺时针还是逆时针旋转

float  Voltage;

long map(long x, long in_min, long in_max, long out_min, long out_max) 
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//信息更新
void UpdateFanDate()
{
		//风速
		AD0 = Get_Adc_Average(ADC_Channel_0,5); //读取ad转换值 5次求平均	
		if(AD0<=1) AD0=0;	//滤除干扰
		vol_1=map(AD0, 0, 4095, 0, 5000);	
		fanSpeed= vol_1*0.027;//
		if(fanSpeed>300)fanSpeed=300;
		//风向
		AD1 = Get_Adc_Average(ADC_Channel_2,5);//读取ad转换值 5次求平均	
		Delay_ms(50);
		if(AD1<=1) AD1=0;	//滤除干扰
		fanDir= ((float)AD1 / 4095 * 3.3)*2*360/5;// 
		if(fanDir>360)fanDir=0;	
		OLED_ShowNum(1, 7, fanSpeed, 3);
		OLED_ShowNum(2, 5, fanDir, 3);
}



int main(void)
{
	
	OLED_Init();
	AD_Init();
	Key_Init();
	Motor_GPIO_Init();
	SystemInit();
	LED_Init();
	OLED_ShowString(1,1,"speed:");
	OLED_ShowString(2,1,"Dir:");
	OLED_ShowString(4,1,"now_dir");
	
	
	while (1)
	{
		
		Key_Num=Key_GetNum();
		OLED_ShowString(2,8,"   ");
		if(Key_Num==1)    mode=1;//模式切换，
		else if(Key_Num==2)  mode=0;//模式切换，
	
		if(mode==0)//按键A3，自动
		{
			while(1)
			{	UpdateFanDate();//风速风向
				OLED_ShowString(3,1,"auto  ");
				Key_Num=Key_GetNum();
				
				//200毫秒读取一次按键
				Delay_ms(50);
				Motor_RotateTo(fanDir);	//自动旋转控制部分，
				UpdateFanDate();
				OLED_ShowNum(4, 8, fanDir, 3);
				if(Key_Num==1)    mode=1;//模式切换，
				else if(Key_Num==2)  mode=0;//模式切换
				if (mode != 0) break;
				
			}	
			
		}	
		 if(mode==1)//按键A3，手动
		{
			while(1)
			{
			OLED_ShowString(3,1,"manual");
			UpdateFanDate();//风速风向
			Key_Num=Key_GetNum();
			//50毫秒读取一次按键
			Delay_ms(50);

			//旋转控制部分
				
				Key_Num1=Key_GetNum1();	
				if(Key_Num1==3)//B0
				{	
					OLED_ShowString(2,8,"+10");
					Motor_Rotate(10.0, 0);//正转10度
					now_dir+=10;
					if(now_dir>360) now_dir=0;
					OLED_ShowNum(4, 8, now_dir, 3);
				}
		
				if(Key_Num1==4)//B11
				{	
					OLED_ShowString(2,8,"-10");
					Motor_Rotate(10.0, 1);//正转10度
					now_dir-=10;
					if(now_dir<0) now_dir=350;
					OLED_ShowNum(4, 8, now_dir, 3);
				}
				
				if(Key_Num==1)    mode=1;//模式切换，
				else if(Key_Num==2)  mode=0;//模式切换
				if (mode != 1) break;
				
			}
		}	

			
		}	
			
	}


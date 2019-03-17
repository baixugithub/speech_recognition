#include "includes.h"

/***********************************************************
* 名    称：void Delayms(uint16 i)
* 功    能： 延时n毫秒
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法：
**********************************************************/ 
void Delayms(uint16 i)
{
	unsigned char a,b;
	for(;i>0;i--)
	    for(b=4;b>0;b--)
            for(a=113;a>0;a--);	
}

/***********************************************************
* 名    称：void Glide_LED(void)
* 功    能： 实现流水灯功能
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Glide_LED(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	Delayms(0xfff);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	Delayms(0xfff);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	Delayms(0xfff);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	Delayms(0xfff);

}

/***********************************************************
* 名    称： void Flicker_LED(void)
* 功    能：  实现灯闪烁功能
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Flicker_LED(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	Delayms(0XFFF);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	Delayms(0XFFF);
}

/***********************************************************
* 名    称： Ctrl_Relay
* 功    能： 控制继电器
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void On_Relay(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_5); 				 //PD.5 输出高 ;
}
void Off_Relay(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}

/***********************************************************
* 名    称：void Off_LED(void)
* 功    能：  关闭全部LED
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Off_LED(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}

/***********************************************************
* 名    称：void Board_text(uint8 Code_Val)
* 功    能：	开发板测试程序
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Board_text(uint8 Code_Val)
{																					 
	 switch(Code_Val)		   /*对结果执行相关操作,客户修改*/
	{
		case CODE_KD:			/*命令“流水灯”*/
				 					On_Relay();
										 break;
		case CODE_GD:	 /*命令“闪烁”*/
									Off_Relay();
										 break;
		case CODE_AJCF:		/*命令“按键触发”*/
									Flicker_LED();
										break;
		case CODE_QM:		/*命令“全灭”*/
									Off_LED();
										break;
		default:break;
	}	


}

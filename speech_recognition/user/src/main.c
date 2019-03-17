/*************端口信息********************
 * 接线说明
 * RST      PB6
 * CS   	PA4//使能ld3320
 * WR/SPIS  PC4
 * P2/SDCK  PA5
 * P1/SDO   PA6
 * P0/SDI   PA7
 * IRQ      PC1
 * CLK  与单片机的晶振共用 */

#include "includes.h"

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
//void NVIC_Configuration(void);
void  USART_Configuration(void);
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data);

extern void  LD3320_main(void);
/***********************************************************
* 名    称： 主函数
* 功    能：	程序入口
* 入口参数：  无
* 出口参数：	无
* 说    明：	无
* 调用方法： 	无
**********************************************************/ 


int main(void)
{

	/* System Clocks Configuration */
	RCC_Configuration();  //外设时钟配置
	USART_Configuration();//USART 配置	
	LD3320_main();				//LD3320执行函数
	while (1)
	{

	}//while(1)
	
}
/***********************************************************
* 名    称： void  USART_Configuration(void)
* 功    能： 串口配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void  USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);	//类似与GPIO口，配置完后初始化一次
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //使能接收 中断 
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  //使能发送 中断 
	USART_Cmd(USART1, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         //USART1 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);		    //A端口 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //复用开漏输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //A端口 
}
/***********************************************************
* 名    称：void PrintCom(USART_TypeDef* USARTx, uint8_t *Data)
* 功    能：串口数据打印
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data)
{ 
	while(*Data)
	{
		USART_SendData(USARTx, *Data++);    /*发送单个数据 */
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);/* 检查指定的USART标志位即RESET＝1时发送完成*/
	}													
}
/***********************************************************
* 名    称：void RCC_Configuration(void)
* 功    能：配置外设时钟
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
//需修改  SystemInit();  

void RCC_Configuration(void)
{
//  SystemInit();  
  /* Enable USART1, GPIOA, GPIOx and AFIO clocks */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |\
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD ,ENABLE); //开启相关的AHP外设时钟
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

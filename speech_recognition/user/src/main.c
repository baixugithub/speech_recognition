/*************�˿���Ϣ********************
 * ����˵��
 * RST      PB6
 * CS   	PA4//ʹ��ld3320
 * WR/SPIS  PC4
 * P2/SDCK  PA5
 * P1/SDO   PA6
 * P0/SDI   PA7
 * IRQ      PC1
 * CLK  �뵥Ƭ���ľ����� */

#include "includes.h"

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
//void NVIC_Configuration(void);
void  USART_Configuration(void);
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data);

extern void  LD3320_main(void);
/***********************************************************
* ��    �ƣ� ������
* ��    �ܣ�	�������
* ��ڲ�����  ��
* ���ڲ�����	��
* ˵    ����	��
* ���÷����� 	��
**********************************************************/ 


int main(void)
{

	/* System Clocks Configuration */
	RCC_Configuration();  //����ʱ������
	USART_Configuration();//USART ����	
	LD3320_main();				//LD3320ִ�к���
	while (1)
	{

	}//while(1)
	
}
/***********************************************************
* ��    �ƣ� void  USART_Configuration(void)
* ��    �ܣ� ��������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
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
	USART_Init(USART1, &USART_InitStructure);	//������GPIO�ڣ���������ʼ��һ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //ʹ�ܽ��� �ж� 
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  //ʹ�ܷ��� �ж� 
	USART_Cmd(USART1, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         //USART1 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);		    //A�˿� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //���ÿ�©����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //A�˿� 
}
/***********************************************************
* ��    �ƣ�void PrintCom(USART_TypeDef* USARTx, uint8_t *Data)
* ��    �ܣ��������ݴ�ӡ
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void PrintCom(USART_TypeDef* USARTx, uint8_t *Data)
{ 
	while(*Data)
	{
		USART_SendData(USARTx, *Data++);    /*���͵������� */
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);/* ���ָ����USART��־λ��RESET��1ʱ�������*/
	}													
}
/***********************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ���������ʱ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
//���޸�  SystemInit();  

void RCC_Configuration(void)
{
//  SystemInit();  
  /* Enable USART1, GPIOA, GPIOx and AFIO clocks */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |\
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD ,ENABLE); //������ص�AHP����ʱ��
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

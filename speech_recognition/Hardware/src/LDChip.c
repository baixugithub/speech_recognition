/*******************************************************
**  �ļ����ƣ�����ģ��ײ�����
**	CPU: STM32f103RCT6
**	����8MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.4����ʶ�𿪷���
**  ˵���������� �߱�����ʶ�𡢴���ͨ�š����������������ʾ��
*/
#include "includes.h"
#include "stdio.h"
uint8 nLD_Mode = LD_MODE_IDLE;	 //������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3

uint8 ucRegVal;					//�ݴ��ж�������
extern uint8  nAsrStatus;		//�ݴ�һ��ʶ�����̽������ʶ����


/***********************************************************
* ��    �ƣ�void LD_reset(void)
* ��    �ܣ�LDоƬӲ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_reset(void)
{
	LD_RST_H();
	LD3320_delay(100);
	LD_RST_L();
	LD3320_delay(100);
	LD_RST_H();
	LD3320_delay(100);
	LD_CS_L();
	LD3320_delay(100);
	LD_CS_H();		
	LD3320_delay(100);

}

/***********************************************************
* ��    �ƣ� void LD_Init_Common(void)
* ��    �ܣ� ��ʼ������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_Init_Common(void)
{
	LD_ReadReg(0x06);			//FIFO״̬(ֻ��!)  
	LD_WriteReg(0x17, 0x35);	//д35H��LD3320����Soft Reset
	LD3320_delay(5);			//��ʱ
	LD_ReadReg(0x06);  			//FIFO״̬(ֻ��!)

	LD_WriteReg(0x89, 0x03);	//ģ���·���Ƴ�ʼʱд03H	  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   	//�ڲ�ʡ��ģʽ���ó�ʼ��ʱд43H
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);	//ASR����ȡASR�������4��
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);	//ʱ��Ƶ������1       
	if (nLD_Mode == LD_MODE_MP3)	//�ж��Ƿ�ΪMP3ģʽ
	{
		LD_WriteReg(0x1E, 0x00); 			//ADC ר�ÿ��ƣ�Ӧ��ʼ��Ϊ00H
		LD_WriteReg(0x19, LD_PLL_MP3_19);  //ʱ��Ƶ������2  
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);  //ʱ��Ƶ������3  
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);  //ʱ��Ƶ������4 
	}
	else
	{
		LD_WriteReg(0x1E,0x00);				//ADC ר�ÿ��ƣ�Ӧ��ʼ��Ϊ00H
		LD_WriteReg(0x19, LD_PLL_ASR_19);	//ʱ��Ƶ������2 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);	//ʱ��Ƶ������3	
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);	//ʱ��Ƶ������4

	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);	//DSP ��������ʼ��ʱд��04H ����DSP ����
	LD_WriteReg(0x17, 0x4c); 	//д4CH ����ʹDSP����,�Ƚ�ʡ��
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);	//ASR����ǰ���ʶ�����ַ������ȣ�ƴ���ַ����� 
								//��ʼ��ʱд��00H;ÿ���һ��ʶ����Ҫ�趨һ��		   
	LD_WriteReg(0xCF, 0x4F); 	//�ڲ�ʡ��ģʽ����;��ʼ��ʱ д��43H  
								//MP3 ��ʼ����ASR ��ʼ��ʱд�� 4FH  
	LD_WriteReg(0x6F, 0xFF); 	//��оƬ���г�ʼ��ʱ����Ϊ0xFF 
}

/***********************************************************
* ��    �ƣ�void LD_Init_ASR(void)
* ��    �ܣ���ʼ��ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 	
void LD_Init_ASR(void)
{
//	u8 s[20];
	nLD_Mode=LD_MODE_ASR_RUN;	//����ASRʶ��
	LD_Init_Common();			//����LD3320��ʼ��
//	PrintCom(USART1,"����LD3320��ʼ���ɹ�\r\n"); /*text.....*/

	LD_WriteReg(0xBD, 0x00);	//��ʼ���������Ĵ���;д��00HȻ������ΪASRģ��
							
	LD_WriteReg(0x17, 0x48);	//����DSP
//	sprintf((char *)s, "val  %d", LD_ReadReg(0x17));
//	PrintCom(USART1, s); /*text.....*/
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    //FIFO_EXT ���޵�8 λ��LowerBoundary L��						
	LD_WriteReg(0x3E, 0x07);	//FIFO_EXT ���޸�8 λ��LowerBoundary H��
	LD_WriteReg(0x38, 0xff);	//FIFO_EXT ���޵�8 λ��UpperBoundary L�� 
	LD_WriteReg(0x3A, 0x07);	//FIFO_EXT ���޸�8 λ��UpperBoundary H��

	LD_WriteReg(0x40, 0);		//FIFO_EXT MCU ˮ�ߵ�8 λ��MCU water mark L�� 
	LD_WriteReg(0x42, 8);	    //FIFO_EXT MCU ˮ�߸�8 λ��MCU water mark H�� 
	LD_WriteReg(0x44, 0);       //FIFO_EXT DSP ˮ�ߵ�8 λ��DSP water mark L�� 
	LD_WriteReg(0x46, 8); 	    //FIFO_EXT DSP ˮ�߸�8 λ��DSP water mark H��           

	LD3320_delay( 1 );
}

/***********************************************************
* ��    �ƣ�void ProcessInt0(void)
* ��    �ܣ�ʶ������
* ��ڲ�����  
* ���ڲ�����
* ˵    �����������ò�ѯ��ʽ�����жϷ�ʽ���е���
* ���÷����� 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);	/*	�ж������ţ��ɶ�д�� 
										��4λ����ȡֵΪ1��ʾ����ʶ���н��������MCU�����㡣 
										��2λ����ȡֵΪ1��ʾоƬ�ڲ�FIFO �жϷ�����
										MP3����ʱ������жϱ�־�����ⲿMCU��FIFO_DATA��Reload���ݡ� 
										��3λ����ȡֵΪ1��ʾоƬ�ڲ��Ѿ����ִ��� 
										ֵ��ע����ǣ�������ж���Ӧʱ������λΪ 1����Ҫ��о
										Ƭ��������Reset,�ſ��Լ���������
									*/
	PrintCom(USART1,"����������\r\n"); /*text.....*/

	// ����ʶ��������ж�
	// �����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
	LD_WriteReg(0x29,0) ;	//�����ж�λ���㡿�ж������ɶ�д�� 
							//��2 λ��FIFO �ж�����1 ��ʾ����0 ��ʾ������ 
							//��4 λ��ͬ���ж�����1 ��ʾ����0 ��ʾ������
	LD_WriteReg(0x02,0) ;	/*�����ж�λ���㡿FIFO �ж����� 
								��0 λ������FIFO_DATA �жϣ�
								��2 λ������FIFO_EXT �жϣ�*/ 
	/*	ASR��DSP æ��״̬;0x21 ��ʾ�У���ѯ��Ϊ��״̬���Խ�����һ��ASR����
		ASR��ASR ״̬����Ĵ���;������ֵΪ0x35������ȷ����һ������ʶ����������������
		�루0xb2���Ĵ�����0x21 ֵ���ʹ��
	*/
	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 				 
							
		nAsrResCount = LD_ReadReg(0xba);	//�жϸ�����Ϣ����������Ϊ00�� 
											//MP3�������ж�ʱ�� 
											//��5 λ=1 ��ʾ�������ѷ���MP3 �Ľ�β��	
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUNDOK; 		//��ʾһ��ʶ�����̽�������һ��ʶ����		
		}
		else
	    {
			nAsrStatus=LD_ASR_FOUNDZERO;	//��ʾһ��ʶ�����̽�����û��ʶ����
		}	
	}
	else
	{
		nAsrStatus=LD_ASR_FOUNDZERO;		//ִ��û��ʶ��
	}

	LD_WriteReg(0x2B, 0);	//���ж�����
	LD_WriteReg(0x1C, 0);	/*д0:ADC������*/
	
	LD_WriteReg(0x29, 0) ;	//���ж�����
	LD_WriteReg(0x02, 0) ;	//���ж�����
	LD_WriteReg(0x2B, 0);	//���ж�����
	LD_WriteReg(0xBA, 0);	//�жϸ�����Ϣ����������Ϊ00��
	LD_WriteReg(0xBC, 0);	//���ʶ�������ǿ�ƽ���λ
	LD_WriteReg(0x08, 1);	 /*���FIFO_DATA*/
	LD_WriteReg(0x08, 0);	/*���FIFO_DATA�� �ٴ�д0*/

	//LD3320_delay(1);

}

/***********************************************************
* ��    �ƣ�uint8 LD_Check_ASRBusyFlag_b2(void)
* ��    �ܣ���� ASR �Ƿ�æ״̬
* ��ڲ�����flag ---1������״̬  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		LD3320_delay(10);		
	}
	return flag;
}
/***********************************************************
* ��    �ƣ� void LD_AsrStart(void)
* ��    �ܣ�
* ��ڲ�����  ASR��ʼ��
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();

}
/***********************************************************
* ��    �ƣ� uint8 LD_AsrRun(void)
* ��    �ܣ� ASRִ�к���
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� ��������1Ϊ��������
* ���÷����� 
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);	//mic����67
	LD_WriteReg(0x1C, 0x09);	//ADC���ؿ���;д09H Reserve ����������
	LD_WriteReg(0xBD, 0x20);	//��ʼ�����ƼĴ���,д20H Reserve ����������
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);	//���ָ��FIFO,����д��һ��00H;�ж����Ѿ������д0,�˴��ٴ�д0
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)	//��� ASR �Ƿ�æ״̬[��1;æ0]
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	//���·�����ʶ���������ǰ����Ҫ���B2�Ĵ�����״̬	
	LD_WriteReg(0x37, 0x06);	//����ʶ����������·��Ĵ��� 
								//д06H��֪ͨDSP ��ʼʶ��������
								//д04H��֪ͨDSP Ҫ���һ��ʶ��䡣  							��
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);	//ADC���ؿ���:д0BH ��˷�����ADC ͨ������
	LD_WriteReg(0x29, 0x10);	//ͬ���ж�����
	
	LD_WriteReg(0xBD, 0x00);   	//��ʼ�����ƼĴ���,00HΪ����ASRģ��
	return 1;
}

/***********************************************************
* ��    �ƣ�uint8 LD_AsrAddFixed(void)
* ��    �ܣ����ʶ��ؼ�����
* ��ڲ�����  
* ���ڲ����� flag-- 1��success
* ˵    ���������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"��
						���������������մ�����÷������ʶ���ʡ�
* ���÷����� �޸Ĺؼ��ʺ�ʶ����
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 4    /*�����ά��ֵ,�û�������޸�*/
	#define DATE_B 20		/*����һά��ֵ,�û�������޸�*/
	uint8  sRecog[DATE_A][DATE_B] = {
										"kai deng",\
										"guan bi",\
										"an jian chu fa",\
										 "quan mie"\
										};	/*���u�u�u�u��u�*/
	 uint8  pCode[DATE_A] = {
	 															CODE_KD,\
																CODE_GD,\
																 CODE_AJCF,\
																  CODE_QM\
															};	/*���ʶ���룬�û��޸�*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k]);	//ASR��ʶ����Index��00H��FFH��
		LD_WriteReg(0xc3, 0);			//ASR��ʶ�������ʱд��00
		LD_WriteReg(0x08, 0x04);		//
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);		//���FIFO ���ݣ����ָ��FIFO ����д��һ��00H��
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);	//FIFO_EXT ���ݿ�
		}
		LD_WriteReg(0xb9, nAsrAddLength);	//ASR����ǰ���ʶ�����ַ������ȣ�ƴ���ַ����� 
												//ÿ���һ��ʶ����Ҫ�趨һ�Ρ�
		LD_WriteReg(0xb2, 0xff);		 //���·�����ʶ���������ǰ����Ҫ���B2�Ĵ�����״
		LD_WriteReg(0x37, 0x04);		//����ʶ����������·��Ĵ��� 
											//д04H��֪ͨDSP Ҫ���һ��ʶ���
//		LD_WriteReg(0x37, 0x04);		
	}	 									    			
    return flag;
}

/***********************************************************
* ��    �ƣ� uint8 LD_GetResult(void)
* ��    �ܣ� ��ȡʶ����
* ��ڲ�����  
* ���ڲ����� ����ʶ���� LD_ReadReg(0xc5 );
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5);	//ASR����ȡASR�������ѣ� 
}

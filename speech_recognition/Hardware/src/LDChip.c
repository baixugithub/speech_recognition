/*******************************************************
**  ÎÄ¼þÃû³Æ£ºÓïÒôÄ£¿éµ×²ãÇý¶¯
**	CPU: STM32f103RCT6
**	¾§Õñ£º8MHZ
**	²¨ÌØÂÊ£º9600 bit/S
**	ÅäÌ×²úÆ·ÐÅÏ¢£ºYS-V0.4ÓïÒôÊ¶±ð¿ª·¢°å
**  ËµÃ÷£º±¾³ÌÐò ¾ß±¸ÓïÒôÊ¶±ð¡¢´®¿ÚÍ¨ÐÅ¡¢¿ª·¢°å»ù±¾¹¦ÄÜÑÝÊ¾¡£
*/
#include "includes.h"
#include "stdio.h"
uint8 nLD_Mode = LD_MODE_IDLE;	 //ÓÃÀ´¼ÇÂ¼µ±Ç°ÊÇÔÚ½øÐÐASRÊ¶±ð»¹ÊÇÔÚ²¥·ÅMP3

uint8 ucRegVal;					//ÔÝ´æÖÐ¶ÏÇëÇó±àºÅ
extern uint8  nAsrStatus;		//ÔÝ´æÒ»´ÎÊ¶±ðÁ÷³Ì½áÊøºóµÄÊ¶±ð½á¹û


/***********************************************************
* Ãû    ³Æ£ºvoid LD_reset(void)
* ¹¦    ÄÜ£ºLDÐ¾Æ¬Ó²¼þ³õÊ¼»¯
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
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
* Ãû    ³Æ£º void LD_Init_Common(void)
* ¹¦    ÄÜ£º ³õÊ¼»¯ÃüÁî
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 
void LD_Init_Common(void)
{
	LD_ReadReg(0x06);			//FIFO×´Ì¬(Ö»¶Á!)  
	LD_WriteReg(0x17, 0x35);	//Ð´35H¶ÔLD3320½øÐÐSoft Reset
	LD3320_delay(5);			//ÑÓÊ±
	LD_ReadReg(0x06);  			//FIFO×´Ì¬(Ö»¶Á!)

	LD_WriteReg(0x89, 0x03);	//Ä£ÄâµçÂ·¿ØÖÆ³õÊ¼Ê±Ð´03H	  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   	//ÄÚ²¿Ê¡µçÄ£Ê½ÉèÖÃ³õÊ¼»¯Ê±Ð´43H
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);	//ASR£º¶ÁÈ¡ASR½á¹û£¨ºò²¹4£©
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);	//Ê±ÖÓÆµÂÊÉèÖÃ1       
	if (nLD_Mode == LD_MODE_MP3)	//ÅÐ¶ÏÊÇ·ñÎªMP3Ä£Ê½
	{
		LD_WriteReg(0x1E, 0x00); 			//ADC ×¨ÓÃ¿ØÖÆ£¬Ó¦³õÊ¼»¯Îª00H
		LD_WriteReg(0x19, LD_PLL_MP3_19);  //Ê±ÖÓÆµÂÊÉèÖÃ2  
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);  //Ê±ÖÓÆµÂÊÉèÖÃ3  
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);  //Ê±ÖÓÆµÂÊÉèÖÃ4 
	}
	else
	{
		LD_WriteReg(0x1E,0x00);				//ADC ×¨ÓÃ¿ØÖÆ£¬Ó¦³õÊ¼»¯Îª00H
		LD_WriteReg(0x19, LD_PLL_ASR_19);	//Ê±ÖÓÆµÂÊÉèÖÃ2 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);	//Ê±ÖÓÆµÂÊÉèÖÃ3	
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);	//Ê±ÖÓÆµÂÊÉèÖÃ4

	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);	//DSP ÐÝÃßÉèÖÃÊ¼»¯Ê±Ð´Èë04H ÔÊÐíDSP ÐÝÃß
	LD_WriteReg(0x17, 0x4c); 	//Ð´4CH ¿ÉÒÔÊ¹DSPÐÝÃß,±È½ÏÊ¡µç
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);	//ASR£ºµ±Ç°Ìí¼ÓÊ¶±ð¾äµÄ×Ö·û´®³¤¶È£¨Æ´Òô×Ö·û´®£© 
								//³õÊ¼»¯Ê±Ð´Èë00H;Ã¿Ìí¼ÓÒ»ÌõÊ¶±ð¾äºóÒªÉè¶¨Ò»´Î		   
	LD_WriteReg(0xCF, 0x4F); 	//ÄÚ²¿Ê¡µçÄ£Ê½ÉèÖÃ;³õÊ¼»¯Ê± Ð´Èë43H  
								//MP3 ³õÊ¼»¯ºÍASR ³õÊ¼»¯Ê±Ð´Èë 4FH  
	LD_WriteReg(0x6F, 0xFF); 	//¶ÔÐ¾Æ¬½øÐÐ³õÊ¼»¯Ê±ÉèÖÃÎª0xFF 
}

/***********************************************************
* Ãû    ³Æ£ºvoid LD_Init_ASR(void)
* ¹¦    ÄÜ£º³õÊ¼»¯ASR
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 	
void LD_Init_ASR(void)
{
//	u8 s[20];
	nLD_Mode=LD_MODE_ASR_RUN;	//ÔËÐÐASRÊ¶±ð
	LD_Init_Common();			//µ÷ÓÃLD3320³õÊ¼»¯
//	PrintCom(USART1,"µ÷ÓÃLD3320³õÊ¼»¯³É¹¦\r\n"); /*text.....*/

	LD_WriteReg(0xBD, 0x00);	//³õÊ¼»¯¿ØÖÆÆ÷¼Ä´æÆ÷;Ð´Èë00HÈ»ºóÆô¶¯ÎªASRÄ£¿é
							
	LD_WriteReg(0x17, 0x48);	//¼¤»îDSP
//	sprintf((char *)s, "val  %d", LD_ReadReg(0x17));
//	PrintCom(USART1, s); /*text.....*/
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    //FIFO_EXT ÏÂÏÞµÍ8 Î»£¨LowerBoundary L£©						
	LD_WriteReg(0x3E, 0x07);	//FIFO_EXT ÏÂÏÞ¸ß8 Î»£¨LowerBoundary H£©
	LD_WriteReg(0x38, 0xff);	//FIFO_EXT ÉÏÏÞµÍ8 Î»£¨UpperBoundary L£© 
	LD_WriteReg(0x3A, 0x07);	//FIFO_EXT ÉÏÏÞ¸ß8 Î»£¨UpperBoundary H£©

	LD_WriteReg(0x40, 0);		//FIFO_EXT MCU Ë®ÏßµÍ8 Î»£¨MCU water mark L£© 
	LD_WriteReg(0x42, 8);	    //FIFO_EXT MCU Ë®Ïß¸ß8 Î»£¨MCU water mark H£© 
	LD_WriteReg(0x44, 0);       //FIFO_EXT DSP Ë®ÏßµÍ8 Î»£¨DSP water mark L£© 
	LD_WriteReg(0x46, 8); 	    //FIFO_EXT DSP Ë®Ïß¸ß8 Î»£¨DSP water mark H£©           

	LD3320_delay( 1 );
}

/***********************************************************
* Ãû    ³Æ£ºvoid ProcessInt0(void)
* ¹¦    ÄÜ£ºÊ¶±ð´¦Àíº¯Êý
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º¿ÉÒÔÀûÓÃ²éÑ¯·½Ê½£¬»òÖÐ¶Ï·½Ê½½øÐÐµ÷ÓÃ
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);	/*	ÖÐ¶ÏÇëÇó±àºÅ£¨¿É¶ÁÐ´£© 
										µÚ4Î»£º¶ÁÈ¡ÖµÎª1±íÊ¾ÓïÒôÊ¶±ðÓÐ½á¹û²úÉú£»MCU¿ÉÇåÁã¡£ 
										µÚ2Î»£º¶ÁÈ¡ÖµÎª1±íÊ¾Ð¾Æ¬ÄÚ²¿FIFO ÖÐ¶Ï·¢Éú¡£
										MP3²¥·ÅÊ±»á²úÉúÖÐ¶Ï±êÖ¾ÇëÇóÍâ²¿MCUÏòFIFO_DATAÖÐReloadÊý¾Ý¡£ 
										µÚ3Î»£º¶ÁÈ¡ÖµÎª1±íÊ¾Ð¾Æ¬ÄÚ²¿ÒÑ¾­³öÏÖ´íÎó¡£ 
										ÖµµÃ×¢ÒâµÄÊÇ£ºÈç¹ûÔÚÖÐ¶ÏÏìÓ¦Ê±¶Áµ½ÕâÎ»Îª 1£¬ÐèÒª¶ÔÐ¾
										Æ¬½øÐÐÖØÆôReset,²Å¿ÉÒÔ¼ÌÐø¹¤×÷¡£
									*/
	PrintCom(USART1,"ÓÐÉùÒôÊäÈë\r\n"); /*text.....*/

	// ÓïÒôÊ¶±ð²úÉúµÄÖÐ¶Ï
	// £¨ÓÐÉùÒôÊäÈë£¬²»ÂÛÊ¶±ð³É¹¦»òÊ§°Ü¶¼ÓÐÖÐ¶Ï£©
	LD_WriteReg(0x29,0) ;	//¡¾½«ÖÐ¶ÏÎ»ÖÃÁã¡¿ÖÐ¶ÏÔÊÐí£¨¿É¶ÁÐ´£© 
							//µÚ2 Î»£ºFIFO ÖÐ¶ÏÔÊÐí£¬1 ±íÊ¾ÔÊÐí£»0 ±íÊ¾²»ÔÊÐí¡£ 
							//µÚ4 Î»£ºÍ¬²½ÖÐ¶ÏÔÊÐí£¬1 ±íÊ¾ÔÊÐí£»0 ±íÊ¾²»ÔÊÐí
	LD_WriteReg(0x02,0) ;	/*¡¾½«ÖÐ¶ÏÎ»ÖÃÁã¡¿FIFO ÖÐ¶ÏÔÊÐí 
								µÚ0 Î»£ºÔÊÐíFIFO_DATA ÖÐ¶Ï£»
								µÚ2 Î»£ºÔÊÐíFIFO_EXT ÖÐ¶Ï£»*/ 
	/*	ASR£ºDSP Ã¦ÏÐ×´Ì¬;0x21 ±íÊ¾ÏÐ£¬²éÑ¯µ½ÎªÏÐ×´Ì¬¿ÉÒÔ½øÐÐÏÂÒ»²½ASR¶¯×÷
		ASR£ºASR ×´Ì¬±¨¸æ¼Ä´æÆ÷;¶Áµ½ÊýÖµÎª0x35£¬¿ÉÒÔÈ·¶¨ÊÇÒ»´ÎÓïÒôÊ¶±ðÁ÷³ÌÕý³£½áÊø£¬
		Óë£¨0xb2£©¼Ä´æÆ÷µÄ0x21 ÖµÅäºÏÊ¹ÓÃ
	*/
	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 				 
							
		nAsrResCount = LD_ReadReg(0xba);	//ÖÐ¶Ï¸¨ÖúÐÅÏ¢£¬£¨¶Á»òÉèÎª00£© 
											//MP3£º²¥·ÅÖÐ¶ÏÊ±£¬ 
											//µÚ5 Î»=1 ±íÊ¾²¥·ÅÆ÷ÒÑ·¢ÏÖMP3 µÄ½áÎ²¡£	
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUNDOK; 		//±íÊ¾Ò»´ÎÊ¶±ðÁ÷³Ì½áÊøºó£¬ÓÐÒ»¸öÊ¶±ð½á¹û		
		}
		else
	    {
			nAsrStatus=LD_ASR_FOUNDZERO;	//±íÊ¾Ò»´ÎÊ¶±ðÁ÷³Ì½áÊøºó£¬Ã»ÓÐÊ¶±ð½á¹û
		}	
	}
	else
	{
		nAsrStatus=LD_ASR_FOUNDZERO;		//Ö´ÐÐÃ»ÓÐÊ¶±ð
	}

	LD_WriteReg(0x2B, 0);	//½«ÖÐ¶ÏÖÃÁã
	LD_WriteReg(0x1C, 0);	/*Ð´0:ADC²»¿ÉÓÃ*/
	
	LD_WriteReg(0x29, 0) ;	//½«ÖÐ¶ÏÖÃÁã
	LD_WriteReg(0x02, 0) ;	//½«ÖÐ¶ÏÖÃÁã
	LD_WriteReg(0x2B, 0);	//½«ÖÐ¶ÏÖÃÁã
	LD_WriteReg(0xBA, 0);	//ÖÐ¶Ï¸¨ÖúÐÅÏ¢£¬£¨¶Á»òÉèÎª00£©
	LD_WriteReg(0xBC, 0);	//Çå³ýÊ¶±ð¹ý³ÌÖÐÇ¿ÖÆ½áÊøÎ»
	LD_WriteReg(0x08, 1);	 /*Çå³ýFIFO_DATA*/
	LD_WriteReg(0x08, 0);	/*Çå³ýFIFO_DATAºó ÔÙ´ÎÐ´0*/

	//LD3320_delay(1);

}

/***********************************************************
* Ãû    ³Æ£ºuint8 LD_Check_ASRBusyFlag_b2(void)
* ¹¦    ÄÜ£º¼ì²â ASR ÊÇ·ñÃ¦×´Ì¬
* Èë¿Ú²ÎÊý£ºflag ---1£º¿ÕÏÐ×´Ì¬  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
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
* Ãû    ³Æ£º void LD_AsrStart(void)
* ¹¦    ÄÜ£º
* Èë¿Ú²ÎÊý£º  ASR³õÊ¼»¯
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();

}
/***********************************************************
* Ãû    ³Æ£º uint8 LD_AsrRun(void)
* ¹¦    ÄÜ£º ASRÖ´ÐÐº¯Êý
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º
* Ëµ    Ã÷£º º¯Êý·µ»Ø1ÎªÕý³£Æô¶¯
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);	//micÒôÁ¿67
	LD_WriteReg(0x1C, 0x09);	//ADC¿ª¹Ø¿ØÖÆ;Ð´09H Reserve ±£ÁôÃüÁî×Ö
	LD_WriteReg(0xBD, 0x20);	//³õÊ¼»¯¿ØÖÆ¼Ä´æÆ÷,Ð´20H Reserve ±£ÁôÃüÁî×Ö
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);	//Çå³ýÖ¸¶¨FIFO,ºóÔÙÐ´ÈëÒ»´Î00H;ÖÐ¶ÏÄÚÒÑ¾­Çå³ý²¢Ð´0,´Ë´¦ÔÙ´ÎÐ´0
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)	//¼ì²â ASR ÊÇ·ñÃ¦×´Ì¬[ÏÐ1;Ã¦0]
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	//ÔÚÏÂ·¢ÓïÒôÊ¶±ð¿ØÖÆÃüÁîÇ°£¬ÐèÒª¼ì²éB2¼Ä´æÆ÷µÄ×´Ì¬	
	LD_WriteReg(0x37, 0x06);	//ÓïÒôÊ¶±ð¿ØÖÆÃüÁîÏÂ·¢¼Ä´æÆ÷ 
								//Ð´06H£ºÍ¨ÖªDSP ¿ªÊ¼Ê¶±ðÓïÒô¡£
								//Ð´04H£ºÍ¨ÖªDSP ÒªÌí¼ÓÒ»ÏîÊ¶±ð¾ä¡£  							¡£
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);	//ADC¿ª¹Ø¿ØÖÆ:Ð´0BH Âó¿Ë·çÊäÈëADC Í¨µÀ¿ÉÓÃ
	LD_WriteReg(0x29, 0x10);	//Í¬²½ÖÐ¶ÏÔÊÐí
	
	LD_WriteReg(0xBD, 0x00);   	//³õÊ¼»¯¿ØÖÆ¼Ä´æÆ÷,00HÎªÆô¶¯ASRÄ£¿é
	return 1;
}

/***********************************************************
* Ãû    ³Æ£ºuint8 LD_AsrAddFixed(void)
* ¹¦    ÄÜ£ºÌí¼ÓÊ¶±ð¹Ø¼ü´ÊÓï
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º flag-- 1£ºsuccess
* Ëµ    Ã÷£º¿ª·¢Õß¿ÉÒÔÑ§Ï°"ÓïÒôÊ¶±ðÐ¾Æ¬LD3320¸ß½×ÃØ¼®.pdf"ÖÐ
						¹ØÓÚÀ¬»ø´ÊÓïÎüÊÕ´íÎóµÄÓÃ·¨£¬Ìá¸ßÊ¶±ðÂÊ¡£
* µ÷ÓÃ·½·¨£º ÐÞ¸Ä¹Ø¼ü´ÊºÍÊ¶±ðÂë
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 4    /*Êý×é¶þÎ¬ÊýÖµ,ÓÃ»§Ìí¼ÓÐèÐÞ¸Ä*/
	#define DATE_B 20		/*Êý×éÒ»Î¬ÊýÖµ,ÓÃ»§Ìí¼ÓÐèÐÞ¸Ä*/
	uint8  sRecog[DATE_A][DATE_B] = {
										"kai deng",\
										"guan bi",\
										"an jian chu fa",\
										 "quan mie"\
										};	/*¼ÓØu´u¬u»uÐÞuÄ*/
	 uint8  pCode[DATE_A] = {
	 															CODE_KD,\
																CODE_GD,\
																 CODE_AJCF,\
																  CODE_QM\
															};	/*Ìí¼ÓÊ¶±ðÂë£¬ÓÃ»§ÐÞ¸Ä*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k]);	//ASR£ºÊ¶±ð×ÖIndex£¨00H¡ªFFH£©
		LD_WriteReg(0xc3, 0);			//ASR£ºÊ¶±ð×ÖÌí¼ÓÊ±Ð´Èë00
		LD_WriteReg(0x08, 0x04);		//
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);		//Çå³ýFIFO ÄÚÈÝ£¨Çå³ýÖ¸¶¨FIFO ºóÔÙÐ´ÈëÒ»´Î00H£©
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);	//FIFO_EXT Êý¾Ý¿Ú
		}
		LD_WriteReg(0xb9, nAsrAddLength);	//ASR£ºµ±Ç°Ìí¼ÓÊ¶±ð¾äµÄ×Ö·û´®³¤¶È£¨Æ´Òô×Ö·û´®£© 
												//Ã¿Ìí¼ÓÒ»ÌõÊ¶±ð¾äºóÒªÉè¶¨Ò»´Î¡£
		LD_WriteReg(0xb2, 0xff);		 //ÔÚÏÂ·¢ÓïÒôÊ¶±ð¿ØÖÆÃüÁîÇ°£¬ÐèÒª¼ì²éB2¼Ä´æÆ÷µÄ×´
		LD_WriteReg(0x37, 0x04);		//ÓïÒôÊ¶±ð¿ØÖÆÃüÁîÏÂ·¢¼Ä´æÆ÷ 
											//Ð´04H£ºÍ¨ÖªDSP ÒªÌí¼ÓÒ»ÏîÊ¶±ð¾ä
//		LD_WriteReg(0x37, 0x04);		
	}	 									    			
    return flag;
}

/***********************************************************
* Ãû    ³Æ£º uint8 LD_GetResult(void)
* ¹¦    ÄÜ£º »ñÈ¡Ê¶±ð½á¹û
* Èë¿Ú²ÎÊý£º  
* ³ö¿Ú²ÎÊý£º ·µ»ØÊ¶±ðÂë LD_ReadReg(0xc5 );
* Ëµ    Ã÷£º
* µ÷ÓÃ·½·¨£º 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5);	//ASR£º¶ÁÈ¡ASR½á¹û£¨×î¼Ñ£© 
}

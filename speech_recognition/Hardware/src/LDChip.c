/*******************************************************
**  文件名称：语音模块底层驱动
**	CPU: STM32f103RCT6
**	晶振：8MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.4语音识别开发板
**  说明：本程序 具备语音识别、串口通信、开发板基本功能演示。
*/
#include "includes.h"
#include "stdio.h"
uint8 nLD_Mode = LD_MODE_IDLE;	 //用来记录当前是在进行ASR识别还是在播放MP3

uint8 ucRegVal;					//暂存中断请求编号
extern uint8  nAsrStatus;		//暂存一次识别流程结束后的识别结果


/***********************************************************
* 名    称：void LD_reset(void)
* 功    能：LD芯片硬件初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
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
* 名    称： void LD_Init_Common(void)
* 功    能： 初始化命令
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_Init_Common(void)
{
	LD_ReadReg(0x06);			//FIFO状态(只读!)  
	LD_WriteReg(0x17, 0x35);	//写35H对LD3320进行Soft Reset
	LD3320_delay(5);			//延时
	LD_ReadReg(0x06);  			//FIFO状态(只读!)

	LD_WriteReg(0x89, 0x03);	//模拟电路控制初始时写03H	  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   	//内部省电模式设置初始化时写43H
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);	//ASR：读取ASR结果（候补4）
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);	//时钟频率设置1       
	if (nLD_Mode == LD_MODE_MP3)	//判断是否为MP3模式
	{
		LD_WriteReg(0x1E, 0x00); 			//ADC 专用控制，应初始化为00H
		LD_WriteReg(0x19, LD_PLL_MP3_19);  //时钟频率设置2  
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);  //时钟频率设置3  
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);  //时钟频率设置4 
	}
	else
	{
		LD_WriteReg(0x1E,0x00);				//ADC 专用控制，应初始化为00H
		LD_WriteReg(0x19, LD_PLL_ASR_19);	//时钟频率设置2 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);	//时钟频率设置3	
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);	//时钟频率设置4

	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);	//DSP 休眠设置始化时写入04H 允许DSP 休眠
	LD_WriteReg(0x17, 0x4c); 	//写4CH 可以使DSP休眠,比较省电
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);	//ASR：当前添加识别句的字符串长度（拼音字符串） 
								//初始化时写入00H;每添加一条识别句后要设定一次		   
	LD_WriteReg(0xCF, 0x4F); 	//内部省电模式设置;初始化时 写入43H  
								//MP3 初始化和ASR 初始化时写入 4FH  
	LD_WriteReg(0x6F, 0xFF); 	//对芯片进行初始化时设置为0xFF 
}

/***********************************************************
* 名    称：void LD_Init_ASR(void)
* 功    能：初始化ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 	
void LD_Init_ASR(void)
{
//	u8 s[20];
	nLD_Mode=LD_MODE_ASR_RUN;	//运行ASR识别
	LD_Init_Common();			//调用LD3320初始化
//	PrintCom(USART1,"调用LD3320初始化成功\r\n"); /*text.....*/

	LD_WriteReg(0xBD, 0x00);	//初始化控制器寄存器;写入00H然后启动为ASR模块
							
	LD_WriteReg(0x17, 0x48);	//激活DSP
//	sprintf((char *)s, "val  %d", LD_ReadReg(0x17));
//	PrintCom(USART1, s); /*text.....*/
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    //FIFO_EXT 下限低8 位（LowerBoundary L）						
	LD_WriteReg(0x3E, 0x07);	//FIFO_EXT 下限高8 位（LowerBoundary H）
	LD_WriteReg(0x38, 0xff);	//FIFO_EXT 上限低8 位（UpperBoundary L） 
	LD_WriteReg(0x3A, 0x07);	//FIFO_EXT 上限高8 位（UpperBoundary H）

	LD_WriteReg(0x40, 0);		//FIFO_EXT MCU 水线低8 位（MCU water mark L） 
	LD_WriteReg(0x42, 8);	    //FIFO_EXT MCU 水线高8 位（MCU water mark H） 
	LD_WriteReg(0x44, 0);       //FIFO_EXT DSP 水线低8 位（DSP water mark L） 
	LD_WriteReg(0x46, 8); 	    //FIFO_EXT DSP 水线高8 位（DSP water mark H）           

	LD3320_delay( 1 );
}

/***********************************************************
* 名    称：void ProcessInt0(void)
* 功    能：识别处理函数
* 入口参数：  
* 出口参数：
* 说    明：可以利用查询方式，或中断方式进行调用
* 调用方法： 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);	/*	中断请求编号（可读写） 
										第4位：读取值为1表示语音识别有结果产生；MCU可清零。 
										第2位：读取值为1表示芯片内部FIFO 中断发生。
										MP3播放时会产生中断标志请求外部MCU向FIFO_DATA中Reload数据。 
										第3位：读取值为1表示芯片内部已经出现错误。 
										值得注意的是：如果在中断响应时读到这位为 1，需要对芯
										片进行重启Reset,才可以继续工作。
									*/
	PrintCom(USART1,"有声音输入\r\n"); /*text.....*/

	// 语音识别产生的中断
	// （有声音输入，不论识别成功或失败都有中断）
	LD_WriteReg(0x29,0) ;	//【将中断位置零】中断允许（可读写） 
							//第2 位：FIFO 中断允许，1 表示允许；0 表示不允许。 
							//第4 位：同步中断允许，1 表示允许；0 表示不允许
	LD_WriteReg(0x02,0) ;	/*【将中断位置零】FIFO 中断允许 
								第0 位：允许FIFO_DATA 中断；
								第2 位：允许FIFO_EXT 中断；*/ 
	/*	ASR：DSP 忙闲状态;0x21 表示闲，查询到为闲状态可以进行下一步ASR动作
		ASR：ASR 状态报告寄存器;读到数值为0x35，可以确定是一次语音识别流程正常结束，
		与（0xb2）寄存器的0x21 值配合使用
	*/
	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 				 
							
		nAsrResCount = LD_ReadReg(0xba);	//中断辅助信息，（读或设为00） 
											//MP3：播放中断时， 
											//第5 位=1 表示播放器已发现MP3 的结尾。	
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUNDOK; 		//表示一次识别流程结束后，有一个识别结果		
		}
		else
	    {
			nAsrStatus=LD_ASR_FOUNDZERO;	//表示一次识别流程结束后，没有识别结果
		}	
	}
	else
	{
		nAsrStatus=LD_ASR_FOUNDZERO;		//执行没有识别
	}

	LD_WriteReg(0x2B, 0);	//将中断置零
	LD_WriteReg(0x1C, 0);	/*写0:ADC不可用*/
	
	LD_WriteReg(0x29, 0) ;	//将中断置零
	LD_WriteReg(0x02, 0) ;	//将中断置零
	LD_WriteReg(0x2B, 0);	//将中断置零
	LD_WriteReg(0xBA, 0);	//中断辅助信息，（读或设为00）
	LD_WriteReg(0xBC, 0);	//清除识别过程中强制结束位
	LD_WriteReg(0x08, 1);	 /*清除FIFO_DATA*/
	LD_WriteReg(0x08, 0);	/*清除FIFO_DATA后 再次写0*/

	//LD3320_delay(1);

}

/***********************************************************
* 名    称：uint8 LD_Check_ASRBusyFlag_b2(void)
* 功    能：检测 ASR 是否忙状态
* 入口参数：flag ---1：空闲状态  
* 出口参数：
* 说    明：
* 调用方法： 
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
* 名    称： void LD_AsrStart(void)
* 功    能：
* 入口参数：  ASR初始化
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();

}
/***********************************************************
* 名    称： uint8 LD_AsrRun(void)
* 功    能： ASR执行函数
* 入口参数：  
* 出口参数：
* 说    明： 函数返回1为正常启动
* 调用方法： 
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);	//mic音量67
	LD_WriteReg(0x1C, 0x09);	//ADC开关控制;写09H Reserve 保留命令字
	LD_WriteReg(0xBD, 0x20);	//初始化控制寄存器,写20H Reserve 保留命令字
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);	//清除指定FIFO,后再写入一次00H;中断内已经清除并写0,此处再次写0
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)	//检测 ASR 是否忙状态[闲1;忙0]
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	//在下发语音识别控制命令前，需要检查B2寄存器的状态	
	LD_WriteReg(0x37, 0x06);	//语音识别控制命令下发寄存器 
								//写06H：通知DSP 开始识别语音。
								//写04H：通知DSP 要添加一项识别句。  							。
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);	//ADC开关控制:写0BH 麦克风输入ADC 通道可用
	LD_WriteReg(0x29, 0x10);	//同步中断允许
	
	LD_WriteReg(0xBD, 0x00);   	//初始化控制寄存器,00H为启动ASR模块
	return 1;
}

/***********************************************************
* 名    称：uint8 LD_AsrAddFixed(void)
* 功    能：添加识别关键词语
* 入口参数：  
* 出口参数： flag-- 1：success
* 说    明：开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
						关于垃圾词语吸收错误的用法，提高识别率。
* 调用方法： 修改关键词和识别码
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 4    /*数组二维数值,用户添加需修改*/
	#define DATE_B 20		/*数组一维数值,用户添加需修改*/
	uint8  sRecog[DATE_A][DATE_B] = {
										"kai deng",\
										"guan bi",\
										"an jian chu fa",\
										 "quan mie"\
										};	/*加豼磚瑄籾修u�*/
	 uint8  pCode[DATE_A] = {
	 															CODE_KD,\
																CODE_GD,\
																 CODE_AJCF,\
																  CODE_QM\
															};	/*添加识别码，用户修改*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k]);	//ASR：识别字Index（00H—FFH）
		LD_WriteReg(0xc3, 0);			//ASR：识别字添加时写入00
		LD_WriteReg(0x08, 0x04);		//
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);		//清除FIFO 内容（清除指定FIFO 后再写入一次00H）
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);	//FIFO_EXT 数据口
		}
		LD_WriteReg(0xb9, nAsrAddLength);	//ASR：当前添加识别句的字符串长度（拼音字符串） 
												//每添加一条识别句后要设定一次。
		LD_WriteReg(0xb2, 0xff);		 //在下发语音识别控制命令前，需要检查B2寄存器的状
		LD_WriteReg(0x37, 0x04);		//语音识别控制命令下发寄存器 
											//写04H：通知DSP 要添加一项识别句
//		LD_WriteReg(0x37, 0x04);		
	}	 									    			
    return flag;
}

/***********************************************************
* 名    称： uint8 LD_GetResult(void)
* 功    能： 获取识别结果
* 入口参数：  
* 出口参数： 返回识别码 LD_ReadReg(0xc5 );
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5);	//ASR：读取ASR结果（最佳） 
}

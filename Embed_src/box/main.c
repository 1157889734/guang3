
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


#include	"config.h"
#include	"timer.h"
#include	"USART.h"
#include	"delay.h"
#include "stat_switch.h"
#include<main.h>

/*************	功能说明	**************

双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************	本地常量声明	**************/
char code version_buf1[] = "u1DATE:2014-01-07\r\nV1.0\r\n" ;
char code version_buf2[] = "u2DATE:2014-01-07\r\nV1.0\r\n" ;
/*************	本地变量声明	**************/


/*************	本地函数声明	**************/
void delay10ms (byte count)     /*延时10ms子程序 实际延时时间9.644ms*/
{
   byte i,j;   
   while(count--!=0)
   {
      for(i=0;i<15;i++)
      {
         for(j=0;j<72;j++)
         	;
         	
       }
       //RS485_Com_OP();
       _nop_();
   }
} 

/************************ 定时器配置 ****************************/
void	Timer_config(void)
{

	TR0 = 0;
	ET0 = 1;
	PT0 = 1;
	TMOD &=~0x03;
	AUXR |= 0x80;
	TMOD &=~0x04;

	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
	
	#if 0
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = DISABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 100000UL);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	if(Timer_Inilize(Timer0,&TIM_InitStructure))
		while (1);				//初始化Timer0	  Timer0,Timer1,Timer2
      /*
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//初始化Timer1	  Timer0,Timer1,Timer2

	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (50*12));		//初值
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2
	*/

	/*
	AUXR |= 0x80;
	//AUXR &= ~0x80;
	TMOD = 0x00;

	//TMOD &=~0x04;

	TL0 = DELAY_10MS_L;
	TH0 = DELAY_10MS_H;
	//TL0	= (65536 - MAIN_Fosc /2/38400);
	//TH0 = (65536 - MAIN_Fosc /2/38400)>>8;
	TR0 = 1;
	INT_CLKO = 0x01;
	*/
	
	#endif
}

/*************  外部函数和变量声明 *****************/

void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 9600ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 9600ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2

	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1发送一个字符串
	//PrintString2("STC15F2K60S2 UART2 Test Prgramme!\r\n");	//SUART2发送一个字符串
}


/**********************************************/
void main(void)
{
	
	Timer_config();	
	UART_config();	
	EA = 1;
			
	delay10ms(1);
	LedAllOpen();
	delay10ms(400);
	BoxDataInit();
	LedInit();
	TalkStatInit();
	MIC_CTRL_PIN = 1;
	BUZZER_CRTL_PIN = 1;
	//putnchar(version_buf, 23);

	Uart2StartSend(version_buf2, 23);
	Uart1StartSend(version_buf1, 23);


	while(1)
	{
		LedRun();
		BoxDataProc();
		LcdDataProc();
		ScanKey();
		//TalkStatSwitch();		
		WDT_CONTR = 0x3b;
	}
}




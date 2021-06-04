
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

#ifndef	__TIMER_H
#define	__TIMER_H

#include	"config.h"
#include <general.h>

#define	Timer0						0
#define	Timer1						1
#define	Timer2						2
#define	Timer3						3
#define	Timer4						4

#define	TIM_16BitAutoReload			0
#define	TIM_16Bit					1
#define	TIM_8BitAutoReload			2
#define	TIM_16BitAutoReloadNoMask	3

#define	TIM_CLOCK_1T				0
#define	TIM_CLOCK_12T				1
#define	TIM_CLOCK_Ext				2


#define TIMER_DISENABLED 0x00
#define TIMER_OK 0x01

#define TIMER_DISENABLED_WORD 0x0000
#define TIMER_OK_WORD 0x0001

#define DELAY_10MS_H 0xd8
#define DELAY_10MS_L 0xef
#define TIME_COM_SEND_OVER 0x42
#define TIME_ESU_CALL_FLASH 0x1b
#define TIME_MIC_REQ_FLASH 0x10
#define TIME_CALL_REQ_FLASH 0x10
#define TIME_ERR_FLASH 0x40
#define TIME_SALVE_ESU_ACK_SHOW 0x80



extern unsigned long gSys10msTick;
extern unsigned long gPcLedTick;
extern unsigned long gCcLedTick;
extern unsigned long gPaLedTick;
extern unsigned long gRunLedTick;

typedef struct
{
	u8	TIM_Mode;		//工作模式,  	TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	u8	TIM_Polity;		//优先级设置	PolityHigh,PolityLow
	u8	TIM_Interrupt;	//中断允许		ENABLE,DISABLE
	u8	TIM_ClkSource;	//时钟源		TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	u8	TIM_ClkOut;		//可编程时钟输出,	ENABLE,DISABLE
	u16	TIM_Value;		//装载初值
	u8	TIM_Run;		//是否运行		ENABLE,DISABLE
} TIM_InitTypeDef;

u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx);

#endif

/*
*******************************************************************************
**  Copyright (c) 2014, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 
**  创建日期: 2014.01.03
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/


#define LED_WORK			(0X0001)
#define LED_MAIN			(0X0002)
#define LED_PA				(0X0004)
#define LED_PTT				(0X0008)
#define LED_CC				(0X0010)
#define LED_M				(0X0020)
#define LED_PC				(0X0040)
#define LED_SP				(0X0080)
#define LED_PC_RST				(0X1000)
#define LED_SP_L				(0X2000)
#define LED_SP_M			(0X4000)
#define LED_SP_S			(0X8000)

#define LED_OPEN				1
#define LED_CLOSE 				0

typedef enum{
	LED_PC_FLASH = 1,
	LED_PC_OPEN = 2,
	LED_PC_CLOSE = 3,
}eLedPcStat;

extern void LedRun(void);

extern void LedInit(void);
extern void SetPcStat(eLedPcStat stat);
extern void SetPaStat(eLedPcStat stat);

extern void SetCcStat(eLedPcStat stat);

extern void LedAllOpen(void);
extern void LedSwitch( unsigned short type, char stat);



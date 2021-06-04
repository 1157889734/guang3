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
#include<main.h>


eLedPcStat gFlashPcLed;  
eLedPcStat gFlashPaLed;  
eLedPcStat gFlashCcLed;  





unsigned short gLedStat = 0;
unsigned short  gLedStatOld;

void delay_50us(unsigned short tick)
{
	byte i;
	while(tick--)
	{
		for (i=0; i<10; i++);
	}
}

void LedShiftOut()
{
	char i;
	unsigned short  temp;

	gLedStatOld = gLedStat;
	temp = gLedStat;
	SHIFT_CRTL_LE = 0;
	for (i=0; i<16; i++)
	{
		SHIFT_CRTL_CLK = 0;
		delay_50us(10);
		if (temp & 0x8000)
		{
			SHIFT_CRTL_DATA = 1;
		}
		else 
		{
			SHIFT_CRTL_DATA = 0;
		}
		delay_50us(10);
		SHIFT_CRTL_CLK = 1;				
		delay_50us(10);
		temp <<= 1;
	}

	SHIFT_CRTL_LE = 0;
	delay_50us(10);
	SHIFT_CRTL_LE = 1;
	delay_50us(10);
	SHIFT_CRTL_LE = 0;
}



void LedSwitch( unsigned short type, char stat)
{
	if (LED_OPEN == stat)
	{
		gLedStat |= type;
	}
	else 
	{
		gLedStat &= ~type;
	}

	if (gLedStatOld == gLedStat) return;
	LedShiftOut();
}

void WorkLed()
{
	static char stat = 1;
	

	if (gRunLedTick >= 50) {
		if (stat == LED_OPEN)
		{
			LedSwitch(LED_WORK, LED_CLOSE);
			stat = LED_CLOSE;
		}
		else 
		{
			LedSwitch(LED_WORK, LED_OPEN);
			stat = LED_OPEN;
		}
		gRunLedTick = 0;
	}	
}

void SetPcStat(eLedPcStat stat)
{
	gFlashPcLed = stat;
}

void SetPaStat(eLedPcStat stat)
{
	gFlashPaLed = stat;
}

void SetCcStat(eLedPcStat stat)
{
	gFlashCcLed = stat;
}

void CcKeyLed()
{
	static char stat;


	if (gFlashCcLed == LED_PC_CLOSE)
	{
		LedSwitch(LED_CC, LED_CLOSE);
	}
	else if (gFlashCcLed == LED_PC_FLASH)
	{
		if (gCcLedTick >= 50) {
			if (stat == LED_OPEN)
			{
				LedSwitch(LED_CC, LED_CLOSE);
				stat = LED_CLOSE;
				//BUZZER_CRTL_PIN = 1;
			}
			else 
			{
				LedSwitch(LED_CC, LED_OPEN);
				stat = LED_OPEN;
				//BUZZER_CRTL_PIN = 0;
			}
			gCcLedTick = 0;
		}	
	}
	else if(gFlashCcLed == LED_PC_OPEN)
	{
		LedSwitch(LED_CC, LED_OPEN);
	}
}
void PaKeyLed()
{
	static char stat;


	if (gFlashPaLed == LED_PC_CLOSE)
	{
		LedSwitch(LED_PA, LED_CLOSE);
	}
	else if (gFlashPaLed == LED_PC_FLASH)
	{
		if (gPaLedTick >= 50) {
			if (stat == LED_OPEN)
			{
				LedSwitch(LED_PA, LED_CLOSE);
				stat = LED_CLOSE;
				//BUZZER_CRTL_PIN = 1;
			}
			else 
			{
				LedSwitch(LED_PA, LED_OPEN);
				stat = LED_OPEN;
				//BUZZER_CRTL_PIN = 0;
			}
			gPaLedTick = 0;
		}	
	}
	else if(gFlashPaLed == LED_PC_OPEN)
	{
		LedSwitch(LED_PA, LED_OPEN);
	}
}
void PcKeyLed()
{
	static char stat;


	if (gFlashPcLed == LED_PC_CLOSE)
	{
		LedSwitch(LED_PC, LED_CLOSE);
	}
	else if (gFlashPcLed == LED_PC_FLASH)
	{
		if (gPcLedTick >= 50) {
			if (stat == LED_OPEN)
			{
				LedSwitch(LED_PC, LED_CLOSE);
				stat = LED_CLOSE;
				//BUZZER_CRTL_PIN = 1;
			}
			else 
			{
				LedSwitch(LED_PC, LED_OPEN);
				stat = LED_OPEN;
				//BUZZER_CRTL_PIN = 0;
			}
			gPcLedTick = 0;
		}	
	}
	else if(gFlashPcLed == LED_PC_OPEN)
	{
		LedSwitch(LED_PC, LED_OPEN);
	}
}


void LedRun(void)
{
	WorkLed();
	PcKeyLed();
	PaKeyLed();
	CcKeyLed();
}

void LedAllOpen(void)
{
	gLedStat = 0xffff;
	LedShiftOut();
}

void LedInit(void)
{
	gFlashPcLed = LED_PC_CLOSE;
	gLedStat = 0;
}



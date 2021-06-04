/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 状态指示灯的控制
**  创建日期: 2013.12.26
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/

#include "led_control.h"
#include "include.h"
#include "data_box.h"


static eLedPcStat gLedPcStat;
static eLedPaStat gLedPaStat;
static eLedBoxComStat gLedBoxComStat;


static void LedRunCtrl(void)
{
	static u32 tick;
	static u8 flag;

	if (gSystemTick - tick >= 500) {
		tick = gSystemTick;
		if (flag == 0) {
			flag = 1;
			//LED_RUN_OPEN();
			//LED_PA_OPEN();
		}
		else {
			flag = 0;
			//LED_RUN_CLOSE();
			//LED_PA_CLOSE();
		}
	}
}


//LED BOX COM=============================================================================================
static void LedBoxComFlash(void)
{
	static u32 tick;

	if (gSystemTick - tick < 100) 
	{
		LED_BOX_COM_OPEN();
	}
	else if (gSystemTick - tick < 1000)
	{
		LED_BOX_COM_CLOSE();
	}
	else 
	{
		tick = gSystemTick;
	}
}

void LedSetBoxDataStat(eLedBoxComStat stat)
{
	gLedBoxComStat = stat;
}

void LedBoxComCrtl(void)
{
	switch(gLedBoxComStat)
	{
		case LED_BOX_COM_HAVE_DATA:
			LedBoxComFlash();
			break;
		case LED_BOX_COM_NO_DATA:
			LED_BOX_COM_OPEN();
			break;
	}
}
//END LED BOX COM=============================================================================================




//LED PC=============================================================================================
void LedPcCall(void)
{
	static u32 tick;

	if (gSystemTick - tick < 500) 
	{
		LED_PC_OPEN();
	}
	else if (gSystemTick - tick < 1000)
	{
		LED_PC_CLOSE();
	}
	else 
	{
		tick = gSystemTick;
	}
}


void LedPcDriverTalk(void)
{
	static u32 tick;

	if (gSystemTick - tick < 100) 
	{
		LED_PC_OPEN();
	}
	else if (gSystemTick - tick < 200)
	{
		LED_PC_CLOSE();
	}
	else 
	{
		tick = gSystemTick;
	}
}

void LedSetPcStat( eLedPcStat stat )
{
	gLedPcStat = stat;
}


void LedPcCrtl(void)
{
	switch(gLedPcStat)
	{
		case LED_PC_CLOSE:
			LED_PC_CLOSE();
			break;
		case LED_PC_CALL:
			LedPcCall();
			break;
		case LED_PC_CONN:
			LED_PC_OPEN();
			break;
		case LED_PC_DRIVER_TALK:
			LedPcDriverTalk();
			break;
	}
}
//END   LED PC=============================================================================================

//LED PA=============================================================================================


void LedPaRequest(void)
{
	static u32 tick;

	if (gSystemTick - tick < 100) 
	{
		LED_PA_OPEN();
	}
	else if (gSystemTick - tick < 200)
	{
		LED_PA_CLOSE();
	}
	else 
	{
		tick = gSystemTick;
	}
}

void LedSetPaStat( eLedPaStat  stat )
{
	gLedPaStat = stat;
}


void LedPaCrtl(void)
{
	switch(gLedPaStat)
	{
		case LED_PA_CLOSE:
			LED_PA_CLOSE();
			break;
		case LED_PA_OPEN:
			LED_PA_OPEN();
			break;
		case LED_PA_REQUEST:
			LedPaRequest();
			break;
	}
}
//END   LED PA=============================================================================================

/*
功能:  通过上电时闪烁所有LED 亮灭次数表明设备ID号
*/
void LedDispDeviceId(void)
{
	u8 deviceId;
	u8 i;
	u32 tick;
	
	deviceId = GetDeviceId();

	for (i=0; i<deviceId; i++)
	{
		LED_PC_OPEN();
		LED_BOX_COM_OPEN();
		LED_TRAIN_COM_OPEN();
		LED_PA_OPEN();
		tick = gSystemTick;
		while (gSystemTick-tick<200);
		
		LED_PC_CLOSE();
		LED_BOX_COM_CLOSE();
		LED_TRAIN_COM_CLOSE();
		LED_PA_CLOSE();
		tick = gSystemTick;
		while (gSystemTick-tick<300);		
	}
	
	tick = gSystemTick;
	while(gSystemTick-tick<1000);
	
}


void LedProc(void)
{
	LedRunCtrl();
	LedBoxComCrtl();
	LedPcCrtl();
	LedPaCrtl();
	//LedUsbCtrl();
	//LedAlarmCtrl();
}



/*
*******************************************************************************
**  Copyright (c) 2014, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理紧急报警状态切换
**  创建日期: 2014.01.06
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "stat_switch.h"
#include "general.h"
#include "com.h"
#include "led_control.h"

//总的状态
static eTalkStat gTalkStat;


void SetTalkStat(eTalkStat stat)
{	
	gTalkStat = stat;
}

eTalkStat GetTalkStat()
{
	return gTalkStat;
}


void TalkStatSwitch(void)
{
	eTalkStat  stat;
	eKeyType key;

	if (GetUartIfSend()) return;

	key = GetKeyValue();

	if (//(key == KEY_TYPE_IDLE) || 
		(key == KEY_TYPE_CC) || \
		(key == KEY_TYPE_PA) || \
		(key == KEY_TYPE_PTT_DOWN) || \
		(key == KEY_TYPE_PTT_UP) )
	{
		switch(key)
		{
			case KEY_TYPE_PTT_DOWN:
				LedSwitch(LED_PTT, LED_OPEN);
				MIC_CTRL_PIN = 0;				
				BoxDataUpdateKeyPtt(0);
				break;
			case KEY_TYPE_PTT_UP:
				LedSwitch(LED_PTT, LED_CLOSE);
				MIC_CTRL_PIN = 1;
				BoxDataUpdateKeyPtt(1);
				break;
			case KEY_TYPE_PA:
				LedSwitch(LED_PA, LED_OPEN);
				BoxDataUpdateKeyPa(0);
				//BoxDataUpdateKeyCc(1);  //CC 切到PA 时，两种状态会同时触发，add by suven
				break;
			case KEY_TYPE_CC:
				LedSwitch(LED_CC, LED_OPEN);
				BoxDataUpdateKeyCc(0);
				//BoxDataUpdateKeyPa(1);//PA 切到CC 时，两种状态会同时触发，add by suven
				break;

			case KEY_TYPE_PCRST:
				BoxDataUpdateKeyPcRst(0);
				//BoxDataUpdateKeyPa(1);
				//BoxDataUpdateKeyCc(1);
				break;

		}
	}
	else 
	{
		stat = GetTalkStat();
		switch( stat )
		{
			case TALK_STAT_IDLE:
				BoxDataUpdateKeyPc(0);
				break;
			case TALK_STAT_ALARM:
				if (key == KEY_TYPE_PC)
				{
					BoxDataUpdateKeyPc(1);
				}
				break;
			case TALK_STAT_CONN:
				if (key == KEY_TYPE_PC)
				{
					BoxDataUpdateKeyPc(2);
				}
				break;
			default:
				SetTalkStat(TALK_STAT_IDLE);
				break;
		}
	}
}

void TalkStatInit(void)
{
	gTalkStat = TALK_STAT_IDLE;
}


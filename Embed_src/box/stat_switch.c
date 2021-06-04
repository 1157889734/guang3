/*
*******************************************************************************
**  Copyright (c) 2014, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �����������״̬�л�
**  ��������: 2014.01.06
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "stat_switch.h"
#include "general.h"
#include "com.h"
#include "led_control.h"

//�ܵ�״̬
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
				//BoxDataUpdateKeyCc(1);  //CC �е�PA ʱ������״̬��ͬʱ������add by suven
				break;
			case KEY_TYPE_CC:
				LedSwitch(LED_CC, LED_OPEN);
				BoxDataUpdateKeyCc(0);
				//BoxDataUpdateKeyPa(1);//PA �е�CC ʱ������״̬��ͬʱ������add by suven
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


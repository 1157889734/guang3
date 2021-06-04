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



bit gKeyPcRst = KEY_INVALID;
bit gKeyPc = KEY_INVALID;
bit gKeyCc = KEY_INVALID;
bit gKeyPA = KEY_INVALID;
bit gKeyPtt = KEY_INVALID;
//bit gKeyIdle = KEY_INVALID; // 按键既不在PA  也不在PC
bit gKeyLedTest = KEY_INVALID;

//static eKeyType xdata gKey;

tKey gKey;

/*
void AppendKeyValue(eKeyType keyType)
{
	
	if (((gKey.q_in+1)%KEY_QUEUE_NUM) == gKey.q_out) return;

	gKey.key_queue[gKey.q_in] = keyType;

	gKey.q_in += 1;
	gKey.q_in %= KEY_QUEUE_NUM;
}
*/


eKeyType GetKeyValue(void)
{
	eKeyType value;

	
	if (gKey.q_in == gKey.q_out) return KEY_TYPE_NULL;

	value = gKey.key_queue[gKey.q_out];
	gKey.q_out += 1;
	gKey.q_out %= KEY_QUEUE_NUM;
	
	return value;
}

/*
byte GetKeyPaStat(void)
{
	if (gKeyPA == KEY_VALID) return 0;
	else return 1;
}

byte GetKeyCcStat(void)
{
	if (gKeyCc == KEY_VALID) return 0;
	else return 1;
}

byte GetKeyPcStat(void)
{
	if (gKeyPc == KEY_VALID) return 0;
	else return 1;
}

byte
*/


byte ScanKey()
{
	static xdata unsigned short pcrst_tick;
	static xdata unsigned short pc_tick;
	static xdata unsigned short ptt_tick;
	//static xdata unsigned short idle_tick;
	static xdata unsigned short pa_tick;
	static xdata unsigned short cc_tick;
	static xdata unsigned short ledtest_tick;

	KEY_PCRST_STAT = 1;
	if (0 == KEY_PCRST_STAT) // PCRST 按键扫描
	{
		if (gSys10msTick-pcrst_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPcRst== KEY_INVALID)
			{
				gKeyPcRst = KEY_VALID;
				BoxDataUpdateKeyPcRst(0);
			}
		}
		if(gKeyPcRst == KEY_VALID)
			pcrst_tick = gSys10msTick;
	}
	else 
	{
		if (gSys10msTick-pcrst_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPcRst == KEY_VALID)
			{
				gKeyPcRst = KEY_INVALID;
				BoxDataUpdateKeyPcRst(1);
			}
		}
		if(gKeyPcRst == KEY_INVALID)
			pcrst_tick = gSys10msTick;
	}
	
	KEY_PC_STAT = 1;								
	if (0 == KEY_PC_STAT)       // PC 按键扫描
	{
		if (gSys10msTick-pc_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPc == KEY_INVALID)
			{
				gKeyPc = KEY_VALID;
				BoxDataUpdateKeyPc(0);
			}
		}
		if(gKeyPc == KEY_VALID)
			pc_tick = gSys10msTick;
	}
	else 
	{
		if (gSys10msTick-pc_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPc == KEY_VALID)
			{
				gKeyPc = KEY_INVALID;
				BoxDataUpdateKeyPc(1);
			}
		}
		if(gKeyPc == KEY_INVALID)
			pc_tick = gSys10msTick;
	}

	KEY_PTT_STAT = 1;
	if (0 == KEY_PTT_STAT)  // PTT 按键扫描
	{
		if (gSys10msTick-ptt_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPtt == KEY_INVALID)
			{
				gKeyPtt = KEY_VALID;
				BoxDataUpdateKeyPtt(0);
			}
		}
		if (gKeyPtt == KEY_VALID)
		{
			ptt_tick = gSys10msTick;
		}
	}
	else 
	{		
		if (gSys10msTick-ptt_tick > KEY_SCAN_DEBOUNCE)
		{
			if(gKeyPtt == KEY_VALID)
			{
				BoxDataUpdateKeyPtt(1);
				gKeyPtt = KEY_INVALID;
			}
		}		
		if (gKeyPtt == KEY_INVALID)
		{
			ptt_tick = gSys10msTick;
		}
	}

	KEY_CC_STAT = 1;
	if (0 == KEY_CC_STAT)        // CC 按键扫描
	{
		if (gSys10msTick-cc_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyCc == KEY_INVALID)
			{
				gKeyCc = KEY_VALID;
				BoxDataUpdateKeyCc(0);				
			}
		}	
		if(gKeyCc == KEY_VALID)
			cc_tick = gSys10msTick;
	}
	else 
	{	
		if (gSys10msTick-cc_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyCc == KEY_VALID)
			{
				gKeyCc = KEY_INVALID;
				BoxDataUpdateKeyCc(1);
			}
		}
		if(gKeyCc == KEY_INVALID)
			cc_tick = gSys10msTick;
	}

	KEY_PA_STAT = 1;
	if (0 == KEY_PA_STAT) 	// PA 按键扫描
	{
		if (gSys10msTick-pa_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPA== KEY_INVALID)
			{
				gKeyPA = KEY_VALID;
				BoxDataUpdateKeyPa(0);				
			}
		}	
		if(gKeyPA == KEY_VALID)
			pa_tick = gSys10msTick;
	}
	else 
	{	
		if (gSys10msTick-pa_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyPA == KEY_VALID)
			{
				gKeyPA = KEY_INVALID;
				BoxDataUpdateKeyPa(1);
			}
		}
		if(gKeyPA == KEY_INVALID)
			pa_tick = gSys10msTick;
	}

	KEY_LED_TEST_STAT = 1;
	if (0 == KEY_LED_TEST_STAT) 	// LED_TEST 按键扫描
	{
		if (gSys10msTick-ledtest_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyLedTest == KEY_INVALID)
			{
				gKeyLedTest = KEY_VALID;
				BoxDataUpdateKeyLedTest(0);
			}
		}
		if(gKeyLedTest == KEY_VALID)			
			ledtest_tick = gSys10msTick;
	}
	else 
	{
		if (gSys10msTick-ledtest_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyLedTest == KEY_VALID)
			{
				gKeyLedTest =KEY_INVALID ;
				BoxDataUpdateKeyLedTest(1);
			}
		}
		if(gKeyLedTest == KEY_INVALID)			
			ledtest_tick = gSys10msTick;
	}


	/*
	if ((1 == KEY_PA_STAT) && (1 == KEY_CC_STAT))
	{
		if (gSys10msTick-idle_tick>KEY_SCAN_DEBOUNCE)
		{
			if (gKeyIdle == KEY_INVALID)
			{
				gKeyIdle = KEY_VALID;
				AppendKeyValue(KEY_TYPE_IDLE);
				goto scan_key_over;
			}
		}
	}
*/
	
//scan_key_over:

	return 0;
}





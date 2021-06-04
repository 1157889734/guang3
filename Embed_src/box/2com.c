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
#include<stat_switch.h>
#include "timer.h"
#include "key.h"
#include "led_control.h"
#include "com.h"

#define LCD_DATA_LEN      12
#define UART1_BUF_LEN      50

static byte xdata LcdPacketBuf[LCD_DATA_LEN]={0};
static byte xdata gLcdRecvPacketBuf[UART1_BUF_LEN];
static byte xdata gLcdRecvPaketLength = 0;

static byte xdata gBoxDataSendCnt = 0;

static byte xdata gKeyTypeOld = 0;

byte xdata gUart1TxBuf[UART1_BUF_LEN];
byte xdata gUart1TxIndex = 0;
byte xdata gUart1TxCnt = 0;
byte xdata uart1_recv_buf[UART1_BUF_LEN];

byte uart1_recv_fifo_in = 0;
byte uart1_recv_fifo_out =0;

byte uart1_send_cnt;
byte uart1_send_index;

byte uart1_stat = 0;  // 1:send now    0:idle

//byte repeat_send_now = 0;

byte uart1_busy()
{
	return uart1_stat;
}

void uart1_set_busy()
{
	uart1_stat = 1;
}

void uart1_clr_busy()
{
	uart1_stat = 0;
}


byte uart1_read_char(byte *ch)
{ 
    if(uart1_recv_fifo_out == uart1_recv_fifo_in)
	return 0;
    *ch = uart1_recv_buf[uart1_recv_fifo_out++];
    if(uart1_recv_fifo_out == UART1_BUF_LEN)
	uart1_recv_fifo_out = 0;
    return 1;
}


void UART1_int(void) interrupt 4
{
	byte tmp;

	if(RI)
	{
		RI = 0;
		tmp = SBUF;
		uart1_recv_buf[uart1_recv_fifo_in++] = (byte)tmp;
		if(uart1_recv_fifo_in >= UART1_BUF_LEN)
		{
			uart1_recv_fifo_in = 0;
		}
		
	}
	if (TI)
	{
		TI= 0;
		if (gUart1TxCnt > 0)
		{
			SBUF = gUart1TxBuf[gUart1TxIndex];
			gUart1TxIndex ++;
			gUart1TxCnt --;
		}
		else 
		{
			uart1_clr_busy();
			//Com_R_W =0;
		}
	}

}

void Uart1StartSend(byte *buf, byte len)
{
	byte i;

	if (!uart1_busy())
	{
		//Com_R_W = 1 ;  // 开发送
		uart1_set_busy();	
		for (i=0; i<len; i++)
		{
			gUart1TxBuf[i] = buf[i];
		}
		SBUF = buf[0];
		gUart1TxIndex = 1;
		gUart1TxCnt = len-1;
	}
}


//发送到bcu数据
byte LcdSend( byte* buf, byte length )
{
	byte xdata sendBuf[50];
	byte sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	Uart1StartSend( sendBuf, sendLen );
        return sendLen;
}

void LcdSendData(  )
{
	byte buf[11] = { 0x00, 0x1a, 0x00, 0x19, 0x00, 0x01, 0xaa, 0xaa, 0xaa, 0xaa,0xaa};

	/*
	buf[6] = gKeyPASt;
	buf[7] = gKeyCcSt;
	buf[8] = gKeyPcSt;
	buf[9] = gKeyPttSt;
	buf[10]= gKeyPcRstSt;
	*/
	LcdSend(buf, 11);
}

static void LcdDataSendRepeat()
{
	static unsigned long tick;
	
	if (uart1_busy())  // car 485
	{
		tick = gSys10msTick;
	}
	else 
	{
		if (gSys10msTick - tick > 10)
		{
			LcdSendData();
		}
	}
}


static byte LcdDataGetPacket(void)
{
	byte temp;
	while(uart1_read_char(&temp))
	{			
		if(temp == 0x7e)
		{
			//判断长度 判断是否有数据
			if(gLcdRecvPaketLength >= 3)
			{
				return 1;
			}
			gLcdRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gLcdRecvPacketBuf[gLcdRecvPaketLength] = temp;
			if( ++gLcdRecvPaketLength >= 255 )
				gLcdRecvPaketLength = 0;
		}
	}
	return 0;
}




void LcdDataProc(void)
{
	tHmiHeader *tHmidata;
	static byte token=0;
	static unsigned long  token_time = 0;
	byte buf[23] = { 0x00, 0xf2, 0x00, 0x19, 0x00, 0x011, };
	byte  len;
	byte temp;
	byte i;
	//eTalkStat talkStat;

	// 1. 收lcd数据
	if (LcdDataGetPacket()) 
	{
		len = VerifyPaket( gLcdRecvPacketBuf, gLcdRecvPaketLength );
		tHmidata = (tHmiHeader *)gLcdRecvPacketBuf;
		if ( len > 0 && (((tHmidata->dest_device_id &0x0f) == 0x09)||
			((tHmidata->dest_device_id &0x0f) == 0x02)) )
		{
			for(i=0;i<LCD_DATA_LEN;i++)
			{
				LcdPacketBuf[i] = gLcdRecvPacketBuf[6+i];
			}
			token = 1;
			//LcdDataSendRepeat();
		}		
		gLcdRecvPaketLength = 0;
	}

	// 2. 发数据给bcu
	if(token)
	{
		for(i=0;i<LCD_DATA_LEN;i++)
		{
			buf[6+i] = LcdPacketBuf[i];
		}
		//buf[];
		BoxDataSend(buf , 23);
		token = 0;
		token_time = gSys10msTick ;
		//BoxDataSendRepeat();
	}

	if(gSys10msTick - token_time > 15)
		token =1;
}






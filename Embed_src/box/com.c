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


#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符



#define UART2_BUF_LEN      50
#define BCU_DATA_LEN        44

static byte xdata BcuPacketBuf[BCU_DATA_LEN]={0};
static byte xdata gTrainRecvPacketBuf[UART2_BUF_LEN];
static byte xdata gTrainRecvPaketLength = 0;

static byte xdata gBoxDataSendCnt = 0;

static byte xdata gKeyTypeOld = 0;

byte xdata gUart2TxBuf[UART2_BUF_LEN];
byte xdata gUart2TxIndex = 0;
byte xdata gUart2TxCnt = 0;



byte xdata uart2_recv_buf[UART2_BUF_LEN];

byte uart2_recv_fifo_in = 0;
byte uart2_recv_fifo_out =0;

byte uart2_send_cnt;
byte uart2_send_index;

byte uart2_stat = 0;  // 1:send now    0:idle

byte repeat_send_now = 0;

byte uart2_busy()
{
	return uart2_stat;
}

void uart2_set_busy()
{
	uart2_stat = 1;
}

void uart2_clr_busy()
{
	uart2_stat = 0;
}


byte GetUartIfSend(void)
{
	return repeat_send_now;
}

byte uart2_read_char(byte *ch)
{ 
    if(uart2_recv_fifo_out == uart2_recv_fifo_in)
	return 0;
    *ch = uart2_recv_buf[uart2_recv_fifo_out++];
    if(uart2_recv_fifo_out == UART2_BUF_LEN)
	uart2_recv_fifo_out = 0;
    return 1;
}


void UART2_int(void) interrupt 8
{
	byte tmp;

	if(RI2)
	{
		CLR_RI2();
		tmp = S2BUF;
		uart2_recv_buf[uart2_recv_fifo_in++] = (byte)tmp;
		if(uart2_recv_fifo_in >= UART2_BUF_LEN)
		{
			uart2_recv_fifo_in = 0;
		}
		
	}
	if (TI2)
	{
		CLR_TI2();
		if (gUart2TxCnt > 0)
		{
			S2BUF = gUart2TxBuf[gUart2TxIndex];
			gUart2TxIndex ++;
			gUart2TxCnt --;
		}
		else 
		{
			uart2_clr_busy();
			//Com_R_W =0;
		}
	}

}







/*
执行转码并判断校验
*/
byte VerifyPaket(byte *paketBuf, byte length)
{
    byte recv_flag=0;
    byte checksum=0;
    byte src_length=0;//逆转码之前的长度
    byte dst_length=0;//逆转码以后的长度
//    static u8 length_of_send = 0;
    	    
           //执行逆转码 并计算校验和
	   for(src_length=0; src_length<length; src_length++)
	    {
			//拷贝数据
			paketBuf[dst_length]=paketBuf[src_length];
			//判断刚刚拷贝的是否是转码符
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_TAB 并跳过下一个字符
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TMS_PACKET_TAB
					src_length++;//并跳过下一个字符
				}
				//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_DLE 并跳过下一个字符
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TMS_PACKET_DLE
					src_length++;//并跳过下一个字符
				}
				else
				{
					//print_line("tms 7f error");
					//数据错误----7f没有被正确转码
					dst_length=0;//清除转码以后的长度
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //减掉校验和的长度
		}
		else
		{
			recv_flag = 0;
		}
		
		return recv_flag;
}



/*
功能:  实现数据打包，
             1、添加包头7E
             2、添加包尾7E
             3、数据转码
             4、计算校验和
*/
byte PackageData( byte *destBuf, byte *srcBuf, byte length )
{
	byte send_length = 0;
	byte checksum=0;
	byte cnt = 0;
	byte *ptr = NULL;


	destBuf[send_length++] = 0x7e;

       ptr = (byte *)srcBuf;
	for(cnt = 0; cnt < length; cnt++)
	{
	    if(*ptr == 0x7e)
	    {
	         destBuf[send_length++] = 0x7f;
		   destBuf[send_length++] = 0x80;
	    }
	    else if(*ptr == 0x7f)
	    {
	        destBuf[send_length++] = 0x7f;
		destBuf[send_length++] = 0x81;
	    }
	     else
	     {
	     	 destBuf[send_length++] = *ptr;
	     }
	     checksum += *ptr;
	     ptr++;
	}

	checksum = 0x55 -checksum;
      //校验和
	if(checksum == 0x7e)
	{
	    destBuf[send_length++] = 0x7f;
          destBuf[send_length++] = 0x80;
	}
	else if(checksum == 0x7f)
	{
	    destBuf[send_length++] = 0x7f;
           destBuf[send_length++] = 0x81;
	}
      else
	{
	    destBuf[send_length++] = checksum;
	}
	//send_buf[send_length++] = checksum;  //校验和
      destBuf[send_length++] = 0x7e;

	return send_length;
}





void Uart2StartSend(byte *buf, byte len)
{
	byte i;

	if (!uart2_busy())
	{
		//Com_R_W = 1 ;  // 开发送
		uart2_set_busy();	
		for (i=0; i<len; i++)
		{
			gUart2TxBuf[i] = buf[i];
		}
		S2BUF = buf[0];
		gUart2TxIndex = 1;
		gUart2TxCnt = len-1;
	}
}

byte gKeyPcSt=0;
byte gKeyCcSt=1;
byte gKeyPASt = 1;
byte gKeyPttSt = 1;
byte gKeyPcRstSt = 1;
byte gKeyLedTestSt = 1;


//发送到bcu数据
byte BoxDataSend( byte* buf, byte length )
{
	byte xdata sendBuf[30];
	byte sendLen;

	buf[18] = gKeyPASt;
	buf[19] = gKeyCcSt;
	buf[20] = gKeyPcSt;
	buf[21] = gKeyPttSt;
	buf[22]= gKeyPcRstSt;
	
	sendLen = PackageData( sendBuf, buf, length );
	Uart2StartSend( sendBuf, sendLen );
        return sendLen;
}

void BoxDataUpdateKeyLedTest(byte stat)
{
	gKeyLedTestSt = stat;
}

void BoxDataUpdateKeyPa(byte stat)
{
	gKeyPASt = stat;
}

void BoxDataUpdateKeyCc(byte stat)
{
	gKeyCcSt = stat;
}

void BoxDataUpdateKeyPc(byte stat)
{
	gKeyPcSt= stat;
}

void BoxDataUpdateKeyPcRst(byte stat)
{
	gKeyPcRstSt= stat;
}

void BoxDataUpdateKeyPtt(byte stat)
{
	gKeyPttSt = stat;
}

void BoxDataSendKey(  )
{
	byte buf[11] = { 0x00, 0xf2, 0x00, 0x19, 0x00, 0x01, 0xaa, 0xaa, 0xaa, 0xaa,0xaa};

	buf[6] = gKeyPASt;
	buf[7] = gKeyCcSt;
	buf[8] = gKeyPcSt;
	buf[9] = gKeyPttSt;
	buf[10]= gKeyPcRstSt;
	BoxDataSend(buf, 11);
}

static void BoxDataSendRepeat(void)
{
	static unsigned long tick;
	
	if (uart2_busy())  // car 485
	{
		tick = gSys10msTick;
	}
	else 
	{
		if (gSys10msTick - tick > 10)
		{
			BoxDataSendKey(  );
		}
	}
}


static byte BoxDataGetPacket(void)
{
	byte temp;
	while(uart2_read_char(&temp))
	{			
		if(temp == 0x7e)
		{
			//判断长度 判断是否有数据
			if(gTrainRecvPaketLength >= 3)
			{
				return 1;
			}
			gTrainRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gTrainRecvPacketBuf[gTrainRecvPaketLength] = temp;
			if( ++gTrainRecvPaketLength >= 255 )
				gTrainRecvPaketLength = 0;
		}
	}
	return 0;
}




void BoxDataProc(void)
{
	tFrameHeader *frameHeader;
	byte  len;
	byte temp;
	byte i;
	//eTalkStat talkStat;

	//BoxDataSendRepeat(); //测试串口

	
	// 1. 收BCU 数据
	if (BoxDataGetPacket()) 
	{
		//LcdSend( gTrainRecvPacketBuf, gTrainRecvPaketLength );  //转发数据给lcd
		
		len = VerifyPaket( gTrainRecvPacketBuf, gTrainRecvPaketLength );
		frameHeader = (tFrameHeader *)gTrainRecvPacketBuf;
		if ( len > 0 && ((frameHeader->dest_device_id &0x0f) == 0x09)) 
		{
			for(i= 0; i<BCU_DATA_LEN ; i++)
			{
				BcuPacketBuf[i] = gTrainRecvPacketBuf[i];
			}
			LcdSend( BcuPacketBuf, BCU_DATA_LEN );  //转发数据给lcd
		
			//设置pc灯
			temp = frameHeader->data_area[0];
			//talkStat = GetTalkStat();
			if (temp == 0x01)
			{
				SetTalkStat(TALK_STAT_ALARM);
				SetPcStat(LED_PC_FLASH);
			}
			else if (temp == 0x02) 
			{
				SetTalkStat(TALK_STAT_CONN);
				SetPcStat(LED_PC_OPEN);
				MIC_CTRL_PIN = 0;
			}
			else if (temp == 0x03) 
			{
				gKeyTypeOld = KEY_TYPE_NULL;
				SetTalkStat(TALK_STAT_IDLE);
				SetPcStat(LED_PC_CLOSE);
			}			

			//设置cc灯   0无   1对侧   2本侧
			temp = frameHeader->data_area[1];
			if (temp == 0x00)
			{
				SetCcStat(LED_PC_CLOSE);				
			}
			else if (temp == 0x01) 
			{
				SetCcStat(LED_PC_FLASH);				
			}
			else if (temp == 0x02) 
			{			
				SetCcStat(LED_PC_OPEN);
				MIC_CTRL_PIN = 0;
			}	

			//设置pa灯   0   1申请   2申请成功
			temp = frameHeader->data_area[2];
			if (temp == 0x00)
			{
				SetPaStat(LED_PC_CLOSE);				
			}
			else if (temp == 0x01) 
			{
				SetPaStat(LED_PC_FLASH);				
			}
			else if (temp == 0x02) 
			{			
				SetPaStat(LED_PC_OPEN);
				MIC_CTRL_PIN = 0;
			}	

			if(frameHeader->data_area[0] ==3 && frameHeader->data_area[1] == 0
				&& frameHeader->data_area[2] == 0)
				MIC_CTRL_PIN = 1;

			//BoxDataSendRepeat();
			//BoxDataSend(gTrainRecvPacketBuf, gTrainRecvPaketLength);
			
		}
		gTrainRecvPaketLength = 0;
	}	

	
}


void BoxDataInit(void)
{
	gBoxDataSendCnt = 0;

}




/*
*******************************************************************************
**  Copyright (c) 2014, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: 
**  ��������: 2014.01.03
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include<stat_switch.h>
#include "timer.h"
#include "key.h"
#include "led_control.h"
#include "com.h"


#define TBA_PACKET_TAB			0x7E	//���ݰ��ָ��
#define TBA_PACKET_DLE			0x7F	//���ݰ�ȫ��ת���
#define TBA_PACKET_DLE_TAB		0x80	//���ݰ��ָ����ת���
#define TBA_PACKET_DLE_DLE		0x81	//���ݰ�ת�����ת���



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
ִ��ת�벢�ж�У��
*/
byte VerifyPaket(byte *paketBuf, byte length)
{
    byte recv_flag=0;
    byte checksum=0;
    byte src_length=0;//��ת��֮ǰ�ĳ���
    byte dst_length=0;//��ת���Ժ�ĳ���
//    static u8 length_of_send = 0;
    	    
           //ִ����ת�� ������У���
	   for(src_length=0; src_length<length; src_length++)
	    {
			//��������
			paketBuf[dst_length]=paketBuf[src_length];
			//�жϸոտ������Ƿ���ת���
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//�ж���һ���ַ��Ƿ��Ƿָ����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB ��������һ���ַ�
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB
					src_length++;//��������һ���ַ�
				}
				//�ж���һ���ַ��Ƿ���ת�����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE ��������һ���ַ�
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE
					src_length++;//��������һ���ַ�
				}
				else
				{
					//print_line("tms 7f error");
					//���ݴ���----7fû�б���ȷת��
					dst_length=0;//���ת���Ժ�ĳ���
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //����У��͵ĳ���
		}
		else
		{
			recv_flag = 0;
		}
		
		return recv_flag;
}



/*
����:  ʵ�����ݴ����
             1����Ӱ�ͷ7E
             2����Ӱ�β7E
             3������ת��
             4������У���
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
      //У���
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
	//send_buf[send_length++] = checksum;  //У���
      destBuf[send_length++] = 0x7e;

	return send_length;
}





void Uart2StartSend(byte *buf, byte len)
{
	byte i;

	if (!uart2_busy())
	{
		//Com_R_W = 1 ;  // ������
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


//���͵�bcu����
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
			//�жϳ��� �ж��Ƿ�������
			if(gTrainRecvPaketLength >= 3)
			{
				return 1;
			}
			gTrainRecvPaketLength=0;
		}
		//�����ַ���ֱ���ͻ�����
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

	//BoxDataSendRepeat(); //���Դ���

	
	// 1. ��BCU ����
	if (BoxDataGetPacket()) 
	{
		//LcdSend( gTrainRecvPacketBuf, gTrainRecvPaketLength );  //ת�����ݸ�lcd
		
		len = VerifyPaket( gTrainRecvPacketBuf, gTrainRecvPaketLength );
		frameHeader = (tFrameHeader *)gTrainRecvPacketBuf;
		if ( len > 0 && ((frameHeader->dest_device_id &0x0f) == 0x09)) 
		{
			for(i= 0; i<BCU_DATA_LEN ; i++)
			{
				BcuPacketBuf[i] = gTrainRecvPacketBuf[i];
			}
			LcdSend( BcuPacketBuf, BCU_DATA_LEN );  //ת�����ݸ�lcd
		
			//����pc��
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

			//����cc��   0��   1�Բ�   2����
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

			//����pa��   0   1����   2����ɹ�
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




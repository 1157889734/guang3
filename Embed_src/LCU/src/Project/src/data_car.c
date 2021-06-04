/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �������������ݣ�����ģ���ȡ�����������ݶ���������
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "data_public.h"
#include "data_car.h"
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "utils.h"
#include "device_info.h"
//#include "data_public.h"
#include "include.h"



static u8 gCarRecvPaketBuf[256];
static u8 gCarRecvPaketLength = 0;


#define CAR_DATA_SEND_CNT					2
static u8 gCarDataSendCnt = 0;
static tStationStat gStationStatOld;

static u8 CalcuSum(u8 *buf, u8 len)
{
	u8 i;
	u8 sum;

	sum = 0;
	for (i=0; i<len; i++)
	{
		sum += buf[i];
	}
	return sum;
}

void CarDataClearDmp()
{
	u8 clearBuf[18] = {0xFF, 0xC0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFE, 0x00, 0x00, 0x00};

	uart2_write_buf( clearBuf, 18 );
	print_line("clear dmp");

}

static void CarDataSendToDmp( tStationStat stationStat )
{
	u8 sendBuf[15] = {0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xF0, 0xFE};

	//�ȴ����ڿ���
	while(uart2_busy());

	if ( (gStationStatOld.mStartStation!=stationStat.mStartStation) || (gStationStatOld.mEndStation!=stationStat.mEndStation) )
	{
		gCarDataSendCnt = CAR_DATA_SEND_CNT*2 - 1;
		CarDataClearDmp();
	}
	else if (gCarDataSendCnt > CAR_DATA_SEND_CNT) 
	{
		CarDataClearDmp();
	}
	else 
	{
		switch (stationStat.mRunStat)
		{
			case RUN_STAT_OPEN_DOOR:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x80 + stationStat.mDoorSide;
				break;
			case RUN_STAT_CLOSE_DOOR:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x03;
				break;
			case RUN_STAT_LEAVE_BROADCAST:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x00;
				break;
			case RUN_STAT_PRE_DOOR_SIDE:
				sendBuf[6] = stationStat.mDoorSide;
				sendBuf[7] = 0x00;				
				break;
				break;
		}
		sendBuf[3] = stationStat.mStartStation;
		sendBuf[4] = stationStat.mEndStation;
		sendBuf[5] = stationStat.mNextStation;
		sendBuf[13] = CalcuSum(&sendBuf[1], 12);
		uart2_write_buf( sendBuf, 15 );
	}
	memcpy( &gStationStatOld, &stationStat, sizeof(tStationStat) );

}

void CarDataStartToSend( tStationStat stationStat )
{
	gCarDataSendCnt = 1;  // ����ֻ�ܷŵ����У���Ϊ����ĺ�����ȥ���ܻ��޸ĸ�ֵ
	CarDataSendToDmp( stationStat );
	print_line("start station:");
	print_int("%d", stationStat.mStartStation);
	print_line("end station:");
	print_int("%d", stationStat.mEndStation);
	print_line("next station:");
	print_int("%d", stationStat.mNextStation);
}


static void CarDataSendRepeat()
{
	static u32 tick;
	
	if (uart2_busy())  // car 485
	{
		tick = gSystemTick;
	}
	else 
	{
		if (gSystemTick - tick > 20)
		{
			if ( gCarDataSendCnt > 0 )
			{
				CarDataSendToDmp( gStationStatOld );
				gCarDataSendCnt --;
			}
		}
	}

}

u8 CarDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;

	//�ȴ����ڿ���
	while(uart2_busy());
	
	sendLen = PackageData( sendBuf, buf, length );
	uart2_write_buf( sendBuf, sendLen );
        return 0;
}

static u8 CarDataGetPacket(void)
{
	u8 temp;
	while(uart2_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//�жϳ��� �ж��Ƿ�������
			if(gCarRecvPaketLength >= 3)
			{
				return 1;
			}
			gCarRecvPaketLength=0;
		}
		//�����ַ���ֱ���ͻ�����
		else
		{
			gCarRecvPaketBuf[gCarRecvPaketLength] = temp;
			if( ++gCarRecvPaketLength >= 255 )
			{
				gCarRecvPaketLength = 0;
			}
		}
	}
	return 0;
}




void CarDataProc(void)
{
	tFrameHeader *frameHeader;
	tPiscHeader *piscData;
	u8 len;

	if (CarDataGetPacket()) 
	{
		len = VerifyPaket( gCarRecvPaketBuf, gCarRecvPaketLength );
		if ( len > 0 ) 
		{
			frameHeader = (tFrameHeader *)gCarRecvPaketBuf;
			switch (frameHeader->cmd.packet_type) 
			{
				case PROCESS_PACKET:
					
					break;
				case TOKEN_PACKET:

					break;
				case DOWNLOAD_PACKET:

					break;
				default :
					break;
			}
		}
		gCarRecvPaketLength = 0;
	}
	
	CarDataSendRepeat();
}


void MapInit(void)
{
#if 0
	u8 sendBuf[15] = {0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFE};

	sendBuf[13] = CalcuSum(&sendBuf[1], 12);
	uart2_write_buf( sendBuf, 15 );
#endif	
}


void CarDataInit()
{

	CarDataClearDmp();
/*
	//���Ͱ汾��
	buf[0] = DEVICE_SOFTWARE_VERSION_HIGHT;
	buf[1] = DEVICE_SOFTWARE_VERSION_LOW;
	CarDataSend( buf, 2 );
	LED_CAR_COM_OPEN();
*/	
}



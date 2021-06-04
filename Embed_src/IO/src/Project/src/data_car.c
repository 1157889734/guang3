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
#include "data_train.h"
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "utils.h"
#include "device_info.h"
//#include "data_public.h"
#include "include.h"

tCarCheck gCarCheck;

static u8 gCarRecvPaketBuf[256];
static u8 gCarRecvPaketLength = 0;


#define CAR_DATA_SEND_CNT					2
#define CAR_POLL_ERROR_NUM				10




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

void DrmdTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[20];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.drmd_index++;
	if(gCarCheck.drmd_index>DrmdNumMax)
		gCarCheck.drmd_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_DMP;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.drmd_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //Ѳ�� ���� 02
	lcuHeader->len = (u8)0x00;

	#if 0
	lcuHeader->data_area.ehp_st = GetEhpAlarmStat();
	lcuHeader->data_area.sapu_st = GetEhpComStat();
	lcuHeader->data_area.spk_st = 0x00;
	lcuHeader->data_area.led_st = 0x00;
	lcuHeader->data_area.dmp1_st = 0x00;
	lcuHeader->data_area.dmp2_st = 0x00;
	GetSoftwareVersion(&vh, &vl);
	lcuHeader->data_area.version_h= vh;
	lcuHeader->data_area.version_l= vl;
	#endif
	
	CarDataSend( tempBuf, 6 );
}

void IduTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[100];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.idu_index++;
	if(gCarCheck.idu_index>IduNumMax)
		gCarCheck.idu_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_IDU;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.idu_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //Ѳ�� ���� 02
	lcuHeader->len = (u8)0x00;

	#if 0
	lcuHeader->data_area.ehp_st = GetEhpAlarmStat();
	lcuHeader->data_area.sapu_st = GetEhpComStat();
	lcuHeader->data_area.spk_st = 0x00;
	lcuHeader->data_area.led_st = 0x00;
	lcuHeader->data_area.dmp1_st = 0x00;
	lcuHeader->data_area.dmp2_st = 0x00;
	GetSoftwareVersion(&vh, &vl);
	lcuHeader->data_area.version_h= vh;
	lcuHeader->data_area.version_l= vl;
	#endif
	
	CarDataSend( tempBuf, 6 );
}

/*************************************************
����: Ѳ��Car �����豸
*************************************************/
void CarTokenSend()
{
	if(gCarCheck.token)   //�������Ȩ
	{
		gCarCheck.poll_type = 0 ;
		DrmdTokenSend();	
	
		gCarCheck.token = 0 ;  //�ͷ�����
		gCarCheck.poll_time_value= gSystemTick ;  
	}
#if 0
	static u8 cnt=0;
	
	if(gCarCheck.token)   //�������Ȩ
	{
		if(cnt%2)      //Ѳ��drmd
		{
			gCarCheck.poll_type = 0 ;
			DrmdTokenSend();
		}
		else            //Ѳ��idu
		{			
			gCarCheck.poll_type = 1;
			IduTokenSend();
		}
		cnt ++;
		gCarCheck.token = 0 ;  //�ͷ�����
		gCarCheck.poll_time_value= gSystemTick ;  
	}
#endif
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
	static u8 index=0;

	//���ն����������ݰ�
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
					gCarCheck.token = 1; //��������Ȩ��
					//if(gSystemTick - gCarCheck.check_time_value <200)
					{
						//�յ����ƻظ�����ϰ�
						if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_DMP)
						{
							gCarCheck.drmdpoll_error_cnt[frameHeader->src_eqnum.eq_num-1] =0;
							gCarCheck.drmd_error[(frameHeader->src_eqnum.eq_num-1)/8] &= ~(1<< (frameHeader->src_eqnum.eq_num-1)%8);
						}
						else if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_IDU)
						{
							gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] = 0;
							gCarCheck.idu_error &= ~(1<< (frameHeader->src_eqnum.eq_num-1));
						}
					}
					break;
				case DOWNLOAD_PACKET:

					break;
				default :
					break;
			}
		}
		gCarRecvPaketLength = 0;
	}

	if(gSystemTick - gCarCheck.poll_time_value>200 && gCarCheck.token==0)
	{
		gCarCheck.token = 1;
		if(!gCarCheck.poll_type)
		{
			gCarCheck.drmdpoll_error_cnt[gCarCheck.drmd_index-1] ++;
			if(gCarCheck.drmdpoll_error_cnt[gCarCheck.drmd_index-1] >CAR_POLL_ERROR_NUM)
				gCarCheck.drmd_error[(gCarCheck.drmd_index-1)/8] |= (1<< (gCarCheck.drmd_index-1)%8 );
		}
		else
		{
			gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] ++;
			if(gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] >CAR_POLL_ERROR_NUM)
				gCarCheck.idu_error |= (1<< (gCarCheck.idu_index-1));
		}
	}
	

	// Ѳ����������豸
	if(index%2)
		CarTokenSend();
	else
	{
		if(gCarCheck.token)
			CarDataSend((u8*)&gpiscData, sizeof(tPiscHeader));
	}

	index++;
	//CarDataSendRepeat();
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



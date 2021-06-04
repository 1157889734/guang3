/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �����г��������ݣ�����ģ���ȡ�г��������ݶ���������
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "uart4.h"
#include "utils.h"
#include "include.h"
#include "data_public.h"
#include "data_ehp.h"
#include "data_train.h"
#include "data_car.h"
#include "progupdate.h"
#include "led_control.h"


static u32 gNoDataTick;

static u8 gTrainRecvPacketTempBuf[256];

static u8 gTrainRecvPacketBuf[256];
static u8 gTrainRecvPaketLength = 0;

static tStationStat gStationStat;


//��Ž�Ҫ���͸�����������������
static eEhpBcbStat gEhpBcbBuf[EHP_MAX_NUM];
	

/*
����:  ������
deviceId: �±��1��ʼ
*/
eEhpBcbStat TrainDataGetEhpStat( u8 deviceId )
{
	if (deviceId > EHP_MAX_NUM) return EHP_STAT_IDLE;
	if (deviceId == 0) return EHP_STAT_IDLE;
	
	return gEhpBcbBuf[deviceId-1];
}

u8 TrainDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	uart1_write_buf( sendBuf, sendLen );
        return sendLen;
}

static void TrainDataTokenReply( void )
{
	tLcuHeader *lcuHeader;
	u8 tempBuf[100];
	u8 vh, vl;

	u32 tick;

	tick = gSystemTick;
	while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)0x0f;
	lcuHeader->dest_eqnum.eq_num = (u8)0x0f;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x10;
	lcuHeader->len = (u8)0x08;

	lcuHeader->data_area.ehp_st = GetEhpAlarmStat();
	lcuHeader->data_area.sapu_st = GetEhpComStat();
	lcuHeader->data_area.spk_st = 0x00;
	lcuHeader->data_area.led_st = 0x00;
	lcuHeader->data_area.dmp1_st = 0x00;
	lcuHeader->data_area.dmp2_st = 0x00;
	GetSoftwareVersion(&vh, &vl);
	lcuHeader->data_area.version_h= vh;
	lcuHeader->data_area.version_l= vl;

	TrainDataSend( tempBuf, 14 );
}


const u8 shiftNumBuf[EHP_MAX_NUM] = { 6, 4, 2 };

/*
����: ��������״̬����
             ��������״̬�й㲥���ƺз������г�����
             ���ؿ������������г�����
             ���ؿ�������������������
             ������������������������
             ��������״̬��3��
             ���б��ؿ����������г����ߺ͹㲥���ƺз���һ��
             �����������в�ͬ �ɲο�Э���ĵ�


             ���ؿ�����Ѳ�����������
             00������
             01��æµ
             02��Ӧ��

		�㲥���ƺз���
		00	����
		01	����״̬
		02	Ӧ��״̬
		03	�˿ͽ�����������

		ֻҪһ�����ҽ�ͨ�˱��������еı�������������æµ

*/
 void TrainDataBcbAnalyse(u8 *buf)
 {
	tBcbHeader *bcbData;
	u8 i, j;
	u8 deviceId;
	u8 temp1, temp2;

	bcbData = (tBcbHeader*)buf;

	deviceId = GetDeviceId();
	for ( i=0; i<LCU_MAX_NUM; i++ )
	{
		temp1 = bcbData->data_area.esu_status[i];
		for ( j=0; j<EHP_MAX_NUM; j++ )
		{
			temp2 = temp1 >> shiftNumBuf[j];
			temp2 &= 0x03;
			if ( i== (deviceId-1) )
			{
				if ( temp2 ==  0x00 )  //����
				{
					//if ( gEhpBcbBuf[j] != EHP_STAT_ANSWER )
					{
						gEhpBcbBuf[j] = EHP_STAT_IDLE;
					}
				}
				else if ( temp2 ==  0x01 ) // ����
				{
					gEhpBcbBuf[j] = EHP_STAT_IDLE;
				}
				else if ( temp2 ==  0x02 )  // ��ͨ
				{
					gEhpBcbBuf[j] = EHP_STAT_ANSWER;
					//����������Ҫ����æµ
					for ( i=0; i<EHP_MAX_NUM; i++ )
					{
						if (i != j)
						{
							gEhpBcbBuf[i] = EHP_STAT_BUSY;
						}
					}
					return;
				}
				else if ( temp2 ==  0x03 )  // �Ҷ�
				{
					gEhpBcbBuf[j] = EHP_STAT_HANGUP;
				}
			}
			else 
			{
				if (temp2 ==  EHP_STAT_ANSWER ) 
				{	
					// ֻҪ����������һ��ΪӦ��״̬����ô������ȫ��Ϊæµ
					for ( i=0; i<EHP_MAX_NUM; i++ )
					{
						gEhpBcbBuf[i] = EHP_STAT_BUSY;
					}
					return;
				}	
			}
		}
		
	}
 }

 void TrainDataPiscAnalyse(u8 *buf)
{
	tPiscHeader *piscData;
	static u8 leaveFlag = 0;
	static u8 stopFlag = 0;
	static u8 closeDoorFlag = 0;
	static u8 preDoorFlag = 0;
	tStationStat stationStat;
	u8 sendFlag = 0;  // û�д����źŵ�ʱ��Ҫ����һ��

	piscData = (tPiscHeader*)buf;

#if 0
	//��Ч���ж�
	if ((gStationStat.mStartStation>LINE_MAX_STATION_NUM) || (gStationStat.mEndStation>LINE_MAX_STATION_NUM))
	{

	}
#endif

	if ( (piscData->data_area.start_station !=gStationStat.mStartStation) || \
		(piscData->data_area.end_station !=gStationStat.mEndStation)   || \
		piscData->data_area.next_station !=gStationStat.mNextStation)  
	{
		gStationStat.mStartStation = piscData->data_area.start_station;
		gStationStat.mEndStation = piscData->data_area.end_station;
		gStationStat.mNextStation = piscData->data_area.next_station;
		sendFlag = 1;
	}
	
	stationStat.mStartStation = piscData->data_area.start_station;
	stationStat.mEndStation = piscData->data_area.end_station;
	stationStat.mNextStation = piscData->data_area.next_station;
	if ( (1 == piscData->data_area.status_hbit.door_right_sides) && (0 == piscData->data_area.status_hbit.door_left_sides) )
	{
		stationStat.mDoorSide = DOOR_SIDE_RIGHT;
	}
	else if ( (0 == piscData->data_area.status_hbit.door_right_sides) && (1 == piscData->data_area.status_hbit.door_left_sides) )
	{
		stationStat.mDoorSide = DOOR_SIDE_LEFT;
	}
	else if ( (1 == piscData->data_area.status_hbit.door_right_sides) && (1 == piscData->data_area.status_hbit.door_left_sides) )
	{
		stationStat.mDoorSide = DOOR_SIDE_BOTH;
	}
	else if ( (0 == piscData->data_area.status_hbit.door_right_sides) && (0 == piscData->data_area.status_hbit.door_left_sides) )
	{
		stationStat.mDoorSide = DOOR_SIDE_NONE;
	}

	//����Գ�ת��
	if (GetDeviceId() >= 4)
	{
		if (stationStat.mDoorSide == DOOR_SIDE_LEFT)
		{
			stationStat.mDoorSide = DOOR_SIDE_RIGHT;
		}
		else if (stationStat.mDoorSide == DOOR_SIDE_RIGHT)
		{
			stationStat.mDoorSide = DOOR_SIDE_LEFT;
		}
	}


	#if 0
	if ( DOOR_SIDE_NONE != stationStat.mDoorSide )
	{
		if ( 0 == preDoorFlag ) //�²⣬������������ֻ��ʾһ��������suven
		{
			sendFlag = 0;
			preDoorFlag = 1;
			stationStat.mRunStat = RUN_STAT_PRE_DOOR_SIDE;
			CarDataStartToSend(stationStat);
			if (stationStat.mDoorSide == DOOR_SIDE_LEFT)print_line("=============door side left");
			else if (stationStat.mDoorSide == DOOR_SIDE_RIGHT)print_line("=============door side right");
			else if (stationStat.mDoorSide == DOOR_SIDE_BOTH)print_line("=============door side both");
		}
	}
	else 
	{
		preDoorFlag = 0;
	}

	#else
	
	if ( DOOR_SIDE_NONE != stationStat.mDoorSide  && preDoorFlag != stationStat.mDoorSide)
	{
		//if ( 0 == preDoorFlag ) //�²⣬������������ֻ��ʾһ��������suven
		{
			preDoorFlag = stationStat.mDoorSide;
			sendFlag = 0;
			stationStat.mRunStat = RUN_STAT_PRE_DOOR_SIDE;
			CarDataStartToSend(stationStat);
			if (stationStat.mDoorSide == DOOR_SIDE_LEFT)print_line("=============door side left");
			else if (stationStat.mDoorSide == DOOR_SIDE_RIGHT)print_line("=============door side right");
			else if (stationStat.mDoorSide == DOOR_SIDE_BOTH)print_line("=============door side both");
		}
	}
	

	#endif
	
	if ( (piscData->data_area.trigger_hbit.leave_broadcast))
	{
		if ( 0==leaveFlag )
		{
			sendFlag = 0;
			leaveFlag = 1;
			stationStat.mRunStat = RUN_STAT_LEAVE_BROADCAST;
			CarDataStartToSend(stationStat);
			print_line("=============trigger leave broadcast");
		}
	}
	else 
	{
		leaveFlag = 0;
	}
	
	if ( (piscData->data_area.trigger_hbit.reach_stop))
	{
		if ( 0==stopFlag  )
		{
			sendFlag = 0;
			stopFlag  = 1;
			stationStat.mRunStat = RUN_STAT_OPEN_DOOR;
			CarDataStartToSend(stationStat);	
			print_line("=============trigger reach stop");
		}
	}
	else 
	{
		stopFlag  = 0;
	}
	
	if ( (piscData->data_area.trigger_hbit.leave))
	{
		if ( 0==closeDoorFlag )
		{
			sendFlag = 0;
			closeDoorFlag = 1;
			stationStat.mRunStat = RUN_STAT_CLOSE_DOOR;
			CarDataStartToSend(stationStat);
			print_line("=============trigger leave");
		}
	}
	else 
	{
		closeDoorFlag = 0;
	}
	
	if ( sendFlag == 1 )
	{
		sendFlag = 0;
		if (piscData->data_area.trigger_hbit.leave_broadcast) stationStat.mRunStat = RUN_STAT_LEAVE_BROADCAST;
		else if (piscData->data_area.trigger_hbit.reach_stop) stationStat.mRunStat = RUN_STAT_OPEN_DOOR;
		else if (piscData->data_area.trigger_hbit.leave) stationStat.mRunStat = RUN_STAT_CLOSE_DOOR;
		CarDataStartToSend(stationStat);
		print_line("=============send to dmp");
	}
	
}


static u8 TrainDataGetPacket(void)
{
	u8 temp;
	while(uart1_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
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




void TrainDataProc(void)
{
	tFrameHeader *frameHeader;
	tPiscHeader *piscData;
	tBcbHeader *bcbData;
	u8 deviceId;
	u8 len;

	if (TrainDataGetPacket()) 
	{
		len = VerifyPaket( gTrainRecvPacketBuf, gTrainRecvPaketLength );
		if ( len > 0 ) 
		{
			frameHeader = (tFrameHeader *)gTrainRecvPacketBuf;
			switch (frameHeader->cmd.packet_type) 
			{
				case PROCESS_PACKET:
					//if ((frameHeader->dest_eqnum.eq_num != 0xF) || (frameHeader->dest_eqnum.eq_type != 0xF)) break;
					
					//�Ƿ�Ϊ��������
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_PISC ) 
					{
						piscData = (tPiscHeader *)gTrainRecvPacketBuf;
						//�Ƿ�Ϊ��
						if ( piscData->data_area.status_hbit.master ) 
						{
							//�������ݲ�ת�����г�����
							TrainDataPiscAnalyse( gTrainRecvPacketBuf );
						}
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB ) 
					{
						bcbData = (tBcbHeader*)gTrainRecvPacketBuf;
						if (bcbData->data_area.status_hbit.active)
						{
							//�������� ����Խ�״̬
							TrainDataBcbAnalyse( gTrainRecvPacketBuf );
						}
					}
					break;
				case TOKEN_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_LCU )
					{
						deviceId = GetDeviceId();
						if ( frameHeader->dest_eqnum.eq_num == deviceId )
						{
							TrainDataTokenReply();
						}
					}
					
					break;
				case DOWNLOAD_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_LCU )
					{
						ProgramUpdate( gTrainRecvPacketBuf );
					}
					else 
					{
						memcpy(gTrainRecvPacketTempBuf, gTrainRecvPacketBuf, len);
						CarDataSend( gTrainRecvPacketTempBuf, len );
					}
					break;
				default :
					break;
			}
			gNoDataTick = gSystemTick;
		}
		gTrainRecvPaketLength = 0;
	}

	if ( gSystemTick-gNoDataTick > 3000 )
	{
		LED_TRAIN_COM_OPEN();
	}

}


void TrainDataInit(void)
{
	u8 i;

	for (i=0; i<EHP_MAX_NUM; i++)
	{
		gEhpBcbBuf[i] = EHP_STAT_IDLE;
	}
	
	gStationStat.mStartStation = 0;
	gStationStat.mEndStation = 0;
	gStationStat.mDoorSide = DOOR_SIDE_NONE;
	gStationStat.mNextStation = 0;
	gStationStat.mRunStat = RUN_STAT_CLOSE_DOOR;

}




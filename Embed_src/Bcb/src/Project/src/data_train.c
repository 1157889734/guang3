/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理列车总线数据，其他模块获取列车总线数据都从这里获得
**  创建日期: 2013.12.19
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "uart1.h"
#include "uart2.h"
#include "utils.h"
#include "include.h"
#include "data_public.h"
#include "data_train.h"
#include "data_box.h"
#include "progupdate.h"
#include "led_control.h"
#include "esu.h"
#include "stat_switch.h"
#include "esu.h"
#include "set_vlum.h"



static u32 gNoDataTick;

static u8 gTrainRecvPacketBuf[256];
static u8 gTrainRecvPaketLength = 0;

static u8 gOtherSideDriverTalk = 0;
static u8 gOtherSidePasTalk = 0;
static u8 gOtherSidePa = 0;
static u8 gOtherSideKeySignal = 0;


static tPaPriority gPaPriority;



u8 TrainDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	uart2_write_buf( sendBuf, sendLen );
        return sendLen;
}


const u8 esu_fill_shift[4] = {0, 2, 4, 6};
static void TrainDataTokenReply( void )
{
	tBcuHeader *bcbHeader;
	//u8 tempBuf[100];
	u8 vh, vl;
	u8 i, j;
	u8 temp, temp1;
	u32 tick;

	#if 1
	tick = gSystemTick;
	while(gSystemTick-tick<=3);

	//memset(tempBuf, 0, sizeof(tempBuf));

	//bcbHeader = (tBcbHeader*)tempBuf;


	gBcuData.dest_netnum = (u8)0x00;
	gBcuData.dest_eqnum.eq_type = (u8)0x0f;
	gBcuData.dest_eqnum.eq_num = (u8)0x0f;
	gBcuData.src_netnum = (u8)0x00;
	gBcuData.src_eqnum.eq_type = DEVICE_TYPE_BCB;
	gBcuData.src_eqnum.eq_num = GetDeviceId();
	gBcuData.cmd = (u8)0x00;
	gBcuData.len = (u8)0x17;  //23

	gBcuData.data_area.trigger_hbit.man_req = GetRequestBroadcastFlag();
	gBcuData.data_area.trigger_hbit.man_ing = GetNowBroadcastFlag();
	gBcuData.data_area.status_hbit.talk_st = GetDriverTalkStat();
	
	if ( ALARM_STEP_CNCT ==EhpGetAllAlarmStep() )
		gBcuData.data_area.status_hbit.pas_talk = 1;
	else 
		gBcuData.data_area.status_hbit.pas_talk = 0;
	
	gBcuData.data_area.status_hbit.active = GetKeySignal();

	//报警状态填充
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN; i++) {
		temp1 = 0;
		for (j=0; j<MAX_EHP_NUM_PER_CAR; j++) {
			temp = (u8)gAllAlarmSt[i*MAX_EHP_NUM_PER_CAR+j];
			temp >>= esu_fill_shift[j];
			temp1 |= temp;
		}
		gBcuData.data_area.esu_status[i] = temp1;
	}
	for (; i<8; i++) {
		gBcuData.data_area.esu_status[i] = 0;
	}
	//END 报警状态填充

	gBcuData.data_area.volume.moni_broadcast = VolumeGetMoniBroadcast();
	gBcuData.data_area.volume.moni_driver_talk = VolumeGetMoniDriverTalk();
	gBcuData.data_area.volume.moni_pas_talk = VolumeGetMoniPasTalk();
	gBcuData.data_area.volume.mic_broadcast = VolumeGetMicBroadcast();
	gBcuData.data_area.volume.mic_driver_talk = VolumeGetMicDriverTalk();
	gBcuData.data_area.volume.mic_pas_talk = VolumeGetMicPasTalk();
	
	
	GetSoftwareVersion(&vh, &vl);
	gBcuData.data_area.version_h= vh;
	gBcuData.data_area.version_l= vl;

	TrainDataSend( (u8 *)&gBcuData, sizeof(tBcuHeader) );
	gHmiData.data_area.dcp_reply_tiems++;
#endif

	
#if 0
	tick = gSystemTick;
	while(gSystemTick-tick<=3);

	memset(tempBuf, 0, sizeof(tempBuf));

	bcbHeader = (tBcbHeader*)tempBuf;

	bcbHeader->dest_netnum = (u8)0x00;
	bcbHeader->dest_eqnum.eq_type = (u8)0x0f;
	bcbHeader->dest_eqnum.eq_num = (u8)0x0f;
	bcbHeader->src_netnum = (u8)0x00;
	bcbHeader->src_eqnum.eq_type = DEVICE_TYPE_BCB;
	bcbHeader->src_eqnum.eq_num = GetDeviceId();
	bcbHeader->cmd = (u8)0x10;
	bcbHeader->len = (u8)0x16;  //22

	bcbHeader->data_area.trigger_hbit.man_req = GetRequestBroadcastFlag();
	bcbHeader->data_area.trigger_hbit.man_ing = GetNowBroadcastFlag();
	bcbHeader->data_area.status_hbit.talk_st = GetDriverTalkStat();
	if ( ALARM_STEP_CNCT ==EhpGetAllAlarmStep() ) bcbHeader->data_area.status_hbit.pas_talk = 1;
	else bcbHeader->data_area.status_hbit.pas_talk = 0;
	bcbHeader->data_area.status_hbit.active = GetKeySignal();

	//报警状态填充
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN; i++) {
		temp1 = 0;
		for (j=0; j<MAX_EHP_NUM_PER_CAR; j++) {
			temp = (u8)gAllAlarmSt[i*MAX_EHP_NUM_PER_CAR+j];
			temp >>= esu_fill_shift[j];
			temp1 |= temp;
		}
		bcbHeader->data_area.esu_status[i] = temp1;
	}
	for (; i<8; i++) {
		bcbHeader->data_area.esu_status[i] = 0;
	}
	//END 报警状态填充

	bcbHeader->data_area.volume.moni_broadcast = VolumeGetMoniBroadcast();
	bcbHeader->data_area.volume.moni_driver_talk = VolumeGetMoniDriverTalk();
	bcbHeader->data_area.volume.moni_pas_talk = VolumeGetMoniPasTalk();
	bcbHeader->data_area.volume.mic_broadcast = VolumeGetMicBroadcast();
	bcbHeader->data_area.volume.mic_driver_talk = VolumeGetMicDriverTalk();
	bcbHeader->data_area.volume.mic_pas_talk = VolumeGetMicPasTalk();
	
	
	GetSoftwareVersion(&vh, &vl);
	bcbHeader->data_area.version_h= vh;
	bcbHeader->data_area.version_l= vl;

	TrainDataSend( tempBuf, 28 );
#endif

}


const u8 shiftNumBuf[EHP_MAX_NUM] = { 6, 4, 2 };

/*
功能:获得对端司机室是否在紧急对讲
返回: 0 空闲        1正在进行紧急对讲
*/
u8 GetOtherSidePasTalkStat( void )
{
	return gOtherSidePasTalk;
}

/*
功能:获得对端司机室是否在司机对讲
返回: 0 空闲       >0 有司机对讲
*/
u8 GetOtherSideDriverTalkStat( void )
{
	return gOtherSideDriverTalk;
}

u8 GetOtherSidePaStat( void )
{
	return gOtherSidePa;
}

u8 GetOtherSideKeySignal( void )
{
	return gOtherSideKeySignal;
}



 void TrainDataBcbAnalyse(u8 *buf)
 {
	tBcuHeader *bcbData;

	bcbData = (tBcuHeader*)buf;

	if (bcbData->data_area.status_hbit.talk_st == 0) gOtherSideDriverTalk = 0;
	else gOtherSideDriverTalk = 1;

	if (bcbData->data_area.status_hbit.pas_talk == 0) gOtherSidePasTalk = 0;
	else gOtherSidePasTalk = 1;

	if (bcbData->data_area.trigger_hbit.man_ing == 0) gOtherSidePa= 0;
	else gOtherSidePa = 1;

	if (bcbData->data_area.status_hbit.active == 0) gOtherSideKeySignal = 0;
	else gOtherSideKeySignal = 1;
	
 }

u32 GetTrainComTick(void)
{
	return gPaPriority.tick_485;
}

/*
	return 0,    MANUAL IS THE HIGHEST
	return 1,   MANUAL IS  LOW
*/
u8 PaArbitrate(void)
{
	if (gPaPriority.type == PA_TYPE_OCC) {
		if (gPaPriority.manual > gPaPriority.occ) return 0;
		else return 1;
	}
	else if (gPaPriority.type == PA_TYPE_MANUAL) {
		if (gPaPriority.manual > gPaPriority.manual) return 0;
		else return 1;
	}
	else if (gPaPriority.type == PA_TYPE_DVA) {
		if (gPaPriority.manual > gPaPriority.dva) return 0;
		else return 1;
	}	
	else if (gPaPriority.type == PA_TYPE_MEDIA) {
		if (gPaPriority.manual > gPaPriority.lcd_media) return 0;
		else return 1;
	}	
	else if (gPaPriority.type == PA_TYPE_EME) {
		if (gPaPriority.manual > gPaPriority.eme) return 0;
		else return 1;
	}	
	else if (gPaPriority.type == PA_TYPE_CLOSE_DOOR) {
		if (gPaPriority.manual > gPaPriority.close_door) return 0;
		else return 1;
	}
	else return 0;
}


 void TrainDataPiscAnalyse(u8 *buf)
{
	tPiscHeader *piscData;
	piscData = (tPiscHeader *)buf;
	u8 mode, volume;

	memcpy((u8 *)&gPiscData , buf , sizeof(tPiscHeader)); //保存中央控制器过程数据
	
	gPaPriority.tick_485 = gSystemTick;

	gPaPriority.occ = piscData->data_area.broadcast_priority.occ;
	gPaPriority.manual = piscData->data_area.broadcast_priority.manual;
	gPaPriority.dva = piscData->data_area.broadcast_priority.dva;
	gPaPriority.lcd_media = piscData->data_area.broadcast_priority.media;
	gPaPriority.eme = piscData->data_area.broadcast_priority.special;
	gPaPriority.close_door = piscData->data_area.broadcast_priority.door;
	gPaPriority.type = (tPaType)piscData->data_area.broadcast_signal.broadcast_type;

	mode = piscData->data_area.panel_volume.mode;
	volume = piscData->data_area.panel_volume.value;
	SaveVolume(mode, volume);

}




static u8 TrainDataGetPacket(void)
{
	u8 temp;
	while(uart2_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
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




void TrainDataProc(void)
{
	tFrameHeader *frameHeader;
	tPiscHeader *piscData;
	tLcuHeader *lcuData;
	u8 deviceId;
	u8 len;
	u8 buf[2];

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
					
					//是否为过程数据
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_PISC ) 
					{
						piscData = (tPiscHeader *)gTrainRecvPacketBuf;
						//是否为主
						if ( piscData->data_area.status_hbit.master ) 
						{
							TrainDataPiscAnalyse( gTrainRecvPacketBuf );
						}
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB ) 
					{
						TrainDataBcbAnalyse( gTrainRecvPacketBuf );
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_LCU )
					{
						lcuData = (tLcuHeader *)gTrainRecvPacketBuf;
						buf[0] = lcuData->data_area.ehp_st;
						EhpLcuData( buf, lcuData->src_eqnum.eq_num-1 );
					}
					break;
				case TOKEN_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_BCB )
					{
						deviceId = GetDeviceId();
						if ( frameHeader->dest_eqnum.eq_num == deviceId )
						{
							TrainDataTokenReply();
						}
					}					
					break;
				case DOWNLOAD_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_BCB )
					{
						ProgramUpdate( gTrainRecvPacketBuf );
					}
					else 
					{
						
					}
					break;
				default :
					break;
			}
			gNoDataTick = gSystemTick;
		}
		gTrainRecvPaketLength = 0;
	}

	if ( gSystemTick-gNoDataTick > 2000 )
	{
		LED_TRAIN_COM_OPEN();
	}
	else 
	{
		if (!uart2_busy())
		{
			LED_TRAIN_COM_CLOSE();
		}
	}

}


void TrainDataInit(void)
{

	//发送版本号
//	buf[0] = DEVICE_SOFTWARE_VERSION_HIGHT;
//	buf[1] = DEVICE_SOFTWARE_VERSION_LOW;
//	TrainDataSend( buf, 2 );

}




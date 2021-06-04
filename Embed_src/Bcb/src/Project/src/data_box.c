/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理车辆总线数据，其他模块获取车辆总线数据都从这里获得
**  创建日期: 2013.12.19
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "data_public.h"
#include "data_box.h"
#include "uart1.h"
#include "uart2.h"
#include "utils.h"
#include "device_info.h"
#include "data_public.h"
#include "include.h"
#include "set_vlum.h"
#include "led_control.h"

#define BROADCAST_TYPE_OCC		0x00	//无线广播
#define BROADCAST_TYPE_PC		       0x01	//乘客报警
#define BROADCAST_TYPE_CC			0x02	//司机对讲
#define BROADCAST_TYPE_PA			0x03	//人工广播
#define BROADCAST_TYPE_DOOR		0x04	//关门提示音
#define BROADCAST_TYPE_DVA		0x05	//数字报站
#define BROADCAST_TYPE_MEDIA		0x06	//媒体播放
#define BROADCAST_TYPE_SPECIAL	0x07	//紧急广播
#define BROADCAST_TYPE_TEST		0x08	//测试音
extern void DebugPrintVolume(void);

static u32 gNoDataTick;

static u8 gBoxRecvPaketBuf[256];
static u8 gBoxRecvPaketLength = 0;

//static u8 gHmiRecvPaketBuf[256];
//static u8 gHmiRecvPaketLength = 0;
 


static u8 gKeyPcSt = 1;
static u8 gKeyCcSt = 1;
static u8 gKeyPASt = 1;
static u8 gKeyPttSt = 1;
static u8 gKeyPcRstSt = 1;


static u8 gKeyPcStOld = 0;
static u8 gKeyCcStOld = 1;
static u8 gKeyPAStOld = 1;


u8 BoxDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;
	static u32 tick;

	if (uart1_busy())
	{
		tick = gSystemTick;
		return 0;
	}
	if (gSystemTick - tick < 200) return 0;
	
	sendLen = PackageData( sendBuf, buf, length );
	uart1_write_buf( sendBuf, sendLen );
        return 0;
}

u8 BoxDataGetPacket(void)
{
	u8 temp;
	while(uart1_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			//if(gBoxRecvPaketLength >= 3)
			if((gBoxRecvPaketLength >=5))
				//(gBoxRecvPaketBuf[3]&0x0f==0x09 ||gBoxRecvPaketBuf[3]&0x0f==0x0A))
			{
				return 1;
			}
			gBoxRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gBoxRecvPaketBuf[gBoxRecvPaketLength] = temp;
			if( ++gBoxRecvPaketLength >= 255 )
			{
				gBoxRecvPaketLength = 0;
			}
		}
	}
	return 0;
}

void BoxDataAlarmLedFlash(void)
{
	gBoxData.data[0] = 0x01;
}

void BoxDataAlarmLedOpen(void)
{
	gBoxData.data[0] = 0x02;		
}

void BoxDataAlarmLedClsoe(void)
{
	gBoxData.data[0] = 0x03;
}

eKeyType GetKeyValue(void)
//#ifdef suven
#if 1
{
	eKeyType key = KEY_TYPE_NULL; 
	static eKeyType keyOld = KEY_TYPE_NULL;
	u8 highest_priority=0;
	u8 broadcast_priority =0;
	u8 buf[8];
	u8 cnt=0;
	u8 index=0;
	u8 highest_index =0xFF;

	//固定优先级
	#if 1
		// 优先级排列 CC >PA>PC 					

		if(gKeyPcSt==0 && gKeyPcSt != gKeyPcStOld)
		{
			gKeyPcStOld = gKeyPcSt;		
			if (gBoxData.data[0]== 0) // pecu st = 空闲
				key = KEY_TYPE_NULL;
			if (gBoxData.data[0]== 1) // pecu st = 报警  接通  suven   如果有多个报警gBoxData.data状态是否还为2，根据灯的显示应该是2
				key = KEY_TYPE_PC_CONN;  
			if (gBoxData.data[0]== 2) // pecu st = 通话  讲
				key = KEY_TYPE_PC_DOWN;				
			//return key;
		}		
		
		if(gKeyPcSt==1)
		{
			gKeyPcStOld = gKeyPcSt;	
			if (gBoxData.data[0]== 2) // pecu st = 通话  听
			{
				key = KEY_TYPE_PC_UP;		
				//return key;
			}
		}

		if(gKeyPcRstSt==0)  
		{
			key = KEY_TYPE_PC_DISCONN;    //pecu 复位
			//return key;
		}
		
		if (gKeyPASt == 0) 
			key = KEY_TYPE_PA;
		
		if (gKeyCcSt == 0)  
			key = KEY_TYPE_CC;
		return key;

	#else

	buf[BROADCAST_TYPE_PC] = gPiscData.data_area.broadcast_priority.pecu;
	buf[BROADCAST_TYPE_CC] = gPiscData.data_area.broadcast_priority.talk;
	buf[BROADCAST_TYPE_PA] = gPiscData.data_area.broadcast_priority.manual;
	buf[BROADCAST_TYPE_OCC] = gPiscData.data_area.broadcast_priority.occ;
	buf[BROADCAST_TYPE_DVA] = gPiscData.data_area.broadcast_priority.dva;
	buf[BROADCAST_TYPE_MEDIA] = gPiscData.data_area.broadcast_priority.media;	
	buf[BROADCAST_TYPE_DOOR] = gPiscData.data_area.broadcast_priority.door;
	buf[BROADCAST_TYPE_SPECIAL] = gPiscData.data_area.broadcast_priority.special;

	broadcast_priority = gPiscData.data_area.broadcast_signal.broadcast_type;  //当前系统的广播

	for(index=0 ;index<8; index++)
	{
		for(cnt=0 ;cnt<8; cnt++)
		{
			if(buf[cnt]>highest_priority)
			{
				highest_priority = buf[cnt];
				highest_index =cnt;
			}	
		}

		if(highest_index == BROADCAST_TYPE_CC && broadcast_priority <=buf[BROADCAST_TYPE_CC]) //CC
		{
			if(gKeyCcSt == 0)
			{
				key = KEY_TYPE_CC;
				return key;
			}
		}
		else if(highest_index == BROADCAST_TYPE_PA && broadcast_priority <=buf[BROADCAST_TYPE_PA])//PA
		{
			if(gKeyPASt == 0)
			{
				key = KEY_TYPE_PA;
				return key;
			}
		}

		else if(highest_index == BROADCAST_TYPE_PC && broadcast_priority <=buf[BROADCAST_TYPE_PC])//PC
		{
			if(gKeyPcRstSt==0)  
			{
				key = KEY_TYPE_PC_DISCONN;    //pecu 复位
				return key;
			}			

			if(gKeyPcSt==0 && gKeyPcSt != gKeyPcStOld)
			{
				gKeyPcStOld = gKeyPcSt;		
				if (gBoxData.data[0]== 0) // pecu st = 空闲
					key = KEY_TYPE_NULL;
				if (gBoxData.data[0]== 1) // pecu st = 报警  接通  suven   如果有多个报警gBoxData.data状态是否还为2，根据灯的显示应该是2
					key = KEY_TYPE_PC_CONN;  
				if (gBoxData.data[0]== 2) // pecu st = 通话  讲
					key = KEY_TYPE_PC_DOWN;				
				return key;
			}
			if(gKeyPcSt==1)
			{
				gKeyPcStOld = gKeyPcSt;	
				if (gBoxData.data[0]== 2) // pecu st = 通话  听
				{
					key = KEY_TYPE_PC_UP;		
					return key;
				}
			}				
		}
		buf[highest_index] =0;  //重新寻找高优先级
		highest_priority = 0;
	}

	return key;
	#endif
}
#else
{
	eKeyType key; 
	static eKeyType keyOld = KEY_TYPE_NULL;
	char flag = 0;

	if (gKeyPASt != gKeyPAStOld)
	{
		flag = 1;
		gKeyPAStOld = gKeyPASt;
		if (gKeyPASt == 0) key = KEY_TYPE_PA;
		else key = KEY_TYPE_IDLE;
	}
	else if (gKeyCcSt != gKeyCcStOld)
	{
		flag = 1;
		gKeyCcStOld = gKeyCcSt;
		if (gKeyCcSt == 0) key = KEY_TYPE_CC;
		else key = KEY_TYPE_IDLE;
	}
	else if (gKeyPcSt != gKeyPcStOld)
	{
	 	flag = 1;
		gKeyPcStOld = gKeyPcSt;
		if (gKeyPcSt == 1) key = KEY_TYPE_PC_CONN;
		else if (gKeyPcSt == 2) key = KEY_TYPE_PC_DISCONN;
		else key = KEY_TYPE_NULL;
	}
	else 
	{
	}
	if (flag == 1)
	{
		if (key == KEY_TYPE_PC_DISCONN) keyOld = KEY_TYPE_NULL;
		else keyOld = key;
	}
	else 
	{
		key = keyOld;
	}

	return key;
}
#endif

/*
eKeyType GetPttStat(void)
{
	if (gKeyPttSt == 0) return KEY_TYPE_PTT_DOWN;
	else return KEY_TYPE_PTT_UP;
}*/

void BoxDataProc(void)
{
	//tFrameHeader *frameHeader;
	tLcdHeader *LcdHeader;
	static u8 car_token=0;
	//static u8 token_index=0;
	static u32 token_time=0;
	//static u16 hmi_interval=0;
	//static u16 box_interval=0;

	tLogicFlag paflag =INVALID;
	static u32 hmi_time = 0;
	static u32 box_time = 0 ;

	static u8 screen_first_cnt=0;  //触摸屏回复的过程中不巡检box的计数标志。
	static u8 screen_ack_flag =0 ;
	
	u8 len;
	u8 dcp_reply=0;

	// 1. 接收数据处理
	if (BoxDataGetPacket()) 
	{
		len = VerifyPaket( gBoxRecvPaketBuf, gBoxRecvPaketLength );
		if ( len > 0 ) 
		{
			switch(gBoxRecvPaketBuf[4] & 0x0f)
			{
				case PROCESS_PACKET:		
					car_token = 1; //收到回复收回令牌权
					
					if (( gBoxRecvPaketBuf[3]&0x0f)== DEVICE_TYPE_BOX) 
					{
						gNoDataTick = gSystemTick;
						LedSetBoxDataStat(LED_BOX_COM_HAVE_DATA);
						
						LcdHeader = (tLcdHeader *)gBoxRecvPaketBuf;

						//站代码
						gBcuData.data_area.start_station = LcdHeader->data_area.beginStation;
						gBcuData.data_area.end_station= LcdHeader->data_area.endStation;
						gBcuData.data_area.current_station= LcdHeader->data_area.currentStation;
						gBcuData.data_area.next_station= LcdHeader->data_area.nextStation;
						gBcuData.data_area.jump_stations= LcdHeader->data_area.passStationBro;
						gBcuData.data_area.urgent_broadcast_no= LcdHeader->data_area.urgentBroCode;

						//触发标志
						gBcuData.data_area.trigger_hbit.leave_broadcast= LcdHeader->data_area.broadcast_hbit.leave_broadcast;
						gBcuData.data_area.trigger_hbit.arrived_broadcast= LcdHeader->data_area.broadcast_hbit.arrived_broadcast;
						gBcuData.data_area.trigger_hbit.jump= LcdHeader->data_area.broadcast_hbit.jump;
						gBcuData.data_area.trigger_hbit.urgent_broadcast= LcdHeader->data_area.broadcast_hbit.urgent_broadcast;
						gBcuData.data_area.trigger_hbit.brdcst_stop= LcdHeader->data_area.broadcast_hbit.stopBroadcast;

						//模式线路
						gBcuData.data_area.line_mode.city_id =  LcdHeader->data_area.line_mode.cityId;

						//工作模式
						gBcuData.data_area.line_mode.pisc_mode = LcdHeader->data_area.work_mode.workmode;

						//监听音量
						gBcuData.data_area.line_mode.moni_broadcast_off = LcdHeader->data_area.line_mode.moni_broadcast_off;

						//更新广播控制盒数据部分
						gKeyPASt = LcdHeader->key_value[0];
						gKeyCcSt = LcdHeader->key_value[1];
						gKeyPcSt = LcdHeader->key_value[2];
						gKeyPttSt = LcdHeader->key_value[3];
						gKeyPcRstSt = LcdHeader->key_value[4];
					}
					break;
				case TOKEN_PACKET:
					break;
				default :
					break;
			}
		}
		gBoxRecvPaketLength = 0;
	}

	// 2. 巡检处理
	if(car_token)
	{
		token_time = gSystemTick ;
		gHmiData.dest_netnum = (u8)0x00;   					//目标网络
		gHmiData.dest_eqnum.eq_type = (u8)0x09; 				//目标设备类型
		gHmiData.dest_eqnum.eq_num = (u8)0x01; 				//目标设备id
		gHmiData.src_netnum = (u8)0x00;						//源网络
		gHmiData.src_eqnum.eq_type = DEVICE_TYPE_BCB;			//源设备类型
		gHmiData.src_eqnum.eq_num = GetDeviceId();			//源设备id
		gHmiData.cmd.packet_type = TOKEN_PACKET;				//数据包类型
		gHmiData.len= gPiscData.len + 3;
		memcpy((u8 *)&gHmiData.data_area, (u8 *)&gPiscData.data_area, sizeof(pisc_data_t));
		gHmiData.data_area.status_hbit.active = GetKeySignal();
		
		gHmiData.boxdata[0] = gBoxData.data[0];
		gHmiData.boxdata[1] = GetDriverTalkStat();       				//本侧cc 状态 为 2

		if(gHmiData.boxdata[1] == 0)
			gHmiData.boxdata[1] = GetOtherSideDriverTalkStat();     // 对侧cc 值为1  

		paflag = GetNowBroadcastFlag();											//pa 状态  2
		if(paflag == VALID)
			gHmiData.boxdata[2] = 	0x02;		
		else
			gHmiData.boxdata[2] = 	0x00;

		if(gHmiData.boxdata[2] == 0)
			gHmiData.boxdata[2] = GetRequestBroadcastFlag();			//pa 申请状态  1

		BoxDataSend((u8 *)&gHmiData, sizeof(tHmiHeader));
		car_token =0;
	}	

	if(gSystemTick - token_time >120)  //巡检超时
		car_token =1;
	

	if ( gSystemTick-gNoDataTick > 5000 )
	{
		LedSetBoxDataStat(LED_BOX_COM_NO_DATA);
	}
	
}



void BoxDataInit()
{
	//发送版本号
//	buf[0] = DEVICE_SOFTWARE_VERSION_HIGHT;
//	buf[1] = DEVICE_SOFTWARE_VERSION_LOW;
//	BoxDataSend( buf, 2 );

	//gKeyType = KEY_TYPE_NULL;
	LedSetBoxDataStat(LED_BOX_COM_NO_DATA);
	gBcuData.data_area.line_mode.pisc_mode = 0x01;   //默认自动模式
}



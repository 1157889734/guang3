/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理紧急报警状态切换
**  创建日期: 2013.12.28
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
#include "set_vlum.h"
#include "stat_switch.h"

//#define suven   // 使用新的广播操作方法

static u32 gPaTick;


//总的状态
static eTalkStat gTalkStat;
static eTalkStat gTalkStatOld;


static tLogicFlag gRequestBroadcastFlag = INVALID;
static tLogicFlag gNowBroadcastFlag = INVALID;

static u8 gPaLineStat = 1;
static u8 gPcLineStat = 1;


//本端司机对讲状态 0:空闲   1:听     2:讲
static u8 gDriverTalkStat = 0;

eTalkStat GetTalkStat(void)
{
	return gTalkStat;
}

//总得状态设置
void SetTalkStat(eTalkStat stat)
{
	gTalkStat = stat;
}

void TalkStatPasAlarmLedCtrl( void )
{
	tAlarmStep alarmStep;

	alarmStep = EhpGetAllAlarmStep();
	if ( ALARM_STEP_IDLE==alarmStep )
	{
		BoxDataAlarmLedClsoe();
	}
	else if ( ALARM_STEP_CALL==alarmStep )
	{
		//发给盒子
		BoxDataAlarmLedFlash();		
		//本地PC LED 闪烁
		LedSetPcStat(LED_PC_CALL);
	}
	else if ( ALARM_STEP_CNCT==alarmStep )
	{
		BoxDataAlarmLedOpen();
		LedSetPcStat(LED_PC_CONN);
	}		

}


static u8 doubleKeyFlag = 0;

void TalkStatPasAlarmRingCtrl( void )
{
	tAlarmStep alarmStep;

	alarmStep = EhpGetAllAlarmStep();
	if ( ALARM_STEP_IDLE==alarmStep )
	{
		if (doubleKeyFlag == 0)
		{
			PLAY_VOICE_DISABLE();
			MONI_SELECT_PAPC();
		}
	}
	else if ( ALARM_STEP_CALL==alarmStep )
	{
		PLAY_VOICE_ENABLE();
		MONI_SELECT_RING();
		MONITOR_CONN_ENABLE();
	}
	else if ( ALARM_STEP_CNCT==alarmStep )
	{
		if (doubleKeyFlag == 0)
		{
			PLAY_VOICE_DISABLE();
			MONI_SELECT_PAPC();
		}
	}		

}


void BothSideKeySignalRingCtrl(void)
{
	tAlarmStep alarmStep;

	alarmStep = EhpGetAllAlarmStep();
	if (ALARM_STEP_CALL != alarmStep)
	{
		if ( (1 == GetKeySignal()) && (1 == GetOtherSideKeySignal()) )
		{
			doubleKeyFlag = 1;
			PLAY_VOICE_ENABLE();
			MONI_SELECT_RING();
			MONITOR_CONN_ENABLE();
		}
		else 
		{
			doubleKeyFlag = 0;
			PLAY_VOICE_DISABLE();
			MONI_SELECT_PAPC();
		}
	}
}

void TalkStatEhpConnCreate(char recnnct)
{
	SetVolume( VOLUME_TYPE_MONI_PAS_TALK);
	SetVolume( VOLUME_TYPE_MIC_PAS_TALK);
	SetTalkStat(TALK_STAT_PAS_TALK);
	PLAY_VOICE_DISABLE();
	MIC_AUDIO_TO_PC();
	MONI_SELECT_PC();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();
	LedSetPcStat(LED_PC_CONN);
	if (0==recnnct)
	{
		EhpAlarmCnctStatUpdate();
	}
}

void TalkStatPAConnCreate(void)
//#ifdef suven
#if 1
{
	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	SetVolume( VOLUME_TYPE_MIC_BROADCAST);
	gRequestBroadcastFlag = INVALID;
	SetTalkStat(TALK_STAT_BROADCAST);
	LedSetPaStat(LED_PA_OPEN);
	gNowBroadcastFlag = VALID;
	SetLocalPaLineStat(0);

	gPaTick = gSystemTick;
}
#else
{
	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	SetVolume( VOLUME_TYPE_MIC_BROADCAST);
	gRequestBroadcastFlag = INVALID;
	SetTalkStat(TALK_STAT_BROADCAST);
	MIC_AUDIO_TO_PA();
	MONI_SELECT_PA();
	MONI_SELECT_PAPC();
	LedSetPaStat(LED_PA_OPEN);

	gNowBroadcastFlag = VALID;
	MONITOR_CONN_DISABLE();
	SetLocalPaLineStat(0);
	PA_DIRECT_CONN_ENABLE();


	gPaTick = gSystemTick;
}
#endif

void TalkStatCCConnCreate(void)
{
	SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
	SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
	SetTalkStat(TALK_STAT_DRIVER_TALK);
	MIC_AUDIO_TO_PC();
	MONI_SELECT_PC();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();
	
}


void TalkStatEhpConnDestroy(void)
{
	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	PLAY_VOICE_DISABLE();
	MIC_AUDIO_TO_PA();
	MONI_SELECT_PA();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();

	EhpAlarmDestroyStatUpdate();	
	LedSetPcStat(LED_PC_CLOSE);
	
	if ( TALK_STAT_PAS_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
	}
	else if ( TALK_STAT_BROADCAST == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatPAConnCreate();
	}
	else if ( TALK_STAT_DRIVER_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatCCConnCreate();
	}
	else 
	{
		SetTalkStat(TALK_STAT_IDLE);
	}
}

void TalkStatPAConnDestroy(void)
{
	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	gNowBroadcastFlag = INVALID;
	gRequestBroadcastFlag = INVALID;
	SetLocalPaLineStat(1);
	MIC_AUDIO_TO_PA();
	MIC_CONN_DISABLE();	
	MONI_SELECT_PA();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();
	LedSetPaStat(LED_PA_CLOSE);
	PA_DIRECT_CONN_DISABLE();

	if ( TALK_STAT_PAS_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatEhpConnCreate(1);
	}
	else if ( TALK_STAT_BROADCAST == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
	}
	else if ( TALK_STAT_DRIVER_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatCCConnCreate();
	}
	else 
	{
		SetTalkStat(TALK_STAT_IDLE);
	}
}

void TalkStatCCConnDestroy(void)
{
	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	gDriverTalkStat = 0x00;
	SetLocalPcLineStat(1);
	PLAY_VOICE_DISABLE();
	MIC_AUDIO_TO_PA();
	MONI_SELECT_PA();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();
	LedSetPcStat(LED_PC_CLOSE);

	if ( TALK_STAT_PAS_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatEhpConnCreate(1);
	}
	else if ( TALK_STAT_BROADCAST == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
		TalkStatPAConnCreate();
	}
	else if ( TALK_STAT_DRIVER_TALK == gTalkStatOld )
	{
		gTalkStatOld = TALK_STAT_IDLE;
	}
	else 
	{
		SetTalkStat(TALK_STAT_IDLE);
	}
}


void PcLineStatUpdate(u8 stat)
{
	gPcLineStat = stat;
}

void PaLineStatUpdate(u8 stat)
{
	gPaLineStat = stat;
}



u8 GetOtherPcLineStat(void)
{
	//如果是自己拉低，则检测到的结果强制为高电平
	if (0 == GetLocalPcLineStat()) return 1;
	else return gPcLineStat;
}

u8 GetOtherPaLineStat(void)
{
	//如果是自己拉低，则检测到的结果强制为高电平
	if (0 == GetLocalPaLineStat()) return 1;
	else return gPaLineStat;
}


tLogicFlag GetRequestBroadcastFlag(  void )
{
	return gRequestBroadcastFlag;
}

tLogicFlag GetNowBroadcastFlag(  void )
{
	return gNowBroadcastFlag;
}

u8 GetDriverTalkStat( void )
{
	return gDriverTalkStat;
}



void WaitRequestBroadcast(void )
{
	u32 tick;

	//如果TRAIN485 3秒内没有钥匙信号的PISC数据，则直接占用PA线
	tick = GetTrainComTick();
	if (gSystemTick - tick > 600)
	{
		TalkStatPAConnCreate();
		return;
	}

	//先申请等待2.5秒，然后再去检测PA总线
	if ( gSystemTick - gPaTick>200 )
	{
		if (1 == GetOtherPaLineStat())
		{
			TalkStatPAConnCreate();
		}

		if ((0 == PaArbitrate()) && (gSystemTick - gPaTick>200))
		{
			TalkStatPAConnCreate();
		}
		
/*	
		while(0 == GetOtherPaLineStat())
		{
			if (1 == PaArbitrate())
			{
				LedSetPaStat(LED_PA_CLOSE);
				gRequestBroadcastFlag = INVALID;
				SetTalkStat(TALK_STAT_IDLE);
				return;
			}
			else if ( gSystemTick - gPaTick>6000 )
			{
				break;
			}
		}
*/		
	}
}

 //监听pa线
void MoniBroadcast(void )
{
	if (doubleKeyFlag == 1) return;

	SetVolume( VOLUME_TYPE_MONI_BROADCAST);
	gDriverTalkStat = 0x00;
	MONI_SELECT_PA();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();
	MIC_CONN_DISABLE();
	LedSetPcStat(LED_PC_CLOSE);
}

 //人工广播MIC设置
 void MicBroadcast(void)
 {
	gNowBroadcastFlag = VALID;
	MIC_CONN_ENABLE();		//麦克风声音打开	
	MIC_AUDIO_TO_PA();  //麦克风选择pa输出				
	SetLocalPaLineStat(0);   //占用pa线
	PA_DIRECT_CONN_ENABLE(); // 占列车总线		
	MONITOR_CONN_DISABLE();  //关闭监听扬声器
 }

 //监听pc线
void Monipc(void )
{
	SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
	
	MONI_SELECT_PC();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();	
	MIC_CONN_DISABLE();
}

// CC 时MIC 设置
void MicCC(void)
{
	gDriverTalkStat = 0x02;
	SetLocalPcLineStat(0); //占用pc线
	MIC_AUDIO_TO_PC(); //麦克选择输出到pc
	MIC_CONN_ENABLE(); //打开麦克风输出
	MONITOR_CONN_DISABLE(); //关闭扬声器监听
}

void TalkStatSwitch(void)
//#ifdef suven
#if 1
{
	eTalkStat  stat;
	eKeyType key;
	//u8 close_pc_flag;
	//tAlarmStep alarmStep;
	static eKeyType keyOld = KEY_TYPE_NULL;  //记录按键值，因为广播控制盒只发送两次按键
	eKeyType pttKey;
	//u8 cancelPa = 0;
	//u8 ccNow = 0;
	//u8 paPttDown = 0;

	//cancelPa = 0;
	//close_pc_flag = 1;

	//不论什么状态，不论是否激活，盒子的报警灯还是要亮的
	TalkStatPasAlarmLedCtrl();

	key = GetKeyValue();
	//pttKey = GetPttStat();

	ClearEhpStat();
	BothSideKeySignalRingCtrl();
	
	stat = GetTalkStat();

	switch(key)
	{
		case KEY_TYPE_PC_CONN:  //接通紧急报警
			
			if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )//是否乘客报警
			{
				//清除当前状态
				if( stat == TALK_STAT_BROADCAST 
				&&  stat == TALK_STAT_REQUEST_BROADCAST)  //当前状态是pa
					TalkStatPAConnDestroy(); 
				if( stat == TALK_STAT_DRIVER_TALK) //当前状态是cc
					TalkStatCCConnDestroy(); 
				
				gTalkStatOld = TALK_STAT_IDLE ;   //挂断后直接进入idle状态					
				TalkStatEhpConnCreate(0); //切换到乘客报警状态
				break;
			}
			
			break;
			
		case KEY_TYPE_PC_DISCONN:  //挂断紧急报警
			
			if ( ALARM_STEP_CNCT==EhpGetAllAlarmStep() ) //应答状态中切断
			{
				TalkStatEhpConnDestroy();  // 清除pas						
			}
		
			break;
			
		case KEY_TYPE_PA:
			
			if( stat == TALK_STAT_DRIVER_TALK) 
				TalkStatCCConnDestroy(); 		// 清除cc	
			if( stat == TALK_STAT_PAS_TALK) 
				TalkStatEhpConnDestroy();  // 清除pc	
		
			gTalkStatOld = TALK_STAT_IDLE ;   //挂断后直接进入idle状态
			
			#if 1
			//if(GetOtherSideDriverTalkStat()==0) 				
			if(GetOtherPcLineStat()==1)//对端无cc line状态
			{
				if ( 1 == GetKeySignal() ) //本端钥匙
				{
					if( stat != TALK_STAT_REQUEST_BROADCAST && stat != TALK_STAT_BROADCAST) 
					{
						SetTalkStat(TALK_STAT_REQUEST_BROADCAST); //申请pa	
						gPaTick = gSystemTick;
					}
				}
				else  
				{
					if (0 == GetOtherSidePaStat())// && (GetOtherPaLineStat()==1) )  //等待钥匙侧pa 完毕
					{
						if( stat != TALK_STAT_REQUEST_BROADCAST && stat != TALK_STAT_BROADCAST) 
						{
							SetTalkStat(TALK_STAT_REQUEST_BROADCAST); //申请pa	
							gPaTick = gSystemTick;
						}
					}
					else
					{
						if( stat == TALK_STAT_BROADCAST 
						||  stat == TALK_STAT_REQUEST_BROADCAST)  //当前状态是pa
							TalkStatPAConnDestroy(); 
							
						SetTalkStat(TALK_STAT_IDLE); 
					}
				}
			}
			else		
			{
				if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  //当前状态是pa
					TalkStatPAConnDestroy(); 

				SetTalkStat(TALK_STAT_IDLE); 
				
			}
			#endif				
		
			break;
			
		case KEY_TYPE_CC:
			
			if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  
				TalkStatPAConnDestroy();  // 清除pa

			if( stat == TALK_STAT_PAS_TALK) 
				TalkStatEhpConnDestroy();  // 清除pc
			
			gTalkStatOld = TALK_STAT_IDLE ;   //挂断后直接进入idle状态

			if ( 1 == GetKeySignal() ) //本端钥匙
				TalkStatCCConnCreate();
		
			else  //对端无PC
			{
				if((GetOtherSideDriverTalkStat()==0)&& ( 1 == GetOtherPcLineStat()))//等待对侧cc完毕
					TalkStatCCConnCreate();
				else
				{
					if( stat == TALK_STAT_DRIVER_TALK)  //当前状态是cc
						TalkStatCCConnDestroy();
					
					SetTalkStat(TALK_STAT_IDLE); 
				} 
			}		
			break;

		case KEY_TYPE_NULL:
			
			if(stat ==TALK_STAT_PAS_TALK )
				break;

			if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  //当前状态是pa
				TalkStatPAConnDestroy();  

			if( stat == TALK_STAT_DRIVER_TALK) //当前状态是cc
				TalkStatCCConnDestroy(); 

			break;
		
	}
	

	stat = GetTalkStat();
	switch( stat )
	{
		/*********** 空闲状态**************/
		case TALK_STAT_IDLE:		

			if(GetOtherSideDriverTalkStat()|| !GetOtherPcLineStat())  //对侧司机室cc
				Monipc();	 //监听pc 线
			else  
			{
				if((gPiscData.data_area.broadcast_signal.broadcast_type ==0x04)
				||gBcuData.data_area.line_mode.moni_broadcast_off == 1)
					MONITOR_CONN_DISABLE();
				else
					MoniBroadcast();   //监听pa线
			}
			if(ALARM_STEP_CALL != EhpGetAllAlarmStep())//无紧急报警
				LedSetPcStat(LED_PC_CLOSE); 

			LedSetPaStat(LED_PA_CLOSE);

			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//是否有乘客报警, 需要响铃
			}
			
			break;

		/***********人工广播 *************/
		case TALK_STAT_BROADCAST:       
			SetVolume( VOLUME_TYPE_MIC_BROADCAST);		

			MicBroadcast();
			// 占用pa线					
			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//是否有乘客报警, 需要响铃
			}
			
			break;


		/*********** 司机对讲 *************/
		case TALK_STAT_DRIVER_TALK:
			
			SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
			SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
			LedSetPcStat(LED_PC_DRIVER_TALK);

			MicCC();			

			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//是否有乘客报警, 需要响铃
			}
			
			break;

		/***********  乘客紧急对讲*************/
		case TALK_STAT_PAS_TALK:
			SetVolume( VOLUME_TYPE_MONI_PAS_TALK);
			SetVolume( VOLUME_TYPE_MIC_PAS_TALK);
			LedSetPcStat(LED_PC_CONN);
			
			if ( key == KEY_TYPE_PC_DOWN )
			{
				SetLocalPcLineStat(0); //占用pc线
				MIC_CONN_ENABLE();
				MONITOR_CONN_DISABLE();
			}
			else if ( key == KEY_TYPE_PC_UP )
			{
				if (doubleKeyFlag == 0) 
				{
					SetLocalPcLineStat(1); //释放pc线
					MIC_CONN_DISABLE();
					MONITOR_CONN_ENABLE();
				}
			}

			if (EhpIfPasHangUp())
			{
				EhpClearPasHangUp();
				TalkStatEhpConnDestroy();
			}
			
			break;

		/***********  人工广播申请*************/	
		case TALK_STAT_REQUEST_BROADCAST:
			LedSetPaStat(LED_PA_REQUEST);
			gRequestBroadcastFlag = VALID;
			WaitRequestBroadcast();
			if ( key == KEY_TYPE_NULL )
			{
				LedSetPaStat(LED_PA_CLOSE);
				gRequestBroadcastFlag = INVALID;
				SetTalkStat(TALK_STAT_IDLE);
			}

			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//是否有乘客报警, 需要响铃
			}
			
			break;


		/*********** NO ACTION *************/
		default:
			SetTalkStat( TALK_STAT_IDLE );
			break;
	}
}
#else
{
	eTalkStat  stat;
	eKeyType key;
	u8 close_pc_flag;
	tAlarmStep alarmStep;
	static eKeyType keyOld = KEY_TYPE_NULL;  //记录按键值，因为广播控制盒只发送两次按键
	eKeyType pttKey;
	u8 cancelPa = 0;
	u8 ccNow = 0;
	u8 paPttDown = 0;

	cancelPa = 0;
	close_pc_flag = 1;

	//不论什么状态，不论是否激活，盒子的报警灯还是要亮的
	TalkStatPasAlarmLedCtrl();

	key = GetKeyValue();
	pttKey = GetPttStat();

	ClearEhpStat();
	BothSideKeySignalRingCtrl();
	
	stat = GetTalkStat();
	switch( stat )
	{
		case TALK_STAT_IDLE:
			if ( 1 == GetKeySignal() )
			{
				//是否有乘客报警, 需要响铃
				TalkStatPasAlarmRingCtrl();
				if ( key == KEY_TYPE_PC_CONN )
				{
					//是否接通乘客紧急报警
					if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )
					{
						TalkStatEhpConnCreate(0);
						break;
					}
				}
				if ( key == KEY_TYPE_PC_DISCONN )
				{
					//是否接通乘客紧急报警
					if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )
					{
						TalkStatEhpConnCreate(0);
						TalkStatEhpConnDestroy();
						break;
					}
				}
			}
			else 
			{
				PLAY_VOICE_DISABLE();
			}

			// 以下做的目的是为了防止，对端正在进行人工广播或者乘客对讲
			//而不能马上建立连接，需要等待直到对端空闲
			if ( key == KEY_TYPE_IDLE )
			{
				keyOld = KEY_TYPE_NULL;
			}
			if ( keyOld != KEY_TYPE_NULL )
			{
				key = keyOld;
			}
			if ( key == KEY_TYPE_PA )
			{
				//确保有钥匙一端PA优先级最高
				if ( 1 == GetKeySignal() )
				{
					keyOld = KEY_TYPE_NULL;
					gPaTick = gSystemTick;
					SetTalkStat(TALK_STAT_REQUEST_BROADCAST);
				}
				else if (0 == GetOtherSidePaStat())
				{
					keyOld = KEY_TYPE_NULL;
					gPaTick = gSystemTick;
					SetTalkStat(TALK_STAT_REQUEST_BROADCAST);
				}
				else 
				{
					keyOld = KEY_TYPE_PA;
				}
				break;
			}
			else if ( key == KEY_TYPE_CC )
			{
				//判断对端司机是否在紧急对讲,等待直到结束
				if (0x00 == GetOtherSidePasTalkStat())
				{
					keyOld = KEY_TYPE_NULL;
					TalkStatCCConnCreate();
				}
				else 
				{
					keyOld = KEY_TYPE_CC;
				}
				break;
			}

			if ( GetOtherSideDriverTalkStat() > 0 )
			{
				close_pc_flag = 0;
				LedSetPcStat(LED_PC_DRIVER_TALK);
				if ( 0 == GetOtherPcLineStat() )				
				{
					SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
					gDriverTalkStat = 0x00;
					MIC_AUDIO_TO_PC();
					MONI_SELECT_PC();
					MONITOR_CONN_ENABLE();
					MONI_SELECT_PAPC();
				}	
				else 
				{
					if (pttKey == KEY_TYPE_PTT_DOWN)
					{
						SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
						MIC_AUDIO_TO_PC();
						MONI_SELECT_PC();
						MONITOR_CONN_ENABLE();
						MONI_SELECT_PAPC();
						gDriverTalkStat = 0x00;
						SetLocalPcLineStat(0);
						MIC_CONN_ENABLE();
						MONITOR_CONN_DISABLE();
					}
					else if(pttKey == KEY_TYPE_PTT_UP) 
					{
						SetVolume(VOLUME_TYPE_MONI_BROADCAST);
						MONI_SELECT_PA();
						MONITOR_CONN_ENABLE();
						MONI_SELECT_PAPC();
						gDriverTalkStat = 0x00;
						SetLocalPcLineStat(1);
						MIC_CONN_DISABLE();
						MONITOR_CONN_ENABLE();
					}
				}
			}
			//对端是否发起乘客对讲
			else if (0x00 == GetOtherSidePasTalkStat())
			{
				if ( 0 == GetOtherPcLineStat() )
				{
					SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
					gDriverTalkStat = 0x00;
					MONI_SELECT_PC();
					MONITOR_CONN_ENABLE();
					MONI_SELECT_PAPC();
					LedSetPcStat(LED_PC_DRIVER_TALK);
					close_pc_flag= 0;
				}
				else //恢复监听PA
				{
					SetLocalPcLineStat(1);
					MIC_CONN_DISABLE();
					alarmStep = EhpGetAllAlarmStep();
					if (0 == GetKeySignal())
					{
						MoniBroadcast();
					}
					// 如果没有播放报警音
					else if (alarmStep == ALARM_STEP_IDLE)
					{
						MoniBroadcast();
					}
				}
			}

			
			//如果没有报警并且对端也没有司机对讲则熄灭司机对讲
			if ((ALARM_STEP_CALL != EhpGetAllAlarmStep()) && (1 == close_pc_flag))
			{
				LedSetPcStat(LED_PC_CLOSE);
			}
			
			break;
		case TALK_STAT_BROADCAST:
			SetVolume( VOLUME_TYPE_MIC_BROADCAST);
			if ( key == KEY_TYPE_PC_CONN)
			{
				//是否接通乘客紧急报警
				if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )
				{
					gTalkStatOld = TALK_STAT_BROADCAST;
					TalkStatEhpConnCreate(0);
                                        break;
				}
			}
			else if ( key == KEY_TYPE_PC_DISCONN )
			{
				if ( ALARM_STEP_CNCT==EhpGetAllAlarmStep() )
				{
					if (gTalkStatOld == TALK_STAT_PAS_TALK)
					{
						gTalkStatOld = TALK_STAT_IDLE;
						TalkStatEhpConnDestroy();
						TalkStatPAConnCreate();
					}
				}
			}
			else if ( key == KEY_TYPE_CC )
			{
				TalkStatPAConnDestroy();
				TalkStatCCConnCreate();
			}
			else if ( key == KEY_TYPE_IDLE )
			{
				TalkStatPAConnDestroy();
				break;
			}
			if ( pttKey == KEY_TYPE_PTT_DOWN )
			{
				paPttDown = 1;
				gPaTick = gSystemTick;
				if ( 1 == GetKeySignal() )
				{
					MIC_CONN_ENABLE();
					
					gNowBroadcastFlag = VALID;
					MONITOR_CONN_DISABLE();
					SetLocalPaLineStat(0);
					PA_DIRECT_CONN_ENABLE();
					MIC_AUDIO_TO_PA();
					
				}
				else 
				{
					if ( 0 == GetOtherSidePaStat() )
					{
						MIC_CONN_ENABLE();	

						gNowBroadcastFlag = VALID;
						MONITOR_CONN_DISABLE();
						SetLocalPaLineStat(0);
						PA_DIRECT_CONN_ENABLE();
						MIC_AUDIO_TO_PA();
					}
				}
				break;
			}
			else if ( pttKey == KEY_TYPE_PTT_UP )
			{
				MIC_CONN_DISABLE();
				PA_DIRECT_CONN_DISABLE();	
				/*
				gNowBroadcastFlag = INVALID;
				MONITOR_CONN_ENABLE();
				SetLocalPaLineStat(1);
				PA_DIRECT_CONN_DISABLE();				
				*/
			}



			if ( (GetOtherSideDriverTalkStat() > 0))
			{
			
				LedSetPcStat(LED_PC_DRIVER_TALK);
				close_pc_flag = 0;
				LedSetPcStat(LED_PC_DRIVER_TALK);
				if ( 0 == GetOtherPcLineStat() )				
				{
					SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
					gDriverTalkStat = 0x00;
					MIC_AUDIO_TO_PC();
					MONI_SELECT_PC();
					MONITOR_CONN_ENABLE();
					MONI_SELECT_PAPC();
					ccNow = 1;
				}	
				else 
				{
					//MONITOR_CONN_DISABLE();
					gDriverTalkStat = 0x00;
					SetLocalPcLineStat(1);
					//MIC_CONN_ENABLE();
					SetVolume( VOLUME_TYPE_MIC_BROADCAST);
					MIC_AUDIO_TO_PA();
					LedSetPaStat(LED_PA_OPEN);
					//PA_DIRECT_CONN_ENABLE();	
					LedSetPcStat(LED_PC_CLOSE);
				
				}
			}
			//对端是否发起乘客对讲
			else if (0x00 == GetOtherSidePasTalkStat())
			{
				//为了预防没有过程数据时
				if ( 0 == GetOtherPcLineStat() )
				{
					SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
					gDriverTalkStat = 0x00;
					MONI_SELECT_PC();
					MONITOR_CONN_ENABLE();
					MONI_SELECT_PAPC();
					LedSetPcStat(LED_PC_DRIVER_TALK);
					ccNow = 1;
				}
				else //人工广播
				{
#if 0                                  
					if ( (cancelPa == 0) && (doubleKeyFlag == 0))
					{
						//MONITOR_CONN_DISABLE();
						gDriverTalkStat = 0x00;
						SetLocalPcLineStat(1);
						MIC_CONN_ENABLE();
						SetVolume( VOLUME_TYPE_MIC_BROADCAST);
						MIC_AUDIO_TO_PA();
						LedSetPaStat(LED_PA_OPEN);
						PA_DIRECT_CONN_ENABLE();	
					}
#endif
				}
			}
			else 
			{			
				LedSetPcStat(LED_PC_CLOSE);
			}

			//如果XX秒没有按下PTT，则自动挂断
			if ((gSystemTick - gPaTick > (5*1000)) && (ccNow == 0))
			{
				gNowBroadcastFlag = INVALID;
				MONITOR_CONN_ENABLE();
				SetLocalPaLineStat(1);
				PA_DIRECT_CONN_DISABLE();	

				SetVolume( VOLUME_TYPE_MONI_BROADCAST);
				MONI_SELECT_PA();
				MONI_SELECT_PAPC();
				MONITOR_CONN_ENABLE();
				PA_DIRECT_CONN_DISABLE();
				cancelPa = 1;
			}


			

			if ( 1 == GetKeySignal() )
			{
				//是否有乘客报警, 需要响铃
				TalkStatPasAlarmRingCtrl();
			}
			
			break;
		case TALK_STAT_DRIVER_TALK:
			
			SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
			SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
			LedSetPcStat(LED_PC_DRIVER_TALK);
			if ( key == KEY_TYPE_PC_CONN )
			{
				//是否接通乘客紧急报警
				if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )
				{
					LedSetPcStat(LED_PC_CLOSE);
					gTalkStatOld = TALK_STAT_DRIVER_TALK;
					TalkStatEhpConnCreate(0);
					break;
				}
			}
			else if ( key == KEY_TYPE_PC_DISCONN )
			{
				if ( ALARM_STEP_CNCT==EhpGetAllAlarmStep() )
				{
					if (gTalkStatOld == TALK_STAT_PAS_TALK)
					{
						gTalkStatOld = TALK_STAT_IDLE;
						TalkStatEhpConnDestroy();
						TalkStatCCConnCreate();
					}
				}
			}
			else if ( key == KEY_TYPE_PA )
			{
				TalkStatCCConnDestroy();
				SetTalkStat(TALK_STAT_REQUEST_BROADCAST);
			}
			else if ( key == KEY_TYPE_IDLE )
			{
				TalkStatCCConnDestroy();
			}
			
			if ( pttKey == KEY_TYPE_PTT_DOWN )
			{
				if ( 1 == GetOtherPcLineStat() )
				{
					gDriverTalkStat = 0x02;
					SetLocalPcLineStat(0);
					MIC_CONN_ENABLE();
					MONITOR_CONN_DISABLE();
				}
			}
			else if ( pttKey == KEY_TYPE_PTT_UP )
			{
				if (doubleKeyFlag == 0) 
				{
					gDriverTalkStat = 0x01;
					SetLocalPcLineStat(1);
					MIC_CONN_DISABLE();
					MONI_SELECT_PC();
					MONI_SELECT_PAPC();
					MONITOR_CONN_ENABLE();
				}
			}
			if ( 1 == GetKeySignal() )
			{
				//是否有乘客报警, 需要响铃
				TalkStatPasAlarmRingCtrl();
			}
			
			break;
		case TALK_STAT_PAS_TALK:
			SetVolume( VOLUME_TYPE_MONI_PAS_TALK);
			SetVolume( VOLUME_TYPE_MIC_PAS_TALK);
			LedSetPcStat(LED_PC_CONN);
			if ( key == KEY_TYPE_PC_DISCONN )
			{
				TalkStatEhpConnDestroy();
			}
			else if ( key == KEY_TYPE_PA )
			{
				gTalkStatOld = TALK_STAT_PAS_TALK;
				SetTalkStat(TALK_STAT_REQUEST_BROADCAST);
			}
			else if ( key == KEY_TYPE_CC )
			{
				gTalkStatOld = TALK_STAT_PAS_TALK;
				TalkStatCCConnCreate();				
			}
			else if ( key == KEY_TYPE_IDLE )
			{
				if (gTalkStatOld == TALK_STAT_DRIVER_TALK) 
				{
					gTalkStatOld = TALK_STAT_IDLE;
				}
				if (gTalkStatOld == TALK_STAT_BROADCAST) 
				{
					gTalkStatOld = TALK_STAT_IDLE;
					SetLocalPaLineStat(1);
					LedSetPaStat(LED_PA_CLOSE);
					PA_DIRECT_CONN_DISABLE();					
				}
			}
			
			if ( pttKey == KEY_TYPE_PTT_DOWN )
			{
				SetLocalPcLineStat(0);
				MIC_CONN_ENABLE();
				MONITOR_CONN_DISABLE();
			}
			else if ( pttKey == KEY_TYPE_PTT_UP )
			{
				if (doubleKeyFlag == 0) 
				{
					SetLocalPcLineStat(1);
					MIC_CONN_DISABLE();
					MONITOR_CONN_ENABLE();
				}
			}



			if (EhpIfPasHangUp())
			{
				EhpClearPasHangUp();
				TalkStatEhpConnDestroy();
			}
			break;
		case TALK_STAT_REQUEST_BROADCAST:
			LedSetPaStat(LED_PA_REQUEST);
			gRequestBroadcastFlag = VALID;
			WaitRequestBroadcast();
			if ( key == KEY_TYPE_IDLE )
			{
				LedSetPaStat(LED_PA_CLOSE);
				gRequestBroadcastFlag = INVALID;
				SetTalkStat(TALK_STAT_IDLE);
			}
			break;
		default:
			SetTalkStat( TALK_STAT_IDLE );
			break;
	}
}
#endif

void TalkStatInit(void)
{
	gTalkStat = TALK_STAT_IDLE;
	gTalkStatOld = TALK_STAT_IDLE;
}



/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �����������״̬�л�
**  ��������: 2013.12.28
**
**  ��ǰ�汾��1.0
**  ���ߣ����
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

//#define suven   // ʹ���µĹ㲥��������

static u32 gPaTick;


//�ܵ�״̬
static eTalkStat gTalkStat;
static eTalkStat gTalkStatOld;


static tLogicFlag gRequestBroadcastFlag = INVALID;
static tLogicFlag gNowBroadcastFlag = INVALID;

static u8 gPaLineStat = 1;
static u8 gPcLineStat = 1;


//����˾���Խ�״̬ 0:����   1:��     2:��
static u8 gDriverTalkStat = 0;

eTalkStat GetTalkStat(void)
{
	return gTalkStat;
}

//�ܵ�״̬����
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
		//��������
		BoxDataAlarmLedFlash();		
		//����PC LED ��˸
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
	//������Լ����ͣ����⵽�Ľ��ǿ��Ϊ�ߵ�ƽ
	if (0 == GetLocalPcLineStat()) return 1;
	else return gPcLineStat;
}

u8 GetOtherPaLineStat(void)
{
	//������Լ����ͣ����⵽�Ľ��ǿ��Ϊ�ߵ�ƽ
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

	//���TRAIN485 3����û��Կ���źŵ�PISC���ݣ���ֱ��ռ��PA��
	tick = GetTrainComTick();
	if (gSystemTick - tick > 600)
	{
		TalkStatPAConnCreate();
		return;
	}

	//������ȴ�2.5�룬Ȼ����ȥ���PA����
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

 //����pa��
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

 //�˹��㲥MIC����
 void MicBroadcast(void)
 {
	gNowBroadcastFlag = VALID;
	MIC_CONN_ENABLE();		//��˷�������	
	MIC_AUDIO_TO_PA();  //��˷�ѡ��pa���				
	SetLocalPaLineStat(0);   //ռ��pa��
	PA_DIRECT_CONN_ENABLE(); // ռ�г�����		
	MONITOR_CONN_DISABLE();  //�رռ���������
 }

 //����pc��
void Monipc(void )
{
	SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
	
	MONI_SELECT_PC();
	MONITOR_CONN_ENABLE();
	MONI_SELECT_PAPC();	
	MIC_CONN_DISABLE();
}

// CC ʱMIC ����
void MicCC(void)
{
	gDriverTalkStat = 0x02;
	SetLocalPcLineStat(0); //ռ��pc��
	MIC_AUDIO_TO_PC(); //���ѡ�������pc
	MIC_CONN_ENABLE(); //����˷����
	MONITOR_CONN_DISABLE(); //�ر�����������
}

void TalkStatSwitch(void)
//#ifdef suven
#if 1
{
	eTalkStat  stat;
	eKeyType key;
	//u8 close_pc_flag;
	//tAlarmStep alarmStep;
	static eKeyType keyOld = KEY_TYPE_NULL;  //��¼����ֵ����Ϊ�㲥���ƺ�ֻ�������ΰ���
	eKeyType pttKey;
	//u8 cancelPa = 0;
	//u8 ccNow = 0;
	//u8 paPttDown = 0;

	//cancelPa = 0;
	//close_pc_flag = 1;

	//����ʲô״̬�������Ƿ񼤻���ӵı����ƻ���Ҫ����
	TalkStatPasAlarmLedCtrl();

	key = GetKeyValue();
	//pttKey = GetPttStat();

	ClearEhpStat();
	BothSideKeySignalRingCtrl();
	
	stat = GetTalkStat();

	switch(key)
	{
		case KEY_TYPE_PC_CONN:  //��ͨ��������
			
			if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )//�Ƿ�˿ͱ���
			{
				//�����ǰ״̬
				if( stat == TALK_STAT_BROADCAST 
				&&  stat == TALK_STAT_REQUEST_BROADCAST)  //��ǰ״̬��pa
					TalkStatPAConnDestroy(); 
				if( stat == TALK_STAT_DRIVER_TALK) //��ǰ״̬��cc
					TalkStatCCConnDestroy(); 
				
				gTalkStatOld = TALK_STAT_IDLE ;   //�ҶϺ�ֱ�ӽ���idle״̬					
				TalkStatEhpConnCreate(0); //�л����˿ͱ���״̬
				break;
			}
			
			break;
			
		case KEY_TYPE_PC_DISCONN:  //�ҶϽ�������
			
			if ( ALARM_STEP_CNCT==EhpGetAllAlarmStep() ) //Ӧ��״̬���ж�
			{
				TalkStatEhpConnDestroy();  // ���pas						
			}
		
			break;
			
		case KEY_TYPE_PA:
			
			if( stat == TALK_STAT_DRIVER_TALK) 
				TalkStatCCConnDestroy(); 		// ���cc	
			if( stat == TALK_STAT_PAS_TALK) 
				TalkStatEhpConnDestroy();  // ���pc	
		
			gTalkStatOld = TALK_STAT_IDLE ;   //�ҶϺ�ֱ�ӽ���idle״̬
			
			#if 1
			//if(GetOtherSideDriverTalkStat()==0) 				
			if(GetOtherPcLineStat()==1)//�Զ���cc line״̬
			{
				if ( 1 == GetKeySignal() ) //����Կ��
				{
					if( stat != TALK_STAT_REQUEST_BROADCAST && stat != TALK_STAT_BROADCAST) 
					{
						SetTalkStat(TALK_STAT_REQUEST_BROADCAST); //����pa	
						gPaTick = gSystemTick;
					}
				}
				else  
				{
					if (0 == GetOtherSidePaStat())// && (GetOtherPaLineStat()==1) )  //�ȴ�Կ�ײ�pa ���
					{
						if( stat != TALK_STAT_REQUEST_BROADCAST && stat != TALK_STAT_BROADCAST) 
						{
							SetTalkStat(TALK_STAT_REQUEST_BROADCAST); //����pa	
							gPaTick = gSystemTick;
						}
					}
					else
					{
						if( stat == TALK_STAT_BROADCAST 
						||  stat == TALK_STAT_REQUEST_BROADCAST)  //��ǰ״̬��pa
							TalkStatPAConnDestroy(); 
							
						SetTalkStat(TALK_STAT_IDLE); 
					}
				}
			}
			else		
			{
				if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  //��ǰ״̬��pa
					TalkStatPAConnDestroy(); 

				SetTalkStat(TALK_STAT_IDLE); 
				
			}
			#endif				
		
			break;
			
		case KEY_TYPE_CC:
			
			if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  
				TalkStatPAConnDestroy();  // ���pa

			if( stat == TALK_STAT_PAS_TALK) 
				TalkStatEhpConnDestroy();  // ���pc
			
			gTalkStatOld = TALK_STAT_IDLE ;   //�ҶϺ�ֱ�ӽ���idle״̬

			if ( 1 == GetKeySignal() ) //����Կ��
				TalkStatCCConnCreate();
		
			else  //�Զ���PC
			{
				if((GetOtherSideDriverTalkStat()==0)&& ( 1 == GetOtherPcLineStat()))//�ȴ��Բ�cc���
					TalkStatCCConnCreate();
				else
				{
					if( stat == TALK_STAT_DRIVER_TALK)  //��ǰ״̬��cc
						TalkStatCCConnDestroy();
					
					SetTalkStat(TALK_STAT_IDLE); 
				} 
			}		
			break;

		case KEY_TYPE_NULL:
			
			if(stat ==TALK_STAT_PAS_TALK )
				break;

			if( stat == TALK_STAT_BROADCAST 
				||  stat == TALK_STAT_REQUEST_BROADCAST)  //��ǰ״̬��pa
				TalkStatPAConnDestroy();  

			if( stat == TALK_STAT_DRIVER_TALK) //��ǰ״̬��cc
				TalkStatCCConnDestroy(); 

			break;
		
	}
	

	stat = GetTalkStat();
	switch( stat )
	{
		/*********** ����״̬**************/
		case TALK_STAT_IDLE:		

			if(GetOtherSideDriverTalkStat()|| !GetOtherPcLineStat())  //�Բ�˾����cc
				Monipc();	 //����pc ��
			else  
			{
				if((gPiscData.data_area.broadcast_signal.broadcast_type ==0x04)
				||gBcuData.data_area.line_mode.moni_broadcast_off == 1)
					MONITOR_CONN_DISABLE();
				else
					MoniBroadcast();   //����pa��
			}
			if(ALARM_STEP_CALL != EhpGetAllAlarmStep())//�޽�������
				LedSetPcStat(LED_PC_CLOSE); 

			LedSetPaStat(LED_PA_CLOSE);

			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//�Ƿ��г˿ͱ���, ��Ҫ����
			}
			
			break;

		/***********�˹��㲥 *************/
		case TALK_STAT_BROADCAST:       
			SetVolume( VOLUME_TYPE_MIC_BROADCAST);		

			MicBroadcast();
			// ռ��pa��					
			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//�Ƿ��г˿ͱ���, ��Ҫ����
			}
			
			break;


		/*********** ˾���Խ� *************/
		case TALK_STAT_DRIVER_TALK:
			
			SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
			SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
			LedSetPcStat(LED_PC_DRIVER_TALK);

			MicCC();			

			if ( 1 == GetKeySignal() )
			{
				TalkStatPasAlarmRingCtrl();//�Ƿ��г˿ͱ���, ��Ҫ����
			}
			
			break;

		/***********  �˿ͽ����Խ�*************/
		case TALK_STAT_PAS_TALK:
			SetVolume( VOLUME_TYPE_MONI_PAS_TALK);
			SetVolume( VOLUME_TYPE_MIC_PAS_TALK);
			LedSetPcStat(LED_PC_CONN);
			
			if ( key == KEY_TYPE_PC_DOWN )
			{
				SetLocalPcLineStat(0); //ռ��pc��
				MIC_CONN_ENABLE();
				MONITOR_CONN_DISABLE();
			}
			else if ( key == KEY_TYPE_PC_UP )
			{
				if (doubleKeyFlag == 0) 
				{
					SetLocalPcLineStat(1); //�ͷ�pc��
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

		/***********  �˹��㲥����*************/	
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
				TalkStatPasAlarmRingCtrl();//�Ƿ��г˿ͱ���, ��Ҫ����
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
	static eKeyType keyOld = KEY_TYPE_NULL;  //��¼����ֵ����Ϊ�㲥���ƺ�ֻ�������ΰ���
	eKeyType pttKey;
	u8 cancelPa = 0;
	u8 ccNow = 0;
	u8 paPttDown = 0;

	cancelPa = 0;
	close_pc_flag = 1;

	//����ʲô״̬�������Ƿ񼤻���ӵı����ƻ���Ҫ����
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
				//�Ƿ��г˿ͱ���, ��Ҫ����
				TalkStatPasAlarmRingCtrl();
				if ( key == KEY_TYPE_PC_CONN )
				{
					//�Ƿ��ͨ�˿ͽ�������
					if ( ALARM_STEP_CALL==EhpGetAllAlarmStep() )
					{
						TalkStatEhpConnCreate(0);
						break;
					}
				}
				if ( key == KEY_TYPE_PC_DISCONN )
				{
					//�Ƿ��ͨ�˿ͽ�������
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

			// ��������Ŀ����Ϊ�˷�ֹ���Զ����ڽ����˹��㲥���߳˿ͶԽ�
			//���������Ͻ������ӣ���Ҫ�ȴ�ֱ���Զ˿���
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
				//ȷ����Կ��һ��PA���ȼ����
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
				//�ж϶Զ�˾���Ƿ��ڽ����Խ�,�ȴ�ֱ������
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
			//�Զ��Ƿ���˿ͶԽ�
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
				else //�ָ�����PA
				{
					SetLocalPcLineStat(1);
					MIC_CONN_DISABLE();
					alarmStep = EhpGetAllAlarmStep();
					if (0 == GetKeySignal())
					{
						MoniBroadcast();
					}
					// ���û�в��ű�����
					else if (alarmStep == ALARM_STEP_IDLE)
					{
						MoniBroadcast();
					}
				}
			}

			
			//���û�б������ҶԶ�Ҳû��˾���Խ���Ϩ��˾���Խ�
			if ((ALARM_STEP_CALL != EhpGetAllAlarmStep()) && (1 == close_pc_flag))
			{
				LedSetPcStat(LED_PC_CLOSE);
			}
			
			break;
		case TALK_STAT_BROADCAST:
			SetVolume( VOLUME_TYPE_MIC_BROADCAST);
			if ( key == KEY_TYPE_PC_CONN)
			{
				//�Ƿ��ͨ�˿ͽ�������
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
			//�Զ��Ƿ���˿ͶԽ�
			else if (0x00 == GetOtherSidePasTalkStat())
			{
				//Ϊ��Ԥ��û�й�������ʱ
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
				else //�˹��㲥
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

			//���XX��û�а���PTT�����Զ��Ҷ�
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
				//�Ƿ��г˿ͱ���, ��Ҫ����
				TalkStatPasAlarmRingCtrl();
			}
			
			break;
		case TALK_STAT_DRIVER_TALK:
			
			SetVolume( VOLUME_TYPE_MONI_DRIVER_TALK);
			SetVolume( VOLUME_TYPE_MIC_DRIVER_TALK);
			LedSetPcStat(LED_PC_DRIVER_TALK);
			if ( key == KEY_TYPE_PC_CONN )
			{
				//�Ƿ��ͨ�˿ͽ�������
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
				//�Ƿ��г˿ͱ���, ��Ҫ����
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



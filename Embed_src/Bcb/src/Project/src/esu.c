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



static tAlarmSt gAlarmSt;

//��¼ÿ����������״̬��BCB���������ϵ�������ο�������
//����˳��Ϊ0:��һ�ڳ����һ��������
tAlarmStep gAllAlarmSt[MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR];



const u8 alarm_shift_bit[4] = {0, 2, 4, 6};

u32 gLcuTickBuf[MAX_CAR_NUM_PER_TRAIN];

/*
����: ���±����ܱ�
index : �±��0��ʼ
*/
void EhpAlarmSetAllStat( u8 index, tAlarmStep step )
{
	//�������Ҫ�Ҷϵı���������ȴ��������ظ�����ָ��,
	// ����������ߵı������Ҷ�״̬
	if (gAlarmSt.hang_up_index == (index+1)) 
	{
		if ( step == ALARM_STEP_IDLE )
		{
			gAlarmSt.hang_up_index = 0;
			gAllAlarmSt[index] = ALARM_STEP_IDLE;
		}
	}
	else 
	{
		gAllAlarmSt[index] = step;
	}	
}


/*
��������:�Ӷ�����ɾ��ָ���ı���
*/
void EhpAlarmDelete(u8 index)
{
	u8 k;
	u8 flag;
	
	k = 0;
	flag = 0;
	while(flag == 0) {
		if (gAlarmSt.queue[k] == index) {
			while(gAlarmSt.queue[k+1] != 0) {
				gAlarmSt.queue[k] = gAlarmSt.queue[k+1];
				k ++;
			}	
			gAlarmSt.queue[k] = 0;
			flag = 1;
		}
		k ++;
		if (gAlarmSt.queue[k] == 0) break;
	}

}

/*
��������:��������еı�������
*/
u8 EhpAlarmNum(void)
{
	u8 i;

	i = 0;
	while(gAlarmSt.queue[i] != 0) {
		i ++;
	}
	return i;
}



/*
����:  ��õ�ǰ����״̬
*/
tAlarmStep EhpGetAllAlarmStep(void)
{
	u8 i;
	u8 flag = 0;
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR; i++)
	{
		if (gAllAlarmSt[i] == ALARM_STEP_CNCT) 
		{
			flag = 2;
			break;
		}
		if (gAllAlarmSt[i] == ALARM_STEP_CALL) 
		{
			flag = 1;
		}
	}
	if ( 0 == flag ) return ALARM_STEP_IDLE;
	else if ( 1 == flag ) {
		if (gAlarmSt.queue[0]==0)return ALARM_STEP_IDLE;
		else return ALARM_STEP_CALL;
	}
	else if ( 2 == flag ) return ALARM_STEP_CNCT;
        return ALARM_STEP_IDLE;
}


static u32 conn_tick;

/*
��������:����ͨ����������ʱ
*/
void EhpAlarmCnctStatUpdate(void)
{
	gAlarmSt.index = gAlarmSt.queue[0];
	EhpAlarmDelete(gAlarmSt.queue[0]);
	EhpAlarmSetAllStat(gAlarmSt.index-1, ALARM_STEP_CNCT);
	gAlarmSt.step = ALARM_STEP_CNCT;
	conn_tick = gSystemTick;
}

/*
����:�Ҷ�ʱ������Ӧ״̬
*/
void EhpAlarmDestroyStatUpdate(void)
{
	EhpAlarmSetAllStat(gAlarmSt.index-1, ALARM_STEP_HANGUP);
	gAlarmSt.hang_up_index = gAlarmSt.index;
	gAlarmSt.index = 0;
	gAlarmSt.step = ALARM_STEP_IDLE;
}


u8 EhpIfPasHangUp()
{
	if (gSystemTick - conn_tick < 10*1000) return 0;
	
	if (gAlarmSt.pas_hang_up == VALID)
	{
		return 1;
	}
	else 
	{
		return 0;
	}	
}

void EhpClearPasHangUp()
{
	gAlarmSt.pas_hang_up = INVALID;
}

/*
* fun:  �����ʱ��û���յ����ؿ��������ݣ��������ǰ״̬
*/
void ClearEhpStat(void)
{
	u8 i, j;
	u8 id;
	
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN; i++)
	{
		if (gSystemTick - gLcuTickBuf[i] > 30*1000)
		{
			for (j=0; j<MAX_EHP_NUM_PER_CAR; j++)
			{
				id = i * MAX_EHP_NUM_PER_CAR + j +1;
				if (id == gAlarmSt.index) gAlarmSt.pas_hang_up = VALID;
				else EhpAlarmDelete(id);
			}
		}
	}
}


/*
��������:1������Ӧ����ŵı���״̬�����ŶӶ���
2��������������ȡ����״̬���

  ����˵��: buf:������״̬��LCU�ṩ,ֻҪ��һ���ֽ�
  car_id:����ţ��±��0��ʼ
*/
void EhpLcuData(const u8 *buf, u8 car_id)
{
	u8 i, j;
	u8 id;
	tAlarmStep step;
	u8 temp;
	u8 flag;
	
	// �쳣�����ж�
	if (car_id >= MAX_CAR_NUM_PER_TRAIN) return;

	gLcuTickBuf[car_id] = gSystemTick;
	
	temp = *buf;
	for (i=0; i<MAX_EHP_NUM_PER_CAR; i++) 
	{
		id = car_id * MAX_EHP_NUM_PER_CAR + i +1;
		step = (tAlarmStep)(temp & 0xc0);
		temp <<= 2;
		if (step == ALARM_STEP_IDLE) 
		{
			EhpAlarmSetAllStat( id-1, step );
			//�����ǰ��ͨ�ı��������˿͹Ҷϣ����ó˿͹Ҷϱ�־
			if(id == gAlarmSt.index) 
			{
				gAlarmSt.pas_hang_up = VALID;
			}
			else 
			{
				//���Ҷ������Ƿ����Ŷӵı�����������ȡ��
				j = 0;
				while(gAlarmSt.queue[j] != 0) 
				{
					if (id == gAlarmSt.queue[j]) 
					{
						//ɾ�������еı��������
						EhpAlarmDelete(gAlarmSt.queue[j]);
					}
					j ++;
				}
			}
		}
		else if (step == ALARM_STEP_CALL) 
		{	
			if ((gAlarmSt.step == ALARM_STEP_CNCT) && (gAlarmSt.index == id)) 
			{
				continue;
			}
			EhpAlarmSetAllStat( id-1, step );

			//���뱨��������			
			j = 0;
			flag = 0;
			while(gAlarmSt.queue[j] != 0) 
			{
				if (gAlarmSt.queue[j] == id) flag = 1;
				j ++;
			}
			if (flag == 0) 
			{
				gAlarmSt.queue[j] = id;
				gAlarmSt.queue[j+1] = 0;
			}
		}
		else if (step == ALARM_STEP_CNCT) 
		{
			
		}
	}
}





void EhpInit(void)
{
	u8 i;
	
	//״ֵ̬��ʼ��
	gAlarmSt.index = 0;
	gAlarmSt.pas_hang_up = INVALID;
	gAlarmSt.driver_hang_up = INVALID;
	gAlarmSt.step = ALARM_STEP_IDLE;
	gAlarmSt.queue[0] = 0;
	gAlarmSt.queue[1] = 0;
	gAlarmSt.hang_up_index = 0;
	
		
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR; i++)gAllAlarmSt[i] = ALARM_STEP_IDLE;

	//�̵�����ʼ��
	
	
}









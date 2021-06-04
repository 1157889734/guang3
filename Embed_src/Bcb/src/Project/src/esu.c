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



static tAlarmSt gAlarmSt;

//记录每个报警器的状态，BCB发往总线上的数据需参考此数组
//排列顺序为0:第一节车厢第一个报警器
tAlarmStep gAllAlarmSt[MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR];



const u8 alarm_shift_bit[4] = {0, 2, 4, 6};

u32 gLcuTickBuf[MAX_CAR_NUM_PER_TRAIN];

/*
功能: 更新报警总表
index : 下标从0开始
*/
void EhpAlarmSetAllStat( u8 index, tAlarmStep step )
{
	//如果有需要挂断的报警器，则等待报警器回复空闲指令,
	// 清除发往总线的报警器挂断状态
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
功能描述:从队列中删除指定的报警
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
功能描述:计算队列中的报警个数
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
功能:  获得当前报警状态
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
功能描述:当接通按键被按下时
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
功能:挂断时更新相应状态
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
* fun:  如果长时间没有收到本地控制器数据，则清除当前状态
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
功能描述:1、将对应车厢号的报警状态加入排队队列
2、将报警器主动取消的状态清除

  参数说明: buf:报警器状态由LCU提供,只要传一个字节
  car_id:车厢号，下标从0开始
*/
void EhpLcuData(const u8 *buf, u8 car_id)
{
	u8 i, j;
	u8 id;
	tAlarmStep step;
	u8 temp;
	u8 flag;
	
	// 异常数据判断
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
			//如果当前接通的报警器被乘客挂断，设置乘客挂断标志
			if(id == gAlarmSt.index) 
			{
				gAlarmSt.pas_hang_up = VALID;
			}
			else 
			{
				//查找队列中是否有排队的报警器，有则取消
				j = 0;
				while(gAlarmSt.queue[j] != 0) 
				{
					if (id == gAlarmSt.queue[j]) 
					{
						//删除队列中的报警器序号
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

			//加入报警队列中			
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
	
	//状态值初始化
	gAlarmSt.index = 0;
	gAlarmSt.pas_hang_up = INVALID;
	gAlarmSt.driver_hang_up = INVALID;
	gAlarmSt.step = ALARM_STEP_IDLE;
	gAlarmSt.queue[0] = 0;
	gAlarmSt.queue[1] = 0;
	gAlarmSt.hang_up_index = 0;
	
		
	for (i=0; i<MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR; i++)gAllAlarmSt[i] = ALARM_STEP_IDLE;

	//继电器初始化
	
	
}









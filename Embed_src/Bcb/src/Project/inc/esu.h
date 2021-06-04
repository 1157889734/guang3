#ifndef Esu_H_
#define Esu_H_

#include "data_public.h"
#include "device_info.h"

//每列车车厢数
#define 	MAX_CAR_NUM_PER_TRAIN			6
#define 	MAX_EHP_NUM_PER_CAR				3



//报警状态定义，接通或未接通
typedef enum{
	ALARM_STEP_IDLE=0x00,	//空闲
	ALARM_STEP_CALL=0x40,	//呼叫
	ALARM_STEP_CNCT=0x80,	//应答
	ALARM_STEP_HANGUP=0xC0	//挂断
}tAlarmStep;



typedef struct{
	//当前正在报警或者接通的报警器, 下表从1开始,第一车厢第一个报警器索引为1，第二个为2
	// 0的时候，表示当前没有报警或者接通的报警器
	u8 index;


	//要挂断报警器的序号，下标从1开始，第一车厢第一个报警器索引为1，第二个为2
	// 0的时候，表示当前没有需要挂断的报警器
	u8 hang_up_index;
	
	
	//乘客主动挂断标志
	tLogicFlag pas_hang_up;

	//司机主动挂断标志
	tLogicFlag driver_hang_up;

	
	//报警状态,只有ALARM_STEP_IDLE 和 ALARM_STEP_CNCT两种状态
	tAlarmStep step;
	//存放报警队列，依次存放报警序列。以0结尾。1车厢1号报警器为1，2号为2  
	//只有接通才从队列中移除
	u8 queue[MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR+1];	

	u32 cnnt_tick;
}tAlarmSt;





extern tAlarmStep gAllAlarmSt[MAX_CAR_NUM_PER_TRAIN*MAX_EHP_NUM_PER_CAR];


extern void EhpInit(void);
extern void EhpLcuData(const u8 *buf, u8 car_id);
extern tAlarmStep EhpGetAllAlarmStep(void);
extern void EhpAlarmCnctStatUpdate(void);
extern void EhpAlarmDestroyStatUpdate(void);
extern u8 EhpIfPasHangUp();
extern void EhpClearPasHangUp();
extern void ClearEhpStat(void);


#endif


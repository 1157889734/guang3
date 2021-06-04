#ifndef Esu_H_
#define Esu_H_

#include "data_public.h"
#include "device_info.h"

//ÿ�г�������
#define 	MAX_CAR_NUM_PER_TRAIN			6
#define 	MAX_EHP_NUM_PER_CAR				3



//����״̬���壬��ͨ��δ��ͨ
typedef enum{
	ALARM_STEP_IDLE=0x00,	//����
	ALARM_STEP_CALL=0x40,	//����
	ALARM_STEP_CNCT=0x80,	//Ӧ��
	ALARM_STEP_HANGUP=0xC0	//�Ҷ�
}tAlarmStep;



typedef struct{
	//��ǰ���ڱ������߽�ͨ�ı�����, �±��1��ʼ,��һ�����һ������������Ϊ1���ڶ���Ϊ2
	// 0��ʱ�򣬱�ʾ��ǰû�б������߽�ͨ�ı�����
	u8 index;


	//Ҫ�Ҷϱ���������ţ��±��1��ʼ����һ�����һ������������Ϊ1���ڶ���Ϊ2
	// 0��ʱ�򣬱�ʾ��ǰû����Ҫ�Ҷϵı�����
	u8 hang_up_index;
	
	
	//�˿������Ҷϱ�־
	tLogicFlag pas_hang_up;

	//˾�������Ҷϱ�־
	tLogicFlag driver_hang_up;

	
	//����״̬,ֻ��ALARM_STEP_IDLE �� ALARM_STEP_CNCT����״̬
	tAlarmStep step;
	//��ű������У����δ�ű������С���0��β��1����1�ű�����Ϊ1��2��Ϊ2  
	//ֻ�н�ͨ�ŴӶ������Ƴ�
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


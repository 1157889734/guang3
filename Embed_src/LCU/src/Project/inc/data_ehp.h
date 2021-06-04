#ifndef DATA_EHP_H_
#define DATA_EHP_H_

#include "data_public.h"

#define EHP_MAX_NUM						3    //��������������
#define EHP_TOKEN_INTERVAL_TIME			150   //������Ѳ����ʱ��  MS
#define EHP_TOKEN_WAIT_OUTTIME			100   // Ѳ��ȴ���ʱʱ��  MS


//���ؿ�����Ѳ�����������״̬
typedef enum {
	EHP_STAT_IDLE = 0,
	EHP_STAT_BUSY,
	EHP_STAT_ANSWER,
	EHP_STAT_HANGUP,
}eEhpBcbStat;




//���Ѳ��״ֵ̬
typedef struct {
	u8 mDeviceId;  //��ǰ����Ѳ����豸 �±��1��ʼ
	u8 mWaitFlag;  //�ȴ��ظ���ǣ� 1:���ڵȴ��ظ�     0: ���ڵȴ�
	u32 mTick;  //��¼Ѳ��ʱ��ʱ��, �������� 1Ѳ�쳬ʱ����  2Ѳ��������
	
}tEhpTokenStat;


extern void EhpDataProc(void);
extern u8 GetEhpComStat();
extern u8 GetEhpAlarmStat();
extern void EhpDataInit();

#endif



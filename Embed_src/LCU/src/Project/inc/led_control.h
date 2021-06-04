#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include "include.h"

typedef enum{
	LED_STAT_OPEN,
	LED_STAT_CLOSE
}eLedStat;


/*
	��ע�� ǰ4�����ܸı�˳��,EHPģ����õ�
*/
typedef enum{
	LED_ALARM_IDLE,
	LED_ALARM_EHP_LOSE1,
	LED_ALARM_EHP_LOSE2,
	LED_ALARM_EHP_LOSE3,
	LED_ALARM_ALARM,
	LED_ALARM_CONN,
	LED_ALARM_EHP_LOSE
}eLedAlarmStat;

typedef enum{
	LED_ALARM_TYPE_ALARM,
	LED_ALARM_TYPE_CONN,
	LED_ALARM_TYPE_LOSE,
}eLedAlarmType;


typedef struct{
	eLedAlarmStat mAlarmStat;
	eLedStat mLedStat;
	u32 mTick;
}tLedAlarmParam;


typedef enum{
	LED_PC_IDLE,
	LED_PC_LISTEN,
	LED_PC_TALK,
}eLedPcStat;


typedef struct{
	eLedPcStat mPcStat;
	eLedStat mLedStat;
	u32 mTick;
}tLedPcParam;


extern void LedProc(void);
extern void LedUsbOpen(void);
extern void LedAlarmSwitch(eLedStat stat);
extern u8 LedAlarmStatSet( eLedAlarmType type, eLedAlarmStat stat );
extern void LedDispDeviceId(void);
extern u8 LedPcStatSet( eLedPcStat stat );

#endif



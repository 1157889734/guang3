#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include "include.h"

typedef enum{
	LED_STAT_OPEN,
	LED_STAT_CLOSE
}eLedStat;


/*
	请注意 前4个不能改变顺序,EHP模块会用到
*/
typedef enum{
	LED_ALARM_IDLE,
	LED_ALARM_EHP_LOSE1,
	LED_ALARM_EHP_LOSE2,
	LED_ALARM_EHP_LOSE3,
	LED_ALARM_UPDATE_LOSE_PAKET,
	LED_ALARM_TRAIN_NODATA
}eLedAlarmStat;


typedef struct{
	eLedAlarmStat mAlarmStat;
	eLedStat mLedStat;
	u32 mTick;
}tLedAlarmParam;


typedef enum{
	LED_BOX_COM_NO_DATA,
	LED_BOX_COM_HAVE_DATA,
}eLedBoxComStat;


typedef enum{
	LED_PC_CLOSE,
	LED_PC_CALL,
	LED_PC_CONN,
	LED_PC_DRIVER_TALK,
}eLedPcStat;


typedef enum{
	LED_PA_CLOSE,
	LED_PA_OPEN,
	LED_PA_REQUEST,
}eLedPaStat;


extern void LedProc(void);
extern void LedUsbOpen(void);
extern void LedAlarmSwitch(eLedStat stat);
extern u8 LedAlarmStatSet( eLedAlarmStat original, eLedAlarmStat stat );
extern void LedSetPcStat( eLedPcStat stat );
extern void LedSetBoxDataStat(eLedBoxComStat stat);
extern void LedSetPaStat( eLedPaStat stat );
extern void LedDispDeviceId(void);

#endif



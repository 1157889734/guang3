#ifndef DATA_CAR_H_
#define DATA_CAR_H_

#include "data_public.h"
#include "led_control.h"


//与通讯协议对应，不能随便修改
typedef enum{
	KEY_TYPE_NULL = 0,  //没有按键触发
	KEY_TYPE_IDLE,      //按键打到PA PC 中间位置
	KEY_TYPE_CC,        //按键打到CC 位置
	KEY_TYPE_PA,        //按键打到PA位置
	KEY_TYPE_PC_CONN,        //接通报警
	KEY_TYPE_PC_DISCONN,        //挂断报警
	KEY_TYPE_PC_DOWN,        //PTT按键按下
	KEY_TYPE_PC_UP,        //PTT按键弹起
}eKeyType;

extern void BoxDataProc(void);
extern void BoxDataStartToSend(tStationStat stationStat);
extern void BoxDataInit();
extern u8 BoxDataSend( u8* buf, u8 length );
extern void BoxDataAlarmLedFlash(void);
extern void BoxDataAlarmLedOpen(void);
extern void BoxDataAlarmLedClsoe(void);
extern eKeyType GetKeyValue(void);
extern eKeyType GetPttStat(void);

#endif



#ifndef DATA_CAR_H_
#define DATA_CAR_H_

#include "data_public.h"
#include "led_control.h"


//��ͨѶЭ���Ӧ����������޸�
typedef enum{
	KEY_TYPE_NULL = 0,  //û�а�������
	KEY_TYPE_IDLE,      //������PA PC �м�λ��
	KEY_TYPE_CC,        //������CC λ��
	KEY_TYPE_PA,        //������PAλ��
	KEY_TYPE_PC_CONN,        //��ͨ����
	KEY_TYPE_PC_DISCONN,        //�Ҷϱ���
	KEY_TYPE_PC_DOWN,        //PTT��������
	KEY_TYPE_PC_UP,        //PTT��������
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



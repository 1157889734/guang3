//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �㲥���� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:broadcast.h

//*�ļ�����:�㲥����

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef BROADCAST_H
#define BROADCAST_H



//�㲥��Դָʾ�ƺ�
#define BROADCAST_OCC_LED_NO		7
#define BROADCAST_DVA_LED_NO		8
#define BROADCAST_MEDIA_LED_NO		9

//�㲥��Դ����
#define PISC_BROADCAST_ICC	       0x20
#define PISC_BROADCAST_OCC	0x10
#define PISC_BROADCAST_DVA	0x08
#define PISC_BROADCAST_MEDIA	0x04
#define PISC_BROADCAST_DOOR	0x02
#define PISC_BROADCAST_SPECIAL	0x01
#define PISC_BROADCAST_NONE	0x00


#define EMERGENCY_PA_BASE_ID		700
#define CITY_PA_BASE_ID 		1000
#define STATION_PA_BASE_ID 		10
extern void broadcast_proc(void);



#endif



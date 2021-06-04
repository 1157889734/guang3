/******************************************************************************
*�ļ����ƣ�rtc_isl1208.h		       		         
*��          �ߣ�hk           							 
*��          ����ʵʱʱ�Ӵ���
*�������ڣ�2010-5-18
*�޸ļ�¼����								
******************************************************************************/
#ifndef RTC_ISL1208_H
#define RTC_ISL1208_H


//�����豸��
typedef struct
{
	void (*init)(void);
	void (*get_rtc)(uint8*);
	void (*set_rtc)(uint8*);
}rtc_dev_t;
extern rtc_dev_t rtc_dev;

extern void rtc_init(void);

#endif
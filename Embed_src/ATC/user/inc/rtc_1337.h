

#ifndef _RTC_1337_H
#define _RTC_1337_H

#include "config.h"

typedef struct TIMER
{
    uint8 sec;
    uint8 min;
    uint8 hour;
    uint8 date;
    uint8 month;
    uint8 year;
} tRtcTimer;

void rtc_1337_init(void);
void read_1337_time(tRtcTimer *time);
/***********************************************************************************************************************
*Function Nane      :  set_rtc_time
*Description        :  ����RTCʱ��
*Input              :  time ʱ��
*Output             :  ��
*Return             :  0 ʱ�����  1 �ɹ�
*Others             :  ��
***********************************************************************************************************************/
uint8 set_rtc_time(tRtcTimer  time);
/***********************************************************************************************************************
*Function Nane      :  get_rtc_time
*Description        :  ��ȡRTCʱ��
*Input              :  time ʱ��
*Output             :  ��
*Return             :  0 ʱ�����  1 �ɹ�
*Others             :  ��
*******************************************************************/
void  get_rtc_time(tRtcTimer *time);

/***********************************************************************************************************************
*Function Nane  :  update_cur_time
*Description      :  ���µ�ǰʱ��
*Input              :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void update_cur_time(void);

/***********************************************************************************************************************
*Function Nane  :  get_cur_time
*Description      :  ��ȡ��ǰʱ��
*Input              :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void get_cur_time(tRtcTimer *time);

#endif

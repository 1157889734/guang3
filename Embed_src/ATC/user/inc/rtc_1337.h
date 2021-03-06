

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
*Description        :  设置RTC时间
*Input              :  time 时间
*Output             :  无
*Return             :  0 时间错误  1 成功
*Others             :  无
***********************************************************************************************************************/
uint8 set_rtc_time(tRtcTimer  time);
/***********************************************************************************************************************
*Function Nane      :  get_rtc_time
*Description        :  获取RTC时间
*Input              :  time 时间
*Output             :  无
*Return             :  0 时间错误  1 成功
*Others             :  无
*******************************************************************/
void  get_rtc_time(tRtcTimer *time);

/***********************************************************************************************************************
*Function Nane  :  update_cur_time
*Description      :  更新当前时间
*Input              :  无
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void update_cur_time(void);

/***********************************************************************************************************************
*Function Nane  :  get_cur_time
*Description      :  获取当前时间
*Input              :  无
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void get_cur_time(tRtcTimer *time);

#endif


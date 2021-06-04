#ifndef __MY_RTC_H__
#define __MY_RTC_H__

struct ds1307_time_t {
    int date;
    int month;
    int year;
    int dow;
    int hour;
    int min;
    int sec;
};

void rtc_init();
void SetSystemTime(char *dt);
int compare_time_and_now(char *dt);
int SetDs1307RtcTimeHuman( char *dt );



#endif

/******************************************************************************
*文件名称：rtc_isl1208.h		       		         
*作          者：hk           							 
*描          述：实时时钟处理
*创建日期：2010-5-18
*修改记录：无								
******************************************************************************/
#ifndef RTC_ISL1208_H
#define RTC_ISL1208_H


//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*get_rtc)(uint8*);
	void (*set_rtc)(uint8*);
}rtc_dev_t;
extern rtc_dev_t rtc_dev;

extern void rtc_init(void);

#endif
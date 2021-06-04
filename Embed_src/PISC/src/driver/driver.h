//＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃侭嗤駁強殻會蕗苧＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃
//＃猟周兆各:driver.h
//＃猟周傍苧:侭嗤駁強殻會蕗苧
//＃猟周恬宀:虐  灯
//＃幹秀晩豚:2007定5埖
//＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃



#ifndef DRIVER_H
#define DRIVER_H


//export
#define delay		timer0_delay
#define mdelay		timer0_mdelay
#define checktimer	timer0_check_timer
#define get_clock	timer0_get_clock
#define dog		dog_feed
#define softdog		timer0_dog_feed



//啣周霞編凋綜
#define HARDWARE_TEST_CMD	0xFA



#include "arch.h"
#include "mcu.h"
#include "irq.h"
#include "flash.h"
#include "uart0.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "usb/usb.h"
#include "usb/endp0.h"
#include "timer0.h"
#include "timer1.h"
#include "dog.h"
#include "i2c0.h"
#include "i2c1.h"
#include "led.h"
#include "gpio.h"
#include "at45db161d.h"
#include "rtc_isl1208.h"
#include "spi.h"


#endif



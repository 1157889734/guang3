//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆虚拟设备处理◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//◆文件名称:device.h
//◆文件说明:虚拟设备处理
//◆文件作者:翟  鹏
//◆创建日期:2007年5月
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆



#ifndef DEVICE_H
#define DEVICE_H



#include "sys/sh9_sys.h"

#include "tba_bus/tba_bus.h"
#include "tba_bus/sh9_lcu.h"
#include "tba_bus/sh9_pisc.h"
#include "tba_bus/sh9_dcp.h"
#include "tba_bus/sh9_dstled.h"
#include "tba_bus/sh9_setup.h"
#include "tba_bus/tba_m.h"

#include "tms/sh9_tms.h"
#include "cab_bus/sh9_dva.h"
#include "cab_bus/cab_bus.h"
#include "cab_bus/shzh24_panel.h"
#include "media/sh9_media.h"
#include "apu/sh9_apu.h"
#include "ptu/sh9_ptu.h"

#include "occ/sh9_occ.h"


//虚拟设备指针定义
extern sys_dev_t *sys_dev;
extern ptu_dev_t *ptu_dev;
extern tms_dev_t *tms_dev;
extern tba_bus_dev_t *tba_bus_dev;
extern apu_dev_t *apu_dev;
extern dva_dev_t *dva_dev;
extern media_dev_t *media_dev;
extern panel_dev_t *panel_dev;
extern cab_bus_dev_t *cab_bus_dev;
extern occ_dev_t *occ_dev;
extern rtc_dev_t *rtc_dev_p;

//接口函数
extern void device_init(void);




#endif



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 虚拟设备处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:device.c

//*文件作用:虚拟设备处理

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//虚拟设备指针定义
sys_dev_t *sys_dev=NULL;
ptu_dev_t *ptu_dev=NULL;
tms_dev_t *tms_dev=NULL;
tba_bus_dev_t *tba_bus_dev=NULL;
apu_dev_t *apu_dev=NULL;
dva_dev_t *dva_dev=NULL;
media_dev_t *media_dev=NULL;
panel_dev_t *panel_dev=NULL;
cab_bus_dev_t *cab_bus_dev=NULL;
occ_dev_t *occ_dev = NULL;
rtc_dev_t *rtc_dev_p = NULL;

//***********************************************************************************************************************
//函数作用:虚拟设备处理初始化
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void device_init(void)
{
		
	//指定使用的设备
	sys_dev=&sh9_sys_dev;//指定SYS设备
	ptu_dev=&sh9_ptu_dev;//指定PTU设备
	tms_dev=&sh9_tms_dev;//指定TMS设备
	tba_bus_dev=&sh9_tba_bus_dev;//指定列车总线设备
	apu_dev=&sh9_apu_dev;//指定APU设备
	dva_dev=&sh9_dva_dev;//指定DVA设备
	media_dev=&sh9_media_dev;//指定MEDIA设备
	//panel_dev=&shzh24_panel_dev;//指定面板设备
	occ_dev = &sh9_occ_dev;  //occ 初始化
	cab_bus_dev = &xian2_cab_bus_dev;

	//rtc
	rtc_dev_p = &rtc_dev;
	rtc_dev_p->init();

	
	//初始化设备
	sys_dev->init();//SYS设备初始化
	ptu_dev->init();//PTU设备初始化
	tms_dev->init();//TMS设备初始化
	//tba_bus_dev->init(MY_NET_ID,(((DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))?1:2)<<4)|MY_DEV_TYPE);//列车总线设备初始化
	tba_bus_dev->init(MY_NET_ID,(read_pisc_id()<<4)|MY_DEV_TYPE);//列车总线设备初始化
	apu_dev->init();//APU设备初始化
	//dva_dev->init();//DVA设备初始化
	media_dev->init();//MEDIA设备初始化
	//panel_dev->init();//面板设备初始化
	occ_dev->init();
	cab_bus_dev->init();

     	other_default_data.cab_status.dva_error =1;
	other_default_data.tms_status.tms_valid=1;
	memset((uint8*)&error_pack, 0x00, sizeof(error_packet_t));

       read_flash_id();
	
}



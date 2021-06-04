
/******************************************************************************
*文件名称：config.h	       		         
*作          者：hk           							 
*描          述：编译配置文件	
*创建日期：2010-3-16
*修改记录：无								
******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H



//项目识别号
#define PROJECT_ID		110	//深圳地铁24列车(110)

#define DEBUG_PRINT 0

//版本号
#define VENDER_NAME		"********深圳市北海轨道交通技术有限公司********"
#define SYSTEM_NAME		"************广州3号线改造PIDS系统************"
#define PRODECT_NAME		"****************中央控制器****************"
#define PRODECT_CODE			"TBA2472"

//更改内容:过程数据和巡检令牌一起发送，总线时间30ms，非钥匙侧广播盒巡检间隔200ms，监听音量设置
//#define VERSION_DATE           "版本时间  2014-12-18 v2.2"    
//#define VERSION_DATE           "版本时间  2015-02-09 v2.4"   
//#define VERSION_DATE           "版本时间  2018-04-16 v2.5"   //修改区分上下行语音
//#define VERSION_DATE           "版本时间  2018-05-17 v2.6"   //修改机场北开右门
#define VERSION_DATE           "版本时间  2019-10-10 v2.7"   //添加大小交路

//硬件版本号 1 2....99
#define HARDWARE_VERSION		2
//增加功能版本号
#define ADD_FUNCTION_VERSION		1
//大版本号
#define MAJOR_VERSION			2
//小版本号
#define MINOR_VERSION			7

#define SZ5_LINE  1


//站总数
#define STATION_TOTAL_NUM		30


//网络号----主干网
#define MY_NET_ID		0xFE
//设备类型----中央控制器
#define MY_DEV_TYPE		0x01
//设备编号
#define MY_DEV_NO		0x01





//调试端口
#define DEBUG_BAUDRATE			9600//波特率
#define DEBUG_PORTINIT			usb_init
#define DEBUG_RECEIVE_CHAR		usb_read_char
#define DEBUG_SEND_CHAR			usb_write_char
#define DEBUG_SEND_STR			usb_write_str
#define DEBUG_SEND_BUF			usb_write_buf



#endif



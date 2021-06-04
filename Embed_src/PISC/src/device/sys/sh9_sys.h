//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:sys.h

//*文件作用:系统模块

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef SYS_H
#define SYS_H


//硬件定义
#if 0
#define DEV_ID_DET_REG		GPIO0_PD
#define DEV_ID_DET_BIT		(12)
#define CAB_KEY_REG		GPIO2_PD
#define CAB_KEY_BIT		(1)
#endif
#define DEV_ID_DET_REG 		RD_EXT_IO_STAUTS
#define DEV_ID_DET_BIT0		(14)
#define DEV_ID_DET_BIT1		(10)
#define CAB_KEY_REG		RD_EXT_IO_STAUTS
#define CAB_KEY_BIT              (6)

#define B_S_SWITCH_REG		RD_EXT_IO_STAUTS
#define B_S_SWITCH_BIT        (7)  //主备切换

#define LINK_CTRL_REG		RD_EXT_IO_STAUTS
#define LINK_CTRL_BIT            (9)  //重连

#define B_S_CLOSE_DELAY_REG		RD_EXT_IO_STAUTS
#define B_S_CLOSE_DELAY_BIT        (13)  //延迟关机
//USB连接指示灯号
#define USB_LED_NO		6



//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
}sys_dev_t;
extern sys_dev_t sh9_sys_dev;



#endif



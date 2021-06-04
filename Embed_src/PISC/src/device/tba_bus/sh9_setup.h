//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& PC设置软件 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:setup.h

//*文件作用:PC设置软件

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef SETUP_H
#define SETUP_H



#define TBA_DEV_SETUP		0x0A		//PC设置软件




//接口函数
extern void setup_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);




#endif



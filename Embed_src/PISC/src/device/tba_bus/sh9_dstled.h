//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 终点站屏 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dstled.h

//*文件作用:终点站屏

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DSTLED_H
#define DSTLED_H



#define TBA_DEV_DSTLED		0x07	//终点站屏
#define TBA_DSTLED_NUM		0x02	//终点站屏数量



//通信错误
extern uint16 dstled_comm_error[];



//接口函数
extern void dstled_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);



#endif



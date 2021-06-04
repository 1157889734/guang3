//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 司机室通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:CCTV.h

//*文件作用:CCTV数据处理

//*文件作者:王会斌

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#ifndef CCTV_H
#define CCTV_H

#define CCTV_BUF_SIZE  128
#define CCTV_NUM         1

extern void cctv_proc(uint8 device_no,uint8 data_length,uint8 *buf);
__packed typedef struct
{
	uint8 year;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint8 pis_status;
	uint8 door_status;
	uint8 pecu1_status;
	uint8 pecu2_status;
	__packed struct
	{
		uint8 agent_hand_1:1;
		uint8 agent_hand:6;
		uint8 agent_hand_2:1;
	
	}agent;

}cctv_send_data_t;

__packed typedef struct 
{
	uint16 (*get_status)(void);
	uint8 (*send_data)(uint8,uint8 *,uint8);
	cctv_send_data_t send_to_cctv_data;

}cctv_dev_t;

extern cctv_dev_t sh11_cctv_dev;

#endif
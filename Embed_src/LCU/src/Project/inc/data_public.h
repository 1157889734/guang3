#ifndef DATA_PUBLIC_H_
#define DATA_PUBLIC_H_


#include "device_info.h"


//#define DEBUG_TEST_UART			1



#define LINE_MAX_STATION_NUM				30


//设备类型定义
#define DEVICE_TYPE_PISC						0x01	//中央控制器
#define DEVICE_TYPE_BCB						0x02	//广播控制盒
#define DEVICE_TYPE_LCU						0x03	//本地控制器
#define DEVICE_TYPE_DMP						0x04
#define DEVICE_TYPE_EHP						0x05	//对讲控制器
#define DEVICE_TYPE_IDU						0x06	//内部屏
#define DEVICE_TYPE_FDU						0x07    //前部屏
#define DEVICE_TYPE_SDU						0x08    //侧部屏


#define PROCESS_PACKET			0x00
#define P2P_PACKET				0x01
#define TOKEN_PACKET			0x02
#define CLR_TOKEN				0x03
#define DOWNLOAD_PACKET		0x04


//基本协议数据包头
typedef struct
{
    u8 dest_net_id;
	u8 dest_device_id;
	u8 src_net_id;
	u8 src_device_id;
	u8 cmd;
	u8 len;
}tBaseProtocolHeader;


typedef enum{
	RUN_STAT_PRE_DOOR_SIDE = 0,
	RUN_STAT_OPEN_DOOR,
	RUN_STAT_CLOSE_DOOR,
	RUN_STAT_LEAVE_BROADCAST,
}eRunStat;


typedef enum {
    DOOR_SIDE_NONE = 0,
    DOOR_SIDE_LEFT = 1,
    DOOR_SIDE_RIGHT = 2,
    DOOR_SIDE_BOTH = 3,
}tDoorLorR;



typedef struct{
	u8 mStartStation;
	u8 mEndStation;
	u8 mNextStation;
	tDoorLorR mDoorSide;
	eRunStat mRunStat;		
}tStationStat;



//升级数据
typedef struct
{
    u8 update_type;             //为1的时候是程序升级
    u8 update_flag;             // 1:开始升级；2:升级数据；3:升级结束
    u8 lowest_bye_total_len;    //总长度的最低字节
    u8 lower_bye_total_len;     //总长度的次低字节
    u8 higher_bye_total_len;    //总长度的次高字节
    u8 highest_bye_total_len;   //总长度的最高字节
    u8 low_byte_cur_packet_num; //当前包号的低字节
    u8 high_byte_cur_packet_num;//当前包号的高字节
}tUpdateHeader;


typedef struct
{
    tUpdateHeader header;
    u8 data[255];
}tUpdateData;

//升级数据完整包
typedef struct
{
    tBaseProtocolHeader header;
    tUpdateData         data;
}tUpdateFrame;



__packed typedef struct
{
	u8 len;
	u8 cmd;
	u8 step;
	u8 packet_num_h;
	u8 packet_num_l;
	u8 data_area[3];
}tUsbUpdatePacket;

__packed typedef struct 
{
	u8 dest_netnum;
	//低位在前
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	__packed struct
	{
		u8 packet_type:4;
		u8 token_return:1;
		u8 m_active:1;
		u8 m_error:1;
		u8 m_include:1;			
	}cmd;
	u8 len;
}tFrameHeader;



//===========================过程数据=======================================================


__packed typedef struct 
{
	//触发标志---低位在前
	__packed struct
	{
		u8 leave_broadcast:1;
		u8 arrived_broadcast:1;
		u8 jump:1;
		u8 urgent_broadcast:1;
		u8 closedoor:1;
		u8 opendoor:1;
		u8 leave:1;
		u8 reach_stop:1;
	}trigger_hbit;

	u8 start_station;//起始站
	u8 end_station;//终点站
	u8 current_station;//当前站
	u8 next_station;//下一站
	u8 jump_broadcast_no;//越站广播代码
	u8 urgent_broadcast_no;//紧急广播代码
	
	//状态标志---低位在前
	__packed struct
	{
		u8 jump_valid:1;
		u8 active:1;
		u8 master:1;
		u8 dir_valid:1;
		u8 door_right_sides:1;
		u8 door_left_sides:1;
		u8 dir_down:1;
		u8 dir_up:1;
	}status_hbit;
	
	//uint32 jump_stations;//越站代码
	u8 city_no;//城市号
	u8 year;
	u8 month;
	u8 date;
	
	
	//广播优先级
	__packed struct
	{
		u32 pecu:4;
		u32 talk:4;
		u32 manual:4;
		u32 occ:4;
		u32 media:4;
		u32 dva:4;
		u32 door:4;
		u32 special:4;
	}broadcast_priority;
	
	
	//广播信源---低位在前
	__packed struct
	{
		u8 broadcast_req:1;
		u8 broadcasting:1;
		u8 broadcast_type:6;
	}broadcast_signal;

	//32个报警器状态
	u8 esu_status[8];
	
	//媒体显示模式
	u8 line_no;
	
	//司机室状态
	__packed struct
	{
		u8 media_error:1;
		u8 dva_error:1;
		u8 reserve:6;
	}cab_status;
	
	u8 tms_status;
	
	__packed struct
	{
		u8 mode:4;
		u8 value:4;
	}panel_volume;//面板设置的音量参数	
	u8 hour;
	u8 min;
	__packed struct
	{
		u8 led_test:1;
		u8 broadcast_test:1;
		u8 master_req:1;
		u8 slave_req:1;
		u8 reserve:1;
		u8 link_flag:1;
		u8 run30p:1;
		u8 run60p:1;
	}test_st;
	
}pisc_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//低位在前
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;
	}src_eqnum;
	u8 cmd;
	u8 len;
	pisc_data_t data_area;
}tPiscHeader;

//===========================过程数据END=======================================================



//============================广播控制盒数据=====================================================


__packed typedef struct 
{

	u8 start_station;//起始站
	u8 end_station;//终点站
	u8 current_station;//当前站
	u8 next_station;//下一站
	u8 jump_broadcast_no;//越站广播代码
	u8 urgent_broadcast_no;//紧急广播代码

	//触发标志---低位在前
	__packed struct
	{
		u8 leave_broadcast:1;
		u8 arrived_broadcast:1;
		u8 jump:1;
		u8 urgent_broadcast:1;
		u8 man_req:1;
		u8 man_ing:1;
		u8 brdcst_mute:1;
		u8 brdcst_stop:1;
	}trigger_hbit;
	
	//状态标志---低位在前
	__packed struct
	{
		u8 talk_st:2;	//0:无对讲  1:有对讲
		u8 link:1;   //重联状态位
		u8 pas_talk:1;  //乘客紧急报警对讲
		u8 up_down:1;	//0:上行   1:下行  上行从小到大
		u8 test_st:1;
		u8 force_st:1;
		u8 active:1;
	}status_hbit;

	//32个报警器状态
	u8 esu_status[8];

	//32个越站信息
	u8 jump_stations[4];

	u8 other1;

	//状态标志---低位在前
	__packed struct
	{
		u8 mode:2;
		u8 line_num:6;
	}mode_st;
	
}bcb_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//低位在前
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	u8 len;
	bcb_data_t data_area;
}tBcbHeader;

//============================广播控制盒数据END=================================================




//============================本地控制器数据============================


__packed typedef struct 
{

	u8 ehp_st;   //报警器状态
	u8 sapu_st;	  
	u8 spk_st;
	u8 led_st;
	u8 dmp1_st;
	u8 dmp2_st;
	u8 version_h;
	u8 version_l;
}lcu_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//低位在前
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	u8 len;
	lcu_data_t data_area;
}tLcuHeader;

//============================本地控制器数据END============================



extern u8 VerifyPaket(u8 *paketBuf, u16 length);
extern u8 PackageData( u8 *destBuf, u8 *srcBuf, u8 length );
extern void CheckUartSendControl();

#endif



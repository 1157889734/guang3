//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 中央控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:pisc.h

//*文件作用:中央控制器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PISC_H
#define PISC_H



#define TBA_DEV_PISC	0x01	//中央控制器
#define TBA_PISC_NUM	0x02	//中央控制器数量

#define COMM_PISC_TIMEOUT 20000

//中央控制器过程数据结构
__packed typedef struct
{
	//触发标志---低位在前
	__packed struct
	{
		uint8 leave_broadcast:1;          //离站广播标志位
		uint8 arrived_broadcast:1;			//到站广播标志位
		uint8 jump:1;									//越战标志位
		uint8 urgent_broadcast:1;			//紧急广播标志位
		uint8 closedoor:1;							//关门标志位
		uint8 opendoor:1;							//开门标志位
		uint8 leave:1;									//列车离站标志位
		uint8 reach_stop:1;						//列车到站标志位
	}trigger_hbit; // 1

	uint8 start_station;// 2 起始站
	uint8 end_station;// 3 终点站
	uint8 current_station;// 4当前站
	uint8 next_station;// 5下一站
	uint8 jump_broadcast_no;// 6越站广播代码
	uint8 urgent_broadcast_no;// 7紧急广播代码
	
	//状态标志---低位在前
	__packed struct
	{
		uint8 jump_valid:1;
		uint8 active:1;
		uint8 master:1;
		uint8 dir_valid:1;
		uint8 door_led_sides:2;
		uint8 dir_down:1;
		uint8 dir_up:1;
	}status_hbit;  //8
	
	uint8 city_id;  // 9城市号
	uint8 year; //10
	uint8 month;  // 11
	uint8 date; // 12
	
	
	//广播优先级
	__packed struct
	{
		uint32 pecu:4;
		uint32 talk:4;
		uint32 manual:4;
		uint32 occ:4;
		uint32 media:4;
		uint32 dva:4;
		uint32 door:4;
		uint32 special:4;
	}broadcast_priority; // 13-16
	
	
	//广播信源---低位在前
	__packed struct
	{
		uint8 broadcast_req:1;
		uint8 broadcasting:1;
		uint8 broadcast_type:6;
	}broadcast_signal; // 17

	//32个报警器状态
	icc_status_t icc_status[8]; // 18-25
	
	//媒体显示模式
	//uint8 media_mode;
	uint8 pis_version;  // 26    7            6      5   4      3   2   1   0   //  例如: 0x12   pis1软件版本 1.2 ; 0x92  pis2软件版本 1.2
					//  0:pis1 1:pis2   version_high    version_low          
					  
	//司机室状态
	__packed struct
	{
		uint8 media_error:1;
		uint8 dva_error:1;
		uint8 apu_error:1;
		uint8 record_error:1;
		uint8 reserve:2;
		uint8 drmd_welcome:1;  //动态地图欢迎显示标志位
		uint8 pis_mode:1;		
	}cab_status; //27
	
	//TMS状态
	__packed struct
	{
		uint8 tms_valid:1;
		uint8 tms_active:1;
		uint8 lamp_test:1;
		uint8 atc_valid:1;
		uint8 map_display:1;  // 1:关闭动态地图
		uint8 reserve:1;    // Operation in HMI : =1 manual mode (TCMS) =0 auto mode.(ATC, default)
		uint8 tms_mode:2;
	}tms_status;//28
	
	uint8 dcp_volume;// 29    广播盒音量参数
	uint8 hour;//30
	//uint8 next_pass_station;//下一个经过的站--不一定停车，但一定经过
	uint8 minute;//31
	
	__packed struct
	{
		uint8 drmd_test:1;  //动态地图测试
		uint8 broadcast_test:1;
		uint8 master_req:1;
		uint8 slave_req:1;
		uint8 switch_master:1;   //按切主按钮
		uint8 reconnection:1;  //重联标识   1:重联
		uint8 third_distance:1;   // 三分之距离
		uint8 two_part_distance:1; //三分之二
	}test_status; //32

	__packed struct
	{
		uint8 status1;    
		uint8 status2;
		uint8 status3;
	}error_data; //33 - 35	
}pisc_default_data_t;
extern pisc_default_data_t other_default_data;


//通信错误
extern uint16 pisc_comm_error[];



//接口函数
extern void pisc_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern uint8 read_pisc_id(void);


#endif



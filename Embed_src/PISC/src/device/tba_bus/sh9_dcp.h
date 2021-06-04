//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 控制盒 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dcp.h

//*文件作用:控制盒

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DCP_H
#define DCP_H



#define TBA_DEV_DCP	0x02	//控制盒
#define TBA_DCP_NUM	0x02	//控制盒数量

//控制盒过程数据结构
__packed typedef struct
{
	uint8 start_station;// 1起始站
	uint8 end_station;// 2终点站
	uint8 current_station;// 3当前站
	uint8 next_station;// 4下一站
	uint8 jump_broadcast_no;// 5越站广播代码
	uint8 urgent_broadcast_no;// 6紧急广播代码
	//广播控制位
	__packed struct
	{
		uint8 leave_broadcast:1;   //预报广播触发
		uint8 arrived_broadcast:1;  //到站广播触发
		uint8 jump_broadcast:1;     //越站广播触发
		uint8 urgent_broadcast:1;   //紧急广播触发
		uint8 broadcast_req:1;       //口播申请
		uint8 broadcasting:1;         //正在口播
		uint8 mute_broadcast:1;   //静音
		uint8 stop_broadcast:1;     //停止报站广播
	}broadcast_hbit; //7
	//状态位
	__packed struct
	{
		uint8 talking:2;  //司机对讲状态1,司机对讲状态2
		uint8 link:1;    //重连
		uint8 em_talk:1; //紧急对讲
		uint8 run_dir:1;  //上下行
		uint8 test_state:1;//测试状态
		uint8 force_state:1;  //强制状态
		uint8 active:1;     //激活状态
	}status_hbit; //8
	
	uint8 icc_status[8]; //9-16 32个报警器状态字
	uint8 jump_stations;//17越站代码
	uint8 dcp_pacc_listen; //18   0-4 pa监听  5-7 cc监听
	uint8 dcp_pcpa_listen; //19  0-4 pc监听 5-7  pa mic
	uint8 dcp_ccpc_mic; //20     0-4 cc mic         5-7  pc mic

	__packed struct
	{
		uint8 pisc_mode:2;
		uint8 city_id:2;
		uint8 moni_broadcast:4;  //广播盒监听音量设置
	}line_mode;   //21

	uint8 version_h;//22

	uint8 version_l;//23
		
}dcp_default_data_t;
extern dcp_default_data_t dcp_default_data[];


//通信错误
extern uint16 dcp_comm_error[];



//接口函数
extern void dcp_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern uint8 dcp_get_media_on(void);
extern uint8 dcp_get_broadcast_req(void);
extern uint8 dcp_get_broadcast_state(void);
extern uint8 dcp_get_talk_state(void);
extern uint8 dcp_get_emtalking_state(void);
extern uint8 dcp_get_talk_req(void);
extern uint8 dcp_get_media_mode(void);

extern void  dcp_record(uint8 dev_no);

#endif



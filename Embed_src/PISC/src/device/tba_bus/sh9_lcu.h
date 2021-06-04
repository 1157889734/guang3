//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 客室控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:lcu.h

//*文件作用:客室控制器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef LCU_H
#define LCU_H



#define TBA_DEV_LCU		0x03	//客室控制器
#define TBA_LCU_NUM		6	//客室控制器数量



//呼叫状态
#define TBA_LCU_PECU_SHUT	0x00		//挂断
#define TBA_LCU_PECU_CALL	0x01		//呼叫
#define TBA_LCU_PECU_CONNECT	0x02		//连接



//4个报警器状态
__packed typedef struct
{
	uint8 pecu4:2;
	uint8 pecu3:2;
	uint8 pecu2:2;
	uint8 pecu1:2;
}icc_status_t;
	

//客室控制器过程数据结构
__packed typedef struct
{
	//4个报警器状态
	icc_status_t icc_status;// 1
	
	//客室报警器错误字
	__packed struct
	{
		uint8 sapu_in_position:1;//sapu在位
		uint8 sapu_comm:1;//sapu通信
		uint8 reserve:2;//保留		
		uint8 pecu4:1;//pecu4错误
		uint8 pecu3:1;//pecu3错误
		uint8 pecu2:1;//pecu2错误
		uint8 pecu1:1;//pecu1错误
	}icc_error; // 2
	
	//功放扬声器错误
	__packed struct
	{
		uint8 amp1:1;
		uint8 amp2:1;
		uint8 spk1:1;
		uint8 spk2:1;
		uint8 reserve:4;
	}amp_spk_error;// 3
	
	//led屏错误
	__packed struct
	{
		uint8 interior_led1:1;
		uint8 interior_led2:1;
		uint8 side_led1:1;
		uint8 side_led2:1;
		uint8 reserve:4;
	}led_error; // 4
	
	//个动态地图错误
	__packed struct
	{
	      uint8 dmp9:1;
		uint8 dmp10:1;
		uint8 reserve:6;  // 本地控制器 大端
		
		uint8 dmp1:1;
		uint8 dmp2:1;
		uint8 dmp3:1;
		uint8 dmp4:1;
		uint8 dmp5:1;
		uint8 dmp6:1;
		uint8 dmp7:1;
		uint8 dmp8:1;
		
	}dmp_error; // 5-6

	uint8 version_h; //7
	uint8 version_l; //8
	
}lcu_default_data_t;
extern lcu_default_data_t lcu_default_data[];


//8个客室控制器通信错误
extern uint16 lcu_comm_error[];



//接口函数
extern void lcu_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern void lcu_get_icc_state(icc_status_t *status);
extern uint8 lcu_get_icc_connect(void);



#endif



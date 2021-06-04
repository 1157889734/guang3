//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.h

//*文件作用:数字报站器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DVA_H
#define DVA_H



//硬件定义
//硬件版本1
#if(HARDWARE_VERSION==1)
#define DVA_PLAYING_DET_REG		GPIO2_PD
#define DVA_PLAYING_DET_BIT		(6)
//硬件版本2
#elif(HARDWARE_VERSION==2)
#define DVA_PLAYING_DET_REG		GPIO2_PD
#define DVA_PLAYING_DET_BIT		(12)
#endif


//输入输出端口定义--根据硬件接口改变
/*
#define DVA_BAUDRATE		9600			//波特率
#define DVA_PORTINIT		uart3_init		//通信端口初始化
#define DVA_RECEIVE_CHAR	uart3_read_char		//通信端口接收
#define DVA_SEND_CHAR		uart3_write_char	//通信端口发送
#define DVA_SEND_BUF		uart3_write_buf		//通信端口发送
#define DVA_FIFO_BACKUP		uart3_fifo_out_backup	//底层指针保存
#define DVA_FIFO_RESUME		uart3_fifo_out_resume	//底层指针回读
*/

//缓冲区长度定义--根据具体应用改变
#define DVA_BUF_SIZE		64
#define DVA_LIST_MAX		8

#define COMM_DVA_TIMEOUT	80

//begin add by hk 2010-5-24   for 新语音文件命名规则
#define  NAME_RULE_NEW    1
//end add by hk 2010-5-24   for 新语音文件命名规则

//DVA数量
#define DVA_NUM             1
extern void dva_proc(uint8 device_no,uint8 data_length,uint8 *buf);
//虚拟设备类
typedef struct
{
	//void (*init)(void);
	//void (*proc)(void);
	#if NAME_RULE_NEW 
	void (*play_list)(uint16);
	#else
	void (*play_list)(uint16 *);
	#endif
	void (*play_special)(uint16);
	void (*stop)(void);
	uint8 (*read_version)(void);
	uint8 (*read_status)(void);
	void (*set_volume)(uint8);
	uint8 (*get_playing)(void);
	//void (*send_cmd)(uint8 , uint8 *, uint8);
	uint8 (*get_record_status)(void);
	void (*dva_clear)(void);
	//uint8 (*dva_get_play_status)(void);
	uint16 comm_error;//dva  串口通信错误
	uint16 record_error;//录音模块错误
}dva_dev_t;
extern dva_dev_t sh9_dva_dev;

//extern uint8 dva_comm_err[DVA_NUM];

#endif



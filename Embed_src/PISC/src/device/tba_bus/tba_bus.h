//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tba_bus.h

//*文件作用:列车通信总线接口

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TBA_BUS_H
#define TBA_BUS_H



//列车总线通信指示灯号
#define TBA_BUS_LED_NO		3



//输入输出端口定义--根据硬件接口改变
#define TBA_BUS_BAUDRATE	9600			//波特率
#define TBA_BUS_PORTINIT	uart1_init		//通信端口初始化
#define TBA_BUS_RECEIVE_CHAR	uart1_read_char		//通信端口接收
#define TBA_BUS_SEND_BUF	uart1_write_buf		//通信端口发送
#define TBA_BUS_SEND_CHAR	uart1_write_char	//通信端口发送
#define TBA_BUS_FIFO_BAKUP		uart1_fifo_out_backup
#define TBA_BUS_FIFO_RESUME		uart1_fifo_out_resume



//通信协议特殊字符
#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符

//总线命令类型
#define TBA_CMD_DEFAULTPACKET		0x00	//过程数据包
#define TBA_CMD_P2PPACKET		0x01	//点对点命令
#define TBA_CMD_TOKENPACKET		0x02	//令牌包
#define TBA_CMD_TOKENCLEAR		0x03	//清除所有令牌的包
#define TBA_CMD_DOWNLOAD		0x04	//下载程序


//缓冲区长度定义--根据具体应用改变
#define TBA_BUF_SIZE		512

#define BUS_INTERVAL_TIME  30   //总线的平复时间
#define TOKEN_INTERVAL_TIME  250   //总线的平复时间
#define PROCESS_TO_TOKEN_TIME 60 //发送过程数据到巡检间隔时间


//列车总线数据包结构
__packed typedef struct
{
	uint8 dst_net_id;
	uint8 dst_dev_id;
	uint8 src_net_id;
	uint8 src_dev_id;
	//列车总线命令字结构---低位在前
	__packed struct
	{
		uint8 packet_type:4;//包类型
		uint8 token_free:1;//发送方释放令牌的标志
		uint8 m_reserve:3;//保留
	}bus_cmd;
	uint8 app_length;
	uint8 app_data[TBA_BUF_SIZE];
}tba_packet_t;



//虚拟设备类
typedef struct
{
	void (*init)(uint8, uint8);
	void (*proc)(void);
	uint8 (*send_data)(uint8, uint8, uint8, uint8, uint8 *, uint8);
	void (*set_debug)(uint8);
	uint8 data_flag;//0:总线上有数据   1:总线上没有数据
	uint32 bus_timer;
	uint8 dcp_volume_set;
	uint8 tba_send;
	//uint8 data_flag_key;//0:总线上有数据   1:总线上没有数据
	//uint8 data_flag_reconn;//0:总线上有数据   1:总线上没有数据
}tba_bus_dev_t;
extern tba_bus_dev_t sh9_tba_bus_dev;



#endif



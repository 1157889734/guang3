
/******************************************************************************
*文件名称：cab_bus.c		       		         
*作          者：hk           							 
*描          述：司机室通信总线接口处理		
*创建日期：2010-3-10
*修改记录：无								
******************************************************************************/
#ifndef CAB_BUS_H
#define CAB_BUS_H

//输入输出端口定义--根据硬件接口改变
#define CAB_BUS_BAUDRATE	9600			//波特率
#define CAB_BUS_PORTINIT	uart2_init		//通信端口初始化
#define CAB_BUS_RECV_CHAR	uart2_read_char		//通信端口接收
#define CAB_BUS_SEND_BUF	uart2_write_buf		//通信端口发送
#define CAB_BUS_SEND_CHAR	uart2_write_char	//通信端口发送
#define CAB_BUS_FIFO_BACKUP     uart2_fifo_out_backup
#define CAB_BUS_FIFO_RESUME		uart2_fifo_out_resume

//通信协议特殊字符
#define CAB_PACKET_TAB			0x7E	//数据包分割符

//总线设备类型
#define CAB_DVA               0x07    //司机室总线DVA设备类型编号
#define CAB_CCTV              0x06    //司机室总线radio设备类型编号
#define CAB_PANEL            0x0A    //司机室总线面板设备类型编号


#define  DVA_DEVICE_NO			0x01	//DVA 模块设备号
#define  RECORD_DEVICE_NO         0x02      //录音模块设备号

//缓冲区长度定义--根据具体应用改变
#define CAB_BUF_SIZE		255

#define DVA_NEW   1     
#define DVA_OLD    0  

#if DVA_NEW  //new dva
__packed typedef struct 
{
	uint8 data_length;			//数据长度
	uint8 device_type;			//设备类型
	uint8 device_no;			//设备编号
	uint8 cab_data[CAB_BUF_SIZE];				//设备的数据长度
} cab_bus_packet_t;
#endif

#if DVA_OLD //old dva
__packed typedef struct 
{
	uint8 data_length;			//数据长度
	uint8 cab_data[CAB_BUF_SIZE];				//设备的数据长度
} cab_bus_packet_t;
#endif

typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	uint8 (*send_data)(uint8,uint8,uint8,uint8,uint8 *);
	void (*set_debug)(uint8);
	//void (*set_debug)(uint8);
}cab_bus_dev_t;

extern cab_bus_dev_t xian2_cab_bus_dev;
#endif


//----------------------------------------------------------------------------------
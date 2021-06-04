//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 面板设备类 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:panel.h

//*文件作用:面板设备类

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PANEL_H
#define PANEL_H




//输入输出端口定义--根据硬件接口改变
#define PANEL_BAUDRATE			9600			//波特率
#define PANEL_PORTINIT			uart2_init		//通信端口初始化
#define PANEL_RECEIVE_CHAR		uart2_read_char		//通信端口接收
#define PANEL_SEND_CHAR		uart2_write_char	//通信端口发送
#define PANEL_SEND_BUF			uart2_write_buf		//通信端口发送
#define PANEL_FIFO_BACKUP		uart2_fifo_out_backup	//底层指针保存
#define PANEL_FIFO_RESUME		uart2_fifo_out_resume	//底层指针回读



//缓冲区长度定义--根据具体应用改变
#define PANEL_BUF_SIZE		64


//逻辑键定义
#define KEY_PRESET	1	//预设
#define KEY_MUTE	3	//静音
#define KEY_MANUAL	5	//手动
#define KEY_AUTO	7	//自动
#define KEY_TEST	9	//测试
#define KEY_60DB	2	//60dB
#define KEY_70DB	4	//70dB
#define KEY_80DB	6	//80dB
#define KEY_90DB	8	//90dB
#define KEY_100DB	10	//100dB


//模式定义
#define MODE_PRESET	1	//预设模式
#define MODE_MUTE	2	//静音模式
#define MODE_MANUAL	3	//手动模式
#define MODE_AUTO	4	//自动模式
#define MODE_TEST	5	//测试模式

//音量定义
#define VOLUME_60DB	5	//60dB
#define VOLUME_70DB	4	//70dB
#define VOLUME_80DB	3	//80dB
#define VOLUME_90DB	2	//90dB
#define VOLUME_100DB	1	//100dB

//发送的应答
#define PANEL_ACK_LED			0x19	//发送面板指示灯 [模式字节][音量字节]


//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(uint8,uint8,uint8 *);
	
	uint16 refresh_timer;//刷新时间 用于两边同步
	
}panel_dev_t;
extern panel_dev_t shzh24_panel_dev;

extern  void panel_proc(uint8 device_no,uint8 data_length,uint8 *buf);
extern void panel_send_cmd(uint8 cmd, uint8 *param, uint8 length);

#endif



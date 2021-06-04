//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 上位机设置 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:ptu.h

//*文件作用:上位机设置

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PTU_H
#define PTU_H



//PTU通信指示灯号
#define PTU_LED_NO		6


//输入输出端口定义--根据硬件接口改变
#define PTU_BAUDRATE		9600			//波特率
#define PTU_PORTINIT		usb_init		//通信端口初始化
#define PTU_RECEIVE_CHAR	usb_read_char		//通信端口接收
#define PTU_SEND_CHAR		usb_write_char		//通信端口发送
#define PTU_SEND_BUF		usb_write_buf		//通信端口发送

//设置命令
#define SETUP_CMD_STATIONNUM			0x01	//车站总数
#define SETUP_CMD_STATIONCODE			0x02	//站代码
#define SETUP_CMD_STATIONDISTANCE		0x03	//站间距
#define SETUP_CMD_PULSEDISTANCE		0x04	//脉冲运行距离
#define SETUP_CMD_ARRIVEDISTANCE		0x05	//到站广播距离
#define SETUP_CMD_LEAVEDISTANCE		0x06	//离站广播距离
#define SETUP_CMD_VOLUME			       0x07	//音量
#define SETUP_CMD_LEAVETABLE			0x08	//离站广播列表
#define SETUP_CMD_ARRIVETABLE			0x09	//到站广播列表
#define SETUP_CMD_PRE_DEST                    0x10      //终点站预报站广播
#define SETUP_CMD_VERSION			       0x11	//查询版本号
#define SETUP_CMD_RESET				 0x12	//复位
#define SETUP_CMD_ARRIVE_DEST                0x13      //到终点站广播列表
#define SETUP_CMD_DIS_STATIONCODE        0x14     //显示站代码
#define SETUP_CMD_DIS_VOLUME                 0x15     //查询音量
#define SETUP_CMD_DIS_ERROR                    0x16     //查询异常存储信息
#define SETUP_CMD_DIS_FLASH                      0x17  //查询flash
#define SETUP_CMD_UPDATE_SOFT                0x19     //升级
#define SETUP_CMD_LOAD_DEFAULT_PARM     0x20 //恢复默认配置
#define SETUP_CMD_OK                                 0x21   //收到ptu命令后，回复此命令
#define SETUP_CMD_DIS_BROADCASTPRIORITY  0x22  //查询广播优先级
#define SETUP_CMD_SETSTATION_NAME          0x23    //设置站名
#define SETUP_CMD_BROC_DELAY_TIME          0x24   //手动模式下设置每个站的延迟报站时间
#define SETUP_CMD_DIS_BROC_DELAY_TIME   0x25   //查询手动模式下设置每个站的延迟报站时间

#define SETUP_CMD_DMP_WDS_VERSION         0x30  //查询动态地图文字屏软件版本
#define SETUP_CMD_SECTION_PLAYLIST          0x31 //设置区间运行终点站提示语播放列表
#define SETUP_CMD_DIS_SECTION_PLAYLIST           0x32 //查询区间运行终点站提示语设置列表

#define SETUP_CMD_DCP_VOLUME           0x33 //设置广播盒控制音量
#define SETUP_CMD_DIS_DCP_VOLUME           0x34 //查询广播盒控制音量

#define SETUP_CMD_SPECIALTIMES			  0x0B	//特殊广播次数
#define SETUP_CMD_BROADCASTPRIORITY	  0x0C	//广播优先级
#define SETUP_CMD_MEDIASAMETIME		   0x0D	//是否同时媒体播放
#define SETUP_CMD_MTABLE			         0x0E	//令牌巡检表



//缓冲区长度定义--不能改变 关系到下载程序
#define PTU_BUF_SIZE		1024
extern uint8 ptu_recv_buf[];



//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	uint8 *(*get_debug_cmd)(void);
	void (*ptu_send_data)(uint8 *, uint8);
	uint8 print_flag;
}ptu_dev_t;
extern ptu_dev_t sh9_ptu_dev;



#endif



#ifndef __PROCESS_TCMS_H__
#define __PROCESS_TCMS_H__

#include <time.h>
#include "log.h"

#define PACK_ALIGN	__attribute__((packed))//__attribute__ ((packed))的作用就是告诉编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐
#define TCMS_UART_DEV "/dev/ttyAMA3"
#define MARK_START 0x7e
#define MARK_STOP  0x7e
#define MARK_DEST_DIFFE_NETWORK 0x02 //目的不同网络
#define MARK_DEST_SAME_NETWORK 0x00 //目的同一个网络
#define MARK_DEST_DEVICE 0x14 //不同网络 低4位填充设备类型，高4位填充设备跳线
#define MARK_SRC_DIFFE_NETWORK 0x02 //源不同网络
#define MARK_SRC_SAME_NETWORK 0x00 //源同一个网络
#define MARK_SRC_DEVICE 0x11 //不同网络 低4位填充设备类型，高4位填充设备跳线

#define Mem_DEV "/dev/mem"



#define MAX_CARRIAGE  8

typedef enum _ReadState_
{
	READ_STATE_START = 0,
	READ_STATE_LEN,
	READ_STATE_NETWOEK,
	READ_STATE_CONTENT,	
	READ_STATE_STOP,
} READ_STATE_EN;

typedef struct _DcpToLcd_
{
	int				fd;				// 串口句柄
	int 			readState;		// 控制读取那一部分
	unsigned char 	start;			// 开始
	unsigned char		destNetworkNum; //目的网络号
	unsigned char		destDeviceNum;//目的设备号
	unsigned char 	srcNetworkNum;//源网络号
	unsigned char		srcDeviceNum;//源设备号
	unsigned char		CMD;//令牌标志
	unsigned char 	len;			// 长度
	unsigned char 	content[256];	// 内容
	unsigned char		checksum; //校验
	unsigned char 	hadReadLen;		// 已接收的长度
	unsigned char 	stop;			// 停止
	unsigned char	recvSuccess;	// 1, 成功接收到一帧, 需要处理, 否则不处理
	struct timeval	t_base;			// 心跳基时间				
} DCP_TO_LCD_T;

typedef struct _TcmsTrigger_
{
#ifdef BIG_DEBAIN //大端 
	unsigned char arrAndStop:1;		// 到达停稳触发	
	unsigned char leaveStation:1;	// 离站触发
	unsigned char openDoor:1;	// 开门触发
	unsigned char closeDoor:1;	// 关门触发
	unsigned char urgentBro:1;	// 紧急广播触发
	unsigned char passStation:1;	// 越站触发
	unsigned char arrStation:1;	// 到站触发
	unsigned char preBroStation:1;	// 预报站触发触发
#else
	unsigned char preBroStation:1;	// 预报站触发触发	
	unsigned char arrStation:1;	// 到站触发
	unsigned char passStation:1;	// 越站触发
	unsigned char urgentBro:1;	// 紧急广播触发
	unsigned char closeDoor:1;	// 关门触发
	unsigned char openDoor:1;	// 开门触发
	unsigned char leaveStation:1;	// 离站触发
	unsigned char arrAndStop:1;		// 到达停稳触发 7
#endif
} PACK_ALIGN TCMS_TRIGGER_T;

typedef struct _TcmsCode_
{
	unsigned char beginStation; 	// 起始站代码
	unsigned char endStation; 		// 终点站代码	
	unsigned char currentStation; 	// 当前站代码
	unsigned char nextStation; 		// 下一站代码
	unsigned char passStationBro; 	// 越站广播代码
	unsigned char urgentBroCode; 	// 紧急广播代码
} PACK_ALIGN TCMS_CODE_T;

typedef struct _TcmsInfo_
{
#ifdef BIG_DEBAIN
	unsigned char goUp:1;			// 上行
	unsigned char goDown:1;			// 下行
	unsigned char openLeftDoor:1;	// 开左侧门	
	unsigned char openRightDoor:1;	// 开右侧门
	unsigned char upDownAvailable:1;		// 上下行有效
	unsigned char MainOrSlave:1;			// 	主备
	unsigned char activate:1;				// 	激活
	unsigned char passStationAvailable:1;	// 	越站有效
#else
	unsigned char passStationAvailable:1;	// 	越站有效 0
	unsigned char activate:1;				// 	激活
	unsigned char MainOrSlave:1;			// 	主备
	unsigned char upDownAvailable:1;		// 上下行有效
	unsigned char openRightDoor:1;	// 开右侧门
	unsigned char openLeftDoor:1;	// 开左侧门	
	unsigned char goDown:1;			// 下行
	unsigned char goUp:1;			// 上行 
#endif
} PACK_ALIGN TCMS_INFO_T;

typedef struct _DcpCity_
{
	unsigned char moniBrodcast:4;
	unsigned char piscMode:2;
	unsigned char cityId:2;
	
}PACK_ALIGN DCP_CITY_T;

typedef struct _DcpPriority_
{
	unsigned char clientUrgency:3;//乘客紧急报警优先级-3
	unsigned char zero1:1;
	unsigned char driverTalk:3;//司机对讲优先级-2
	unsigned char zero2:1;

	unsigned char manBroadcast:3;//人工广播优先级-6
	unsigned char zero3:1;
	unsigned char OCCBroadcast:3;//OCC广播优先级-4
	unsigned char zero4:1;

	unsigned char LCDBroadcast:3;//LCD伴音广播优先级-1
	unsigned char zero5:1;
	unsigned char DVABroadcast:3;// 紧急广播优先级
	unsigned char zero6:1;

	unsigned char closeTip:3;//关门提示音优先级-5
	unsigned char zero7:1;
	unsigned char urgBroadcast:3;// 紧急广播优先级-3
	unsigned char zero8:1;
}PACK_ALIGN DCP_PRIORITY_T;

typedef struct _DcpBroadsour_
{
	unsigned char applybro:1;//申请广播
	unsigned char broading:1;//正在广播
	unsigned char urgbro:1;//紧急广播
	unsigned char closetip:1;//关门提示音
	unsigned char LCDAud:1;//LCD伴音音源
	unsigned char numAud:1;//数字语音音源
	unsigned char OCCAud:1;//OCC广播音源
	unsigned char alarmclient:1;//乘客紧急报警
}PACK_ALIGN DCP_BROADSOUR_T;

typedef struct _DcpCab_
{
	unsigned char cab_1_alarm_4:2;//报警器No.4
	unsigned char cab_1_alarm_3:2;
	unsigned char cab_1_alarm_2:2;
	unsigned char cab_1_alarm_1:2;

	unsigned char cab_2_alarm_4:2;
	unsigned char cab_2_alarm_3:2;
	unsigned char cab_2_alarm_2:2;
	unsigned char cab_2_alarm_1:2;

	unsigned char cab_3_alarm_4:2;
	unsigned char cab_3_alarm_3:2;
	unsigned char cab_3_alarm_2:2;
	unsigned char cab_3_alarm_1:2;

	unsigned char cab_4_alarm_4:2;
	unsigned char cab_4_alarm_3:2;
	unsigned char cab_4_alarm_2:2;
	unsigned char cab_4_alarm_1:2;

	unsigned char cab_5_alarm_4:2;
	unsigned char cab_5_alarm_3:2;
	unsigned char cab_5_alarm_2:2;
	unsigned char cab_5_alarm_1:2;

	unsigned char cab_6_alarm_4:2;
	unsigned char cab_6_alarm_3:2;
	unsigned char cab_6_alarm_2:2;
	unsigned char cab_6_alarm_1:2;

	unsigned char cab_7_alarm_4:2;
	unsigned char cab_7_alarm_3:2;
	unsigned char cab_7_alarm_2:2;
	unsigned char cab_7_alarm_1:2;

	unsigned char cab_8_alarm_4:2;
	unsigned char cab_8_alarm_3:2;
	unsigned char cab_8_alarm_2:2;
	unsigned char cab_8_alarm_1:2;
}PACK_ALIGN DCP_CAB_T;

typedef struct _DcpLineNum_
{
	unsigned char version_low:4;
	unsigned char version_high:3;
	unsigned char pis:1;
}PACK_ALIGN DCP_LINE_T;

typedef struct _CommunFault_
{
	unsigned char mediaErr:1;//媒体通信错误
	unsigned char DVAErr:1;//DVA通信错误
	unsigned char APUErr:1;//APU通信错误
	unsigned char reserve:2;
	unsigned char driStat1:1;
	unsigned char driStat2:1;
	unsigned char AtcorHandMode:1;
}PACK_ALIGN COMMUN_FAULT_T;

typedef struct _TMS_
{
	unsigned char checkDcpToPiscCount:6;//检查dcp发送了pis数据包的次数
	unsigned char tms_mode:2;
}PACK_ALIGN TMS_T;

typedef struct _Volume_
{
	unsigned char moni_broadcast_off:4;
	unsigned char volumeFlag:4;
}PACK_ALIGN VOLUME_T;

typedef struct _TimeDay_
{
	unsigned char hour;//小时
	unsigned char minutes;//分钟
}PACK_ALIGN TIME_DAY_T;

typedef struct _TestStat_
{
	unsigned char LEDTest:1;//Led test
	unsigned char broadcastTest:1;//broadcast test
	unsigned char MasterReq:1;//Master req
	unsigned char SlaveReq:1;//slave req
	unsigned char reserve:1;//
	unsigned char relineFlag:1;//重联标识
	unsigned char moveDistentOneThrid:1;//运行距离三分之一
	unsigned char moveDistentTwoThrid:1;//运行距离三分之二
}PACK_ALIGN TEST_STAT_T;

typedef struct _Datatime_
{
	unsigned char year:8;		// 年
	unsigned char month:8;	// 月
	unsigned char day:8;	// 日
} PACK_ALIGN DATATIME_T;

typedef struct _Error_
{
	unsigned char srck1:1;
	unsigned char srck2:1;
	unsigned char srck3:1;
	unsigned char srck4:1;
	unsigned char srck5:1;
	unsigned char srck6:1;//客室6故障
	unsigned char crck1:1;//
	unsigned char crck2:1;//中央控制器2故障
	
	unsigned char dva1:1;
	unsigned char dva2:1;//dva2通信错误
	unsigned char apu1:1;
	unsigned char apu2:1;//apu2通信错误
	unsigned char dcp1:1;
	unsigned char dcp2:1;//广播控制盒
	unsigned char record1:1;
	unsigned char record2:1;//录音模块..;没有
	
	unsigned char atc1:1;
	unsigned char atc2:1;//ATC/TCMS模块
	unsigned char io1:1;	
	unsigned char io2:1;
	unsigned char io3:1;
	unsigned char io4:1;
	unsigned char io5:1;
	unsigned char io6:1;//io6或动态地图故障

}PACK_ALIGN ERROTOR_T;

typedef struct _Reserve_
{
	unsigned char reserve1;
	unsigned char reserve2;
	unsigned char reserve3;
	unsigned char reserve4;
	unsigned char reserve5;
}PACK_ALIGN RESERVE_T;
// 接收内容!!!!!!!!!!!
typedef struct _DcpToLcdContent_
{
	TCMS_TRIGGER_T 	trigger; //触发 1
	TCMS_CODE_T	code;//代码 2-7
	TCMS_INFO_T		info;//第8个字节的信息 8
	DCP_CITY_T		city;//城市号 9
	DATATIME_T		datatime;//时间 10-12
	DCP_PRIORITY_T 	priority; //优先级别 13-16
	DCP_BROADSOUR_T source;//广播信源 17
	DCP_CAB_T		cab;//车厢1-8 18-25
	DCP_LINE_T		linemode;//线路号 26
	COMMUN_FAULT_T communfault;//司机室通信错误 27
	TMS_T			TMS;//TMS专用字节 28
	VOLUME_T		volume;//音量字节 29
	TIME_DAY_T		time;//时间 30-31
	TEST_STAT_T		testStat;//测试状态 32
	ERROTOR_T		errorProc;//故障信息 33-35
	//RESERVE_T		reserve;//保留36-40
} PACK_ALIGN DCP_TO_LCD_CONTENT_T;

/********************************发送内容***********************************/

typedef struct _lcdCode_
{
	unsigned char beginStation; 	// 起始站代码
	unsigned char endStation; 		// 终点站代码	
	unsigned char currentStation; 	// 当前站代码
	unsigned char nextStation; 		// 下一站代码
	unsigned char passStationBro; 	// 越站广播代码
	unsigned char urgentBroCode; 	// 紧急广播代码6
} PACK_ALIGN LCD_CODE_T;

typedef struct _LCDTrigger_
{
	unsigned char leaveBroadcast:1;//预报广播触发
	unsigned char arivedBroadcast:1;//到站广播触发
	unsigned char jumpBroadcast:1;	//越站广播触发
	unsigned char stopUrgentBroadcast:1;//紧急广播触发
	unsigned char stopBroadcast:1;//停止广播触发
	unsigned char muteBroadcast:1;//静音触发
	unsigned char monitorBroadcast:1;//监听广播触发
	unsigned char gusetroomBroadcast:1;//客室广播触发7
}PACK_ALIGN LCD_TRIGGER_T;

typedef struct _status_
{
	unsigned char talking:2;//司机对讲状态1,司机对讲状态2
	unsigned char link:1;//重连
	unsigned char emTalk:1;//紧急对讲
	unsigned char rundir:1;//上下行
	unsigned char testState:1;//测试状态
	unsigned char foreState:1;//强制状态
	unsigned char active:1;//激活状态8
}PACK_ALIGN STATUS_T;

typedef struct _LineMode_//路线模式 
{
	unsigned char piscMode:2;
	unsigned char cityId:2;
	unsigned char moniBroadcast:4;
}PACK_ALIGN LINE_MODE_T;

typedef struct _WorkMode_
{
	unsigned char workmode:2;//工作模式 01DCP自动 10手动 11逻辑
	unsigned char closeATC:1;// 1关闭ATC 0不关闭
	unsigned char reserve:5;
}PACK_ALIGN WORK_MODE_T;

typedef struct _Version_ //版本 
{
	unsigned char versionh;
	unsigned char versionl;
}PACK_ALIGN VERSION_T;

//发送内容
typedef struct _LcdToDcpContent_
{
	LCD_CODE_T		code;//代码 1-6
	LCD_TRIGGER_T	trigger;//触发 7
	STATUS_T		status;//状态 8
	LINE_MODE_T		lineMode;//路线模式 9
	WORK_MODE_T	workMode;//工作模式 10
	VERSION_T		version;//版本 11-12
}PACK_ALIGN LCD_TO_DCP_CONNECT_T;

typedef struct _LCDToDcp_
{
	unsigned char 	start;			// 开始
	unsigned char		destNetworkNum; //目的网络号
	unsigned char		destDeviceNum;//目的设备号
	unsigned char 	srcNetworkNum;//源网络号
	unsigned char		srcDeviceNum;//源设备号
	unsigned char		CMD;//令牌标志
	unsigned char 	len;			// 长度
	LCD_TO_DCP_CONNECT_T 	content;				// 内容包
	unsigned char		checksum; //校验
	unsigned char 	stop;			// 停止	
	int				fd;				// 串口句柄
} PACK_ALIGN LCD_TO_DCP_T;


#undef  PACK_ALIGN //取消宏定义

void Start_process_tcms_thread();

#endif // __PROCESS_TCMS_H__


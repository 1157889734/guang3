#ifndef __LCDUART_H__
#define __LCDUART_H__

#include <time.h> 
#include <assert.h>

#define PACK_ALIGN	__attribute__((packed))//__attribute__ ((packed))的作用就是告诉编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐

#define UART_BUF_LEN 255

#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符

#define PROCESS_PACKET			0x00
#define P2P_PACKET				0x01
#define TOKEN_PACKET			0x02
#define CLR_TOKEN				0x03
#define DOWNLOAD_PACKET			0x04

#define DEVICE_TYPE_BCB			0x02	//广播控制盒


/*typedef struct t_Dcp_To_Lcd
{
	unsigned char beginStation; 	// 起始站代码
	unsigned char endStation; 		// 终点站代码	
	unsigned char currentStation; 	// 当前站代码
	unsigned char nextStation; 		// 下一站代码
	unsigned char passStationBro; 	// 越站广播代码
	unsigned char urgentBroCode; 	// 紧急广播代码//代码 1-6
	
	struct 
	{
		unsigned char leave_broadcast:1;//预报广播触发
		unsigned char arrived_broadcast:1;//到站广播触发
		unsigned char jump:1;	//越站广播触发
		unsigned char urgent_broadcast:1;//紧急广播触发
		unsigned char stopBroadcast:1;//停止广播触发
		unsigned char muteBroadcast:1;//静音触发
		unsigned char monitorBroadcast:1;//监听广播触发
		unsigned char gusetroomBroadcast:1;//客室广播触发
	}broadcast_hbit;//触发 7

	 struct 
	{
		unsigned char talking:2;//司机对讲状态1,司机对讲状态2
		unsigned char link:1;//重连
		unsigned char emTalk:1;//紧急对讲
		unsigned char rundir:1;//上下行
		unsigned char testState:1;//测试状态
		unsigned char foreState:1;//强制状态
		unsigned char active:1;//激活状态
	}status_hbit;
	
	struct //路线模式 
	{
		unsigned char piscMode:2;
		unsigned char cityId:6;
	}line_mode;
	struct 
	{
		unsigned char workmode:2;//工作模式 01DCP自动 10手动 11逻辑
		unsigned char closeATC:1;// 1关闭ATC 0不关闭
		unsigned char reserve:5;
	}work_mode;//工作模式 10
	
	unsigned char version_h;
	unsigned char version_l; //版本 11-12
}tDcpToLcd;
*/
typedef struct t_Frame_Header
{
	unsigned char dest_netnum;
	//低位在前
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;		
		
	}PACK_ALIGN dest_eqnum;
	unsigned char src_netnum;
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;		
	}PACK_ALIGN src_eqnum;
	struct
	{
		unsigned char packet_type:4;
		unsigned char token_return:1;
		unsigned char m_active:1;
		unsigned char m_error:1;
		unsigned char m_include:1;			
	}PACK_ALIGN cmd;
	unsigned char len;
	unsigned char data[3];
}PACK_ALIGN tFrameHeader;



/******************************************************************/
/*typedef struct _TcmsTrigger_
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
	unsigned char citynum:8;
}PACK_ALIGN DCP_CITY_T;

typedef struct _Datatime_
{
	unsigned char year:8;		// 年
	unsigned char month:8;	// 月
	unsigned char day:8;	// 日
} PACK_ALIGN DATATIME_T;

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
	unsigned char lineMode;//线路号
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
	unsigned char reserve;
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
} PACK_ALIGN DCP_TO_LCD_CONTENT_T;*/
/*************************************************************/
typedef struct t_Dcp_Header
{
	unsigned char dest_netnum;
	//低位在前
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;		
		
	}PACK_ALIGN dest_eqnum;
	unsigned char src_netnum;
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;		
	}PACK_ALIGN src_eqnum;
	unsigned char cmd;
	unsigned char len;
	DCP_TO_LCD_CONTENT_T data_area;
}PACK_ALIGN tDcpHeader;


void Lcd_Serial_process_Install();

#endif

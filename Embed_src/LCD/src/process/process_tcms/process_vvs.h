#ifndef __PROCESSVVS_H__
#define __PROCESSVVS_H__

#include "process_tcms.h"
#include "LcdUart.h"


#define PACK_ALIGN	__attribute__((packed))

#define VVS_START	0x7E
#define VVS_LEN		0x0E
#define VVS_CMD		0x82
#define VVS_STOP	0x0A

#define VVS_IP		"192.168.101.68"
#define VVS_PORT	5505

typedef struct _NvrtToVvs_
{
	int	fd;				// 网络句柄
	int t_reconnect;
} NVRT_TO_VVS_T;

typedef struct _NvrtToVvsContent_
{
	//unsigned char 	start;			// =0x7E 开始	
	//unsigned char 	len;			// =0x0E 后面总共跟多少个字节
	//unsigned char 	cmd;			// =0x82 开始
	TCMS_TRIGGER_T 	trigger; //触发 1
	TCMS_CODE_T		code;//代码 2-7
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
} PACK_ALIGN NVRT_TO_VVS_CONTENT_T;

#undef PACK_ALIGN

void Start_process_vvs_thread();
void VvsSetContent( DCP_TO_LCD_CONTENT_T *pTcmsToNvrtContent );


#endif // __PROCESSVVS_H__


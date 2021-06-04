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
	int	fd;				// ������
	int t_reconnect;
} NVRT_TO_VVS_T;

typedef struct _NvrtToVvsContent_
{
	//unsigned char 	start;			// =0x7E ��ʼ	
	//unsigned char 	len;			// =0x0E �����ܹ������ٸ��ֽ�
	//unsigned char 	cmd;			// =0x82 ��ʼ
	TCMS_TRIGGER_T 	trigger; //���� 1
	TCMS_CODE_T		code;//���� 2-7
	TCMS_INFO_T		info;//��8���ֽڵ���Ϣ 8
	DCP_CITY_T		city;//���к� 9
	DATATIME_T		datatime;//ʱ�� 10-12
	DCP_PRIORITY_T 	priority; //���ȼ��� 13-16
	DCP_BROADSOUR_T source;//�㲥��Դ 17
	DCP_CAB_T		cab;//����1-8 18-25
	DCP_LINE_T		linemode;//��·�� 26
	COMMUN_FAULT_T communfault;//˾����ͨ�Ŵ��� 27
	TMS_T			TMS;//TMSר���ֽ� 28
	VOLUME_T		volume;//�����ֽ� 29
	TIME_DAY_T		time;//ʱ�� 30-31
	TEST_STAT_T		testStat;//����״̬ 32
	ERROTOR_T		errorProc;//������Ϣ 33-35
} PACK_ALIGN NVRT_TO_VVS_CONTENT_T;

#undef PACK_ALIGN

void Start_process_vvs_thread();
void VvsSetContent( DCP_TO_LCD_CONTENT_T *pTcmsToNvrtContent );


#endif // __PROCESSVVS_H__


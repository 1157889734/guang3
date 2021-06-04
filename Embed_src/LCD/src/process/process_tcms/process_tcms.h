#ifndef __PROCESS_TCMS_H__
#define __PROCESS_TCMS_H__

#include <time.h>
#include "log.h"

#define PACK_ALIGN	__attribute__((packed))//__attribute__ ((packed))�����þ��Ǹ��߱�����ȡ���ṹ�ڱ�������е��Ż�����,����ʵ��ռ���ֽ������ж���
#define TCMS_UART_DEV "/dev/ttyAMA3"
#define MARK_START 0x7e
#define MARK_STOP  0x7e
#define MARK_DEST_DIFFE_NETWORK 0x02 //Ŀ�Ĳ�ͬ����
#define MARK_DEST_SAME_NETWORK 0x00 //Ŀ��ͬһ������
#define MARK_DEST_DEVICE 0x14 //��ͬ���� ��4λ����豸���ͣ���4λ����豸����
#define MARK_SRC_DIFFE_NETWORK 0x02 //Դ��ͬ����
#define MARK_SRC_SAME_NETWORK 0x00 //Դͬһ������
#define MARK_SRC_DEVICE 0x11 //��ͬ���� ��4λ����豸���ͣ���4λ����豸����

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
	int				fd;				// ���ھ��
	int 			readState;		// ���ƶ�ȡ��һ����
	unsigned char 	start;			// ��ʼ
	unsigned char		destNetworkNum; //Ŀ�������
	unsigned char		destDeviceNum;//Ŀ���豸��
	unsigned char 	srcNetworkNum;//Դ�����
	unsigned char		srcDeviceNum;//Դ�豸��
	unsigned char		CMD;//���Ʊ�־
	unsigned char 	len;			// ����
	unsigned char 	content[256];	// ����
	unsigned char		checksum; //У��
	unsigned char 	hadReadLen;		// �ѽ��յĳ���
	unsigned char 	stop;			// ֹͣ
	unsigned char	recvSuccess;	// 1, �ɹ����յ�һ֡, ��Ҫ����, ���򲻴���
	struct timeval	t_base;			// ������ʱ��				
} DCP_TO_LCD_T;

typedef struct _TcmsTrigger_
{
#ifdef BIG_DEBAIN //��� 
	unsigned char arrAndStop:1;		// ����ͣ�ȴ���	
	unsigned char leaveStation:1;	// ��վ����
	unsigned char openDoor:1;	// ���Ŵ���
	unsigned char closeDoor:1;	// ���Ŵ���
	unsigned char urgentBro:1;	// �����㲥����
	unsigned char passStation:1;	// Խվ����
	unsigned char arrStation:1;	// ��վ����
	unsigned char preBroStation:1;	// Ԥ��վ��������
#else
	unsigned char preBroStation:1;	// Ԥ��վ��������	
	unsigned char arrStation:1;	// ��վ����
	unsigned char passStation:1;	// Խվ����
	unsigned char urgentBro:1;	// �����㲥����
	unsigned char closeDoor:1;	// ���Ŵ���
	unsigned char openDoor:1;	// ���Ŵ���
	unsigned char leaveStation:1;	// ��վ����
	unsigned char arrAndStop:1;		// ����ͣ�ȴ��� 7
#endif
} PACK_ALIGN TCMS_TRIGGER_T;

typedef struct _TcmsCode_
{
	unsigned char beginStation; 	// ��ʼվ����
	unsigned char endStation; 		// �յ�վ����	
	unsigned char currentStation; 	// ��ǰվ����
	unsigned char nextStation; 		// ��һվ����
	unsigned char passStationBro; 	// Խվ�㲥����
	unsigned char urgentBroCode; 	// �����㲥����
} PACK_ALIGN TCMS_CODE_T;

typedef struct _TcmsInfo_
{
#ifdef BIG_DEBAIN
	unsigned char goUp:1;			// ����
	unsigned char goDown:1;			// ����
	unsigned char openLeftDoor:1;	// �������	
	unsigned char openRightDoor:1;	// ���Ҳ���
	unsigned char upDownAvailable:1;		// ��������Ч
	unsigned char MainOrSlave:1;			// 	����
	unsigned char activate:1;				// 	����
	unsigned char passStationAvailable:1;	// 	Խվ��Ч
#else
	unsigned char passStationAvailable:1;	// 	Խվ��Ч 0
	unsigned char activate:1;				// 	����
	unsigned char MainOrSlave:1;			// 	����
	unsigned char upDownAvailable:1;		// ��������Ч
	unsigned char openRightDoor:1;	// ���Ҳ���
	unsigned char openLeftDoor:1;	// �������	
	unsigned char goDown:1;			// ����
	unsigned char goUp:1;			// ���� 
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
	unsigned char clientUrgency:3;//�˿ͽ����������ȼ�-3
	unsigned char zero1:1;
	unsigned char driverTalk:3;//˾���Խ����ȼ�-2
	unsigned char zero2:1;

	unsigned char manBroadcast:3;//�˹��㲥���ȼ�-6
	unsigned char zero3:1;
	unsigned char OCCBroadcast:3;//OCC�㲥���ȼ�-4
	unsigned char zero4:1;

	unsigned char LCDBroadcast:3;//LCD�����㲥���ȼ�-1
	unsigned char zero5:1;
	unsigned char DVABroadcast:3;// �����㲥���ȼ�
	unsigned char zero6:1;

	unsigned char closeTip:3;//������ʾ�����ȼ�-5
	unsigned char zero7:1;
	unsigned char urgBroadcast:3;// �����㲥���ȼ�-3
	unsigned char zero8:1;
}PACK_ALIGN DCP_PRIORITY_T;

typedef struct _DcpBroadsour_
{
	unsigned char applybro:1;//����㲥
	unsigned char broading:1;//���ڹ㲥
	unsigned char urgbro:1;//�����㲥
	unsigned char closetip:1;//������ʾ��
	unsigned char LCDAud:1;//LCD������Դ
	unsigned char numAud:1;//����������Դ
	unsigned char OCCAud:1;//OCC�㲥��Դ
	unsigned char alarmclient:1;//�˿ͽ�������
}PACK_ALIGN DCP_BROADSOUR_T;

typedef struct _DcpCab_
{
	unsigned char cab_1_alarm_4:2;//������No.4
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
	unsigned char mediaErr:1;//ý��ͨ�Ŵ���
	unsigned char DVAErr:1;//DVAͨ�Ŵ���
	unsigned char APUErr:1;//APUͨ�Ŵ���
	unsigned char reserve:2;
	unsigned char driStat1:1;
	unsigned char driStat2:1;
	unsigned char AtcorHandMode:1;
}PACK_ALIGN COMMUN_FAULT_T;

typedef struct _TMS_
{
	unsigned char checkDcpToPiscCount:6;//���dcp������pis���ݰ��Ĵ���
	unsigned char tms_mode:2;
}PACK_ALIGN TMS_T;

typedef struct _Volume_
{
	unsigned char moni_broadcast_off:4;
	unsigned char volumeFlag:4;
}PACK_ALIGN VOLUME_T;

typedef struct _TimeDay_
{
	unsigned char hour;//Сʱ
	unsigned char minutes;//����
}PACK_ALIGN TIME_DAY_T;

typedef struct _TestStat_
{
	unsigned char LEDTest:1;//Led test
	unsigned char broadcastTest:1;//broadcast test
	unsigned char MasterReq:1;//Master req
	unsigned char SlaveReq:1;//slave req
	unsigned char reserve:1;//
	unsigned char relineFlag:1;//������ʶ
	unsigned char moveDistentOneThrid:1;//���о�������֮һ
	unsigned char moveDistentTwoThrid:1;//���о�������֮��
}PACK_ALIGN TEST_STAT_T;

typedef struct _Datatime_
{
	unsigned char year:8;		// ��
	unsigned char month:8;	// ��
	unsigned char day:8;	// ��
} PACK_ALIGN DATATIME_T;

typedef struct _Error_
{
	unsigned char srck1:1;
	unsigned char srck2:1;
	unsigned char srck3:1;
	unsigned char srck4:1;
	unsigned char srck5:1;
	unsigned char srck6:1;//����6����
	unsigned char crck1:1;//
	unsigned char crck2:1;//���������2����
	
	unsigned char dva1:1;
	unsigned char dva2:1;//dva2ͨ�Ŵ���
	unsigned char apu1:1;
	unsigned char apu2:1;//apu2ͨ�Ŵ���
	unsigned char dcp1:1;
	unsigned char dcp2:1;//�㲥���ƺ�
	unsigned char record1:1;
	unsigned char record2:1;//¼��ģ��..;û��
	
	unsigned char atc1:1;
	unsigned char atc2:1;//ATC/TCMSģ��
	unsigned char io1:1;	
	unsigned char io2:1;
	unsigned char io3:1;
	unsigned char io4:1;
	unsigned char io5:1;
	unsigned char io6:1;//io6��̬��ͼ����

}PACK_ALIGN ERROTOR_T;

typedef struct _Reserve_
{
	unsigned char reserve1;
	unsigned char reserve2;
	unsigned char reserve3;
	unsigned char reserve4;
	unsigned char reserve5;
}PACK_ALIGN RESERVE_T;
// ��������!!!!!!!!!!!
typedef struct _DcpToLcdContent_
{
	TCMS_TRIGGER_T 	trigger; //���� 1
	TCMS_CODE_T	code;//���� 2-7
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
	//RESERVE_T		reserve;//����36-40
} PACK_ALIGN DCP_TO_LCD_CONTENT_T;

/********************************��������***********************************/

typedef struct _lcdCode_
{
	unsigned char beginStation; 	// ��ʼվ����
	unsigned char endStation; 		// �յ�վ����	
	unsigned char currentStation; 	// ��ǰվ����
	unsigned char nextStation; 		// ��һվ����
	unsigned char passStationBro; 	// Խվ�㲥����
	unsigned char urgentBroCode; 	// �����㲥����6
} PACK_ALIGN LCD_CODE_T;

typedef struct _LCDTrigger_
{
	unsigned char leaveBroadcast:1;//Ԥ���㲥����
	unsigned char arivedBroadcast:1;//��վ�㲥����
	unsigned char jumpBroadcast:1;	//Խվ�㲥����
	unsigned char stopUrgentBroadcast:1;//�����㲥����
	unsigned char stopBroadcast:1;//ֹͣ�㲥����
	unsigned char muteBroadcast:1;//��������
	unsigned char monitorBroadcast:1;//�����㲥����
	unsigned char gusetroomBroadcast:1;//���ҹ㲥����7
}PACK_ALIGN LCD_TRIGGER_T;

typedef struct _status_
{
	unsigned char talking:2;//˾���Խ�״̬1,˾���Խ�״̬2
	unsigned char link:1;//����
	unsigned char emTalk:1;//�����Խ�
	unsigned char rundir:1;//������
	unsigned char testState:1;//����״̬
	unsigned char foreState:1;//ǿ��״̬
	unsigned char active:1;//����״̬8
}PACK_ALIGN STATUS_T;

typedef struct _LineMode_//·��ģʽ 
{
	unsigned char piscMode:2;
	unsigned char cityId:2;
	unsigned char moniBroadcast:4;
}PACK_ALIGN LINE_MODE_T;

typedef struct _WorkMode_
{
	unsigned char workmode:2;//����ģʽ 01DCP�Զ� 10�ֶ� 11�߼�
	unsigned char closeATC:1;// 1�ر�ATC 0���ر�
	unsigned char reserve:5;
}PACK_ALIGN WORK_MODE_T;

typedef struct _Version_ //�汾 
{
	unsigned char versionh;
	unsigned char versionl;
}PACK_ALIGN VERSION_T;

//��������
typedef struct _LcdToDcpContent_
{
	LCD_CODE_T		code;//���� 1-6
	LCD_TRIGGER_T	trigger;//���� 7
	STATUS_T		status;//״̬ 8
	LINE_MODE_T		lineMode;//·��ģʽ 9
	WORK_MODE_T	workMode;//����ģʽ 10
	VERSION_T		version;//�汾 11-12
}PACK_ALIGN LCD_TO_DCP_CONNECT_T;

typedef struct _LCDToDcp_
{
	unsigned char 	start;			// ��ʼ
	unsigned char		destNetworkNum; //Ŀ�������
	unsigned char		destDeviceNum;//Ŀ���豸��
	unsigned char 	srcNetworkNum;//Դ�����
	unsigned char		srcDeviceNum;//Դ�豸��
	unsigned char		CMD;//���Ʊ�־
	unsigned char 	len;			// ����
	LCD_TO_DCP_CONNECT_T 	content;				// ���ݰ�
	unsigned char		checksum; //У��
	unsigned char 	stop;			// ֹͣ	
	int				fd;				// ���ھ��
} PACK_ALIGN LCD_TO_DCP_T;


#undef  PACK_ALIGN //ȡ���궨��

void Start_process_tcms_thread();

#endif // __PROCESS_TCMS_H__


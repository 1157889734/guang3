
/******************************************************************************
*�ļ����ƣ�config.h	       		         
*��          �ߣ�hk           							 
*��          �������������ļ�	
*�������ڣ�2010-3-16
*�޸ļ�¼����								
******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H



//��Ŀʶ���
#define PROJECT_ID		110	//���ڵ���24�г�(110)

#define DEBUG_PRINT 0

//�汾��
#define VENDER_NAME		"********�����б��������ͨ�������޹�˾********"
#define SYSTEM_NAME		"************����3���߸���PIDSϵͳ************"
#define PRODECT_NAME		"****************���������****************"
#define PRODECT_CODE			"TBA2472"

//��������:�������ݺ�Ѳ������һ���ͣ�����ʱ��30ms����Կ�ײ�㲥��Ѳ����200ms��������������
//#define VERSION_DATE           "�汾ʱ��  2014-12-18 v2.2"    
//#define VERSION_DATE           "�汾ʱ��  2015-02-09 v2.4"   
//#define VERSION_DATE           "�汾ʱ��  2018-04-16 v2.5"   //�޸���������������
//#define VERSION_DATE           "�汾ʱ��  2018-05-17 v2.6"   //�޸Ļ�����������
#define VERSION_DATE           "�汾ʱ��  2019-10-10 v2.7"   //��Ӵ�С��·

//Ӳ���汾�� 1 2....99
#define HARDWARE_VERSION		2
//���ӹ��ܰ汾��
#define ADD_FUNCTION_VERSION		1
//��汾��
#define MAJOR_VERSION			2
//С�汾��
#define MINOR_VERSION			7

#define SZ5_LINE  1


//վ����
#define STATION_TOTAL_NUM		30


//�����----������
#define MY_NET_ID		0xFE
//�豸����----���������
#define MY_DEV_TYPE		0x01
//�豸���
#define MY_DEV_NO		0x01





//���Զ˿�
#define DEBUG_BAUDRATE			9600//������
#define DEBUG_PORTINIT			usb_init
#define DEBUG_RECEIVE_CHAR		usb_read_char
#define DEBUG_SEND_CHAR			usb_write_char
#define DEBUG_SEND_STR			usb_write_str
#define DEBUG_SEND_BUF			usb_write_buf



#endif



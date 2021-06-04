//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ���ֱ�վ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:dva.h

//*�ļ�����:���ֱ�վ��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DVA_H
#define DVA_H



//Ӳ������
//Ӳ���汾1
#if(HARDWARE_VERSION==1)
#define DVA_PLAYING_DET_REG		GPIO2_PD
#define DVA_PLAYING_DET_BIT		(6)
//Ӳ���汾2
#elif(HARDWARE_VERSION==2)
#define DVA_PLAYING_DET_REG		GPIO2_PD
#define DVA_PLAYING_DET_BIT		(12)
#endif


//��������˿ڶ���--����Ӳ���ӿڸı�
/*
#define DVA_BAUDRATE		9600			//������
#define DVA_PORTINIT		uart3_init		//ͨ�Ŷ˿ڳ�ʼ��
#define DVA_RECEIVE_CHAR	uart3_read_char		//ͨ�Ŷ˿ڽ���
#define DVA_SEND_CHAR		uart3_write_char	//ͨ�Ŷ˿ڷ���
#define DVA_SEND_BUF		uart3_write_buf		//ͨ�Ŷ˿ڷ���
#define DVA_FIFO_BACKUP		uart3_fifo_out_backup	//�ײ�ָ�뱣��
#define DVA_FIFO_RESUME		uart3_fifo_out_resume	//�ײ�ָ��ض�
*/

//���������ȶ���--���ݾ���Ӧ�øı�
#define DVA_BUF_SIZE		64
#define DVA_LIST_MAX		8

#define COMM_DVA_TIMEOUT	80

//begin add by hk 2010-5-24   for �������ļ���������
#define  NAME_RULE_NEW    1
//end add by hk 2010-5-24   for �������ļ���������

//DVA����
#define DVA_NUM             1
extern void dva_proc(uint8 device_no,uint8 data_length,uint8 *buf);
//�����豸��
typedef struct
{
	//void (*init)(void);
	//void (*proc)(void);
	#if NAME_RULE_NEW 
	void (*play_list)(uint16);
	#else
	void (*play_list)(uint16 *);
	#endif
	void (*play_special)(uint16);
	void (*stop)(void);
	uint8 (*read_version)(void);
	uint8 (*read_status)(void);
	void (*set_volume)(uint8);
	uint8 (*get_playing)(void);
	//void (*send_cmd)(uint8 , uint8 *, uint8);
	uint8 (*get_record_status)(void);
	void (*dva_clear)(void);
	//uint8 (*dva_get_play_status)(void);
	uint16 comm_error;//dva  ����ͨ�Ŵ���
	uint16 record_error;//¼��ģ�����
}dva_dev_t;
extern dva_dev_t sh9_dva_dev;

//extern uint8 dva_comm_err[DVA_NUM];

#endif



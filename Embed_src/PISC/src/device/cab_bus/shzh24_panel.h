//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ����豸�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:panel.h

//*�ļ�����:����豸��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PANEL_H
#define PANEL_H




//��������˿ڶ���--����Ӳ���ӿڸı�
#define PANEL_BAUDRATE			9600			//������
#define PANEL_PORTINIT			uart2_init		//ͨ�Ŷ˿ڳ�ʼ��
#define PANEL_RECEIVE_CHAR		uart2_read_char		//ͨ�Ŷ˿ڽ���
#define PANEL_SEND_CHAR		uart2_write_char	//ͨ�Ŷ˿ڷ���
#define PANEL_SEND_BUF			uart2_write_buf		//ͨ�Ŷ˿ڷ���
#define PANEL_FIFO_BACKUP		uart2_fifo_out_backup	//�ײ�ָ�뱣��
#define PANEL_FIFO_RESUME		uart2_fifo_out_resume	//�ײ�ָ��ض�



//���������ȶ���--���ݾ���Ӧ�øı�
#define PANEL_BUF_SIZE		64


//�߼�������
#define KEY_PRESET	1	//Ԥ��
#define KEY_MUTE	3	//����
#define KEY_MANUAL	5	//�ֶ�
#define KEY_AUTO	7	//�Զ�
#define KEY_TEST	9	//����
#define KEY_60DB	2	//60dB
#define KEY_70DB	4	//70dB
#define KEY_80DB	6	//80dB
#define KEY_90DB	8	//90dB
#define KEY_100DB	10	//100dB


//ģʽ����
#define MODE_PRESET	1	//Ԥ��ģʽ
#define MODE_MUTE	2	//����ģʽ
#define MODE_MANUAL	3	//�ֶ�ģʽ
#define MODE_AUTO	4	//�Զ�ģʽ
#define MODE_TEST	5	//����ģʽ

//��������
#define VOLUME_60DB	5	//60dB
#define VOLUME_70DB	4	//70dB
#define VOLUME_80DB	3	//80dB
#define VOLUME_90DB	2	//90dB
#define VOLUME_100DB	1	//100dB

//���͵�Ӧ��
#define PANEL_ACK_LED			0x19	//�������ָʾ�� [ģʽ�ֽ�][�����ֽ�]


//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(uint8,uint8,uint8 *);
	
	uint16 refresh_timer;//ˢ��ʱ�� ��������ͬ��
	
}panel_dev_t;
extern panel_dev_t shzh24_panel_dev;

extern  void panel_proc(uint8 device_no,uint8 data_length,uint8 *buf);
extern void panel_send_cmd(uint8 cmd, uint8 *param, uint8 length);

#endif



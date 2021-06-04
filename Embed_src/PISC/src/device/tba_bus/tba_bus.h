//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �г�ͨ�����߽ӿ� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:tba_bus.h

//*�ļ�����:�г�ͨ�����߽ӿ�

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TBA_BUS_H
#define TBA_BUS_H



//�г�����ͨ��ָʾ�ƺ�
#define TBA_BUS_LED_NO		3



//��������˿ڶ���--����Ӳ���ӿڸı�
#define TBA_BUS_BAUDRATE	9600			//������
#define TBA_BUS_PORTINIT	uart1_init		//ͨ�Ŷ˿ڳ�ʼ��
#define TBA_BUS_RECEIVE_CHAR	uart1_read_char		//ͨ�Ŷ˿ڽ���
#define TBA_BUS_SEND_BUF	uart1_write_buf		//ͨ�Ŷ˿ڷ���
#define TBA_BUS_SEND_CHAR	uart1_write_char	//ͨ�Ŷ˿ڷ���
#define TBA_BUS_FIFO_BAKUP		uart1_fifo_out_backup
#define TBA_BUS_FIFO_RESUME		uart1_fifo_out_resume



//ͨ��Э�������ַ�
#define TBA_PACKET_TAB			0x7E	//���ݰ��ָ��
#define TBA_PACKET_DLE			0x7F	//���ݰ�ȫ��ת���
#define TBA_PACKET_DLE_TAB		0x80	//���ݰ��ָ����ת���
#define TBA_PACKET_DLE_DLE		0x81	//���ݰ�ת�����ת���

//������������
#define TBA_CMD_DEFAULTPACKET		0x00	//�������ݰ�
#define TBA_CMD_P2PPACKET		0x01	//��Ե�����
#define TBA_CMD_TOKENPACKET		0x02	//���ư�
#define TBA_CMD_TOKENCLEAR		0x03	//����������Ƶİ�
#define TBA_CMD_DOWNLOAD		0x04	//���س���


//���������ȶ���--���ݾ���Ӧ�øı�
#define TBA_BUF_SIZE		512

#define BUS_INTERVAL_TIME  30   //���ߵ�ƽ��ʱ��
#define TOKEN_INTERVAL_TIME  250   //���ߵ�ƽ��ʱ��
#define PROCESS_TO_TOKEN_TIME 60 //���͹������ݵ�Ѳ����ʱ��


//�г��������ݰ��ṹ
__packed typedef struct
{
	uint8 dst_net_id;
	uint8 dst_dev_id;
	uint8 src_net_id;
	uint8 src_dev_id;
	//�г����������ֽṹ---��λ��ǰ
	__packed struct
	{
		uint8 packet_type:4;//������
		uint8 token_free:1;//���ͷ��ͷ����Ƶı�־
		uint8 m_reserve:3;//����
	}bus_cmd;
	uint8 app_length;
	uint8 app_data[TBA_BUF_SIZE];
}tba_packet_t;



//�����豸��
typedef struct
{
	void (*init)(uint8, uint8);
	void (*proc)(void);
	uint8 (*send_data)(uint8, uint8, uint8, uint8, uint8 *, uint8);
	void (*set_debug)(uint8);
	uint8 data_flag;//0:������������   1:������û������
	uint32 bus_timer;
	uint8 dcp_volume_set;
	uint8 tba_send;
	//uint8 data_flag_key;//0:������������   1:������û������
	//uint8 data_flag_reconn;//0:������������   1:������û������
}tba_bus_dev_t;
extern tba_bus_dev_t sh9_tba_bus_dev;



#endif



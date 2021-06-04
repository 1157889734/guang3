//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �г�ͨ�����߹����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:tba_m.h

//*�ļ�����:�г�ͨ�����߹�����

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TBA_M_H
#define TBA_M_H

#define pisc_token_num  		2      // Ѳ��pisc ����
#define dcp_token_num  		2		 // Ѳ��dcp ����
#define lcu_token_num   		6		 // Ѳ��lcu ����
#define io_token_num    			6		 // Ѳ��io ����


#define TBA_M_ACK_TIMEOUT	350	//Ѳ��Ӧ��ʱ
//modify by hk   100-->200
#define TBA_M_WARNING_TIMEOUT	200	//Ѳ��Ӧ��ʱ����
#define TBA_M_TABLE_NUM		32	//Ѳ�������


//�ӿں���
extern void tba_m_init(uint8 net_id, uint8 dev_id);
extern void tba_m_proc(tba_packet_t *packet);
extern void tba_m_set_table(uint8 *dev_id, uint16 *interval);
extern uint8 tba_m_get_quality(uint8 *dev_id, uint16 *total, uint16 *error);
extern void tba_m_send_token(uint8 dev_id);

extern uint8 send_token_cnt;
//����Ѳ���ṹ
__packed typedef struct
{
	uint8 dev_id;//�豸��
	uint16 interval;//Ѳ���� 0xFFFF--��������Ѳ��  0--��������Ѳ��
	uint32 timer;//Ѳ������ʱ��
	uint16 poll_total;//Ѳ������---ͨ��������ĸ
	uint16 poll_error;//�����Ѳ������---ͨ����������
	uint8 poll_continue_timeout;//������ʱ��Ѳ������ �������Ѳ�쳬ʱ ��������豸�����ȼ�
      uint32 time_out;

	uint8 poll_flag; //  1:�ѷ���Ѳ������
	uint8 poll_interval_time;   //Ѳ��������   �ڴ˴�����û�յ��ظ����ط�����
}token_polling_table_t; 

extern token_polling_table_t token_polling_pisc_table[pisc_token_num];
extern token_polling_table_t token_polling_dcp_table[dcp_token_num];
extern token_polling_table_t token_polling_lcu_table[lcu_token_num];
extern token_polling_table_t token_polling_io_table[io_token_num];

#endif



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ���ƺ� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:dcp.h

//*�ļ�����:���ƺ�

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DCP_H
#define DCP_H



#define TBA_DEV_DCP	0x02	//���ƺ�
#define TBA_DCP_NUM	0x02	//���ƺ�����

//���ƺй������ݽṹ
__packed typedef struct
{
	uint8 start_station;// 1��ʼվ
	uint8 end_station;// 2�յ�վ
	uint8 current_station;// 3��ǰվ
	uint8 next_station;// 4��һվ
	uint8 jump_broadcast_no;// 5Խվ�㲥����
	uint8 urgent_broadcast_no;// 6�����㲥����
	//�㲥����λ
	__packed struct
	{
		uint8 leave_broadcast:1;   //Ԥ���㲥����
		uint8 arrived_broadcast:1;  //��վ�㲥����
		uint8 jump_broadcast:1;     //Խվ�㲥����
		uint8 urgent_broadcast:1;   //�����㲥����
		uint8 broadcast_req:1;       //�ڲ�����
		uint8 broadcasting:1;         //���ڿڲ�
		uint8 mute_broadcast:1;   //����
		uint8 stop_broadcast:1;     //ֹͣ��վ�㲥
	}broadcast_hbit; //7
	//״̬λ
	__packed struct
	{
		uint8 talking:2;  //˾���Խ�״̬1,˾���Խ�״̬2
		uint8 link:1;    //����
		uint8 em_talk:1; //�����Խ�
		uint8 run_dir:1;  //������
		uint8 test_state:1;//����״̬
		uint8 force_state:1;  //ǿ��״̬
		uint8 active:1;     //����״̬
	}status_hbit; //8
	
	uint8 icc_status[8]; //9-16 32��������״̬��
	uint8 jump_stations;//17Խվ����
	uint8 dcp_pacc_listen; //18   0-4 pa����  5-7 cc����
	uint8 dcp_pcpa_listen; //19  0-4 pc���� 5-7  pa mic
	uint8 dcp_ccpc_mic; //20     0-4 cc mic         5-7  pc mic

	__packed struct
	{
		uint8 pisc_mode:2;
		uint8 city_id:2;
		uint8 moni_broadcast:4;  //�㲥�м�����������
	}line_mode;   //21

	uint8 version_h;//22

	uint8 version_l;//23
		
}dcp_default_data_t;
extern dcp_default_data_t dcp_default_data[];


//ͨ�Ŵ���
extern uint16 dcp_comm_error[];



//�ӿں���
extern void dcp_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern uint8 dcp_get_media_on(void);
extern uint8 dcp_get_broadcast_req(void);
extern uint8 dcp_get_broadcast_state(void);
extern uint8 dcp_get_talk_state(void);
extern uint8 dcp_get_emtalking_state(void);
extern uint8 dcp_get_talk_req(void);
extern uint8 dcp_get_media_mode(void);

extern void  dcp_record(uint8 dev_no);

#endif



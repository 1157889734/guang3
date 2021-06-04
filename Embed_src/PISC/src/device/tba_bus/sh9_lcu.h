//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ���ҿ����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:lcu.h

//*�ļ�����:���ҿ�����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef LCU_H
#define LCU_H



#define TBA_DEV_LCU		0x03	//���ҿ�����
#define TBA_LCU_NUM		6	//���ҿ���������



//����״̬
#define TBA_LCU_PECU_SHUT	0x00		//�Ҷ�
#define TBA_LCU_PECU_CALL	0x01		//����
#define TBA_LCU_PECU_CONNECT	0x02		//����



//4��������״̬
__packed typedef struct
{
	uint8 pecu4:2;
	uint8 pecu3:2;
	uint8 pecu2:2;
	uint8 pecu1:2;
}icc_status_t;
	

//���ҿ������������ݽṹ
__packed typedef struct
{
	//4��������״̬
	icc_status_t icc_status;// 1
	
	//���ұ�����������
	__packed struct
	{
		uint8 sapu_in_position:1;//sapu��λ
		uint8 sapu_comm:1;//sapuͨ��
		uint8 reserve:2;//����		
		uint8 pecu4:1;//pecu4����
		uint8 pecu3:1;//pecu3����
		uint8 pecu2:1;//pecu2����
		uint8 pecu1:1;//pecu1����
	}icc_error; // 2
	
	//��������������
	__packed struct
	{
		uint8 amp1:1;
		uint8 amp2:1;
		uint8 spk1:1;
		uint8 spk2:1;
		uint8 reserve:4;
	}amp_spk_error;// 3
	
	//led������
	__packed struct
	{
		uint8 interior_led1:1;
		uint8 interior_led2:1;
		uint8 side_led1:1;
		uint8 side_led2:1;
		uint8 reserve:4;
	}led_error; // 4
	
	//����̬��ͼ����
	__packed struct
	{
	      uint8 dmp9:1;
		uint8 dmp10:1;
		uint8 reserve:6;  // ���ؿ����� ���
		
		uint8 dmp1:1;
		uint8 dmp2:1;
		uint8 dmp3:1;
		uint8 dmp4:1;
		uint8 dmp5:1;
		uint8 dmp6:1;
		uint8 dmp7:1;
		uint8 dmp8:1;
		
	}dmp_error; // 5-6

	uint8 version_h; //7
	uint8 version_l; //8
	
}lcu_default_data_t;
extern lcu_default_data_t lcu_default_data[];


//8�����ҿ�����ͨ�Ŵ���
extern uint16 lcu_comm_error[];



//�ӿں���
extern void lcu_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern void lcu_get_icc_state(icc_status_t *status);
extern uint8 lcu_get_icc_connect(void);



#endif



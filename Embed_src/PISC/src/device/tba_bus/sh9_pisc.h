//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��������� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:pisc.h

//*�ļ�����:���������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PISC_H
#define PISC_H



#define TBA_DEV_PISC	0x01	//���������
#define TBA_PISC_NUM	0x02	//�������������

#define COMM_PISC_TIMEOUT 20000

//����������������ݽṹ
__packed typedef struct
{
	//������־---��λ��ǰ
	__packed struct
	{
		uint8 leave_broadcast:1;          //��վ�㲥��־λ
		uint8 arrived_broadcast:1;			//��վ�㲥��־λ
		uint8 jump:1;									//Խս��־λ
		uint8 urgent_broadcast:1;			//�����㲥��־λ
		uint8 closedoor:1;							//���ű�־λ
		uint8 opendoor:1;							//���ű�־λ
		uint8 leave:1;									//�г���վ��־λ
		uint8 reach_stop:1;						//�г���վ��־λ
	}trigger_hbit; // 1

	uint8 start_station;// 2 ��ʼվ
	uint8 end_station;// 3 �յ�վ
	uint8 current_station;// 4��ǰվ
	uint8 next_station;// 5��һվ
	uint8 jump_broadcast_no;// 6Խվ�㲥����
	uint8 urgent_broadcast_no;// 7�����㲥����
	
	//״̬��־---��λ��ǰ
	__packed struct
	{
		uint8 jump_valid:1;
		uint8 active:1;
		uint8 master:1;
		uint8 dir_valid:1;
		uint8 door_led_sides:2;
		uint8 dir_down:1;
		uint8 dir_up:1;
	}status_hbit;  //8
	
	uint8 city_id;  // 9���к�
	uint8 year; //10
	uint8 month;  // 11
	uint8 date; // 12
	
	
	//�㲥���ȼ�
	__packed struct
	{
		uint32 pecu:4;
		uint32 talk:4;
		uint32 manual:4;
		uint32 occ:4;
		uint32 media:4;
		uint32 dva:4;
		uint32 door:4;
		uint32 special:4;
	}broadcast_priority; // 13-16
	
	
	//�㲥��Դ---��λ��ǰ
	__packed struct
	{
		uint8 broadcast_req:1;
		uint8 broadcasting:1;
		uint8 broadcast_type:6;
	}broadcast_signal; // 17

	//32��������״̬
	icc_status_t icc_status[8]; // 18-25
	
	//ý����ʾģʽ
	//uint8 media_mode;
	uint8 pis_version;  // 26    7            6      5   4      3   2   1   0   //  ����: 0x12   pis1����汾 1.2 ; 0x92  pis2����汾 1.2
					//  0:pis1 1:pis2   version_high    version_low          
					  
	//˾����״̬
	__packed struct
	{
		uint8 media_error:1;
		uint8 dva_error:1;
		uint8 apu_error:1;
		uint8 record_error:1;
		uint8 reserve:2;
		uint8 drmd_welcome:1;  //��̬��ͼ��ӭ��ʾ��־λ
		uint8 pis_mode:1;		
	}cab_status; //27
	
	//TMS״̬
	__packed struct
	{
		uint8 tms_valid:1;
		uint8 tms_active:1;
		uint8 lamp_test:1;
		uint8 atc_valid:1;
		uint8 map_display:1;  // 1:�رն�̬��ͼ
		uint8 reserve:1;    // Operation in HMI : =1 manual mode (TCMS) =0 auto mode.(ATC, default)
		uint8 tms_mode:2;
	}tms_status;//28
	
	uint8 dcp_volume;// 29    �㲥����������
	uint8 hour;//30
	//uint8 next_pass_station;//��һ��������վ--��һ��ͣ������һ������
	uint8 minute;//31
	
	__packed struct
	{
		uint8 drmd_test:1;  //��̬��ͼ����
		uint8 broadcast_test:1;
		uint8 master_req:1;
		uint8 slave_req:1;
		uint8 switch_master:1;   //��������ť
		uint8 reconnection:1;  //������ʶ   1:����
		uint8 third_distance:1;   // ����֮����
		uint8 two_part_distance:1; //����֮��
	}test_status; //32

	__packed struct
	{
		uint8 status1;    
		uint8 status2;
		uint8 status3;
	}error_data; //33 - 35	
}pisc_default_data_t;
extern pisc_default_data_t other_default_data;


//ͨ�Ŵ���
extern uint16 pisc_comm_error[];



//�ӿں���
extern void pisc_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);
extern uint8 read_pisc_id(void);


#endif



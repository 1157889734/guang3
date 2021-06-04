//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �г��ܿ����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:tms.h

//*�ļ�����:�г��ܿ�����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TMS_H
#define TMS_H



//�Ƿ���ģʽ
#define BIG_ENDIAN


/*
//Ӳ������
#define TMS_0KM_IN_REG			GPIO0_PD
#define TMS_0KM_IN_BIT			(7)
#define TMS_DOOR1_IN_REG		GPIO2_PD
#define TMS_DOOR1_IN_BIT		(2)
#define TMS_DOOR2_IN_REG		GPIO0_PD
#define TMS_DOOR2_IN_BIT		(6)

*/



//TMSͨ��ָʾ�ƺ�
#define TMS_LED_NO		2
	
	

//��������˿ڶ���--����Ӳ���ӿڸı�
#define TMS_BAUDRATE		19200			//������

#define TMS_PORTINIT		uart3_init		//ͨ�Ŷ˿ڳ�ʼ��
#define TMS_RECEIVE_CHAR	uart3_read_char		//ͨ�Ŷ˿ڽ���
#define TMS_SEND_BUF		uart3_write_buf		//ͨ�Ŷ˿ڷ��� 
#define TMS_SEND_CHAR		uart3_write_char	      //ͨ�Ŷ˿ڷ���
#define TMS_FIFO_BAKUP		uart3_fifo_out_backup
#define TMS_FIFO_RESUME	uart3_fifo_out_resume

//���������ȶ���--���ݾ���Ӧ�øı�
#define TMS_BUF_SIZE		512

#define LINE_ID 5   //5����
#define FROM_STATION_ID 1      //վ�����1��27
#define TO_STATION_ID      27

#define ATC_UP_FROM_STATION_ID  114
#define ATC_UP_TO_STATION_ID       140
#define ATC_DN_FROM_STATION_ID  87
#define ATC_DN_TO_STATION_ID       113

//�г�����״̬
#define BUS_STATUS_ERROR	0
#define BUS_STATUS_MASTER	1
#define BUS_STATUS_SLAVE	2
#define BUS_STATUS_PARTLY	3

//�������������ģʽ
#define WORK_AT_TCMS					0x00         //TCMS HMI
#define WORK_AT_AUTO					0x01         //ATO ģʽ
//#define WORK_AT_MANU					0x02         //�ֶ����Թ㲥���ƺ�

//�г��ܿ������������ݰ��ṹ
__packed typedef struct
{
	uint8 fram_begin;   // 1 0xFE

	/*���ݲ���*/

	/*Port 437  4 Bytes*/
	uint8 port437_reserves[3]; // 2-4
	__packed struct
	{
		uint8 iocmb_comm_error:1;
		uint8 pacub_comm_error:1;
		uint8 picu1b_comm_error:1;
		uint8 picu2b_comm_error:1;
		uint8 picu3b_comm_error:1;
		uint8 pis_a1_diagnostic_reverse:3;
	}pis_a1_diagnostic;  // 5

	/*Port 438  16 Bytes*/
	__packed struct
	{
		uint8 self_test_active:1;
		uint8 chinese_disable:1;
		uint8 english_disable:1;
		uint8 cantonese_disable:1;
		uint8 transfer_disable:1;
		uint8 led_map_disable:1;
		uint8 bit6:1;
		uint8 TDACmaster:1;
		
		uint8 TDACslave:1;
		uint8 bit9:1;
		uint8 broken_selftest:1;
		uint8 sucessful_selftest:1;
		uint8 bit12_15:4;		
	}pis_a1_lw11;  //6-7

	__packed struct
	{
		uint8 pa_active:1;
		uint8 dacu_active_pa:1;
		uint8 radio_broadcast:1;
		uint8 dacu_call_dacu:1;
		uint8 driver_to_pa:1;
		uint8 bit5_7:3;

		uint8 bit8_15:8;		
	}pis_a2_lw12; //8-9

	uint8 start_station; 	//10
	uint8 end_station;  	//11
	uint8 current_station;		//12
	uint8 pis_a1_routenumber;		//13
	uint8 port438_reverses[8];  		//14-21

	/*����� 4 Bytes */
	uint32 fill_area; 		//22-25

	/*Port 439  8  Bytes*/
	__packed struct
	{
		uint8 call_picu1_a1:1;
		uint8 active_picu1_a1:1;
		uint8 call_picu2_a1:1;
		uint8 active_picu2_a1:1;
		uint8 call_picu3_a1:1;
		uint8 active_picu3_a1:1;
		uint8 bit6:1;
		uint8 call_picu1_b:1;
		
		uint8 active_picu1_b:1;
		uint8 call_picu2_b:1;
		uint8 active_picu2_b:1;
		uint8 call_picu3_b:1;
		uint8 active_picu3_b:1;
		uint8 bit13_15:3;
	}pis_a1_lw21;    //26-27

	__packed struct
	{
		uint8  active_picu1_a2:1;
		uint8  res_picu1_a2:1;
		uint8  active_picu2_a2:1;
		uint8  res_picu2_a2:1;
		uint8  active_picu3_a2:1;
		uint8  res_picu3_a2:1;
		uint8  bit6_7:2;

		uint8  bit8_15:8;
	}pis_a1_lw22;  //28-29

	__packed struct
	{
		uint8  active_picu1_a1:1;
		uint8  res_picu1_a1:1;
		uint8  active_picu2_a1:1;
		uint8  res_picu2_a1:1;
		uint8  active_picu3_a1:1;
		uint8  res_picu3_a1:1;
		uint8  active_picu1_b:1;
		uint8  res_picu1_b:1;
		
		uint8  active_picu2_b:1;
		uint8  res_picu2_b:1;
		uint8  active_picu3_b:1;
		uint8  res_picu3_b:1;
		uint8  bit12_15:4;
	}pis_a1_lw23;  //30-31

	uint8 port439_reverses[2];		//32-33

	/*Port 440  32  Bytes*/
	
	uint16 sw_version_tdac;		//34-35
	uint16 sw_version_acsu;		//36-37
	uint16 sw_version_pacu;		//38-39
	uint16 sw_version_pacub;		//40-41
	uint16 sw_version_audio_db;		//42-43
	uint16 sw_version_station_db;		//44-45
	uint16 version_nsdb;		//46-47

	uint8 port440_reverses[18];		//48-65

	uint8  	checksum;    //  66
	uint8 fram_end;   	// 67  0xFF	
}
tms_send_packet_t;



//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	uint8 (*get_special_broadcast)(void);
	uint8 (*get_door_broadcast)(void);
	uint8 (*get_station_broadcast)(void);
	void (*set_pecu)(icc_status_t *);
      void (*set_broadcast)(uint8);
	void (*set_special_broadcast)(uint8);
	//void(*init_station)(void);
	
	//uint8 (*drm_test)(void);
	uint16 comm_error;//����ͨ�Ŵ���
	uint16 special_broadcast_code;
	uint8 ato_valid;//�Զ��г���Ч
	uint8 start_station;
	uint8 end_station;
	uint8 current_station;
	uint8 next_station; // ��һ��ͣ��վ
	
	uint8 arrived;
	uint8 open_door;
	uint8 door_sides; //10 ���  01 �Ҳ�11 ����
	uint8 run_dir;
	uint8 run_distance_percent;//���еľ���--�ٷֱ�
	uint8 next_pass_station;//��һ��������վ--��һ��ͣ������һ������
	uint8 drm_test;
	//uint8 broadcast_test;

	uint8 pis_work_mode;     // 1:ATC  0:TCMS  2:manual

	uint8 emergency_status; // 1: ֹͣ�����㲥
	uint16 city_id;              //����
	uint16 route_id;/* 
	1:�����ϵ�����;  2.������������   3:��͵���خ  4:��خ�����
	5:ͬ�͵���ʯ     6.��ʯ��ͬ��     7:ͬ�͵�����  8:������ͬ��  
	9: ��������خ  10. ��خ������
*/
	uint8 month;  //�·�
	uint8 date;     //����
	uint8 year;
	uint8 hour;
	uint8 minute;
	
	uint8 debug_flag;//���Ա�־

	uint8 english_enable;
	uint8 chinese_enable;
	uint8 cantonese_enable;  //����
	uint8 transfer_announce_enable;
	uint8 line_map_display_enable;

	//��TMS���͵����ݰ�
	tms_send_packet_t tms_send_packet;
}tms_dev_t;
extern tms_dev_t sh9_tms_dev;

extern uint16 next_station_code; 
extern uint16 end_station_code; 

#endif



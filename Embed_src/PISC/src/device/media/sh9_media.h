//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��ý�岥���� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:media.h

//*�ļ�����:��ý�岥����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef MEDIA_H
#define MEDIA_H



//Ӳ������
#define MEDIA_PLAY_IN_REG		GPIO2_PD
#define MEDIA_PLAY_IN_BIT		(6)



//ý�岥��ͨ��ָʾ�ƺ�
#define MEDIA_LED_NO		5


//��������˿ڶ���--����Ӳ���ӿڸı�
#define MEDIA_BAUDRATE		9600			//������
#define MEDIA_PORTINIT		uart0_init		//ͨ�Ŷ˿ڳ�ʼ��
#define MEDIA_RECEIVE_CHAR	uart0_read_char		//ͨ�Ŷ˿ڽ���
#define MEDIA_SEND_BUF		uart0_write_buf		//ͨ�Ŷ˿ڷ���
#define MEDIA_SEND_CHAR		uart0_write_char	//ͨ�Ŷ˿ڷ���
#define MEDIA_FIFO_BACKUP	uart0_fifo_out_backup	//�ײ�ָ�뱣��
#define MEDIA_FIFO_RESUME	uart0_fifo_out_resume	//�ײ�ָ��ض�



//���������ȶ���--���ݾ���Ӧ�øı�
#define MEDIA_BUF_SIZE		255


typedef struct
{
    __packed struct
	{
		uint8 cctv_encode_err:1;// ˾���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_encode_err:1;// ý������
		uint8 reserve1:4;// 
	}cab1; //˾����1
	__packed struct
	{
		uint8 cctv_encode_err:1;// ˾���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_encode_err:1;// ý������
		uint8 reserve1:4;// 
	}cab2; //˾����1
	__packed struct
	{
		uint8 cctv_encode_err:1;// ���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_decode_err:1;// ý������
		uint8 reserve1:4;// 
	}car1; //����1
	__packed struct
	{
		uint8 cctv_encode_err:1;// ���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_decode_err:1;// ý������
		uint8 reserve1:4;// 
	}car2; //����2
	__packed struct
	{
		uint8 cctv_encode_err:1;// ���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_decode_err:1;// ý������
		uint8 reserve1:4;// 
	}car3; //����3
	__packed struct
	{
		uint8 cctv_encode_err:1;// ���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_decode_err:1;// ý������
		uint8 reserve1:4;// 
	}car4; //����4
	__packed struct
	{
		uint8 cctv_encode_err:1;// ���Ҽ�ر����
		uint8 camera1:1;//����ͷ1
		uint8 camera2:1;// ����ͷ1
		uint8 media_decode_err:1;// ý������
		uint8 reserve1:4;// 
	}car5; //����5
	uint8 reserve[2];
}device_state_t;

//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	void (*send_data)();
	uint8 (*get_playing)(void);
	
	uint16 comm_error;//����ͨ�Ŵ���
	
	device_state_t device_state;
	
}media_dev_t;
extern media_dev_t sh9_media_dev;

typedef struct
{
    __packed struct
	{
		uint8 emergency:1;// 1:���ڲ��Ž����㲥
		uint8 door_sides:2; //10 ���  01 �Ҳ�11 ����
		//uint8 open_right_door:1;// ������
		//uint8 open_left_door:1;// ������
		uint8 up_dir:1;// ����
		uint8 down_dir:1;// ����
		uint8 close_delay:1;//  �ӳٹػ�
		uint8 leave_broadcast:1;//  ��վ 
		uint8 arrived_broadcast:1;
	}status;
	__packed struct
	{
		uint8 master_slave:1;//  1: master  0:slave
		uint8 key:1;//Կ��
		uint8 reserve:1;
	}cab;
	uint8 start_station_id;
	uint8 destination_station_id;
	uint8 current_station_id;
	uint8 next_station_id;   //6
	uint8 emergency_id;
	uint8 city_id;
	uint8 line_id;
	uint8 velocity_h; //10
	uint8 velocity_l;
	
	uint8 year;
	uint8 month;
	uint8 date;
	uint8 hour;
	uint8 minute;  //16
	uint8 second;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}tc1;
	*/
	uint8 tc1_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}mp1;
	*/
	uint8 mp1_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}m;
	*/
	uint8 m_doors_unlock;  //20
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}mp2;                   
	*/
	uint8 mp2_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}tc2;
	*/
	uint8 tc2_doors_unlock;
      uint8 reserve_for_door;   //23
	__packed struct
	{
		uint8 dva:1; 
		uint8 mvb:1; 
		uint8 atc:1; 
		uint8 fdu:1; //�յ�վ�� 
		uint8 fire_alarm_tc1_cab:1;
		uint8 apu:1;
		uint8 dcp:1;
		uint8 reserve:1;
	}pcu1;
	__packed struct
	{
		uint8 dva:1; 
		uint8 mvb:1; 
		uint8 atc:1; 
		uint8 fdu:1; //�յ�վ�� 
		uint8 fire_alarm_tc2_cab:1;
		uint8 apu:1;
		uint8 dcp:1;
		uint8 reserve:1;
	}pcu2;                        //25
	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//�Խ������� 
		uint8 amp1:1; //����
		uint8 amp2:1; //�յ�վ�� 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_tc1_saloon:1;
		uint8 reserve:1;
	}scu1_1;
	__packed struct
	{
		uint8 idu1:1; //�ڲ���
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //�ಿ��
		uint8 reserve:4; 
	}scu1_2;                 //27
	__packed struct
	{
		uint8 drmd1:1;  //��̬��ͼ 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu1_3;
	uint8 reserve_1;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//�Խ������� 
		uint8 amp1:1; //����
		uint8 amp2:1; //�յ�վ�� 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_mp1_saloon:1;
		uint8 reserve:1;
	}scu2_1;
	__packed struct
	{
		uint8 idu1:1; //�ڲ���
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //�ಿ��
		uint8 reserve:4; 
	}scu2_2;              //31
	__packed struct
	{
		uint8 drmd1:1;  //��̬��ͼ 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu2_3;
	uint8 reserve_2;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//�Խ������� 
		uint8 amp1:1; //����
		uint8 amp2:1; //�յ�վ�� 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_m_saloon:1;
		uint8 reserve:1;
	}scu3_1;
	__packed struct
	{
		uint8 idu1:1; //�ڲ���
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //�ಿ��
		uint8 reserve:4; 
	}scu3_2;
	__packed struct
	{
		uint8 drmd1:1;  //��̬��ͼ 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu3_3;            //36
	uint8 reserve_3;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//�Խ������� 
		uint8 amp1:1; //����
		uint8 amp2:1; //�յ�վ�� 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_mp2_saloon:1;
		uint8 reserve:1;
	}scu4_1;
	__packed struct
	{
		uint8 idu1:1; //�ڲ���
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //�ಿ��
		uint8 reserve:4; 
	}scu4_2;                             //39
	__packed struct
	{
		uint8 drmd1:1;  //��̬��ͼ 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu4_3;
	uint8 reserve_4;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//�Խ������� 
		uint8 amp1:1; //����
		uint8 amp2:1; //�յ�վ�� 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_tc2_saloon:1;
		uint8 reserve:1;
	}scu5_1;
	__packed struct
	{
		uint8 idu1:1; //�ڲ���
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //�ಿ��
		uint8 reserve:4; 
	}scu5_2;                          //43
	__packed struct
	{
		uint8 drmd1:1;  //��̬��ͼ 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu5_3;
	uint8 reserve_5;
	uint8 reverse[2];
}media_send_packet_t;

extern media_send_packet_t media_send_packet;
#endif



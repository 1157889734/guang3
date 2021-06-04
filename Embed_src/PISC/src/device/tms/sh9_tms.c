/******************************************************************************
*�ļ����ƣ�sh9_tms.c		       		         
*��          �ߣ�hk           							 
*��          ����TCMS ���ݿ��ƴ���
*�������ڣ�2010-3-11
*�޸ļ�¼����								
******************************************************************************/


#include "include.h"

//ͨ��Э��ؼ���
#define TMS_PACKET_HEAD	      0x7E
#define TMS_PACKET_END	             0x0D
#define TMS_PACKET_TAB			0x7E	//���ݰ��ָ��
#define TMS_PACKET_DLE			0x7F	//���ݰ�ȫ��ת���
#define TMS_PACKET_DLE_TAB		0x80	//���ݰ��ָ����ת���
#define TMS_PACKET_DLE_DLE		0x81	//���ݰ�ת�����ת���
#define TMS_PACKET_LENGTH		(153)//(81)


#define LEAVE_STATION_TRIGGER_DISTANCE		300    //��վ�˾��봥��Ԥ��վ�㲥
#define ARRIVE_STATION_TRIGGER_DISTANCE	       100    //��վ�˾��봥����վ�㲥
#define EMERGENCY_PA_ID_MAX		16

#define LEAVE_STATION_PA_TRIGGER		(1 << 0)    //��վ����
#define PRE_ARRIVE_STATION_PA_TRIGGER	(1 << 1)   //Ԥ��վ����
#define SKIP_STATION_TRIGGER			(1 << 2)   //Խվ����
#define EMERGENCY_PA_TRIGGER			(1 << 3)   //��������
#define CLOSE_DOOR_TRIGGER				(1 << 4)
#define OPEN_DOOR_TRIGGER				(1 << 5)
#define LEAVE_STATION_TRIGGER			(1 << 6)
#define STOP_STATION_TRIGGER			(1 << 7)


#define PRE_ARRIVAL    1   //�г���Ҫ���ﳵվ���г���ͷ��վǰ20S��
#define DEPARTURE       8  //�г���վ
#define ARRIVAL           2   //��վ

#define LEAVE_DELAY    0   //��վ�㲥��ʱ

#define close_mathod  0   // = 0 ����ȡopenλ �� =1 ����ȡclose λ

//���ջ�����
static uint8 xdata tms_recv_buf[TMS_BUF_SIZE];

static uint16 xdata tms_recv_length=0;

//��ʱ��TMS�������ݼ�ʱ��
static uint32 tms_timer=0;

//static uint8 tms_get_flag=0;

//�г��ܿ������������ݰ��ṹ  from tcms
__packed typedef struct
{
	uint8 fram_begin;  		// 1   0xFE

	/* ���ݲ���*/

	/* Port 374   16 Bytes*/
	__packed struct
	{
		uint8 oknumbercarA:1;
		uint8 okloadweightA1:1;
		uint8 okdiameterwheelA1:1;
		uint8 ac_ventilation1_onA1:1;
		uint8 ac_ventilation2_onA1:1;
		uint8 va_conallA1:1;
		uint8 va_con1A1:1;
		uint8 release_compr1startA1:1;

		uint8 release_compr2startA1:1;
		uint8 start_selftest_ep_brakeA1:1;
		uint8 lwca1_10:1;
		uint8 lwca1_11:1;
		uint8 lwca1_12:1;
		uint8 lwca1_13:1;
		uint8 lwca1_14:1;
		uint8 lwca1_15:1;	
	}lwca;		// 2-3

	uint8 port374_reserves1[2]; // 4-5
	uint32 numvercarA; 				// 6-9
	uint8 port374_reserves2[8]; //10-17    suven MVB����8���ֽ�

	/*Port 393   32 Bytes*/
	uint8 port_time1;  		// 18
	uint8 port_time2;			//19
	uint8 port_time3;			//20	
	uint8 port_time4;			//21
	uint8 port_time5;			//22
	uint8 port_time6;			//23

	__packed struct
	{
		uint8 oktimedate:1;
		uint8 restricted_drive:1;
		uint8 activation_on_gw:1;
		uint8 cab_manned_A1:1;
		uint8 cab_manned_A2:1;
		uint8 lwg1_5:1;
		uint8 lwg1_6:1;
		uint8 lwg1_7:1;
		
		uint8 lwg1_8:1;
		uint8 lwg1_9:1;
		uint8 lwg1_10:1;
		uint8 lwg1_11:1;
		uint8 lwg1_12:1;
		uint8 lwg1_13:1;
		uint8 lwg1_14:1;
		uint8 lwg1_15:1;		
	}lwg1;//24-25
	
	uint8 port393_reserves[24]; //26-49

	/*Port 481  16 Bytes*/
	__packed struct
	{
		uint8 atc_a1_reserves2:8;

		uint8 ato_mode:1;
		uint8 atp_reverse:1;
		uint8 leftdoor_open:1;
		uint8 rightdoor_open:1;
		uint8 ato_depart:1;
		uint8 atc_a1_reserves1:3;
	}atc_a1;  //50-51

	uint16 atc_a1_code_id;		//52-53
	uint16 atc_a1_platform_id; 	//54-55
	uint8 port481_reserves[10]; //56-65

	/*Port 652   32 Bytes*/
	__packed struct
	{
		uint8 haul_drive_active:1;
		uint8 skip_station:1;
		uint8 door_open:1;
		uint8 lwp1_11:1;
		uint8 manual_leftdoor_open:1;
		uint8 manual_rightdoor_open:1;
		uint8 manual_nextstation:1;
		uint8 manual_docked_station:1;
	
		uint8 oknumbercarB:1;
		uint8 self_test:1;
		uint8 cab_a1_manned:1;
		uint8 cab_a2_manned:1;
		uint8 train_manned:1;
		uint8 mechanical_coupler_active:1;
		uint8 electr_coupler_a1_active:1;
		uint8 electr_coupler_a2_active:1;		
	}lwp1;   //66-67

	__packed struct
	{
		uint8 lwp2_8:1;
		uint8 lwp2_9:1;
		uint8 lwp2_10:1;
		uint8 lwp2_11:1;
		uint8 lwp2_12:1;
		uint8 lwp2_13:1;
		uint8 lwp2_14:1;
		uint8 lwp2_15:1;
	
		uint8 chinese_disable:1;
		uint8 english_disable:1;
		uint8 cantonese_disable:1;
		uint8 transfer_announce_disable:1;
		uint8 led_map_disable:1;
		uint8 lwp2_5:1;
		uint8 lwp2_6:1;
		uint8 lwp2_7:1;				
	}lwp2;  //68-69

	uint8 start_station;   	//70
	uint8 end_station;  		//71
	uint8 next_station;		//72
	uint8 route_number;	//73
	uint32 numbercarB;		//74-77
	uint16 emc_annoucement;  //78-79
#if 1
	uint8 port652_reserves[18]; //80-97

	/*Port 653   4 Bytes*/
	__packed struct
	{
		uint8  call_picu1_a1:1;
		uint8  call_picu2_a1:1;
		uint8  call_picu3_a1:1;
		uint8  call_picu1_b:1;
		uint8  call_picu2_b:1;
		uint8  call_picu3_b:1;
		uint8  call_picu1_a2:1;
		uint8  call_picu2_a2:1;
		
		uint8  call_picu3_a2:1;
		uint8  lwp11_9:1;
		uint8  active_picu1_a1:1;
		uint8  res_picu1_a1:1;
		uint8  active_picu2_a1:1;
		uint8  res_picu2_a1:1;
		uint8  active_picu3_a1:1;
		uint8  res_picu3_a1:1;
	}lwp11; //98-99

	__packed struct
	{
		uint8  active_picu1_b:1;
		uint8  res_picu1_b:1;
		uint8  active_picu2_b:1;
		uint8  res_picu2_b:1;
		uint8  active_picu3_b:1;
		uint8  res_picu3_b:1;
		uint8  active_picu1_a2:1;
		uint8  res_picu1_a2:1;
		
		uint8  active_picu2_a2:1;
		uint8  res_picu2_a2:1;
		uint8  active_picu3_a2:1;
		uint8  res_picu3_a2:1;
		uint8  lwp12_12;
		uint8  lwp12_13;
		uint8  lwp12_14;
		uint8  lwp12_15;
	}lwp12; //100-101

	
	/*Port 696   32 Bytes*/
	uint8 port696_reserves1[10]; //102-111
	uint16 pis_a2_SWVersStationDB;  //112-113
	uint8 port696_reserves2[20]; //114-133

	/*Port 737   16 Bytes*/
	__packed struct
	{
		uint8 ato_mode:1;
		uint8 atp_reverse:1;
		uint8 leftdoor_open:1;
		uint8 rightdoor_open:1;
		uint8 ato_depart:1;
		uint8 atc_a2_reserves1:3;

		uint8 atc_a2_reserves2:8;
	}atc_a2;  //134-135

	uint16 atc_a2_code_id;			//136-137
	uint32 atc_a2_platform_id; 	//138-141
	uint8 port737_reserves[8]; 	//142-149

	/*Port 440  2 Bytes*/       
	uint16 pis_a1_SWVersStationDB; //150-151
#endif	
	uint8  	checksum;					//	 152     80
	uint8 	fram_end;   				//  153  0xFF       81
}tms_recv_packet_t;


//CRC-CCITT:(��ŷ��CCITT�Ƽ�)G(X) = X16 + X12 + X5 + 1
unsigned int ats_crc_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, //7
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, //15
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, //23
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, //31
    
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, //63
    
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, //95
    
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static tms_recv_packet_t * xdata tms_recv_packet=(tms_recv_packet_t *)&tms_recv_buf;//�������ݰ���ָ��

//˽�б���
static uint8 station_broadcast=0;//վ�㲥��־
static uint8 special_broadcast=0;//����㲥��־
static uint8 door_broadcast=0;//���Ź㲥

static uint8 tms_work_mode =0;  //tms �Ĺ���ģʽ
static uint8 manual_docked_station = 0 ; //���Զ���վ�ź�

static uint16 start_station_code=0; //ʼ��վ
//static uint16 pre_start_station_code=0;

uint16 end_station_code=0;   //�յ�վ
static uint8 start_chg_flag=0; 
static uint16 atc_a1_code_id = 0;
static uint8 atc_a1_code_id_cnt = 0;

uint16 next_station_code=0;  //��ǰ���ݰ��е���һվ
//static uint16 pre_next_station_code=0; 
//static uint8 manual_station_announcement=0;//�ֶ���վ

//static uint16 current_station_code=0;   //��ǰ��վ
//static uint16 pre_current_station_code=0;  

//static uint16 dis_from_next_station=0;   //����һվ����
//static uint16 dis_from_pre_station = 0;  //�뿪��һվ�ľ���
//static uint16 dis_from_next_station_tmp=0;   //����һվ����
//static uint16 dis_from_pre_station_tmp = 0;  //�뿪��һվ�ľ���
//static uint16 between_station_distance = 0;     //��վ֮��ľ���

//static uint16 DDC_skip_station_1=0;
//static uint16 DDC_skip_station_2=0;

//static uint8 left_door_open_real=0;  //�������ű�ʾ
//static uint8 right_door_open_real=0;


static uint8 ato_door_open=0;  //�������ű�ʾ

//static uint16   emergency_ID = 0;         //�����㲥
//static uint8    pre_emergency = 0;

static uint16 comm_error_tms_timer=0;

static uint8 flag=0;

static uint8 recv_next_flag = 0;  //���յ���һվ�����־��
 
//static uint8         door_status = 0; 
//static uint8      close_right_door_status = 0; 

//static uint8  open_left_door = 0;
//static uint8  open_right_door = 0;

//static uint16     velocity = 0;

//static uint32 timedate=0; //ʱ������

//CRC-CCITT:(��ŷ��CCITT�Ƽ�)G(X) = X16 + X12 + X5 + 1
unsigned int check_ccitt_crc(unsigned char* nData, unsigned int len)
{
    unsigned int crc = 0;
    unsigned char crc_H8;
    unsigned char *ptr = nData;

    while( len-- )
    {
        crc_H8 = (unsigned char)(crc >> 8);
        crc = crc << 8;
        crc = crc ^ ats_crc_table[crc_H8 ^ *ptr];
        ptr++;
    }
    return crc;
}
//***********************************************************************************************************************
//��������:�г��ܿ���������Ӧ��
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************

static void tms_send_ack(void)
{
	uint8 send_buf[128] = {0};
	//uint8 s_buf[128] = {0};
	//uint8 i=0;
	uint8 cnt=0;
	uint8 checksum=0;
	//uint8 *p=send_buf;
	//return;
	//ͷ
	#if 0
	sh9_tms_dev.tms_send_packet.fram_begin=0xFE;	
	//���ݲ���
	sh9_tms_dev.tms_send_packet.sw_version_tdac = 0x100;
	sh9_tms_dev.tms_send_packet.sw_version_acsu= 0x100;
	sh9_tms_dev.tms_send_packet.sw_version_pacu = 0x100;
	sh9_tms_dev.tms_send_packet.sw_version_pacub= 0x100;
	sh9_tms_dev.tms_send_packet.sw_version_audio_db= 0x100;
	sh9_tms_dev.tms_send_packet.sw_version_station_db= 0x100;
	sh9_tms_dev.tms_send_packet.version_nsdb = 0x103;

	sh9_tms_dev.tms_send_packet.fram_end=0xFF;

	memcpy(send_buf,(uint8 *)&(sh9_tms_dev.tms_send_packet),sizeof(tms_send_packet_t));
	#endif
	send_buf[0] = 0xFE ;
	send_buf[1] = my_default_data.error_data.status1;
	send_buf[2] = my_default_data.error_data.status2;
	send_buf[3] = my_default_data.error_data.status3;
	send_buf[5] = 0xFF ;
	
	//����У����
	checksum = send_buf[1];
	for(cnt= 2;cnt<4;cnt++)
	{
		checksum +=send_buf[cnt];
	}
	send_buf[4]=checksum;

	TMS_SEND_BUF(send_buf,6);

	//���͵����Զ˿�
	if(sh9_tms_dev.debug_flag == 4)
		ptu_print_buf((uint8 *)send_buf,6); 
		
	//���͵����Զ˿�
	if(sh9_tms_dev.debug_flag == 2||sh9_tms_dev.debug_flag == 3)
		uart0_write_buf((uint8 *)send_buf,6);
	
}


/****************************************************
*��������com_to_int16			       		         
*���ߣ�hk             							 
*�������Ѹ�8λ�͵�8λ�ϲ�Ϊ16����	
*���룺��										
*�������	
*����ֵ��
*�޸ļ�¼����								
****************************************************/

/*
static uint16 com_to_int16(uint8 high, uint8 low)
{
    uint16 temp = high;
    temp = ((temp<<8)&0xff00)|low;
    return temp;
}
*/

//***********************************************************************************************************************
//����:	suven
//��������:������ID ת��
//����˵��:������վ����
//ע������:��
//����˵��:ת�����ID
//***********************************************************************************************************************
static uint16 Station_code_to_ID (uint16 Station_code_ID)
{	
	uint16 code_id;

	code_id = Station_code_ID ;
	if(code_id == 15)															// 1.��خ�㳡�۷�
		code_id =1;		
	else if(code_id == 6 ||code_id == 8) 		// 4. ��ʯ�۷�
		code_id =4;
	else if(code_id == 45) 													// 8.�ʹ��۷�
		code_id =8;
	else if(code_id == 34 ||code_id == 37 	// 11.������·�۷�
	 	||code_id == 36 ||code_id == 38) 		// 11.������·M
		code_id =11;
	else if(code_id == 28) 													// 13.���ݶ�վ�۷�
		code_id =13;
	else if(code_id == 57 ||code_id == 59) 	// 17. ͬ���۷�
		code_id =17;
	else if(code_id == 82) 													// 21.����վ�۷�
		code_id =21;
	else if(code_id == 75) 													// 23.����վ�۷�
		code_id =23;
	else if(code_id == 71 ||code_id == 73) 	// 24.�������۷�
		code_id =24;
	else if(code_id == 67 ||code_id == 69) 	// 25. �������۷�
		code_id =25;
	else if(code_id == 16 ||code_id == 18) 	// 30. ��ӿ���վ�۷�
		code_id =30;	
	else
	{
		return 0;  //����������뷵��0
	}
	return code_id;
}

#if 1
//PIS����ATC���ݽṹ
#define PIS_RECV_BUFF_SIZE 1500 /*pis���ݻ����С*/
#define PIS_RECV_PBUFF g_recv_tms_buf  /*pis���ݻ���ָ��*/
#define PIS_FRAM_BEGIN 0xFE    /*pis���ݰ�ͷ*/
#define PIS_FRAM_END 0xFF      /*pis���ݰ�β*/
#define PIS_FRAM_LEN TMS_PACKET_LENGTH  /*pis���ݰ�����*/
#define PIS_DATA_BUFF tms_recv_buf  /*pis���ݰ�ָ��*/
#define PIS_READ_UART TMS_RECEIVE_CHAR /*����ѡ��*/
/*У��ͼ��㷽�������в�ͬ����Ҫ�޸�*/
typedef struct PIS_RECV_BUFF{
	//����
	uint16 len;
	//���տ�		
	uint16 in;
	//����
	uint16 out;
	//����buff
	uint8   buf[PIS_RECV_BUFF_SIZE];
}PIS_BUFF;
static PIS_BUFF g_recv_tms_buf=
{
0,
0,
0,
{0,},
};
//������pis���ݶ���g_recv_pis_buf
static uint8 read_pis_to_buf(void)
{
	uint8 temp;
	uint16 len=0;
	while(PIS_READ_UART(&temp))
	{
		PIS_RECV_PBUFF.buf[PIS_RECV_PBUFF.in++] = temp;
		PIS_RECV_PBUFF.in %= PIS_RECV_BUFF_SIZE;
		if(PIS_RECV_PBUFF.in==PIS_RECV_PBUFF.out)
		{
			PIS_RECV_PBUFF.out++;
		}
		else
		{
			PIS_RECV_PBUFF.len++;
		}
		PIS_RECV_PBUFF.out %= PIS_RECV_BUFF_SIZE;
		len++;
		if(len>=PIS_FRAM_LEN)
		{
			break;
		}
		
	}
	return 0;
}
//����g_recv_pis_buf �ĳ���
static uint16 get_pis_len(void)
{
	return PIS_RECV_PBUFF.len;
}
//��g_recv_pis_buf ����ָ���������ݵ�pbuf�����ض����ĳ���
static uint16 read_pis_buf(uint8 *pbuf,uint16 len)
{
	uint16 ret=0;
	uint16 i;
	if(PIS_RECV_PBUFF.len<len&&NULL==pbuf)
	{
		ret=0;
	}
	else
	{
		for(i=0;i<len;i++)
		{
			pbuf[i]=PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out+i)%PIS_RECV_BUFF_SIZE];
		}
		PIS_RECV_PBUFF.out = (PIS_RECV_PBUFF.out+len)%PIS_RECV_BUFF_SIZE;
		PIS_RECV_PBUFF.len -= len;
		ret=len;
	}
	return ret;
}
//��g_recv_pis_buf ɾ��len���ֽ�
static void delete_pis_buf(uint16 len)
{
       if(PIS_RECV_PBUFF.len<=len)
       {
       	PIS_RECV_PBUFF.in=0;
		PIS_RECV_PBUFF.out=0;
		PIS_RECV_PBUFF.len=0;
		return;
       }
	PIS_RECV_PBUFF.out=(PIS_RECV_PBUFF.out+len)%PIS_RECV_BUFF_SIZE;
	PIS_RECV_PBUFF.len -= len;
	return;
}
#if 1
static uint8 tms_get_packet(void)
{
	uint8 temp;	
	uint8 checksum=0;
	uint16 i;
	read_pis_to_buf();
	while(get_pis_len()>=PIS_FRAM_LEN)   //FE 00 23   
	{
		//���ݷָ���
		temp = PIS_RECV_PBUFF.buf[PIS_RECV_PBUFF.out];
		if(PIS_FRAM_BEGIN==temp&&PIS_FRAM_END==PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out+PIS_FRAM_LEN-1)%PIS_RECV_BUFF_SIZE])  
		{
			checksum = 0;
                     for(i=1;i<PIS_FRAM_LEN-2;i++)
                     {
                     	checksum += PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out+i)%PIS_RECV_BUFF_SIZE];
                     }
			if(checksum==PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out+PIS_FRAM_LEN-2)%PIS_RECV_BUFF_SIZE])
			{
				read_pis_buf(PIS_DATA_BUFF,PIS_FRAM_LEN);
				return 1;
			}
		}

		//��pis_recv_buf ɾ��һ���ֽ�
		delete_pis_buf(1);
	}		
	return 0;
}
#endif
#else
/****************************************************
*��������tms_get_packet			       		         
*���ߣ�hk             							 
*����������TMS����			
*���룺��										
*�������	
*����ֵ��1:�յ�������һ������  0 :failed
*�޸ļ�¼����								
****************************************************/
static uint8 tms_get_packet(void)
{
	//static uint8 flag=0;
	uint8 temp;	
	uint8 (*tms_recive_fun)(uint8 *)=TMS_RECEIVE_CHAR;
	while(tms_recive_fun(&temp))
	{
		if(flag==0) 		
		{
			if(temp == 0xFE)  //֡ͷ
			{
				flag =1;
				tms_recv_length =0;
				tms_recv_buf[tms_recv_length++] = temp;
			}
		}
		else			//���ݲ���
		{
			tms_recv_buf[tms_recv_length++] = temp;
			if(tms_recv_length == (TMS_PACKET_LENGTH))
			{				
				//ptu_print_buf(tms_recv_buf, TMS_PACKET_LENGTH);  
				flag = 0 ;
				if(tms_recv_buf[tms_recv_length -1] ==0xFF) // ֡β
				{
					return 1;
				}
			}
		}

		if( tms_recv_length >= TMS_BUF_SIZE )
		{
			tms_recv_length = 0;
			//print_line("long tms packet");
		}
	}
	
	return 0;
}
#endif

/****************************************************
*��������tms_proc			       		         
*���ߣ�hk             							 
*�����������TMS ���յ�����			
*���룺��										
*�������	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void tms_proc(void)
{
    #if LEAVE_DELAY
  	static uint32 leave_broadcast_delay_timer = 0; 
  	static uint8 leave_broadcast_flag = 0;
  	#endif     	

  	static uint32 current_station_delay_timer = 0; 
	static uint8 run_dir = 0;

	static uint8 opendoor=0,closedoor=0;
	uint8 recv_flag=0;	//�Ƿ�õ�������
	uint8 checksum=0;
   	uint16 src_length=0;
	uint8 station_id;
	
	uint8 temp_station_id;		//����վ������ʱ����
	static uint16 start_timer = 0;
	
	if(++start_timer<65530) //����--��ʱ
		return;
	start_timer=65530;	
	
	comm_error_tms_timer++;	
	if(comm_error_tms_timer > COMM_DATA_TIMEOUT)//��ʱ��û���յ�TMS����
	{
	    my_default_data.tms_status.tms_valid = 1;
	    comm_error_tms_timer = 0;
	    print_line("tms timeout");
	    led_onoff(TMS_LED_NO,1);
	}

	if(checktimer(&tms_timer,256))//��TMSӦ��
	{
		tms_send_ack();
	}	
		
	/***����֡����***/
	if(tms_get_packet())
	{	    
       
		led_blink(TMS_LED_NO,300,300);//ͨ�ŵ�

	   
	  	if(sh9_tms_dev.debug_flag == 1||sh9_tms_dev.debug_flag == 3)//���͵����Զ˿�
	  	{
	  		uart0_write_buf(tms_recv_buf, TMS_PACKET_LENGTH);	    	
	  	}

		if(sh9_tms_dev.debug_flag == 5)
			ptu_print_buf(tms_recv_buf, TMS_PACKET_LENGTH);  

		checksum= 0 ; //����У���
	  	for(src_length=1; src_length<TMS_PACKET_LENGTH-2; src_length++) 
	   	{
			checksum +=tms_recv_buf[src_length];
		}
	  
	      if(checksum == tms_recv_buf[TMS_PACKET_LENGTH-2]) //�ж�У���
	      {
	          recv_flag = 1;
	      }
	   	else
	   	{
	   		recv_flag = 0;  //�������ݲ���
	    		print_line("tcms checksum error");
	    		//print_int("%x chec", checksum);
			//print_int("%x ch", tms_recv_buf[TMS_PACKET_LENGTH-2]);
	   	}
		
		if ((tms_recv_packet->start_station==0xff)//atc�ղ���tms���ݻ�0xFF
			&& (tms_recv_packet->end_station==0xff))
		{
			//ptu_print_debug("tms_no_ack");
			return;
		}
	}

	/******** TMS ���ݴ���ʼ**********/
	if(recv_flag)
	{
		comm_error_tms_timer = 0;//tms ͨѶ����
		my_default_data.tms_status.tms_valid = 0;//tms  �������
		
		/***********1. ATO ģʽ*************/
		//if(0)
		//if(sh9_tms_dev.pis_work_mode == WORK_AT_AUTO)  

		sh9_tms_dev.pis_work_mode = tms_recv_packet->atc_a1.ato_mode;
		my_default_data.tms_status.tms_mode = tms_recv_packet->atc_a1.ato_mode;
		my_default_data.cab_status.pis_mode =  tms_recv_packet->atc_a1.ato_mode;
		
		//if(tms_recv_packet->atc_a1.ato_mode)  //ato ģʽ
		if(((tms_recv_packet->atc_a1_code_id == 0x04)||(tms_recv_packet->atc_a1_code_id == 0x05)
			||(tms_recv_packet->atc_a1_code_id == 0x06))&&(tms_recv_packet->atc_a1_platform_id))
		{	
			// 1.1��С������ת��			
			//tms_recv_packet->emc_annoucement =swap_int16(tms_recv_packet->emc_annoucement);
				
			// 1.2 վ����仯����
			if((tms_recv_packet->atc_a1_code_id == 0x04)
				||(tms_recv_packet->atc_a1_code_id == 0x05)) // ��һվ
			{
				my_default_data.cab_status.drmd_welcome = 0 ;

				if(atc_a1_code_id==0)  //�ȸ���һվ���ٸ��յ�վ��
					start_chg_flag = 1;
				
				if(tms_recv_packet->atc_a1_code_id == 0x04)		
					recv_next_flag = 1; //�յ���һվ�����ʾ
				else
					recv_next_flag = 2; // �յ�Խս�����ʾ
			
				print_int("%x n_id:", tms_recv_packet->atc_a1_platform_id);
				station_id=Station_code_to_ID(tms_recv_packet->atc_a1_platform_id);
				if(station_id)
					sh9_tms_dev.next_station = station_id;		
				else
				{	
					temp_station_id =0;
					while(saveparam.stations_code_table_up[temp_station_id])
					{
					       
						if( tms_recv_packet->atc_a1_platform_id == saveparam.stations_code_table_up[temp_station_id] )
						{
							sh9_tms_dev.next_station = temp_station_id+1;	
							
							break;
						}

						if( tms_recv_packet->atc_a1_platform_id == saveparam.stations_code_table_down[temp_station_id] )
						{
							sh9_tms_dev.next_station = temp_station_id+1;	
							break;
						}
						temp_station_id++;					
					}		
				}

				#if 0
				if(next_station_code != sh9_tms_dev.next_station ) //��һվ�ı�
				{
					next_station_code = sh9_tms_dev.next_station ;	
					if(start_chg_flag)  //����ʼվ��ֵ
					{
						start_chg_flag = 0;
						sh9_tms_dev.start_station =sh9_tms_dev.next_station;						
					}
					
					sh9_tms_dev.arrived = 0;//����վ
					my_default_data.trigger_hbit.leave_broadcast=1;
					#if LEAVE_DELAY
					    leave_broadcast_delay_timer = get_clock();
					    leave_broadcast_flag = 1; 
					#else 
						station_broadcast = 1; //��վ�㲥						
					#endif
					print_line("pre broadcast");
				}	
				#endif
			}
			else if(tms_recv_packet->atc_a1_code_id == 0x06
				&& end_station_code != tms_recv_packet->atc_a1_platform_id) //�յ�վ
			{
				start_chg_flag = 1;   //�յ�վ�ı�ʱ����ʼվ��־�ϣ��ȴ���һ����һվѡ�����䡣
				//print_int("%dendidr:",tms_recv_packet->atc_a1_platform_id);
				end_station_code = tms_recv_packet->atc_a1_platform_id ;				
				station_id=Station_code_to_ID(tms_recv_packet->atc_a1_platform_id);
				if(station_id)		
					sh9_tms_dev.end_station = station_id;			
				else
				{
					temp_station_id =0;
					while(saveparam.stations_code_table_up[temp_station_id])
					{
					       
						if( tms_recv_packet->atc_a1_platform_id == saveparam.stations_code_table_up[temp_station_id] )
						{
							sh9_tms_dev.end_station = temp_station_id+1;	
							
							break;
						}

						if( tms_recv_packet->atc_a1_platform_id == saveparam.stations_code_table_down[temp_station_id] )
						{
							sh9_tms_dev.end_station = temp_station_id+1;	
							break;
						}
						temp_station_id++;					
					}
					//print_int("%dendid2:",sh9_tms_dev.end_station); 
				}			
			}

			
			// 1.3 ��վ�㲥��ʱ				
			#if LEAVE_DELAY
		 	if(leave_broadcast_flag && checktimer(&leave_broadcast_delay_timer,saveparam.broadcast_delay_time[sh9_tms_dev.next_station -1] ))
			{
				
			       leave_broadcast_flag = 0;
				if(sh9_tms_dev.next_station != sh9_tms_dev.start_station)
				{
					station_broadcast = 1;
					sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//����һվʱ�������ź�ȡĬ��ֵ
					if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
						&& sh9_tms_dev.next_station ==11)
						sh9_tms_dev.door_sides == 0x03 ; 
				   	print_line(" leave delay boardcast");
				}
			}       
			#endif

			// 1.4	��վ
			if(tms_recv_packet->atc_a1.ato_depart)
			{
				//sh9_tms_dev.arrived = 0;				
			}

			// 1.5 ��վ
			if(tms_recv_packet->lwp1.door_open)
			{
				recv_next_flag = 0 ;  //���ž����㣬�����Ƿ�Խս
				 sh9_tms_dev.arrived = 1;  //����վ
				 //sh9_tms_dev.current_station = sh9_tms_dev.next_station;
				 my_default_data.trigger_hbit.leave_broadcast=0;
	    			my_default_data.trigger_hbit.arrived_broadcast=1;
		     		ptu_print_debug("arrive");
			}			

			// 1.6 ������			
			if (tms_recv_packet->lwp1.door_open || tms_recv_packet->atc_a1.leftdoor_open
				|| tms_recv_packet->atc_a1.rightdoor_open)   //���Ŵ���
			//if (tms_recv_packet->lwp1.door_open || tms_recv_packet->lwp1.manual_leftdoor_open
			//	||tms_recv_packet->lwp1.manual_rightdoor_open|| tms_recv_packet->atc_a1.leftdoor_open
			//	|| tms_recv_packet->atc_a1.rightdoor_open)
			{	
				sh9_tms_dev.arrived = 1;  //����վ
				sh9_tms_dev.current_station =sh9_tms_dev.next_station ; 	//���µ�ǰվ	
				sh9_tms_dev.door_sides = 0x80 ;	//��8λ���ϱ�ʾ�г������ź�				
				if(tms_recv_packet->atc_a1.leftdoor_open) // ato ������
					sh9_tms_dev.door_sides |= (1<<1);

				if(tms_recv_packet->atc_a1.rightdoor_open) //ato ������
					sh9_tms_dev.door_sides |= (1<<0);		

				if(!tms_recv_packet->lwp1.door_open)
				{
					if(recv_next_flag == 1)
						recv_next_flag = 0 ;  // Խս������ ,�Է��������Ŵ���λû����
				}
			
				my_default_data.trigger_hbit.opendoor = 1;
				my_default_data.trigger_hbit.closedoor = 0;			
				current_station_delay_timer = get_clock();

				#if 1
				
					if(opendoor ==0)
					{
						//station_broadcast = 3; //������ʾ��
						opendoor =1 ;
					}
					closedoor = 0;
				#endif
			}
			else  //û���г������źţ�����Ĭ�Ͽ����źŴ���
			{			
				my_default_data.trigger_hbit.opendoor = 0;							
				sh9_tms_dev.arrived = 0;  //����վ

				//print_line("dt0");
				if(closedoor==0 && (get_clock() - current_station_delay_timer >4000))
				{	
					sh9_tms_dev.door_sides = 0;// �ֲ���������ţ�����ȫ��!
					my_default_data.trigger_hbit.closedoor = 1;
					//station_broadcast = 3;  //������ʾ��
					closedoor =1 ;
				}
				opendoor = 0;				

				#if 0
				if (closedoor) //û�����ź�Ĭ�Ͽ��Ų���ʾ
				{
					if( tms_recv_packet->atc_a1_code_id ==0x04 ||tms_recv_packet->atc_a1_code_id ==0x05)
					{
						//print_line("dt3");
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//����һվʱ�������ź�ȡĬ��ֵ
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					}
				}
				#endif
				
			}	 			

			// 1.7 30���źŴ��� ��վ
			if((!tms_recv_packet->lwp1.manual_docked_station )
			   && (manual_docked_station != tms_recv_packet->lwp1.manual_docked_station)
			   &&(tms_recv_packet->lwp1.manual_nextstation))
			{
				//  1.71û���յ���һվ����
				if((recv_next_flag == 0) && (sh9_tms_dev.next_station !=sh9_tms_dev.end_station)) 
				{					
					#if 1
					if(sh9_tms_dev.next_station <=25 && sh9_tms_dev.end_station >25)
					{											
						 if(sh9_tms_dev.next_station > 11 && sh9_tms_dev.next_station<=25)
							sh9_tms_dev.next_station--;
						 else if(sh9_tms_dev.next_station == 11)			
						 	sh9_tms_dev.next_station =26 ;
						 else
						 	sh9_tms_dev.next_station++;
					}
					else if(sh9_tms_dev.next_station > 25 && sh9_tms_dev.end_station <=25)
					{
						if(sh9_tms_dev.next_station == 26)
							sh9_tms_dev.next_station = 11;
						else 
							sh9_tms_dev.next_station --;
					}
					else
					{
						//print_line("tcms next++");
						if(sh9_tms_dev.next_station < sh9_tms_dev.end_station)
						{
							sh9_tms_dev.next_station++;
							if(sh9_tms_dev.next_station ==  23)
								sh9_tms_dev.next_station++;
						}
						else if(sh9_tms_dev.next_station > sh9_tms_dev.end_station)
						{
							sh9_tms_dev.next_station--;	
							if(sh9_tms_dev.next_station ==  23)
								sh9_tms_dev.next_station--;
						}
					}
					#endif
				}

				// 1.72 30�ױ�վ
				#if 1
				if(next_station_code != sh9_tms_dev.next_station ) //��һվ�ı�
				{
					next_station_code = sh9_tms_dev.next_station ;	
					if(start_chg_flag)  //����ʼվ��ֵ
					{
						start_chg_flag = 0;

						if((sh9_tms_dev.next_station == 1 ) ||(sh9_tms_dev.next_station == 30 )
							||(sh9_tms_dev.next_station == 25 ))
						{							
							sh9_tms_dev.start_station =sh9_tms_dev.next_station;
						}					
						else
						{						
							if(sh9_tms_dev.next_station < sh9_tms_dev.end_station)
								sh9_tms_dev.start_station =sh9_tms_dev.next_station  - 1;
							else if(sh9_tms_dev.next_station > sh9_tms_dev.end_station)
								sh9_tms_dev.start_station =sh9_tms_dev.next_station  + 1;

							if((sh9_tms_dev.next_station == 26)
							  &&(sh9_tms_dev.next_station<sh9_tms_dev.end_station))
								sh9_tms_dev.start_station = 11;
						}
					}
					
					sh9_tms_dev.arrived = 0;//����վ
					my_default_data.trigger_hbit.leave_broadcast=1;
					#if LEAVE_DELAY
					    leave_broadcast_delay_timer = get_clock();
					    leave_broadcast_flag = 1; 
					#else 
						station_broadcast = 1; //��վ�㲥			
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//����һվʱ�������ź�ȡĬ��ֵ
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					#endif
					print_line("pre broadcast");
				}	
				#endif
			}
		}
		
		/**************2. ato û��Խս**************/
		else if(tms_recv_packet->atc_a1.ato_mode ) // ato ���� ��վ����
		{
			my_default_data.cab_status.drmd_welcome = 1 ;
		}
		/**************3. ���Զ�/�ֶ�ģʽ**************/
		else
		{
			my_default_data.cab_status.drmd_welcome = 0 ;
			//��һվ�Զ��ۼ�/��
			if((!tms_recv_packet->lwp1.manual_docked_station )
			   && (manual_docked_station != tms_recv_packet->lwp1.manual_docked_station)
			   && (sh9_tms_dev.next_station !=sh9_tms_dev.end_station)
		      	   &&(tms_recv_packet->lwp1.manual_nextstation))
			{					
				#if 1
					if(sh9_tms_dev.next_station <=25 && sh9_tms_dev.end_station >25)
					{											
						 if(sh9_tms_dev.next_station > 11 && sh9_tms_dev.next_station<=25)
							sh9_tms_dev.next_station--;
						 else if(sh9_tms_dev.next_station == 11)			
						 	sh9_tms_dev.next_station =26 ;
						 else
						 	sh9_tms_dev.next_station++;
					}
					else if(sh9_tms_dev.next_station > 25 && sh9_tms_dev.end_station <=25)
					{
						if(sh9_tms_dev.next_station == 26)
							sh9_tms_dev.next_station = 11;
						else 
							sh9_tms_dev.next_station --;
					}
					else
					{
						//print_line("tcms next++");
						if(sh9_tms_dev.next_station < sh9_tms_dev.end_station)
							sh9_tms_dev.next_station++;
						
						else if(sh9_tms_dev.next_station > sh9_tms_dev.end_station)
							sh9_tms_dev.next_station--;					
					}
				#else
						
					if(sh9_tms_dev.start_station <=25 && sh9_tms_dev.end_station >25)
					{
						//print_line("next2");
						if(sh9_tms_dev.start_station <= 11 )
						{
							if(sh9_tms_dev.next_station == 11)
								sh9_tms_dev.next_station = 26;
							else 
								sh9_tms_dev.next_station++;
						}
						else if(sh9_tms_dev.start_station > 11 )
						{
							if(sh9_tms_dev.next_station == 11)
								sh9_tms_dev.next_station = 26;
							else if(sh9_tms_dev.next_station >=26)
								sh9_tms_dev.next_station++;
							else
								sh9_tms_dev.next_station--;
						}
					}
					else if(sh9_tms_dev.start_station > 25 && sh9_tms_dev.end_station <=25)
					{
						//print_line("next3");
						if(sh9_tms_dev.end_station <= 11 )
						{
							if(sh9_tms_dev.next_station == 26)
								sh9_tms_dev.next_station = 11;
							else 
								sh9_tms_dev.next_station--;
						}
						else if(sh9_tms_dev.end_station > 11 )
						{
							if(sh9_tms_dev.next_station == 26)
								sh9_tms_dev.next_station = 11;
							else if(sh9_tms_dev.next_station >26)
								sh9_tms_dev.next_station--;
							else
								sh9_tms_dev.next_station++;
						}
					}
					else
					{
						//print_line("tcms next++");
						if(sh9_tms_dev.start_station < sh9_tms_dev.end_station)
							sh9_tms_dev.next_station++;
						
						else if(sh9_tms_dev.start_station > sh9_tms_dev.end_station)
							sh9_tms_dev.next_station--;					
					}
				#endif

				
				if(next_station_code != sh9_tms_dev.next_station)
				{		
					//print_line("next4");
					next_station_code = sh9_tms_dev.next_station;
					sh9_tms_dev.arrived = 0;//����վ
					my_default_data.trigger_hbit.leave_broadcast=1;
					station_broadcast = 1;

					#if 1			
					//if((sh9_tms_dev.door_sides >>7) ==0)//suven  �޸��ֶ�ģʽ�¿��Ų�
					{
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1] ;
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					}
					#endif	

				}
			}
			
		
		 	// 2.2 �����Ŵ���
			
			if (tms_recv_packet->lwp1.door_open || (tms_recv_packet->lwp1.manual_leftdoor_open)
				||(tms_recv_packet->lwp1.manual_rightdoor_open))   //���Ŵ���
			{	
				sh9_tms_dev.door_sides = 0x80 ;	//��8λ���ϱ�ʾ�г������ź�				
				if(tms_recv_packet->lwp1.manual_leftdoor_open) // ato ������
					sh9_tms_dev.door_sides |= (1<<1);

				if(tms_recv_packet->lwp1.manual_rightdoor_open) //ato ������
					sh9_tms_dev.door_sides |= (1<<0);			
			
				my_default_data.trigger_hbit.opendoor = 1;
				my_default_data.trigger_hbit.closedoor = 0;			

				if(tms_recv_packet->lwp1.door_open)
				{
					 sh9_tms_dev.arrived = 1;  //����վ					 
					 
					 //sh9_tms_dev.current_station = sh9_tms_dev.next_station;
					 my_default_data.trigger_hbit.leave_broadcast=0;
		    			my_default_data.trigger_hbit.arrived_broadcast=1;
			     		ptu_print_debug("arrive");
				}

				//if(tms_recv_packet->lwp1.door_open)  // ��״̬:��
				{
					if(opendoor ==0)
					{
						//station_broadcast = 3; //������ʾ��
						opendoor =1 ;
					}
					closedoor = 0;	
				}			
			}
			else    // �޿����ź�
			{							
				sh9_tms_dev.arrived = 0;  //����վ
				//if(tms_recv_packet->lwp1.door_open == 0 )//��״̬: ��
				{		
					my_default_data.trigger_hbit.opendoor = 0;
					my_default_data.trigger_hbit.closedoor = 1;

					if(closedoor==0)
					{	
						sh9_tms_dev.door_sides = 0;// �ֲ���������ţ�����ȫ��!
						//station_broadcast = 3;  //������ʾ��
						closedoor =1 ;
					}
					opendoor = 0;
			 	}
			}
			
		}
		
		/*3. ����ģʽ��ͬ*/

		manual_docked_station = tms_recv_packet->lwp1.manual_docked_station;
		
		if(tms_recv_packet->atc_a1_code_id == 0)
		{
			atc_a1_code_id_cnt ++ ;
			if(atc_a1_code_id_cnt >=10)  //��ֹ���������
			{
				atc_a1_code_id = 0;
				atc_a1_code_id_cnt = 10;
			}
		}
		else
		{
			atc_a1_code_id_cnt = 0;
			atc_a1_code_id = tms_recv_packet->atc_a1_code_id;
		}
		
		//��ʼվ����
		if(sh9_tms_dev.next_station < sh9_tms_dev.end_station)
		{
			sh9_tms_dev.run_dir = 1;   //���� ��خ->���
			if(sh9_tms_dev.next_station < sh9_tms_dev.start_station)
				sh9_tms_dev.start_station = sh9_tms_dev.next_station ; 
		}
		else if(sh9_tms_dev.next_station > sh9_tms_dev.end_station)
		{
			sh9_tms_dev.run_dir = 0;   //���� ���->��خ
			if(sh9_tms_dev.next_station > sh9_tms_dev.start_station)
				sh9_tms_dev.start_station = sh9_tms_dev.next_station ; 
		}

		#if 0 
		if(run_dir != sh9_tms_dev.run_dir)
		{
			run_dir = sh9_tms_dev.run_dir ;
			sh9_tms_dev.start_station = sh9_tms_dev.next_station ; 
		}
		#endif
		//if(tms_recv_packet->lw2.timedate_vaild && tms_recv_packet->date_time2 )//����ʱ��
	//		timedate = tms_recv_packet->date_time2 ; 

		sh9_tms_dev.chinese_enable = tms_recv_packet->lwp2.chinese_disable; //�㲥����
		sh9_tms_dev.english_enable= tms_recv_packet->lwp2.english_disable;
		sh9_tms_dev.cantonese_enable= tms_recv_packet->lwp2.cantonese_disable;
		sh9_tms_dev.line_map_display_enable = tms_recv_packet->lwp2.transfer_announce_disable;
		sh9_tms_dev.line_map_display_enable = tms_recv_packet->lwp2.led_map_disable;//��̬��ͼ��ʾ����
	
	}


	
}

//***********************************************************************************************************************
//��������:�г��ܿ������������㲥
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 tms_get_special_broadcast(void)
{
	if(!special_broadcast)return 0;
	special_broadcast=0;
	return 1;
}

//***********************************************************************************************************************
//��������:�г��ܿ�������ù��Ź㲥
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 tms_get_door_broadcast(void)
{
	if(!door_broadcast)return 0;
	door_broadcast=0;
	return 1;
}

//***********************************************************************************************************************
//��������:�г��ܿ��������վ�㲥
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 tms_get_station_broadcast(void)
{
	uint8 temp;
	
	if( !station_broadcast )
		return 0;
	temp = station_broadcast;
	station_broadcast = 0;
	
	return temp;
}

//***********************************************************************************************************************
//��������:�г��ܿ��������ñ���״̬
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void tms_set_pecu(icc_status_t *status)
{
	return ;
}

//***********************************************************************************************************************
//��������:�г��ܿ�������ʼ������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void tms_init(void)
{
/*
	//���ùܽ�����Ϊ����+������
	GPIO0_PC0 |= (1<<7);
	GPIO0_PC1 |= (1<<7);
	GPIO0_PC2 &= ~(1<<7);//0KM
	
	GPIO2_PC0 |= (1<<2);
	GPIO2_PC1 |= (1<<2);
	GPIO2_PC2 &= ~(1<<2);//DOOR1
	
	GPIO0_PC0 |= (1<<6);
	GPIO0_PC1 |= (1<<6);
	GPIO0_PC2 &= ~(1<<6);//DOOR2
*/	
	//��ʼ��ͨ�Ŷ˿�
	TMS_PORTINIT(TMS_BAUDRATE);
	
	//���ֹͣ��tmsģ����� �ָ����Ա�־
	if( sh9_tms_dev.debug_flag == 4 )
		sh9_tms_dev.debug_flag = 0;
}

/****************************************************
*��������tms_set_broadcast			       		         
*���ߣ�hk             							 
*���������ù㲥����			
*���룺broadcast_type  ----�㲥����
                                                 1:Ԥ��վ 2:��վ 3:������ʾ��								
*�������	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void tms_set_broadcast(uint8 broadcast_type)
{
      station_broadcast = broadcast_type;
}

/****************************************************
*��������tms_set_special_broadcast			       		         
*���ߣ�hk             							 
*��������������㲥
*���룺flag  ----1:����						
*�������	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void tms_set_special_broadcast(uint8 flag)
{
      special_broadcast = flag;
}

//��������豸��
tms_dev_t sh9_tms_dev=
{
	tms_init,
	tms_proc,
	tms_get_special_broadcast,
	tms_get_door_broadcast,
	tms_get_station_broadcast,
	tms_set_pecu,
	tms_set_broadcast,
	tms_set_special_broadcast,
	
	0,//����ͨ�Ŵ���
       0,//����㲥����
       
	0,//�Զ��г���Ч	
	1,//��ʼվ
	STATION_TOTAL_NUM,//�յ�վ
	1,//��ǰվ
	1,//��һվ
	
	0,//��վ��־
	0,//���ű�־
	0,//���Ų�
	0,//������
	0,//���еľ���--�ٷֱ�
	1,//������վ--��һ��ͣ��
	0,//led����//0,//�㲥����

       0,// 1:ATC  0:TCMS   2:manual
	0,
	5,//city id
	0,//route id
	0,//�·�
	0,//����
	0,//��
	0,//ʱ
	0,//��
	
	0,//���Ա�־
	1,//english
	1,//chinese
	1,//����

	1,
	1,  //��̬��ͼĬ�ϴ�
			
	//��TMS���͵����ݰ�
	/*{
	//{TMS_PACKET_TAB0,TMS_PACKET_TAB1,TMS_PACKET_TAB2,TMS_PACKET_TAB3},//ͷ
	{TMS_PACKET_TAB,0,0,1},//head
	0,//ͨѶ״̬
	0,//��������
	0,//�г�����״̬
	0,//�˿ͽ���ͨ������
	PROJECT_ID,//��Ŀʶ���
	ADD_FUNCTION_VERSION,//С�汾��
	HARDWARE_VERSION,//���汾��
	MINOR_VERSION,//��չ�汾��
	MAJOR_VERSION,//��汾��

	{0},//�˿ͽ���ͨ��״̬
	{0},//dva�͹��Ŵ�����
	{0},//dcp��lcu������
        	
	//��̬��ͼ����
	{{0},{0},{0},{0},{0},{0},{0},{0},},

	{0},//Ԥ��
	
	0x55,//TMS������־
	0,//У���
	//{TMS_PACKET_TAB0,TMS_PACKET_TAB1,TMS_PACKET_TAB2,TMS_PACKET_TAB3},//β
	},
	*/
};



/******************************************************************************
*文件名称：sh9_tms.c		       		         
*作          者：hk           							 
*描          述：TCMS 数据控制处理
*创建日期：2010-3-11
*修改记录：无								
******************************************************************************/


#include "include.h"

//通信协议关键字
#define TMS_PACKET_HEAD	      0x7E
#define TMS_PACKET_END	             0x0D
#define TMS_PACKET_TAB			0x7E	//数据包分割符
#define TMS_PACKET_DLE			0x7F	//数据包全局转码符
#define TMS_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TMS_PACKET_DLE_DLE		0x81	//数据包转码符的转码符
#define TMS_PACKET_LENGTH		(153)//(81)


#define LEAVE_STATION_TRIGGER_DISTANCE		300    //离站此距离触发预报站广播
#define ARRIVE_STATION_TRIGGER_DISTANCE	       100    //离站此距离触发到站广播
#define EMERGENCY_PA_ID_MAX		16

#define LEAVE_STATION_PA_TRIGGER		(1 << 0)    //离站触发
#define PRE_ARRIVE_STATION_PA_TRIGGER	(1 << 1)   //预报站触发
#define SKIP_STATION_TRIGGER			(1 << 2)   //越站触发
#define EMERGENCY_PA_TRIGGER			(1 << 3)   //紧急触发
#define CLOSE_DOOR_TRIGGER				(1 << 4)
#define OPEN_DOOR_TRIGGER				(1 << 5)
#define LEAVE_STATION_TRIGGER			(1 << 6)
#define STOP_STATION_TRIGGER			(1 << 7)


#define PRE_ARRIVAL    1   //列车将要到达车站（列车车头进站前20S）
#define DEPARTURE       8  //列车出站
#define ARRIVAL           2   //到站

#define LEAVE_DELAY    0   //离站广播延时

#define close_mathod  0   // = 0 关门取open位 ； =1 关门取close 位

//接收缓冲区
static uint8 xdata tms_recv_buf[TMS_BUF_SIZE];

static uint16 xdata tms_recv_length=0;

//定时向TMS发送数据计时器
static uint32 tms_timer=0;

//static uint8 tms_get_flag=0;

//列车总控制器接收数据包结构  from tcms
__packed typedef struct
{
	uint8 fram_begin;  		// 1   0xFE

	/* 数据部分*/

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
	uint8 port374_reserves2[8]; //10-17    suven MVB少了8个字节

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


//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
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

static tms_recv_packet_t * xdata tms_recv_packet=(tms_recv_packet_t *)&tms_recv_buf;//接收数据包的指针

//私有变量
static uint8 station_broadcast=0;//站广播标志
static uint8 special_broadcast=0;//特殊广播标志
static uint8 door_broadcast=0;//关门广播

static uint8 tms_work_mode =0;  //tms 的工作模式
static uint8 manual_docked_station = 0 ; //半自动离站信号

static uint16 start_station_code=0; //始发站
//static uint16 pre_start_station_code=0;

uint16 end_station_code=0;   //终点站
static uint8 start_chg_flag=0; 
static uint16 atc_a1_code_id = 0;
static uint8 atc_a1_code_id_cnt = 0;

uint16 next_station_code=0;  //当前数据包中的下一站
//static uint16 pre_next_station_code=0; 
//static uint8 manual_station_announcement=0;//手动报站

//static uint16 current_station_code=0;   //当前点站
//static uint16 pre_current_station_code=0;  

//static uint16 dis_from_next_station=0;   //距下一站距离
//static uint16 dis_from_pre_station = 0;  //离开上一站的距离
//static uint16 dis_from_next_station_tmp=0;   //距下一站距离
//static uint16 dis_from_pre_station_tmp = 0;  //离开上一站的距离
//static uint16 between_station_distance = 0;     //两站之间的距离

//static uint16 DDC_skip_station_1=0;
//static uint16 DDC_skip_station_2=0;

//static uint8 left_door_open_real=0;  //真正开门标示
//static uint8 right_door_open_real=0;


static uint8 ato_door_open=0;  //真正开门标示

//static uint16   emergency_ID = 0;         //紧急广播
//static uint8    pre_emergency = 0;

static uint16 comm_error_tms_timer=0;

static uint8 flag=0;

static uint8 recv_next_flag = 0;  //接收到下一站代码标志。
 
//static uint8         door_status = 0; 
//static uint8      close_right_door_status = 0; 

//static uint8  open_left_door = 0;
//static uint8  open_right_door = 0;

//static uint16     velocity = 0;

//static uint32 timedate=0; //时间日期

//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
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
//函数作用:列车总控制器发送应答
//参数说明:
//注意事项:
//返回说明:
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
	//头
	#if 0
	sh9_tms_dev.tms_send_packet.fram_begin=0xFE;	
	//数据部分
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
	
	//计算校验码
	checksum = send_buf[1];
	for(cnt= 2;cnt<4;cnt++)
	{
		checksum +=send_buf[cnt];
	}
	send_buf[4]=checksum;

	TMS_SEND_BUF(send_buf,6);

	//发送到调试端口
	if(sh9_tms_dev.debug_flag == 4)
		ptu_print_buf((uint8 *)send_buf,6); 
		
	//发送到调试端口
	if(sh9_tms_dev.debug_flag == 2||sh9_tms_dev.debug_flag == 3)
		uart0_write_buf((uint8 *)send_buf,6);
	
}


/****************************************************
*函数名：com_to_int16			       		         
*作者：hk             							 
*描述：把高8位和低8位合并为16数据	
*输入：无										
*输出：无	
*返回值：
*修改记录：无								
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
//作者:	suven
//函数作用:备用线ID 转换
//参数说明:备用线站代码
//注意事项:无
//返回说明:转换后的ID
//***********************************************************************************************************************
static uint16 Station_code_to_ID (uint16 Station_code_ID)
{	
	uint16 code_id;

	code_id = Station_code_ID ;
	if(code_id == 15)															// 1.番禺广场折返
		code_id =1;		
	else if(code_id == 6 ||code_id == 8) 		// 4. 大石折返
		code_id =4;
	else if(code_id == 45) 													// 8.客村折返
		code_id =8;
	else if(code_id == 34 ||code_id == 37 	// 11.体育西路折返
	 	||code_id == 36 ||code_id == 38) 		// 11.体育西路M
		code_id =11;
	else if(code_id == 28) 													// 13.广州东站折返
		code_id =13;
	else if(code_id == 57 ||code_id == 59) 	// 17. 同和折返
		code_id =17;
	else if(code_id == 82) 													// 21.龙归站折返
		code_id =21;
	else if(code_id == 75) 													// 23.高增站折返
		code_id =23;
	else if(code_id == 71 ||code_id == 73) 	// 24.机场南折返
		code_id =24;
	else if(code_id == 67 ||code_id == 69) 	// 25. 机场北折返
		code_id =25;
	else if(code_id == 16 ||code_id == 18) 	// 30. 天河客运站折返
		code_id =30;	
	else
	{
		return 0;  //不是上面代码返回0
	}
	return code_id;
}

#if 1
//PIS接收ATC数据结构
#define PIS_RECV_BUFF_SIZE 1500 /*pis数据缓存大小*/
#define PIS_RECV_PBUFF g_recv_tms_buf  /*pis数据缓存指针*/
#define PIS_FRAM_BEGIN 0xFE    /*pis数据包头*/
#define PIS_FRAM_END 0xFF      /*pis数据包尾*/
#define PIS_FRAM_LEN TMS_PACKET_LENGTH  /*pis数据包长度*/
#define PIS_DATA_BUFF tms_recv_buf  /*pis数据包指针*/
#define PIS_READ_UART TMS_RECEIVE_CHAR /*串口选择*/
/*校验和计算方法可能有不同，需要修改*/
typedef struct PIS_RECV_BUFF{
	//长度
	uint16 len;
	//接收口		
	uint16 in;
	//读口
	uint16 out;
	//数据buff
	uint8   buf[PIS_RECV_BUFF_SIZE];
}PIS_BUFF;
static PIS_BUFF g_recv_tms_buf=
{
0,
0,
0,
{0,},
};
//把所有pis数据读到g_recv_pis_buf
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
//计算g_recv_pis_buf 的长度
static uint16 get_pis_len(void)
{
	return PIS_RECV_PBUFF.len;
}
//从g_recv_pis_buf 读出指定长度数据到pbuf，返回读出的长度
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
//从g_recv_pis_buf 删除len个字节
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
		//数据分隔符
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

		//从pis_recv_buf 删除一个字节
		delete_pis_buf(1);
	}		
	return 0;
}
#endif
#else
/****************************************************
*函数名：tms_get_packet			       		         
*作者：hk             							 
*描述：接收TMS数据			
*输入：无										
*输出：无	
*返回值：1:收到完整的一条数据  0 :failed
*修改记录：无								
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
			if(temp == 0xFE)  //帧头
			{
				flag =1;
				tms_recv_length =0;
				tms_recv_buf[tms_recv_length++] = temp;
			}
		}
		else			//数据部分
		{
			tms_recv_buf[tms_recv_length++] = temp;
			if(tms_recv_length == (TMS_PACKET_LENGTH))
			{				
				//ptu_print_buf(tms_recv_buf, TMS_PACKET_LENGTH);  
				flag = 0 ;
				if(tms_recv_buf[tms_recv_length -1] ==0xFF) // 帧尾
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
*函数名：tms_proc			       		         
*作者：hk             							 
*描述：处理从TMS 接收的数据			
*输入：无										
*输出：无	
*返回值：无
*修改记录：无								
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
	uint8 recv_flag=0;	//是否得到完整包
	uint8 checksum=0;
   	uint16 src_length=0;
	uint8 station_id;
	
	uint8 temp_station_id;		//处理站代码临时变量
	static uint16 start_timer = 0;
	
	if(++start_timer<65530) //开机--延时
		return;
	start_timer=65530;	
	
	comm_error_tms_timer++;	
	if(comm_error_tms_timer > COMM_DATA_TIMEOUT)//长时间没有收到TMS数据
	{
	    my_default_data.tms_status.tms_valid = 1;
	    comm_error_tms_timer = 0;
	    print_line("tms timeout");
	    led_onoff(TMS_LED_NO,1);
	}

	if(checktimer(&tms_timer,256))//对TMS应答
	{
		tms_send_ack();
	}	
		
	/***接收帧数据***/
	if(tms_get_packet())
	{	    
       
		led_blink(TMS_LED_NO,300,300);//通信灯

	   
	  	if(sh9_tms_dev.debug_flag == 1||sh9_tms_dev.debug_flag == 3)//发送到调试端口
	  	{
	  		uart0_write_buf(tms_recv_buf, TMS_PACKET_LENGTH);	    	
	  	}

		if(sh9_tms_dev.debug_flag == 5)
			ptu_print_buf(tms_recv_buf, TMS_PACKET_LENGTH);  

		checksum= 0 ; //计算校验和
	  	for(src_length=1; src_length<TMS_PACKET_LENGTH-2; src_length++) 
	   	{
			checksum +=tms_recv_buf[src_length];
		}
	  
	      if(checksum == tms_recv_buf[TMS_PACKET_LENGTH-2]) //判断校验和
	      {
	          recv_flag = 1;
	      }
	   	else
	   	{
	   		recv_flag = 0;  //测试数据部分
	    		print_line("tcms checksum error");
	    		//print_int("%x chec", checksum);
			//print_int("%x ch", tms_recv_buf[TMS_PACKET_LENGTH-2]);
	   	}
		
		if ((tms_recv_packet->start_station==0xff)//atc收不到tms数据回0xFF
			&& (tms_recv_packet->end_station==0xff))
		{
			//ptu_print_debug("tms_no_ack");
			return;
		}
	}

	/******** TMS 数据处理开始**********/
	if(recv_flag)
	{
		comm_error_tms_timer = 0;//tms 通讯正常
		my_default_data.tms_status.tms_valid = 0;//tms  故障清除
		
		/***********1. ATO 模式*************/
		//if(0)
		//if(sh9_tms_dev.pis_work_mode == WORK_AT_AUTO)  

		sh9_tms_dev.pis_work_mode = tms_recv_packet->atc_a1.ato_mode;
		my_default_data.tms_status.tms_mode = tms_recv_packet->atc_a1.ato_mode;
		my_default_data.cab_status.pis_mode =  tms_recv_packet->atc_a1.ato_mode;
		
		//if(tms_recv_packet->atc_a1.ato_mode)  //ato 模式
		if(((tms_recv_packet->atc_a1_code_id == 0x04)||(tms_recv_packet->atc_a1_code_id == 0x05)
			||(tms_recv_packet->atc_a1_code_id == 0x06))&&(tms_recv_packet->atc_a1_platform_id))
		{	
			// 1.1大小端数据转换			
			//tms_recv_packet->emc_annoucement =swap_int16(tms_recv_packet->emc_annoucement);
				
			// 1.2 站代码变化处理
			if((tms_recv_packet->atc_a1_code_id == 0x04)
				||(tms_recv_packet->atc_a1_code_id == 0x05)) // 下一站
			{
				my_default_data.cab_status.drmd_welcome = 0 ;

				if(atc_a1_code_id==0)  //先给下一站，再给终点站码
					start_chg_flag = 1;
				
				if(tms_recv_packet->atc_a1_code_id == 0x04)		
					recv_next_flag = 1; //收到下一站代码标示
				else
					recv_next_flag = 2; // 收到越战代码标示
			
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
				if(next_station_code != sh9_tms_dev.next_station ) //下一站改变
				{
					next_station_code = sh9_tms_dev.next_station ;	
					if(start_chg_flag)  //给起始站赋值
					{
						start_chg_flag = 0;
						sh9_tms_dev.start_station =sh9_tms_dev.next_station;						
					}
					
					sh9_tms_dev.arrived = 0;//车离站
					my_default_data.trigger_hbit.leave_broadcast=1;
					#if LEAVE_DELAY
					    leave_broadcast_delay_timer = get_clock();
					    leave_broadcast_flag = 1; 
					#else 
						station_broadcast = 1; //离站广播						
					#endif
					print_line("pre broadcast");
				}	
				#endif
			}
			else if(tms_recv_packet->atc_a1_code_id == 0x06
				&& end_station_code != tms_recv_packet->atc_a1_platform_id) //终点站
			{
				start_chg_flag = 1;   //终点站改变时，起始站标志上，等待第一次下一站选定区间。
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

			
			// 1.3 离站广播延时				
			#if LEAVE_DELAY
		 	if(leave_broadcast_flag && checktimer(&leave_broadcast_delay_timer,saveparam.broadcast_delay_time[sh9_tms_dev.next_station -1] ))
			{
				
			       leave_broadcast_flag = 0;
				if(sh9_tms_dev.next_station != sh9_tms_dev.start_station)
				{
					station_broadcast = 1;
					sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//给下一站时，开门信号取默认值
					if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
						&& sh9_tms_dev.next_station ==11)
						sh9_tms_dev.door_sides == 0x03 ; 
				   	print_line(" leave delay boardcast");
				}
			}       
			#endif

			// 1.4	离站
			if(tms_recv_packet->atc_a1.ato_depart)
			{
				//sh9_tms_dev.arrived = 0;				
			}

			// 1.5 到站
			if(tms_recv_packet->lwp1.door_open)
			{
				recv_next_flag = 0 ;  //开门就清零，不管是否越战
				 sh9_tms_dev.arrived = 1;  //车到站
				 //sh9_tms_dev.current_station = sh9_tms_dev.next_station;
				 my_default_data.trigger_hbit.leave_broadcast=0;
	    			my_default_data.trigger_hbit.arrived_broadcast=1;
		     		ptu_print_debug("arrive");
			}			

			// 1.6 开关门			
			if (tms_recv_packet->lwp1.door_open || tms_recv_packet->atc_a1.leftdoor_open
				|| tms_recv_packet->atc_a1.rightdoor_open)   //开门处理
			//if (tms_recv_packet->lwp1.door_open || tms_recv_packet->lwp1.manual_leftdoor_open
			//	||tms_recv_packet->lwp1.manual_rightdoor_open|| tms_recv_packet->atc_a1.leftdoor_open
			//	|| tms_recv_packet->atc_a1.rightdoor_open)
			{	
				sh9_tms_dev.arrived = 1;  //车到站
				sh9_tms_dev.current_station =sh9_tms_dev.next_station ; 	//更新当前站	
				sh9_tms_dev.door_sides = 0x80 ;	//第8位置上标示列车开门信号				
				if(tms_recv_packet->atc_a1.leftdoor_open) // ato 开左门
					sh9_tms_dev.door_sides |= (1<<1);

				if(tms_recv_packet->atc_a1.rightdoor_open) //ato 开右门
					sh9_tms_dev.door_sides |= (1<<0);		

				if(!tms_recv_packet->lwp1.door_open)
				{
					if(recv_next_flag == 1)
						recv_next_flag = 0 ;  // 越战不清零 ,以防丢掉开门触发位没清零
				}
			
				my_default_data.trigger_hbit.opendoor = 1;
				my_default_data.trigger_hbit.closedoor = 0;			
				current_station_delay_timer = get_clock();

				#if 1
				
					if(opendoor ==0)
					{
						//station_broadcast = 3; //开门提示音
						opendoor =1 ;
					}
					closedoor = 0;
				#endif
			}
			else  //没有列车开门信号，按照默认开门信号处理
			{			
				my_default_data.trigger_hbit.opendoor = 0;							
				sh9_tms_dev.arrived = 0;  //车到站

				//print_line("dt0");
				if(closedoor==0 && (get_clock() - current_station_delay_timer >4000))
				{	
					sh9_tms_dev.door_sides = 0;// 分不清关左右门，所以全关!
					my_default_data.trigger_hbit.closedoor = 1;
					//station_broadcast = 3;  //关门提示音
					closedoor =1 ;
				}
				opendoor = 0;				

				#if 0
				if (closedoor) //没开门信号默认开门侧提示
				{
					if( tms_recv_packet->atc_a1_code_id ==0x04 ||tms_recv_packet->atc_a1_code_id ==0x05)
					{
						//print_line("dt3");
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//给下一站时，开门信号取默认值
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					}
				}
				#endif
				
			}	 			

			// 1.7 30米信号处理 报站
			if((!tms_recv_packet->lwp1.manual_docked_station )
			   && (manual_docked_station != tms_recv_packet->lwp1.manual_docked_station)
			   &&(tms_recv_packet->lwp1.manual_nextstation))
			{
				//  1.71没有收到下一站代码
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

				// 1.72 30米报站
				#if 1
				if(next_station_code != sh9_tms_dev.next_station ) //下一站改变
				{
					next_station_code = sh9_tms_dev.next_station ;	
					if(start_chg_flag)  //给起始站赋值
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
					
					sh9_tms_dev.arrived = 0;//车离站
					my_default_data.trigger_hbit.leave_broadcast=1;
					#if LEAVE_DELAY
					    leave_broadcast_delay_timer = get_clock();
					    leave_broadcast_flag = 1; 
					#else 
						station_broadcast = 1; //离站广播			
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1];//给下一站时，开门信号取默认值
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					#endif
					print_line("pre broadcast");
				}	
				#endif
			}
		}
		
		/**************2. ato 没码越战**************/
		else if(tms_recv_packet->atc_a1.ato_mode ) // ato 出库 无站代码
		{
			my_default_data.cab_status.drmd_welcome = 1 ;
		}
		/**************3. 半自动/手动模式**************/
		else
		{
			my_default_data.cab_status.drmd_welcome = 0 ;
			//下一站自动累加/减
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
					sh9_tms_dev.arrived = 0;//车离站
					my_default_data.trigger_hbit.leave_broadcast=1;
					station_broadcast = 1;

					#if 1			
					//if((sh9_tms_dev.door_sides >>7) ==0)//suven  修改手动模式下开门侧
					{
						sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1] ;
						if(sh9_tms_dev.next_station == sh9_tms_dev.end_station
							&& sh9_tms_dev.next_station ==11)
							sh9_tms_dev.door_sides == 0x03 ; 
					}
					#endif	

				}
			}
			
		
		 	// 2.2 开关门处理
			
			if (tms_recv_packet->lwp1.door_open || (tms_recv_packet->lwp1.manual_leftdoor_open)
				||(tms_recv_packet->lwp1.manual_rightdoor_open))   //开门处理
			{	
				sh9_tms_dev.door_sides = 0x80 ;	//第8位置上标示列车开门信号				
				if(tms_recv_packet->lwp1.manual_leftdoor_open) // ato 开左门
					sh9_tms_dev.door_sides |= (1<<1);

				if(tms_recv_packet->lwp1.manual_rightdoor_open) //ato 开右门
					sh9_tms_dev.door_sides |= (1<<0);			
			
				my_default_data.trigger_hbit.opendoor = 1;
				my_default_data.trigger_hbit.closedoor = 0;			

				if(tms_recv_packet->lwp1.door_open)
				{
					 sh9_tms_dev.arrived = 1;  //车到站					 
					 
					 //sh9_tms_dev.current_station = sh9_tms_dev.next_station;
					 my_default_data.trigger_hbit.leave_broadcast=0;
		    			my_default_data.trigger_hbit.arrived_broadcast=1;
			     		ptu_print_debug("arrive");
				}

				//if(tms_recv_packet->lwp1.door_open)  // 门状态:开
				{
					if(opendoor ==0)
					{
						//station_broadcast = 3; //开门提示音
						opendoor =1 ;
					}
					closedoor = 0;	
				}			
			}
			else    // 无开门信号
			{							
				sh9_tms_dev.arrived = 0;  //车到站
				//if(tms_recv_packet->lwp1.door_open == 0 )//门状态: 关
				{		
					my_default_data.trigger_hbit.opendoor = 0;
					my_default_data.trigger_hbit.closedoor = 1;

					if(closedoor==0)
					{	
						sh9_tms_dev.door_sides = 0;// 分不清关左右门，所以全关!
						//station_broadcast = 3;  //关门提示音
						closedoor =1 ;
					}
					opendoor = 0;
			 	}
			}
			
		}
		
		/*3. 两种模式共同*/

		manual_docked_station = tms_recv_packet->lwp1.manual_docked_station;
		
		if(tms_recv_packet->atc_a1_code_id == 0)
		{
			atc_a1_code_id_cnt ++ ;
			if(atc_a1_code_id_cnt >=10)  //防止错误包清零
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
		
		//起始站修正
		if(sh9_tms_dev.next_station < sh9_tms_dev.end_station)
		{
			sh9_tms_dev.run_dir = 1;   //上行 番禺->天客
			if(sh9_tms_dev.next_station < sh9_tms_dev.start_station)
				sh9_tms_dev.start_station = sh9_tms_dev.next_station ; 
		}
		else if(sh9_tms_dev.next_station > sh9_tms_dev.end_station)
		{
			sh9_tms_dev.run_dir = 0;   //上行 天客->番禺
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
		//if(tms_recv_packet->lw2.timedate_vaild && tms_recv_packet->date_time2 )//日期时间
	//		timedate = tms_recv_packet->date_time2 ; 

		sh9_tms_dev.chinese_enable = tms_recv_packet->lwp2.chinese_disable; //广播语言
		sh9_tms_dev.english_enable= tms_recv_packet->lwp2.english_disable;
		sh9_tms_dev.cantonese_enable= tms_recv_packet->lwp2.cantonese_disable;
		sh9_tms_dev.line_map_display_enable = tms_recv_packet->lwp2.transfer_announce_disable;
		sh9_tms_dev.line_map_display_enable = tms_recv_packet->lwp2.led_map_disable;//动态地图显示开关
	
	}


	
}

//***********************************************************************************************************************
//函数作用:列车总控制器获得特殊广播
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 tms_get_special_broadcast(void)
{
	if(!special_broadcast)return 0;
	special_broadcast=0;
	return 1;
}

//***********************************************************************************************************************
//函数作用:列车总控制器获得关门广播
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 tms_get_door_broadcast(void)
{
	if(!door_broadcast)return 0;
	door_broadcast=0;
	return 1;
}

//***********************************************************************************************************************
//函数作用:列车总控制器获得站广播
//参数说明:
//注意事项:
//返回说明:
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
//函数作用:列车总控制器设置报警状态
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void tms_set_pecu(icc_status_t *status)
{
	return ;
}

//***********************************************************************************************************************
//函数作用:列车总控制器初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void tms_init(void)
{
/*
	//配置管脚属性为输入+弱上拉
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
	//初始化通信端口
	TMS_PORTINIT(TMS_BAUDRATE);
	
	//如果停止了tms模拟调试 恢复调试标志
	if( sh9_tms_dev.debug_flag == 4 )
		sh9_tms_dev.debug_flag = 0;
}

/****************************************************
*函数名：tms_set_broadcast			       		         
*作者：hk             							 
*描述：设置广播类型			
*输入：broadcast_type  ----广播类型
                                                 1:预报站 2:到站 3:关门提示音								
*输出：无	
*返回值：无
*修改记录：无								
****************************************************/
static void tms_set_broadcast(uint8 broadcast_type)
{
      station_broadcast = broadcast_type;
}

/****************************************************
*函数名：tms_set_special_broadcast			       		         
*作者：hk             							 
*描述：设置特殊广播
*输入：flag  ----1:设置						
*输出：无	
*返回值：无
*修改记录：无								
****************************************************/
static void tms_set_special_broadcast(uint8 flag)
{
      special_broadcast = flag;
}

//输出虚拟设备类
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
	
	0,//串口通信错误
       0,//特殊广播代码
       
	0,//自动行车有效	
	1,//起始站
	STATION_TOTAL_NUM,//终点站
	1,//当前站
	1,//下一站
	
	0,//到站标志
	0,//开门标志
	0,//开门侧
	0,//上下行
	0,//运行的距离--百分比
	1,//经过的站--不一定停车
	0,//led测试//0,//广播测试

       0,// 1:ATC  0:TCMS   2:manual
	0,
	5,//city id
	0,//route id
	0,//月份
	0,//日期
	0,//年
	0,//时
	0,//分
	
	0,//调试标志
	1,//english
	1,//chinese
	1,//粤语

	1,
	1,  //动态地图默认打开
			
	//向TMS发送的数据包
	/*{
	//{TMS_PACKET_TAB0,TMS_PACKET_TAB1,TMS_PACKET_TAB2,TMS_PACKET_TAB3},//头
	{TMS_PACKET_TAB,0,0,1},//head
	0,//通讯状态
	0,//生命周期
	0,//列车总线状态
	0,//乘客紧急通信连接
	PROJECT_ID,//项目识别号
	ADD_FUNCTION_VERSION,//小版本号
	HARDWARE_VERSION,//主版本号
	MINOR_VERSION,//发展版本号
	MAJOR_VERSION,//大版本号

	{0},//乘客紧急通信状态
	{0},//dva和功放错误字
	{0},//dcp和lcu错误字
        	
	//动态地图错误
	{{0},{0},{0},{0},{0},{0},{0},{0},},

	{0},//预留
	
	0x55,//TMS启动标志
	0,//校验和
	//{TMS_PACKET_TAB0,TMS_PACKET_TAB1,TMS_PACKET_TAB2,TMS_PACKET_TAB3},//尾
	},
	*/
};



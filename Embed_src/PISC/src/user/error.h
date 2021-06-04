//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �豸������ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:error.h

//*�ļ�����:�豸������

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef ERROR_H
#define ERROR_H


//ͨ�ų�ʱ
#define COMM_DATA_TIMEOUT	60000///60000


//10���ӽ���һ���쳣���
#define ERROR_CHECK_TIME   500000//700000

extern void error_proc(void);

__packed typedef struct 
{
       //   0      1        2        3       4       5       6        7
       //srck1  srck2   srck3    srck4   srck5   srck6   crck1    crck2
	uint8 status1;	// 6
	
	//   0      1        2      3        4       5          6         7
      //dva1   dva2      apu1   apu2     dcp1    dcp2    record1   record2
	uint8 status2;	// 7

	//   0      1        2      3        4       5          6         7
      //atc1   atc2       io1    io2      io3     io4       io5        io6        
	uint8 status3;		// 8
	
	uint8 srck[TBA_LCU_NUM][8];//	 9-56	���������豸״̬
	
		
} error_packet_t;
extern error_packet_t error_pack;
//extern error_packet_t pre_error_pack;

extern void set_error_record(uint8 flag);
#endif



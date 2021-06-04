//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��λ������ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:ptu.c

//*�ļ�����:��λ������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"

#define error_data_length 0x38


//���ջ�����
uint8 ptu_recv_buf[PTU_BUF_SIZE];
static uint16 ptu_recv_length=0;

static uint32 soft_update_time=0;
static uint8 ptu_debug_cmd=0;//�õ���������ı�־


extern uint8 destled_version[4];

uint8 xdata const PTU_volume_table[41][10]={
{0,1,0,0,0,0,0,0,0,0},{9,2,0,0,0,0,0,0,0,0},{15,3,0,0,0,0,0,0,0,0},{19,4,0,0,0,0,0,0,0,0},{22,5,0,0,0,0,0,0,0,0},   // 1-5
{25,6,0,0,0,0,0,0,0,0},{27,7,0,0,0,0,0,0,0,0},{28,8,0,0,0,0,0,0,0,0},{30,9,0,0,0,0,0,0,0,0},{31,10,0,0,0,0,0,0,0,0}, // 6-10
{33,11,0,0,0,0,0,0,0,0},{34,12,0,0,0,0,0,0,0,0},{35,13,0,0,0,0,0,0,0,0},{36,14,0,0,0,0,0,0,0,0},{37,15,0,0,0,0,0,0,0,0}, // 11-15
{38,16,0,0,0,0,0,0,0,0},{39,17,0,0,0,0,0,0,0,0},{40,18,19,0,0,0,0,0,0,0},{41,20,0,0,0,0,0,0,0,0},{42,21,22,0,0,0,0,0,0,0},// 16-20
{43,23,24,0,0,0,0,0,0,0},{44,25,0,0,0,0,0,0,0,0},{45,26,27,0,0,0,0,0,0,0},{46,28,29,0,0,0,0,0,0,0},{47,30,31,32,0,0,0,0,0,0},// 21-25
{48,33,34,0,0,0,0,0,0,0},{49,35,36,37,0,0,0,0,0,0},{50,38,39,40,0,0,0,0,0,0},{51,41,42,43,0,0,0,0,0,0},{52,44,45,46,0,0,0,0,0,0},// 26-30
{53,47,48,49,0,0,0,0,0,0},{54,50,51,52,53,0,0,0,0,0},{55,54,55,56,57,0,0,0,0,0},{56,58,59,60,61,62,0,0,0,0},{57,63,64,65,66,0,0,0,0,0},// 31-35
{58,67,68,69,70,71,0,0,0,0},{59,72,73,74,75,76,77,0,0,0},{60,78,79,80,81,82,83,0,0,0},{61,84,85,86,87,88,89,0,0,0},{62,90,91,92,93,94,95,96,0,0},// 36-40
{63,97,98,99,100,0,0,0,0,0}// 41
};

//*************************************************************************************************
//��������: �������ò��
//����˵��: �ٷֱ�
//����˵��: ����ֵ
//*************************************************************************************************
/*
static void ptu_volume_check (uint8* volume)
{
	uint8 i=0,j=0,k=0;

	dog();
	
	
	for(i=0;i<7;i++)
	{	
		for(j=0;j<41;j++)
		{
			if (i>=7)
				break;
			
			for(k=0;k<9;k++)
			{
				
				if(PTU_volume_table[j][k+1]==0 && volume[i+3]!=0)
					break;
				if(volume[i+3]==0)
				{
					switch (i)
					{
						case 0:
								saveparam.global_volume = 0 ;
								break;
						case 1:
								saveparam.occ_volume	 = 0 ;
								break;	
						case 2:
								saveparam.dva_volume = 0 ;
								break;
						case 3:
								saveparam.media_volume = 0;
								break;
						case 4:
								saveparam.door_volume = 0;
								break;
						case 5:
								saveparam.apu_bass = 0;
								break;
						case 6:
								saveparam.apu_treble = 0;
								break;
						default :
								break;
					}
					i++;
					j=0;
					break;
				}
				
				if( volume[i+3]==PTU_volume_table[j][k+1] )
				{
					switch (i)
					{
						case 0:
								saveparam.global_volume = PTU_volume_table[j][0] ;
								break;
						case 1:
								saveparam.occ_volume	 = PTU_volume_table[j][0] ;
								break;	
						case 2:
								saveparam.dva_volume = PTU_volume_table[j][0] ;
								break;
						case 3:
								saveparam.media_volume = PTU_volume_table[j][0] ;
								break;
						case 4:
								saveparam.door_volume = PTU_volume_table[j][0] ;
								break;
						case 5:
								saveparam.apu_bass = PTU_volume_table[j][0] ;
								break;
						case 6:
								saveparam.apu_treble = PTU_volume_table[j][0] ;
								break;
						default :
								break;
					}
					i++;
					j=0;
					break;
				}
			}
		}
	}
	print_int("%d i:", i);
	print_int("%d global:", saveparam.global_volume);
	print_int("%d occ:", saveparam.occ_volume);
	print_int("%d dva:", saveparam.dva_volume);
	print_int("%d media:", saveparam.media_volume);
	print_int("%d door:", saveparam.door_volume);
	print_int("%d bass:", saveparam.apu_bass);
	print_int("%d treble:", saveparam.apu_treble);
	print_line("volend");
}*/

//***********************************************************************************************************************
//��������:��λ�����ý�������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 ptu_get_packet(void)
{
	static uint8 start_flag = 0;
	uint8 temp;
	
	//�ж��Ƿ�ѵ��Կ�����tms����
	if(tms_dev->debug_flag == 44)
		return 0;
		
	while(PTU_RECEIVE_CHAR(&temp))
	{		
		//uart0_write_char(temp);
		//�Զ�������
		if( !start_flag )
		{
			//�����ָ��� �ж��Ƿ�������
			if(temp == '\r')
			{
				if(ptu_recv_length)
				{
					ptu_recv_buf[ptu_recv_length]=0;//��ӽ�����
					ptu_recv_length=0;//������ָ����0
					return 2;//�õ�һ����������
				}
			}
			else if(temp == '\n')
			{
				
			}
			else if(temp == TBA_PACKET_TAB)
			{
				start_flag = 1;
				ptu_recv_length = 0;
			}
			else if(temp)//ע����� \0 �ַ�
			{
				ptu_recv_buf[ptu_recv_length++] = temp;//�򻺴�������
				if(ptu_recv_length >= PTU_BUF_SIZE-1)
					ptu_recv_length=0;//�ж��Ƿ񳬳�	
			}
		}
		//��������
		else
		{
			//�����ָ��� �ж��Ƿ������� ��ִ����ת�� Ȼ���ж�ת���Ժ��ܳ��Ⱥ�У���
			if(temp == TBA_PACKET_TAB)
			{
				//�жϳ��� �ж��Ƿ�������
				if(ptu_recv_length >= 3)
				{
					start_flag=0;
					return 1;
				}
				ptu_recv_length=0;
			}
			//�����ַ���ֱ���ͻ�����
			else
			{
				ptu_recv_buf[ptu_recv_length] = temp;
				if(++ptu_recv_length >= TBA_BUF_SIZE)
					ptu_recv_length = 0;
			}
		}
	}
	
	return 0;
}


/****************************************************
*��������ptu_send_data			       		         
*���ߣ�hk             							 
*������send  data to ptu
*���룺packet  --- the point of data buffer
                 length   ---the length of data
*�������	
*����ֵ��
*�޸ļ�¼����								
****************************************************/
static void ptu_send_data(uint8 *packet, uint8 length)
{
      uint8 * ptr = NULL;
	uint8 cnt = 0;
	uint8 send_buf[256] = {0};
	uint8 send_length = 0;
	uint8 checksum = 0;
	send_buf[send_length++] = 0x7e;
      ptr = (uint8 *)packet;
	for(cnt = 0; cnt < length; cnt++)
	{
	    if(*ptr == 0x7e)
	    {
	         send_buf[send_length++] = 0x7f;
		   send_buf[send_length++] = 0x80;
	    }
	    else if(*ptr == 0x7f)
	    {
	         send_buf[send_length++] = 0x7f;
		   send_buf[send_length++] = 0x81;
	    }
	     else
	     	{
	     	     send_buf[send_length++] = *ptr;
	     	}
	     checksum += *ptr;
	     ptr++;
	}

	checksum = 0x55 - checksum;
      //У���
	if(checksum == 0x7e)
	{
	    send_buf[send_length++] = 0x7f;
          send_buf[send_length++] = 0x80;
	}
	else if(checksum == 0x7f)
	{
	    send_buf[send_length++] = 0x7f;
           send_buf[send_length++] = 0x81;
	}
      else
	{
	    send_buf[send_length++] = checksum;
	}
	//send_buf[send_length++] = checksum;  //У���
      send_buf[send_length++] = 0x7e;

	PTU_SEND_BUF((uint8 *)send_buf,send_length);
	
	//uart0_write_buf((uint8 *)send_buf,send_length);//debug send data to ptu
	
}


//***********************************************************************************************************************
//��������:��λ������ִ�������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void exce_ptu_cmd(uint8 *packet, uint8 length)
{
	uint8 recv_flag=0;
	uint8 checksum=0;
	uint16 src_length=0;//��ת��֮ǰ�ĳ���
	uint16 dst_length=0;//��ת���Ժ�ĳ���
	static uint8 ptu_send_data_buf[PTU_BUF_SIZE];
	uint16 ptu_recv_data_len = 0;
	uint8 tmp_len=0;
	uint8 num = 0;
	uint8 cnt = 0;
	uint8 volume = 0;
	uint32 error_addr = 0;
	int setup_cmd = 0;  //������������óɹ�

	uint16 temp = 0;
	//uint16 ptu_recv_data_len =(uint16)(packet[1]&0x00ff);
	//ptu_recv_data_len |=(uint16)((packet[2]&0xff00)<<8);
	//print_line("exce_ptu_cmd");//ptu_send_data(packet, ptu_recv_length);

	//ִ����ת�� ������У���
	for(src_length=0; src_length<ptu_recv_length; src_length++)
	{
		//��������
		packet[dst_length]=packet[src_length];
		//�жϸոտ������Ƿ���ת���
		if(packet[src_length]==TBA_PACKET_DLE && src_length<ptu_recv_length-1)
		{
			//�ж���һ���ַ��Ƿ��Ƿָ����ת��� �Ѹոտ�����ת�����ԭΪTBA_PACKET_TAB ��������һ���ַ�
			if(packet[src_length+1]==TBA_PACKET_DLE_TAB)
			{
				packet[dst_length]=TBA_PACKET_TAB;//�Ѹոտ�����ת�����ԭΪTBA_PACKET_TAB
				src_length++;//��������һ���ַ�
			}
			//�ж���һ���ַ��Ƿ���ת�����ת��� �Ѹոտ�����ת�����ԭΪTBA_PACKET_DLE ��������һ���ַ�
			else if(packet[src_length+1]==TBA_PACKET_DLE_DLE)
			{
				packet[dst_length]=TBA_PACKET_DLE;//�Ѹոտ�����ת�����ԭΪTBA_PACKET_DLE
				src_length++;//��������һ���ַ�
			}
			else
			{
				print_line("ptu 7f error");
				//���ݴ���----7fû�б���ȷת��
				dst_length=0;//���ת���Ժ�ĳ���
				break;
			}
		}
		checksum+=packet[dst_length];
		dst_length++;
	}
	if(checksum == 0x55)
	{
		recv_flag=1;
		//ptu_recv_length=0;
		print_line("PTU checksum ok");
	}
	else
	{
		recv_flag=0;
		//ptu_recv_length=0;
		print_int("%xPTU checksum error",checksum);
		
	}
	if(recv_flag)
	{	
	     ptu_recv_data_len = packet[1];
	     ptu_recv_data_len = ((ptu_recv_data_len<<8)&0xff00)|packet[2];
				
	    //��������
	    switch(packet[0])
		{
			//վ����
			case SETUP_CMD_STATIONNUM:
				print_line("SETUP_CMD_STATIONNUM");
				if(packet[3]>= STATION_NUM_MAX-1)
					break;
				saveparam.station_total_num = packet[3];
				for(tmp_len=0;tmp_len<packet[3];tmp_len++)
				{
				    temp = packet[tmp_len*2+4];
				    saveparam.stations_code_table_up[tmp_len] = (temp<<8)|packet[tmp_len*2+5];
				    //print_int("%x ", saveparam.stations_code_table[tmp_len]);
				}
				saveparam.stations_code_table_up[saveparam.station_total_num]=0;//������
				//�������
				save_set_task();
				//TMS_PORTINIT(TMS_BAUDRATE);
				//setup_cmd = 1;
				UART3_IER=0x0000;
				UART3_CR=0x0000;
				break;

			//��������
			case SETUP_CMD_VOLUME:
				print_line("SETUP_CMD_VOLUME");
				/*
				saveparam.global_volume_pcent=packet[3];//������
				saveparam.occ_volume_pcent=packet[4];
				saveparam.dva_volume_pcent=packet[5];
				saveparam.media_volume_pcent=packet[6];
				saveparam.door_volume_pcent=packet[7];
				saveparam.apu_bass_pcent=packet[8];	//�ص���
				saveparam.apu_treble_pcent=packet[9];	//����
				ptu_volume_check((uint8 *)packet);
				*/		

				saveparam.global_volume = packet[3];//������
				saveparam.occ_volume = packet[4];
				saveparam.dva_volume = packet[5];
				saveparam.media_volume = packet[6];
				saveparam.door_volume = packet[7];
				saveparam.apu_bass = packet[8];	//�ص���
				saveparam.apu_treble = packet[9];	//����

				if( packet[3] >0x2e )
					saveparam.global_volume = 0x2e;
	
				if( packet[4] >0x3f )
					saveparam.occ_volume = 0x3f ;

				if( packet[5] >0x3f )
					saveparam.dva_volume = 0x3f ;

				if( packet[6] >0x3f )
					saveparam.media_volume = 0x3f ;

				if( packet[7] >0x3f )
					saveparam.door_volume = 0x3f ;

				if( packet[8] >0x10 )
					saveparam.apu_bass = 0x10 ;

				if( packet[9] >0x10 )
					saveparam.apu_treble = 0x10 ;
				
				 
				//APU_SEND_BUF(ptu_send_data_buf,7);
				//�������
				save_set_task();
				setup_cmd = 1;
				break;
				
			//����㲥����
			case SETUP_CMD_SPECIALTIMES:
				print_line("SETUP_CMD_SPECIALTIMES");
				for(num = 0; num<10; num++)
				{
				    saveparam.special_broadcast_times[0]=packet[3];
				    saveparam.special_broadcast_times[2]=packet[3];
				}
				
				//print_int("%d times", packet[3]);
				//�������
				save_set_task();
				setup_cmd = 1;
				break;
								
			//�㲥���ȼ�
			case SETUP_CMD_BROADCASTPRIORITY:
				print_line("SETUP_CMD_BROADCASTPRIORITY");
				saveparam.broadcast_priority[0]=packet[3];
				saveparam.broadcast_priority[1]=packet[4];
				saveparam.broadcast_priority[2]=packet[5];
				saveparam.broadcast_priority[3]=packet[6];
				saveparam.broadcast_priority[4]=packet[7];
				saveparam.broadcast_priority[5]=packet[8];
				saveparam.broadcast_priority[6]=packet[9];
				saveparam.broadcast_priority[7]=packet[10];
				saveparam.broadcast_priority[8]=packet[11];
				
				//�������
				save_set_task();
				setup_cmd = 1;
				break;
			
			//�Ƿ�ͬʱý�岥��			
			case SETUP_CMD_MEDIASAMETIME:
				saveparam.media_sametime=packet[3];
				//�������
				save_set_task();
				break;

                   //��վ�㲥����
			case SETUP_CMD_ARRIVEDISTANCE:
				//sh9_tms_dev.pre_arrived_distance = packet[3];
				//sh9_tms_dev.pre_arrived_distance = (sh9_tms_dev.pre_arrived_distance << 8)|packet[4];
				//�������
				//save_set_task();
				break;

			case SETUP_CMD_LEAVEDISTANCE:
				//sh9_tms_dev.leave_distance = packet[3];
				//sh9_tms_dev.leave_distance = (sh9_tms_dev.leave_distance << 8)|packet[4];
				//�������
				//save_set_task();
				break;

                   //��ѯ����
			case SETUP_CMD_DIS_VOLUME:
				print_line("SETUP_CMD_DIS_VOLUME");
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_VOLUME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x07;
				/*
				ptu_send_data_buf[temp++] = saveparam.global_volume_pcent;//������
				ptu_send_data_buf[temp++] = saveparam.occ_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.dva_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.media_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.door_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.apu_bass_pcent;	//�ص���
				ptu_send_data_buf[temp++] = saveparam.apu_treble_pcent;	//����
				*/

				ptu_send_data_buf[temp++] = saveparam.global_volume;//������
				ptu_send_data_buf[temp++] = saveparam.occ_volume;
				ptu_send_data_buf[temp++] = saveparam.dva_volume;
				ptu_send_data_buf[temp++] = saveparam.media_volume;
				ptu_send_data_buf[temp++] = saveparam.door_volume;
				ptu_send_data_buf[temp++] = saveparam.apu_bass;	//�ص���
				ptu_send_data_buf[temp++] = saveparam.apu_treble;	//����
				
				ptu_send_data(ptu_send_data_buf,temp);
				break;

			//display station code
			case SETUP_CMD_DIS_STATIONCODE:
				print_line("SETUP_CMD_DIS_STATIONCODE");
				ptu_send_data_buf[0] = SETUP_CMD_DIS_STATIONCODE; //������
				ptu_send_data_buf[1] = 0x00;
				ptu_send_data_buf[2] = saveparam.station_total_num*2 + 1;   //data length
				ptu_send_data_buf[3] = saveparam.station_total_num;
				temp = 4;
				for(num = 0; num < saveparam.station_total_num; num++)
				{
				    ptu_send_data_buf[temp++] = ((saveparam.stations_code_table_up[num]>>8)&0x00ff);
				    ptu_send_data_buf[temp++] = (saveparam.stations_code_table_up[num]&0x00ff);
				}
				ptu_send_data(ptu_send_data_buf,temp);
				//delay(3);

				ptu_send_data_buf[0] = SETUP_CMD_SETSTATION_NAME; //������
				ptu_send_data_buf[1] = 0x00;
				
				for(num = 0; num < saveparam.station_total_num; num++)
				{  
				    ptu_send_data_buf[3] = num+1;//վ��˳��Ŵ�1��ʼ
				    temp = 4;
				    for(tmp_len = 0;saveparam.station_name[num][tmp_len] != 0; tmp_len++)
				    {
				        ptu_send_data_buf[temp++] = saveparam.station_name[num][tmp_len];
				    	}
				    ptu_send_data_buf[2] = tmp_len+1;
				    ptu_send_data(ptu_send_data_buf,temp);
				    mdelay(2);
				}
				break;

			case SETUP_CMD_DIS_ERROR:
				print_line("SETUP_CMD_DIS_ERROR");
				//set_error_record(packet[3]);
				ptu_send_data_buf[0]=0x16;
				ptu_send_data_buf[1]=0x00;
				ptu_send_data_buf[2]=error_data_length;
				ptu_send_data_buf[3]=0x00;
				ptu_send_data_buf[4]=0x00;
				ptu_send_data_buf[5]=0x00;
				ptu_send_data_buf[6]=0x00;
				ptu_send_data_buf[7]=0x00;
				memcpy((uint8*)&ptu_send_data_buf[8], (uint8*)&error_pack, sizeof(error_packet_t));
				ptu_dev->ptu_send_data(ptu_send_data_buf,error_data_length+3);
				break;

			//�汾��
			case SETUP_CMD_VERSION:
				print_line("SETUP_CMD_VERSION");
				//Ӧ��
				ptu_send_data_buf[0]=0x7e;
				ptu_send_data_buf[0]=0x00;
				ptu_send_data_buf[1]=0x05;
				ptu_send_data_buf[2]=SETUP_CMD_VERSION;
				ptu_send_data_buf[3]=HARDWARE_VERSION;
				ptu_send_data_buf[4]=ADD_FUNCTION_VERSION;
				ptu_send_data_buf[5]=MAJOR_VERSION;
				ptu_send_data_buf[6]=MINOR_VERSION;

				ptu_send_data(ptu_send_data_buf,7);
				//tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,5);
	
				break;

			//��λ
			case SETUP_CMD_RESET:
				while(1);
				break;

			case SETUP_CMD_LOAD_DEFAULT_PARM:
                          error_addr = saveparam.error_flash_addr;
				volume = saveparam.panel_volume_param;
				num = saveparam.re_write_flash;
				save_load_default();
				saveparam.error_flash_addr = error_addr;
				saveparam.re_write_flash = num;
				saveparam.panel_volume_param = volume;
				print_line("SETUP_CMD_LOAD_DEFAULT_PARM");
				setup_cmd = 1;
				break;
			case SETUP_CMD_DIS_BROADCASTPRIORITY:
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_BROADCASTPRIORITY;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x09;
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[0];//0cc
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[1];//�˿ͱ���
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[2];//˾���Խ�
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[3];//�˹��㲥
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[4];//������ʾ��
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[5];	//���ֱ�վ
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[6];	//ý�岥��
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[7];//�����㲥
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[8];//test��������
				ptu_send_data(ptu_send_data_buf,temp);
				break;
			case SETUP_CMD_SETSTATION_NAME:
				temp = ptu_recv_data_len;
				for(temp=0;temp<(ptu_recv_data_len-1);temp++)
				{
				    saveparam.station_name[packet[3]-1][temp] = packet[4+temp];
				}
				saveparam.station_name[packet[3]-1][temp] = 0;
				//saveparam.station_name[packet[3]][0] = 0;
				//�������
				save_set_task();
				//dog();
				//softdog();
				if(saveparam.station_total_num == packet[3])
				{
				   setup_cmd = 1;
				   TMS_PORTINIT(TMS_BAUDRATE);
				}
				break;

			case SETUP_CMD_BROC_DELAY_TIME:
				print_line("SETUP_CMD_BROC_DELAY_TIME");
			      for(temp = 0; temp < ptu_recv_data_len; temp++)
			      {
				    saveparam.broadcast_delay_time[temp] = packet[3+temp]*1000;
					print_int("%d ", saveparam.broadcast_delay_time[temp] );
				}
							
				//�������
				save_set_task();
				setup_cmd = 1;
				break;
			case SETUP_CMD_DIS_BROC_DELAY_TIME:
				print_line("SETUP_CMD_DIS_BROC_DELAY_TIME");
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_BROC_DELAY_TIME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = saveparam.station_total_num;
				for(num=0; num<saveparam.station_total_num; num++)
				{
				    ptu_send_data_buf[temp++]= (saveparam.broadcast_delay_time[num] / 1000);
				}
				ptu_send_data(ptu_send_data_buf,temp);

				break;
				
			case SETUP_CMD_UPDATE_SOFT:   //��������ת��
				print_line("update begin");
				uart0_write_char(0x7e);
				uart0_write_buf(&ptu_recv_buf[3], ptu_recv_length-4);
				uart0_write_char(0x7e);
		             TBA_BUS_SEND_CHAR(0x7e);
				//֮ǰ�Ѿ�������ת�봦���������ٴ�ת��
				 for(temp = 3; temp < (ptu_recv_length-4 +3); temp++)
				 {
				     if(ptu_recv_buf[temp] == 0x7e)
				     	{
				     	    TBA_BUS_SEND_CHAR(0x7f);
					    TBA_BUS_SEND_CHAR(0x80);
				     	}
				      else if(ptu_recv_buf[temp] == 0x7f)
				      	{
				      	    TBA_BUS_SEND_CHAR(0x7f);
					    TBA_BUS_SEND_CHAR(0x81);
				      	}
					 else
					 {
					     TBA_BUS_SEND_CHAR(ptu_recv_buf[temp]);
					 }
				 }
   			      //TBA_BUS_SEND_BUF(&ptu_recv_buf[3], ptu_recv_length-4);
   			      TBA_BUS_SEND_CHAR(0x7e);  
   				soft_update_time = get_clock();
   			      while(1)
   			      {
   			        temp = ptu_get_packet();
   				  if(temp )
   				  {
   				   	TBA_BUS_SEND_CHAR(0x7e);
   				  	if(((0x7f ==ptu_recv_buf[2])&&(0x80 ==ptu_recv_buf[3])) ||((0x7f ==ptu_recv_buf[2])&&(0x81 ==ptu_recv_buf[3])))
						TBA_BUS_SEND_BUF(&ptu_recv_buf[4], ptu_recv_length-5);
					else 
   					 	TBA_BUS_SEND_BUF(&ptu_recv_buf[3], ptu_recv_length-4);
					   
   					 TBA_BUS_SEND_CHAR(0x7e);  
				/*
					 uart0_write_char(0x7e);
				uart0_write_buf(&ptu_recv_buf[3], ptu_recv_length-4);
				uart0_write_char(0x7e);
				*/
   					 soft_update_time = get_clock();
   				  }
   				  dog();
				  softdog(); //��ֹ����Ӻ�λ
   				  if(checktimer(&soft_update_time, 4000))
   				  	break;
   			      }
				print_line("update end");
				break;

			case SETUP_CMD_DMP_WDS_VERSION:	//��ѯDMP  & WDS  version

				print_line("check dmp wds version");
				
				ptu_send_data_buf[0]=SETUP_CMD_DMP_WDS_VERSION;
				ptu_send_data_buf[1]=00;
				ptu_send_data_buf[2]=12;

				cnt=0;
				for (num=0;num<6;num++)
				{
					ptu_send_data_buf[cnt*2+3]=lcu_default_data[num].version_l;
					ptu_send_data_buf[cnt*2+4]=lcu_default_data[num].version_h;
					cnt++;
				}
				ptu_send_data(ptu_send_data_buf,15);
				break;

			case SETUP_CMD_DIS_FLASH:
				print_line("DIS_FLASH");
				set_error_record(packet[3]);
				break;


			case SETUP_CMD_SECTION_PLAYLIST:        //�������������յ�վ��ʾ�ﲥ���б�
                           print_line("SETUP_CMD_SECTION_PLAYLIST");
                           if(packet[3]>=1 && packet[3]<=30)//�ж��յ�վid ��Χ
                           {
					saveparam.endstation_mind_list[packet[3]-1][0]=packet[4];//�����б�
					saveparam.endstation_mind_list[packet[3]-1][1]=packet[5];
					saveparam.endstation_mind_list[packet[3]-1][2]=packet[6];
					saveparam.endstation_mind_list[packet[3]-1][3]=packet[7];
					saveparam.endstation_mind_list[packet[3]-1][4]=packet[8];
				}
				//�������
				save_set_task();
				setup_cmd = 1;
				break;

			case SETUP_CMD_DIS_SECTION_PLAYLIST:        //��ѯ���������յ�վ��ʾ�ﲥ���б�
				print_line("SETUP_CMD_DIS_SECTION_PLAYLIST");
				ptu_send_data_buf[0] = SETUP_CMD_DIS_SECTION_PLAYLIST;
				ptu_send_data_buf[1] = 0;
				ptu_send_data_buf[2] = 6; //���ݳ���

				for(cnt=0;cnt<30;cnt++)
				{
					ptu_send_data_buf[3] = cnt+1;
					ptu_send_data_buf[4] = saveparam.endstation_mind_list[cnt][0];
					ptu_send_data_buf[5] = saveparam.endstation_mind_list[cnt][1];
					ptu_send_data_buf[6] = saveparam.endstation_mind_list[cnt][2];
					ptu_send_data_buf[7] = saveparam.endstation_mind_list[cnt][3];
					ptu_send_data_buf[8] = saveparam.endstation_mind_list[cnt][4];

					ptu_send_data(ptu_send_data_buf,9);
				}				
				break;
				
			//���ù㲥������
			case SETUP_CMD_DCP_VOLUME:
				print_line("SETUP_CMD_DCP_VOLUME");
				for(cnt=3;cnt<9;cnt++)
				{
					if(packet[cnt]>15 || packet[cnt]<0)
					{
						print_line("volume value >15");
						break;
					}
				}
				if(cnt>=9)
				{
					saveparam.dcp_pa_listen_volume=packet[3];
					saveparam.dcp_cc_listen_volume=packet[4];
					saveparam.dcp_pc_listen_volume=packet[5];
					saveparam.dcp_pa_mic_volume=packet[6];
					saveparam.dcp_cc_mic_volume=packet[7];
					saveparam.dcp_pc_mic_volume=packet[8];				
				}
				//�������
				save_set_task();
				setup_cmd = 1;
				break;

			//��ѯ�㲥������
			case SETUP_CMD_DIS_DCP_VOLUME:
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_DCP_VOLUME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x06;
				ptu_send_data_buf[temp++] = saveparam.dcp_pa_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_cc_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pc_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pa_mic_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_cc_mic_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pc_mic_volume;
				ptu_send_data(ptu_send_data_buf,temp);
				break;
									
			default:
				break;
		}
		if(setup_cmd)
		{
		         ptu_send_data_buf[0] = SETUP_CMD_OK;
			     ptu_send_data_buf[1] = 0x00;
			     ptu_send_data_buf[2] = 0x00;
			     ptu_send_data(ptu_send_data_buf,3);
		}
	}


}
//***********************************************************************************************************************
//��������:��λ���������ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void ptu_proc(void)
{
	uint8 type;
	
	//��������
	type = ptu_get_packet();	
	//��������
	if(type == 1)
	{
	/*
		//ת�����г�����
		TBA_BUS_SEND_CHAR(TBA_PACKET_TAB);
		TBA_BUS_SEND_BUF(ptu_recv_buf, ptu_recv_length);
		TBA_BUS_SEND_CHAR(TBA_PACKET_TAB);
	*/
		//ִ��PTU CMD
		exce_ptu_cmd(ptu_recv_buf, ptu_recv_length);
		ptu_recv_length = 0;
	}
	else if(type == 2)
	{
		ptu_debug_cmd = 1;
	}
	
	//ͨ��ָʾ��
	if(type)
		led_toggle(PTU_LED_NO);
}

//***********************************************************************************************************************
//��������:��λ���������ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 *ptu_get_debug_cmd(void)
{
	if(!ptu_debug_cmd)
		return NULL;
	ptu_debug_cmd=0;
	return ptu_recv_buf;
}

//***********************************************************************************************************************
//��������:��λ�����ó�ʼ������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void ptu_init(void)
{	
	//��ʼ��ͨ�Ŷ˿�
	PTU_PORTINIT(PTU_BAUDRATE);
	
	//��Ʒ��Ϣ
	delay(1);
	print_line(VENDER_NAME);
	print_line(SYSTEM_NAME);
	print_line(PRODECT_NAME);
	print_line(VERSION_DATE);
	print_line("VER: "PRODECT_CODE);
	print_int("%d",HARDWARE_VERSION);
	print_int("%d",ADD_FUNCTION_VERSION);
	print_int("%d",MAJOR_VERSION);
	print_int("%d",MINOR_VERSION);
}



//��������豸��
ptu_dev_t sh9_ptu_dev=
{
	ptu_init,
	ptu_proc,
	ptu_get_debug_cmd,
	ptu_send_data,
	1,
};




/******************************************************************************
*�ļ����ƣ�sh9_dva.c		       		         
*��          �ߣ�hk           							 
*��          �������ֱ�վ������		
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"



//ͨ��Э��ؼ���
#define DVA_PACKET_HEAD		0x7E	//����ͷ
#define DVA_PACKET_END		0x0D	//����β
#define DVA_PACKET_PLAY		0x01	//��������

#define DVA_PACKET_STOP		0x02	//ֹͣ����
#define DVA_PACKET_SPECIAL	0x01	//����㲥
#define DVA_PACKET_VERSION	0x04	//��ȡ�汾��
#define DVA_PACKET_STATUS	0x05	//��ȡ״̬
#define DVA_PACKET_VOLUME	0x06	//��������

//¼��ģ������
#define RECORD_START            0x07      //��ʼ¼��
#define RECORD_STOP              0x08     //ֹͣ¼��

//��෢�ʹ���
#define SEND_TIMES_MAX			7

//����״̬��
#define DVA_TASK_IDLE			0	//����
#define DVA_TASK_SEND_PLAY		1	//���Ͳ�������
#define DVA_TASK_SEND_PREVSTOP		2	//����Ԥֹͣ����---��Ϊ���ڲ�����������
#define DVA_TASK_SEND_STOP		3	//����ֹͣ����

//DVA״̬��ʾ
//#define DVA_BUSY                1
#define DVA_IDLE                0
#define DVA_PLAYING				1
#define DVA_STOP				2
//����״̬���� 
#define DVA_DEBUG

//���ջ�����
static uint8 dva_recv_buf[DVA_BUF_SIZE];//���ջ�����
//static uint8 dva_recv_length=0;//�����ܳ���
//static uint8 dva_data_length=0;//���ݲ��ֵĳ���

static uint16 dva_list_buf[DVA_LIST_MAX];//�����б�����
static uint8 dva_list_length=0;//�����б���
static uint8 dva_play_type=0;//��������

static uint8 volatile dva_task_status=DVA_TASK_IDLE;//����״̬��
static uint16 dva_send_times=0;//�ط��Ĵ���
static uint16 dva_wait_time=0;//�ȴ�ʱ��
static uint8 dva_version=0;//�汾��
static uint8 dva_status=0;//DVA�豸״̬

static uint8 record_status = 0;    //¼��ģ��ĵ�ǰ״̬
//static uint8 play_status = 0;

//��ѯDVA��ʱ��
//static uint32 dva_check_status_timer=0;
//ͨѶ������
uint8 dva_comm_err[DVA_NUM]={0};
//����˾���Ҷ����������ݽṹ
__packed typedef struct
{
	//uint8 length;
	//uint8 device_type;
	//uint8 device_no;
	uint8 cmd;
	uint8 dva_data[DVA_BUF_SIZE];
	
}cab_bus_dva_t;

//static cab_bus_dva_t cab_dva
static cab_bus_dva_t *cab_dva_packet=(cab_bus_dva_t *)&dva_recv_buf;

/****************************************************
*��������dva_send_cmd			       		         
*���ߣ�hk             							 
*���������ֱ�վ����������				
*���룺cmd       --- ��������
                 param    ---���͵�����
                 length    ---���ݳ���
                  
*�������	
*����ֵ��0:failed  1:succeed
*�޸ļ�¼����								
****************************************************/
static uint8 dva_send_cmd(uint8 cmd, uint8 *param, uint8 length)
{
	uint8 buf[DVA_BUF_SIZE+1]={0};

	//begin add by hk 2010-5-24   for �������ļ���������
     // #if NAME_RULE_NEW
	//if(cmd == 0x01 ) //cmd =DVA_PACKET_SPECIAL || cmd=DVA_PACKET_PLAY
	//  dva_list_length = 0;
	//#endif
	//end add by hk 2010-5-24
	
	buf[0]=cmd;
	if(length>DVA_BUF_SIZE)
	{

		return 0;
	}
		
	else 
		memcpy((uint8 *)&buf[1],param,length);
	cab_bus_dev->send_data(0x07,DVA_DEVICE_NO,1,length+1,buf);

	return 1;
}

/****************************************************
*��������dva_proc			       		         
*���ߣ�hk             							 
*���������ֱ�վ�����ݴ���				
*���룺device_no       --- �豸��
                 data_length    ---���ݳ���
                 buf                ---���������
                  
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
 void dva_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{

//	play_status = 0;
	//����DVA����
	memcpy(dva_recv_buf,buf,data_length);

       //add by hk
      // debug_print_line("dva resp");
      if(DVA_DEVICE_NO != device_no)
	{
	    if(RECORD_DEVICE_NO == device_no)
	    {
	        sh9_dva_dev.record_error = 0;

		 switch(cab_dva_packet->cmd)
	       {
	             case RECORD_START:
			//��ʼ¼��
			record_status = 1;
			break;

			case RECORD_STOP:
			//ֹͣ¼��
			record_status = 0;
			break;

			default:
		      break;
		 }
	    }
	    return;
      	}

	 //���ͨѶ����
	//dva_comm_err[device_no-1]=0;
	sh9_dva_dev.comm_error=0;
	   
	switch(cab_dva_packet->cmd)
	{
		case DVA_PACKET_STATUS:
		dva_status=	cab_dva_packet->dva_data[2];   //���ڲ�����
             // if(cab_dva_packet->dva_data[2])
		debug_print_int("%d  9",cab_dva_packet->dva_data[2]);
	       //print_int("%d  status",cab_dva_packet->dva_data[1]);
	       //print_int("%d  status",cab_dva_packet->dva_data[2]);
	       //�����ǰΪ�����㲥���Ҳ��������ļ�����ֹͣ���Ž����㲥
	       if(my_default_data.trigger_hbit.urgent_broadcast && cab_dva_packet->dva_data[1])
	       {
	           dva_dev->stop();
		     dva_list_length = 0;
		    sh9_tms_dev.emergency_status = 1; //ֹͣ�����㲥��־
	       }
		break;
		
		case DVA_PACKET_STOP:          //�յ�DVA����ֹͣ�����DVA��Ϊ����
		dva_status=DVA_STOP;
		debug_print_line("dva stop");
		break;
		
		case DVA_PACKET_VERSION:
		//dva_version=cab_bus_packet->cab_data[0];
		break;
		
		default:
		break;
	}

	//����״̬�ַ��ദ��
	switch(dva_task_status)
	{
		//����
		case DVA_TASK_IDLE:
		//print_line("IDLE");
			break;

		//���Ͳ�������
		case DVA_TASK_SEND_PLAY:
                    
			//���û�п�ʼ���� һֱ���Ͳ�������
			if(dva_status==DVA_IDLE)
			{
				//dva_status=DVA_PLAYING;
				//�ȴ�dva����
				if(dva_wait_time)
				{
					dva_wait_time--;
					break;	
				}
				
				if(dva_send_times)
				{
					dva_send_times--;
					//dva_send_times = 0;
					//should delete hk
					//apu_dev->set_channel(APU_CHANNEL_DVA);
					dva_send_cmd(dva_play_type,(uint8 *)&dva_list_buf,dva_list_length<<1);
					//play_status = 1;
                                    //
                                dva_wait_time = 1;
					print_line("send play");
				}
				else
				{
				      //����dva_send_times�β��ɹ� �л�������״̬
                                  
					dva_task_status=DVA_TASK_IDLE;
					#if NAME_RULE_NEW
					   dva_list_length = 0;
					#endif
					print_line("send error");
					debug_print_int("%d  8",cab_dva_packet->dva_data[2]);
				}
			}
			//�����ʼ������ ˵������ɹ� �л�������״̬
			else
			{
			    
				dva_task_status=DVA_TASK_IDLE;
				#if NAME_RULE_NEW
				    dva_list_length = 0;
				#endif
				debug_print_line("dva playing");
			}
			break;
			
		//����Ԥֹͣ����---ֹͣ���ڲ��ŵ�
		case DVA_TASK_SEND_PREVSTOP:
		
			//���û��ֹͣ һֱ����ֹͣ����
			if(dva_status==DVA_PLAYING)
			{
				//dva_status==DVA_IDLE;
				//�ȴ�dva����
				if(dva_wait_time)
				{
					dva_wait_time--;
					break;	
				}
				
				if(dva_send_times)
				{
					dva_send_times--;
					dva_send_cmd(DVA_PACKET_STOP,NULL,0);
					debug_print_line("send pre_stop");
					dva_wait_time=1;	
				}
				else
				{
					//ǿ���л������Ͳ�������״̬
					dva_task_status=DVA_TASK_SEND_PLAY;
					//�ͷ��ʹ���
					dva_send_times=1;
					dva_wait_time=0;
					print_line("dva prestop error");
				}
			}
			else
			{

				debug_print_line("dva prestop ok");
				
				dva_task_status=DVA_TASK_SEND_PLAY;
				//�ͷ��ʹ���
				dva_send_times=7;
				dva_wait_time=0;
			}
			break;
			
		//����ֹͣ����
		case DVA_TASK_SEND_STOP:
			//���û��ֹͣ һֱ����ֹͣ����
			if(dva_status==DVA_PLAYING)
			{
				if(dva_wait_time)
				{
					dva_wait_time--;
					break;	
				}
				
				if(dva_send_times)
				{
					dva_send_times--;
					dva_send_cmd(DVA_PACKET_STOP,NULL,0);
					debug_print_line("send stop");
					dva_wait_time=0;
				}
				else
				{
					dva_task_status=DVA_TASK_IDLE;
					//sh9_dva_dev.comm_error=COMM_DATA_TIMEOUT;
					print_line("dva stop error");
				}
			}
			//���ֹͣ�� ˵������ɹ� �л�������״̬
			else
			{
				dva_task_status=DVA_TASK_IDLE;
				//sh9_dva_dev.comm_error=0;
				debug_print_line("dva stop");
			}
			break;
		
		//����״̬
		default:
			break;
	}
	
		
}

/****************************************************
*��������dva_play_list			       		         
*���ߣ�hk             							 
*�����������ļ��������ֱ�վ�������б�			
*���룺list       --- �����ļ�
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void dva_play_list(uint16 *list)
{
	//���������б�
	dva_list_length=0;
	while(*list)
	{
		dva_list_buf[dva_list_length++]=*list++;
		if(dva_list_length>=DVA_LIST_MAX-1)break;
	}
	//�Ͳ�������
	dva_play_type=DVA_PACKET_PLAY;
	//������״̬
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//�ͷ��ʹ���
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;

}

/****************************************************
*��������dva_play_list_new			       		         
*���ߣ�hk             							 
*�����������ļ��������ֱ�վ�������б�			
*���룺list       --- �����ļ�
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void dva_play_list_new(uint16  list)
{
	//���������б�

	dva_list_buf[dva_list_length++] = list;

	print_int("%dplay:" , list);
	
	//�Ͳ�������
	dva_play_type=DVA_PACKET_PLAY;
	//������״̬
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//�ͷ��ʹ���
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
}

/****************************************************
*��������dva_play_special			       		         
*���ߣ�hk             							 
*��������������㲥		
*���룺play_code       --- ����㲥����
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void dva_play_special(uint16 play_code)
{
	//���������б�
	dva_list_buf[dva_list_length++]=play_code;
	//dva_list_length=1;
	//�Ͳ�������
	dva_play_type=DVA_PACKET_SPECIAL;
	//������״̬
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//�ͷ��ʹ���
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
	//��ѯDVA��ǰ״̬
	//dva_send_cmd(DVA_PACKET_STATUS,NULL,0);
}

/****************************************************
*��������dva_stop			       		         
*���ߣ�hk             							 
*������ֹͣdva����		
*���룺��
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void dva_stop(void)
{
	//������״̬
	dva_task_status=DVA_TASK_SEND_STOP;
	//�ͷ��ʹ���
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
}


/****************************************************
*��������dva_read_version			       		         
*���ߣ�hk             							 
*��������ȡdva �汾��
*���룺��
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static uint8 dva_read_version(void)
{
	dva_send_cmd(DVA_PACKET_VERSION,NULL,0);
	return dva_version;
}

/****************************************************
*��������dva_read_status			       		         
*���ߣ�hk             							 
*��������ȡdva ״̬
*���룺��
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static uint8 dva_read_status(void)
{
	dva_send_cmd(DVA_PACKET_STATUS,NULL,0);
		
	return dva_status;
}


/****************************************************
*��������dva_set_volume			       		         
*���ߣ�hk             							 
*����������dva����
*���룺volume  ---  ������С
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void dva_set_volume(uint8 volume)
{
	dva_send_cmd(DVA_PACKET_VOLUME,&volume,1);
}

/****************************************************
*��������dva_get_playing			       		         
*���ߣ�hk             							 
*���������ֱ�վ����ȡ����״̬
*���룺��
*�������	
*����ֵ��1:����״̬  0:����	
*�޸ļ�¼����								
****************************************************/
static uint8 dva_get_playing(void)
{	
	//ֻҪ������ ǿ�Ʒ���1
	if(dva_task_status)
		return 1;

	//ֻҪ������ ǿ�Ʒ���1
	//if(dva_task_status == DVA_TASK_SEND_PLAY)
	//	return 1;
	
	//if(DVA_PLAYING_DET_REG&(0x0001<<DVA_PLAYING_DET_BIT))return 0;
	if(dva_status==DVA_PLAYING) 
		return 1;
	 return 0;
}

/****************************************************
*��������dva_clear			       		         
*���ߣ�hk             							 
*���������DVA״̬�Ͳ����б�
*���룺��
*�������	
*����ֵ��
*�޸ļ�¼����								
****************************************************/
static void dva_clear(void)
{	
	dva_task_status = DVA_TASK_IDLE;
	dva_status = DVA_IDLE;
      dva_list_length = 0;
}

/****************************************************
*��������dva_get_record_status			       		         
*���ߣ�hk             							 
*����������¼��ģ��ĵ�ǰ״̬
*���룺��
*�������	
*����ֵ��1:¼��״̬  0:ֹͣ״̬
*�޸ļ�¼����								
****************************************************/
static uint8 dva_get_record_status(void)
{
    return record_status;
}

//��������豸��
dva_dev_t sh9_dva_dev=
{
	//dva_init,
//	dva_proc,
	
	#if NAME_RULE_NEW   
	dva_play_list_new,
	#else
	dva_play_list,
	#endif
	dva_play_special,
	dva_stop,
	dva_read_version,
	dva_read_status,
	dva_set_volume,
	dva_get_playing,
	dva_get_record_status,
	dva_clear,
	//dva_get_play_status,
	0,//dva ����ͨ�Ŵ���
	0,//¼��ģ�����
};


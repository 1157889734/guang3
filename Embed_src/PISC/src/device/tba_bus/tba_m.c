//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �г�ͨ�����߹����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:tba_m.c

//*�ļ�����:�г�ͨ�����߹�����

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"

token_polling_table_t token_polling_pisc_table[pisc_token_num]=
{
	{0x11,0,0,0,0,0,100,0,0},//PISC
	{0x21,0,0,0,0,0,100,0,0},//PISC
};

token_polling_table_t token_polling_dcp_table[dcp_token_num]=
{
	{0x12,0,0,0,0,0,100,0,0},//DCP
	{0x22,0,0,0,0,0,100,0,0},//DCP
};

token_polling_table_t token_polling_lcu_table[lcu_token_num]=
{
	{0x13,0,0,0,0,0,100,0,0},//LCU
	{0x23,0,0,0,0,0,100,0,0},//LCU
	{0x33,0,0,0,0,0,100,0,0},//LCU
	{0x43,0,0,0,0,0,100,0,0},//LCU
	{0x53,0,0,0,0,0,100,0,0},//LCU
	{0x63,0,0,0,0,0,100,0,0},//LCU
};

token_polling_table_t token_polling_io_table[io_token_num]=
{
	{0x18,0,0,0,0,0,100,0,0},//IO
	{0x28,0,0,0,0,0,100,0,0},//IO
	{0x38,0,0,0,0,0,100,0,0},//IO
	{0x48,0,0,0,0,0,100,0,0},//IO
	{0x58,0,0,0,0,0,100,0,0},//IO
	{0x68,0,0,0,0,0,100,0,0},//IO
};


//token_polling_table_t token_polling_table[TBA_M_TABLE_NUM]=

/*
{
	{0x11,300,0,0,0,0,100,0,0},
	{0x21,300,0,0,0,0,100,0,0},

	{0x13,300,0,0,0,0,100,0,0},//LCU
	{0x23,300,0,0,0,0,100,0,0},//LCU
	{0x33,300,0,0,0,0,100,0,0},//LCU
	{0x43,300,0,0,0,0,100,0,0},//LCU
	{0x53,300,0,0,0,0,100,0,0},//LCU
	{0x63,300,0,0,0,0,100,0,0},//LCU
	{0x12,300,0,0,0,0,100,0,0},//DCP
	{0x22,300,0,0,0,0,100,0,0},//DCP
	{0x17,600,0,0,0,0,100,0,0},//DESET LED
	{0x27,600,0,0,0,0,100,0,0},//DESET LED
	{0x00,0,  0,0,0,0,0,0,0},
};
*/

//M״̬��
#define TBA_M_STATUS_DEAD		0	//����״̬
#define TBA_M_STATUS_SLEEP		1	//����״̬
#define TBA_M_STATUS_SENDTOKEN		2	//��������״̬
#define TBA_M_STATUS_WAITACK		3	//�ȴ�Ӧ��״̬



//���ͻ�����
static uint8 xdata tba_m_send_buf[TBA_BUF_SIZE];
static uint16 xdata tba_m_send_length=0;



//static uint8 tba_m_status=TBA_M_STATUS_SLEEP;//M״̬
//static uint8 tba_m_sleep_times=0;//���ߵĴ���
//static uint32 tba_no_packet_timer=0;//û���κ����ݰ��ļ�ʱ��
static uint16 tba_no_packet_interval=0;//û���κ����ݰ��ļ��
//static uint32 tba_m_waitack_timer=0;//M�ȴ�Ѳ��Ӧ��Ķ�ʱ��
//static uint8 token_index=0;//Ѳ���������---����ȫ�ֱ�����Ŀ���Ǳ�֤�����豸���ܱ�Ѳ�쵽
//static uint8 current_token_index=0;//��ǰѲ�������

static uint8 xdata my_net_id=0;//�Լ��������
static uint8 xdata my_dev_id=0;//�Լ����豸ID


uint8 send_token_cnt = 0;



//***********************************************************************************************************************
//*��������:�г����߷�������
//*����˵��:
//*ע������:
//*����˵��:
//***********************************************************************************************************************
void tba_m_send_token(uint8 dev_id)
{
	tba_packet_t xdata temp_send_packet;//�������ݰ��ṹ---��ʱ
	uint8 *xdata send_buf_p=tba_m_send_buf;//���շ��ͻ�������ָ��
	uint16 *xdata send_length_p=&tba_m_send_length;//���շ��ͳ��ȵ�ָ��
	uint8 * xdata byte_add_p=(uint8 *)&temp_send_packet;//�����ֽ��ۼӵ�ָ��
	uint16 xdata src_length;//Դ���ݳ���---����ת��
	uint8 xdata checksum=0;//У���
	
		
	//��Ŀ�ĵ�ַ
	temp_send_packet.dst_net_id=0x00;//Ŀ�������--�������ڵ�
	temp_send_packet.dst_dev_id=dev_id;//Ŀ���豸ID
	//��Դ��ַ
	temp_send_packet.src_net_id=0x00;//Դ�����--�������ڵ�
	temp_send_packet.src_dev_id=my_dev_id;//Դ�豸ID
	//������������
	temp_send_packet.bus_cmd.packet_type=TBA_CMD_TOKENPACKET;//������
	temp_send_packet.bus_cmd.token_free=0;//�Ƿ��ͷ�����
	temp_send_packet.bus_cmd.m_reserve=0;//M��Ԥ����־
	//�����ݳ���
	temp_send_packet.app_length=0;
	
	//��������ַ�������ת��һ��תһ���ͷ��ͻ����� ͬʱ����У��� ����ӷָ���
	src_length=sizeof(tba_packet_t)-TBA_BUF_SIZE+temp_send_packet.app_length;
	*send_length_p=0;
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//��ӷָ���
	while(src_length--)
	{
		//�ж��Ƿ��������ַ�
		if(*byte_add_p==TBA_PACKET_TAB)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
		}
		else if(*byte_add_p==TBA_PACKET_DLE)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
		}
		else
		{
			send_buf_p[(*send_length_p)++]=*byte_add_p;
		}
		checksum+=*byte_add_p;
		byte_add_p++;
	}
	checksum=0x55-checksum;
	//�ж�У����Ƿ��������ַ�
	if(checksum==TBA_PACKET_TAB)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
	}
	else if(checksum==TBA_PACKET_DLE)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
	}
	else
	{
		send_buf_p[(*send_length_p)++]=checksum;		
	}
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//��ӷָ���
	
	//����
	sh9_tba_bus_dev.tba_send = 0;  //�����ݷ���
	TBA_BUS_SEND_BUF(tba_m_send_buf,tba_m_send_length);	
	if(sh9_tms_dev.debug_flag==6)
	{
		ptu_print_buf(tba_m_send_buf, tba_m_send_length);
	}
	//for test
	//uart0_write_buf(tba_m_send_buf,tba_m_send_length);	
	//print_line("poll");
}

//***********************************************************************************************************************
//*��������:�г����߷��������������
//*����˵��:
//*ע������:
//*����˵��:
//***********************************************************************************************************************
#if 0
static void tba_m_send_clear(void)
{
	tba_packet_t xdata temp_send_packet;//�������ݰ��ṹ---��ʱ
	uint8 *xdata send_buf_p=tba_m_send_buf;//���շ��ͻ�������ָ��
	uint16 *xdata send_length_p=&tba_m_send_length;//���շ��ͳ��ȵ�ָ��
	uint8 * xdata byte_add_p=(uint8 *)&temp_send_packet;//�����ֽ��ۼӵ�ָ��
	uint16 xdata src_length;//Դ���ݳ���---����ת��
	uint8 xdata checksum=0;//У���
	
		
	//��Ŀ�ĵ�ַ
	temp_send_packet.dst_net_id=0x00;//Ŀ�������--�������ڵ�
	temp_send_packet.dst_dev_id=0xFF;//Ŀ���豸ID--�㲥��
	//��Դ��ַ
	temp_send_packet.src_net_id=0x00;//Դ�����--�������ڵ�
	temp_send_packet.src_dev_id=my_dev_id;//Դ�豸ID
	//������������
	temp_send_packet.bus_cmd.packet_type=TBA_CMD_TOKENCLEAR;//������
	temp_send_packet.bus_cmd.token_free=0;//�Ƿ��ͷ�����
	temp_send_packet.bus_cmd.m_reserve=0;//M��Ԥ����־
	//�����ݳ���
	temp_send_packet.app_length=0;
	
	//��������ַ�������ת��һ��תһ���ͷ��ͻ����� ͬʱ����У��� ����ӷָ���
	src_length=sizeof(tba_packet_t)-TBA_BUF_SIZE+temp_send_packet.app_length;
	*send_length_p=0;
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//��ӷָ���
	while(src_length--)
	{
		//�ж��Ƿ��������ַ�
		if(*byte_add_p==TBA_PACKET_TAB)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
		}
		else if(*byte_add_p==TBA_PACKET_DLE)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
		}
		else
		{
			send_buf_p[(*send_length_p)++]=*byte_add_p;
		}
		checksum+=*byte_add_p;
		byte_add_p++;
	}
	checksum=0x55-checksum;
	//�ж�У����Ƿ��������ַ�
	if(checksum==TBA_PACKET_TAB)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
	}
	else if(checksum==TBA_PACKET_DLE)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
	}
	else
	{
		send_buf_p[(*send_length_p)++]=checksum;		
	}
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//��ӷָ���
	
	//����
	TBA_BUS_SEND_BUF(tba_m_send_buf,tba_m_send_length);	
}

//***********************************************************************************************************************
//*��������:�г����߹���������
//*����˵��:
//*ע������:
//*����˵��:
//***********************************************************************************************************************

void tba_m_proc(tba_packet_t *packet)
{
	uint8 token_index_backup;//�����ϴε�Ѳ���λ��
	
	//ֻҪ�յ����˵����ݰ� ���û�����ݰ��Ķ�ʱ�� ������ߴ���
	if(packet && packet->src_dev_id!=my_dev_id)
	{
		tba_no_packet_timer=get_clock();
		tba_m_sleep_times=0;
	}


	//M��ͬ״̬�Ĵ���
	switch(tba_m_status)
	{
		//����״̬---ֻ���л�������״̬
		case TBA_M_STATUS_DEAD:
			if(!tba_m_sleep_times)
			{
				tba_m_status=TBA_M_STATUS_SLEEP;//�л�״̬��
				print_line("M alive");
			}
			break;
		
		//����״̬---����M���������Ƶ�״̬
		case TBA_M_STATUS_SLEEP:
			//�ܳ�ʱ��û���յ��κ����ݰ� ˵������M������ ����M���������Ƶ�״̬
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{
				//������� ��������״̬
				if(++tba_m_sleep_times>=5)
				{
					tba_m_status=TBA_M_STATUS_DEAD;//�л�״̬��
					print_line("M dead");
				}
				else
				{
					tba_m_status=TBA_M_STATUS_SENDTOKEN;//�л�״̬��
					tba_m_send_clear();//��������������Ƶ�����
					print_line("M wakeup");					
				}
			}
			break;
			
		//��������״̬---�л����ȴ�Ӧ��״̬ ����ܳ�ʱ��û���κ����ݰ��л�������״̬
		case TBA_M_STATUS_SENDTOKEN:
			//�ܳ�ʱ��û���յ��κ����ݰ�--�Լ���ë���� �л�M������״̬
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{			
				tba_m_status=TBA_M_STATUS_SLEEP;//�л�״̬��
				print_line("M sleep");
				break;
			}
			//������ ���������ҷ����ݻ��߱������ҵ�M
			if(packet)
			{
				//�жϰ�����
				switch(packet->bus_cmd.packet_type)
				{
					case TBA_CMD_TOKENPACKET://���ư�				
					case TBA_CMD_TOKENCLEAR://����������Ƶİ�
						//�Լ��������ƺ�������
						if(packet->src_dev_id==my_dev_id)
						{
							print_line("M my token");
						}
						else
						{
							//�л�״̬��
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM is reaved:",packet->src_dev_id);
						}
						break;
			
					default://������
						tba_m_status=TBA_M_STATUS_SLEEP;
						print_int("%xM send but no token:",packet->src_dev_id);
						break;
				}
				break;
			}
							
 			//���ϴ�Ѳ����λ�ð�����Ѳ����һȦ ��֤ÿ���豸���л���
 			token_index_backup=token_index;//�����ϴε�Ѳ���λ��
			do
			{
				//�ж�����豸��ʱ�䵽 ע�����poll_continue_timeout�ж�����Ѳ�쳬ʱ���豸���������ȼ�
				if( checktimer((uint32 *)&token_polling_table[token_index_backup].timer,token_polling_table[token_index_backup].interval+(uint16)token_polling_table[token_index_backup].poll_continue_timeout*TBA_M_ACK_TIMEOUT) )
				{
					tba_m_status=TBA_M_STATUS_WAITACK;//�л�״̬��
					current_token_index=token_index_backup;//���䵱ǰѲ�������
					tba_m_waitack_timer=get_clock();//���¿�ʼ��ʱ
					tba_m_send_token(token_polling_table[current_token_index].dev_id);//��������
					token_polling_table[current_token_index].poll_total++;//���������ͨ������ͳ�� ����Ѳ������
					if(token_polling_table[current_token_index].poll_total==0)
						token_polling_table[current_token_index].poll_error=0;//����Ժ�ȫ�����
					print_int("%xM give token to:",token_polling_table[current_token_index].dev_id);
					break;
				}
				//ָ����һ��Ѳ���豸
				token_index_backup++;
				//Ѳ���ת��һȦ�� ��ͷ��ʼ
				if(token_polling_table[token_index_backup].interval==0)
					token_index_backup=0;
			}while(token_index!=token_index_backup);
			//ָ����һ��Ѳ���豸
			token_index++;
			//Ѳ���ת��һȦ�� ��ͷ��ʼ
			if(token_polling_table[token_index].interval==0)
				token_index=0;
			break;
			
		//�ȴ�����Ӧ��״̬---����������л�������״̬ ����ܳ�ʱ��û���κ����ݰ��л�������״̬
		case TBA_M_STATUS_WAITACK:
			//�ܳ�ʱ��û���յ��κ����ݰ�--�Լ���ë���� �л�M������״̬
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{					
				tba_m_status=TBA_M_STATUS_SLEEP;//�л�״̬��
				print_line("M sleep");
				break;
			}
			//��ʱ��û���յ�����Ӧ�� �л�����������״̬����Ѳ����һ���豸
			if(checktimer(&tba_m_waitack_timer,TBA_M_ACK_TIMEOUT))
			{
				tba_m_status=TBA_M_STATUS_SENDTOKEN;//�л�״̬��
				token_polling_table[current_token_index].poll_error++;//���������ͨ������ͳ�� ���ӳɴ��������
				if(token_polling_table[current_token_index].poll_error==0)
					token_polling_table[current_token_index].poll_total=0;//����Ժ�ȫ�����
				if(token_polling_table[current_token_index].poll_continue_timeout<200)
					token_polling_table[current_token_index].poll_continue_timeout++;//����Ѳ�쳬ʱ���м��� 100��Ϊ���� ��������������н������ȼ�
                            //for test hk
				//tba_m_send_clear();//��������������Ƶ�����

				
				print_int("%xM timeout:",token_polling_table[current_token_index].dev_id);
				break;
			}
							
			//������ �ж��Ƿ�����ȷ����Ӧ��
			if(packet)
			{
				//�жϰ�����
				switch(packet->bus_cmd.packet_type)
				{						
					case TBA_CMD_TOKENPACKET://���ư�
					case TBA_CMD_TOKENCLEAR://����������Ƶİ�
						//�Լ��������ƺ�������
						if(packet->src_dev_id==my_dev_id)
						{
							print_line("M my token");
						}
						else
						{
							//�л�״̬��
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM is reaved:",packet->src_dev_id);
						}
						break;
			
					default://������
						//�豸��ȷӦ�� Դ�豸�ŵ��ڸո�Ѳ����豸��ַ ����Դ����ŵ��ڸո�Ѳ����豸��ַ--����LCUת�������ڲ�������
						if(token_polling_table[current_token_index].dev_id==packet->src_dev_id || token_polling_table[current_token_index].dev_id==packet->src_net_id)
						{
							//�л�״̬��---�л����Ʋ����л�(������������)
							if(packet->bus_cmd.token_free)
							{
								tba_m_status=TBA_M_STATUS_SENDTOKEN;
								//print_int("%xM token free:",packet->src_dev_id);
								if(get_clock()-tba_m_waitack_timer>=TBA_M_WARNING_TIMEOUT)
								{
									print_int("%xdev_id:",token_polling_table[current_token_index].dev_id);
									print_int("%dM ack time:",get_clock()-tba_m_waitack_timer);
								}
							}
							else
							{
								print_int("%xM continue send:",packet->src_dev_id);
							}
							tba_m_waitack_timer=get_clock();//���¿�ʼ��ʱ
							token_polling_table[current_token_index].poll_continue_timeout=0;//�����ǰ�豸����Ѳ�쳬ʱ����---�ָ����ȼ�
						}
						else
						{
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM send but no token:",packet->src_dev_id);
						}
						break;
				}				
			}
			break;
		
		//����״̬
		default:
			break;
	}
	
}
#endif
//***********************************************************************************************************************
//*��������:����Ѳ���
//*����˵��:
//*ע������:
//*����˵��:
//***********************************************************************************************************************
void tba_m_set_table(uint8 *dev_id, uint16 *interval)
{
	return;
	/*
	uint16 index=0;
	
	while(*dev_id)
	{
		token_polling_table[index].dev_id=*dev_id++;
		token_polling_table[index].interval=*interval++;
		index++;
		if(index >= TBA_M_TABLE_NUM-1)
			break;
	}
	//�ͽ�����
	token_polling_table[index].interval=0;
	//�����ǰ��������
	token_index=0;
	current_token_index=0;*/
}

//***********************************************************************************************************************
//*��������:��ѯͨ������
//*����˵��:
//*ע������:
//*����˵��:�豸����
//***********************************************************************************************************************
uint8 tba_m_get_quality(uint8 *dev_id, uint16 *total, uint16 *error)
{
	/*
	uint16 index=0;
	
	while(token_polling_table[index].interval)
	{
		*dev_id++=token_polling_table[index].dev_id;
		*total++=token_polling_table[index].poll_total;
		*error++=token_polling_table[index].poll_error;
		index++;
	}
	return index;*/
}

//***********************************************************************************************************************
//��������:��������ʼ������
//����˵��:net_id---�Լ��������  dev_id---�Լ����豸ID
//ע������:
//����˵��:
//***********************************************************************************************************************
void tba_m_init(uint8 net_id, uint8 dev_id)
{	
	//�����Լ��������
	my_net_id=net_id;
	//�����Լ����豸ID
	my_dev_id=dev_id;
	//�����豸�Ų���--û���κ����ݰ��ļ��
	tba_no_packet_interval=(my_dev_id)*TBA_M_ACK_TIMEOUT+1000;
	//���õ�M
	tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
}



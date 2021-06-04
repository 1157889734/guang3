/******************************************************************************
*�ļ����� ��tba_bus.c		       		         
*��          �� ��thomson   							 
*��          �� ���г����ߴ���		
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/


#include "include.h"

#define BUS_TIME_OUT  15000      //�г�485����û���յ����ݣ����л�����״̬
#define BUS_TIME_OUT_MS  7000 	//�������л�������ͣ������7.5S
#define BUS_TIME_OUT_MS_RECON  2000 
#define BUS_DATA_ERROR_TIME_OUT  15000 

#define PISC_DEV_ID  		1
#define DCP_DEV_ID  			2
#define LCU_DEV_ID  			3
#define IO_DEV_ID  			8

#define POLL_TOTAL  			15       	//Ѳ�����
#define POLL_INTERVAL_MAX		30000 		//Ѳ��ʱ����



//���ջ�����
static uint8   tba_bus_recv_buf[TBA_BUF_SIZE];
static uint16 tba_bus_recv_length=0;
//���ͻ�����
static uint8   tba_bus_default_send_buf[TBA_BUF_SIZE];
static uint16 tba_bus_default_send_length=0;
static uint8   tba_bus_p2p_send_buf[TBA_BUF_SIZE];
static uint16 tba_bus_p2p_send_length=0;

static uint8   bus_cmd = 0;

uint32 bus_data_error_timer = 0;
//˽�б���
static tba_packet_t * tba_recv_packet=(tba_packet_t *)&tba_bus_recv_buf;//�������ݰ���ָ��
static uint8 have_token=0;//�Ƿ�õ�����
static uint8 table_token=0;//�Ƿ�õ�����
static uint8 my_net_id=0;//�Լ��������
static uint8 my_dev_id=0;//�Լ����豸ID
static uint8 m_dev_id=0;//M���豸ID
static uint8 is_default_send=0;//�Ƿ��й�������Ҫ����
static uint8 is_p2p_send=0;//�Ƿ��е�Ե�����Ҫ����
static uint8 token_free_flag=1;//�ͷ����Ƶı�־ ȱʡΪ�ͷ�����
static uint16 comm_error_start_timer=0;//ͨ�Ŵ���ʼ��ⶨʱ��
static uint8 tba_bus_debug=0;//���Ա�־

static uint8 dcp_setvolume_times=0;
static uint8 dcp_volume_num=0;

static uint32 table_token_timer= 0;	

//extern uint8 pisc_status;
uint8 bus_data_error_cont = 0 ;
extern uint8 test_uart1_irq_flag;
extern uint16 MAX485_CTRL_REG_D ; 
//extern uint8 recv_data_flag_hd ;
//�����豸���ݴ����б�
__packed typedef struct
{
	uint8 dev_type;//�豸����
	void (*dev_proc)(uint8, uint8, uint8, uint8 *, uint8);//�豸���ݴ���
}dev_proc_list_t;
static dev_proc_list_t const dev_proc_list[]=
{
	{TBA_DEV_PISC,pisc_proc},//���������
	{TBA_DEV_DCP,dcp_proc},//�㲥���ƺ�, sz24 maybe no device
	{TBA_DEV_LCU,lcu_proc},//���ҿ�����
	//{TBA_DEV_DSTLED,dstled_proc},//�յ�վ��, sz24 no this device
	{TBA_DEV_SETUP,setup_proc},//PC�������
	
	{0,NULL},//������
};


static uint8 tba_send_token(token_polling_table_t* table )
{
#if 0
	if(table.poll_flag == 0
			|| table.poll_interval_time >= 3)
	{
		tba_m_send_token(table.dev_id);		
		table.timer = get_clock();
		table.poll_flag = 1;
		table.poll_interval_time = 1;
		table.poll_total++;  //Ѳ���������
		send_token_cnt ++;
		table_token = 0 ;  // �ͷ�485����Ȩ���ȴ��ظ�ʱ����Ȩ��
		table_token_timer = get_clock(); // ���㷢��ʱ��
		return 1 ;
	}
	else
		table.poll_interval_time++;
#else
	if(table->poll_flag == 0
			|| table->poll_interval_time >= 3)
	{
		tba_m_send_token(table->dev_id);		
		table->timer = get_clock();
		table->poll_flag = 1;
		table->poll_interval_time = 1;
		table->poll_total++;  //Ѳ���������
		send_token_cnt ++;
		table_token = 0 ;  // �ͷ�485����Ȩ���ȴ��ظ�ʱ����Ȩ��
		table_token_timer = get_clock(); // ���㷢��ʱ��
		return 1 ;
	}
	else
		table->poll_interval_time++;
#endif
	return 0;

}

/*************************************************
Ѳ�촦��
*************************************************/
static void tba_token_handle(void)
{
	static uint8 token_index =0;
	static uint8 token_dcp_index =0;
	static uint8 token_lcu_index =0;
	static uint8 token_io_index =0;

	
	token_index++;
	if(token_index>10)
		token_index = 1;

	if((token_index %2)==1)//Ѳ��dcp
	{
		if(token_dcp_index>=dcp_token_num)
			token_dcp_index = 0;
		
		while(token_dcp_index<dcp_token_num)
		{						
			if((get_clock() - token_polling_dcp_table[token_dcp_index].timer) 
			> token_polling_dcp_table[token_dcp_index].interval) // Ѳ��ʱ����
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_dcp_table[token_dcp_index]))
				{
					token_dcp_index++;
					break;
				}
			}	
			token_dcp_index++;
		}			
	}
	else if(token_index == 2 ||token_index == 6) // Ѳ��lcu
	{
		if(token_lcu_index>=lcu_token_num)
			token_lcu_index = 0;

		while(token_lcu_index<lcu_token_num)
		{						
			if((get_clock() - token_polling_lcu_table[token_lcu_index].timer) 
			> token_polling_lcu_table[token_lcu_index].interval) // Ѳ��ʱ����
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_lcu_table[token_lcu_index]))
				{
					token_lcu_index++;
					break;
				}
			}	
			token_lcu_index++;
		}
	}
	
	else if(token_index == 4 ||token_index == 8) // Ѳ��pisc
	{						
		if(read_pisc_id()==1)  //��Ѳ���Լ�
		{
			if((get_clock() - token_polling_pisc_table[1].timer) 
			> token_polling_pisc_table[1].interval) // Ѳ��ʱ����
				tba_send_token((token_polling_table_t*)&token_polling_pisc_table[1]);	
		}
		else
		{
			if((get_clock() - token_polling_pisc_table[0].timer) 
			> token_polling_pisc_table[0].interval) // Ѳ��ʱ����
				tba_send_token((token_polling_table_t*)&token_polling_pisc_table[0]);
		}	
	}
	else if(token_index == 10) // Ѳ��io
	{
		if(token_io_index>=io_token_num)
			token_io_index = 0;
		while(token_io_index<io_token_num)
		{						
			if((get_clock() - token_polling_io_table[token_io_index].timer) 
			> token_polling_io_table[token_io_index].interval) // Ѳ��ʱ����
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_io_table[token_io_index]))
				{
					token_io_index++;
					break;
				}
			}	
			token_io_index++;
		}
	}
}


/**************************************************

�㲥���ƺ���������

**************************************************/

static void dcp_volume_handle (void)
{
	dcp_setvolume_times++;  //ÿ�������ķ��ʹ���
	//print_int("%x dcp:", dcp_volume_num);
	switch(dcp_volume_num)
	{
		case 0:
			if((saveparam.dcp_pa_listen_volume != (dcp_default_data[0].dcp_pacc_listen &0x0f))
				||(saveparam.dcp_pa_listen_volume != (dcp_default_data[1].dcp_pacc_listen &0x0f)))
			{
				ptu_print_debug("d1");
				//dcp_volume_num = 1;
				my_default_data.dcp_volume = ((1<<4)&0Xf0)|saveparam.dcp_pa_listen_volume ;
			}
			
			
			break;
			
		case 1:
			if((saveparam.dcp_cc_listen_volume != ((dcp_default_data[0].dcp_pacc_listen>>4) &0x0f))
				||(saveparam.dcp_cc_listen_volume != ((dcp_default_data[1].dcp_pacc_listen>>4) &0x0f)))
			{
				ptu_print_debug("d2");
				//dcp_volume_num = 2;
				my_default_data.dcp_volume = ((2<<4)&0Xf0)|saveparam.dcp_cc_listen_volume ;
			}
			break;
	
		case 2:
			if((saveparam.dcp_pc_listen_volume != (dcp_default_data[0].dcp_pcpa_listen &0x0f))
				||(saveparam.dcp_pc_listen_volume != (dcp_default_data[1].dcp_pcpa_listen &0x0f)))
			{
				ptu_print_debug("d3");
				//dcp_volume_num = 3;
				my_default_data.dcp_volume = ((3<<4)&0Xf0)|saveparam.dcp_pc_listen_volume ;
			}
			break;

		case 3:
			if((saveparam.dcp_pa_mic_volume != ((dcp_default_data[0].dcp_pcpa_listen>>4) &0x0f))
				||(saveparam.dcp_pa_mic_volume != ((dcp_default_data[1].dcp_pcpa_listen>>4) &0x0f)))
			{
				ptu_print_debug("d4");
				//dcp_volume_num = 4;
				my_default_data.dcp_volume = ((4<<4)&0Xf0)|saveparam.dcp_pa_mic_volume ;
			}
			break;

		case 4:
			 if((saveparam.dcp_cc_mic_volume != (dcp_default_data[0].dcp_ccpc_mic&0x0f))
				||(saveparam.dcp_cc_mic_volume != (dcp_default_data[1].dcp_ccpc_mic &0x0f)))
			{
				ptu_print_debug("d5");
				//dcp_volume_num = 5;
				my_default_data.dcp_volume = ((5<<4)&0Xf0)|saveparam.dcp_cc_mic_volume ;
			}
			break;

		case 5:
			if((saveparam.dcp_pc_mic_volume != ((dcp_default_data[0].dcp_ccpc_mic >>4)&0x0f))
				||(saveparam.dcp_pc_mic_volume != ((dcp_default_data[1].dcp_ccpc_mic >>4)&0x0f)))
			{
				ptu_print_debug("d6");
				//dcp_volume_num = 6;
				my_default_data.dcp_volume = ((6<<4)&0Xf0)|saveparam.dcp_pc_mic_volume ;
			}
			break;

		default:
			break;
	}

	if(dcp_setvolume_times>=2)
	{
		dcp_volume_num++;
		dcp_setvolume_times=0;	
		if(dcp_volume_num>=6)
		{
			ptu_print_debug("dp");
			dcp_volume_num=0;
			sh9_tba_bus_dev.dcp_volume_set= 0;
			my_default_data.dcp_volume = 0;
		}
	}

#if 0
	if((saveparam.dcp_pa_listen_volume != (dcp_default_data[0].dcp_pacc_listen &0x0f))
		||(saveparam.dcp_pa_listen_volume != (dcp_default_data[1].dcp_pacc_listen &0x0f)))
	{
		print_line("d1");
		dcp_volume_num = 1;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pa_listen_volume ;
	}	
	else if((saveparam.dcp_cc_listen_volume != ((dcp_default_data[0].dcp_pacc_listen>>4) &0x0f))
			||(saveparam.dcp_cc_listen_volume != ((dcp_default_data[1].dcp_pacc_listen>>4) &0x0f)))
	{
		print_line("d2");
		dcp_volume_num = 2;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_cc_listen_volume ;
	}
	else if((saveparam.dcp_pc_listen_volume != (dcp_default_data[0].dcp_pcpa_listen &0x0f))
		||(saveparam.dcp_pc_listen_volume != (dcp_default_data[1].dcp_pcpa_listen &0x0f)))
	{
		print_line("d3");
		dcp_volume_num = 3;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pc_listen_volume ;
	}
	else if((saveparam.dcp_pa_mic_volume != ((dcp_default_data[0].dcp_pcpa_listen>>4) &0x0f))
		||(saveparam.dcp_pa_mic_volume != ((dcp_default_data[1].dcp_pcpa_listen>>4) &0x0f)))
	{
		print_line("d4");
		dcp_volume_num = 4;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pa_mic_volume ;
	}
	else if((saveparam.dcp_cc_mic_volume != (dcp_default_data[0].dcp_ccpc_mic&0x0f))
		||(saveparam.dcp_cc_mic_volume != (dcp_default_data[1].dcp_ccpc_mic &0x0f)))
	{
		print_line("d5");
		dcp_volume_num = 5;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_cc_mic_volume ;
	}
	else if((saveparam.dcp_pc_mic_volume != ((dcp_default_data[0].dcp_ccpc_mic >>4)&0x0f))
		||(saveparam.dcp_pc_mic_volume != ((dcp_default_data[1].dcp_ccpc_mic >>4)&0x0f)))
	{
		print_line("d6");
		dcp_volume_num = 6;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pc_mic_volume ;
	}
	else
		dcp_volume_num = 7;	
#endif
}

/****************************************************
*��������	tba_bus_send_data		         
*���ߣ�thomson             							 
*�������г�����Ӧ�ò㷢������
*���룺dst_net_id       --- Ŀ������ID
                 dst_dev_id       ---Ŀ���豸ID
                 data_type        ---���ݰ�����
                 token_free       ---�ͷ����Ʊ�־
                  *data_buf      ---Ӧ������Ŀ��ָ��
                  length    ---���ݳ���
                  
*�������	
*����ֵ�����ͳɹ�����1 ������������0
*�޸ļ�¼����								
****************************************************/
static uint8 tba_bus_send_data(uint8 dst_net_id, uint8 dst_dev_id, uint8 data_type, uint8 token_free, uint8 *data_buf, uint8 length)
{
	tba_packet_t temp_send_packet;//�������ݰ��ṹ---��ʱ
	uint8 *xdata send_buf_p;//���շ��ͻ�������ָ��
	uint16 *xdata send_length_p;//���շ��ͳ��ȵ�ָ��
	uint8 * byte_add_p=(uint8 *)&temp_send_packet;//�����ֽ��ۼӵ�ָ��
	uint16 src_length;//Դ���ݳ���---����ת��
	uint8 checksum=0;//У���

	//�����һ�ε�Ե㻹û�еõ����Ʒ��ͳ�ȥ �����Ҫ���͵�Ե� ����ʧ��		
	if(data_type==TBA_CMD_P2PPACKET && is_p2p_send)
		return 0;
	
	//��Ŀ�ĵ�ַ
	temp_send_packet.dst_net_id = dst_net_id;//Ŀ�������
	temp_send_packet.dst_dev_id = dst_dev_id;//Ŀ���豸ID
	//��Դ��ַ---����Ŀ��������Ƿ����0 �����Դ�����
	if(temp_send_packet.dst_net_id)
		temp_send_packet.src_net_id = my_net_id;//�������
	else
		temp_send_packet.src_net_id = 0x00;//�������ڵ�
	temp_send_packet.src_dev_id = my_dev_id;//Դ�豸ID
	//������������
	temp_send_packet.bus_cmd.packet_type = data_type;//������
	temp_send_packet.bus_cmd.token_free = token_free;//�Ƿ��ͷ�����
	if(token_free)
		token_free_flag=1;//����ͷ����� ��������Ƶı�־
	else
		token_free_flag=0;//������ͷ����� ������Ƶı�־=0
	temp_send_packet.bus_cmd.m_reserve=0;//M��Ԥ����־
	//�����ݳ���
	temp_send_packet.app_length=length;
	//�ж���û��Ӧ�ò�����---ֻ������
	if( !temp_send_packet.app_length )
	{
		temp_send_packet.dst_net_id = 0x00;//ֻ������---Ŀ�����������Ϊ00
		temp_send_packet.src_net_id = 0x00;//ֻ������---Դ���������Ϊ00
		temp_send_packet.dst_dev_id = m_dev_id;//ֻ������---Ŀ���豸ID����ΪM���豸ID
	}
	//�ж���û��Ӧ�ò�����---��Ӧ�ò�����
	else
	{
		if(temp_send_packet.app_length>=255)
			temp_send_packet.app_length=255;//���ȳ����ж�
		memcpy( (uint8 *)&temp_send_packet.app_data, data_buf, temp_send_packet.app_length );//��������		
	}
	
	//�������������Ͳ�ͬ�Ļ�����	
	
	if(data_type == TBA_CMD_P2PPACKET)
	{
		send_buf_p = (uint8 *)&tba_bus_p2p_send_buf;
		send_length_p = &tba_bus_p2p_send_length;
		if(temp_send_packet.app_length)
			is_p2p_send=1;//��������ݲ����ͱ�־
		else
			is_p2p_send=0;//��������ݲ����ͱ�־
	}
	else if(data_type == TBA_CMD_DEFAULTPACKET)    //��������
	{
		send_buf_p = (uint8 *)&tba_bus_default_send_buf;
		send_length_p = &tba_bus_default_send_length;
		if(temp_send_packet.app_length)
			is_default_send=1;//��������ݲ����ͱ�־
		else
			is_default_send=0;
	}

	//��������ַ�������ת��һ��תһ���ͷ��ͻ����� ͬʱ����У��� ����ӷָ���
	src_length = sizeof(tba_packet_t) - TBA_BUF_SIZE + temp_send_packet.app_length;
	*send_length_p = 0;
	send_buf_p[(*send_length_p)++] = TBA_PACKET_TAB;//��ӷָ���
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
	
	return 1;
}

/****************************************************
*��������	tba_bus_get_packet   		         
*���ߣ�thomson             							 
*�������г����߻������
*���룺��                 
*�������	
*����ֵ��
*�޸ļ�¼����								
****************************************************/
static uint8 tba_bus_get_packet(void)
{
	uint8 temp;
	
	while( TBA_BUS_RECEIVE_CHAR(&temp) )
	{			
	       //uart0_write_char(temp);
		//�����ָ��� �ж��Ƿ������� ��ִ����ת�� Ȼ���ж�ת���Ժ��ܳ��Ⱥ�У���
		if(temp == TBA_PACKET_TAB)//7e ͷ
		{
		       if(bus_cmd == 0)
		       {
		           TBA_BUS_FIFO_BAKUP();
			    bus_cmd = 1;
		       }
			//�жϳ��� �ж��Ƿ�������
			else if(bus_cmd == 1 && (tba_bus_recv_length >= sizeof(tba_packet_t)-TBA_BUF_SIZE))  //7e β
			{
			       bus_cmd = 0;
				return 1;
			}
			else
			{
			    bus_cmd = 0;
			    TBA_BUS_FIFO_RESUME();
			}
			tba_bus_recv_length=0;
		}
		//�����ַ���ֱ���ͻ�����
		else
		{
		       if(bus_cmd == 1)
		       {
			tba_bus_recv_buf[tba_bus_recv_length] = temp;
			if( ++tba_bus_recv_length >= TBA_BUF_SIZE )
				tba_bus_recv_length = 0;
			//debug_print_int("%x", temp);
		       	}
		}
	}
	
	return 0;
}

/****************************************************
*��������tba_bus_proc       		         
*���ߣ�thomson             							 
*�������г����ߴ���
*���룺��                 
*�������	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void tba_bus_proc(void)
{
	uint8 index;
	static uint8 tba_send_flag = 0;
	static uint32 tba_send_delay = 0;

	//uint8 proc_cnt= 0;  //���㲥���ƺй������ݼ���
  // static uint8 current_index = 0;
	//static uint8 max = 12;
	uint8 cnt_tmp;
	static uint32 timer_check = 0;	
	uint32 token_reply = 0;
	uint8 is_get_recv=0;//�Ƿ�õ�������
	//uint8 dev_type_tmp = 0;
	uint8 dev_no_tmp = 0;
	
	
	/*************** 1.   �������ݰ�*****************/
	if(tba_bus_get_packet())
	{		
		tba_bus_dev->bus_timer = get_clock();       //485����������
		test_uart1_irq_flag  = 0;
		//�ж�����  �����ڵİ�, �㲥��, ���������������İ�
		if( tba_recv_packet->dst_net_id == 0x00 || tba_recv_packet->dst_net_id == 0xFF
		|| tba_recv_packet->dst_net_id == my_net_id )
		{
			uint8 checksum=0;
			uint16 src_length=0;//��ת��֮ǰ�ĳ���
			uint16 dst_length=0;//��ת���Ժ�ĳ���
			

			//��Ҫ��ת����PTU---����������ߵ��Ա�־
			if( (tba_recv_packet->dst_dev_id & 0x0F) == TBA_DEV_SETUP 
				|| tba_bus_debug == 2)
			{
				uart0_write_char(TBA_PACKET_TAB);
				uart0_write_buf(tba_bus_recv_buf, tba_bus_recv_length);
				uart0_write_char(TBA_PACKET_TAB);
			}

			if ((tba_bus_debug==5)||(tba_bus_debug==6))
			{
				ptu_print_tbabuf(tba_bus_recv_buf,tba_bus_recv_length);
			}
				
			//ִ����ת�� ������У���
			for(src_length=0; src_length<tba_bus_recv_length; src_length++)
			{
				//��������
				tba_bus_recv_buf[dst_length]=tba_bus_recv_buf[src_length];
				//�жϸոտ������Ƿ���ת���
				if(tba_bus_recv_buf[src_length]==TBA_PACKET_DLE && src_length<tba_bus_recv_length-1)
				{
					//�ж���һ���ַ��Ƿ��Ƿָ����ת��� �Ѹոտ�����ת�����ԭΪTBA_PACKET_TAB ��������һ���ַ�
					if(tba_bus_recv_buf[src_length+1]==TBA_PACKET_DLE_TAB)
					{
						tba_bus_recv_buf[dst_length]=TBA_PACKET_TAB;//�Ѹոտ�����ת�����ԭΪTBA_PACKET_TAB
						src_length++;//��������һ���ַ�
					}
					//�ж���һ���ַ��Ƿ���ת�����ת��� �Ѹոտ�����ת�����ԭΪTBA_PACKET_DLE ��������һ���ַ�
					else if(tba_bus_recv_buf[src_length+1]==TBA_PACKET_DLE_DLE)
					{
						tba_bus_recv_buf[dst_length]=TBA_PACKET_DLE;//�Ѹոտ�����ת�����ԭΪTBA_PACKET_DLE
						src_length++;//��������һ���ַ�
					}
					else
					{
						ptu_print_debug("tba 7f error");
						//���ݴ���----7fû�б���ȷת��
						dst_length=0;//���ת���Ժ�ĳ���
						break;
					}
				}
				checksum+=tba_bus_recv_buf[dst_length];
				dst_length++;
			}
			tba_bus_recv_length=0;//ת�����������ճ���
			//�ж�У���
			if(checksum == 0x55)
			{
				//�ж�ת���Ժ��ܳ��� ע������+1 ���������ֽ�
				if((tba_recv_packet->src_dev_id & 0x0F) == 0x08 ) // ˵�� IO ģ�� ������Ŀû��
				{
					if( (sizeof(tba_packet_t) - TBA_BUF_SIZE + tba_recv_packet->app_length + 2) == dst_length )
					{
						is_get_recv=1;
					}
					else
						print_line("IO error");
				}
				else
				{
					if( (sizeof(tba_packet_t) - TBA_BUF_SIZE + tba_recv_packet->app_length + 1) == dst_length )
					{	
						//���յ���������־
						is_get_recv=1;
					}
					else
					{
						ptu_print_debug("tba length error");
					}
				}
				//uart0_write_char(0xbb);
				//uart0_write_char(0xbb);
				//uart0_write_char(0x7e);
				//uart0_write_buf(tba_bus_recv_buf, dst_length);
				//uart0_write_char(0x7e);
			}
			else
			{
			       TBA_BUS_FIFO_RESUME();
				if(!checktimer(&bus_data_error_timer, BUS_DATA_ERROR_TIME_OUT))
				{
				   	
					bus_data_error_cont++;
					//print_line("bus_data_error_cont++");
				}
				else
				{
				   	bus_data_error_timer = get_clock();
					bus_data_error_cont = 0;
					//print_line("bus_data_error_cont=0");
				}
				if(bus_data_error_cont>=10)
				{
					bus_data_error_cont = 0;
					//pisc_status =  0;	//IDLE
				}
				ptu_print_debug("bus checksum error");
				//print_int("%x",tba_recv_packet->dst_dev_id);
				//print_int("%x",tba_recv_packet->src_dev_id);
				//uart0_write_char(0xaa);
				//	uart0_write_char(0xaa);
				//uart0_write_char(0x7e);
				//uart0_write_buf(tba_bus_recv_buf, dst_length);
				//uart0_write_char(0x7e);
			}			
		}
	}
	
	
	
	/*************** 2.   �������ݰ�*****************/	
	if(is_get_recv)//�ж�Ҫ�Ҵ���İ�
	{
		//�ж���Ҫ�Ҵ���İ�  ��ȫ�㲥��(0xFF) �豸�㲥��my_dev_id|0xF0=0xF1 ר�ŷ����ҵİ� 
		if( (tba_recv_packet->dst_dev_id == 0xFF) || (tba_recv_packet->dst_dev_id == (my_dev_id|0xF0) ) 
			|| (tba_recv_packet->dst_dev_id == my_dev_id) )
		{
	       have_token = 1;//�����Ʊ�־		
	       table_token = 1; //�յ��ظ�����������Ȩ��
			timer_check = get_clock();  // �յ��ظ���ʱ��
			led_toggle(TBA_BUS_LED_NO);//ͨ�ŵ�			
			switch( tba_recv_packet->bus_cmd.packet_type )//�жϰ�����
			{			      						
				case TBA_CMD_TOKENPACKET://���ư�					
					if(tba_recv_packet->dst_dev_id == my_dev_id)//�ж��Ƿ���ҵ�����
					{
						send_token_cnt = 0;
						//print_line("by polling");
						token_reply = get_clock();
						while(get_clock() - token_reply <=3);
						TBA_BUS_SEND_BUF(tba_bus_default_send_buf, tba_bus_default_send_length);
					}
					break;
										
				case TBA_CMD_TOKENCLEAR://����������Ƶİ�
					have_token = 0;//������Ʊ�־
						
					break;

				default://������
					//�����Լ����͵İ� ���жϳ���
					//if(tba_recv_packet->src_dev_id!=my_dev_id && tba_recv_packet->app_length)
					{
						  
						//�������豸���ദ��
						dev_proc_list_t const *list;
						for(list=dev_proc_list; list->dev_proc; list++)
						{
							//�ҵ��豸 �������ݴ�����
							if( (tba_recv_packet->src_dev_id & 0x0F) == list->dev_type )
							{		
								//�������ݴ�����
								list->dev_proc( tba_recv_packet->src_net_id,tba_recv_packet->src_dev_id,
												(uint8)tba_recv_packet->bus_cmd.packet_type,(uint8 *)&tba_recv_packet->app_data,
												tba_recv_packet->app_length );
								
								break;
							}
						}

					}
					break;		
			}
		}
	}
	
		
	
	/*************** 3.   ������*****************/	
	if(have_token && is_p2p_send)//����е�Ե�����Ҫ���� ���ȷ���
	{
		//���ڵ�Ե����� ������ͳɹ����־ ���ɹ�˵�������������´������·���
		if( TBA_BUS_SEND_BUF(tba_bus_p2p_send_buf,tba_bus_p2p_send_length) )
		{			
			is_p2p_send=0;//����������־
			
			if(token_free_flag)//�����������Ƶı�־ �������
				have_token=0;
		}
	}
	else if((send_token_cnt >= 2) && (get_clock()-timer_check>BUS_INTERVAL_TIME)&&table_token)//���͹�������---ֻ�����Ƶİ�Ҳ��������
	{					
		if(sh9_tba_bus_dev.dcp_volume_set)//add by suven for dcp volume set
		{
			dcp_volume_handle();
			//sh9_tba_bus_dev.dcp_volume_set=0;
		}		
		send_token_cnt =0;				

		sh9_tba_bus_dev.tba_send = 1;  //�����ݷ���
		TBA_BUS_SEND_BUF(tba_bus_default_send_buf, tba_bus_default_send_length);//���͹������� ���Է��ͽ��
		
	
		if(tba_bus_debug==3)//����������ݵ�uart0���������
		{
		    uart0_write_buf(tba_bus_default_send_buf, tba_bus_default_send_length);
		}

		if((tba_bus_debug==5)||(tba_bus_debug==7))
		{
			ptu_print_buf(tba_bus_default_send_buf, tba_bus_default_send_length);
			DEBUG_SEND_STR("\r\n");
		}
							
		have_token=0; //��������ǿ���������
	}

	
	

	/*************** 4.   ����Ѳ��*****************/	
	if( my_default_data.status_hbit.master == 1)
	{
		if((get_clock()-timer_check>BUS_INTERVAL_TIME) && (send_token_cnt<2)&& table_token )
			//&&(tba_send_flag == 0 && (get_clock()-tba_send_delay>PROCESS_TO_TOKEN_TIME)))
		{
			tba_token_handle();
			have_token=0; //�������
		}
	}

	#if 0
	if(tba_send_flag && (sh9_tba_bus_dev.tba_send==0))
	{
		tba_send_delay = get_clock();		
	}
	tba_send_flag = sh9_tba_bus_dev.tba_send ; 
	#endif 

	if(get_clock() - table_token_timer > TOKEN_INTERVAL_TIME) //�ջ�����
		table_token = 1; 
	
	switch(tba_recv_packet->src_dev_id & 0x0f)//�յ��ظ�������ϱ�־
	{
		case PISC_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < pisc_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_pisc_table[cnt_tmp].dev_id)
				{
					token_polling_pisc_table[cnt_tmp].poll_flag = 0;
					token_polling_pisc_table[cnt_tmp].poll_total= 0;  //���Ѳ�����
					token_polling_pisc_table[cnt_tmp].interval= 0;
				 	error_pack.status1 &= ~(1<<(cnt_tmp+6));	//�������λ
				 	break;
				}
			}				
			break;
			
		case DCP_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < dcp_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_dcp_table[cnt_tmp].dev_id)
				{
					token_polling_dcp_table[cnt_tmp].poll_flag = 0;
					token_polling_dcp_table[cnt_tmp].poll_total= 0;  //���Ѳ�����

					//��Կ�ײ�dcpɨ�������
					if(my_default_data.status_hbit.active
					   &&(read_pisc_id()==((tba_recv_packet->src_dev_id & 0xf0)>>4)))
						token_polling_dcp_table[cnt_tmp].interval = 0;
					else
						token_polling_dcp_table[cnt_tmp].interval = 200;
					
					error_pack.status2 &= ~(1<<(cnt_tmp+4));//�������λ		
					break;
				}
			}
			break;
			
		case LCU_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < lcu_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_lcu_table[cnt_tmp].dev_id)
				{
					token_polling_lcu_table[cnt_tmp].poll_flag = 0;
					token_polling_lcu_table[cnt_tmp].poll_total= 0;  //���Ѳ�����
					token_polling_lcu_table[cnt_tmp].interval = 0;
					error_pack.status1 &= ~(1<<(cnt_tmp));	//�������λ		
					break;
				}
			}
			break;
			
		case IO_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < io_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_io_table[cnt_tmp].dev_id)
				{
					token_polling_io_table[cnt_tmp].poll_flag = 0;
					token_polling_io_table[cnt_tmp].poll_total= 0;  //���Ѳ�����
					token_polling_io_table[cnt_tmp].interval = 0;
					error_pack.status3 &= ~(1<<(cnt_tmp+2));//�ù���λ
					break;
				}
			}
			break;
			
		default:
			break;
	}

	//�޻ظ����豸Ѳ��ʱ�����ӳ�
	for(cnt_tmp = 0; cnt_tmp < pisc_token_num; cnt_tmp++) // pisc
	{
		if(token_polling_pisc_table[cnt_tmp].poll_total > POLL_TOTAL)
		{	
			error_pack.status1 |= (1<<(cnt_tmp+6)); //�ù���λ
			token_polling_pisc_table[cnt_tmp].poll_total = POLL_TOTAL;

			/*
			token_polling_pisc_table[cnt_tmp].interval +=1000; 
			if(token_polling_pisc_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_pisc_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}*/
		}
	}	

	for(cnt_tmp = 0; cnt_tmp < dcp_token_num; cnt_tmp++)  // dcp
	{
		if(token_polling_dcp_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status2 |= (1<<(cnt_tmp+4));//�ù���λ
			token_polling_dcp_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_dcp_table[cnt_tmp].interval +=1000; 
			if(token_polling_dcp_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_dcp_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	for(cnt_tmp = 0; cnt_tmp < lcu_token_num; cnt_tmp++)  // lcu
	{
		if(token_polling_lcu_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status1 |= (1<<(cnt_tmp));//�ù���λ
			token_polling_lcu_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_lcu_table[cnt_tmp].interval +=1000; 
			if(token_polling_lcu_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_lcu_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	for(cnt_tmp = 0; cnt_tmp < io_token_num; cnt_tmp++)  // io
	{
		if(token_polling_io_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status3 |= (1<<(cnt_tmp+2));//�ù���λ
			token_polling_io_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_io_table[cnt_tmp].interval +=1000; 
			if(token_polling_io_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_io_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	
	
#if 0
	if(is_get_recv && (my_default_data.status_hbit.master == 1))
	{
		//tba_m_proc(tba_recv_packet);
		for(cnt_tmp = 0; cnt_tmp < POLL_NUM_MAX; cnt_tmp++)
		{
		    if(tba_recv_packet->src_dev_id == token_polling_table[cnt_tmp].dev_id)
		    {
		        token_polling_table[cnt_tmp].poll_interval_time = 0;
				 token_polling_table[cnt_tmp].poll_flag = 0;

				 //add by hk 2010-9-19
				 token_polling_table[cnt_tmp].poll_error = 0;

				 //Ѳ�쳬ʱ
				if((get_clock() - token_polling_table[cnt_tmp].timer) >= token_polling_table[cnt_tmp].time_out)
				{
					token_polling_table[cnt_tmp].poll_total++;
					//������ʱ3�Σ������ӳ�ʱʱ��
					if((token_polling_table[cnt_tmp].dev_id&0x0f)!=1
					&&token_polling_table[cnt_tmp].poll_total >= 3)
					{
						token_polling_table[cnt_tmp].time_out = token_polling_table[cnt_tmp].time_out + 100;
						token_polling_table[cnt_tmp].poll_total = 0;
						token_polling_table[cnt_tmp].interval = (get_clock() - token_polling_table[cnt_tmp].timer) + 100;
						print_int("%x time out",token_polling_table[cnt_tmp].dev_id);
					}
					if(token_polling_table[cnt_tmp].time_out > 30000)
					{
						token_polling_table[cnt_tmp].time_out = BUS_INTERVAL_TIME;
					}
				}
				else   //Ѳ��û�г�ʱ
				{
				    token_polling_table[cnt_tmp].poll_total = 0;	                           
				    if((get_clock() - token_polling_table[cnt_tmp].timer)<2*BUS_INTERVAL_TIME)  //����Ѳ�����ͳ�ʱʱ��
				    {
			    	    token_polling_table[cnt_tmp].interval = 2*BUS_INTERVAL_TIME;
				    }
				    else
			    	{
			    	    token_polling_table[cnt_tmp].interval = (get_clock() - token_polling_table[cnt_tmp].timer) + 100;
			    	}
				}

			    //begin  add by hk 2010-5-21   for ���Ƴ�ʱ����
			    dev_type_tmp = token_polling_table[cnt_tmp].dev_id&0x0f;
			    dev_no_tmp = token_polling_table[cnt_tmp].dev_id >> 4;
			    if(dev_type_tmp == 1)//pisc
			    {
			        error_pack.status1 &= ~(1<<(dev_no_tmp+5));
			    }
			    else if(dev_type_tmp == 2) //dcp
		    	{
		    	    error_pack.status2 &= ~(1<<(dev_no_tmp+3));
		    	}
			    else if(dev_type_tmp == 3)//lcu
		    	{
		    	    error_pack.status1 &= ~(1<<(dev_no_tmp-1));				    
		    	}				    
			    //end  add by hk 2010-5-21   for ���Ƴ�ʱ����
		    }			    
		}
	}
	//û���յ������� ��NULLָ��
	else
	{
		//tba_m_proc(NULL);
	}

	if( my_default_data.status_hbit.master == 1)
	{
	    cnt =0;
		if(checktimer(&timer_check,BUS_INTERVAL_TIME) && (send_token_cnt<2))
		{
		    while(current_index < POLL_NUM_MAX)
			{
				cnt++;
				if((get_clock() - token_polling_table[current_index].timer) > token_polling_table[current_index].interval)
				{
		             //û�з���Ѳ�����ƻ��ͼ���ѳ���3�Σ����ط�����
					if(token_polling_table[current_index].poll_flag == 0 || token_polling_table[current_index].poll_interval_time >= 3)
					{						              	              
						//��Ѳ���Լ�
						if(token_polling_table[current_index].dev_id == my_dev_id)
						{
						    current_index++;
							current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
							continue;
						}
		              //begin  add by hk 2010-5-21   for ���Ƴ�ʱ����
						if(token_polling_table[current_index].poll_interval_time >= 3)
						{
							token_polling_table[current_index].poll_error++;
							if(token_polling_table[current_index].interval < 50000)  //��ֹ���
							token_polling_table[current_index].interval += BUS_INTERVAL_TIME;
							if(token_polling_table[current_index].time_out < 60000)  //��ֹ���
							token_polling_table[current_index].time_out += BUS_INTERVAL_TIME; 
							if(token_polling_table[current_index].poll_error >= 200)
							{
							 token_polling_table[current_index].poll_error = 10;
							}
							//  print_int("%x time out dev_id ",token_polling_table[current_index].dev_id);
							//    print_int("%x poll_error  ",token_polling_table[current_index].poll_error);
						}  
			          //end  add by hk 2010-5-21   for ���Ƴ�ʱ����

						tba_m_send_token(token_polling_table[current_index].dev_id);
						// debug_print_int("%x no",token_polling_table[current_index].dev_id);
						token_polling_table[current_index].timer = get_clock();
						token_polling_table[current_index].poll_flag = 1;
						token_polling_table[current_index].poll_interval_time = 1;

						//debug_print_int("%d c_index", current_index);
						send_token_cnt++;
						current_index++;
						current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
					}
					else    //�ѷ���Ѳ��ָ���û���յ��ظ�
					{
						token_polling_table[current_index].poll_interval_time++;
						//token_polling_table[current_index].timer = get_clock();
						continue;
					}
					break;
				}
				//debug_print_int("%d current_index22", current_index);
			   current_index++;
				current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
				if(cnt == POLL_NUM_MAX)
					break;
			}
		}
	}
#endif

    //485�������ݳ�ʱ���
	if(checktimer(&tba_bus_dev->bus_timer, BUS_TIME_OUT))
	{
	    //other_default_data.test_status.slave_req = 1;    //485��������BUS_TIME_OUTʱ����û�����ݣ����豸�л�����
       //my_default_data.test_status.master_req = 1;
	    tba_bus_dev->data_flag = 1;
	    print_line("485 hava no data");
	}

	//����ͨ�Ŵ��������
	if(++comm_error_start_timer>=50000)
	{
		comm_error_start_timer=50000;
		
		for(index=0;index<TBA_PISC_NUM;index++)
		{
			if(++pisc_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				pisc_comm_error[index]=COMM_DATA_TIMEOUT;
				//memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
			}
		}
		for(index=0;index<TBA_DCP_NUM;index++)
		{
			if(++dcp_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				dcp_comm_error[index]=COMM_DATA_TIMEOUT;
				memset((uint8 *)&dcp_default_data[index],0,sizeof(dcp_default_data_t));
			
			}
		}
		for(index=0;index<TBA_LCU_NUM;index++)
		{
			if(++lcu_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				lcu_comm_error[index]=COMM_DATA_TIMEOUT;
				//memset((uint8 *)&lcu_default_data[index],0,sizeof(lcu_default_data_t));
				//print_line("memset lcu data");print_int("%d index ", index);
			}
		}
	}
}

/****************************************************
*��������tba_bus_init       		         
*���ߣ�thomson             							 
*���������߳�ʼ������
*���룺��                 
*�����net_id---�Լ��������  dev_id---�Լ����豸ID
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void tba_bus_init(uint8 net_id, uint8 dev_id)
{
	//�����Լ��������
	my_net_id=net_id;
	//�����Լ����豸ID
	my_dev_id=dev_id;
	
	//���߹�������ʼ��
	tba_m_init(my_net_id,my_dev_id);
	print_int("%x dev_id", my_dev_id);
	
	//��ʼ��ͨ�Ŷ˿�
	TBA_BUS_PORTINIT(TBA_BUS_BAUDRATE);
}

/****************************************************
*��������tba_bus_set_debug       		         
*���ߣ�thomson             							 
*���������õ��Ա�־
*���룺��                 
*�����flag---���Ա�־
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void tba_bus_set_debug(uint8 flag)
{
	tba_bus_debug=flag;
}



//��������豸��
tba_bus_dev_t sh9_tba_bus_dev=
{
	tba_bus_init,
	tba_bus_proc,
	tba_bus_send_data,
	tba_bus_set_debug,
	0,
	0,
	0,
	0,
};



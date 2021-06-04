
/******************************************************************************
*�ļ����ƣ�cab_bus.c		       		         
*��          �ߣ�hk           							 
*��          ����˾����ͨ�����߽ӿڴ���		
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/
#include "include.h"

#define SEND_DEFAULT	0
#define SEND_SPECIAL	1
//#define DVA_DEVICE_NO			0x01   //DVA ģ���豸��
//#define DEBUG
      
//���ջ�����
static uint8 cab_bus_recv_buf[CAB_BUF_SIZE];
//static uint16 cab_bus_recv_length;
static uint8 start_flag=0;
static uint8 recv_length=0;
static uint8 cab_recv_sum=0;

//���ͻ�����
static uint8 cab_default_send_buf[CAB_BUF_SIZE];
static uint8 cab_special_send_buf[CAB_BUF_SIZE];
static uint16 cab_bus_send_length=0;
//extern uint8 detect_dva_flag;
//���ݷ��ͱ�־
static uint8 special_data_send=0;		//�������ݷ��ͱ�־λ��
//static uint8 default_data_send=0;		//�������ݷ��ͱ�־λ

static uint8 cab_bus_debug = 0;

//��ѯDVA��ʱ��
static uint32 dva_check_status_timer=0;

//static uint8 cab_recv_data_lenth=0;
//�豸�����б�ṹ
__packed typedef struct 
{
	uint8 device_type;
	void (*proc)(uint8,uint8,uint8 *);
}cab_proc_list_t;

static cab_bus_packet_t *cab_recv_packet=(cab_bus_packet_t *)cab_bus_recv_buf;

//�豸�����б�
static cab_proc_list_t  const cab_device_proc_list[]=
{
	{CAB_DVA,dva_proc},
	//{CAB_PANEL,panel_proc},
	//{CAB_CCTV,cctv_proc},
	{0,NULL},
};

/****************************************************
*��������cab_bus_send_data			       		         
*���ߣ�hk             							 
*������˾�������߷�������
*���룺device_type        ---�豸����
                  device_no          ---�豸���
                  data_type          ---��������
                  data_length        ---���ݳ���
                  buf                    ---���ݻ�����
*����ֵ��1:succeed   0:failed
*�޸ļ�¼����								
****************************************************/
static uint8 cab_bus_send_data(uint8 device_type,uint8 device_no,uint8 data_type,uint8 data_length, uint8 *buf)
{
    cab_bus_packet_t cab_send_packet;				//��������
    uint8 temp = 0;
	uint8 *send_data_p;								//��������ָ��
	//uint8 *send_length_p=&cab_bus_send_length;                           //�������ݳ���ָ��
	#if DVA_OLD //old dva
	uint8 *add_data_p=(uint8 *)&cab_send_packet.cab_data;
	#endif
	#if DVA_NEW //new dva
	uint8 *add_data_p=(uint8 *)&cab_send_packet.device_type;
	#endif
	
	uint8 checksum=0;
	if(data_type==SEND_SPECIAL&&special_data_send)
		return 0; //��������δ����������0
	
	cab_bus_send_length=0;

	#if DVA_OLD //old dva
	cab_send_packet.data_length=data_length;
	#endif
	#if DVA_NEW //new dva
	cab_send_packet.data_length=data_length+2;
	cab_send_packet.device_type=device_type;
	cab_send_packet.device_no=device_no;
//cab_send_packet.device_no=0x0f;
	#endif
	//cab_send_packet.device_type=device_type;
	//cab_send_packet.device_no=device_no;
	
	memcpy((uint8 *)&cab_send_packet.cab_data,buf,data_length);				//�������ݵ��������ݰ�
	
	if(data_type==SEND_SPECIAL)
	{
		send_data_p=cab_special_send_buf;
		special_data_send=1;
	}
	else if(data_type==SEND_DEFAULT)
	{	
		send_data_p=cab_default_send_buf;
		
	}
	#if DVA_NEW     //add later   ��ǰ20���ֽڼ�0
	temp = 0;
	 while(temp < 10)
	 {
	     send_data_p[temp] = 0x00;
	     temp++;
	 }
	 cab_bus_send_length = 0;
	#endif
	
	send_data_p[cab_bus_send_length++]=CAB_PACKET_TAB;
	send_data_p[cab_bus_send_length++]=cab_send_packet.data_length;
	#if DVA_NEW     //add later   ��ǰ20���ֽڼ�0
	while(cab_bus_send_length<cab_send_packet.data_length+2)
	#endif
	#if DVA_OLD
	while(cab_bus_send_length<cab_send_packet.data_length+2)
	#endif
	{
		send_data_p[cab_bus_send_length++]=*add_data_p;
		checksum+=*add_data_p;
		add_data_p++;
	}
	send_data_p[cab_bus_send_length++] = checksum;
	send_data_p[cab_bus_send_length++] = 0x0d;
	

	return 1;
}

/****************************************************
*��������cab_bus_get_packet			       		         
*���ߣ�hk             							 
*������˾�������߽�������,������cab_bus_recv_buf��������
*���룺device_type        ---�豸����
                  device_no          ---�豸���
                  data_type          ---��������
                  data_length        ---���ݳ���
                  buf                    ---���ݻ�����
*����ֵ��1:succeed   0:failed
*�޸ļ�¼����								
****************************************************/
static uint8 cab_bus_get_packet(void)
{
	uint8 temp;
	while(CAB_BUS_RECV_CHAR(&temp))
	{//uart0_write_char(temp);
		if(start_flag==0)
		{
			if(temp==CAB_PACKET_TAB)
			{
				start_flag++;
				recv_length=0;
				cab_recv_sum=0;
				CAB_BUS_FIFO_BACKUP();
			}
		}
		else if(start_flag==1)             //�������ݳ���
		{
			start_flag++;
			cab_bus_recv_buf[recv_length++]=temp;  
		}
		else if(start_flag>1)
		{
			if(recv_length<(cab_recv_packet->data_length+1))
			{
				cab_bus_recv_buf[recv_length++]=temp;        //��������
				cab_recv_sum+=temp;                           //����У���
			}
			else
			{
				start_flag=0;
				if(cab_recv_sum==temp)        //�ж�У����Ƿ���ȷ
				{
					return 1;
				}
				else
				{
					CAB_BUS_FIFO_RESUME();
				}
			}
		}
	}
	
	return 0;
}

/****************************************************
*��������cab_bus_get_packet			       		         
*���ߣ�hk             							 
*������˾�������ߴ���,�����豸���͵�����Ӧ�Ĵ�����
*���룺��	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void cab_bus_proc(void)
{	
//       static uint8 first_time = 0;    //dva ������
	static cab_proc_list_t const *list;				//�豸�����б�
	static uint8 cnt = 0;
	
	uint8 buf[CAB_BUF_SIZE]={0};
	//cab_bus_debug =3;//test
	if(cab_bus_get_packet())       //����յ����ݣ�����cab_bus�Ͻ��յ�������
	{
	       #if DVA_NEW   //new dva
		for(list=cab_device_proc_list;list->proc;list++)
		{
		      //�����豸���ͣ�������Ӧ���豸������
			if(list->device_type==cab_recv_packet->device_type)
				list->proc(cab_recv_packet->device_no,(cab_recv_packet->data_length-2),(uint8 *)&cab_recv_packet->cab_data);	
		}
		#endif
		#if DVA_OLD   //old dva
		dva_proc(0,(cab_recv_packet->data_length),(uint8 *)&cab_recv_packet->cab_data);
		#endif
		//uart0_write_buf((uint8 *)&cab_recv_packet->cab_data, (cab_recv_packet->data_length));
		//print_buf((uint8 *)&cab_recv_packet->cab_data, (cab_recv_packet->data_length));
		//��ӡ������Ϣ
		if((cab_bus_debug == 2) || (cab_bus_debug == 3))
             {
                 uart0_write_char(0x7e);
                 uart0_write_buf((uint8 *)&cab_recv_packet->data_length, (cab_recv_packet->data_length+1));
		    uart0_write_char(0x7e);
		}
	}
	//if(dva_dev->dva_get_play_status() ==1)
	if(special_data_send)
	{
	     dva_check_status_timer = timer0_get_clock();
	     debug_print_line("b");
	}
	//ÿ100ms��ѯһ��DVA״̬
	buf[0]=0x05;
	buf[1]=0;
	//if(checktimer(&dva_check_status_timer,1100) && special_data_send ==0)
	if(checktimer(&dva_check_status_timer,150))
	{
		//cab_bus_dev->send_data(CAB_DVA,DVA_DEVICE_NO,1,1,buf);
		//sh9_dva_dev.comm_error++;
		//debug_print_line("a");

		if(cnt == 30)//Ѳ��¼��ģ��
		{
		    cab_bus_dev->send_data(CAB_DVA,RECORD_DEVICE_NO,1,1,buf);
		    sh9_dva_dev.record_error++;
		    if(sh9_dva_dev.record_error>COMM_DVA_TIMEOUT)
			sh9_dva_dev.record_error=COMM_DVA_TIMEOUT;
		    cnt = 0;

		    if(sh9_dva_dev.record_error >= COMM_DVA_TIMEOUT)
		    {		  	
			my_default_data.cab_status.record_error = 1;			
		    }
		    else
		    {
		    	my_default_data.cab_status.record_error = 0;
	           }
		}
		
		else	if((cnt%2) == 0) //Ѳ��dva
		{
			cab_bus_dev->send_data(CAB_DVA,DVA_DEVICE_NO,1,1,buf);
			sh9_dva_dev.comm_error++;
			if(sh9_dva_dev.comm_error > COMM_DVA_TIMEOUT)
				sh9_dva_dev.comm_error = COMM_DVA_TIMEOUT;

                	my_default_data.cab_status.dva_error = 0;
		   	//dva ��ʱ��û�лظ����dva״̬�Ͳ����б�
		  	 if(dva_dev->comm_error > (COMM_DVA_TIMEOUT/2))
		   	{
		   	   	 dva_dev->dva_clear();
		   	}
		   
			//dva error
		   	 if((dva_dev->comm_error>=COMM_DVA_TIMEOUT) || (apu_error >= 40))
		    	{
		    	    	my_default_data.cab_status.dva_error = 1;
			   	// print_line("dva timeout");
		    	}
		  // debug_print_line("dva");
		}
		cnt++;
	}

	//��������
	if(special_data_send)        
	{
		special_data_send=0;
		CAB_BUS_SEND_BUF(cab_special_send_buf,cab_bus_send_length);

             //��ӡ������Ϣ
		if((cab_bus_debug == 1)|| (cab_bus_debug == 3))
             {
                 uart0_write_buf(cab_special_send_buf,cab_bus_send_length);
		}
		debug_print_line("6");
	}
	
	//ͨ�Ŵ�����
	/*if(++comm_err_start_timer>50000)
	{
		uint8 index;
		for(index=0;index<DVA_NUM;index++)
		{
		
		}
	}*/
	
}

/****************************************************
*��������cab_bus_init			       		         
*���ߣ�hk             							 
*������˾�������߳�ʼ������
*���룺��	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void cab_bus_init(void)
{
	
	CAB_BUS_PORTINIT(CAB_BUS_BAUDRATE);
}

/****************************************************
*��������cab_bus_set_debug			       		         
*���ߣ�hk             							 
*���������õ��Ա�־
*���룺��	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
static void cab_bus_set_debug(uint8 flag)
{
	
	cab_bus_debug = flag;
}


//��������豸
cab_bus_dev_t xian2_cab_bus_dev=
{
	cab_bus_init,
	cab_bus_proc,
	cab_bus_send_data,
	cab_bus_set_debug,
};

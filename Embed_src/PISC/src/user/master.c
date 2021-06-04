#if 0
/******************************************************************************
*�ļ����� ��master.c		       		         
*��          �� ��thomson   							 
*��          �� ����������
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"


#define MASTER_FOLLOW_KEY	0x01
#define MASTER_OTHER_TIMEOUT	0x02
#define MASTER_OTHER_ERROR	0x03

#define SLAVE_FOLLOW_KEY	0x11
#define SLAVE_TMS_ERROR		0x12
#define SLAVE_DVA_ERROR		0x13
#define SLAVE_OTHER_MASTER	0x14

#define TMS_COMM_DATA_TIMEOUT 25000
//�������������״ָ̬ʾ
#define IDLE	0
#define MASTER	1
#define SLAVE	2
#define M_S		3
#define S_M		4
#define PRETRRMIT 5
#define F_M		6
#define COMM_DVA_TIMEOUT	80
//����״̬
//static uint8 working_status=0;
#define WAIT_TIME_DVA  82000
uint32 start_detect_dva_timer=0;
uint8 detect_dva_flag = 0;
//��ʱ���
static uint16 master_start_timer=0;
//��ʱ������
static uint16 master_proc_timer=0;

//�������������״̬����
uint8 pisc_status=IDLE;

/****************************************************
*��������	master_proc 		         
*���ߣ�thomson             							 
*��������������
*���룺��                 
*�����	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void master_proc(void)
{
	uint8 pisc_id;
	static uint8 m_s_switch = 0;
	static uint8 flag = 0;
	uint8 tmp;

	if(checktimer(&start_detect_dva_timer, WAIT_TIME_DVA)&&(!detect_dva_flag))
		detect_dva_flag = 1;
	if(detect_dva_flag)
		start_detect_dva_timer = get_clock();   
	//�տ���--��ʱ����������� ��Ϊ���߻�û������
	if(++master_start_timer<10000)
		return;
	master_start_timer=10000;

	//��ʱ������
	if(++master_proc_timer>=10000)
	{
	master_proc_timer=0;
	pisc_id = read_pisc_id();
	//����--�ж�TMS�����־��Կ��
	my_default_data.status_hbit.active=(((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0)?1:0 );
	
	//my_default_data.status_hbit.active=((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0 || (tms_dev->active!=0));
	switch(pisc_status)
	{
		case IDLE:
				my_default_data.status_hbit.master=0;
				
				if((my_default_data.test_status.master_req)&&(tba_bus_dev->data_flag_reconn ==1))//����
				{
				     pisc_status=MASTER;
				     print_line("IDLE to MASTER req");
				}
				else
				{
				    pisc_status=SLAVE;
				    print_line("IDLE to SLAVE");
				}
				break;

		case MASTER:
				my_default_data.status_hbit.master=1;
				my_default_data.test_status.master_req = 0;
				my_default_data.test_status.slave_req = 0;

				//���೤ʱ��û���յ�TMS���ݡ���������
				if((my_default_data.tms_status.tms_valid == 1) && (other_default_data.tms_status.tms_valid == 0)
					&& (!other_default_data.cab_status.dva_error )&&(!tba_bus_dev->data_flag))  //&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				     pisc_status=SLAVE;
	 			     my_default_data.test_status.slave_req = 1;//�б�

				     my_default_data.test_status.master_req = 0;
				     //my_default_data.test_status.change_ms_flag = 1;
				     print_line(" TMS timeout MASTER to SLAVE");
				}
				//����DVA���ϡ�TCMS�������Բ�DVA������TCMS��������������
				if((my_default_data.cab_status.dva_error ==1) //&& (other_default_data.tms_status.tms_valid == 0) 
					&& (other_default_data.cab_status.dva_error == 0)&&(!tba_bus_dev->data_flag))
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=SLAVE;
				    my_default_data.test_status.slave_req = 1;//�б�
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 0;
				    print_line(" MASTER to SLAVE DVA ERROR");
				}
				//������Կ�ף��Բ���Կ�������豸��������������
				if(( other_default_data.status_hbit.active)&&(!my_default_data.status_hbit.active)
					&& (!other_default_data.tms_status.tms_valid)&& (!other_default_data.cab_status.dva_error)
					&&(!tba_bus_dev->data_flag)
					) //&&(!my_default_data.test_status.switch_master )  &&(m_s_switch==0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=SLAVE;
				    my_default_data.test_status.slave_req = 1;//�б�
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 0;
				    print_line(" MASTER to SLAVE key active");
				}	
							//�������л���ť��������һ��ʱ��
				if((m_s_switch==0)&&(other_default_data.test_status.switch_master))
					//&&(!my_default_data.test_status.change_ms_flag))   //&&(m_s_switch<6)
				{
				     pisc_status=SLAVE;
	                        my_default_data.test_status.master_req = 0;//��������
	                        my_default_data.test_status.slave_req = 1;
				     //my_default_data.test_status.switch_master = 1;  
				     m_s_switch++;
				     print_int("CONN M to S %d m_s_switch", m_s_switch);
				}
		
				break;
		case SLAVE:
				my_default_data.status_hbit.master=0;
				//my_default_data.test_status.switch_master = 0;
				my_default_data.test_status.master_req = 0;
				my_default_data.test_status.slave_req = 0;
				
				//485����û������
				if((tba_bus_dev->data_flag))
					//&&(!my_default_data.test_status.change_ms_flag))
					//&& (m_s_switch == 0)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.master_req = 1;//��������
	                       my_default_data.test_status.slave_req = 0;
				   //my_default_data.test_status.change_ms_flag  = 1 ;
				  memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
				  other_default_data.tms_status.tms_valid = 1;
				  other_default_data.cab_status.dva_error = 1;
				    print_line("485 no data,change to master");
				}
		
				//��������Կ���л�
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active)
					&&(tba_bus_dev->data_flag_key==1))	
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
					pisc_status=MASTER;
	                          my_default_data.test_status.master_req = 1;//��������
	                          my_default_data.test_status.slave_req = 0;
					print_line("  SLAVE to MASTER REQ key active");
				}
					//����DVA�������Բ�DVA���ϡ���������
				if((my_default_data.cab_status.dva_error ==0) //&& (my_default_data.tms_status.tms_valid == 0) 
					&& (other_default_data.cab_status.dva_error == 1)&&(tba_bus_dev->data_flag_key==1))
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.slave_req = 0;//�б�
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 1;
				    print_line(" SLAVE to MASTER other  DVA ERROR");
				}
				//����TCMS�������Բ�TCMS���ϡ���������������DVA����
				if((my_default_data.tms_status.tms_valid == 0) && (other_default_data.tms_status.tms_valid == 1)
					&&(my_default_data.cab_status.dva_error ==0) &&(tba_bus_dev->data_flag_key==1)) 
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.slave_req = 0;//�б�
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 1;

				    print_line(" SLAVE to MASTER other TCMS ERROR");
				}
				
						//�ֶ��л�������,��һ����Ч
				if(((B_S_SWITCH_REG&(1<<B_S_SWITCH_BIT))==0) && (m_s_switch == 0))
					//&&(!my_default_data.test_status.change_ms_flag))
				{
				     //pisc_status=MASTER;
				     m_s_switch++;
					my_default_data.test_status.switch_master = 1;
	                        my_default_data.test_status.master_req = 1;//��������
	                        my_default_data.test_status.slave_req = 0;
				     print_line("CONN SWITCH to MASTER REQ");
				}
				//�ֶ�����ֻ��һ��
				if((m_s_switch==1)&&(tba_bus_dev->data_flag_reconn ==1)
					&&(my_default_data.test_status.switch_master == 1))
					//&&(!my_default_data.test_status.change_ms_flag))//((B_S_SWITCH_REG&(1<<B_S_SWITCH_BIT))==0) &&
				{
					pisc_status=MASTER;
					m_s_switch ++;
					//my_default_data.test_status.switch_master = 0;
					print_line("CONN SWITCH to MASTER ");
				}
				break;
		          default:
				my_default_data.status_hbit.master=0;
				pisc_status=IDLE;
	}
	 tmp = other_default_data.test_status.reconnection;
	 //memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
	 other_default_data.test_status.reconnection = tmp;
    }


	//for test
	//my_default_data.status_hbit.master=1;

	
	//����ָʾ��
	if(my_default_data.status_hbit.master)
	{
		//�����豸����˸
		//if(DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))
		if(pisc_id ==1)
			led_blink(MASTER_LED_NO,300,300);
		else 
			led_blink(MASTER_LED_NO,500,500);
		//led_onoff(MASTER_LED_NO,1);
		led_onoff(SLAVE_LED_NO,1);
	}
	else
	{
		//�����豸����˸
		//if(DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))
		if(pisc_id ==1)
			led_blink(SLAVE_LED_NO,300,300);
		else 
			led_blink(SLAVE_LED_NO,500,500);
		//led_onoff(SLAVE_LED_NO,1);
		led_onoff(MASTER_LED_NO,1);
	}
}
#endif

/******************************************************************************
*�ļ����� ��master.c		       		         
*��          �� ��thomson   							 
*��          �� ����������
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"


#define MASTER_FOLLOW_KEY	0x01
#define MASTER_OTHER_TIMEOUT	0x02
#define MASTER_OTHER_ERROR	0x03

#define SLAVE_FOLLOW_KEY	0x11
#define SLAVE_TMS_ERROR		0x12
#define SLAVE_DVA_ERROR		0x13
#define SLAVE_OTHER_MASTER	0x14

#define TMS_COMM_DATA_TIMEOUT 25000
//�������������״ָ̬ʾ
#define IDLE	0
#define MASTER	1
#define SLAVE	2
#define M_S		3
#define S_M		4
#define PRETRRMIT 5
#define F_M		6
#define COMM_DVA_TIMEOUT	80
#define uart_test_master 1
//����״̬
//static uint8 working_status=0;

//��ʱ���
static uint16 master_start_timer=0;
//��ʱ������
static uint16 master_proc_timer=0;

//�����л���ʱ
static uint32 StoM_chg_timer=0;
static uint32 MS_chg_timer=0;
static uint32 MtoS_chg_timer=0;



//�������������״̬����
static uint8 pisc_status=IDLE;
static uint8 pre_pisc_status=IDLE;

/****************************************************
*��������	master_proc 		         
*���ߣ�thomson             							 
*��������������
*���룺��                 
*�����	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void master_proc(void)
{
	uint8 pisc_id;
	static uint8 active_flag=0;  //Կ�ױ�־
	//static uint8 MS_dva_error=0;

	my_default_data.status_hbit.active=(((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0)?1:0 );
	if(my_default_data.status_hbit.active)
		other_default_data.status_hbit.active =0;
	//�տ���--��ʱ����������� ��Ϊ���߻�û������
	if(++master_start_timer<10000)
		return;
	master_start_timer=10000;

	//��ʱ������
	if(++master_proc_timer>=200)
	{
	master_proc_timer=0;
	pisc_id = read_pisc_id();
	
	switch(pisc_status)
      {
		case IDLE:

			pre_pisc_status = IDLE;
			my_default_data.status_hbit.master=0;
			
			if(other_default_data.test_status.master_req) //�Բ�����
			{
			    	pisc_status=SLAVE;
				tba_bus_dev->data_flag =0;
				tba_bus_dev->bus_timer=get_clock();
			    	print_line("IDLE to SLAVE other_req");
				if (uart_test_master==1)
				uart0_write_str("IDLE to SLAVE other_req");
			}
			else if(my_default_data.test_status.master_req)//����
			{
			     	pisc_status=MASTER;
				tba_bus_dev->data_flag =0;
				tba_bus_dev->bus_timer=get_clock();
				//StoM_chg_timer = get_clock()-3000; // 2s��ʱ
			     	print_line("IDLE to MASTER req");
				if (uart_test_master==1)
				uart0_write_str("IDLE to MASTER req");
			}
			else if (my_default_data.status_hbit.active && (!other_default_data.status_hbit.active))
			{
			    	my_default_data.test_status.master_req = 1;//��������			    
			    	my_default_data.test_status.slave_req = 0;
			    	print_line("IDLE to MASTER KEY");
				if (uart_test_master==1)
				uart0_write_str("IDLE to MASTER KEY");	
			}
			else if(checktimer(&MS_chg_timer,5000))
			{
				if(read_pisc_id()==1)
				{
					my_default_data.test_status.master_req = 1;//��������			    
			    		my_default_data.test_status.slave_req = 0;
			    		print_line(" IDLE to MASTER PISC_ID");
					if (uart_test_master==1)
					uart0_write_str(" IDLE to MASTER PISC_ID");
				}
				else
				{
					pisc_status=SLAVE;
					tba_bus_dev->data_flag =0;
					tba_bus_dev->bus_timer=get_clock();
			    		print_line("IDLE to SLAVE PISC_ID");
					if (uart_test_master==1)
					uart0_write_str("IDLE to SLAVE PISC_ID");
				}
			}
                   break;

		case MASTER:

			my_default_data.test_status.master_req = 1;
			my_default_data.test_status.slave_req = 0;

			if (checktimer(&StoM_chg_timer,5000))
			{
				pre_pisc_status = MASTER;
			}

			if(pre_pisc_status == pisc_status)
			{
				my_default_data.status_hbit.master=1;		
				
				 //M1.1 �Բ�����
				if(other_default_data.test_status.master_req)
				{
					pisc_status=SLAVE;
			    		my_default_data.test_status.slave_req = 1;//�б�
					//MtoS_chg_timer=get_clock();      //�յ��Բ��������������б�
			   		my_default_data.test_status.master_req = 0;
					other_default_data.test_status.slave_req = 0;
			   		//tms_dev->get_atc_recv_flag(0);
			   		tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
			    		print_line("MASTER to SLAVE");
					if (uart_test_master==1)
						uart0_write_str("MASTER to SLAVE");
				}	

				//M1.2 ����TMS���ϡ��Բ�TMS��������������
				if((other_default_data.tms_status.tms_valid==0)&&my_default_data.tms_status.tms_valid
					&&(tba_bus_dev->data_flag != 1))
				{				
					pisc_status=SLAVE;
	    				my_default_data.test_status.slave_req = 1;//�б�
		   			my_default_data.test_status.master_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					MtoS_chg_timer=get_clock();
	   				print_line(" MASTER to SLAVE TMS ERROR");
					if (uart_test_master==1)
					uart0_write_str(" MASTER to SLAVE TMS ERROR");
				}				
			
				//M1.3 ����DVA���ϣ��Բ�DVA������TCMS��������������
				if((my_default_data.cab_status.dva_error ==1) && (tba_bus_dev->data_flag != 1) 
				&& (other_default_data.cab_status.dva_error == 0)&&(other_default_data.tms_status.tms_valid==0))
				{				
					pisc_status=SLAVE;
	    				my_default_data.test_status.slave_req = 1;//�б�
		   			my_default_data.test_status.master_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					MtoS_chg_timer=get_clock();
	   				print_line(" MASTER to SLAVE DVA ERROR");
					if (uart_test_master==1)
					uart0_write_str(" MASTER to SLAVE DVA ERROR");
				}			

				#if 0
				//M1.4 ����dva���쳣��Կ�ײ�Ϊ��
				if((my_default_data.cab_status.dva_error == 1) && (other_default_data.cab_status.dva_error == 1) 
					&& other_default_data.status_hbit.active)
				{
				     	pisc_status=SLAVE;
                        		my_default_data.test_status.master_req = 0;//��������
                        		my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();
					print_line(" MASTER to slave two side dva error");
				}

				//M1.5 ����TMS���쳣��Կ�ײ�Ϊ��
				if(my_default_data.tms_status.tms_valid && other_default_data.tms_status.tms_valid 
					&& other_default_data.status_hbit.active)
				{
				     	pisc_status=SLAVE;
                        		my_default_data.test_status.master_req = 0;//��������
                        		my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();
					print_line(" MASTER to slave two side tms error");
				}
				#endif
				
				//M1.6 ������Կ�ף��Բ���Կ�������豸��������������
				if(( other_default_data.status_hbit.active)&&(!my_default_data.status_hbit.active)
					&& (!other_default_data.cab_status.dva_error)&&(!tba_bus_dev->data_flag)
					&&(other_default_data.tms_status.tms_valid==0)) 
				{						
					pisc_status=SLAVE;
                			my_default_data.test_status.master_req = 0;//��������
                			my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();						
					print_line(" MASTER to SLAVE key side ok");
					if (uart_test_master==1)
					uart0_write_str(" MASTER to SLAVE key side ok");
				}	
				
				
				//M1.7 Կ�׹����ٿ���������
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active ))
				{
					if(active_flag!=my_default_data.status_hbit.active)
					{
  						pisc_status=MASTER;
						StoM_chg_timer = get_clock();
                    	    		my_default_data.test_status.master_req = 1;//��������
                     	   		my_default_data.test_status.slave_req = 0;
						//tms_dev->get_atc_recv_flag(0);
						tba_bus_dev->data_flag = 0;
						tba_bus_dev->bus_timer=get_clock();

						//��Կ�׳�ʼ����ǰվ����һվ
						//sh9_tms_dev.current_station=sh9_tms_dev.start_station;
						//sh9_tms_dev.next_station=sh9_tms_dev.start_station;
						next_station_code=0;//��0��ֹ��һ��վ�������
						print_line("MASTER by key on");
						if (uart_test_master==1)
						uart0_write_str("MASTER by key on");
					}
			   	}
			}
			else
			{
				/*/���������л���Բ�pisc ����
				MS_dva_error = other_default_data.cab_status.dva_error;
				memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
				other_default_data.cab_status.dva_error = MS_dva_error ;
				*/
				other_default_data.test_status.master_req = 0;
		
			}	
			break;
			
		case SLAVE:
			if(checktimer(&MtoS_chg_timer,2000))
			{
				pre_pisc_status = SLAVE;			
			}
			if (pre_pisc_status == pisc_status)
			{
				my_default_data.status_hbit.master=0;
				my_default_data.test_status.master_req = 0;
				my_default_data.test_status.slave_req = 1;
				//S1.1 �Բ��б�
				if(other_default_data.test_status.slave_req)
				{
			     		pisc_status=MASTER;
					StoM_chg_timer = get_clock();
                       	 	my_default_data.test_status.master_req = 1;//��������
                     	   	my_default_data.test_status.slave_req  = 0;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
				   	//tms_dev->get_atc_recv_flag(0);
					print_line("SLAVE to MASTER req");
					if (uart_test_master==1)
					uart0_write_str("SLAVE to MASTER req");
				}

				//S1.2 Կ�׹����ٿ���������
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active ))
				{
					if(active_flag!=my_default_data.status_hbit.active)
					{
  						pisc_status=MASTER;
						StoM_chg_timer = get_clock();
                   		     		my_default_data.test_status.master_req = 1;//��������
                  		      		my_default_data.test_status.slave_req = 0;
						//tms_dev->get_atc_recv_flag(0);
						tba_bus_dev->data_flag = 0;
						tba_bus_dev->bus_timer=get_clock();

						//��Կ�׳�ʼ����ǰվ����һվ
						//sh9_tms_dev.current_station=sh9_tms_dev.start_station;
						//sh9_tms_dev.next_station=sh9_tms_dev.start_station;
						next_station_code=0;//��0��ֹ��һ��վ�������
						print_line("SLAVE to MASTER by key on");
						if (uart_test_master==1)
						uart0_write_str("SLAVE to MASTER by key on");
					}
			   
				}
			
				//S1.3 485����û������
				if(tba_bus_dev->data_flag)
				{					
					pisc_status=MASTER;
					//StoM_chg_timer = get_clock();
					other_default_data.test_status.master_req = 0;
		   	 		my_default_data.test_status.master_req = 1;//��������
                   			my_default_data.test_status.slave_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					other_default_data.cab_status.dva_error=1;//��Բ�����״̬�����Է��Ƿ�����
					other_default_data.tms_status.tms_valid=1;
					other_default_data.status_hbit.active =0;
					print_line("SLAVE to MASTER Master 485 error");
					if (uart_test_master==1)
					uart0_write_str("SLAVE to MASTER Master 485 error");
				}
			
				//S1.4 ����dva���쳣��Կ�ײ�Ϊ��
				if((my_default_data.cab_status.dva_error == 1) && (other_default_data.cab_status.dva_error == 1) 
					&& (my_default_data.tms_status.tms_valid==0)&&my_default_data.status_hbit.active)
				{
			  	   	pisc_status=MASTER;
					StoM_chg_timer = get_clock();
                   		     	my_default_data.test_status.master_req = 1;//��������
                     	   	my_default_data.test_status.slave_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					print_line("slave to master two side dva error");
					if (uart_test_master==1)
					uart0_write_str("slave to master two side dva error");
				}
				
			}
			else
				other_default_data.test_status.slave_req=0;
			break;

             default:
			my_default_data.status_hbit.master=0;
			pisc_status=IDLE;
			break;
      	}
	//tmp = other_default_data.test_status.reconnection;
	//memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
	//other_default_data.test_status.reconnection = tmp;

	active_flag = my_default_data.status_hbit.active;
    }


	//for test
	//my_default_data.status_hbit.master=1;

	
	//����ָʾ��
	if(my_default_data.status_hbit.master)
	{
		//�����豸����˸
		//if(DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))
		if(pisc_id ==1)
			led_blink(MASTER_LED_NO,300,300);
		else 
			led_blink(MASTER_LED_NO,500,500);
		//led_onoff(MASTER_LED_NO,1);
		led_onoff(SLAVE_LED_NO,1);
	}
	else
	{
		//�����豸����˸
		//if(DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))
		if(pisc_id ==1)
			led_blink(SLAVE_LED_NO,300,300);
		else 
			led_blink(SLAVE_LED_NO,500,500);
		//led_onoff(SLAVE_LED_NO,1);
		led_onoff(MASTER_LED_NO,1);
	}
}




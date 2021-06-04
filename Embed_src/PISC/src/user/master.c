#if 0
/******************************************************************************
*文件名称 ：master.c		       		         
*作          者 ：thomson   							 
*描          述 ：主备处理
*创建日期：2010-3-10
*修改记录：无								
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
//中央控制器工作状态指示
#define IDLE	0
#define MASTER	1
#define SLAVE	2
#define M_S		3
#define S_M		4
#define PRETRRMIT 5
#define F_M		6
#define COMM_DVA_TIMEOUT	80
//工作状态
//static uint8 working_status=0;
#define WAIT_TIME_DVA  82000
uint32 start_detect_dva_timer=0;
uint8 detect_dva_flag = 0;
//延时检测
static uint16 master_start_timer=0;
//定时间隔检测
static uint16 master_proc_timer=0;

//中央控制器主备状态变量
uint8 pisc_status=IDLE;

/****************************************************
*函数名：	master_proc 		         
*作者：thomson             							 
*描述：主备处理
*输入：无                 
*输出：	
*返回值：无
*修改记录：无								
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
	//刚开机--延时进行主备检测 因为总线还没有启动
	if(++master_start_timer<10000)
		return;
	master_start_timer=10000;

	//定时间隔检测
	if(++master_proc_timer>=10000)
	{
	master_proc_timer=0;
	pisc_id = read_pisc_id();
	//激活--判断TMS激活标志和钥匙
	my_default_data.status_hbit.active=(((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0)?1:0 );
	
	//my_default_data.status_hbit.active=((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0 || (tms_dev->active!=0));
	switch(pisc_status)
	{
		case IDLE:
				my_default_data.status_hbit.master=0;
				
				if((my_default_data.test_status.master_req)&&(tba_bus_dev->data_flag_reconn ==1))//切主
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

				//本侧长时间没有收到TMS数据、总线正常
				if((my_default_data.tms_status.tms_valid == 1) && (other_default_data.tms_status.tms_valid == 0)
					&& (!other_default_data.cab_status.dva_error )&&(!tba_bus_dev->data_flag))  //&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				     pisc_status=SLAVE;
	 			     my_default_data.test_status.slave_req = 1;//切备

				     my_default_data.test_status.master_req = 0;
				     //my_default_data.test_status.change_ms_flag = 1;
				     print_line(" TMS timeout MASTER to SLAVE");
				}
				//本侧DVA故障、TCMS正常，对侧DVA正常、TCMS正常、总线正常
				if((my_default_data.cab_status.dva_error ==1) //&& (other_default_data.tms_status.tms_valid == 0) 
					&& (other_default_data.cab_status.dva_error == 0)&&(!tba_bus_dev->data_flag))
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=SLAVE;
				    my_default_data.test_status.slave_req = 1;//切备
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 0;
				    print_line(" MASTER to SLAVE DVA ERROR");
				}
				//本侧无钥匙，对侧有钥匙其他设备正常、总线正常
				if(( other_default_data.status_hbit.active)&&(!my_default_data.status_hbit.active)
					&& (!other_default_data.tms_status.tms_valid)&& (!other_default_data.cab_status.dva_error)
					&&(!tba_bus_dev->data_flag)
					) //&&(!my_default_data.test_status.switch_master )  &&(m_s_switch==0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=SLAVE;
				    my_default_data.test_status.slave_req = 1;//切备
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 0;
				    print_line(" MASTER to SLAVE key active");
				}	
							//按主备切换按钮，保持主一段时间
				if((m_s_switch==0)&&(other_default_data.test_status.switch_master))
					//&&(!my_default_data.test_status.change_ms_flag))   //&&(m_s_switch<6)
				{
				     pisc_status=SLAVE;
	                        my_default_data.test_status.master_req = 0;//切主请求
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
				
				//485总线没有数据
				if((tba_bus_dev->data_flag))
					//&&(!my_default_data.test_status.change_ms_flag))
					//&& (m_s_switch == 0)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.master_req = 1;//切主请求
	                       my_default_data.test_status.slave_req = 0;
				   //my_default_data.test_status.change_ms_flag  = 1 ;
				  memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
				  other_default_data.tms_status.tms_valid = 1;
				  other_default_data.cab_status.dva_error = 1;
				    print_line("485 no data,change to master");
				}
		
				//正常跟随钥匙切换
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active)
					&&(tba_bus_dev->data_flag_key==1))	
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
					pisc_status=MASTER;
	                          my_default_data.test_status.master_req = 1;//切主请求
	                          my_default_data.test_status.slave_req = 0;
					print_line("  SLAVE to MASTER REQ key active");
				}
					//本侧DVA正常，对侧DVA故障、总线正常
				if((my_default_data.cab_status.dva_error ==0) //&& (my_default_data.tms_status.tms_valid == 0) 
					&& (other_default_data.cab_status.dva_error == 1)&&(tba_bus_dev->data_flag_key==1))
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.slave_req = 0;//切备
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 1;
				    print_line(" SLAVE to MASTER other  DVA ERROR");
				}
				//本侧TCMS正常，对侧TCMS故障、总线正常、本侧DVA正常
				if((my_default_data.tms_status.tms_valid == 0) && (other_default_data.tms_status.tms_valid == 1)
					&&(my_default_data.cab_status.dva_error ==0) &&(tba_bus_dev->data_flag_key==1)) 
					//&& (m_s_switch == 0)&&(!my_default_data.test_status.change_ms_flag)
				{
				    pisc_status=MASTER;
				    my_default_data.test_status.slave_req = 0;//切备
				    //my_default_data.test_status.change_ms_flag = 1;
				    my_default_data.test_status.master_req = 1;

				    print_line(" SLAVE to MASTER other TCMS ERROR");
				}
				
						//手动切换主备机,仅一次有效
				if(((B_S_SWITCH_REG&(1<<B_S_SWITCH_BIT))==0) && (m_s_switch == 0))
					//&&(!my_default_data.test_status.change_ms_flag))
				{
				     //pisc_status=MASTER;
				     m_s_switch++;
					my_default_data.test_status.switch_master = 1;
	                        my_default_data.test_status.master_req = 1;//切主请求
	                        my_default_data.test_status.slave_req = 0;
				     print_line("CONN SWITCH to MASTER REQ");
				}
				//手动切主只切一次
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

	
	//主备指示灯
	if(my_default_data.status_hbit.master)
	{
		//根据设备号闪烁
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
		//根据设备号闪烁
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
*文件名称 ：master.c		       		         
*作          者 ：thomson   							 
*描          述 ：主备处理
*创建日期：2010-3-10
*修改记录：无								
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
//中央控制器工作状态指示
#define IDLE	0
#define MASTER	1
#define SLAVE	2
#define M_S		3
#define S_M		4
#define PRETRRMIT 5
#define F_M		6
#define COMM_DVA_TIMEOUT	80
#define uart_test_master 1
//工作状态
//static uint8 working_status=0;

//延时检测
static uint16 master_start_timer=0;
//定时间隔检测
static uint16 master_proc_timer=0;

//主备切换定时
static uint32 StoM_chg_timer=0;
static uint32 MS_chg_timer=0;
static uint32 MtoS_chg_timer=0;



//中央控制器主备状态变量
static uint8 pisc_status=IDLE;
static uint8 pre_pisc_status=IDLE;

/****************************************************
*函数名：	master_proc 		         
*作者：thomson             							 
*描述：主备处理
*输入：无                 
*输出：	
*返回值：无
*修改记录：无								
****************************************************/
void master_proc(void)
{
	uint8 pisc_id;
	static uint8 active_flag=0;  //钥匙标志
	//static uint8 MS_dva_error=0;

	my_default_data.status_hbit.active=(((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0)?1:0 );
	if(my_default_data.status_hbit.active)
		other_default_data.status_hbit.active =0;
	//刚开机--延时进行主备检测 因为总线还没有启动
	if(++master_start_timer<10000)
		return;
	master_start_timer=10000;

	//定时间隔检测
	if(++master_proc_timer>=200)
	{
	master_proc_timer=0;
	pisc_id = read_pisc_id();
	
	switch(pisc_status)
      {
		case IDLE:

			pre_pisc_status = IDLE;
			my_default_data.status_hbit.master=0;
			
			if(other_default_data.test_status.master_req) //对侧切主
			{
			    	pisc_status=SLAVE;
				tba_bus_dev->data_flag =0;
				tba_bus_dev->bus_timer=get_clock();
			    	print_line("IDLE to SLAVE other_req");
				if (uart_test_master==1)
				uart0_write_str("IDLE to SLAVE other_req");
			}
			else if(my_default_data.test_status.master_req)//切主
			{
			     	pisc_status=MASTER;
				tba_bus_dev->data_flag =0;
				tba_bus_dev->bus_timer=get_clock();
				//StoM_chg_timer = get_clock()-3000; // 2s延时
			     	print_line("IDLE to MASTER req");
				if (uart_test_master==1)
				uart0_write_str("IDLE to MASTER req");
			}
			else if (my_default_data.status_hbit.active && (!other_default_data.status_hbit.active))
			{
			    	my_default_data.test_status.master_req = 1;//切主请求			    
			    	my_default_data.test_status.slave_req = 0;
			    	print_line("IDLE to MASTER KEY");
				if (uart_test_master==1)
				uart0_write_str("IDLE to MASTER KEY");	
			}
			else if(checktimer(&MS_chg_timer,5000))
			{
				if(read_pisc_id()==1)
				{
					my_default_data.test_status.master_req = 1;//切主请求			    
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
				
				 //M1.1 对侧切主
				if(other_default_data.test_status.master_req)
				{
					pisc_status=SLAVE;
			    		my_default_data.test_status.slave_req = 1;//切备
					//MtoS_chg_timer=get_clock();      //收到对侧切主请求马上切备
			   		my_default_data.test_status.master_req = 0;
					other_default_data.test_status.slave_req = 0;
			   		//tms_dev->get_atc_recv_flag(0);
			   		tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
			    		print_line("MASTER to SLAVE");
					if (uart_test_master==1)
						uart0_write_str("MASTER to SLAVE");
				}	

				//M1.2 本侧TMS故障、对侧TMS正常、总线正常
				if((other_default_data.tms_status.tms_valid==0)&&my_default_data.tms_status.tms_valid
					&&(tba_bus_dev->data_flag != 1))
				{				
					pisc_status=SLAVE;
	    				my_default_data.test_status.slave_req = 1;//切备
		   			my_default_data.test_status.master_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					MtoS_chg_timer=get_clock();
	   				print_line(" MASTER to SLAVE TMS ERROR");
					if (uart_test_master==1)
					uart0_write_str(" MASTER to SLAVE TMS ERROR");
				}				
			
				//M1.3 本侧DVA故障，对侧DVA正常、TCMS正常、总线正常
				if((my_default_data.cab_status.dva_error ==1) && (tba_bus_dev->data_flag != 1) 
				&& (other_default_data.cab_status.dva_error == 0)&&(other_default_data.tms_status.tms_valid==0))
				{				
					pisc_status=SLAVE;
	    				my_default_data.test_status.slave_req = 1;//切备
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
				//M1.4 两端dva都异常，钥匙侧为主
				if((my_default_data.cab_status.dva_error == 1) && (other_default_data.cab_status.dva_error == 1) 
					&& other_default_data.status_hbit.active)
				{
				     	pisc_status=SLAVE;
                        		my_default_data.test_status.master_req = 0;//切主请求
                        		my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();
					print_line(" MASTER to slave two side dva error");
				}

				//M1.5 两端TMS都异常，钥匙侧为主
				if(my_default_data.tms_status.tms_valid && other_default_data.tms_status.tms_valid 
					&& other_default_data.status_hbit.active)
				{
				     	pisc_status=SLAVE;
                        		my_default_data.test_status.master_req = 0;//切主请求
                        		my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();
					print_line(" MASTER to slave two side tms error");
				}
				#endif
				
				//M1.6 本侧无钥匙，对侧有钥匙其他设备正常、总线正常
				if(( other_default_data.status_hbit.active)&&(!my_default_data.status_hbit.active)
					&& (!other_default_data.cab_status.dva_error)&&(!tba_bus_dev->data_flag)
					&&(other_default_data.tms_status.tms_valid==0)) 
				{						
					pisc_status=SLAVE;
                			my_default_data.test_status.master_req = 0;//切主请求
                			my_default_data.test_status.slave_req = 1;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					//tms_dev->get_atc_recv_flag(0);
					MtoS_chg_timer=get_clock();						
					print_line(" MASTER to SLAVE key side ok");
					if (uart_test_master==1)
					uart0_write_str(" MASTER to SLAVE key side ok");
				}	
				
				
				//M1.7 钥匙关了再开请求切主
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active ))
				{
					if(active_flag!=my_default_data.status_hbit.active)
					{
  						pisc_status=MASTER;
						StoM_chg_timer = get_clock();
                    	    		my_default_data.test_status.master_req = 1;//切主请求
                     	   		my_default_data.test_status.slave_req = 0;
						//tms_dev->get_atc_recv_flag(0);
						tba_bus_dev->data_flag = 0;
						tba_bus_dev->bus_timer=get_clock();

						//开钥匙初始化当前站和下一站
						//sh9_tms_dev.current_station=sh9_tms_dev.start_station;
						//sh9_tms_dev.next_station=sh9_tms_dev.start_station;
						next_station_code=0;//清0防止第一个站不报情况
						print_line("MASTER by key on");
						if (uart_test_master==1)
						uart0_write_str("MASTER by key on");
					}
			   	}
			}
			else
			{
				/*/发生主备切换清对侧pisc 数据
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
				//S1.1 对侧切备
				if(other_default_data.test_status.slave_req)
				{
			     		pisc_status=MASTER;
					StoM_chg_timer = get_clock();
                       	 	my_default_data.test_status.master_req = 1;//切主请求
                     	   	my_default_data.test_status.slave_req  = 0;
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
				   	//tms_dev->get_atc_recv_flag(0);
					print_line("SLAVE to MASTER req");
					if (uart_test_master==1)
					uart0_write_str("SLAVE to MASTER req");
				}

				//S1.2 钥匙关了再开请求切主
				if(( !other_default_data.status_hbit.active)&&(my_default_data.status_hbit.active ))
				{
					if(active_flag!=my_default_data.status_hbit.active)
					{
  						pisc_status=MASTER;
						StoM_chg_timer = get_clock();
                   		     		my_default_data.test_status.master_req = 1;//切主请求
                  		      		my_default_data.test_status.slave_req = 0;
						//tms_dev->get_atc_recv_flag(0);
						tba_bus_dev->data_flag = 0;
						tba_bus_dev->bus_timer=get_clock();

						//开钥匙初始化当前站和下一站
						//sh9_tms_dev.current_station=sh9_tms_dev.start_station;
						//sh9_tms_dev.next_station=sh9_tms_dev.start_station;
						next_station_code=0;//清0防止第一个站不报情况
						print_line("SLAVE to MASTER by key on");
						if (uart_test_master==1)
						uart0_write_str("SLAVE to MASTER by key on");
					}
			   
				}
			
				//S1.3 485总线没有数据
				if(tba_bus_dev->data_flag)
				{					
					pisc_status=MASTER;
					//StoM_chg_timer = get_clock();
					other_default_data.test_status.master_req = 0;
		   	 		my_default_data.test_status.master_req = 1;//切主请求
                   			my_default_data.test_status.slave_req = 0;
					//tms_dev->get_atc_recv_flag(0);
					tba_bus_dev->data_flag = 0;
					tba_bus_dev->bus_timer=get_clock();
					other_default_data.cab_status.dva_error=1;//清对侧数据状态，检查对方是否在线
					other_default_data.tms_status.tms_valid=1;
					other_default_data.status_hbit.active =0;
					print_line("SLAVE to MASTER Master 485 error");
					if (uart_test_master==1)
					uart0_write_str("SLAVE to MASTER Master 485 error");
				}
			
				//S1.4 两端dva都异常，钥匙侧为主
				if((my_default_data.cab_status.dva_error == 1) && (other_default_data.cab_status.dva_error == 1) 
					&& (my_default_data.tms_status.tms_valid==0)&&my_default_data.status_hbit.active)
				{
			  	   	pisc_status=MASTER;
					StoM_chg_timer = get_clock();
                   		     	my_default_data.test_status.master_req = 1;//切主请求
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

	
	//主备指示灯
	if(my_default_data.status_hbit.master)
	{
		//根据设备号闪烁
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
		//根据设备号闪烁
		//if(DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))
		if(pisc_id ==1)
			led_blink(SLAVE_LED_NO,300,300);
		else 
			led_blink(SLAVE_LED_NO,500,500);
		//led_onoff(SLAVE_LED_NO,1);
		led_onoff(MASTER_LED_NO,1);
	}
}




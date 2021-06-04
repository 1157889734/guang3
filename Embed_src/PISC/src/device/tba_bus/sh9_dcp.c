
/******************************************************************************
*�ļ����ƣ�sh9_dcp.c		       		         
*��          �ߣ�hk           							 
*��          �����㲥���ƺд���		
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"

#define    ZERO_KM_MASK                     0x4000     //���ٶ�������
#define    OPEN_LEFT_DOOR_MASK		 0x0100     //������������
#define    OPEN_RIGHT_DOOR_MASK	 0x1000     //������������
#define    CLOSE_DOOR_MASK			 0x0004	   //����������

#define   TMS_0KM_STATUS                     ((RD_EXT_IO_STAUTS)&(ZERO_KM_MASK))             //���ٶȱ�ʶ
#define   CLOSE_DOOR_STATUS                ((RD_EXT_IO_STAUTS)&(CLOSE_DOOR_MASK))        //��״̬
#define   OPEN_LEFT_DOOR_STATUS         ((RD_EXT_IO_STAUTS)&(OPEN_LEFT_DOOR_MASK))   //����״̬
#define   OPEN_RIGHT_DOOR_STATUS       ((RD_EXT_IO_STAUTS)&(OPEN_RIGHT_DOOR_MASK))  //����״̬


//�Խ�״̬
#define    REQ_TALK           0x01      //����״̬
#define    RSP_TALK            0x02     //Ӧ��״̬
#define    URG_TALK_OFF      0x03     //�˿ͽ�����������
#define    TALK_OFF             0x00    //�Ҷ�״̬��˾���Խ�/�˿ͱ���ͨ�ã�

//¼��ģ������
#define   RECORD_TASK_START    0x01     //¼������
#define   RECORD_TASK_STOP     0x02      //ֹͣ����

#define 	ROUTE_ID_NUM        8     // Ŀǰ���ŵ���·����

//����DCP�Ĺ������ݽṹ
dcp_default_data_t dcp_default_data[TBA_DCP_NUM]=
{
	{0,0,0,0,0,0,{0},{0},{0},0,{0},0,},
	{0,0,0,0,0,0,{0},{0},{0},0,{0},0,},
};
//static uint8  pis_work_mode = WORK_AT_TCMS;    //�������������ģʽ

// 2��DCPͨ�Ŵ���
uint16 dcp_comm_error[TBA_DCP_NUM]={0};

static uint8  record_task = RECORD_TASK_STOP;    //¼��ģ����������
static uint32  record_time = 0;                             //��¼��ģ�鷢������Ŀ�ʼʱ��
static uint32  record_timeout = 200; 

/****************************************************
*��������dcp_proc			       		         
*���ߣ�hk             							 
*�������㲥���ƺ����ݴ���				
*���룺src_net_id       --- Դ�����
                 src_dev_id       ---Դ�豸id
                 data_type        ---��������
                 packet            ---����
                 length             ---���ݳ���
                  
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void dcp_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{   
	static uint8 leave_broadcast = 0;    //Ԥ��վ����
	static uint8 arrived_broadcast = 0;  //��վ����

	static uint8 stop_broadcast = 0;    // 1:ͣ�㲥
	static uint8 urgent_broadcast = 0;
	uint8 dev_no = 0;
	uint8 cnt =0;
	dev_no=src_dev_id>>4;//�豸��	   

	
	//print_line("dcp1");
	if((data_type==TBA_CMD_DEFAULTPACKET) && (length==sizeof(dcp_default_data_t))) //��������
	{			
		//print_line("dcp2");
		if((dev_no) && (dev_no<=TBA_DCP_NUM))//�ж�Դ�豸���
		{
		   dev_no = dev_no - 1;			
			dcp_comm_error[dev_no]=0;//���ͨ�Ŵ���			
			memcpy( (uint8 *)&dcp_default_data[dev_no], packet, sizeof(dcp_default_data_t) );//��������
		}

		//��������������
		/*
		if(dcp_default_data[dev_no].line_mode.moni_broadcast)
		{
			saveparam.dcp_pa_listen_volume = dcp_default_data[dev_no].line_mode.moni_broadcast;
			save_set_task();
		}*/
				

		//�ж�dcp�����Ƿ�= pisc 		
		if((dcp_default_data[dev_no].dcp_pacc_listen&0x0f) != saveparam.dcp_pa_listen_volume
			||(dcp_default_data[dev_no].dcp_pacc_listen&0xf0) != (saveparam.dcp_pc_listen_volume<<4)
			||(dcp_default_data[dev_no].dcp_pcpa_listen&0x0f) != saveparam.dcp_pc_listen_volume
			||(dcp_default_data[dev_no].dcp_pcpa_listen&0xf0) != (saveparam.dcp_pa_mic_volume<<4)
			||(dcp_default_data[dev_no].dcp_ccpc_mic&0x0f) != saveparam.dcp_cc_mic_volume
			||(dcp_default_data[dev_no].dcp_ccpc_mic&0xf0) != (saveparam.dcp_pc_mic_volume<<4))
		{
			#if 0
			if(my_default_data.status_hbit.master==0) // ��������ֵ���ڹ㲥���ƺ�
			{
				saveparam.dcp_pa_listen_volume = dcp_default_data[dev_no].dcp_pacc_listen & 0x0f;
				saveparam.dcp_pc_listen_volume = (dcp_default_data[dev_no].dcp_pacc_listen >> 4) & 0x0f; 
				saveparam.dcp_pc_listen_volume = (dcp_default_data[dev_no].dcp_pcpa_listen&0x0f);
				saveparam.dcp_pa_mic_volume = (dcp_default_data[dev_no].dcp_pcpa_listen>>4)&0x0f;
				saveparam.dcp_cc_mic_volume = (dcp_default_data[dev_no].dcp_ccpc_mic&0x0f);
				saveparam.dcp_pc_mic_volume = (dcp_default_data[dev_no].dcp_ccpc_mic>>4)&0x0f;	
				if(other_default_data.dcp_volume&0xf0 ==0)  //����û���޸�����ֵ
				{
					save_set_task();
				}
			}
			else
			#endif
			if(my_default_data.status_hbit.master)  //�޸Ĺ㲥������
				sh9_tba_bus_dev.dcp_volume_set = 1;
		}

		//print_int("%dmode:",sh9_tms_dev.pis_work_mode);

		/*****************�ֶ�ģʽ����*********************/		
		if(dcp_default_data[dev_no].status_hbit.active != 1)  //�Ǽ���˹㲥���ƺ�
		    return;

	
		// if(sh9_tms_dev.pis_work_mode == WORK_AT_MANU)  //����ʲôģʽ����Ӧ����������
		{
              	//sh9_tms_dev.pis_work_mode = WORK_AT_MANU ;
	  		//sh9_tms_dev.run_dir = dcp_default_data[dev_no].status_hbit.run_dir;//��ȡ������״̬ DCP 0:����  1:����
			sh9_tms_dev.city_id = dcp_default_data[dev_no].line_mode.city_id;

			/*
				���������������ʼվ�źź��յ�վ�ź��Ǵ������������Զ�ģʽ���޸���·��
				�����һֱ���źţ��������޸���·��
				Ŀǰ��һֱ��
			*/
			if(!sh9_tms_dev.pis_work_mode)  //�Զ�ģʽ�£��������޸���· 
			{
				if((sh9_tms_dev.start_station != dcp_default_data[dev_no].start_station)
					|| (sh9_tms_dev.end_station != dcp_default_data[dev_no].end_station)) //��·�仯
				{
					if( dcp_default_data[dev_no].start_station) 
						sh9_tms_dev.start_station = dcp_default_data[dev_no].start_station;	
					if( dcp_default_data[dev_no].end_station) 
					{
						if(sh9_tms_dev.end_station != dcp_default_data[dev_no].end_station)//�޸����յ�վ
							end_station_code = 0;
						sh9_tms_dev.end_station = dcp_default_data[dev_no].end_station;						
					}									
				}
			}
			if( dcp_default_data[dev_no].next_station)  //��һվ
				sh9_tms_dev.next_station = dcp_default_data[dev_no].next_station;
			

			//sh9_tms_dev.current_station =  dcp_default_data[dev_no].current_station;


			//�ֶ�Ԥ��վ
			if(leave_broadcast ==0 && dcp_default_data[dev_no].broadcast_hbit.leave_broadcast)
			{
				sh9_tms_dev.arrived = 0;				
				tms_dev->set_broadcast(1); //Ԥ��վ�㲥
				next_station_code = sh9_tms_dev.next_station ; //��ֹ�ֶ���վ���ر�
				
				my_default_data.trigger_hbit.leave_broadcast=1;

				#if 1			
				if((sh9_tms_dev.door_sides >>7) ==0)//suven  �޸��ֶ�ģʽ�¿��Ų�
				{
					sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1] ;
				}
				#endif	
			}
			leave_broadcast = dcp_default_data[dev_no].broadcast_hbit.leave_broadcast;

			//�ֶ���վ�㲥
			if(arrived_broadcast == 0 && dcp_default_data[dev_no].broadcast_hbit.arrived_broadcast)
			{
				//station_broadcast = 2;   //��վ�㲥
				tms_dev->set_broadcast(2);
				sh9_tms_dev.arrived = 1;
				//sh9_tms_dev.current_station =sh9_tms_dev.next_station ; 	//���µ�ǰվ	
				my_default_data.trigger_hbit.arrived_broadcast=1;
			}
			arrived_broadcast = dcp_default_data[dev_no].broadcast_hbit.arrived_broadcast;

			//�����㲥
			if((urgent_broadcast == 0) && dcp_default_data[dev_no].broadcast_hbit.urgent_broadcast )
			{
			   sh9_tms_dev.special_broadcast_code = dcp_default_data[dev_no].urgent_broadcast_no;
			   //special_broadcast = 1;
			   tms_dev->set_special_broadcast(1);
			   print_line("emergency br");
			}

			/*
			if(my_default_data.trigger_hbit.urgent_broadcast && urgent_broadcast 
				&& (dcp_default_data[dev_no].broadcast_hbit.urgent_broadcast == 0))
			{
			   dva_dev->stop();
			   sh9_tms_dev.emergency_status = 1; //ֹͣ�����㲥��־
			   print_line("stop emergency br");
			}*/
			urgent_broadcast = dcp_default_data[dev_no].broadcast_hbit.urgent_broadcast;

			//ֹͣ�㲥
			if(stop_broadcast == 0 && dcp_default_data[dev_no].broadcast_hbit.stop_broadcast)
			{
			   dva_dev->stop();
			   sh9_tms_dev.emergency_status = 1; //ֹͣ�����㲥��־
			   print_line("stop dva br");
			}
			stop_broadcast = dcp_default_data[dev_no].broadcast_hbit.stop_broadcast;

			/*/��������
			if(dcp_default_data[dev_no].broadcast_hbit.mute_broadcast)
			{
			   apu_dev->set_mute_flag(1);			   
			}
			else
			{
			   apu_dev->set_mute_flag(0);
			}*/
		 }			
		
	}
	//��������
	else
	{
		return;
	}
	//¼������
	//dcp_record(dev_no);
}

/****************************************************
*��������dcp_record			       		         
*���ߣ�hk             							 
*������¼������				
*���룺dev_no           ---�豸��                  
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void  dcp_record(uint8 dev_no)
{
    static uint8 pre_talk_status[32] = {0};  //˾���Խ�״̬1��˾���Խ�״̬2
    uint8 talk_status[32] = {0};
    uint8 buf[64] = {0};
    static uint8 stop_times = 0;
    static uint8 start_times = 0;
    uint8 temp = 0;
    uint8 icc_num = 0;
    uint8 rtc_time[5] = {0};
    static uint8 driver_to_passager_flag = 0;


    //ûԿ�׶˹㲥�����ݲ�����
    if(dcp_default_data[dev_no].status_hbit.active==0)
	return;
	
    for(temp = 0; temp < 8; temp++)
    {
        talk_status[icc_num++] = ((dcp_default_data[dev_no].icc_status[temp] >> 6)&0x03);
        talk_status[icc_num++] = ((dcp_default_data[dev_no].icc_status[temp] >> 4)&0x03);
        talk_status[icc_num++] = ((dcp_default_data[dev_no].icc_status[temp] >> 2)&0x03);
        talk_status[icc_num++] = (dcp_default_data[dev_no].icc_status[temp] & 0x03);
    }

   // talk_status = dcp_default_data[dev_no].status_hbit.talking;

   for(icc_num=0; icc_num < 32; icc_num++)
   {

	   //˾���Խ�״̬��Ӧ��״̬ -->  �Ҷ� ,����ֹͣ¼��
	  if(((pre_talk_status[icc_num] == RSP_TALK) && ((talk_status[icc_num] == TALK_OFF) || (talk_status[icc_num] == URG_TALK_OFF)))&& (record_task == RECORD_TASK_START))
	  {
	        buf[0] = 0x08;   //ֹͣ¼������
	        record_task = RECORD_TASK_STOP;
		 record_time = get_clock();
		 cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 1, buf);  //����ֹͣ¼������
		 print_line("stop record");
		 record_timeout = 200;      //record_timeoutʱ����û�յ��ظ��������·���
		 stop_times++;
		 start_times = 0;

		 driver_to_passager_flag--;
	  }

	  
        //˾���Խ�״̬�ɺ���״̬--> Ӧ��״̬ ��ʾ�ѿ�ʼ�Խ����ɽ���¼��
        if((talk_status[icc_num]== RSP_TALK) && (pre_talk_status[icc_num] != RSP_TALK) &&(record_task != RECORD_TASK_START))   
        {
              rtc_dev_p->get_rtc(rtc_time);
		 tms_dev->month = (rtc_time[1] >> 4)*10 +(rtc_time[1]&0x0f);
		 tms_dev->date =  (rtc_time[2] >> 4)*10 +(rtc_time[2]&0x0f);
            	 buf[0] = 0x07;   //¼������
	        buf[1] = tms_dev->month; //�·�
		 buf[2] = tms_dev->date;   //����
	        record_task = RECORD_TASK_START;
		 record_time = get_clock();
		 cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 3, buf);  //����¼������
		 print_line("start record");
		 record_timeout = 200;      //record_timeoutʱ����û�յ��ظ��������·���
		 start_times++;
		 stop_times = 0;
		 
		 driver_to_passager_flag++;
         }	
	pre_talk_status[icc_num] = talk_status[icc_num];
   }

    //����״̬Ϊֹͣ¼����¼��ģ�鵱ǰ״̬������ֹͣ¼��״̬
    if((record_task == RECORD_TASK_STOP) && (dva_dev->get_record_status() ))
    {
        if(timer0_check_timer(&record_time, record_timeout)&& (stop_times < 3))
        {
            buf[0] = 0x08;   //ֹͣ¼������
            record_timeout = record_timeout + 200;
	     record_time = get_clock();
    
           cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 1, buf);  //����ֹͣ¼������
           print_line("stop record+");
	    stop_times++;
        }
     }

     //����״̬Ϊ��ʼ¼����¼��ģ�鵱ǰ״̬������¼��״̬
    if((record_task == RECORD_TASK_START) && (dva_dev->get_record_status() != 1))
    {
        if(timer0_check_timer(&record_time, record_timeout) && (start_times < 3))
        {
            rtc_dev_p->get_rtc(rtc_time);
	     tms_dev->month = (rtc_time[1] >> 4)*10 +(rtc_time[1]&0x0f);
	     tms_dev->date =  (rtc_time[2] >> 4)*10 +(rtc_time[2]&0x0f);
            buf[0] = 0x07;   //¼������
            buf[1] = tms_dev->month; //�·�
            buf[2] = tms_dev->date;   //����
            record_timeout = record_timeout + 200;
	     record_time = get_clock();

            cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 3, buf);  //����¼������
            print_line("start record+");
	     start_times++;
         }
     }
	
}

//***********************************************************************************************************************
//��������:���ƺл�ȡý�岥�ſ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
uint8 dcp_get_media_on(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		//Ѱ�Ҽ����豸
		if(dcp_default_data[index].status_hbit.active)
		{
			//return dcp_default_data[index].status_hbit.media_on;
			return 1;
		}
	}
	
	return 0;
}

//***********************************************************************************************************************
//��������:���ƺл�ȡ�㲥����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
uint8 dcp_get_broadcast_req(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		if(dcp_default_data[index].broadcast_hbit.broadcast_req)
			return 1;
	}
	
	return 0;	
}

//***********************************************************************************************************************
//��������:���ƺл�ȡ�㲥״̬
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
uint8 dcp_get_broadcast_state(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		if(dcp_default_data[index].broadcast_hbit.broadcasting)
			return 1;
	}
	
	return 0;	
}

//***********************************************************************************************************************
//��������:���ƺл�ȡ�Խ�״̬
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************

uint8 dcp_get_talk_state(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		if(dcp_default_data[index].status_hbit.talking)
		{
			return 1;
		}
	}
	
	return 0;	
}


uint8 dcp_get_emtalking_state(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		if(dcp_default_data[index].status_hbit.em_talk)
		{
			return 1;
		}
	}
	
	return 0;	
}

//***********************************************************************************************************************
//��������:���ƺл�ȡ�Խ�����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
/*
uint8 dcp_get_talk_req(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		if(dcp_default_data[index].status_hbit.talk_req)
		{
			return 1;
		}
	}
	
	return 0;	
}
*/

//***********************************************************************************************************************
//��������:���ƺл�ȡý����ʾģʽ
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
uint8 dcp_get_media_mode(void)
{
	uint8 index;
	
	for(index=0; index<TBA_DCP_NUM; index++)
	{
		//Ѱ�Ҽ����豸
		if(dcp_default_data[index].status_hbit.active)
		{
			//return dcp_default_data[index].media_mode;
			return 1;
		}
	}
	
	return 0;	
}



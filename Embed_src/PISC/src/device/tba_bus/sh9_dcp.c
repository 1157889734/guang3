
/******************************************************************************
*文件名称：sh9_dcp.c		       		         
*作          者：hk           							 
*描          述：广播控制盒处理		
*创建日期：2010-3-10
*修改记录：无								
******************************************************************************/

#include "include.h"

#define    ZERO_KM_MASK                     0x4000     //零速度屏蔽字
#define    OPEN_LEFT_DOOR_MASK		 0x0100     //开左门屏蔽字
#define    OPEN_RIGHT_DOOR_MASK	 0x1000     //开右门屏蔽字
#define    CLOSE_DOOR_MASK			 0x0004	   //关门屏蔽字

#define   TMS_0KM_STATUS                     ((RD_EXT_IO_STAUTS)&(ZERO_KM_MASK))             //零速度标识
#define   CLOSE_DOOR_STATUS                ((RD_EXT_IO_STAUTS)&(CLOSE_DOOR_MASK))        //门状态
#define   OPEN_LEFT_DOOR_STATUS         ((RD_EXT_IO_STAUTS)&(OPEN_LEFT_DOOR_MASK))   //左门状态
#define   OPEN_RIGHT_DOOR_STATUS       ((RD_EXT_IO_STAUTS)&(OPEN_RIGHT_DOOR_MASK))  //右门状态


//对讲状态
#define    REQ_TALK           0x01      //呼叫状态
#define    RSP_TALK            0x02     //应答状态
#define    URG_TALK_OFF      0x03     //乘客紧急报警挂起
#define    TALK_OFF             0x00    //挂断状态（司机对讲/乘客报警通用）

//录音模块任务
#define   RECORD_TASK_START    0x01     //录音命令
#define   RECORD_TASK_STOP     0x02      //停止命令

#define 	ROUTE_ID_NUM        8     // 目前开放的线路总数

//两个DCP的过程数据结构
dcp_default_data_t dcp_default_data[TBA_DCP_NUM]=
{
	{0,0,0,0,0,0,{0},{0},{0},0,{0},0,},
	{0,0,0,0,0,0,{0},{0},{0},0,{0},0,},
};
//static uint8  pis_work_mode = WORK_AT_TCMS;    //中央控制器工作模式

// 2个DCP通信错误
uint16 dcp_comm_error[TBA_DCP_NUM]={0};

static uint8  record_task = RECORD_TASK_STOP;    //录音模块任务命令
static uint32  record_time = 0;                             //往录音模块发送命令的开始时间
static uint32  record_timeout = 200; 

/****************************************************
*函数名：dcp_proc			       		         
*作者：hk             							 
*描述：广播控制盒数据处理				
*输入：src_net_id       --- 源网络号
                 src_dev_id       ---源设备id
                 data_type        ---数据类型
                 packet            ---数据
                 length             ---数据长度
                  
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
void dcp_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{   
	static uint8 leave_broadcast = 0;    //预报站触发
	static uint8 arrived_broadcast = 0;  //到站触发

	static uint8 stop_broadcast = 0;    // 1:停广播
	static uint8 urgent_broadcast = 0;
	uint8 dev_no = 0;
	uint8 cnt =0;
	dev_no=src_dev_id>>4;//设备号	   

	
	//print_line("dcp1");
	if((data_type==TBA_CMD_DEFAULTPACKET) && (length==sizeof(dcp_default_data_t))) //过程数据
	{			
		//print_line("dcp2");
		if((dev_no) && (dev_no<=TBA_DCP_NUM))//判断源设备编号
		{
		   dev_no = dev_no - 1;			
			dcp_comm_error[dev_no]=0;//清除通信错误			
			memcpy( (uint8 *)&dcp_default_data[dev_no], packet, sizeof(dcp_default_data_t) );//拷贝数据
		}

		//触摸屏音量设置
		/*
		if(dcp_default_data[dev_no].line_mode.moni_broadcast)
		{
			saveparam.dcp_pa_listen_volume = dcp_default_data[dev_no].line_mode.moni_broadcast;
			save_set_task();
		}*/
				

		//判断dcp音量是否= pisc 		
		if((dcp_default_data[dev_no].dcp_pacc_listen&0x0f) != saveparam.dcp_pa_listen_volume
			||(dcp_default_data[dev_no].dcp_pacc_listen&0xf0) != (saveparam.dcp_pc_listen_volume<<4)
			||(dcp_default_data[dev_no].dcp_pcpa_listen&0x0f) != saveparam.dcp_pc_listen_volume
			||(dcp_default_data[dev_no].dcp_pcpa_listen&0xf0) != (saveparam.dcp_pa_mic_volume<<4)
			||(dcp_default_data[dev_no].dcp_ccpc_mic&0x0f) != saveparam.dcp_cc_mic_volume
			||(dcp_default_data[dev_no].dcp_ccpc_mic&0xf0) != (saveparam.dcp_pc_mic_volume<<4))
		{
			#if 0
			if(my_default_data.status_hbit.master==0) // 备机音量值等于广播控制盒
			{
				saveparam.dcp_pa_listen_volume = dcp_default_data[dev_no].dcp_pacc_listen & 0x0f;
				saveparam.dcp_pc_listen_volume = (dcp_default_data[dev_no].dcp_pacc_listen >> 4) & 0x0f; 
				saveparam.dcp_pc_listen_volume = (dcp_default_data[dev_no].dcp_pcpa_listen&0x0f);
				saveparam.dcp_pa_mic_volume = (dcp_default_data[dev_no].dcp_pcpa_listen>>4)&0x0f;
				saveparam.dcp_cc_mic_volume = (dcp_default_data[dev_no].dcp_ccpc_mic&0x0f);
				saveparam.dcp_pc_mic_volume = (dcp_default_data[dev_no].dcp_ccpc_mic>>4)&0x0f;	
				if(other_default_data.dcp_volume&0xf0 ==0)  //主机没有修改音量值
				{
					save_set_task();
				}
			}
			else
			#endif
			if(my_default_data.status_hbit.master)  //修改广播盒音量
				sh9_tba_bus_dev.dcp_volume_set = 1;
		}

		//print_int("%dmode:",sh9_tms_dev.pis_work_mode);

		/*****************手动模式处理*********************/		
		if(dcp_default_data[dev_no].status_hbit.active != 1)  //非激活端广播控制盒
		    return;

	
		// if(sh9_tms_dev.pis_work_mode == WORK_AT_MANU)  //不管什么模式都响应触摸屏操作
		{
              	//sh9_tms_dev.pis_work_mode = WORK_AT_MANU ;
	  		//sh9_tms_dev.run_dir = dcp_default_data[dev_no].status_hbit.run_dir;//提取上下行状态 DCP 0:上行  1:下行
			sh9_tms_dev.city_id = dcp_default_data[dev_no].line_mode.city_id;

			/*
				如果触摸屏给的起始站信号和终点站信号是触发，可以在自动模式下修改线路；
				如果是一直给信号，则不允许修改线路；
				目前是一直给
			*/
			if(!sh9_tms_dev.pis_work_mode)  //自动模式下，不允许修改线路 
			{
				if((sh9_tms_dev.start_station != dcp_default_data[dev_no].start_station)
					|| (sh9_tms_dev.end_station != dcp_default_data[dev_no].end_station)) //线路变化
				{
					if( dcp_default_data[dev_no].start_station) 
						sh9_tms_dev.start_station = dcp_default_data[dev_no].start_station;	
					if( dcp_default_data[dev_no].end_station) 
					{
						if(sh9_tms_dev.end_station != dcp_default_data[dev_no].end_station)//修改了终点站
							end_station_code = 0;
						sh9_tms_dev.end_station = dcp_default_data[dev_no].end_station;						
					}									
				}
			}
			if( dcp_default_data[dev_no].next_station)  //下一站
				sh9_tms_dev.next_station = dcp_default_data[dev_no].next_station;
			

			//sh9_tms_dev.current_station =  dcp_default_data[dev_no].current_station;


			//手动预报站
			if(leave_broadcast ==0 && dcp_default_data[dev_no].broadcast_hbit.leave_broadcast)
			{
				sh9_tms_dev.arrived = 0;				
				tms_dev->set_broadcast(1); //预报站广播
				next_station_code = sh9_tms_dev.next_station ; //防止手动报站后，重报
				
				my_default_data.trigger_hbit.leave_broadcast=1;

				#if 1			
				if((sh9_tms_dev.door_sides >>7) ==0)//suven  修改手动模式下开门侧
				{
					sh9_tms_dev.door_sides = saveparam.door_sides_up[sh9_tms_dev.next_station -1] ;
				}
				#endif	
			}
			leave_broadcast = dcp_default_data[dev_no].broadcast_hbit.leave_broadcast;

			//手动到站广播
			if(arrived_broadcast == 0 && dcp_default_data[dev_no].broadcast_hbit.arrived_broadcast)
			{
				//station_broadcast = 2;   //到站广播
				tms_dev->set_broadcast(2);
				sh9_tms_dev.arrived = 1;
				//sh9_tms_dev.current_station =sh9_tms_dev.next_station ; 	//更新当前站	
				my_default_data.trigger_hbit.arrived_broadcast=1;
			}
			arrived_broadcast = dcp_default_data[dev_no].broadcast_hbit.arrived_broadcast;

			//紧急广播
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
			   sh9_tms_dev.emergency_status = 1; //停止紧急广播标志
			   print_line("stop emergency br");
			}*/
			urgent_broadcast = dcp_default_data[dev_no].broadcast_hbit.urgent_broadcast;

			//停止广播
			if(stop_broadcast == 0 && dcp_default_data[dev_no].broadcast_hbit.stop_broadcast)
			{
			   dva_dev->stop();
			   sh9_tms_dev.emergency_status = 1; //停止紧急广播标志
			   print_line("stop dva br");
			}
			stop_broadcast = dcp_default_data[dev_no].broadcast_hbit.stop_broadcast;

			/*/静音处理
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
	//其它数据
	else
	{
		return;
	}
	//录音处理
	//dcp_record(dev_no);
}

/****************************************************
*函数名：dcp_record			       		         
*作者：hk             							 
*描述：录音处理				
*输入：dev_no           ---设备号                  
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
void  dcp_record(uint8 dev_no)
{
    static uint8 pre_talk_status[32] = {0};  //司机对讲状态1，司机对讲状态2
    uint8 talk_status[32] = {0};
    uint8 buf[64] = {0};
    static uint8 stop_times = 0;
    static uint8 start_times = 0;
    uint8 temp = 0;
    uint8 icc_num = 0;
    uint8 rtc_time[5] = {0};
    static uint8 driver_to_passager_flag = 0;


    //没钥匙端广播盒数据不处理
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

	   //司机对讲状态由应答状态 -->  挂断 ,发送停止录音
	  if(((pre_talk_status[icc_num] == RSP_TALK) && ((talk_status[icc_num] == TALK_OFF) || (talk_status[icc_num] == URG_TALK_OFF)))&& (record_task == RECORD_TASK_START))
	  {
	        buf[0] = 0x08;   //停止录音命令
	        record_task = RECORD_TASK_STOP;
		 record_time = get_clock();
		 cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 1, buf);  //发送停止录音命令
		 print_line("stop record");
		 record_timeout = 200;      //record_timeout时间内没收到回复，则重新发送
		 stop_times++;
		 start_times = 0;

		 driver_to_passager_flag--;
	  }

	  
        //司机对讲状态由呼叫状态--> 应答状态 表示已开始对讲，可进行录音
        if((talk_status[icc_num]== RSP_TALK) && (pre_talk_status[icc_num] != RSP_TALK) &&(record_task != RECORD_TASK_START))   
        {
              rtc_dev_p->get_rtc(rtc_time);
		 tms_dev->month = (rtc_time[1] >> 4)*10 +(rtc_time[1]&0x0f);
		 tms_dev->date =  (rtc_time[2] >> 4)*10 +(rtc_time[2]&0x0f);
            	 buf[0] = 0x07;   //录音命令
	        buf[1] = tms_dev->month; //月份
		 buf[2] = tms_dev->date;   //日期
	        record_task = RECORD_TASK_START;
		 record_time = get_clock();
		 cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 3, buf);  //发送录音命令
		 print_line("start record");
		 record_timeout = 200;      //record_timeout时间内没收到回复，则重新发送
		 start_times++;
		 stop_times = 0;
		 
		 driver_to_passager_flag++;
         }	
	pre_talk_status[icc_num] = talk_status[icc_num];
   }

    //任务状态为停止录音，录音模块当前状态不处在停止录音状态
    if((record_task == RECORD_TASK_STOP) && (dva_dev->get_record_status() ))
    {
        if(timer0_check_timer(&record_time, record_timeout)&& (stop_times < 3))
        {
            buf[0] = 0x08;   //停止录音命令
            record_timeout = record_timeout + 200;
	     record_time = get_clock();
    
           cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 1, buf);  //发送停止录音命令
           print_line("stop record+");
	    stop_times++;
        }
     }

     //任务状态为开始录音，录音模块当前状态不处在录音状态
    if((record_task == RECORD_TASK_START) && (dva_dev->get_record_status() != 1))
    {
        if(timer0_check_timer(&record_time, record_timeout) && (start_times < 3))
        {
            rtc_dev_p->get_rtc(rtc_time);
	     tms_dev->month = (rtc_time[1] >> 4)*10 +(rtc_time[1]&0x0f);
	     tms_dev->date =  (rtc_time[2] >> 4)*10 +(rtc_time[2]&0x0f);
            buf[0] = 0x07;   //录音命令
            buf[1] = tms_dev->month; //月份
            buf[2] = tms_dev->date;   //日期
            record_timeout = record_timeout + 200;
	     record_time = get_clock();

            cab_bus_dev->send_data(0x07,RECORD_DEVICE_NO, 1, 3, buf);  //发送录音命令
            print_line("start record+");
	     start_times++;
         }
     }
	
}

//***********************************************************************************************************************
//函数作用:控制盒获取媒体播放开关
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
uint8 dcp_get_media_on(void)
{
	uint8 index;
	
	for(index=0;index<TBA_DCP_NUM;index++)
	{
		//寻找激活设备
		if(dcp_default_data[index].status_hbit.active)
		{
			//return dcp_default_data[index].status_hbit.media_on;
			return 1;
		}
	}
	
	return 0;
}

//***********************************************************************************************************************
//函数作用:控制盒获取广播请求
//参数说明:
//注意事项:
//返回说明:
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
//函数作用:控制盒获取广播状态
//参数说明:
//注意事项:
//返回说明:
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
//函数作用:控制盒获取对讲状态
//参数说明:
//注意事项:
//返回说明:
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
//函数作用:控制盒获取对讲请求
//参数说明:
//注意事项:
//返回说明:
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
//函数作用:控制盒获取媒体显示模式
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
uint8 dcp_get_media_mode(void)
{
	uint8 index;
	
	for(index=0; index<TBA_DCP_NUM; index++)
	{
		//寻找激活设备
		if(dcp_default_data[index].status_hbit.active)
		{
			//return dcp_default_data[index].media_mode;
			return 1;
		}
	}
	
	return 0;	
}



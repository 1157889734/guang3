
/******************************************************************************
*文件名称：station.c		       		         
*作          者：hk           							 
*描          述：过程数据组装
*创建日期：2010-3-12
*修改记录：无								
******************************************************************************/


#include "include.h"


uint8 temp_door_open_side = 0;
/****************************************************
*函数名：station_proc			       		         
*作者：hk             							 
*描述：过程数据组装	
*输入：无      
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
void station_proc(void)
{	
	static uint8 pis_version_flag = 0 ;
	//触发标志
	my_default_data.trigger_hbit.jump=0;//越站
	//my_default_data.trigger_hbit.opendoor=(tms_dev->door_sides!=0);
	//my_default_data.trigger_hbit.closedoor=(tms_dev->door_sides==0);
	my_default_data.trigger_hbit.leave=!tms_dev->arrived;//离站
	my_default_data.trigger_hbit.reach_stop=tms_dev->arrived;//到站停稳
	
	//更新站
	my_default_data.start_station=tms_dev->start_station;
	my_default_data.end_station=tms_dev->end_station;
	my_default_data.current_station=tms_dev->current_station;
	my_default_data.next_station=tms_dev->next_station;
	
	//越站广播代码
	my_default_data.jump_broadcast_no=0;
	
	my_default_data.status_hbit.jump_valid=1;//越站有效
	my_default_data.status_hbit.dir_valid=1;//上下行有效

	my_default_data.status_hbit.door_led_sides=tms_dev->door_sides;//开门侧

	if(my_default_data.status_hbit.door_led_sides !=0)// 体育西开门侧处理
	{
		if((sh9_tms_dev.next_station == 11)&&(sh9_tms_dev.next_station == sh9_tms_dev.end_station))
			my_default_data.status_hbit.door_led_sides = 0x03;
	}
	//my_default_data.tms_status.tms_mode = sh9_tms_dev.pis_work_mode;
	//my_default_data.cab_status.pis_mode =  sh9_tms_dev.pis_work_mode;

	//如果二号激活，门侧取反，或者一号为主，但没有激活，门侧也取反
	#if 0
	if( ((read_pisc_id() ==2) && (my_default_data.status_hbit.active 
		||(other_default_data.status_hbit.active == 0 && (error_pack.status1 & 0x40 == 0)))) 
		|| ((read_pisc_id() == 1) && (!my_default_data.status_hbit.active 
		&& (other_default_data.status_hbit.active  && (error_pack.status1 & 0x80 == 0)))))
	#else
	if( ((read_pisc_id() ==2) && my_default_data.status_hbit.active) 
		|| ((read_pisc_id() == 1) && !my_default_data.status_hbit.active) )
	#endif
	{
		if(my_default_data.status_hbit.door_led_sides == 1)
			my_default_data.status_hbit.door_led_sides = 2;
		else if(my_default_data.status_hbit.door_led_sides == 2)
			my_default_data.status_hbit.door_led_sides = 1;
	}
	
	my_default_data.status_hbit.dir_down=(tms_dev->run_dir)?0:1;//下行
	my_default_data.status_hbit.dir_up=(tms_dev->run_dir)?1:0;//上行


	//触摸屏故障信息
	my_default_data.error_data.status1 = error_pack.status1;
	my_default_data.error_data.status2 = error_pack.status2;
	my_default_data.error_data.status3 = error_pack.status3;
	//越站代码
	//my_default_data.jump_stations=0;	

	//------------------------对讲处理-------------------------------
	//把报警器呼叫状态更新到过程数据
	lcu_get_icc_state(my_default_data.icc_status);

	//把报警器状态更新到TMS数据
	tms_dev->set_pecu(my_default_data.icc_status); 

	//发送过程数据版本号
	if(my_default_data.status_hbit.master)
	{
		if(pis_version_flag)
		{
			my_default_data.pis_version = MINOR_VERSION | (MAJOR_VERSION<<4);
			if((read_pisc_id() == 1))
				my_default_data.pis_version &= 0x7f;   
			else
				my_default_data.pis_version |= 0x80;
			pis_version_flag = 0;
		}
		else
		{
			if(other_default_data.pis_version)
				my_default_data.pis_version = other_default_data.pis_version ; 
			else
				my_default_data.pis_version = MINOR_VERSION | (MAJOR_VERSION<<4);
			
			if((read_pisc_id() == 2))
				my_default_data.pis_version &= 0x7f;
			else
				my_default_data.pis_version |= 0x80;
			pis_version_flag = 1;
		}
	}
	else
		my_default_data.pis_version = MINOR_VERSION | (MAJOR_VERSION<<4);	

	//给监听过程数据
	if((my_default_data.dcp_volume&0xf0 )==0  
		&& sh9_tba_bus_dev.dcp_volume_set == 0)
			my_default_data.dcp_volume = saveparam.dcp_pa_listen_volume & 0x0f; //监听音量小于15
}



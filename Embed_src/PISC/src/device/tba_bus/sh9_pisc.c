
/******************************************************************************
*文約名称 ：pisc.c		       		         
*作          者 ：thomson   							 
*描          述 ：中央控制器	
*创建日期：2010-3-10
*GS改记录：无							
******************************************************************************/


#include "include.h"



//过程数据
pisc_default_data_t other_default_data=
{
	{0,0,0,0,0,0,0,0,},//触发标志
	1,//起始站
	STATION_TOTAL_NUM,//终点站
	1,//当前站
	2,//下一站
	0,//越站广播
	0,//紧急广播
	{1,0,0,1,0,0,1},//状态标志
	0,0,0,0,//越站代码
	{0},//广播优先级
	{0},//广播信源
	{{0},{0},{0},{0},{0},{0},{0},{0},},//32个报警器状态字
	
	0,//媒体显示模式
	{0},//司机室状态
	{0},//TMS状态
	
	0,//面板设置的音量参数
	
	0,//运行的距离---百分比
	1,//经过的站--不一定停车
	
	0,//保留
};


//通信错误
uint16 pisc_comm_error[TBA_PISC_NUM]={0};

/****************************************************
*函蔵名：	pisc_proc 		         
*作者：thomson             							 
*描述：諫央控制器蔵綢处理
*输入：蜸                 
*输出：	
*返回值：蜸
*GS改记录：蜸								
****************************************************/
void pisc_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
       static uint8 other_pisc_master = 0;
	//过程数据
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		//判断长度
		if(length==sizeof(pisc_default_data_t))
		{	
			uint8 dev_no=src_dev_id>>4;
			
			//判断源设备编号
			if((dev_no) && (dev_no<=TBA_PISC_NUM))
			{
				//清除通信错误
				pisc_comm_error[dev_no-1]=0;
				
				//拷贝对方蔵綢
				//if(dev_no!=((DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))?1:2))
				if(dev_no!= read_pisc_id())		//xi'an Line NO.2
				{	
					memcpy( (uint8 *)&other_default_data, packet, sizeof(pisc_default_data_t) );

					//同步广播监听、对讲、麦克风音量					
					if(other_default_data.status_hbit.master && other_default_data.dcp_volume&0xf0 !=0)
					{
						switch(other_default_data.dcp_volume&0xf0)  //对侧在设置音量
						{							
							case 1:
								saveparam.dcp_pa_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;
								
							case 2:
								saveparam.dcp_cc_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 3:
								saveparam.dcp_pc_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 4:
								saveparam.dcp_pa_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 5:
								saveparam.dcp_cc_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 6:
								saveparam.dcp_pc_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							default:
								break;
						}
					}
					else if(my_default_data.status_hbit.master) //本端主，检测对端监听音量
					{
						if((other_default_data.dcp_volume & 0x0f) != saveparam.dcp_pa_listen_volume)
							sh9_tba_bus_dev.dcp_volume_set = 1;
					}
					
					

                               	if(other_default_data.test_status.master_req || other_default_data.test_status.slave_req)
				          tba_bus_dev->data_flag = 0;
							   
					if(other_default_data.status_hbit.master && my_default_data.test_status.master_req == 0
					   && my_default_data.tms_status.tms_valid)//主的连续次数,两边都有tms，当本端tms故障才同步
					{
					    other_pisc_master = (++other_pisc_master)%200;
					}
					else
						other_pisc_master = 0;

					//手动模式下,对侧为主，同步当前站和下一站
					if((other_pisc_master > 10) && (WORK_AT_AUTO != sh9_tms_dev.pis_work_mode))
					{
						 other_pisc_master = 0;
						 if(sh9_tms_dev.next_station != other_default_data.next_station)
						 {
						     ptu_print_debug("sync with master---next_station");
						     sh9_tms_dev.next_station = other_default_data.next_station;
						 }
						if(sh9_tms_dev.current_station != other_default_data.current_station)
						{
						   sh9_tms_dev.current_station = other_default_data.current_station;
						   ptu_print_debug("sync with master---current_station");
						}
						if(sh9_tms_dev.start_station != other_default_data.start_station)
						 {
						     ptu_print_debug("sync with master---start_station");
						     sh9_tms_dev.start_station = other_default_data.start_station;
						 }
						if(sh9_tms_dev.end_station != other_default_data.end_station)
						{
						   sh9_tms_dev.end_station = other_default_data.end_station;
						   ptu_print_debug("sync with master---end_station");
						}
					}
					//uart0_write_char(0x99);
				       //uart0_write_buf((uint8 *)&other_default_data, sizeof(pisc_default_data_t) );
				}
				//if(other_default_data.status_hbit.master)print_line("get other master");
				//else print_line("get other slave");
			}
		}
	}
	//点对点
	else if(data_type==TBA_CMD_P2PPACKET)
	{
		
	}
}
/****************************************************
*函蔵名：read_pisc_id       		         
*作者：thomson             							 
*描述：读PISC 的ID  
bit IN9 IN6  ID
	0    1    1
	1    0    2
*输入：蜸                 
*输出：蜸	
*返回值：PISC ID
*GS改记录：蜸								
****************************************************/
uint8 read_pisc_id(void)
{
	uint8 dev_pisc_id = 0x0;
	if(!(DEV_ID_DET_REG &(0x0001<<DEV_ID_DET_BIT0)))
		dev_pisc_id =1;
	else if((DEV_ID_DET_REG &(0x0001<<DEV_ID_DET_BIT0)))
		dev_pisc_id =2;
	return 	dev_pisc_id;
	//return 1;
}



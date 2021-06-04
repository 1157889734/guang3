
/******************************************************************************
*文件名称：sh9_dva.c		       		         
*作          者：hk           							 
*描          述：数字报站器处理		
*创建日期：2010-3-10
*修改记录：无								
******************************************************************************/

#include "include.h"



//通信协议关键字
#define DVA_PACKET_HEAD		0x7E	//命令头
#define DVA_PACKET_END		0x0D	//命令尾
#define DVA_PACKET_PLAY		0x01	//播放命令

#define DVA_PACKET_STOP		0x02	//停止命令
#define DVA_PACKET_SPECIAL	0x01	//特殊广播
#define DVA_PACKET_VERSION	0x04	//获取版本号
#define DVA_PACKET_STATUS	0x05	//获取状态
#define DVA_PACKET_VOLUME	0x06	//设置音量

//录音模块命令
#define RECORD_START            0x07      //开始录音
#define RECORD_STOP              0x08     //停止录音

//最多发送次数
#define SEND_TIMES_MAX			7

//任务状态字
#define DVA_TASK_IDLE			0	//空闲
#define DVA_TASK_SEND_PLAY		1	//发送播放命令
#define DVA_TASK_SEND_PREVSTOP		2	//发送预停止命令---因为正在播放其他内容
#define DVA_TASK_SEND_STOP		3	//发送停止命令

//DVA状态表示
//#define DVA_BUSY                1
#define DVA_IDLE                0
#define DVA_PLAYING				1
#define DVA_STOP				2
//调试状态定义 
#define DVA_DEBUG

//接收缓冲区
static uint8 dva_recv_buf[DVA_BUF_SIZE];//接收缓冲区
//static uint8 dva_recv_length=0;//接收总长度
//static uint8 dva_data_length=0;//数据部分的长度

static uint16 dva_list_buf[DVA_LIST_MAX];//播放列表缓冲区
static uint8 dva_list_length=0;//播放列表长度
static uint8 dva_play_type=0;//播放类型

static uint8 volatile dva_task_status=DVA_TASK_IDLE;//任务状态字
static uint16 dva_send_times=0;//重发的次数
static uint16 dva_wait_time=0;//等待时间
static uint8 dva_version=0;//版本号
static uint8 dva_status=0;//DVA设备状态

static uint8 record_status = 0;    //录音模块的当前状态
//static uint8 play_status = 0;

//查询DVA定时器
//static uint32 dva_check_status_timer=0;
//通讯错误处理
uint8 dva_comm_err[DVA_NUM]={0};
//定义司机室二级总线数据结构
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
*函数名：dva_send_cmd			       		         
*作者：hk             							 
*描述：数字报站器发送命令				
*输入：cmd       --- 命令类型
                 param    ---发送的数据
                 length    ---数据长度
                  
*输出：无	
*返回值：0:failed  1:succeed
*修改记录：无								
****************************************************/
static uint8 dva_send_cmd(uint8 cmd, uint8 *param, uint8 length)
{
	uint8 buf[DVA_BUF_SIZE+1]={0};

	//begin add by hk 2010-5-24   for 新语音文件命名规则
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
*函数名：dva_proc			       		         
*作者：hk             							 
*描述：数字报站器数据处理				
*输入：device_no       --- 设备号
                 data_length    ---数据长度
                 buf                ---处理的数据
                  
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
 void dva_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{

//	play_status = 0;
	//复制DVA数据
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
			//开始录音
			record_status = 1;
			break;

			case RECORD_STOP:
			//停止录音
			record_status = 0;
			break;

			default:
		      break;
		 }
	    }
	    return;
      	}

	 //清除通讯错误
	//dva_comm_err[device_no-1]=0;
	sh9_dva_dev.comm_error=0;
	   
	switch(cab_dva_packet->cmd)
	{
		case DVA_PACKET_STATUS:
		dva_status=	cab_dva_packet->dva_data[2];   //正在播音否
             // if(cab_dva_packet->dva_data[2])
		debug_print_int("%d  9",cab_dva_packet->dva_data[2]);
	       //print_int("%d  status",cab_dva_packet->dva_data[1]);
	       //print_int("%d  status",cab_dva_packet->dva_data[2]);
	       //如果当前为紧急广播且找不到播放文件，则停止播放紧急广播
	       if(my_default_data.trigger_hbit.urgent_broadcast && cab_dva_packet->dva_data[1])
	       {
	           dva_dev->stop();
		     dva_list_length = 0;
		    sh9_tms_dev.emergency_status = 1; //停止紧急广播标志
	       }
		break;
		
		case DVA_PACKET_STOP:          //收到DVA返回停止命令，将DVA置为空闲
		dva_status=DVA_STOP;
		debug_print_line("dva stop");
		break;
		
		case DVA_PACKET_VERSION:
		//dva_version=cab_bus_packet->cab_data[0];
		break;
		
		default:
		break;
	}

	//任务状态字分类处理
	switch(dva_task_status)
	{
		//空闲
		case DVA_TASK_IDLE:
		//print_line("IDLE");
			break;

		//发送播放命令
		case DVA_TASK_SEND_PLAY:
                    
			//如果没有开始播放 一直发送播放命令
			if(dva_status==DVA_IDLE)
			{
				//dva_status=DVA_PLAYING;
				//等待dva处理
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
				      //发送dva_send_times次不成功 切换到空闲状态
                                  
					dva_task_status=DVA_TASK_IDLE;
					#if NAME_RULE_NEW
					   dva_list_length = 0;
					#endif
					print_line("send error");
					debug_print_int("%d  8",cab_dva_packet->dva_data[2]);
				}
			}
			//如果开始播放了 说明任务成功 切换到空闲状态
			else
			{
			    
				dva_task_status=DVA_TASK_IDLE;
				#if NAME_RULE_NEW
				    dva_list_length = 0;
				#endif
				debug_print_line("dva playing");
			}
			break;
			
		//发送预停止命令---停止正在播放的
		case DVA_TASK_SEND_PREVSTOP:
		
			//如果没有停止 一直发送停止命令
			if(dva_status==DVA_PLAYING)
			{
				//dva_status==DVA_IDLE;
				//等待dva处理
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
					//强制切换到发送播放命令状态
					dva_task_status=DVA_TASK_SEND_PLAY;
					//送发送次数
					dva_send_times=1;
					dva_wait_time=0;
					print_line("dva prestop error");
				}
			}
			else
			{

				debug_print_line("dva prestop ok");
				
				dva_task_status=DVA_TASK_SEND_PLAY;
				//送发送次数
				dva_send_times=7;
				dva_wait_time=0;
			}
			break;
			
		//发送停止命令
		case DVA_TASK_SEND_STOP:
			//如果没有停止 一直发送停止命令
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
			//如果停止了 说明任务成功 切换到空闲状态
			else
			{
				dva_task_status=DVA_TASK_IDLE;
				//sh9_dva_dev.comm_error=0;
				debug_print_line("dva stop");
			}
			break;
		
		//其他状态
		default:
			break;
	}
	
		
}

/****************************************************
*函数名：dva_play_list			       		         
*作者：hk             							 
*描述：播放文件加入数字报站器播放列表			
*输入：list       --- 播放文件
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void dva_play_list(uint16 *list)
{
	//拷贝播放列表
	dva_list_length=0;
	while(*list)
	{
		dva_list_buf[dva_list_length++]=*list++;
		if(dva_list_length>=DVA_LIST_MAX-1)break;
	}
	//送播放类型
	dva_play_type=DVA_PACKET_PLAY;
	//送任务状态
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//送发送次数
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;

}

/****************************************************
*函数名：dva_play_list_new			       		         
*作者：hk             							 
*描述：播放文件加入数字报站器播放列表			
*输入：list       --- 播放文件
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void dva_play_list_new(uint16  list)
{
	//拷贝播放列表

	dva_list_buf[dva_list_length++] = list;

	print_int("%dplay:" , list);
	
	//送播放类型
	dva_play_type=DVA_PACKET_PLAY;
	//送任务状态
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//送发送次数
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
}

/****************************************************
*函数名：dva_play_special			       		         
*作者：hk             							 
*描述：播放特殊广播		
*输入：play_code       --- 特殊广播代码
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void dva_play_special(uint16 play_code)
{
	//拷贝播放列表
	dva_list_buf[dva_list_length++]=play_code;
	//dva_list_length=1;
	//送播放类型
	dva_play_type=DVA_PACKET_SPECIAL;
	//送任务状态
	dva_task_status=DVA_TASK_SEND_PREVSTOP;
	//送发送次数
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
	//查询DVA当前状态
	//dva_send_cmd(DVA_PACKET_STATUS,NULL,0);
}

/****************************************************
*函数名：dva_stop			       		         
*作者：hk             							 
*描述：停止dva播放		
*输入：无
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void dva_stop(void)
{
	//送任务状态
	dva_task_status=DVA_TASK_SEND_STOP;
	//送发送次数
	dva_send_times=SEND_TIMES_MAX;
	dva_wait_time=0;
}


/****************************************************
*函数名：dva_read_version			       		         
*作者：hk             							 
*描述：获取dva 版本号
*输入：无
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static uint8 dva_read_version(void)
{
	dva_send_cmd(DVA_PACKET_VERSION,NULL,0);
	return dva_version;
}

/****************************************************
*函数名：dva_read_status			       		         
*作者：hk             							 
*描述：获取dva 状态
*输入：无
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static uint8 dva_read_status(void)
{
	dva_send_cmd(DVA_PACKET_STATUS,NULL,0);
		
	return dva_status;
}


/****************************************************
*函数名：dva_set_volume			       		         
*作者：hk             							 
*描述：设置dva音量
*输入：volume  ---  音量大小
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void dva_set_volume(uint8 volume)
{
	dva_send_cmd(DVA_PACKET_VOLUME,&volume,1);
}

/****************************************************
*函数名：dva_get_playing			       		         
*作者：hk             							 
*描述：数字报站器获取播放状态
*输入：无
*输出：无	
*返回值：1:播放状态  0:空闲	
*修改记录：无								
****************************************************/
static uint8 dva_get_playing(void)
{	
	//只要有任务 强制返回1
	if(dva_task_status)
		return 1;

	//只要有任务 强制返回1
	//if(dva_task_status == DVA_TASK_SEND_PLAY)
	//	return 1;
	
	//if(DVA_PLAYING_DET_REG&(0x0001<<DVA_PLAYING_DET_BIT))return 0;
	if(dva_status==DVA_PLAYING) 
		return 1;
	 return 0;
}

/****************************************************
*函数名：dva_clear			       		         
*作者：hk             							 
*描述：清除DVA状态和播放列表
*输入：无
*输出：无	
*返回值：
*修改记录：无								
****************************************************/
static void dva_clear(void)
{	
	dva_task_status = DVA_TASK_IDLE;
	dva_status = DVA_IDLE;
      dva_list_length = 0;
}

/****************************************************
*函数名：dva_get_record_status			       		         
*作者：hk             							 
*描述：返回录音模块的当前状态
*输入：无
*输出：无	
*返回值：1:录音状态  0:停止状态
*修改记录：无								
****************************************************/
static uint8 dva_get_record_status(void)
{
    return record_status;
}

//输出虚拟设备类
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
	0,//dva 串口通信错误
	0,//录音模块错误
};


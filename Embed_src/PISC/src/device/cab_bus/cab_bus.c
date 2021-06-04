
/******************************************************************************
*文件名称：cab_bus.c		       		         
*作          者：hk           							 
*描          述：司机室通信总线接口处理		
*创建日期：2010-3-10
*修改记录：无								
******************************************************************************/
#include "include.h"

#define SEND_DEFAULT	0
#define SEND_SPECIAL	1
//#define DVA_DEVICE_NO			0x01   //DVA 模块设备号
//#define DEBUG
      
//接收缓冲区
static uint8 cab_bus_recv_buf[CAB_BUF_SIZE];
//static uint16 cab_bus_recv_length;
static uint8 start_flag=0;
static uint8 recv_length=0;
static uint8 cab_recv_sum=0;

//发送缓冲区
static uint8 cab_default_send_buf[CAB_BUF_SIZE];
static uint8 cab_special_send_buf[CAB_BUF_SIZE];
static uint16 cab_bus_send_length=0;
//extern uint8 detect_dva_flag;
//数据发送标志
static uint8 special_data_send=0;		//特殊数据发送标志位，
//static uint8 default_data_send=0;		//过程数据发送标志位

static uint8 cab_bus_debug = 0;

//查询DVA定时器
static uint32 dva_check_status_timer=0;

//static uint8 cab_recv_data_lenth=0;
//设备处理列表结构
__packed typedef struct 
{
	uint8 device_type;
	void (*proc)(uint8,uint8,uint8 *);
}cab_proc_list_t;

static cab_bus_packet_t *cab_recv_packet=(cab_bus_packet_t *)cab_bus_recv_buf;

//设备处理列表
static cab_proc_list_t  const cab_device_proc_list[]=
{
	{CAB_DVA,dva_proc},
	//{CAB_PANEL,panel_proc},
	//{CAB_CCTV,cctv_proc},
	{0,NULL},
};

/****************************************************
*函数名：cab_bus_send_data			       		         
*作者：hk             							 
*描述：司机室总线发送数据
*输入：device_type        ---设备类型
                  device_no          ---设备编号
                  data_type          ---数据类型
                  data_length        ---数据长度
                  buf                    ---数据缓冲区
*返回值：1:succeed   0:failed
*修改记录：无								
****************************************************/
static uint8 cab_bus_send_data(uint8 device_type,uint8 device_no,uint8 data_type,uint8 data_length, uint8 *buf)
{
    cab_bus_packet_t cab_send_packet;				//发送数据
    uint8 temp = 0;
	uint8 *send_data_p;								//发送数据指针
	//uint8 *send_length_p=&cab_bus_send_length;                           //发送数据长度指针
	#if DVA_OLD //old dva
	uint8 *add_data_p=(uint8 *)&cab_send_packet.cab_data;
	#endif
	#if DVA_NEW //new dva
	uint8 *add_data_p=(uint8 *)&cab_send_packet.device_type;
	#endif
	
	uint8 checksum=0;
	if(data_type==SEND_SPECIAL&&special_data_send)
		return 0; //单次数据未发出，返回0
	
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
	
	memcpy((uint8 *)&cab_send_packet.cab_data,buf,data_length);				//复制数据到发送数据包
	
	if(data_type==SEND_SPECIAL)
	{
		send_data_p=cab_special_send_buf;
		special_data_send=1;
	}
	else if(data_type==SEND_DEFAULT)
	{	
		send_data_p=cab_default_send_buf;
		
	}
	#if DVA_NEW     //add later   在前20个字节加0
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
	#if DVA_NEW     //add later   在前20个字节加0
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
*函数名：cab_bus_get_packet			       		         
*作者：hk             							 
*描述：司机室总线接收数据,保存在cab_bus_recv_buf缓冲区里
*输入：device_type        ---设备类型
                  device_no          ---设备编号
                  data_type          ---数据类型
                  data_length        ---数据长度
                  buf                    ---数据缓冲区
*返回值：1:succeed   0:failed
*修改记录：无								
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
		else if(start_flag==1)             //接收数据长度
		{
			start_flag++;
			cab_bus_recv_buf[recv_length++]=temp;  
		}
		else if(start_flag>1)
		{
			if(recv_length<(cab_recv_packet->data_length+1))
			{
				cab_bus_recv_buf[recv_length++]=temp;        //接收数据
				cab_recv_sum+=temp;                           //计算校验和
			}
			else
			{
				start_flag=0;
				if(cab_recv_sum==temp)        //判断校验和是否正确
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
*函数名：cab_bus_get_packet			       		         
*作者：hk             							 
*描述：司机室总线处理,根据设备类型调用相应的处理函数
*输入：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void cab_bus_proc(void)
{	
//       static uint8 first_time = 0;    //dva 刚启动
	static cab_proc_list_t const *list;				//设备处理列表
	static uint8 cnt = 0;
	
	uint8 buf[CAB_BUF_SIZE]={0};
	//cab_bus_debug =3;//test
	if(cab_bus_get_packet())       //如果收到数据，处理cab_bus上接收到的数据
	{
	       #if DVA_NEW   //new dva
		for(list=cab_device_proc_list;list->proc;list++)
		{
		      //根据设备类型，调用相应的设备处理函数
			if(list->device_type==cab_recv_packet->device_type)
				list->proc(cab_recv_packet->device_no,(cab_recv_packet->data_length-2),(uint8 *)&cab_recv_packet->cab_data);	
		}
		#endif
		#if DVA_OLD   //old dva
		dva_proc(0,(cab_recv_packet->data_length),(uint8 *)&cab_recv_packet->cab_data);
		#endif
		//uart0_write_buf((uint8 *)&cab_recv_packet->cab_data, (cab_recv_packet->data_length));
		//print_buf((uint8 *)&cab_recv_packet->cab_data, (cab_recv_packet->data_length));
		//打印调试信息
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
	//每100ms查询一次DVA状态
	buf[0]=0x05;
	buf[1]=0;
	//if(checktimer(&dva_check_status_timer,1100) && special_data_send ==0)
	if(checktimer(&dva_check_status_timer,150))
	{
		//cab_bus_dev->send_data(CAB_DVA,DVA_DEVICE_NO,1,1,buf);
		//sh9_dva_dev.comm_error++;
		//debug_print_line("a");

		if(cnt == 30)//巡检录音模块
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
		
		else	if((cnt%2) == 0) //巡检dva
		{
			cab_bus_dev->send_data(CAB_DVA,DVA_DEVICE_NO,1,1,buf);
			sh9_dva_dev.comm_error++;
			if(sh9_dva_dev.comm_error > COMM_DVA_TIMEOUT)
				sh9_dva_dev.comm_error = COMM_DVA_TIMEOUT;

                	my_default_data.cab_status.dva_error = 0;
		   	//dva 长时间没有回复清除dva状态和播放列表
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

	//发送数据
	if(special_data_send)        
	{
		special_data_send=0;
		CAB_BUS_SEND_BUF(cab_special_send_buf,cab_bus_send_length);

             //打印调试信息
		if((cab_bus_debug == 1)|| (cab_bus_debug == 3))
             {
                 uart0_write_buf(cab_special_send_buf,cab_bus_send_length);
		}
		debug_print_line("6");
	}
	
	//通信错误处理
	/*if(++comm_err_start_timer>50000)
	{
		uint8 index;
		for(index=0;index<DVA_NUM;index++)
		{
		
		}
	}*/
	
}

/****************************************************
*函数名：cab_bus_init			       		         
*作者：hk             							 
*描述：司机室总线初始化处理
*输入：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void cab_bus_init(void)
{
	
	CAB_BUS_PORTINIT(CAB_BUS_BAUDRATE);
}

/****************************************************
*函数名：cab_bus_set_debug			       		         
*作者：hk             							 
*描述：设置调试标志
*输入：无	
*返回值：无	
*修改记录：无								
****************************************************/
static void cab_bus_set_debug(uint8 flag)
{
	
	cab_bus_debug = flag;
}


//输出虚拟设备
cab_bus_dev_t xian2_cab_bus_dev=
{
	cab_bus_init,
	cab_bus_proc,
	cab_bus_send_data,
	cab_bus_set_debug,
};

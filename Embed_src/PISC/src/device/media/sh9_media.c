//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 多媒体播放器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:media.c

//*文件作用:多媒体播放器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"




//通信协议关键字
#define MEDIA_PACKET_HEAD		0x7E	//命令头
#define MEDIA_PACKET_END		0x0d	//命令尾
#define MEDIA_PACKET_DEFAULT		0x21	//过程数据

#define MEDIA_ACK_DEFAULT		0x21	//过程数据应答



//接收缓冲区
static uint8 media_recv_buf[MEDIA_BUF_SIZE];//接收缓冲区
static uint8 media_recv_length=0;//接收总长度
//static uint8 media_data_length=0;//数据部分的长度
//发送缓冲区
static uint8 media_send_buf[MEDIA_BUF_SIZE];//发送缓冲区
static uint8 media_send_length=0;//发送总长度


//接受数据结构
__packed typedef struct
{
       uint8 data_length;
	uint8 cmd;
	uint8 DeviceState[12];	
}media_recv_t;
static media_recv_t *media_recv=(media_recv_t *)&media_recv_buf;
media_send_packet_t media_send_packet;
static void media_send_data();

//***********************************************************************************************************************
//函数作用:多媒体播放器接收数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 media_get_packet(void)
{
	static uint8  media_start_flag=0;

	static uint8 recv_sum = 0;
	uint8 temp;
	while(MEDIA_RECEIVE_CHAR(&temp))
	{//print_int("%x ", temp);
	   // usb_write_char(temp);
		if(media_start_flag==0)
		{
			if(temp==MEDIA_PACKET_HEAD)
			{
				media_start_flag++;
				media_recv_length=0;
				recv_sum=0;
				MEDIA_FIFO_BACKUP();
			}
		}
		else if(media_start_flag==1)             //接收数据长度
		{
			media_start_flag++;
			media_recv_buf[media_recv_length++]=temp;  
			recv_sum+=temp;    
		}
		else if(media_start_flag>1)
		{
			if(media_recv_length<(media_recv->data_length+1))
			{
				media_recv_buf[media_recv_length++]=temp;        //接收数据
				recv_sum+=temp;                           //计算校验和
			}
			else
			{
				media_start_flag=0;
				if((uint8)(0x55-recv_sum)==temp)        //判断校验和是否正确
				{
					return 1;
				}
				else
				{
					MEDIA_FIFO_RESUME();
					print_int("%x recv_sum ", 0x55-recv_sum);
					print_int("%x sum ", temp);
				}
			}
		}
	}
	
	return 0;
}

//***********************************************************************************************************************
//函数作用:多媒体播放器数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void media_proc(void)
{
      static uint32 media_send_timer = 0;
	static uint8 send_cnt = 0;
      if(checktimer(&media_send_timer,500))
	{
		send_cnt++;
		//发送应答
		//media_send_data();
		
	}
	//接收数据
	if(media_get_packet())
	{
	      //发送应答
		media_send_data();
		//清除通信错误
		sh9_media_dev.comm_error=0;
		send_cnt = 0;
		//print_line("recv line");
		//通信灯
		//led_toggle(MEDIA_LED_NO);
		led_blink(MEDIA_LED_NO,300,300);
				
		//分析数据
		if(media_recv->cmd == MEDIA_ACK_DEFAULT)
		{
	
			//长度判断
			if(media_recv_length == sizeof(media_recv_t)-3)
			{
				//拷贝数据
				memcpy((uint8 *)&sh9_media_dev.device_state,(uint8 *)&media_recv->DeviceState,sizeof(sh9_media_dev.device_state));
				//print_int("%x  d_s",sh9_media_dev.device_state.cab1.cctv_encode_err);
			}
		}

	}
	
	//增加通信错误次数
	if( send_cnt >= 50)
	{
		sh9_media_dev.comm_error = 1;
	}
}

//***********************************************************************************************************************
//函数作用:多媒体播放器发送数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void media_send_data()
{
	uint8 checksum=0;
	uint8 cnt;
	
	media_send_packet_t *p = &media_send_packet;
	uint8 *ptr = (uint8 *)&media_send_packet;
	
	media_send_length=0;
	//添加起始符
	media_send_buf[media_send_length++]= MEDIA_PACKET_HEAD;
	//送长度
	media_send_buf[media_send_length++]= sizeof(media_send_packet_t) +1 ;// 数据的长度
	checksum += media_send_buf[media_send_length-1];
	//命令字
	media_send_buf[media_send_length++]=  MEDIA_PACKET_DEFAULT;  
      checksum += media_send_buf[media_send_length-1];	

	 p->status.close_delay = (!(B_S_CLOSE_DELAY_REG&(1<<B_S_CLOSE_DELAY_BIT)));
       p->cab.key = (((CAB_KEY_REG&(1<<CAB_KEY_BIT))==0)?1:0 );
//my_default_data.status_hbit.active;
	p->cab.master_slave = my_default_data.status_hbit.master;
       p->status.up_dir = my_default_data.status_hbit.dir_up;
	p->status.down_dir = my_default_data.status_hbit.dir_down;
	p->status.arrived_broadcast = my_default_data.trigger_hbit.arrived_broadcast;
	p->status.leave_broadcast = my_default_data.trigger_hbit.leave_broadcast;
	p->status.door_sides = tms_dev->door_sides;

	if(dva_dev->comm_error>=COMM_DVA_TIMEOUT)
	    p->pcu1.dva = 1;
	else
	    p->pcu1.dva = 0;
	
	

      if(read_pisc_id() == 1)
	{
	    if(apu_error >= 20)
	    {
	        p->pcu1.apu = 1;
	     }
	    else
	     {
	        p->pcu1.apu = 0;
	     }
		p->pcu2.apu = other_default_data.cab_status.apu_error;
		p->pcu2.dva = other_default_data.cab_status.dva_error;
		p->pcu2.atc = other_default_data.tms_status.atc_valid;
		p->pcu2.mvb = other_default_data.tms_status.tms_valid;
		
      	}
	else 
	{
	    if(apu_error >= 20)
	    {
	        p->pcu2.apu = 1;
	     }
	    else
	     {
	        p->pcu2.apu = 0;
	     }
		p->pcu1.apu = other_default_data.cab_status.apu_error;
		p->pcu1.dva = other_default_data.cab_status.dva_error;
		p->pcu1.atc = other_default_data.tms_status.atc_valid;
		p->pcu1.mvb = other_default_data.tms_status.tms_valid;
      	}
	p->start_station_id = my_default_data.start_station;
	p->destination_station_id = my_default_data.end_station;
	p->next_station_id = my_default_data.next_station;
	p->current_station_id = my_default_data.current_station;
	p->city_id = my_default_data.city_id;
	//p->line_id = my_default_data.line_id;
	p->emergency_id = my_default_data.urgent_broadcast_no;
	//p->velocity = tms_recv_packet->atc_velocity;
	
	p->year = my_default_data.year;
	p->month = my_default_data.month;
	p->date = my_default_data.date;
	p->hour = my_default_data.hour;
	p->minute = my_default_data.minute;
	//p->second  = tms_recv_packet->second;

	//p->tc1_doors_unlock = tms_recv_packet->tc1_doors_unlock;
	//p->mp1_doors_unlock = tms_recv_packet->mp1_doors_unlock;
	//p->m_doors_unlock = tms_recv_packet->m_doors_unlock;
	//p->mp2_doors_unlock = tms_recv_packet->mp2_doors_unlock;
	//p->tc2_doors_unlock = tms_recv_packet->tc2_doors_unlock;
	
       //p->pcu1.= sh9_media_dev.device_state.cab1.camera1;

	 p->scu1_3.drmd1 = lcu_default_data[0].dmp_error.dmp1;
	 p->scu1_3.drmd2 = lcu_default_data[0].dmp_error.dmp2;
	 p->scu1_3.drmd3 = lcu_default_data[0].dmp_error.dmp3;
	 p->scu1_3.drmd4 = lcu_default_data[0].dmp_error.dmp4;
	 p->scu1_3.drmd5 = lcu_default_data[0].dmp_error.dmp5;
	 p->scu1_3.drmd6 = lcu_default_data[0].dmp_error.dmp6;
	 p->scu1_3.drmd7 = lcu_default_data[0].dmp_error.dmp7;
	 p->scu1_3.drmd8 = lcu_default_data[0].dmp_error.dmp8;

	 p->scu2_3.drmd1 = lcu_default_data[1].dmp_error.dmp1;
	 p->scu2_3.drmd2 = lcu_default_data[1].dmp_error.dmp2;
	 p->scu2_3.drmd3 = lcu_default_data[1].dmp_error.dmp3;
	 p->scu2_3.drmd4 = lcu_default_data[1].dmp_error.dmp4;
	 p->scu2_3.drmd5 = lcu_default_data[1].dmp_error.dmp5;
	 p->scu2_3.drmd6 = lcu_default_data[1].dmp_error.dmp6;
	 p->scu2_3.drmd7 = lcu_default_data[1].dmp_error.dmp7;
	 p->scu2_3.drmd8 = lcu_default_data[1].dmp_error.dmp8;

	 p->scu3_3.drmd1 = lcu_default_data[2].dmp_error.dmp1;
	 p->scu3_3.drmd2 = lcu_default_data[2].dmp_error.dmp2;
	 p->scu3_3.drmd3 = lcu_default_data[2].dmp_error.dmp3;
	 p->scu3_3.drmd4 = lcu_default_data[2].dmp_error.dmp4;
	 p->scu3_3.drmd5 = lcu_default_data[2].dmp_error.dmp5;
	 p->scu3_3.drmd6 = lcu_default_data[2].dmp_error.dmp6;
	 p->scu3_3.drmd7 = lcu_default_data[2].dmp_error.dmp7;
	 p->scu3_3.drmd8 = lcu_default_data[2].dmp_error.dmp8;

	 p->scu4_3.drmd1 = lcu_default_data[3].dmp_error.dmp1;
	 p->scu4_3.drmd2 = lcu_default_data[3].dmp_error.dmp2;
	 p->scu4_3.drmd3 = lcu_default_data[3].dmp_error.dmp3;
	 p->scu4_3.drmd4 = lcu_default_data[3].dmp_error.dmp4;
	 p->scu4_3.drmd5 = lcu_default_data[3].dmp_error.dmp5;
	 p->scu4_3.drmd6 = lcu_default_data[3].dmp_error.dmp6;
	 p->scu4_3.drmd7 = lcu_default_data[3].dmp_error.dmp7;
	 p->scu4_3.drmd8 = lcu_default_data[3].dmp_error.dmp8;

	 p->scu5_3.drmd1 = lcu_default_data[4].dmp_error.dmp1;
	 p->scu5_3.drmd2 = lcu_default_data[4].dmp_error.dmp2;
	 p->scu5_3.drmd3 = lcu_default_data[4].dmp_error.dmp3;
	 p->scu5_3.drmd4 = lcu_default_data[4].dmp_error.dmp4;
	 p->scu5_3.drmd5 = lcu_default_data[4].dmp_error.dmp5;
	 p->scu5_3.drmd6 = lcu_default_data[4].dmp_error.dmp6;
	 p->scu5_3.drmd7 = lcu_default_data[4].dmp_error.dmp7;
	 p->scu5_3.drmd8 = lcu_default_data[4].dmp_error.dmp8;
//内部屏
	 p->scu1_2.idu1= lcu_default_data[0].led_error.interior_led1;
	 p->scu1_2.idu2 = lcu_default_data[0].led_error.interior_led2;
	 p->scu2_2.idu1 = lcu_default_data[1].led_error.interior_led1;
	 p->scu2_2.idu2 = lcu_default_data[1].led_error.interior_led2;
	 p->scu3_2.idu1 = lcu_default_data[2].led_error.interior_led1;
	 p->scu3_2.idu2 = lcu_default_data[2].led_error.interior_led2;
	 p->scu4_2.idu1 = lcu_default_data[3].led_error.interior_led1;
	 p->scu4_2.idu2 = lcu_default_data[3].led_error.interior_led2;
	 p->scu5_2.idu1 = lcu_default_data[4].led_error.interior_led1;
	 p->scu5_2.idu2 = lcu_default_data[4].led_error.interior_led2;
//外部屏
	 p->scu1_2.sdu1 = lcu_default_data[0].led_error.side_led1;
	 p->scu1_2.sdu2 = lcu_default_data[0].led_error.side_led2;
	 p->scu2_2.sdu1 = lcu_default_data[1].led_error.side_led1;
	 p->scu2_2.sdu2 = lcu_default_data[1].led_error.side_led2;
	 p->scu3_2.sdu1 = lcu_default_data[2].led_error.side_led1;
	 p->scu3_2.sdu2 = lcu_default_data[2].led_error.side_led2;
	 p->scu4_2.sdu1 = lcu_default_data[3].led_error.side_led1;
	 p->scu4_2.sdu2 = lcu_default_data[3].led_error.side_led2;
	 p->scu5_2.sdu1 = lcu_default_data[4].led_error.side_led1;
	 p->scu5_2.sdu2 = lcu_default_data[4].led_error.side_led2;
//报警器
	 p->scu1_1.pea1 = lcu_default_data[0].icc_error.pecu1;
	 p->scu1_1.pea2 = lcu_default_data[0].icc_error.pecu2;
	 p->scu2_1.pea1 = lcu_default_data[1].icc_error.pecu1;
	 p->scu2_1.pea2 = lcu_default_data[1].icc_error.pecu2;
	 p->scu3_1.pea1 = lcu_default_data[2].icc_error.pecu1;
	 p->scu3_1.pea2 = lcu_default_data[2].icc_error.pecu2;
	 p->scu4_1.pea1 = lcu_default_data[3].icc_error.pecu1;
	 p->scu4_1.pea2 = lcu_default_data[3].icc_error.pecu2;
	 p->scu5_1.pea1 = lcu_default_data[4].icc_error.pecu1;
	 p->scu5_1.pea2 = lcu_default_data[4].icc_error.pecu2;

	 //功放
	 p->scu1_1.amp1 = lcu_default_data[0].amp_spk_error.amp1;
	 p->scu1_1.amp2 = lcu_default_data[0].amp_spk_error.amp2;
	 p->scu2_1.amp1 = lcu_default_data[1].amp_spk_error.amp1;
	 p->scu2_1.amp2 = lcu_default_data[1].amp_spk_error.amp2;
	 p->scu3_1.amp1 = lcu_default_data[2].amp_spk_error.amp1;
	 p->scu3_1.amp2 = lcu_default_data[2].amp_spk_error.amp2;
	 p->scu4_1.amp1 = lcu_default_data[3].amp_spk_error.amp1;
	 p->scu4_1.amp2 = lcu_default_data[3].amp_spk_error.amp2;
	 p->scu5_1.amp1 = lcu_default_data[4].amp_spk_error.amp1;
	 p->scu5_1.amp2 = lcu_default_data[4].amp_spk_error.amp2;

       //对讲控制器
	 p->scu1_1.intercom = lcu_default_data[0].icc_error.sapu_comm;
	 p->scu2_1.intercom = lcu_default_data[1].icc_error.sapu_comm;
	 p->scu3_1.intercom = lcu_default_data[2].icc_error.sapu_comm;
	 p->scu4_1.intercom = lcu_default_data[3].icc_error.sapu_comm;
	 p->scu5_1.intercom = lcu_default_data[4].icc_error.sapu_comm;

	 //scu

	 //火警
	// p->pcu1.fire_alarm_tc1_cab = tms_recv_packet->tcms_fire.fire_tc1_cab;
	 //p->scu1_1.fire_alarm_tc1_saloon = tms_recv_packet->tcms_fire.fire_tc1_saloon;
	// p->scu2_1.fire_alarm_mp1_saloon = tms_recv_packet->tcms_fire.fire_mp1_saloon;
    //   p->scu3_1.fire_alarm_m_saloon = tms_recv_packet->tcms_fire.fire_m_saloon;
    //   p->scu4_1.fire_alarm_mp2_saloon = tms_recv_packet->tcms_fire.fire_mp2_saloon;
    //   p->scu5_1.fire_alarm_tc2_saloon = tms_recv_packet->tcms_fire.fire_tc2_saloon;
    //   p->pcu2.fire_alarm_tc2_cab = tms_recv_packet->tcms_fire.fire_tc2_cab;

	for(cnt = 0; cnt < sizeof(media_send_packet_t); cnt++)
	{
	     checksum += *ptr;
	     media_send_buf[media_send_length++] = *ptr;
	     ptr++;
	}

      checksum=0x55-checksum;
      //校验和
	media_send_buf[media_send_length++] = checksum;
	//send_buf[send_length++] = checksum;  //校验和
      media_send_buf[media_send_length++] = MEDIA_PACKET_END;
	
	MEDIA_SEND_BUF(media_send_buf, media_send_length);
}

//***********************************************************************************************************************
//函数作用:媒体播放获取播放
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 media_get_playing(void)
{
	if(MEDIA_PLAY_IN_REG & (1<<MEDIA_PLAY_IN_BIT))
		return 0;
	else
		return 1;
}

//***********************************************************************************************************************
//函数作用:多媒体初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void media_init(void)
{	
	//初始化通信端口
	MEDIA_PORTINIT(MEDIA_BAUDRATE);
	
}



//输出虚拟设备类
media_dev_t sh9_media_dev=
{
	media_init,
	media_proc,
	media_send_data,
	media_get_playing,
	
	0,//串口通信错误
	
	
	{{0},{0},{0},{0},{0},{0},{0},{0},},
};

/******************************************************************************
*文件名称 ：tba_bus.c		       		         
*作          者 ：thomson   							 
*描          述 ：列车总线处理		
*创建日期：2010-3-10
*修改记录：无								
******************************************************************************/


#include "include.h"

#define BUS_TIME_OUT  15000      //列车485总线没有收到数据，则切换到主状态
#define BUS_TIME_OUT_MS  7000 	//做主备切换是总线停发数据7.5S
#define BUS_TIME_OUT_MS_RECON  2000 
#define BUS_DATA_ERROR_TIME_OUT  15000 

#define PISC_DEV_ID  		1
#define DCP_DEV_ID  			2
#define LCU_DEV_ID  			3
#define IO_DEV_ID  			8

#define POLL_TOTAL  			15       	//巡检次数
#define POLL_INTERVAL_MAX		30000 		//巡检时间间隔



//接收缓冲区
static uint8   tba_bus_recv_buf[TBA_BUF_SIZE];
static uint16 tba_bus_recv_length=0;
//发送缓冲区
static uint8   tba_bus_default_send_buf[TBA_BUF_SIZE];
static uint16 tba_bus_default_send_length=0;
static uint8   tba_bus_p2p_send_buf[TBA_BUF_SIZE];
static uint16 tba_bus_p2p_send_length=0;

static uint8   bus_cmd = 0;

uint32 bus_data_error_timer = 0;
//私有变量
static tba_packet_t * tba_recv_packet=(tba_packet_t *)&tba_bus_recv_buf;//接收数据包的指针
static uint8 have_token=0;//是否得到令牌
static uint8 table_token=0;//是否得到令牌
static uint8 my_net_id=0;//自己的网络号
static uint8 my_dev_id=0;//自己的设备ID
static uint8 m_dev_id=0;//M的设备ID
static uint8 is_default_send=0;//是否有过程数据要发送
static uint8 is_p2p_send=0;//是否有点对点数据要发送
static uint8 token_free_flag=1;//释放令牌的标志 缺省为释放令牌
static uint16 comm_error_start_timer=0;//通信错误开始检测定时器
static uint8 tba_bus_debug=0;//调试标志

static uint8 dcp_setvolume_times=0;
static uint8 dcp_volume_num=0;

static uint32 table_token_timer= 0;	

//extern uint8 pisc_status;
uint8 bus_data_error_cont = 0 ;
extern uint8 test_uart1_irq_flag;
extern uint16 MAX485_CTRL_REG_D ; 
//extern uint8 recv_data_flag_hd ;
//各个设备数据处理列表
__packed typedef struct
{
	uint8 dev_type;//设备类型
	void (*dev_proc)(uint8, uint8, uint8, uint8 *, uint8);//设备数据处理
}dev_proc_list_t;
static dev_proc_list_t const dev_proc_list[]=
{
	{TBA_DEV_PISC,pisc_proc},//中央控制器
	{TBA_DEV_DCP,dcp_proc},//广播控制盒, sz24 maybe no device
	{TBA_DEV_LCU,lcu_proc},//客室控制器
	//{TBA_DEV_DSTLED,dstled_proc},//终点站屏, sz24 no this device
	{TBA_DEV_SETUP,setup_proc},//PC设置软件
	
	{0,NULL},//结束符
};


static uint8 tba_send_token(token_polling_table_t* table )
{
#if 0
	if(table.poll_flag == 0
			|| table.poll_interval_time >= 3)
	{
		tba_m_send_token(table.dev_id);		
		table.timer = get_clock();
		table.poll_flag = 1;
		table.poll_interval_time = 1;
		table.poll_total++;  //巡检次数计数
		send_token_cnt ++;
		table_token = 0 ;  // 释放485令牌权，等待回复时回收权限
		table_token_timer = get_clock(); // 计算发送时间
		return 1 ;
	}
	else
		table.poll_interval_time++;
#else
	if(table->poll_flag == 0
			|| table->poll_interval_time >= 3)
	{
		tba_m_send_token(table->dev_id);		
		table->timer = get_clock();
		table->poll_flag = 1;
		table->poll_interval_time = 1;
		table->poll_total++;  //巡检次数计数
		send_token_cnt ++;
		table_token = 0 ;  // 释放485令牌权，等待回复时回收权限
		table_token_timer = get_clock(); // 计算发送时间
		return 1 ;
	}
	else
		table->poll_interval_time++;
#endif
	return 0;

}

/*************************************************
巡检处理
*************************************************/
static void tba_token_handle(void)
{
	static uint8 token_index =0;
	static uint8 token_dcp_index =0;
	static uint8 token_lcu_index =0;
	static uint8 token_io_index =0;

	
	token_index++;
	if(token_index>10)
		token_index = 1;

	if((token_index %2)==1)//巡检dcp
	{
		if(token_dcp_index>=dcp_token_num)
			token_dcp_index = 0;
		
		while(token_dcp_index<dcp_token_num)
		{						
			if((get_clock() - token_polling_dcp_table[token_dcp_index].timer) 
			> token_polling_dcp_table[token_dcp_index].interval) // 巡检时间间隔
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_dcp_table[token_dcp_index]))
				{
					token_dcp_index++;
					break;
				}
			}	
			token_dcp_index++;
		}			
	}
	else if(token_index == 2 ||token_index == 6) // 巡检lcu
	{
		if(token_lcu_index>=lcu_token_num)
			token_lcu_index = 0;

		while(token_lcu_index<lcu_token_num)
		{						
			if((get_clock() - token_polling_lcu_table[token_lcu_index].timer) 
			> token_polling_lcu_table[token_lcu_index].interval) // 巡检时间间隔
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_lcu_table[token_lcu_index]))
				{
					token_lcu_index++;
					break;
				}
			}	
			token_lcu_index++;
		}
	}
	
	else if(token_index == 4 ||token_index == 8) // 巡检pisc
	{						
		if(read_pisc_id()==1)  //不巡检自己
		{
			if((get_clock() - token_polling_pisc_table[1].timer) 
			> token_polling_pisc_table[1].interval) // 巡检时间间隔
				tba_send_token((token_polling_table_t*)&token_polling_pisc_table[1]);	
		}
		else
		{
			if((get_clock() - token_polling_pisc_table[0].timer) 
			> token_polling_pisc_table[0].interval) // 巡检时间间隔
				tba_send_token((token_polling_table_t*)&token_polling_pisc_table[0]);
		}	
	}
	else if(token_index == 10) // 巡检io
	{
		if(token_io_index>=io_token_num)
			token_io_index = 0;
		while(token_io_index<io_token_num)
		{						
			if((get_clock() - token_polling_io_table[token_io_index].timer) 
			> token_polling_io_table[token_io_index].interval) // 巡检时间间隔
			{				
				if(tba_send_token((token_polling_table_t*)&token_polling_io_table[token_io_index]))
				{
					token_io_index++;
					break;
				}
			}	
			token_io_index++;
		}
	}
}


/**************************************************

广播控制盒音量设置

**************************************************/

static void dcp_volume_handle (void)
{
	dcp_setvolume_times++;  //每种音量的发送次数
	//print_int("%x dcp:", dcp_volume_num);
	switch(dcp_volume_num)
	{
		case 0:
			if((saveparam.dcp_pa_listen_volume != (dcp_default_data[0].dcp_pacc_listen &0x0f))
				||(saveparam.dcp_pa_listen_volume != (dcp_default_data[1].dcp_pacc_listen &0x0f)))
			{
				ptu_print_debug("d1");
				//dcp_volume_num = 1;
				my_default_data.dcp_volume = ((1<<4)&0Xf0)|saveparam.dcp_pa_listen_volume ;
			}
			
			
			break;
			
		case 1:
			if((saveparam.dcp_cc_listen_volume != ((dcp_default_data[0].dcp_pacc_listen>>4) &0x0f))
				||(saveparam.dcp_cc_listen_volume != ((dcp_default_data[1].dcp_pacc_listen>>4) &0x0f)))
			{
				ptu_print_debug("d2");
				//dcp_volume_num = 2;
				my_default_data.dcp_volume = ((2<<4)&0Xf0)|saveparam.dcp_cc_listen_volume ;
			}
			break;
	
		case 2:
			if((saveparam.dcp_pc_listen_volume != (dcp_default_data[0].dcp_pcpa_listen &0x0f))
				||(saveparam.dcp_pc_listen_volume != (dcp_default_data[1].dcp_pcpa_listen &0x0f)))
			{
				ptu_print_debug("d3");
				//dcp_volume_num = 3;
				my_default_data.dcp_volume = ((3<<4)&0Xf0)|saveparam.dcp_pc_listen_volume ;
			}
			break;

		case 3:
			if((saveparam.dcp_pa_mic_volume != ((dcp_default_data[0].dcp_pcpa_listen>>4) &0x0f))
				||(saveparam.dcp_pa_mic_volume != ((dcp_default_data[1].dcp_pcpa_listen>>4) &0x0f)))
			{
				ptu_print_debug("d4");
				//dcp_volume_num = 4;
				my_default_data.dcp_volume = ((4<<4)&0Xf0)|saveparam.dcp_pa_mic_volume ;
			}
			break;

		case 4:
			 if((saveparam.dcp_cc_mic_volume != (dcp_default_data[0].dcp_ccpc_mic&0x0f))
				||(saveparam.dcp_cc_mic_volume != (dcp_default_data[1].dcp_ccpc_mic &0x0f)))
			{
				ptu_print_debug("d5");
				//dcp_volume_num = 5;
				my_default_data.dcp_volume = ((5<<4)&0Xf0)|saveparam.dcp_cc_mic_volume ;
			}
			break;

		case 5:
			if((saveparam.dcp_pc_mic_volume != ((dcp_default_data[0].dcp_ccpc_mic >>4)&0x0f))
				||(saveparam.dcp_pc_mic_volume != ((dcp_default_data[1].dcp_ccpc_mic >>4)&0x0f)))
			{
				ptu_print_debug("d6");
				//dcp_volume_num = 6;
				my_default_data.dcp_volume = ((6<<4)&0Xf0)|saveparam.dcp_pc_mic_volume ;
			}
			break;

		default:
			break;
	}

	if(dcp_setvolume_times>=2)
	{
		dcp_volume_num++;
		dcp_setvolume_times=0;	
		if(dcp_volume_num>=6)
		{
			ptu_print_debug("dp");
			dcp_volume_num=0;
			sh9_tba_bus_dev.dcp_volume_set= 0;
			my_default_data.dcp_volume = 0;
		}
	}

#if 0
	if((saveparam.dcp_pa_listen_volume != (dcp_default_data[0].dcp_pacc_listen &0x0f))
		||(saveparam.dcp_pa_listen_volume != (dcp_default_data[1].dcp_pacc_listen &0x0f)))
	{
		print_line("d1");
		dcp_volume_num = 1;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pa_listen_volume ;
	}	
	else if((saveparam.dcp_cc_listen_volume != ((dcp_default_data[0].dcp_pacc_listen>>4) &0x0f))
			||(saveparam.dcp_cc_listen_volume != ((dcp_default_data[1].dcp_pacc_listen>>4) &0x0f)))
	{
		print_line("d2");
		dcp_volume_num = 2;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_cc_listen_volume ;
	}
	else if((saveparam.dcp_pc_listen_volume != (dcp_default_data[0].dcp_pcpa_listen &0x0f))
		||(saveparam.dcp_pc_listen_volume != (dcp_default_data[1].dcp_pcpa_listen &0x0f)))
	{
		print_line("d3");
		dcp_volume_num = 3;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pc_listen_volume ;
	}
	else if((saveparam.dcp_pa_mic_volume != ((dcp_default_data[0].dcp_pcpa_listen>>4) &0x0f))
		||(saveparam.dcp_pa_mic_volume != ((dcp_default_data[1].dcp_pcpa_listen>>4) &0x0f)))
	{
		print_line("d4");
		dcp_volume_num = 4;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pa_mic_volume ;
	}
	else if((saveparam.dcp_cc_mic_volume != (dcp_default_data[0].dcp_ccpc_mic&0x0f))
		||(saveparam.dcp_cc_mic_volume != (dcp_default_data[1].dcp_ccpc_mic &0x0f)))
	{
		print_line("d5");
		dcp_volume_num = 5;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_cc_mic_volume ;
	}
	else if((saveparam.dcp_pc_mic_volume != ((dcp_default_data[0].dcp_ccpc_mic >>4)&0x0f))
		||(saveparam.dcp_pc_mic_volume != ((dcp_default_data[1].dcp_ccpc_mic >>4)&0x0f)))
	{
		print_line("d6");
		dcp_volume_num = 6;
		my_default_data.dcp_volume = (dcp_volume_num<<4)|saveparam.dcp_pc_mic_volume ;
	}
	else
		dcp_volume_num = 7;	
#endif
}

/****************************************************
*函数名：	tba_bus_send_data		         
*作者：thomson             							 
*描述：列车总线应用层发送数据
*输入：dst_net_id       --- 目的网络ID
                 dst_dev_id       ---目的设备ID
                 data_type        ---数据包类型
                 token_free       ---释放令牌标志
                  *data_buf      ---应用数据目的指针
                  length    ---数据长度
                  
*输出：无	
*返回值：发送成功返回1 缓冲区满返回0
*修改记录：无								
****************************************************/
static uint8 tba_bus_send_data(uint8 dst_net_id, uint8 dst_dev_id, uint8 data_type, uint8 token_free, uint8 *data_buf, uint8 length)
{
	tba_packet_t temp_send_packet;//发送数据包结构---临时
	uint8 *xdata send_buf_p;//最终发送缓冲区的指针
	uint16 *xdata send_length_p;//最终发送长度的指针
	uint8 * byte_add_p=(uint8 *)&temp_send_packet;//用于字节累加的指针
	uint16 src_length;//源数据长度---用于转码
	uint8 checksum=0;//校验和

	//如果上一次点对点还没有得到令牌发送出去 这次又要发送点对点 返回失败		
	if(data_type==TBA_CMD_P2PPACKET && is_p2p_send)
		return 0;
	
	//送目的地址
	temp_send_packet.dst_net_id = dst_net_id;//目的网络号
	temp_send_packet.dst_dev_id = dst_dev_id;//目的设备ID
	//送源地址---根据目的网络号是否等于0 来填充源网络号
	if(temp_send_packet.dst_net_id)
		temp_send_packet.src_net_id = my_net_id;//跨网络的
	else
		temp_send_packet.src_net_id = 0x00;//本网络内的
	temp_send_packet.src_dev_id = my_dev_id;//源设备ID
	//送总线命令字
	temp_send_packet.bus_cmd.packet_type = data_type;//包类型
	temp_send_packet.bus_cmd.token_free = token_free;//是否释放令牌
	if(token_free)
		token_free_flag=1;//如果释放令牌 送清除令牌的标志
	else
		token_free_flag=0;//如果不释放令牌 清除令牌的标志=0
	temp_send_packet.bus_cmd.m_reserve=0;//M的预留标志
	//送数据长度
	temp_send_packet.app_length=length;
	//判断有没有应用层数据---只还令牌
	if( !temp_send_packet.app_length )
	{
		temp_send_packet.dst_net_id = 0x00;//只还令牌---目的网络号修正为00
		temp_send_packet.src_net_id = 0x00;//只还令牌---源网络号修正为00
		temp_send_packet.dst_dev_id = m_dev_id;//只还令牌---目的设备ID修正为M的设备ID
	}
	//判断有没有应用层数据---送应用层数据
	else
	{
		if(temp_send_packet.app_length>=255)
			temp_send_packet.app_length=255;//长度超过判断
		memcpy( (uint8 *)&temp_send_packet.app_data, data_buf, temp_send_packet.app_length );//拷贝数据		
	}
	
	//根据数据类型送不同的缓冲区	
	
	if(data_type == TBA_CMD_P2PPACKET)
	{
		send_buf_p = (uint8 *)&tba_bus_p2p_send_buf;
		send_length_p = &tba_bus_p2p_send_length;
		if(temp_send_packet.app_length)
			is_p2p_send=1;//如果有数据才能送标志
		else
			is_p2p_send=0;//如果有数据才能送标志
	}
	else if(data_type == TBA_CMD_DEFAULTPACKET)    //过程数据
	{
		send_buf_p = (uint8 *)&tba_bus_default_send_buf;
		send_length_p = &tba_bus_default_send_length;
		if(temp_send_packet.app_length)
			is_default_send=1;//如果有数据才能送标志
		else
			is_default_send=0;
	}

	//检查特殊字符并进行转码一边转一边送发送缓冲区 同时计算校验和 并添加分隔符
	src_length = sizeof(tba_packet_t) - TBA_BUF_SIZE + temp_send_packet.app_length;
	*send_length_p = 0;
	send_buf_p[(*send_length_p)++] = TBA_PACKET_TAB;//添加分隔符
	while(src_length--)
	{
		//判断是否是特殊字符
		if(*byte_add_p==TBA_PACKET_TAB)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
		}
		else if(*byte_add_p==TBA_PACKET_DLE)
		{
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
			send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
		}
		else
		{
			send_buf_p[(*send_length_p)++]=*byte_add_p;
		}
		checksum+=*byte_add_p;
		byte_add_p++;
	}
	checksum=0x55-checksum;
	//判断校验和是否是特殊字符
	if(checksum==TBA_PACKET_TAB)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_TAB;
	}
	else if(checksum==TBA_PACKET_DLE)
	{
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE;
		send_buf_p[(*send_length_p)++]=TBA_PACKET_DLE_DLE;
	}
	else
	{
		send_buf_p[(*send_length_p)++]=checksum;		
	}
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//添加分隔符
	
	return 1;
}

/****************************************************
*函数名：	tba_bus_get_packet   		         
*作者：thomson             							 
*描述：列车总线获得数据
*输入：无                 
*输出：无	
*返回值：
*修改记录：无								
****************************************************/
static uint8 tba_bus_get_packet(void)
{
	uint8 temp;
	
	while( TBA_BUS_RECEIVE_CHAR(&temp) )
	{			
	       //uart0_write_char(temp);
		//读到分隔符 判断是否有数据 并执行逆转码 然后判断转码以后总长度和校验和
		if(temp == TBA_PACKET_TAB)//7e 头
		{
		       if(bus_cmd == 0)
		       {
		           TBA_BUS_FIFO_BAKUP();
			    bus_cmd = 1;
		       }
			//判断长度 判断是否有数据
			else if(bus_cmd == 1 && (tba_bus_recv_length >= sizeof(tba_packet_t)-TBA_BUF_SIZE))  //7e 尾
			{
			       bus_cmd = 0;
				return 1;
			}
			else
			{
			    bus_cmd = 0;
			    TBA_BUS_FIFO_RESUME();
			}
			tba_bus_recv_length=0;
		}
		//其他字符串直接送缓冲区
		else
		{
		       if(bus_cmd == 1)
		       {
			tba_bus_recv_buf[tba_bus_recv_length] = temp;
			if( ++tba_bus_recv_length >= TBA_BUF_SIZE )
				tba_bus_recv_length = 0;
			//debug_print_int("%x", temp);
		       	}
		}
	}
	
	return 0;
}

/****************************************************
*函数名：tba_bus_proc       		         
*作者：thomson             							 
*描述：列车总线处理
*输入：无                 
*输出：无	
*返回值：无
*修改记录：无								
****************************************************/
static void tba_bus_proc(void)
{
	uint8 index;
	static uint8 tba_send_flag = 0;
	static uint32 tba_send_delay = 0;

	//uint8 proc_cnt= 0;  //给广播控制盒故障数据计数
  // static uint8 current_index = 0;
	//static uint8 max = 12;
	uint8 cnt_tmp;
	static uint32 timer_check = 0;	
	uint32 token_reply = 0;
	uint8 is_get_recv=0;//是否得到完整包
	//uint8 dev_type_tmp = 0;
	uint8 dev_no_tmp = 0;
	
	
	/*************** 1.   接收数据包*****************/
	if(tba_bus_get_packet())
	{		
		tba_bus_dev->bus_timer = get_clock();       //485总线有数据
		test_uart1_irq_flag  = 0;
		//判断网号  本网内的包, 广播包, 其他网发给本网的包
		if( tba_recv_packet->dst_net_id == 0x00 || tba_recv_packet->dst_net_id == 0xFF
		|| tba_recv_packet->dst_net_id == my_net_id )
		{
			uint8 checksum=0;
			uint16 src_length=0;//逆转码之前的长度
			uint16 dst_length=0;//逆转码以后的长度
			

			//需要我转发给PTU---设置命令或者调试标志
			if( (tba_recv_packet->dst_dev_id & 0x0F) == TBA_DEV_SETUP 
				|| tba_bus_debug == 2)
			{
				uart0_write_char(TBA_PACKET_TAB);
				uart0_write_buf(tba_bus_recv_buf, tba_bus_recv_length);
				uart0_write_char(TBA_PACKET_TAB);
			}

			if ((tba_bus_debug==5)||(tba_bus_debug==6))
			{
				ptu_print_tbabuf(tba_bus_recv_buf,tba_bus_recv_length);
			}
				
			//执行逆转码 并计算校验和
			for(src_length=0; src_length<tba_bus_recv_length; src_length++)
			{
				//拷贝数据
				tba_bus_recv_buf[dst_length]=tba_bus_recv_buf[src_length];
				//判断刚刚拷贝的是否是转码符
				if(tba_bus_recv_buf[src_length]==TBA_PACKET_DLE && src_length<tba_bus_recv_length-1)
				{
					//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_TAB 并跳过下一个字符
					if(tba_bus_recv_buf[src_length+1]==TBA_PACKET_DLE_TAB)
					{
						tba_bus_recv_buf[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TBA_PACKET_TAB
						src_length++;//并跳过下一个字符
					}
					//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_DLE 并跳过下一个字符
					else if(tba_bus_recv_buf[src_length+1]==TBA_PACKET_DLE_DLE)
					{
						tba_bus_recv_buf[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TBA_PACKET_DLE
						src_length++;//并跳过下一个字符
					}
					else
					{
						ptu_print_debug("tba 7f error");
						//数据错误----7f没有被正确转码
						dst_length=0;//清除转码以后的长度
						break;
					}
				}
				checksum+=tba_bus_recv_buf[dst_length];
				dst_length++;
			}
			tba_bus_recv_length=0;//转码完毕清除接收长度
			//判断校验和
			if(checksum == 0x55)
			{
				//判断转码以后总长度 注意最后的+1 代表检验和字节
				if((tba_recv_packet->src_dev_id & 0x0F) == 0x08 ) // 说明 IO 模块 其他项目没有
				{
					if( (sizeof(tba_packet_t) - TBA_BUF_SIZE + tba_recv_packet->app_length + 2) == dst_length )
					{
						is_get_recv=1;
					}
					else
						print_line("IO error");
				}
				else
				{
					if( (sizeof(tba_packet_t) - TBA_BUF_SIZE + tba_recv_packet->app_length + 1) == dst_length )
					{	
						//送收到完整包标志
						is_get_recv=1;
					}
					else
					{
						ptu_print_debug("tba length error");
					}
				}
				//uart0_write_char(0xbb);
				//uart0_write_char(0xbb);
				//uart0_write_char(0x7e);
				//uart0_write_buf(tba_bus_recv_buf, dst_length);
				//uart0_write_char(0x7e);
			}
			else
			{
			       TBA_BUS_FIFO_RESUME();
				if(!checktimer(&bus_data_error_timer, BUS_DATA_ERROR_TIME_OUT))
				{
				   	
					bus_data_error_cont++;
					//print_line("bus_data_error_cont++");
				}
				else
				{
				   	bus_data_error_timer = get_clock();
					bus_data_error_cont = 0;
					//print_line("bus_data_error_cont=0");
				}
				if(bus_data_error_cont>=10)
				{
					bus_data_error_cont = 0;
					//pisc_status =  0;	//IDLE
				}
				ptu_print_debug("bus checksum error");
				//print_int("%x",tba_recv_packet->dst_dev_id);
				//print_int("%x",tba_recv_packet->src_dev_id);
				//uart0_write_char(0xaa);
				//	uart0_write_char(0xaa);
				//uart0_write_char(0x7e);
				//uart0_write_buf(tba_bus_recv_buf, dst_length);
				//uart0_write_char(0x7e);
			}			
		}
	}
	
	
	
	/*************** 2.   处理数据包*****************/	
	if(is_get_recv)//判断要我处理的包
	{
		//判断需要我处理的包  完全广播包(0xFF) 设备广播包my_dev_id|0xF0=0xF1 专门发给我的包 
		if( (tba_recv_packet->dst_dev_id == 0xFF) || (tba_recv_packet->dst_dev_id == (my_dev_id|0xF0) ) 
			|| (tba_recv_packet->dst_dev_id == my_dev_id) )
		{
	       have_token = 1;//送令牌标志		
	       table_token = 1; //收到回复，回收总线权限
			timer_check = get_clock();  // 收到回复的时间
			led_toggle(TBA_BUS_LED_NO);//通信灯			
			switch( tba_recv_packet->bus_cmd.packet_type )//判断包类型
			{			      						
				case TBA_CMD_TOKENPACKET://令牌包					
					if(tba_recv_packet->dst_dev_id == my_dev_id)//判断是否给我的令牌
					{
						send_token_cnt = 0;
						//print_line("by polling");
						token_reply = get_clock();
						while(get_clock() - token_reply <=3);
						TBA_BUS_SEND_BUF(tba_bus_default_send_buf, tba_bus_default_send_length);
					}
					break;
										
				case TBA_CMD_TOKENCLEAR://清除所有令牌的包
					have_token = 0;//清除令牌标志
						
					break;

				default://其他包
					//过滤自己发送的包 并判断长度
					//if(tba_recv_packet->src_dev_id!=my_dev_id && tba_recv_packet->app_length)
					{
						  
						//按总线设备分类处理
						dev_proc_list_t const *list;
						for(list=dev_proc_list; list->dev_proc; list++)
						{
							//找到设备 调用数据处理函数
							if( (tba_recv_packet->src_dev_id & 0x0F) == list->dev_type )
							{		
								//调用数据处理函数
								list->dev_proc( tba_recv_packet->src_net_id,tba_recv_packet->src_dev_id,
												(uint8)tba_recv_packet->bus_cmd.packet_type,(uint8 *)&tba_recv_packet->app_data,
												tba_recv_packet->app_length );
								
								break;
							}
						}

					}
					break;		
			}
		}
	}
	
		
	
	/*************** 3.   处理发送*****************/	
	if(have_token && is_p2p_send)//如果有点对点命令要发送 优先发送
	{
		//对于点对点命令 如果发送成功清标志 不成功说明缓冲区满等下次再重新发送
		if( TBA_BUS_SEND_BUF(tba_bus_p2p_send_buf,tba_bus_p2p_send_length) )
		{			
			is_p2p_send=0;//发送完毕清标志
			
			if(token_free_flag)//如果有清除令牌的标志 清除令牌
				have_token=0;
		}
	}
	else if((send_token_cnt >= 2) && (get_clock()-timer_check>BUS_INTERVAL_TIME)&&table_token)//发送过程数据---只还令牌的包也包括在内
	{					
		if(sh9_tba_bus_dev.dcp_volume_set)//add by suven for dcp volume set
		{
			dcp_volume_handle();
			//sh9_tba_bus_dev.dcp_volume_set=0;
		}		
		send_token_cnt =0;				

		sh9_tba_bus_dev.tba_send = 1;  //有数据发送
		TBA_BUS_SEND_BUF(tba_bus_default_send_buf, tba_bus_default_send_length);//发送过程数据 忽略发送结果
		
	
		if(tba_bus_debug==3)//输出过程数据到uart0，方便调试
		{
		    uart0_write_buf(tba_bus_default_send_buf, tba_bus_default_send_length);
		}

		if((tba_bus_debug==5)||(tba_bus_debug==7))
		{
			ptu_print_buf(tba_bus_default_send_buf, tba_bus_default_send_length);
			DEBUG_SEND_STR("\r\n");
		}
							
		have_token=0; //过程数据强制清除令牌
	}

	
	

	/*************** 4.   处理巡检*****************/	
	if( my_default_data.status_hbit.master == 1)
	{
		if((get_clock()-timer_check>BUS_INTERVAL_TIME) && (send_token_cnt<2)&& table_token )
			//&&(tba_send_flag == 0 && (get_clock()-tba_send_delay>PROCESS_TO_TOKEN_TIME)))
		{
			tba_token_handle();
			have_token=0; //清除令牌
		}
	}

	#if 0
	if(tba_send_flag && (sh9_tba_bus_dev.tba_send==0))
	{
		tba_send_delay = get_clock();		
	}
	tba_send_flag = sh9_tba_bus_dev.tba_send ; 
	#endif 

	if(get_clock() - table_token_timer > TOKEN_INTERVAL_TIME) //收回令牌
		table_token = 1; 
	
	switch(tba_recv_packet->src_dev_id & 0x0f)//收到回复清除故障标志
	{
		case PISC_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < pisc_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_pisc_table[cnt_tmp].dev_id)
				{
					token_polling_pisc_table[cnt_tmp].poll_flag = 0;
					token_polling_pisc_table[cnt_tmp].poll_total= 0;  //清除巡检次数
					token_polling_pisc_table[cnt_tmp].interval= 0;
				 	error_pack.status1 &= ~(1<<(cnt_tmp+6));	//清除故障位
				 	break;
				}
			}				
			break;
			
		case DCP_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < dcp_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_dcp_table[cnt_tmp].dev_id)
				{
					token_polling_dcp_table[cnt_tmp].poll_flag = 0;
					token_polling_dcp_table[cnt_tmp].poll_total= 0;  //清除巡检次数

					//非钥匙侧dcp扫描间隔大点
					if(my_default_data.status_hbit.active
					   &&(read_pisc_id()==((tba_recv_packet->src_dev_id & 0xf0)>>4)))
						token_polling_dcp_table[cnt_tmp].interval = 0;
					else
						token_polling_dcp_table[cnt_tmp].interval = 200;
					
					error_pack.status2 &= ~(1<<(cnt_tmp+4));//清除故障位		
					break;
				}
			}
			break;
			
		case LCU_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < lcu_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_lcu_table[cnt_tmp].dev_id)
				{
					token_polling_lcu_table[cnt_tmp].poll_flag = 0;
					token_polling_lcu_table[cnt_tmp].poll_total= 0;  //清除巡检次数
					token_polling_lcu_table[cnt_tmp].interval = 0;
					error_pack.status1 &= ~(1<<(cnt_tmp));	//清除故障位		
					break;
				}
			}
			break;
			
		case IO_DEV_ID:
			for(cnt_tmp = 0; cnt_tmp < io_token_num; cnt_tmp++)
			{
				if(tba_recv_packet->src_dev_id == token_polling_io_table[cnt_tmp].dev_id)
				{
					token_polling_io_table[cnt_tmp].poll_flag = 0;
					token_polling_io_table[cnt_tmp].poll_total= 0;  //清除巡检次数
					token_polling_io_table[cnt_tmp].interval = 0;
					error_pack.status3 &= ~(1<<(cnt_tmp+2));//置故障位
					break;
				}
			}
			break;
			
		default:
			break;
	}

	//无回复的设备巡检时间间隔加长
	for(cnt_tmp = 0; cnt_tmp < pisc_token_num; cnt_tmp++) // pisc
	{
		if(token_polling_pisc_table[cnt_tmp].poll_total > POLL_TOTAL)
		{	
			error_pack.status1 |= (1<<(cnt_tmp+6)); //置故障位
			token_polling_pisc_table[cnt_tmp].poll_total = POLL_TOTAL;

			/*
			token_polling_pisc_table[cnt_tmp].interval +=1000; 
			if(token_polling_pisc_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_pisc_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}*/
		}
	}	

	for(cnt_tmp = 0; cnt_tmp < dcp_token_num; cnt_tmp++)  // dcp
	{
		if(token_polling_dcp_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status2 |= (1<<(cnt_tmp+4));//置故障位
			token_polling_dcp_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_dcp_table[cnt_tmp].interval +=1000; 
			if(token_polling_dcp_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_dcp_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	for(cnt_tmp = 0; cnt_tmp < lcu_token_num; cnt_tmp++)  // lcu
	{
		if(token_polling_lcu_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status1 |= (1<<(cnt_tmp));//置故障位
			token_polling_lcu_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_lcu_table[cnt_tmp].interval +=1000; 
			if(token_polling_lcu_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_lcu_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	for(cnt_tmp = 0; cnt_tmp < io_token_num; cnt_tmp++)  // io
	{
		if(token_polling_io_table[cnt_tmp].poll_total > POLL_TOTAL)
		{
			error_pack.status3 |= (1<<(cnt_tmp+2));//置故障位
			token_polling_io_table[cnt_tmp].poll_total = POLL_TOTAL;
			token_polling_io_table[cnt_tmp].interval +=1000; 
			if(token_polling_io_table[cnt_tmp].interval>POLL_INTERVAL_MAX)
			{
				token_polling_io_table[cnt_tmp].interval = POLL_INTERVAL_MAX;
			}
		}
	}

	
	
#if 0
	if(is_get_recv && (my_default_data.status_hbit.master == 1))
	{
		//tba_m_proc(tba_recv_packet);
		for(cnt_tmp = 0; cnt_tmp < POLL_NUM_MAX; cnt_tmp++)
		{
		    if(tba_recv_packet->src_dev_id == token_polling_table[cnt_tmp].dev_id)
		    {
		        token_polling_table[cnt_tmp].poll_interval_time = 0;
				 token_polling_table[cnt_tmp].poll_flag = 0;

				 //add by hk 2010-9-19
				 token_polling_table[cnt_tmp].poll_error = 0;

				 //巡检超时
				if((get_clock() - token_polling_table[cnt_tmp].timer) >= token_polling_table[cnt_tmp].time_out)
				{
					token_polling_table[cnt_tmp].poll_total++;
					//连续超时3次，则增加超时时间
					if((token_polling_table[cnt_tmp].dev_id&0x0f)!=1
					&&token_polling_table[cnt_tmp].poll_total >= 3)
					{
						token_polling_table[cnt_tmp].time_out = token_polling_table[cnt_tmp].time_out + 100;
						token_polling_table[cnt_tmp].poll_total = 0;
						token_polling_table[cnt_tmp].interval = (get_clock() - token_polling_table[cnt_tmp].timer) + 100;
						print_int("%x time out",token_polling_table[cnt_tmp].dev_id);
					}
					if(token_polling_table[cnt_tmp].time_out > 30000)
					{
						token_polling_table[cnt_tmp].time_out = BUS_INTERVAL_TIME;
					}
				}
				else   //巡检没有超时
				{
				    token_polling_table[cnt_tmp].poll_total = 0;	                           
				    if((get_clock() - token_polling_table[cnt_tmp].timer)<2*BUS_INTERVAL_TIME)  //调整巡检间隔和超时时间
				    {
			    	    token_polling_table[cnt_tmp].interval = 2*BUS_INTERVAL_TIME;
				    }
				    else
			    	{
			    	    token_polling_table[cnt_tmp].interval = (get_clock() - token_polling_table[cnt_tmp].timer) + 100;
			    	}
				}

			    //begin  add by hk 2010-5-21   for 完善超时处理
			    dev_type_tmp = token_polling_table[cnt_tmp].dev_id&0x0f;
			    dev_no_tmp = token_polling_table[cnt_tmp].dev_id >> 4;
			    if(dev_type_tmp == 1)//pisc
			    {
			        error_pack.status1 &= ~(1<<(dev_no_tmp+5));
			    }
			    else if(dev_type_tmp == 2) //dcp
		    	{
		    	    error_pack.status2 &= ~(1<<(dev_no_tmp+3));
		    	}
			    else if(dev_type_tmp == 3)//lcu
		    	{
		    	    error_pack.status1 &= ~(1<<(dev_no_tmp-1));				    
		    	}				    
			    //end  add by hk 2010-5-21   for 完善超时处理
		    }			    
		}
	}
	//没有收到完整包 送NULL指针
	else
	{
		//tba_m_proc(NULL);
	}

	if( my_default_data.status_hbit.master == 1)
	{
	    cnt =0;
		if(checktimer(&timer_check,BUS_INTERVAL_TIME) && (send_token_cnt<2))
		{
		    while(current_index < POLL_NUM_MAX)
			{
				cnt++;
				if((get_clock() - token_polling_table[current_index].timer) > token_polling_table[current_index].interval)
				{
		             //没有发送巡检令牌或发送间隔已超过3次，则重发令牌
					if(token_polling_table[current_index].poll_flag == 0 || token_polling_table[current_index].poll_interval_time >= 3)
					{						              	              
						//不巡检自己
						if(token_polling_table[current_index].dev_id == my_dev_id)
						{
						    current_index++;
							current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
							continue;
						}
		              //begin  add by hk 2010-5-21   for 完善超时处理
						if(token_polling_table[current_index].poll_interval_time >= 3)
						{
							token_polling_table[current_index].poll_error++;
							if(token_polling_table[current_index].interval < 50000)  //防止溢出
							token_polling_table[current_index].interval += BUS_INTERVAL_TIME;
							if(token_polling_table[current_index].time_out < 60000)  //防止溢出
							token_polling_table[current_index].time_out += BUS_INTERVAL_TIME; 
							if(token_polling_table[current_index].poll_error >= 200)
							{
							 token_polling_table[current_index].poll_error = 10;
							}
							//  print_int("%x time out dev_id ",token_polling_table[current_index].dev_id);
							//    print_int("%x poll_error  ",token_polling_table[current_index].poll_error);
						}  
			          //end  add by hk 2010-5-21   for 完善超时处理

						tba_m_send_token(token_polling_table[current_index].dev_id);
						// debug_print_int("%x no",token_polling_table[current_index].dev_id);
						token_polling_table[current_index].timer = get_clock();
						token_polling_table[current_index].poll_flag = 1;
						token_polling_table[current_index].poll_interval_time = 1;

						//debug_print_int("%d c_index", current_index);
						send_token_cnt++;
						current_index++;
						current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
					}
					else    //已发送巡检指令，但没有收到回复
					{
						token_polling_table[current_index].poll_interval_time++;
						//token_polling_table[current_index].timer = get_clock();
						continue;
					}
					break;
				}
				//debug_print_int("%d current_index22", current_index);
			   current_index++;
				current_index =( (current_index) + POLL_NUM_MAX)%POLL_NUM_MAX;
				if(cnt == POLL_NUM_MAX)
					break;
			}
		}
	}
#endif

    //485总线数据超时检测
	if(checktimer(&tba_bus_dev->bus_timer, BUS_TIME_OUT))
	{
	    //other_default_data.test_status.slave_req = 1;    //485总线上在BUS_TIME_OUT时间里没有数据，本设备切换到主
       //my_default_data.test_status.master_req = 1;
	    tba_bus_dev->data_flag = 1;
	    print_line("485 hava no data");
	}

	//增加通信错误计数器
	if(++comm_error_start_timer>=50000)
	{
		comm_error_start_timer=50000;
		
		for(index=0;index<TBA_PISC_NUM;index++)
		{
			if(++pisc_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				pisc_comm_error[index]=COMM_DATA_TIMEOUT;
				//memset((uint8 *)&other_default_data,0,sizeof(pisc_default_data_t));
			}
		}
		for(index=0;index<TBA_DCP_NUM;index++)
		{
			if(++dcp_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				dcp_comm_error[index]=COMM_DATA_TIMEOUT;
				memset((uint8 *)&dcp_default_data[index],0,sizeof(dcp_default_data_t));
			
			}
		}
		for(index=0;index<TBA_LCU_NUM;index++)
		{
			if(++lcu_comm_error[index]>=COMM_DATA_TIMEOUT)
			{
				lcu_comm_error[index]=COMM_DATA_TIMEOUT;
				//memset((uint8 *)&lcu_default_data[index],0,sizeof(lcu_default_data_t));
				//print_line("memset lcu data");print_int("%d index ", index);
			}
		}
	}
}

/****************************************************
*函数名：tba_bus_init       		         
*作者：thomson             							 
*描述：总线初始化函数
*输入：无                 
*输出：net_id---自己的网络号  dev_id---自己的设备ID
*返回值：无
*修改记录：无								
****************************************************/
static void tba_bus_init(uint8 net_id, uint8 dev_id)
{
	//记忆自己的网络号
	my_net_id=net_id;
	//记忆自己的设备ID
	my_dev_id=dev_id;
	
	//总线管理器初始化
	tba_m_init(my_net_id,my_dev_id);
	print_int("%x dev_id", my_dev_id);
	
	//初始化通信端口
	TBA_BUS_PORTINIT(TBA_BUS_BAUDRATE);
}

/****************************************************
*函数名：tba_bus_set_debug       		         
*作者：thomson             							 
*描述：设置调试标志
*输入：无                 
*输出：flag---调试标志
*返回值：无
*修改记录：无								
****************************************************/
void tba_bus_set_debug(uint8 flag)
{
	tba_bus_debug=flag;
}



//输出虚拟设备类
tba_bus_dev_t sh9_tba_bus_dev=
{
	tba_bus_init,
	tba_bus_proc,
	tba_bus_send_data,
	tba_bus_set_debug,
	0,
	0,
	0,
	0,
};



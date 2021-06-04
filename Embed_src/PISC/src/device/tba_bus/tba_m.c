//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tba_m.c

//*文件作用:列车通信总线管理器

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"

token_polling_table_t token_polling_pisc_table[pisc_token_num]=
{
	{0x11,0,0,0,0,0,100,0,0},//PISC
	{0x21,0,0,0,0,0,100,0,0},//PISC
};

token_polling_table_t token_polling_dcp_table[dcp_token_num]=
{
	{0x12,0,0,0,0,0,100,0,0},//DCP
	{0x22,0,0,0,0,0,100,0,0},//DCP
};

token_polling_table_t token_polling_lcu_table[lcu_token_num]=
{
	{0x13,0,0,0,0,0,100,0,0},//LCU
	{0x23,0,0,0,0,0,100,0,0},//LCU
	{0x33,0,0,0,0,0,100,0,0},//LCU
	{0x43,0,0,0,0,0,100,0,0},//LCU
	{0x53,0,0,0,0,0,100,0,0},//LCU
	{0x63,0,0,0,0,0,100,0,0},//LCU
};

token_polling_table_t token_polling_io_table[io_token_num]=
{
	{0x18,0,0,0,0,0,100,0,0},//IO
	{0x28,0,0,0,0,0,100,0,0},//IO
	{0x38,0,0,0,0,0,100,0,0},//IO
	{0x48,0,0,0,0,0,100,0,0},//IO
	{0x58,0,0,0,0,0,100,0,0},//IO
	{0x68,0,0,0,0,0,100,0,0},//IO
};


//token_polling_table_t token_polling_table[TBA_M_TABLE_NUM]=

/*
{
	{0x11,300,0,0,0,0,100,0,0},
	{0x21,300,0,0,0,0,100,0,0},

	{0x13,300,0,0,0,0,100,0,0},//LCU
	{0x23,300,0,0,0,0,100,0,0},//LCU
	{0x33,300,0,0,0,0,100,0,0},//LCU
	{0x43,300,0,0,0,0,100,0,0},//LCU
	{0x53,300,0,0,0,0,100,0,0},//LCU
	{0x63,300,0,0,0,0,100,0,0},//LCU
	{0x12,300,0,0,0,0,100,0,0},//DCP
	{0x22,300,0,0,0,0,100,0,0},//DCP
	{0x17,600,0,0,0,0,100,0,0},//DESET LED
	{0x27,600,0,0,0,0,100,0,0},//DESET LED
	{0x00,0,  0,0,0,0,0,0,0},
};
*/

//M状态机
#define TBA_M_STATUS_DEAD		0	//死亡状态
#define TBA_M_STATUS_SLEEP		1	//休眠状态
#define TBA_M_STATUS_SENDTOKEN		2	//发送令牌状态
#define TBA_M_STATUS_WAITACK		3	//等待应答状态



//发送缓冲区
static uint8 xdata tba_m_send_buf[TBA_BUF_SIZE];
static uint16 xdata tba_m_send_length=0;



//static uint8 tba_m_status=TBA_M_STATUS_SLEEP;//M状态
//static uint8 tba_m_sleep_times=0;//休眠的次数
//static uint32 tba_no_packet_timer=0;//没有任何数据包的计时器
static uint16 tba_no_packet_interval=0;//没有任何数据包的间隔
//static uint32 tba_m_waitack_timer=0;//M等待巡检应答的定时器
//static uint8 token_index=0;//巡检表索引号---定义全局变量的目的是保证所有设备都能被巡检到
//static uint8 current_token_index=0;//当前巡检表索引

static uint8 xdata my_net_id=0;//自己的网络号
static uint8 xdata my_dev_id=0;//自己的设备ID


uint8 send_token_cnt = 0;



//***********************************************************************************************************************
//*函数作用:列车总线发送令牌
//*参数说明:
//*注意事项:
//*返回说明:
//***********************************************************************************************************************
void tba_m_send_token(uint8 dev_id)
{
	tba_packet_t xdata temp_send_packet;//发送数据包结构---临时
	uint8 *xdata send_buf_p=tba_m_send_buf;//最终发送缓冲区的指针
	uint16 *xdata send_length_p=&tba_m_send_length;//最终发送长度的指针
	uint8 * xdata byte_add_p=(uint8 *)&temp_send_packet;//用于字节累加的指针
	uint16 xdata src_length;//源数据长度---用于转码
	uint8 xdata checksum=0;//校验和
	
		
	//送目的地址
	temp_send_packet.dst_net_id=0x00;//目的网络号--本网络内的
	temp_send_packet.dst_dev_id=dev_id;//目的设备ID
	//送源地址
	temp_send_packet.src_net_id=0x00;//源网络号--本网络内的
	temp_send_packet.src_dev_id=my_dev_id;//源设备ID
	//送总线命令字
	temp_send_packet.bus_cmd.packet_type=TBA_CMD_TOKENPACKET;//包类型
	temp_send_packet.bus_cmd.token_free=0;//是否释放令牌
	temp_send_packet.bus_cmd.m_reserve=0;//M的预留标志
	//送数据长度
	temp_send_packet.app_length=0;
	
	//检查特殊字符并进行转码一边转一边送发送缓冲区 同时计算校验和 并添加分隔符
	src_length=sizeof(tba_packet_t)-TBA_BUF_SIZE+temp_send_packet.app_length;
	*send_length_p=0;
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//添加分隔符
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
	
	//发送
	sh9_tba_bus_dev.tba_send = 0;  //有数据发送
	TBA_BUS_SEND_BUF(tba_m_send_buf,tba_m_send_length);	
	if(sh9_tms_dev.debug_flag==6)
	{
		ptu_print_buf(tba_m_send_buf, tba_m_send_length);
	}
	//for test
	//uart0_write_buf(tba_m_send_buf,tba_m_send_length);	
	//print_line("poll");
}

//***********************************************************************************************************************
//*函数作用:列车总线发送清除所有令牌
//*参数说明:
//*注意事项:
//*返回说明:
//***********************************************************************************************************************
#if 0
static void tba_m_send_clear(void)
{
	tba_packet_t xdata temp_send_packet;//发送数据包结构---临时
	uint8 *xdata send_buf_p=tba_m_send_buf;//最终发送缓冲区的指针
	uint16 *xdata send_length_p=&tba_m_send_length;//最终发送长度的指针
	uint8 * xdata byte_add_p=(uint8 *)&temp_send_packet;//用于字节累加的指针
	uint16 xdata src_length;//源数据长度---用于转码
	uint8 xdata checksum=0;//校验和
	
		
	//送目的地址
	temp_send_packet.dst_net_id=0x00;//目的网络号--本网络内的
	temp_send_packet.dst_dev_id=0xFF;//目的设备ID--广播报
	//送源地址
	temp_send_packet.src_net_id=0x00;//源网络号--本网络内的
	temp_send_packet.src_dev_id=my_dev_id;//源设备ID
	//送总线命令字
	temp_send_packet.bus_cmd.packet_type=TBA_CMD_TOKENCLEAR;//包类型
	temp_send_packet.bus_cmd.token_free=0;//是否释放令牌
	temp_send_packet.bus_cmd.m_reserve=0;//M的预留标志
	//送数据长度
	temp_send_packet.app_length=0;
	
	//检查特殊字符并进行转码一边转一边送发送缓冲区 同时计算校验和 并添加分隔符
	src_length=sizeof(tba_packet_t)-TBA_BUF_SIZE+temp_send_packet.app_length;
	*send_length_p=0;
	send_buf_p[(*send_length_p)++]=TBA_PACKET_TAB;//添加分隔符
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
	
	//发送
	TBA_BUS_SEND_BUF(tba_m_send_buf,tba_m_send_length);	
}

//***********************************************************************************************************************
//*函数作用:列车总线管理器处理
//*参数说明:
//*注意事项:
//*返回说明:
//***********************************************************************************************************************

void tba_m_proc(tba_packet_t *packet)
{
	uint8 token_index_backup;//记忆上次的巡检表位置
	
	//只要收到别人的数据包 清除没有数据包的定时器 清除休眠次数
	if(packet && packet->src_dev_id!=my_dev_id)
	{
		tba_no_packet_timer=get_clock();
		tba_m_sleep_times=0;
	}


	//M不同状态的处理
	switch(tba_m_status)
	{
		//死亡状态---只能切换到休眠状态
		case TBA_M_STATUS_DEAD:
			if(!tba_m_sleep_times)
			{
				tba_m_status=TBA_M_STATUS_SLEEP;//切换状态字
				print_line("M alive");
			}
			break;
		
		//休眠状态---唤醒M到发送令牌的状态
		case TBA_M_STATUS_SLEEP:
			//很长时间没有收到任何数据包 说明其他M都死了 唤醒M到发送令牌的状态
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{
				//多次休眠 进入死亡状态
				if(++tba_m_sleep_times>=5)
				{
					tba_m_status=TBA_M_STATUS_DEAD;//切换状态字
					print_line("M dead");
				}
				else
				{
					tba_m_status=TBA_M_STATUS_SENDTOKEN;//切换状态字
					tba_m_send_clear();//发送清除所有令牌的命令
					print_line("M wakeup");					
				}
			}
			break;
			
		//发送令牌状态---切换到等待应答状态 如果很长时间没有任何数据包切换到休眠状态
		case TBA_M_STATUS_SENDTOKEN:
			//很长时间没有收到任何数据包--自己出毛病了 切换M到休眠状态
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{			
				tba_m_status=TBA_M_STATUS_SLEEP;//切换状态字
				print_line("M sleep");
				break;
			}
			//有数据 可能有人乱发数据或者别人抢我的M
			if(packet)
			{
				//判断包类型
				switch(packet->bus_cmd.packet_type)
				{
					case TBA_CMD_TOKENPACKET://令牌包				
					case TBA_CMD_TOKENCLEAR://清除所有令牌的包
						//自己发的令牌和清令牌
						if(packet->src_dev_id==my_dev_id)
						{
							print_line("M my token");
						}
						else
						{
							//切换状态字
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM is reaved:",packet->src_dev_id);
						}
						break;
			
					default://其他包
						tba_m_status=TBA_M_STATUS_SLEEP;
						print_int("%xM send but no token:",packet->src_dev_id);
						break;
				}
				break;
			}
							
 			//从上次巡检表的位置把整个巡检表查一圈 保证每个设备都有机会
 			token_index_backup=token_index;//记忆上次的巡检表位置
			do
			{
				//判断这个设备的时间到 注意根据poll_continue_timeout判断连续巡检超时的设备并降低优先级
				if( checktimer((uint32 *)&token_polling_table[token_index_backup].timer,token_polling_table[token_index_backup].interval+(uint16)token_polling_table[token_index_backup].poll_continue_timeout*TBA_M_ACK_TIMEOUT) )
				{
					tba_m_status=TBA_M_STATUS_WAITACK;//切换状态字
					current_token_index=token_index_backup;//记忆当前巡检表索引
					tba_m_waitack_timer=get_clock();//重新开始计时
					tba_m_send_token(token_polling_table[current_token_index].dev_id);//发送令牌
					token_polling_table[current_token_index].poll_total++;//在这里进行通信质量统计 增加巡检总数
					if(token_polling_table[current_token_index].poll_total==0)
						token_polling_table[current_token_index].poll_error=0;//溢出以后全部清除
					print_int("%xM give token to:",token_polling_table[current_token_index].dev_id);
					break;
				}
				//指向下一个巡检设备
				token_index_backup++;
				//巡检表转了一圈了 从头开始
				if(token_polling_table[token_index_backup].interval==0)
					token_index_backup=0;
			}while(token_index!=token_index_backup);
			//指向下一个巡检设备
			token_index++;
			//巡检表转了一圈了 从头开始
			if(token_polling_table[token_index].interval==0)
				token_index=0;
			break;
			
		//等待令牌应答状态---如果还令牌切换到发送状态 如果很长时间没有任何数据包切换到休眠状态
		case TBA_M_STATUS_WAITACK:
			//很长时间没有收到任何数据包--自己出毛病了 切换M到休眠状态
			if(checktimer(&tba_no_packet_timer,tba_no_packet_interval))
			{					
				tba_m_status=TBA_M_STATUS_SLEEP;//切换状态字
				print_line("M sleep");
				break;
			}
			//长时间没有收到令牌应答 切换到发送令牌状态继续巡检下一个设备
			if(checktimer(&tba_m_waitack_timer,TBA_M_ACK_TIMEOUT))
			{
				tba_m_status=TBA_M_STATUS_SENDTOKEN;//切换状态字
				token_polling_table[current_token_index].poll_error++;//在这里进行通信质量统计 增加成错误的数量
				if(token_polling_table[current_token_index].poll_error==0)
					token_polling_table[current_token_index].poll_total=0;//溢出以后全部清除
				if(token_polling_table[current_token_index].poll_continue_timeout<200)
					token_polling_table[current_token_index].poll_continue_timeout++;//连续巡检超时进行计数 100次为上限 根据这个次数进行降低优先级
                            //for test hk
				//tba_m_send_clear();//发送清除所有令牌的命令

				
				print_int("%xM timeout:",token_polling_table[current_token_index].dev_id);
				break;
			}
							
			//有数据 判断是否是正确令牌应答
			if(packet)
			{
				//判断包类型
				switch(packet->bus_cmd.packet_type)
				{						
					case TBA_CMD_TOKENPACKET://令牌包
					case TBA_CMD_TOKENCLEAR://清除所有令牌的包
						//自己发的令牌和清令牌
						if(packet->src_dev_id==my_dev_id)
						{
							print_line("M my token");
						}
						else
						{
							//切换状态字
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM is reaved:",packet->src_dev_id);
						}
						break;
			
					default://其他包
						//设备正确应答 源设备号等于刚刚巡检的设备地址 或者源网络号等于刚刚巡检的设备地址--用于LCU转发车厢内部的数据
						if(token_polling_table[current_token_index].dev_id==packet->src_dev_id || token_polling_table[current_token_index].dev_id==packet->src_net_id)
						{
							//切换状态字---有还令牌才能切换(用于连续发送)
							if(packet->bus_cmd.token_free)
							{
								tba_m_status=TBA_M_STATUS_SENDTOKEN;
								//print_int("%xM token free:",packet->src_dev_id);
								if(get_clock()-tba_m_waitack_timer>=TBA_M_WARNING_TIMEOUT)
								{
									print_int("%xdev_id:",token_polling_table[current_token_index].dev_id);
									print_int("%dM ack time:",get_clock()-tba_m_waitack_timer);
								}
							}
							else
							{
								print_int("%xM continue send:",packet->src_dev_id);
							}
							tba_m_waitack_timer=get_clock();//重新开始计时
							token_polling_table[current_token_index].poll_continue_timeout=0;//清除当前设备连续巡检超时计数---恢复优先级
						}
						else
						{
							tba_m_status=TBA_M_STATUS_SLEEP;
							print_int("%xM send but no token:",packet->src_dev_id);
						}
						break;
				}				
			}
			break;
		
		//其他状态
		default:
			break;
	}
	
}
#endif
//***********************************************************************************************************************
//*函数作用:设置巡检表
//*参数说明:
//*注意事项:
//*返回说明:
//***********************************************************************************************************************
void tba_m_set_table(uint8 *dev_id, uint16 *interval)
{
	return;
	/*
	uint16 index=0;
	
	while(*dev_id)
	{
		token_polling_table[index].dev_id=*dev_id++;
		token_polling_table[index].interval=*interval++;
		index++;
		if(index >= TBA_M_TABLE_NUM-1)
			break;
	}
	//送结束符
	token_polling_table[index].interval=0;
	//清除当前索引变量
	token_index=0;
	current_token_index=0;*/
}

//***********************************************************************************************************************
//*函数作用:查询通信质量
//*参数说明:
//*注意事项:
//*返回说明:设备数量
//***********************************************************************************************************************
uint8 tba_m_get_quality(uint8 *dev_id, uint16 *total, uint16 *error)
{
	/*
	uint16 index=0;
	
	while(token_polling_table[index].interval)
	{
		*dev_id++=token_polling_table[index].dev_id;
		*total++=token_polling_table[index].poll_total;
		*error++=token_polling_table[index].poll_error;
		index++;
	}
	return index;*/
}

//***********************************************************************************************************************
//函数作用:管理器初始化函数
//参数说明:net_id---自己的网络号  dev_id---自己的设备ID
//注意事项:
//返回说明:
//***********************************************************************************************************************
void tba_m_init(uint8 net_id, uint8 dev_id)
{	
	//记忆自己的网络号
	my_net_id=net_id;
	//记忆自己的设备ID
	my_dev_id=dev_id;
	//根据设备号产生--没有任何数据包的间隔
	tba_no_packet_interval=(my_dev_id)*TBA_M_ACK_TIMEOUT+1000;
	//设置到M
	tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
}



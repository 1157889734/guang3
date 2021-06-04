//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 上位机设置 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:ptu.c

//*文件作用:上位机设置

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"

#define error_data_length 0x38


//接收缓冲区
uint8 ptu_recv_buf[PTU_BUF_SIZE];
static uint16 ptu_recv_length=0;

static uint32 soft_update_time=0;
static uint8 ptu_debug_cmd=0;//得到调试命令的标志


extern uint8 destled_version[4];

uint8 xdata const PTU_volume_table[41][10]={
{0,1,0,0,0,0,0,0,0,0},{9,2,0,0,0,0,0,0,0,0},{15,3,0,0,0,0,0,0,0,0},{19,4,0,0,0,0,0,0,0,0},{22,5,0,0,0,0,0,0,0,0},   // 1-5
{25,6,0,0,0,0,0,0,0,0},{27,7,0,0,0,0,0,0,0,0},{28,8,0,0,0,0,0,0,0,0},{30,9,0,0,0,0,0,0,0,0},{31,10,0,0,0,0,0,0,0,0}, // 6-10
{33,11,0,0,0,0,0,0,0,0},{34,12,0,0,0,0,0,0,0,0},{35,13,0,0,0,0,0,0,0,0},{36,14,0,0,0,0,0,0,0,0},{37,15,0,0,0,0,0,0,0,0}, // 11-15
{38,16,0,0,0,0,0,0,0,0},{39,17,0,0,0,0,0,0,0,0},{40,18,19,0,0,0,0,0,0,0},{41,20,0,0,0,0,0,0,0,0},{42,21,22,0,0,0,0,0,0,0},// 16-20
{43,23,24,0,0,0,0,0,0,0},{44,25,0,0,0,0,0,0,0,0},{45,26,27,0,0,0,0,0,0,0},{46,28,29,0,0,0,0,0,0,0},{47,30,31,32,0,0,0,0,0,0},// 21-25
{48,33,34,0,0,0,0,0,0,0},{49,35,36,37,0,0,0,0,0,0},{50,38,39,40,0,0,0,0,0,0},{51,41,42,43,0,0,0,0,0,0},{52,44,45,46,0,0,0,0,0,0},// 26-30
{53,47,48,49,0,0,0,0,0,0},{54,50,51,52,53,0,0,0,0,0},{55,54,55,56,57,0,0,0,0,0},{56,58,59,60,61,62,0,0,0,0},{57,63,64,65,66,0,0,0,0,0},// 31-35
{58,67,68,69,70,71,0,0,0,0},{59,72,73,74,75,76,77,0,0,0},{60,78,79,80,81,82,83,0,0,0},{61,84,85,86,87,88,89,0,0,0},{62,90,91,92,93,94,95,96,0,0},// 36-40
{63,97,98,99,100,0,0,0,0,0}// 41
};

//*************************************************************************************************
//函数作用: 音量设置查表
//参数说明: 百分比
//返回说明: 音量值
//*************************************************************************************************
/*
static void ptu_volume_check (uint8* volume)
{
	uint8 i=0,j=0,k=0;

	dog();
	
	
	for(i=0;i<7;i++)
	{	
		for(j=0;j<41;j++)
		{
			if (i>=7)
				break;
			
			for(k=0;k<9;k++)
			{
				
				if(PTU_volume_table[j][k+1]==0 && volume[i+3]!=0)
					break;
				if(volume[i+3]==0)
				{
					switch (i)
					{
						case 0:
								saveparam.global_volume = 0 ;
								break;
						case 1:
								saveparam.occ_volume	 = 0 ;
								break;	
						case 2:
								saveparam.dva_volume = 0 ;
								break;
						case 3:
								saveparam.media_volume = 0;
								break;
						case 4:
								saveparam.door_volume = 0;
								break;
						case 5:
								saveparam.apu_bass = 0;
								break;
						case 6:
								saveparam.apu_treble = 0;
								break;
						default :
								break;
					}
					i++;
					j=0;
					break;
				}
				
				if( volume[i+3]==PTU_volume_table[j][k+1] )
				{
					switch (i)
					{
						case 0:
								saveparam.global_volume = PTU_volume_table[j][0] ;
								break;
						case 1:
								saveparam.occ_volume	 = PTU_volume_table[j][0] ;
								break;	
						case 2:
								saveparam.dva_volume = PTU_volume_table[j][0] ;
								break;
						case 3:
								saveparam.media_volume = PTU_volume_table[j][0] ;
								break;
						case 4:
								saveparam.door_volume = PTU_volume_table[j][0] ;
								break;
						case 5:
								saveparam.apu_bass = PTU_volume_table[j][0] ;
								break;
						case 6:
								saveparam.apu_treble = PTU_volume_table[j][0] ;
								break;
						default :
								break;
					}
					i++;
					j=0;
					break;
				}
			}
		}
	}
	print_int("%d i:", i);
	print_int("%d global:", saveparam.global_volume);
	print_int("%d occ:", saveparam.occ_volume);
	print_int("%d dva:", saveparam.dva_volume);
	print_int("%d media:", saveparam.media_volume);
	print_int("%d door:", saveparam.door_volume);
	print_int("%d bass:", saveparam.apu_bass);
	print_int("%d treble:", saveparam.apu_treble);
	print_line("volend");
}*/

//***********************************************************************************************************************
//函数作用:上位机设置接收数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 ptu_get_packet(void)
{
	static uint8 start_flag = 0;
	uint8 temp;
	
	//判断是否把调试口用于tms调试
	if(tms_dev->debug_flag == 44)
		return 0;
		
	while(PTU_RECEIVE_CHAR(&temp))
	{		
		//uart0_write_char(temp);
		//自定义命令
		if( !start_flag )
		{
			//读到分隔符 判断是否有数据
			if(temp == '\r')
			{
				if(ptu_recv_length)
				{
					ptu_recv_buf[ptu_recv_length]=0;//添加结束符
					ptu_recv_length=0;//缓冲区指针清0
					return 2;//得到一个完整命令
				}
			}
			else if(temp == '\n')
			{
				
			}
			else if(temp == TBA_PACKET_TAB)
			{
				start_flag = 1;
				ptu_recv_length = 0;
			}
			else if(temp)//注意过滤 \0 字符
			{
				ptu_recv_buf[ptu_recv_length++] = temp;//向缓存送数据
				if(ptu_recv_length >= PTU_BUF_SIZE-1)
					ptu_recv_length=0;//判断是否超长	
			}
		}
		//总线命令
		else
		{
			//读到分隔符 判断是否有数据 并执行逆转码 然后判断转码以后总长度和校验和
			if(temp == TBA_PACKET_TAB)
			{
				//判断长度 判断是否有数据
				if(ptu_recv_length >= 3)
				{
					start_flag=0;
					return 1;
				}
				ptu_recv_length=0;
			}
			//其他字符串直接送缓冲区
			else
			{
				ptu_recv_buf[ptu_recv_length] = temp;
				if(++ptu_recv_length >= TBA_BUF_SIZE)
					ptu_recv_length = 0;
			}
		}
	}
	
	return 0;
}


/****************************************************
*函数名：ptu_send_data			       		         
*作者：hk             							 
*描述：send  data to ptu
*输入：packet  --- the point of data buffer
                 length   ---the length of data
*输出：无	
*返回值：
*修改记录：无								
****************************************************/
static void ptu_send_data(uint8 *packet, uint8 length)
{
      uint8 * ptr = NULL;
	uint8 cnt = 0;
	uint8 send_buf[256] = {0};
	uint8 send_length = 0;
	uint8 checksum = 0;
	send_buf[send_length++] = 0x7e;
      ptr = (uint8 *)packet;
	for(cnt = 0; cnt < length; cnt++)
	{
	    if(*ptr == 0x7e)
	    {
	         send_buf[send_length++] = 0x7f;
		   send_buf[send_length++] = 0x80;
	    }
	    else if(*ptr == 0x7f)
	    {
	         send_buf[send_length++] = 0x7f;
		   send_buf[send_length++] = 0x81;
	    }
	     else
	     	{
	     	     send_buf[send_length++] = *ptr;
	     	}
	     checksum += *ptr;
	     ptr++;
	}

	checksum = 0x55 - checksum;
      //校验和
	if(checksum == 0x7e)
	{
	    send_buf[send_length++] = 0x7f;
          send_buf[send_length++] = 0x80;
	}
	else if(checksum == 0x7f)
	{
	    send_buf[send_length++] = 0x7f;
           send_buf[send_length++] = 0x81;
	}
      else
	{
	    send_buf[send_length++] = checksum;
	}
	//send_buf[send_length++] = checksum;  //校验和
      send_buf[send_length++] = 0x7e;

	PTU_SEND_BUF((uint8 *)send_buf,send_length);
	
	//uart0_write_buf((uint8 *)send_buf,send_length);//debug send data to ptu
	
}


//***********************************************************************************************************************
//函数作用:上位机设置执行命令函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void exce_ptu_cmd(uint8 *packet, uint8 length)
{
	uint8 recv_flag=0;
	uint8 checksum=0;
	uint16 src_length=0;//逆转码之前的长度
	uint16 dst_length=0;//逆转码以后的长度
	static uint8 ptu_send_data_buf[PTU_BUF_SIZE];
	uint16 ptu_recv_data_len = 0;
	uint8 tmp_len=0;
	uint8 num = 0;
	uint8 cnt = 0;
	uint8 volume = 0;
	uint32 error_addr = 0;
	int setup_cmd = 0;  //设置命令返回设置成功

	uint16 temp = 0;
	//uint16 ptu_recv_data_len =(uint16)(packet[1]&0x00ff);
	//ptu_recv_data_len |=(uint16)((packet[2]&0xff00)<<8);
	//print_line("exce_ptu_cmd");//ptu_send_data(packet, ptu_recv_length);

	//执行逆转码 并计算校验和
	for(src_length=0; src_length<ptu_recv_length; src_length++)
	{
		//拷贝数据
		packet[dst_length]=packet[src_length];
		//判断刚刚拷贝的是否是转码符
		if(packet[src_length]==TBA_PACKET_DLE && src_length<ptu_recv_length-1)
		{
			//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_TAB 并跳过下一个字符
			if(packet[src_length+1]==TBA_PACKET_DLE_TAB)
			{
				packet[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TBA_PACKET_TAB
				src_length++;//并跳过下一个字符
			}
			//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_DLE 并跳过下一个字符
			else if(packet[src_length+1]==TBA_PACKET_DLE_DLE)
			{
				packet[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TBA_PACKET_DLE
				src_length++;//并跳过下一个字符
			}
			else
			{
				print_line("ptu 7f error");
				//数据错误----7f没有被正确转码
				dst_length=0;//清除转码以后的长度
				break;
			}
		}
		checksum+=packet[dst_length];
		dst_length++;
	}
	if(checksum == 0x55)
	{
		recv_flag=1;
		//ptu_recv_length=0;
		print_line("PTU checksum ok");
	}
	else
	{
		recv_flag=0;
		//ptu_recv_length=0;
		print_int("%xPTU checksum error",checksum);
		
	}
	if(recv_flag)
	{	
	     ptu_recv_data_len = packet[1];
	     ptu_recv_data_len = ((ptu_recv_data_len<<8)&0xff00)|packet[2];
				
	    //处理命令
	    switch(packet[0])
		{
			//站代码
			case SETUP_CMD_STATIONNUM:
				print_line("SETUP_CMD_STATIONNUM");
				if(packet[3]>= STATION_NUM_MAX-1)
					break;
				saveparam.station_total_num = packet[3];
				for(tmp_len=0;tmp_len<packet[3];tmp_len++)
				{
				    temp = packet[tmp_len*2+4];
				    saveparam.stations_code_table_up[tmp_len] = (temp<<8)|packet[tmp_len*2+5];
				    //print_int("%x ", saveparam.stations_code_table[tmp_len]);
				}
				saveparam.stations_code_table_up[saveparam.station_total_num]=0;//结束符
				//保存参数
				save_set_task();
				//TMS_PORTINIT(TMS_BAUDRATE);
				//setup_cmd = 1;
				UART3_IER=0x0000;
				UART3_CR=0x0000;
				break;

			//设置音量
			case SETUP_CMD_VOLUME:
				print_line("SETUP_CMD_VOLUME");
				/*
				saveparam.global_volume_pcent=packet[3];//总音量
				saveparam.occ_volume_pcent=packet[4];
				saveparam.dva_volume_pcent=packet[5];
				saveparam.media_volume_pcent=packet[6];
				saveparam.door_volume_pcent=packet[7];
				saveparam.apu_bass_pcent=packet[8];	//重低音
				saveparam.apu_treble_pcent=packet[9];	//高音
				ptu_volume_check((uint8 *)packet);
				*/		

				saveparam.global_volume = packet[3];//总音量
				saveparam.occ_volume = packet[4];
				saveparam.dva_volume = packet[5];
				saveparam.media_volume = packet[6];
				saveparam.door_volume = packet[7];
				saveparam.apu_bass = packet[8];	//重低音
				saveparam.apu_treble = packet[9];	//高音

				if( packet[3] >0x2e )
					saveparam.global_volume = 0x2e;
	
				if( packet[4] >0x3f )
					saveparam.occ_volume = 0x3f ;

				if( packet[5] >0x3f )
					saveparam.dva_volume = 0x3f ;

				if( packet[6] >0x3f )
					saveparam.media_volume = 0x3f ;

				if( packet[7] >0x3f )
					saveparam.door_volume = 0x3f ;

				if( packet[8] >0x10 )
					saveparam.apu_bass = 0x10 ;

				if( packet[9] >0x10 )
					saveparam.apu_treble = 0x10 ;
				
				 
				//APU_SEND_BUF(ptu_send_data_buf,7);
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;
				
			//特殊广播次数
			case SETUP_CMD_SPECIALTIMES:
				print_line("SETUP_CMD_SPECIALTIMES");
				for(num = 0; num<10; num++)
				{
				    saveparam.special_broadcast_times[0]=packet[3];
				    saveparam.special_broadcast_times[2]=packet[3];
				}
				
				//print_int("%d times", packet[3]);
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;
								
			//广播优先级
			case SETUP_CMD_BROADCASTPRIORITY:
				print_line("SETUP_CMD_BROADCASTPRIORITY");
				saveparam.broadcast_priority[0]=packet[3];
				saveparam.broadcast_priority[1]=packet[4];
				saveparam.broadcast_priority[2]=packet[5];
				saveparam.broadcast_priority[3]=packet[6];
				saveparam.broadcast_priority[4]=packet[7];
				saveparam.broadcast_priority[5]=packet[8];
				saveparam.broadcast_priority[6]=packet[9];
				saveparam.broadcast_priority[7]=packet[10];
				saveparam.broadcast_priority[8]=packet[11];
				
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;
			
			//是否同时媒体播放			
			case SETUP_CMD_MEDIASAMETIME:
				saveparam.media_sametime=packet[3];
				//保存参数
				save_set_task();
				break;

                   //到站广播距离
			case SETUP_CMD_ARRIVEDISTANCE:
				//sh9_tms_dev.pre_arrived_distance = packet[3];
				//sh9_tms_dev.pre_arrived_distance = (sh9_tms_dev.pre_arrived_distance << 8)|packet[4];
				//保存参数
				//save_set_task();
				break;

			case SETUP_CMD_LEAVEDISTANCE:
				//sh9_tms_dev.leave_distance = packet[3];
				//sh9_tms_dev.leave_distance = (sh9_tms_dev.leave_distance << 8)|packet[4];
				//保存参数
				//save_set_task();
				break;

                   //查询音量
			case SETUP_CMD_DIS_VOLUME:
				print_line("SETUP_CMD_DIS_VOLUME");
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_VOLUME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x07;
				/*
				ptu_send_data_buf[temp++] = saveparam.global_volume_pcent;//总音量
				ptu_send_data_buf[temp++] = saveparam.occ_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.dva_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.media_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.door_volume_pcent;
				ptu_send_data_buf[temp++] = saveparam.apu_bass_pcent;	//重低音
				ptu_send_data_buf[temp++] = saveparam.apu_treble_pcent;	//高音
				*/

				ptu_send_data_buf[temp++] = saveparam.global_volume;//总音量
				ptu_send_data_buf[temp++] = saveparam.occ_volume;
				ptu_send_data_buf[temp++] = saveparam.dva_volume;
				ptu_send_data_buf[temp++] = saveparam.media_volume;
				ptu_send_data_buf[temp++] = saveparam.door_volume;
				ptu_send_data_buf[temp++] = saveparam.apu_bass;	//重低音
				ptu_send_data_buf[temp++] = saveparam.apu_treble;	//高音
				
				ptu_send_data(ptu_send_data_buf,temp);
				break;

			//display station code
			case SETUP_CMD_DIS_STATIONCODE:
				print_line("SETUP_CMD_DIS_STATIONCODE");
				ptu_send_data_buf[0] = SETUP_CMD_DIS_STATIONCODE; //命令字
				ptu_send_data_buf[1] = 0x00;
				ptu_send_data_buf[2] = saveparam.station_total_num*2 + 1;   //data length
				ptu_send_data_buf[3] = saveparam.station_total_num;
				temp = 4;
				for(num = 0; num < saveparam.station_total_num; num++)
				{
				    ptu_send_data_buf[temp++] = ((saveparam.stations_code_table_up[num]>>8)&0x00ff);
				    ptu_send_data_buf[temp++] = (saveparam.stations_code_table_up[num]&0x00ff);
				}
				ptu_send_data(ptu_send_data_buf,temp);
				//delay(3);

				ptu_send_data_buf[0] = SETUP_CMD_SETSTATION_NAME; //命令字
				ptu_send_data_buf[1] = 0x00;
				
				for(num = 0; num < saveparam.station_total_num; num++)
				{  
				    ptu_send_data_buf[3] = num+1;//站的顺序号从1开始
				    temp = 4;
				    for(tmp_len = 0;saveparam.station_name[num][tmp_len] != 0; tmp_len++)
				    {
				        ptu_send_data_buf[temp++] = saveparam.station_name[num][tmp_len];
				    	}
				    ptu_send_data_buf[2] = tmp_len+1;
				    ptu_send_data(ptu_send_data_buf,temp);
				    mdelay(2);
				}
				break;

			case SETUP_CMD_DIS_ERROR:
				print_line("SETUP_CMD_DIS_ERROR");
				//set_error_record(packet[3]);
				ptu_send_data_buf[0]=0x16;
				ptu_send_data_buf[1]=0x00;
				ptu_send_data_buf[2]=error_data_length;
				ptu_send_data_buf[3]=0x00;
				ptu_send_data_buf[4]=0x00;
				ptu_send_data_buf[5]=0x00;
				ptu_send_data_buf[6]=0x00;
				ptu_send_data_buf[7]=0x00;
				memcpy((uint8*)&ptu_send_data_buf[8], (uint8*)&error_pack, sizeof(error_packet_t));
				ptu_dev->ptu_send_data(ptu_send_data_buf,error_data_length+3);
				break;

			//版本号
			case SETUP_CMD_VERSION:
				print_line("SETUP_CMD_VERSION");
				//应答
				ptu_send_data_buf[0]=0x7e;
				ptu_send_data_buf[0]=0x00;
				ptu_send_data_buf[1]=0x05;
				ptu_send_data_buf[2]=SETUP_CMD_VERSION;
				ptu_send_data_buf[3]=HARDWARE_VERSION;
				ptu_send_data_buf[4]=ADD_FUNCTION_VERSION;
				ptu_send_data_buf[5]=MAJOR_VERSION;
				ptu_send_data_buf[6]=MINOR_VERSION;

				ptu_send_data(ptu_send_data_buf,7);
				//tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,5);
	
				break;

			//复位
			case SETUP_CMD_RESET:
				while(1);
				break;

			case SETUP_CMD_LOAD_DEFAULT_PARM:
                          error_addr = saveparam.error_flash_addr;
				volume = saveparam.panel_volume_param;
				num = saveparam.re_write_flash;
				save_load_default();
				saveparam.error_flash_addr = error_addr;
				saveparam.re_write_flash = num;
				saveparam.panel_volume_param = volume;
				print_line("SETUP_CMD_LOAD_DEFAULT_PARM");
				setup_cmd = 1;
				break;
			case SETUP_CMD_DIS_BROADCASTPRIORITY:
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_BROADCASTPRIORITY;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x09;
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[0];//0cc
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[1];//乘客报警
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[2];//司机对讲
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[3];//人工广播
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[4];//关门提示音
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[5];	//数字报站
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[6];	//媒体播放
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[7];//紧急广播
				ptu_send_data_buf[temp++] = saveparam.broadcast_priority[8];//test测试语音
				ptu_send_data(ptu_send_data_buf,temp);
				break;
			case SETUP_CMD_SETSTATION_NAME:
				temp = ptu_recv_data_len;
				for(temp=0;temp<(ptu_recv_data_len-1);temp++)
				{
				    saveparam.station_name[packet[3]-1][temp] = packet[4+temp];
				}
				saveparam.station_name[packet[3]-1][temp] = 0;
				//saveparam.station_name[packet[3]][0] = 0;
				//保存参数
				save_set_task();
				//dog();
				//softdog();
				if(saveparam.station_total_num == packet[3])
				{
				   setup_cmd = 1;
				   TMS_PORTINIT(TMS_BAUDRATE);
				}
				break;

			case SETUP_CMD_BROC_DELAY_TIME:
				print_line("SETUP_CMD_BROC_DELAY_TIME");
			      for(temp = 0; temp < ptu_recv_data_len; temp++)
			      {
				    saveparam.broadcast_delay_time[temp] = packet[3+temp]*1000;
					print_int("%d ", saveparam.broadcast_delay_time[temp] );
				}
							
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;
			case SETUP_CMD_DIS_BROC_DELAY_TIME:
				print_line("SETUP_CMD_DIS_BROC_DELAY_TIME");
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_BROC_DELAY_TIME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = saveparam.station_total_num;
				for(num=0; num<saveparam.station_total_num; num++)
				{
				    ptu_send_data_buf[temp++]= (saveparam.broadcast_delay_time[num] / 1000);
				}
				ptu_send_data(ptu_send_data_buf,temp);

				break;
				
			case SETUP_CMD_UPDATE_SOFT:   //升级程序转发
				print_line("update begin");
				uart0_write_char(0x7e);
				uart0_write_buf(&ptu_recv_buf[3], ptu_recv_length-4);
				uart0_write_char(0x7e);
		             TBA_BUS_SEND_CHAR(0x7e);
				//之前已经做过逆转码处理，所有需再次转码
				 for(temp = 3; temp < (ptu_recv_length-4 +3); temp++)
				 {
				     if(ptu_recv_buf[temp] == 0x7e)
				     	{
				     	    TBA_BUS_SEND_CHAR(0x7f);
					    TBA_BUS_SEND_CHAR(0x80);
				     	}
				      else if(ptu_recv_buf[temp] == 0x7f)
				      	{
				      	    TBA_BUS_SEND_CHAR(0x7f);
					    TBA_BUS_SEND_CHAR(0x81);
				      	}
					 else
					 {
					     TBA_BUS_SEND_CHAR(ptu_recv_buf[temp]);
					 }
				 }
   			      //TBA_BUS_SEND_BUF(&ptu_recv_buf[3], ptu_recv_length-4);
   			      TBA_BUS_SEND_CHAR(0x7e);  
   				soft_update_time = get_clock();
   			      while(1)
   			      {
   			        temp = ptu_get_packet();
   				  if(temp )
   				  {
   				   	TBA_BUS_SEND_CHAR(0x7e);
   				  	if(((0x7f ==ptu_recv_buf[2])&&(0x80 ==ptu_recv_buf[3])) ||((0x7f ==ptu_recv_buf[2])&&(0x81 ==ptu_recv_buf[3])))
						TBA_BUS_SEND_BUF(&ptu_recv_buf[4], ptu_recv_length-5);
					else 
   					 	TBA_BUS_SEND_BUF(&ptu_recv_buf[3], ptu_recv_length-4);
					   
   					 TBA_BUS_SEND_CHAR(0x7e);  
				/*
					 uart0_write_char(0x7e);
				uart0_write_buf(&ptu_recv_buf[3], ptu_recv_length-4);
				uart0_write_char(0x7e);
				*/
   					 soft_update_time = get_clock();
   				  }
   				  dog();
				  softdog(); //防止五分钟后复位
   				  if(checktimer(&soft_update_time, 4000))
   				  	break;
   			      }
				print_line("update end");
				break;

			case SETUP_CMD_DMP_WDS_VERSION:	//查询DMP  & WDS  version

				print_line("check dmp wds version");
				
				ptu_send_data_buf[0]=SETUP_CMD_DMP_WDS_VERSION;
				ptu_send_data_buf[1]=00;
				ptu_send_data_buf[2]=12;

				cnt=0;
				for (num=0;num<6;num++)
				{
					ptu_send_data_buf[cnt*2+3]=lcu_default_data[num].version_l;
					ptu_send_data_buf[cnt*2+4]=lcu_default_data[num].version_h;
					cnt++;
				}
				ptu_send_data(ptu_send_data_buf,15);
				break;

			case SETUP_CMD_DIS_FLASH:
				print_line("DIS_FLASH");
				set_error_record(packet[3]);
				break;


			case SETUP_CMD_SECTION_PLAYLIST:        //设置区间运行终点站提示语播放列表
                           print_line("SETUP_CMD_SECTION_PLAYLIST");
                           if(packet[3]>=1 && packet[3]<=30)//判断终点站id 范围
                           {
					saveparam.endstation_mind_list[packet[3]-1][0]=packet[4];//更新列表
					saveparam.endstation_mind_list[packet[3]-1][1]=packet[5];
					saveparam.endstation_mind_list[packet[3]-1][2]=packet[6];
					saveparam.endstation_mind_list[packet[3]-1][3]=packet[7];
					saveparam.endstation_mind_list[packet[3]-1][4]=packet[8];
				}
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;

			case SETUP_CMD_DIS_SECTION_PLAYLIST:        //查询区间运行终点站提示语播放列表
				print_line("SETUP_CMD_DIS_SECTION_PLAYLIST");
				ptu_send_data_buf[0] = SETUP_CMD_DIS_SECTION_PLAYLIST;
				ptu_send_data_buf[1] = 0;
				ptu_send_data_buf[2] = 6; //数据长度

				for(cnt=0;cnt<30;cnt++)
				{
					ptu_send_data_buf[3] = cnt+1;
					ptu_send_data_buf[4] = saveparam.endstation_mind_list[cnt][0];
					ptu_send_data_buf[5] = saveparam.endstation_mind_list[cnt][1];
					ptu_send_data_buf[6] = saveparam.endstation_mind_list[cnt][2];
					ptu_send_data_buf[7] = saveparam.endstation_mind_list[cnt][3];
					ptu_send_data_buf[8] = saveparam.endstation_mind_list[cnt][4];

					ptu_send_data(ptu_send_data_buf,9);
				}				
				break;
				
			//设置广播盒音量
			case SETUP_CMD_DCP_VOLUME:
				print_line("SETUP_CMD_DCP_VOLUME");
				for(cnt=3;cnt<9;cnt++)
				{
					if(packet[cnt]>15 || packet[cnt]<0)
					{
						print_line("volume value >15");
						break;
					}
				}
				if(cnt>=9)
				{
					saveparam.dcp_pa_listen_volume=packet[3];
					saveparam.dcp_cc_listen_volume=packet[4];
					saveparam.dcp_pc_listen_volume=packet[5];
					saveparam.dcp_pa_mic_volume=packet[6];
					saveparam.dcp_cc_mic_volume=packet[7];
					saveparam.dcp_pc_mic_volume=packet[8];				
				}
				//保存参数
				save_set_task();
				setup_cmd = 1;
				break;

			//查询广播盒音量
			case SETUP_CMD_DIS_DCP_VOLUME:
				temp = 0;
				ptu_send_data_buf[temp++] = SETUP_CMD_DIS_DCP_VOLUME;
				ptu_send_data_buf[temp++] = 0x00;
				ptu_send_data_buf[temp++] = 0x06;
				ptu_send_data_buf[temp++] = saveparam.dcp_pa_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_cc_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pc_listen_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pa_mic_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_cc_mic_volume;
				ptu_send_data_buf[temp++] = saveparam.dcp_pc_mic_volume;
				ptu_send_data(ptu_send_data_buf,temp);
				break;
									
			default:
				break;
		}
		if(setup_cmd)
		{
		         ptu_send_data_buf[0] = SETUP_CMD_OK;
			     ptu_send_data_buf[1] = 0x00;
			     ptu_send_data_buf[2] = 0x00;
			     ptu_send_data(ptu_send_data_buf,3);
		}
	}


}
//***********************************************************************************************************************
//函数作用:上位机设置数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void ptu_proc(void)
{
	uint8 type;
	
	//接收命令
	type = ptu_get_packet();	
	//分析命令
	if(type == 1)
	{
	/*
		//转发到列车总线
		TBA_BUS_SEND_CHAR(TBA_PACKET_TAB);
		TBA_BUS_SEND_BUF(ptu_recv_buf, ptu_recv_length);
		TBA_BUS_SEND_CHAR(TBA_PACKET_TAB);
	*/
		//执行PTU CMD
		exce_ptu_cmd(ptu_recv_buf, ptu_recv_length);
		ptu_recv_length = 0;
	}
	else if(type == 2)
	{
		ptu_debug_cmd = 1;
	}
	
	//通信指示灯
	if(type)
		led_toggle(PTU_LED_NO);
}

//***********************************************************************************************************************
//函数作用:上位机设置数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 *ptu_get_debug_cmd(void)
{
	if(!ptu_debug_cmd)
		return NULL;
	ptu_debug_cmd=0;
	return ptu_recv_buf;
}

//***********************************************************************************************************************
//函数作用:上位机设置初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void ptu_init(void)
{	
	//初始化通信端口
	PTU_PORTINIT(PTU_BAUDRATE);
	
	//产品信息
	delay(1);
	print_line(VENDER_NAME);
	print_line(SYSTEM_NAME);
	print_line(PRODECT_NAME);
	print_line(VERSION_DATE);
	print_line("VER: "PRODECT_CODE);
	print_int("%d",HARDWARE_VERSION);
	print_int("%d",ADD_FUNCTION_VERSION);
	print_int("%d",MAJOR_VERSION);
	print_int("%d",MINOR_VERSION);
}



//输出虚拟设备类
ptu_dev_t sh9_ptu_dev=
{
	ptu_init,
	ptu_proc,
	ptu_get_debug_cmd,
	ptu_send_data,
	1,
};



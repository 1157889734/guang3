//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& PC设置软件 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:setup.c

//*文件作用:PC设置软件

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//设置命令
#define SETUP_CMD_STATIONNUM			0x01	//车站总数
#define SETUP_CMD_STATIONCODE			0x02	//站代码
#define SETUP_CMD_STATIONDISTANCE		0x03	//站间距
#define SETUP_CMD_PULSEDISTANCE			0x04	//脉冲运行距离
#define SETUP_CMD_ARRIVEDISTANCE		0x05	//到站广播距离
#define SETUP_CMD_LEAVEDISTANCE			0x06	//离站广播距离
#define SETUP_CMD_VOLUME			0x07	//音量
#define SETUP_CMD_LEAVETABLE			0x08	//离站广播列表
#define SETUP_CMD_ARRIVETABLE			0x09	//到站广播列表
#define SETUP_CMD_SPECIALTIMES			0x0B	//特殊广播次数
#define SETUP_CMD_BROADCASTPRIORITY		0x0C	//广播优先级
#define SETUP_CMD_MEDIASAMETIME			0x0D	//是否同时媒体播放
#define SETUP_CMD_MTABLE			0x0E	//令牌巡检表

#define SETUP_CMD_VERSION			0x11	//查询版本号
#define SETUP_CMD_RESET				0x12	//复位


//应答字
#define SETUP_ACK_DOWNLOAD			0x18	//下载程序应答


//应答缓冲区
static uint8 setup_ack_buf[TBA_BUF_SIZE];
//数据缓冲区
static uint8 setup_data_buf[TBA_BUF_SIZE];


//下载数据包结构
#define DOWNLOAD_DATA_SIZE	128
typedef struct
{
	uint32 total_length;//总长度
	uint32 current_length;//当前长度
	uint8 data_buf[DOWNLOAD_DATA_SIZE];//数据
}download_packet_t;
static download_packet_t *download_packet=(download_packet_t *)&setup_data_buf;



static uint32 total_length=0;//总长度
static uint32 recv_num=0;//已经接收的长度
static uint8 download_step=0;//下载步骤
static uint16 checksum_add1=0;//自己计算的累加校验和
static uint16 checksum_xor1=0;//自己计算的异或校验和



//***********************************************************************************************************************
//函数作用:PC设置软件处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void setup_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
	//过程数据
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		
	}
	//下载程序
	else if(data_type==TBA_CMD_DOWNLOAD)
	{
		uint8 dev_no=src_dev_id>>4;
		uint8 encrypt;
		uint16 i;
		
		//判断数据报总长度
		if(length!=sizeof(download_packet_t))
			return;
		//判断源设备编号
		if((dev_no!=1))
			return;
		//拷贝数据
		memcpy(setup_data_buf, packet, sizeof(setup_data_buf));
		
		//如果正在搬移程序 退出
		if(download_step==3)
			return;
		
		//如果是擦除备份FLASH的命令 清总校验和 擦flash
		if(download_packet->current_length==0)
		{
			//判断总长度
			if(download_packet->total_length>FLASH_BACKUP_LENGTH)
			{
				download_step=0;
				print_line("LEN ERROR");
				//错误应答
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				return;					
			}
			//设置擦除备份FLASH
			save_set_erase();
			//纪录总长度 清除当前接收的长度 清除校验和 送步骤标志
			total_length=download_packet->total_length;
			recv_num=0;
			checksum_add1=0;
			checksum_xor1=0;
			download_step=1;
			//到此为止 不能往下写FLASH 因为不包含数据
			return;
		}
		
		//数据写入FLASH阶段
		if(download_step==1)
		{			
			//数据解密
			encrypt=download_packet->data_buf[0]^0x55;
			for(i=1;i<DOWNLOAD_DATA_SIZE;i++)
				download_packet->data_buf[i] ^= encrypt;
			//把接收缓存的数据写入flash
			if(!flash_write(download_packet->data_buf,FLASH_BACKUP_BASE+recv_num,DOWNLOAD_DATA_SIZE))
			{
				download_step=0;
				print_line("WRITE ERROR");//写入失败 错误应答
				return;
			}
			print_line("WRITE OK");//一个包写入完毕 发送应答

			//计算校验和
			for(i=0;i<DOWNLOAD_DATA_SIZE;i++)
				checksum_add1 += *((uint8 *)(FLASH_BACKUP_BASE+recv_num+i));//计算累加校验和
				
			for(i=0;i<DOWNLOAD_DATA_SIZE;i+=2)
				checksum_xor1 ^= *((uint16 *)(FLASH_BACKUP_BASE+recv_num+i));//计算异或校验和
				
			//指向下一包数据
			recv_num += DOWNLOAD_DATA_SIZE;
                	//判断丢包
			if(download_packet->current_length != recv_num)
			{
				download_step=0;
				print_line("LOSS DATA PACKET");//丢失数据包
				//错误应答
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				return;
			}
			//判断进入下一步
			if(recv_num>=total_length)
			{
				download_step=2;
				//print_line("WAIT VERIFY");
			}
		}
		//程序校验和搬移阶段
		else if(download_step==2)
		{
			//清标志
			download_step=0;
					
			//如果是最后的校验和包 判断校验和 并进行程序搬移---从备份flash拷贝到运行flash 拷贝完毕利用看门狗复位
			if(download_packet->total_length==0)
			{				
				//判断校验和
				if((*((uint16 *)&download_packet->data_buf[0])!=checksum_add1) || (*((uint16 *)&download_packet->data_buf[2])!=checksum_xor1))
				{
					print_line("VERIFY ERROR1");
					//错误应答
					setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
					setup_ack_buf[1]=0;
					tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
					return;
				}
				print_line("VERIFY OK");
				
				//成功应答
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=1;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				
				//设置更新程序的标志---不能在这里更新 因为要应答PC机
				save_set_update();
				
				//进入下一步
				download_step=3;
			}
			//如果是其他包 说明最后一包丢失
			else
			{
				//错误应答
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);

				print_line("LOSS LAST PACKET");//丢失数据包 错误应答			
			}		
		}
	}
	//点对点
	else if(data_type==TBA_CMD_P2PPACKET)
	{		
		switch(packet[0])
		{
		#if 0
			//站代码
			case SETUP_CMD_STATIONCODE:
				if(packet[1] >= STATION_NUM_MAX-1)
					break;
				saveparam.stations_code_table[packet[1]] = ((uint16)packet[2]<<8)|packet[3];
				saveparam.stations_code_table[saveparam.station_total_num]=0;//结束符
				//保存参数
				save_set_task();
				break;
              #endif
			//设置音量
			case SETUP_CMD_VOLUME:
				saveparam.global_volume=packet[1];
				saveparam.occ_volume=packet[2];
				saveparam.dva_volume=packet[3];
				saveparam.media_volume=packet[4];
				//保存参数
				save_set_task();
				break;
			
			//离站广播列表
			case SETUP_CMD_LEAVETABLE:
			#if 0
				if(packet[2] >= STATION_NUM_MAX-1)
					break;
				{
					uint8 num=0;
					uint16 *src = (uint16 *)&packet[3];
					uint16 *dst;
					
					if(packet[1] == 0)
						dst = &saveparam.dva_leave_list_up[packet[2]][0];
					else
						dst = &saveparam.dva_leave_list_down[packet[2]][0];
					while(src[num])
					{
						dst[num] = src[num];
						if( ++num >= DVA_LIST_MAX-1 )
							break;//判断长度
					}
					dst[num]=0;//结束符
				}
				//保存参数
				save_set_task();
			#endif
				break;
			
			//特殊广播次数
			case SETUP_CMD_SPECIALTIMES:
				saveparam.special_broadcast_times[0]=packet[1];
				saveparam.special_broadcast_times[1]=packet[2];
				saveparam.special_broadcast_times[2]=packet[3];
				//保存参数
				save_set_task();
				break;
								
			//广播优先级
			case SETUP_CMD_BROADCASTPRIORITY:
				saveparam.broadcast_priority[0]=packet[1];
				saveparam.broadcast_priority[1]=packet[2];
				saveparam.broadcast_priority[2]=packet[3];
				saveparam.broadcast_priority[3]=packet[4];
				saveparam.broadcast_priority[4]=packet[5];
				saveparam.broadcast_priority[5]=packet[6];
				saveparam.broadcast_priority[6]=packet[7];
				saveparam.broadcast_priority[7]=packet[8];
				//保存参数
				save_set_task();
				break;
			
			//是否同时媒体播放			
			case SETUP_CMD_MEDIASAMETIME:
				saveparam.media_sametime=packet[1];
				//保存参数
				save_set_task();
				break;
			
			//令牌巡检表
			case SETUP_CMD_MTABLE:
			{
				uint8 index=0;
				//查找相应的设备
				while(saveparam.token_table_interval[index])
				{
					if(saveparam.token_table_dev_id[index]==packet[1])
					{
						saveparam.token_table_dev_id[index]=packet[1];
						saveparam.token_table_interval[index]=((uint16)packet[2]<<8)|packet[3];
				
						//判断是否要删除
						if(saveparam.token_table_interval[index]==0)
						{
							while(saveparam.token_table_interval[index+1])
							{
								saveparam.token_table_dev_id[index]=saveparam.token_table_dev_id[index+1];
								saveparam.token_table_interval[index]=saveparam.token_table_interval[index+1];
								index++;
								if(index>=TBA_M_TABLE_NUM-1)break;
							}
							saveparam.token_table_dev_id[index]=0;
							saveparam.token_table_interval[index]=0;
						}
						break;
					}
					index++;
					if(index>=TBA_M_TABLE_NUM-1)break;
				}
				//如果没有找到设备说明是添加的新设备 添加到末尾 并添加结束符
				if((saveparam.token_table_interval[index]==0) && (index<TBA_M_TABLE_NUM-1))
				{
					saveparam.token_table_dev_id[index]=packet[1];
					saveparam.token_table_interval[index]=((uint16)packet[2]<<8)|packet[3];
					if(saveparam.token_table_interval[index])index++;
					//添加结束符
					saveparam.token_table_dev_id[index]=0;
					saveparam.token_table_interval[index]=0;
				}
				//设置到M
				tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
				//保存参数
				save_set_task();
				break;
			}
			
			//版本号
			case SETUP_CMD_VERSION:
				//应答
				setup_ack_buf[0]=SETUP_CMD_VERSION;
				setup_ack_buf[1]=HARDWARE_VERSION;
				setup_ack_buf[2]=ADD_FUNCTION_VERSION;
				setup_ack_buf[3]=MAJOR_VERSION;
				setup_ack_buf[4]=MINOR_VERSION;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,5);
				break;

			//复位
			case SETUP_CMD_RESET:
				while(1);
				break;
										
			default:
				break;
		}
	}
}



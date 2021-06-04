//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 面板设备类 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:panel.c

//*文件作用:面板设备类

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//通信协议关键字
#define PANEL_PACKET_HEAD		0x7E	//命令头
#define PANEL_PACKET_END		0x0D	//命令尾

//接收的命令
#define PANEL_PACKET_KEY		0x04	//收到键盘包




//接收缓冲区
static uint8 panel_recv_buf[PANEL_BUF_SIZE];//接收缓冲区
//static uint8 panel_recv_length=0;//接收总长度
//static uint8 panel_data_length=0;//数据部分的长度


//私有变量
static uint32 panel_send_timer=0;//定时发送定时器
static uint8 panel_volume=0;//音量
static uint8 panel_mode=0;//模式
static uint8 panel_mode_backup=0;//记忆模式


//***********************************************************************************************************************
//函数作用:面板发送命令
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void panel_send_cmd(uint8 cmd, uint8 *param, uint8 length)
{
     #if 0  //delete by hk
	//发送缓冲区
	uint8 panel_send_buf[PANEL_BUF_SIZE];
	uint8 panel_send_length=0;
	//校验和
	uint8 checksum=cmd;
	
	panel_send_length=0;
	panel_send_buf[panel_send_length++]=PANEL_PACKET_HEAD;//头
	panel_send_buf[panel_send_length++]=length+1;//长度
	panel_send_buf[panel_send_length++]=cmd;//命令字
	//参数
	if(length)
	{
		memcpy(&panel_send_buf[panel_send_length],param,length);
		panel_send_length+=length;
	}
	while(length--)
		checksum+=*param++;
	panel_send_buf[panel_send_length++]=checksum;//校验和
	panel_send_buf[panel_send_length++]=PANEL_PACKET_END;//尾
	
	//发送
	//PANEL_SEND_BUF(panel_send_buf,panel_send_length);
#endif

      //add by hk
	uint8 buf[PANEL_BUF_SIZE]={0};
	buf[0]=cmd;
	if(length>PANEL_BUF_SIZE)
	{

		return;
	}		
	else 
	{
	    memcpy((uint8 *)&buf[1],param,length);
	}
	
	cab_bus_dev->send_data(CAB_PANEL,0x0f,1,length+1,buf);
}

//***********************************************************************************************************************
//函数作用:面板接收数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
#if 0
static uint8 panel_get_packet(void)
{
	static uint8  start_flag=0;
	static uint16 timeout=0;
	uint8 temp;
	uint8 checksum;
	uint8 i;
	
	//接收直到没有数据
	while(PANEL_RECEIVE_CHAR(&temp))
	{		
		//没有收到起始符的时候 判断是不是起始符
		if(!start_flag)
		{
			//如果是起始符 送标志并保存底层fifo读指针---以便发现假起始符的时候的恢复
			if(temp==PANEL_PACKET_HEAD)
			{
				start_flag=1;
				PANEL_FIFO_BACKUP();
			}
			//其他字符 当作自定义命令
			else
			{
				
			}
		}
		//如果已经收到起始符 判断是不是真正的起始符
		else
		{			
			//刚刚接收到起始符 先接收第二个字节(长度) 把当前字节记忆为长度
			if(start_flag==1)
			{
				start_flag++;
				panel_data_length=temp;
				timeout=2000;//设置接收超时
			}
			//已经接收到长度 就把数据放入缓冲区 接收完毕计算校验和
			else
			{
				//没有接收完毕 送缓冲区
				if(panel_recv_length < panel_data_length)
				{
					panel_recv_buf[panel_recv_length++] = temp;//向缓存送数据
					if(panel_recv_length >= PANEL_BUF_SIZE-1)
						panel_recv_length = 0;//判断是否超长	
				}		
				//接收完毕 当前字节就是校验和 检查校验和
				else
				{
					//清标志
					start_flag=0;
					timeout=0;
					panel_recv_length=0;
					//计算校验和
					checksum=0;
					for(i=0; i<panel_data_length; i++)
						checksum += panel_recv_buf[i];
					//校验和错误 当前起始符是假的 恢复底层fifo读指针
					if(checksum != temp)
					{
						PANEL_FIFO_RESUME();
						print_line("dva checksum error");
					}
					//校验和正确 处理数据
					else
					{
						return 1;
					}
				}
			}
		}
	}
	
	//判断接收超时
	if(timeout)
	{
		//超时说明 当前起始符是假的
		if(--timeout==0)
		{
			//清标志
			start_flag=0;
			timeout=0;
			panel_recv_length=0;
			//当前起始符是假的 恢复底层fifo读指针
			PANEL_FIFO_RESUME();
			print_line("dva recv timeout");
		}
	}
	
	return 0;
}
#endif
/****************************************************
*函数名：panel_proc			       		         
*作者：hk             							 
*描述：面板数据处理				
*输入：device_no       --- 设备号
                 data_length    ---数据长度
                 buf                ---处理的数据
                  
*输出：无	
*返回值：无	
*修改记录：无								
****************************************************/
 void panel_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{
	//应答缓冲区
	uint8 ack_buf[]={0,0};
	
	memcpy((uint8 *)panel_recv_buf, buf, data_length);
	//接收数据
	//if(panel_get_packet())
	{
		//键盘
		if(panel_recv_buf[0]==PANEL_PACKET_KEY)
		{
			shzh24_panel_dev.refresh_timer=65500;
			//分类处理键盘
			/*switch(panel_recv_buf[1])
			{
				//模式键
				case KEY_PRESET:
					panel_mode=MODE_PRESET;
					break;
					
				case KEY_MUTE:
					panel_mode=MODE_MUTE;
					break;
					
				case KEY_MANUAL:
					panel_mode=MODE_MANUAL;
					break;
				
				case KEY_AUTO:
					panel_mode=MODE_AUTO;
					break;
				
				case KEY_TEST:
					//判断进入和退出测试模式
					if(panel_mode!=MODE_TEST)
					{
						panel_mode_backup=panel_mode;
						panel_mode=MODE_TEST;
					}
					else
					{
						panel_mode=panel_mode_backup;
					}
					break;
				
				//音量键	
				case KEY_60DB:
					panel_volume=VOLUME_60DB;
					break;
					
				case KEY_70DB:
					panel_volume=VOLUME_70DB;
					break;
					
				case KEY_80DB:
					panel_volume=VOLUME_80DB;
					break;

				case KEY_90DB:
					panel_volume=VOLUME_90DB;
					break;
					
				case KEY_100DB:
					panel_volume=VOLUME_100DB;
					break;
							
				default:
					break;
			}
			*/
			panel_mode = panel_recv_buf[1];
			panel_volume = panel_recv_buf[2];
								
			//判断参数改变 保存参数
			if(((panel_mode<<4)|panel_volume)!=saveparam.panel_volume_param)
			{
				saveparam.panel_volume_param=(panel_mode<<4)|panel_volume;
				save_set_task();
				print_int("%d set panel volume:", saveparam.panel_volume_param);
			}
		}
		
		//应答---根据模式和音量送面板指示灯
		ack_buf[0]=saveparam.panel_volume_param>>4;
		ack_buf[1]=saveparam.panel_volume_param&0x0F;
		panel_send_cmd(PANEL_ACK_LED,ack_buf,2);
	}
	
	//定时发送应答--防止干扰
	if(checktimer(&panel_send_timer,800))
	{
		ack_buf[0]=saveparam.panel_volume_param>>4;
		ack_buf[1]=saveparam.panel_volume_param&0x0F;
		panel_send_cmd(PANEL_ACK_LED,ack_buf,2);			
	}
}

//***********************************************************************************************************************
//函数作用:面板初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void panel_init(void)
{	
	//初始化通信端口
	PANEL_PORTINIT(PANEL_BAUDRATE);
	
	//装载参数
	panel_mode=saveparam.panel_volume_param>>4;
	panel_mode_backup=panel_mode;
	panel_volume=saveparam.panel_volume_param&0x0F;
}



//输出虚拟设备类
panel_dev_t shzh24_panel_dev=
{
	panel_init,
	panel_proc,
	
	0,//刷新时间 用于两边同步
};


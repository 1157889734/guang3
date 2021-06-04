
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& OCC &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:sh9_occ.c

//*文件作用:OCC 

//*文件作者:

//*创建日期:2010年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"




//通信协议关键字
#define OCC_PACKET_HEAD			0x7E	//命令头
#define OCC_PACKET_END				0x0D	//命令尾
#define OCC_PACKET_DEFAULT		0x01	//过程数据

#define OCC_ACK_DEFAULT			0x81	//过程数据应答



//接收缓冲区
static uint8 occ_recv_buf[OCC_BUF_SIZE];//接收缓冲区
static uint8 occ_recv_length=0;//接收总长度
static uint8 occ_data_length=0;//数据部分的长度
//发送缓冲区
static uint8 occ_send_buf[OCC_BUF_SIZE];//发送缓冲区
static uint8 occ_send_length=0;//发送总长度

static uint8 occ_cmd = 0;    //0:stop occ  1:start occ

//接受数据结构
__packed typedef struct
{
	uint8 cmd;
	uint8 DeviceState[8];	
}occ_recv_t;
static occ_recv_t *occ_recv=(occ_recv_t *)&occ_recv_buf;



//***********************************************************************************************************************
//函数作用:OCC 接收数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 occ_get_packet(void)
{
	static uint8  start_flag=0;
	static uint16 timeout=0;
	uint8 temp;
	uint8 checksum;
	uint8 i;
	
	//接收直到没有数据
	while(OCC_RECEIVE_CHAR(&temp))
	{
		//没有收到起始符的时候 判断是不是起始符
		if( !start_flag )
		{
			//如果是起始符 送标志并保存底层fifo读指针---以便发现假起始符的时候的恢复
			if(temp == OCC_PACKET_HEAD)
			{
				start_flag = 1;
				//OCC_FIFO_BACKUP();
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
				occ_data_length=temp;
				timeout=2000;//设置接收超时
			}
			//已经接收到长度 就把数据放入缓冲区 接收完毕计算校验和
			else
			{
				//没有接收完毕 送缓冲区
				if(occ_recv_length<occ_data_length)
				{
					occ_recv_buf[occ_recv_length++]=temp;//向缓存送数据
					if(occ_recv_length>=OCC_BUF_SIZE-1)
						occ_recv_length=0;//判断是否超长	
				}		
				
				//接收完毕 当前字节就是校验和 检查校验和
				else if(temp == 0x0d)
				{
				
				start_flag=0;
				//uart0_write_char(0xaa);
				//uart0_write_char(occ_recv_buf[0]);
				//uart0_write_buf(occ_recv_buf, occ_recv_length);
				occ_recv_length=0;
				return 1;
				}
				else
				{
					     //OCC_FIFO_RESUME();
					     print_line("occ data error");
				}
					//清标志
					start_flag=0;
					timeout=0;
					occ_recv_length=0;
					//计算校验和
					checksum=0;
					for(i=0;i<occ_data_length;i++)
						checksum^=occ_recv_buf[i];
					//校验和错误 当前起始符是假的 恢复底层fifo读指针
					if(checksum!=temp)
					{
						OCC_FIFO_RESUME();
						print_line("occ checksum error");
					}
					//校验和正确 处理数据
					else
					{
						return 1;
					}
				
			}
		}
	}
	
	//判断接收超时
	/*if(timeout)
	{
		//超时说明 当前起始符是假的
		if(--timeout==0)
		{
			//清标志
			start_flag=0;
			timeout=0;
			occ_recv_length=0;
			//当前起始符是假的 恢复底层fifo读指针
			OCC_FIFO_RESUME();
			print_line("occ recv timeout");
		}
	}
	*/
	return 0;
}

//***********************************************************************************************************************
//函数作用:OCC 数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void occ_proc(void)
{
       static uint16 num = 0;  //指示灯显示次数
	//接收数据
	if(occ_get_packet())
	{
               num = 5000;
		//清除通信错误
		sh9_occ_dev.comm_error=0;

		if(occ_recv->cmd != 0x55 && occ_recv->cmd != 0xbb)
		{
		      //OCC数据异常
		}
		else
		{
		    occ_send_buf[0] = 0x7e;
		    occ_send_buf[1] = 0x01;
		    if(occ_recv->cmd == 0x55)  //occ start
		    {
		    	   occ_cmd = 1; 
			   occ_send_buf[2] = 0xaa;  // 广播应答
		    }
		    else //occ stop
		    {
		        occ_cmd = 0;
			 occ_send_buf[2] = 0xcc;
		    }
		    occ_send_buf[3] = 0x0d;
		    occ_send_length = 4;
		    //发送应答
		   OCC_SEND_BUF(occ_send_buf, occ_send_length);
		}

	}
	else
		{
		//uart0_write_char(0x55);
		occ_recv->cmd = 0;
		//通信灯
		if(num == 0)
			led_onoff(OCC_LED_NO, 1);
		}
	if(num)
	{     num--;
	    	//通信灯
		//led_toggle(OCC_LED_NO);
		 led_blink(OCC_LED_NO,300,300);
	}
	//增加通信错误次数
	if( ++sh9_occ_dev.comm_error > COMM_DATA_TIMEOUT )
	{
		//sh9_occ_dev.comm_error = COMM_DATA_TIMEOUT;
		//memset((uint8 *)&sh9_occ_dev.device_error, 0xFF, 8);
	}
}

//***********************************************************************************************************************
//函数作用:OCC 发送数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void occ_send_data(uint8 *buf, uint8 length)
{
	uint8 checksum=0;
	uint8 *p=&occ_send_buf[2];
	
	occ_send_length=0;
	//添加起始符
	occ_send_buf[occ_send_length++]=OCC_PACKET_HEAD;
	//送长度
	occ_send_buf[occ_send_length++]=length+1;//加上命令的长度
	//送过程数据命令字
	occ_send_buf[occ_send_length++]=OCC_PACKET_DEFAULT;
	//拷贝数据
	memcpy(&occ_send_buf[occ_send_length], buf, length);
	occ_send_length += length;
	//计算校验和
	checksum=0;
	while(p < &occ_send_buf[occ_send_length])
		checksum ^= *p++;
	occ_send_buf[occ_send_length++] = checksum;
	//添加结束符
	occ_send_buf[occ_send_length++] =OCC_PACKET_END;
}

//***********************************************************************************************************************
//函数作用:OCC获取播放
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 occ_get_playing(void)
{
       occ_proc();
	return occ_cmd;
}
//***********************************************************************************************************************
//函数作用OCC 初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void occ_init(void)
{	
	//初始化通信端口
	OCC_PORTINIT(OCC_BAUDRATE);
	
}

//输出虚拟设备类
occ_dev_t sh9_occ_dev=
{
	occ_init,
	occ_proc,
	occ_send_data,
	occ_get_playing,
	
	0,//串口通信错误
	
};

#include "include.h"
extern int uartFd;
#include <termios.h>

unsigned char uart_recv_buf[UART_BUF_LEN];
unsigned char uart_recv_fifo_in = 0;
unsigned char uart_recv_fifo_out = 0;
pthread_mutex_t sem_handle_lock=PTHREAD_MUTEX_INITIALIZER;
static unsigned char gLcdRecvPaketLength = 0;
static unsigned char gLcdRecvPaketBuf[256];
//static eLedBoxComStat gLedBoxComStat;
tDcpHeader gDcpData={0};   // 记录dcp的过程数据
extern int g_checkSend;
unsigned char SrcDeviceNum = 0;
unsigned char recvDate = 1;//默认值是有数据的


unsigned char uart_read_char(unsigned char *ch)
{ 
    if(uart_recv_fifo_out == uart_recv_fifo_in)
    {
		return 0;
    }
    *ch = uart_recv_buf[uart_recv_fifo_out++];
    if(uart_recv_fifo_out == UART_BUF_LEN)
    {
		uart_recv_fifo_out = 0;
		
    }
    return 1;
}

unsigned char LcdDataGetPacket(void)
{
	unsigned char temp;
	while(uart_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			if(gLcdRecvPaketLength >= 45)
			{
				/*for(int i=0;i<gLcdRecvPaketLength;i++)
				{
					printf("%02x ",gLcdRecvPaketBuf[i]);
				}
				printf("\n");*/
				return 1;
			}
			gLcdRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gLcdRecvPaketBuf[gLcdRecvPaketLength] = temp;
			if( ++gLcdRecvPaketLength >= 255 )
			{
				gLcdRecvPaketLength = 0;
			}
		}
	}
	return 0;
}

unsigned char VerifyPaket(unsigned char*paketBuf, unsigned char length)
{
    unsigned char recv_flag=0;
    unsigned char checksum=0;
    unsigned char src_length=0;//逆转码之前的长度
    unsigned char dst_length=0;//逆转码以后的长度
//    static u8 length_of_send = 0;
    	    
           //执行逆转码 并计算校验和
	   for(src_length=0; src_length<length; src_length++)
	    {
			//拷贝数据
			paketBuf[dst_length]=paketBuf[src_length];
			//判断刚刚拷贝的是否是转码符
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_TAB 并跳过下一个字符
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TMS_PACKET_TAB
					src_length++;//并跳过下一个字符
				}
				//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_DLE 并跳过下一个字符
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TMS_PACKET_DLE
					src_length++;//并跳过下一个字符
				}
				else
				{
					//print_line("tms 7f error");
					//数据错误----7f没有被正确转码
					dst_length=0;//清除转码以后的长度
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //减掉校验和的长度
		}
		else
		{
			recv_flag = 0; //正常
			//recv_flag = 44;//测试用的 不校验长度
		}
		
		return recv_flag;
}
//extern NVRT_TO_VVS_CONTENT_T g_nvrtToVvsContent;

void TrainDataDcpAnalyse(unsigned char *buf)
{
	tDcpHeader *DcpData;
	DcpData = (tDcpHeader *)buf;
	//u8 mode, volume;

	memcpy((unsigned char *)&gDcpData , buf , sizeof(tDcpHeader)); //保存中央控制器过程数据

	VvsSetContent(&gDcpData.data_area);
	gLcdRecvPaketLength = 0;
	//printf("code.beginStation %02x\n",gDcpData.data_area.TMS);
	/*printf("code.endStation %02x\n",gDcpData.data_area.code.endStation);
	printf("code.currentStation %02x\n",gDcpData.data_area.code.currentStation);
	printf("code.nextStation %02x\n",gDcpData.data_area.code.nextStation);
	printf("code.passStationBro %02x\n",gDcpData.data_area.code.passStationBro);
	printf("********************\n");
	printf("code.beginStation %02x\n",g_nvrtToVvsContent.code.beginStation);
	printf("code.endStation %02x\n",g_nvrtToVvsContent.code.endStation);
	printf("code.currentStation %02x\n",g_nvrtToVvsContent.code.currentStation);
	printf("code.nextStation %02x\n",g_nvrtToVvsContent.code.nextStation);
	printf("code.passStationBro %02x\n",g_nvrtToVvsContent.code.passStationBro);*/
	/*gPaPriority.tick_485 = gSystemTick;
	

	
	gPaPriority.occ = piscData->data_area.broadcast_priority.occ;
	gPaPriority.manual = piscData->data_area.broadcast_priority.manual;
	gPaPriority.dva = piscData->data_area.broadcast_priority.dva;
	gPaPriority.lcd_media = piscData->data_area.broadcast_priority.media;
	gPaPriority.eme = piscData->data_area.broadcast_priority.special;
	gPaPriority.close_door = piscData->data_area.broadcast_priority.door;
	gPaPriority.type = (tPaType)piscData->data_area.broadcast_signal.broadcast_type;

	mode = piscData->data_area.panel_volume.mode;
	volume = piscData->data_area.panel_volume.value;
	SaveVolume(mode, volume);
*/
}


void LcdDataProc(void)
{
	tFrameHeader *frameHeader;
	tDcpHeader *DcpData;	
	unsigned char len;

	// 1. 接收数据处理
	if (LcdDataGetPacket()) 
	{
		len = VerifyPaket( gLcdRecvPaketBuf, gLcdRecvPaketLength );
		//printf("len = %d\n",len);
		if ( len == 44 ) 
		{
			frameHeader = (tFrameHeader *)gLcdRecvPaketBuf;
			//printf("-------------packet_type----%02x--------------\n",frameHeader->cmd.packet_type);
			switch (frameHeader->cmd.packet_type) 
			{

				case TOKEN_PACKET:		
					//是否为过程数据
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB && frameHeader->dest_eqnum.eq_type == 0x09) 
					{
						SrcDeviceNum = frameHeader->src_eqnum.eq_num;//原设备号

						/******************************************/
						for(int i=0;i<=len;i++)
						{
							printf("%02x ",gLcdRecvPaketBuf[i]);
						}
						 printf("\n");
						/*****************************************/
						g_checkSend = 1; 
						DcpData = (tDcpHeader *)gLcdRecvPaketBuf;
						TrainDataDcpAnalyse( gLcdRecvPaketBuf );	
					}
					break;
				default :
					break;
			}
		}
		gLcdRecvPaketLength = 0;
	}
}

//***********************************************************************************************************************
//函数作用:通信底层接收函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void Uart_read_char()
{
	unsigned char tmp;
	//pthread_mutex_lock(&uart_thread_mutex_t);
	UartRead( uartFd, &tmp, 1 );
	uart_recv_buf[uart_recv_fifo_in++] = tmp;
	if(uart_recv_fifo_in >= UART_BUF_LEN)
	{
	 	uart_recv_fifo_in = 0;
		/*
		for(int i=0;i<UART_BUF_LEN;i++)
		{
			printf("%02x ",uart_recv_buf[i]);
			
		}
		*/
		tcflush(uartFd,TCIOFLUSH);
		//printf("\n\n");
		
	}
//	pthread_mutex_unlock(&uart_thread_mutex_t);
}

//***********************************************************************************************************************
//*函数作用: 负责串口数据接收，对数据进行校验，向处理任务发送消息
//*参数说明:
//*注意事项:
//*返回说明:
//***********************************************************************************************************************
//unsigned char selectRecv = 0;
void *lcd_uart_collect_data_thread(void *param)
{	
	printf("----------------collect data begin----------------------\n");
	int ret = 0;
       static unsigned char cnt=0;
	while(1)
	{
		ret =  SelectReadSocket( uartFd, 1000 );//select阻塞等待可读tcmstonvrt.fd
       // printf( "[Select Receiving.......]%d\n", ret );       
        if( ret > 0 )
        {
			Uart_read_char();
			recvDate = 1;
                    cnt = 0;
        }
		else if( ret == 0)
		{
		       cnt++;
                     //if(cnt>5)
			{
			    cnt =5;
			    recvDate = 0;//收不到数据
                     }
		}
	}
	return 0;
}

void *lcd_uart_proc_data_thread(void *parm)
{
	while(1)
	{
		LcdDataProc();
		//usleep(50);
	}
	return 0;
}

//***********************************************************************************************************************
//函数作用:外部扩展处理模块装载
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void Lcd_Serial_process_Install()
{
	static pthread_t lcd_uart_collect_data_thread_id; 
	static pthread_t Lcd_uart_proc_data_thread_id;
	//创建线程
	pthread_create(&lcd_uart_collect_data_thread_id, NULL, lcd_uart_collect_data_thread, NULL); 	//uart数据采集线程创建
	pthread_create(&Lcd_uart_proc_data_thread_id, NULL, lcd_uart_proc_data_thread, NULL); 	//uart数据采集线程创建
	return;		
}


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
tDcpHeader gDcpData={0};   // ��¼dcp�Ĺ�������
extern int g_checkSend;
unsigned char SrcDeviceNum = 0;
unsigned char recvDate = 1;//Ĭ��ֵ�������ݵ�


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
			//�жϳ��� �ж��Ƿ�������
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
		//�����ַ���ֱ���ͻ�����
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
    unsigned char src_length=0;//��ת��֮ǰ�ĳ���
    unsigned char dst_length=0;//��ת���Ժ�ĳ���
//    static u8 length_of_send = 0;
    	    
           //ִ����ת�� ������У���
	   for(src_length=0; src_length<length; src_length++)
	    {
			//��������
			paketBuf[dst_length]=paketBuf[src_length];
			//�жϸոտ������Ƿ���ת���
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//�ж���һ���ַ��Ƿ��Ƿָ����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB ��������һ���ַ�
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB
					src_length++;//��������һ���ַ�
				}
				//�ж���һ���ַ��Ƿ���ת�����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE ��������һ���ַ�
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE
					src_length++;//��������һ���ַ�
				}
				else
				{
					//print_line("tms 7f error");
					//���ݴ���----7fû�б���ȷת��
					dst_length=0;//���ת���Ժ�ĳ���
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //����У��͵ĳ���
		}
		else
		{
			recv_flag = 0; //����
			//recv_flag = 44;//�����õ� ��У�鳤��
		}
		
		return recv_flag;
}
//extern NVRT_TO_VVS_CONTENT_T g_nvrtToVvsContent;

void TrainDataDcpAnalyse(unsigned char *buf)
{
	tDcpHeader *DcpData;
	DcpData = (tDcpHeader *)buf;
	//u8 mode, volume;

	memcpy((unsigned char *)&gDcpData , buf , sizeof(tDcpHeader)); //���������������������

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

	// 1. �������ݴ���
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
					//�Ƿ�Ϊ��������
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB && frameHeader->dest_eqnum.eq_type == 0x09) 
					{
						SrcDeviceNum = frameHeader->src_eqnum.eq_num;//ԭ�豸��

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
//��������:ͨ�ŵײ���պ���
//����˵��:
//ע������:
//����˵��:
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
//*��������: ���𴮿����ݽ��գ������ݽ���У�飬������������Ϣ
//*����˵��:
//*ע������:
//*����˵��:
//***********************************************************************************************************************
//unsigned char selectRecv = 0;
void *lcd_uart_collect_data_thread(void *param)
{	
	printf("----------------collect data begin----------------------\n");
	int ret = 0;
       static unsigned char cnt=0;
	while(1)
	{
		ret =  SelectReadSocket( uartFd, 1000 );//select�����ȴ��ɶ�tcmstonvrt.fd
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
			    recvDate = 0;//�ղ�������
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
//��������:�ⲿ��չ����ģ��װ��
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void Lcd_Serial_process_Install()
{
	static pthread_t lcd_uart_collect_data_thread_id; 
	static pthread_t Lcd_uart_proc_data_thread_id;
	//�����߳�
	pthread_create(&lcd_uart_collect_data_thread_id, NULL, lcd_uart_collect_data_thread, NULL); 	//uart���ݲɼ��̴߳���
	pthread_create(&Lcd_uart_proc_data_thread_id, NULL, lcd_uart_proc_data_thread, NULL); 	//uart���ݲɼ��̴߳���
	return;		
}


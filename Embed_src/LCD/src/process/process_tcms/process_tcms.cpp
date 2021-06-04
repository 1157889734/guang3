#include "include.h"

int uartFd;
LCD_TO_DCP_CONNECT_T g_LcdtoDcpContent;//���͸�dcp������
extern int g_sendLCDToDcpPackFlag;//����lcd����dcp�ı�־
int g_checkSend = 0;//��Ѳ���ź�����ʱ��ŷ���

//ת��
static int TransCode(LCD_TO_DCP_T *pLcdToDcp,unsigned char *transcode)
{
   int len = 0;
   memcpy(transcode,(unsigned char*)pLcdToDcp+1,sizeof(LCD_TO_DCP_T)-6);
   for(int j = 0; j < (int)sizeof(LCD_TO_DCP_T)-6; j++)//��ѯ������û��7e
   {
        if( transcode[j] == 0x7e)//��һ���ֽ�7f
        {   
            for( int k = 0; k < (int)sizeof(LCD_TO_DCP_T)-6-j; k++)
            {
                transcode[sizeof(LCD_TO_DCP_T)-6+1-k+len-1] = transcode[sizeof(LCD_TO_DCP_T)-6-k+len-1];
            }
            transcode[j+1] = 0x80;  
            len++;
         }         
        else if(transcode[j] == 0x7f)//�ڶ����ֽ�81
        {
            for( int m = 0; m < (int)sizeof(LCD_TO_DCP_T)-6-j; m++)
            {
                transcode[sizeof(LCD_TO_DCP_T)-6+1-m-1+len] = transcode[sizeof(LCD_TO_DCP_T)-6-m-1+len];
            }
            transcode[j+1] = 0x81;  
            len++;
        }
    }
    for( int k = 0; k < (int)sizeof(LCD_TO_DCP_T)-6+len+1; k++)
    {
        transcode[sizeof(LCD_TO_DCP_T)-6+1-k+len] = transcode[sizeof(LCD_TO_DCP_T)-6-k+len];
    }
    transcode[0] = 0x7e;
    transcode[(int)sizeof(LCD_TO_DCP_T)-6+len+1] = 0x7e;
    return len;
}

//��ת��
static void ReverseTransCode(DCP_TO_LCD_T *pTcmsToNvrt)
{
   for(int j = 0; j < pTcmsToNvrt->len; j++)
   {
        if( pTcmsToNvrt->content[j] == 0x7f)//��һ���ֽ�7f
        {
            if( pTcmsToNvrt->content[j+1] == 0x80 )//�ڶ����ֽ�80
            {
                pTcmsToNvrt->content[j] = 0x7e;  
                for( int k = j; k < pTcmsToNvrt->len-2; k++)
                {
                    pTcmsToNvrt->content[k+1] = pTcmsToNvrt->content[k+2];
                }
                pTcmsToNvrt->len--;
            }
            else if(pTcmsToNvrt->content[j+1] == 0x81)//�ڶ����ֽ�81
            {
                pTcmsToNvrt->content[j] = 0x7f;
                 for( int m = j; m < pTcmsToNvrt->len-2; m++)
                {
                    pTcmsToNvrt->content[m+1] = pTcmsToNvrt->content[m+2];
                }
                pTcmsToNvrt->len--;
            }             
        }
   }
}

static void LcdMsgToDcp( LCD_TO_DCP_T *pLcdToDcp )
{
    int len = 0;
	static int sendTwo = 0;
    static unsigned char transcode[30] = {0};
	//usleep(50);
	if( g_sendLCDToDcpPackFlag == 1 )//�ж��Ƿ���Է���
	{
		//usleep(150000);//��֤�����������
		memcpy(&pLcdToDcp->content,&g_LcdtoDcpContent,sizeof(LCD_TO_DCP_CONNECT_T));//���������
		pLcdToDcp->checksum = CheckSum((unsigned char*)pLcdToDcp+1, sizeof(LCD_TO_DCP_T)-4-1-2);    //У��λ 
	    len = TransCode(pLcdToDcp,transcode);//ת��	
		sendTwo = 1;
	}  
	UartWrite( pLcdToDcp->fd, transcode, (int)sizeof(LCD_TO_DCP_T)+len-4 ); 
	if( sendTwo != 0 )
	{
		sendTwo++;
	}
	if((sendTwo == 6 || sendTwo == 7 || sendTwo == 8))
	{
		g_LcdtoDcpContent.trigger.leaveBroadcast = 0;//Ԥ��վȡ������
		g_LcdtoDcpContent.trigger.stopBroadcast= 0;//ֹͣλȡ������
		g_LcdtoDcpContent.trigger.stopUrgentBroadcast = 0;//�����㲥����
		g_LcdtoDcpContent.code.beginStation = 0;
		g_LcdtoDcpContent.code.endStation = 0;
		g_LcdtoDcpContent.code.nextStation = 0;//��һվ����
		//g_LcdtoDcpContent.lineMode.moniBroadcast = 0;//������0
	}
	else if(sendTwo >= 9)
	{
		sendTwo=0;
		memcpy(&pLcdToDcp->content,&g_LcdtoDcpContent,sizeof(LCD_TO_DCP_CONNECT_T));//���������
		pLcdToDcp->checksum = CheckSum((unsigned char*)pLcdToDcp+1, sizeof(LCD_TO_DCP_T)-4-1-2);    //У��λ 
	    len = TransCode(pLcdToDcp,transcode);//ת��	
	}	
}

static void Init_SendPackToDcp(LCD_TO_DCP_T *LcdToDcp)
{
    LcdToDcp->start = 0x7e;
    LcdToDcp->destNetworkNum = 0x00;
    LcdToDcp->destDeviceNum = 0x12;
    LcdToDcp->srcNetworkNum = 0x00;
    LcdToDcp->srcDeviceNum = 0x1a;
    LcdToDcp->CMD = 0x00;
    LcdToDcp->len = 12;
    memset(&LcdToDcp->content,0,sizeof(LCD_TO_DCP_CONNECT_T));
    LcdToDcp->checksum = 0x00;
    LcdToDcp->stop = 0x7e;

    g_LcdtoDcpContent.trigger.monitorBroadcast = 0;//�����㲥����
	g_LcdtoDcpContent.code.urgentBroCode = 1;//�����㲥����
    g_LcdtoDcpContent.code.beginStation = 1;//��ʼվ����
    g_LcdtoDcpContent.code.nextStation= 1;//��һվ����
    g_LcdtoDcpContent.code.endStation= 30;//�յ�վ����
    g_LcdtoDcpContent.workMode.closeATC = 0;//���ر�atc
    
    //����ģʽ
    g_LcdtoDcpContent.workMode.workmode = 0;//�Զ�ģʽ
    g_sendLCDToDcpPackFlag = 1;
}

static void UnInit_SendPackToDcp()
{
    g_LcdtoDcpContent.trigger.arivedBroadcast = 0;////��վ�㲥����
    g_LcdtoDcpContent.trigger.gusetroomBroadcast = 0;//���ҹ㲥����
    g_LcdtoDcpContent.trigger.monitorBroadcast = 0;//��������
    g_LcdtoDcpContent.trigger.muteBroadcast = 0;//��������
   	g_sendLCDToDcpPackFlag =0;
	g_checkSend = 0;
}

static void *pocess_recv_Dcp_thread(void *args)
{
    LCD_TO_DCP_T LcdToDcp;
    memcpy( &LcdToDcp.fd, args, sizeof(LcdToDcp.fd) );
    Init_SendPackToDcp(&LcdToDcp);//LCD���ݰ���ʼ��
    while(1)
    {
        usleep(50);
         if( 1 == g_checkSend )//��Ѳ���ź�����ʱ��ŷ���
         {
            LcdMsgToDcp(&LcdToDcp);//����һ��LCD���ݰ�
            UnInit_SendPackToDcp();//ע������
         }            
    }
    return NULL;    
}

void Start_process_tcms_thread()
{
	int *ptr;
	static pthread_t process_recv_Dcp_thread_id;
	uartFd = UartOpenDev( (char *)TCMS_UART_DEV );

	if( uartFd > 0 )
	{//������ 9600 ����λ8 ֹͣλ1 
		UartSetParam( uartFd, SERIAL_BAUDRATE_9600, SERIAL_DATABITS_8,SERIAL_STOPBITS_1, SERIAL_PARITY_NONE );
		ptr = (int *)malloc( sizeof(int) );
		if( NULL != ptr )
		{
			*ptr = uartFd;
            pthread_create(&process_recv_Dcp_thread_id,NULL,pocess_recv_Dcp_thread,ptr);
		}
	}
}



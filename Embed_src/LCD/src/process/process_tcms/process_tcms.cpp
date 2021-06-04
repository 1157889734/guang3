#include "include.h"

int uartFd;
LCD_TO_DCP_CONNECT_T g_LcdtoDcpContent;//发送给dcp的数据
extern int g_sendLCDToDcpPackFlag;//发送lcd包到dcp的标志
int g_checkSend = 0;//有巡检信号来的时候才发送

//转码
static int TransCode(LCD_TO_DCP_T *pLcdToDcp,unsigned char *transcode)
{
   int len = 0;
   memcpy(transcode,(unsigned char*)pLcdToDcp+1,sizeof(LCD_TO_DCP_T)-6);
   for(int j = 0; j < (int)sizeof(LCD_TO_DCP_T)-6; j++)//轮询查找有没有7e
   {
        if( transcode[j] == 0x7e)//第一个字节7f
        {   
            for( int k = 0; k < (int)sizeof(LCD_TO_DCP_T)-6-j; k++)
            {
                transcode[sizeof(LCD_TO_DCP_T)-6+1-k+len-1] = transcode[sizeof(LCD_TO_DCP_T)-6-k+len-1];
            }
            transcode[j+1] = 0x80;  
            len++;
         }         
        else if(transcode[j] == 0x7f)//第二个字节81
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

//逆转码
static void ReverseTransCode(DCP_TO_LCD_T *pTcmsToNvrt)
{
   for(int j = 0; j < pTcmsToNvrt->len; j++)
   {
        if( pTcmsToNvrt->content[j] == 0x7f)//第一个字节7f
        {
            if( pTcmsToNvrt->content[j+1] == 0x80 )//第二个字节80
            {
                pTcmsToNvrt->content[j] = 0x7e;  
                for( int k = j; k < pTcmsToNvrt->len-2; k++)
                {
                    pTcmsToNvrt->content[k+1] = pTcmsToNvrt->content[k+2];
                }
                pTcmsToNvrt->len--;
            }
            else if(pTcmsToNvrt->content[j+1] == 0x81)//第二个字节81
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
	if( g_sendLCDToDcpPackFlag == 1 )//判断是否可以发送
	{
		//usleep(150000);//保证数据内容组包
		memcpy(&pLcdToDcp->content,&g_LcdtoDcpContent,sizeof(LCD_TO_DCP_CONNECT_T));//把内容组包
		pLcdToDcp->checksum = CheckSum((unsigned char*)pLcdToDcp+1, sizeof(LCD_TO_DCP_T)-4-1-2);    //校验位 
	    len = TransCode(pLcdToDcp,transcode);//转码	
		sendTwo = 1;
	}  
	UartWrite( pLcdToDcp->fd, transcode, (int)sizeof(LCD_TO_DCP_T)+len-4 ); 
	if( sendTwo != 0 )
	{
		sendTwo++;
	}
	if((sendTwo == 6 || sendTwo == 7 || sendTwo == 8))
	{
		g_LcdtoDcpContent.trigger.leaveBroadcast = 0;//预报站取消触发
		g_LcdtoDcpContent.trigger.stopBroadcast= 0;//停止位取消触发
		g_LcdtoDcpContent.trigger.stopUrgentBroadcast = 0;//紧急广播触发
		g_LcdtoDcpContent.code.beginStation = 0;
		g_LcdtoDcpContent.code.endStation = 0;
		g_LcdtoDcpContent.code.nextStation = 0;//下一站代码
		//g_LcdtoDcpContent.lineMode.moniBroadcast = 0;//音量清0
	}
	else if(sendTwo >= 9)
	{
		sendTwo=0;
		memcpy(&pLcdToDcp->content,&g_LcdtoDcpContent,sizeof(LCD_TO_DCP_CONNECT_T));//把内容组包
		pLcdToDcp->checksum = CheckSum((unsigned char*)pLcdToDcp+1, sizeof(LCD_TO_DCP_T)-4-1-2);    //校验位 
	    len = TransCode(pLcdToDcp,transcode);//转码	
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

    g_LcdtoDcpContent.trigger.monitorBroadcast = 0;//监听广播触发
	g_LcdtoDcpContent.code.urgentBroCode = 1;//紧急广播代码
    g_LcdtoDcpContent.code.beginStation = 1;//起始站代码
    g_LcdtoDcpContent.code.nextStation= 1;//下一站代码
    g_LcdtoDcpContent.code.endStation= 30;//终点站代码
    g_LcdtoDcpContent.workMode.closeATC = 0;//不关闭atc
    
    //工作模式
    g_LcdtoDcpContent.workMode.workmode = 0;//自动模式
    g_sendLCDToDcpPackFlag = 1;
}

static void UnInit_SendPackToDcp()
{
    g_LcdtoDcpContent.trigger.arivedBroadcast = 0;////到站广播触发
    g_LcdtoDcpContent.trigger.gusetroomBroadcast = 0;//客室广播触发
    g_LcdtoDcpContent.trigger.monitorBroadcast = 0;//监听触发
    g_LcdtoDcpContent.trigger.muteBroadcast = 0;//静音触发
   	g_sendLCDToDcpPackFlag =0;
	g_checkSend = 0;
}

static void *pocess_recv_Dcp_thread(void *args)
{
    LCD_TO_DCP_T LcdToDcp;
    memcpy( &LcdToDcp.fd, args, sizeof(LcdToDcp.fd) );
    Init_SendPackToDcp(&LcdToDcp);//LCD数据包初始化
    while(1)
    {
        usleep(50);
         if( 1 == g_checkSend )//有巡检信号来的时候才发送
         {
            LcdMsgToDcp(&LcdToDcp);//发送一次LCD数据包
            UnInit_SendPackToDcp();//注销触发
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
	{//波特率 9600 数据位8 停止位1 
		UartSetParam( uartFd, SERIAL_BAUDRATE_9600, SERIAL_DATABITS_8,SERIAL_STOPBITS_1, SERIAL_PARITY_NONE );
		ptr = (int *)malloc( sizeof(int) );
		if( NULL != ptr )
		{
			*ptr = uartFd;
            pthread_create(&process_recv_Dcp_thread_id,NULL,pocess_recv_Dcp_thread,ptr);
		}
	}
}



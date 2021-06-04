#include "include.h"

NVRT_TO_VVS_CONTENT_T g_nvrtToVvsContent;
static int g_isContentAvailable = 0;
static pthread_mutex_t g_vvsMutex = PTHREAD_MUTEX_INITIALIZER;

void VvsSetContent( DCP_TO_LCD_CONTENT_T *pTcmsToNvrtContent )
{
	pthread_mutex_lock( &g_vvsMutex );
	if( 0 == g_isContentAvailable )
	{
		g_isContentAvailable = 1;
		//g_nvrtToVvsContent.start 	= VVS_START;	
		//g_nvrtToVvsContent.len 		= VVS_LEN;	
		//g_nvrtToVvsContent.cmd 		= VVS_CMD;	
		//g_nvrtToVvsContent.stop 	= VVS_STOP;
		g_isContentAvailable = 1;
	}
	g_nvrtToVvsContent.trigger 	= pTcmsToNvrtContent->trigger;
	g_nvrtToVvsContent.code		= pTcmsToNvrtContent->code;
	g_nvrtToVvsContent.info		= pTcmsToNvrtContent->info;
	g_nvrtToVvsContent.city            =pTcmsToNvrtContent->city;
	g_nvrtToVvsContent.datatime    =pTcmsToNvrtContent->datatime;
	g_nvrtToVvsContent.priority       =pTcmsToNvrtContent->priority;
	g_nvrtToVvsContent.source        =pTcmsToNvrtContent->source;
	g_nvrtToVvsContent.cab            =pTcmsToNvrtContent->cab;
	g_nvrtToVvsContent.linemode		=pTcmsToNvrtContent->linemode;
	g_nvrtToVvsContent.communfault  =pTcmsToNvrtContent->communfault;
	g_nvrtToVvsContent.TMS              =pTcmsToNvrtContent->TMS;
	g_nvrtToVvsContent.volume          =pTcmsToNvrtContent->volume;
	g_nvrtToVvsContent.time             = pTcmsToNvrtContent->time;
	g_nvrtToVvsContent.testStat        = pTcmsToNvrtContent->testStat;
	g_nvrtToVvsContent.errorProc	= pTcmsToNvrtContent->errorProc;
	//g_nvrtToVvsContent.volume	= 80;
	pthread_mutex_unlock( &g_vvsMutex );
}

static void VvsReconnect( NVRT_TO_VVS_T *pNvrToVss )
{
	int t_now, interval, ret;
	if( -1 == pNvrToVss->fd )
	{
		t_now = time( NULL );
		interval = t_now - pNvrToVss->t_reconnect;
		if( interval >= 5 )
		{
			SocketTcpConnectTimtout( &pNvrToVss->fd, VVS_IP, VVS_PORT, 3000 );
			pNvrToVss->t_reconnect = t_now;
		}
	}
}

static void VvsSendConnect( NVRT_TO_VVS_T *pNvrToVss )
{
	int ret;
	char buf[10*1024];
	if( pNvrToVss->fd > 0 && 1 == g_isContentAvailable )
	{
		pthread_mutex_lock( &g_vvsMutex );
		tcp_socket_send( &pNvrToVss->fd, (unsigned char *)&g_nvrtToVvsContent, 
								sizeof(g_nvrtToVvsContent) );
								
		pthread_mutex_unlock( &g_vvsMutex );
		if( pNvrToVss->fd > 0 )
		{
			read( pNvrToVss->fd, buf, sizeof(buf) ); // 把AA AA 读出来
		}
	}
}

static void *process_vvs_thread( void *args )
{
	int ret;
	NVRT_TO_VVS_T nvrToVss;

	memset( &nvrToVss, 0x00, sizeof(nvrToVss) );
	nvrToVss.fd 	= -1;
	nvrToVss.t_reconnect 	= -100;

	while( 1 )
	{
		VvsReconnect( &nvrToVss );//vvs从新连接
		VvsSendConnect( &nvrToVss );//发送内容
		sleep( 1 );
	}

	return NULL;
}

void Start_process_vvs_thread()
{
	pthread_t process_vvs_thread_id;
	pthread_create( &process_vvs_thread_id, NULL, process_vvs_thread, NULL );
}


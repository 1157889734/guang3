#include "include.h"

//socket初始化 tcp
int socket_tcp_connect(int *socket_handler, char *SERVER_HOST, int SERVER_PORT)
{
	struct sockaddr_in server_addr;
	if ((*socket_handler = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		*socket_handler = -1;
		return 0;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
	bzero(&(server_addr.sin_zero),8);
	struct timeval tval;
	tval.tv_sec 	= 0;
	tval.tv_usec 	= 1000*100;
 	setsockopt( *socket_handler, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tval, sizeof(tval) );
	if ((connect(*socket_handler, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))) == -1)
	{
		close(*socket_handler);
		*socket_handler = -1;
		return 0;
	}
	return 1;
}

void tcp_socket_send(int *socket_handler, unsigned char *data, int length)
{
	if (*socket_handler == -1) return;
	if (send(*socket_handler, data, length, 0)==-1)
	{
		close(*socket_handler);
		*socket_handler = -1;
	}
}

//socket初始化 udp
void udp_socket_handler_great(int *socket_handler, short send_port, int time_out_us)
{
	int n;
	int socklen = 0;
	struct timeval timeout;
	struct sockaddr_in localaddr_send;
	socklen = sizeof(struct sockaddr_in);
	if ((*socket_handler = socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		//printf("socket creating err in udptalk left\n");
		exit(1);
	}
	if (time_out_us)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = time_out_us;
		setsockopt(*socket_handler,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(struct timeval));
	}
	n = 1;
	setsockopt(*socket_handler,SOL_SOCKET,SO_BROADCAST,&n,sizeof(n));
	memset(&localaddr_send,0,socklen);
	localaddr_send.sin_family = AF_INET;
	localaddr_send.sin_port = htons(send_port);
	if (bind(*socket_handler, (struct sockaddr *)&localaddr_send, socklen)<0)
	{
		//printf("bind local address err in udptalk left!\n");
		exit(2);
	}
	return;
}

//udp发送
void udp_socket_send(int socket_handler, char* remote_ip, short remote_port, unsigned char *parm, int length)
{
	int socklen;
	struct sockaddr_in remoteaddr;
	socklen = sizeof(struct sockaddr_in);
	memset(&remoteaddr,0,socklen);
	remoteaddr.sin_family=AF_INET;
	remoteaddr.sin_port=htons(remote_port);

	if (inet_pton(AF_INET, remote_ip, &remoteaddr.sin_addr)<=0)
	{
		return;
	}
	sendto(socket_handler, parm, length, 0, (struct sockaddr *)&remoteaddr, socklen);
	return;
}

//程序去select的时候，如果没有数据输入，程序会一直等待，直到有数据为止，也就是程序中无需循环和sleep
int SelectReadSocket(int socket, int maxMsec)
{
	int nRet = -1;
	fd_set fd;
	FD_ZERO( &fd );//清空fd与所有文件句柄的联系
	FD_SET( socket, &fd );//建立文件句柄socket与fd的联系

	struct timeval timeout;
	timeout.tv_sec	= maxMsec / 1000;
	timeout.tv_usec	= (maxMsec % 1000) * 1000;

/*
* socket +1 件事的文件句柄数 视进程中打开的文件数而定,一般设为你要监视各文件
* 中的最大文件号加一
* fd  监视的可读文件句柄集合
* NULL 监视的可写文件句柄集合
* NULL 监视的异常文件句柄集合
* timeout 本次select()的超时结束时间
* 返回值：准备就绪的描述符数，若超时则返回0，若出错则返回-1
*/
	nRet = select( socket + 1, &fd, NULL, NULL, &timeout );
	if ( nRet > 0 )
	{
	//检查fd联系的文件句柄socket是否可读写，当>0表示可读写
		if ( FD_ISSET(socket, &fd) > 0 ) nRet = 1;
		else nRet = -1;
	}
	return nRet;
}

/*
* fn: 创建一个TCP非阻塞的连接, connect的超时为 timeoutMs
* pTcpSocket: out, 创建成功后的socket
* ip: 服务器的IP
* port: 服务器的listen 端口
* timeoutMs: 连接的超时时间,ms
*/
int SocketTcpConnectTimtout( int *pTcpSocket, char *ip, unsigned short port, int timeoutMs  )
{
	int nRet = -1;	
	int nSocket;
	int bReUseAddr = 1;
	struct timeval tval;

	tval.tv_sec 	= timeoutMs / 1000;
	tval.tv_usec 	= timeoutMs % 1000;
	
	if( NULL == pTcpSocket || NULL == ip ) return -1;
	
	*pTcpSocket = -1;
	nSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( nSocket == -1 ) return nRet;
	
	nRet = setsockopt( nSocket, SOL_SOCKET, SO_REUSEADDR,
			(const char*)&bReUseAddr, sizeof(bReUseAddr) );	
	if( nRet == -1 ) return nRet;
	
	nRet = setsockopt( nSocket, SOL_SOCKET, SO_SNDTIMEO,
			(const char*)&tval, sizeof(tval) );	 	
	
	struct sockaddr_in my_addr = { 0 };
	my_addr.sin_family		= AF_INET;
	my_addr.sin_port		= htons( port );
	my_addr.sin_addr.s_addr	= inet_addr( ip );

	nRet = connect(nSocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in));	
	if ( nRet == -1 )
	{
		close( nSocket );
	}
	else
	{

		unsigned long ul = 1;
		nRet = ioctl( nSocket, FIONBIO, &ul );  //设置为非阻塞模式	
		if( 0 != nRet )
		{
			close( nSocket );
			return nRet;
		}
		/*
 		int sendBufSize = 64*1024;
 		if( nRet == 0)
 		{
			nRet = setsockopt( nSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sendBufSize, sizeof(sendBufSize) );
			if( 0 != nRet )
			{
				close( nSocket );
				return nRet;
			}
		}
		*/
		*pTcpSocket = nSocket;
	}
	
	return nRet;
}

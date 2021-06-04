#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

//下面定义的几个宏是TFTP协议的操作码，参考协议

#define OP_RRQ        	1 //读请求
#define OP_WRQ  	2 //写请求
#define OP_DATA    	3 //数据包
#define OP_ACK       	4 //确认包
#define OP_ERROR    	5 //错误信息

//TFTP错误信息编码

static char *error_string[] = {
	(char*)"Not defined error!",                    		// 0

	(char*)"File not found!",                        		// 1

	(char*)"Access denied!",                        	// 2

	(char*)"Disk full or allocation exceeded!",    	// 3

	(char*)"Illegal TFTP operation!",                	// 4

	(char*)"Unknown transfer ID!",               		// 5

	(char*)"File already exists!",                    	// 6

	(char*)"No such user!"                            	// 7
};

#define MODE_NETASCII 	"netascii"
#define MODE_OCTET    	"octet"
#define MODE_MAIL     	"mail"

#define TFTP_SERVER_PORT		69	   // 69 is the port used for TFTP protocol initial transaction 

int check_timeout( int socket, int maxMsec )
{
	int nRet = -1;
	fd_set fd;
	FD_ZERO( &fd );
	FD_SET( socket, &fd );

	struct timeval timeout;
	timeout.tv_sec	= maxMsec / 1000;
	timeout.tv_usec	= (maxMsec % 1000) * 1000;

	nRet = select( socket + 1, &fd, NULL, NULL, &timeout );
	if ( nRet > 0 )
	{
		if ( FD_ISSET(socket, &fd) > 0 ) nRet = 1;
		else nRet = -1;
	}
	return nRet;
}

char* tftp_receive(char *server_ip, char *filename, int *file_size)
{
	char *file_buf = NULL; 
	
	int socketFD; //socket描述符

	struct sockaddr_in tftpServer, tftpClient; //自身和服务器的socket信息

	char buffer[516]; //网络通信用的buffer

	int addrlen;
	int lastPacket, currentPacket; //包计数

	unsigned short *s_ptr;
	char *c_ptr;
	int tmp;

	//创建socket
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == socketFD){
		printf("Can not create socket!\n");
		return NULL;
	}

	//客户端自身信息
	bzero(&tftpClient, sizeof (tftpClient));
	tftpClient.sin_family = AF_INET;
	tftpClient.sin_addr.s_addr = INADDR_ANY;
	tftpClient.sin_port = htons(0);

	//绑定socket到本机IP地址
	tmp = bind(socketFD, (struct sockaddr *)&tftpClient, sizeof (tftpClient));
	if (0 != tmp){
		printf("Can not bind socket!\n");
		close(socketFD);
		return NULL;
	}

	//服务器信息
	bzero(&tftpServer, sizeof (tftpServer));
	addrlen = sizeof (tftpServer);
	tftpServer.sin_family = AF_INET;
	tftpServer.sin_addr.s_addr = inet_addr(server_ip);
	tftpServer.sin_port = htons(TFTP_SERVER_PORT); //TFTP服务端口


	s_ptr = (unsigned short *)&buffer[0];
	*s_ptr = htons(OP_RRQ); //操作码

	c_ptr = &buffer[2];

	strcpy(&buffer[2], filename); //远程文件名

	c_ptr += strlen(filename);
	*c_ptr++ = 0;
	strcpy(c_ptr, MODE_OCTET); //传输模式

	c_ptr += 8;
	*c_ptr++ = 0;

	lastPacket = 0;

	int try_check_timeout_times = 0;
	int try_check_packetnum_times = 0;
	while (1)
	{
		//发送请求或者回应包到服务器
		tmp = sendto(socketFD, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftpServer, sizeof (tftpServer));
		if (-1 == tmp){
			printf("Can not send buffer!\n");
			close(socketFD);
			if (file_buf != NULL){
				free(file_buf);
				file_buf = NULL;
			}
			return NULL;
		}
		if(lastPacket == 0) usleep(100 * 1000);
		in_addr ad=tftpServer.sin_addr;
		//printf("send tftpServer: %d, %s, %d\n", ntohs(tftpServer.sin_port), inet_ntoa(ad), tftpServer.sin_family);
		//printf("send  len=%d : %d, %d, %d, %d\n", c_ptr - &buffer[0], buffer[0], buffer[1], buffer[2], buffer[3]);

		//接收数据超时重新发送ACK
		int check_timeout_return = check_timeout(socketFD, 30);
		//printf("check_timeout_return = %d\n", check_timeout_return);
		if (check_timeout_return <= 0)
		{
			//printf("receive tftp data timeout!\n");
			if(lastPacket == 0){
				close(socketFD);
				return NULL;
			}
			try_check_timeout_times++;
			if(try_check_timeout_times > 10){
				close(socketFD);
				if (file_buf != NULL){
					free(file_buf);
					file_buf = NULL;
				}
				return NULL;
			}
			continue;
		}
		try_check_timeout_times = 0;

		//从服务器获取数据包
		memset(buffer, 0x00, 516);
		tmp = recvfrom(socketFD, buffer, 516, 0, (struct sockaddr *)&tftpServer, (socklen_t*)(&addrlen));
		if (-1 == tmp){
			printf("Receive data error!\n");
			close(socketFD);
			if (file_buf != NULL){
				free(file_buf);
				file_buf = NULL;
			}
			return NULL;
		}
		else
		{
			in_addr ad=tftpServer.sin_addr;
			//printf("receive tftpServer: %d, %s, %d\n", ntohs(tftpServer.sin_port), inet_ntoa(ad), tftpServer.sin_family);
			//printf("receive  len=%d : %d, %d, %d, %d\n", tmp, buffer[0], buffer[1], buffer[2], buffer[3]);
			switch (ntohs(*s_ptr))
			{
			case OP_DATA://数据包
				currentPacket = ntohs(*(s_ptr+1));

				if ((lastPacket + 1) != currentPacket)//验证包的顺序
				{
					printf("ERROR: packet error! currentPacket=%d, lastPacket=%d\n", currentPacket, lastPacket);
					try_check_packetnum_times++;
					if(try_check_packetnum_times > 10)
					{
						close(socketFD);
						if (file_buf != NULL)
						{
							free(file_buf);
							file_buf = NULL;
						}
						return NULL;
					}
					*s_ptr = htons(OP_ACK);
					continue;
				}
				try_check_packetnum_times = 0;

				lastPacket = currentPacket;

				tmp -= 4;

#if 1
				//将数据写入file_buf
				*file_size += tmp;
				file_buf = (char*)realloc(file_buf, *file_size);
				if (file_buf == NULL)//内存分配失败
				{
					printf("file_buf = (char*)realloc(file_buf, file_size:%d)\n", *file_size);
					close(socketFD);
					return NULL;
				}
				memcpy(&file_buf[*file_size-tmp], &buffer[4], tmp);
				//printf("memcpy file_buf %d\n", *file_size);
#endif

				if (512 != tmp)//如果不等于512则传输完成(最后一包)
				{
					*s_ptr = htons(OP_ACK);
					*(s_ptr + 1) = htons(currentPacket);
					c_ptr = &buffer[4];
					if (-1 == sendto(socketFD, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftpServer, sizeof (tftpServer)))
					{
						printf("Can not send buffer!\n");
						close(socketFD);
						if (file_buf != NULL)
						{
							free(file_buf);
							file_buf = NULL;
						}
						return NULL;
					}
					close(socketFD);
					return file_buf;
				}

				//准备返回信息
				*s_ptr = htons(OP_ACK);
				*(s_ptr + 1) = htons(currentPacket);
				c_ptr = &buffer[4];

				break;

			case OP_ERROR://服务器返回错误信息
				printf("ERROR: %s\n", error_string[ntohs(*(s_ptr+1))]);//根据错误码，打印错误信息
				close(socketFD);
				if (file_buf != NULL)
				{
					free(file_buf);
					file_buf = NULL;
				}
				return NULL;

			default://发生其他未知错误
				printf("ERROR: Unknow error!\n");
				close(socketFD);
				if (file_buf != NULL)
				{
					free(file_buf);
					file_buf = NULL;
				}
				return NULL;
			}
		}
	}
	close(socketFD);
	if (file_buf != NULL)
	{
		free(file_buf);
		file_buf = NULL;
	}
	return NULL;
}

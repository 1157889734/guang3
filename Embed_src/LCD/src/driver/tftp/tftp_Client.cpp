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

//���涨��ļ�������TFTPЭ��Ĳ����룬�ο�Э��

#define OP_RRQ        	1 //������
#define OP_WRQ  	2 //д����
#define OP_DATA    	3 //���ݰ�
#define OP_ACK       	4 //ȷ�ϰ�
#define OP_ERROR    	5 //������Ϣ

//TFTP������Ϣ����

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
	
	int socketFD; //socket������

	struct sockaddr_in tftpServer, tftpClient; //����ͷ�������socket��Ϣ

	char buffer[516]; //����ͨ���õ�buffer

	int addrlen;
	int lastPacket, currentPacket; //������

	unsigned short *s_ptr;
	char *c_ptr;
	int tmp;

	//����socket
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == socketFD){
		printf("Can not create socket!\n");
		return NULL;
	}

	//�ͻ���������Ϣ
	bzero(&tftpClient, sizeof (tftpClient));
	tftpClient.sin_family = AF_INET;
	tftpClient.sin_addr.s_addr = INADDR_ANY;
	tftpClient.sin_port = htons(0);

	//��socket������IP��ַ
	tmp = bind(socketFD, (struct sockaddr *)&tftpClient, sizeof (tftpClient));
	if (0 != tmp){
		printf("Can not bind socket!\n");
		close(socketFD);
		return NULL;
	}

	//��������Ϣ
	bzero(&tftpServer, sizeof (tftpServer));
	addrlen = sizeof (tftpServer);
	tftpServer.sin_family = AF_INET;
	tftpServer.sin_addr.s_addr = inet_addr(server_ip);
	tftpServer.sin_port = htons(TFTP_SERVER_PORT); //TFTP����˿�


	s_ptr = (unsigned short *)&buffer[0];
	*s_ptr = htons(OP_RRQ); //������

	c_ptr = &buffer[2];

	strcpy(&buffer[2], filename); //Զ���ļ���

	c_ptr += strlen(filename);
	*c_ptr++ = 0;
	strcpy(c_ptr, MODE_OCTET); //����ģʽ

	c_ptr += 8;
	*c_ptr++ = 0;

	lastPacket = 0;

	int try_check_timeout_times = 0;
	int try_check_packetnum_times = 0;
	while (1)
	{
		//����������߻�Ӧ����������
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

		//�������ݳ�ʱ���·���ACK
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

		//�ӷ�������ȡ���ݰ�
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
			case OP_DATA://���ݰ�
				currentPacket = ntohs(*(s_ptr+1));

				if ((lastPacket + 1) != currentPacket)//��֤����˳��
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
				//������д��file_buf
				*file_size += tmp;
				file_buf = (char*)realloc(file_buf, *file_size);
				if (file_buf == NULL)//�ڴ����ʧ��
				{
					printf("file_buf = (char*)realloc(file_buf, file_size:%d)\n", *file_size);
					close(socketFD);
					return NULL;
				}
				memcpy(&file_buf[*file_size-tmp], &buffer[4], tmp);
				//printf("memcpy file_buf %d\n", *file_size);
#endif

				if (512 != tmp)//���������512�������(���һ��)
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

				//׼��������Ϣ
				*s_ptr = htons(OP_ACK);
				*(s_ptr + 1) = htons(currentPacket);
				c_ptr = &buffer[4];

				break;

			case OP_ERROR://���������ش�����Ϣ
				printf("ERROR: %s\n", error_string[ntohs(*(s_ptr+1))]);//���ݴ����룬��ӡ������Ϣ
				close(socketFD);
				if (file_buf != NULL)
				{
					free(file_buf);
					file_buf = NULL;
				}
				return NULL;

			default://��������δ֪����
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

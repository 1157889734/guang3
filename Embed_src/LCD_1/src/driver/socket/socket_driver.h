#ifndef __SOCKET_DRIVER_H__
#define __SOCKET_DRIVER_H__

int socket_tcp_connect(int *socket_handler, char *SERVER_HOST, int SERVER_PORT);
void tcp_socket_send(int *socket_handler, unsigned char *data, int length);
void udp_socket_handler_great(int *socket_handler, short send_port, int time_out_us);
void udp_socket_send(int socket_handler, char* remote_ip, short remote_port, unsigned char *parm, int length);
int SelectReadSocket(int socket, int maxMsec);
int SocketTcpConnectTimtout( int *pTcpSocket, char *ip, unsigned short port, int timeoutMs  );

#endif

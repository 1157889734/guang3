/*
*******************************************************************************
**  Copyright (c) 2011, 深圳市有限公司
**  All rights reserved.
**	
**  文件说明: 串口函数的封装            
**  创建日期: 2011.03.21
**
**  当前版本：1.0
**  作者：oscar
*******************************************************************************
*/
#include <termios.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "include.h"
int UartOpenDev( char *dev )
{
	return open( dev,O_RDWR|O_NDELAY );
}

/*
* fn: 对串口安全写
* 返回: 写入的字节
*/
int UartWrite( int fd, void *buf, int len )
{
	int ret = -1;
	char *ptr = (char*)buf;

	/*for(int i =0;i<len;i++)
	{
		printf("%02X ",ptr[i]);
	}
	printf("\n");
*/
	if(NULL != ptr && len>0 && len <MAX_SERIAL_PACK_SIZE && -1 != fd)
	{
		ret = write( fd, ptr, len );
		
		//printf("shawn debug write-------------%d\n ",len);

	}

	return ret;
}

/*
* fn: 串口读
* 返回: 读到的字节数
*/
int UartRead( int fd, void *buf, int len )
{
	int ret = -1;	
		
	if( NULL != buf && len>0 && len <MAX_SERIAL_PACK_SIZE && -1 != fd )
	{
		ret = read( fd, buf, len );		
		//puts((char*)buf);	
	}

	return ret;
}

void UartClose( int fd )
{	
	if( -1 != fd)
	{
		close(fd);
		g_RS232.handle = -1;
	}
}

/*
* 设置串口参数
*/
int UartSetParam( int handle, SERIAL_BAUDRATE_ST baudrate, 
								  SERIAL_DATABITS_ST databits, 
								  SERIAL_STOPBITS_ST stopbits,
								  SERIAL_PARITY_ST parity )
{
	int ret = -1;
	
	if(handle != -1 ) 
	{
		struct termios options;
		int tiocm;

		const tcflag_t BAUDRATE[] = { B110, B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200 };
		const tcflag_t DATABITS[] = { CS5, CS6, CS7, CS8 };
		const tcflag_t STOPBITS[] = { 0, CSTOPB };
		const tcflag_t PARITY[] = { 0, PARENB | PARODD, PARENB };		
		
		tcgetattr(handle,&options);
		
		cfsetispeed(&options,BAUDRATE[baudrate]);
		cfsetospeed(&options,BAUDRATE[baudrate]);
		options.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG|NOFLSH);
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= DATABITS[databits];
		options.c_cflag |= STOPBITS[stopbits];
		options.c_cflag &= ~(PARENB | PARODD);
		options.c_cflag |= PARITY[parity];
		options.c_cflag &= ~CRTSCTS;
		options.c_iflag &= ~(BRKINT|ICRNL|ISTRIP);
		options.c_iflag &= ~(IXON|IXOFF); // no XOR/XOFF flow control
		options.c_cc[VMIN]=1;
		options.c_cc[VTIME]=0;
		options.c_line=0;
		options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
		options.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);		

		tcsetattr(handle,TCSANOW,&options);

		ioctl(handle,TIOCMGET,&tiocm);
		
		tiocm |= TIOCM_DTR|TIOCM_RTS;
		tiocm &= ~(TIOCM_DSR|TIOCM_CTS);
		
		ioctl(handle,TIOCMSET,&tiocm);
	
		ret = 0;

		printf("FiSerialSetParam:Baudrate(%d) Databits(%d) Stopbits(%d) Parity(%d)!\r\n",					
					baudrate,databits,stopbits,parity);
	} 
	
	return ret;
}

/*
*	func	: 3G 模块 ttyUsb专用参数设置接口
*/
int UartSetParamEx( int handle, SERIAL_BAUDRATE_ST baudrate, 
								  SERIAL_DATABITS_ST databits, 
								  SERIAL_STOPBITS_ST stopbits,
								  SERIAL_PARITY_ST parity )
{
	int ret = -1;
	
	if(handle != -1 ) 
	{
		struct termios options;
		int tiocm;

		const tcflag_t BAUDRATE[] = { B110, B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200 };
		const tcflag_t DATABITS[] = { CS5, CS6, CS7, CS8 };
		const tcflag_t STOPBITS[] = { 0, CSTOPB };
//		const tcflag_t PARITY[] = { 0, PARENB | PARODD, PARENB };		
		
		tcgetattr(handle,&options);
		
		cfsetispeed(&options,BAUDRATE[baudrate]);
		cfsetospeed(&options,BAUDRATE[baudrate]);
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= DATABITS[databits];
		options.c_cflag |= STOPBITS[stopbits];
		options.c_line=0;

		options.c_iflag =  IGNBRK;
		options.c_lflag = 0;
		options.c_oflag = 0;
		options.c_cflag |= CLOCAL | CREAD;
		options.c_cc[VMIN] = 1;
		options.c_cc[VTIME] = 5;
		options.c_iflag &= ~(IXON|IXOFF|IXANY);
		options.c_cflag &= ~(PARENB | PARODD);
		options.c_cflag &= ~CSTOPB;
		tcsetattr(handle,TCSANOW,&options);
		ioctl(handle,TIOCMGET,&tiocm);
		
		tiocm |= TIOCM_DTR|TIOCM_RTS;
		tiocm &= ~(TIOCM_DSR|TIOCM_CTS);
		
		ioctl(handle,TIOCMSET,&tiocm);
	
		ret = 0;

		printf("FiSerialSetParam:Baudrate(%d) Databits(%d) Stopbits(%d) Parity(%d)!\r\n",					
					baudrate,databits,stopbits,parity);
	} 
	
	return ret;
}


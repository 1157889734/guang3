#ifndef __UART_H__
#define __UART_H__

#define MAX_SERIAL_PACK_SIZE	(64*1024)
#define MAX_SERIAL_PACK_SIZE_EX	(2048)
#define STRERROR_ERRNO	( strerror(errno) )      


#define SERIAL_DEV_RS232		"/dev/ttyAMA3"	// RS232端口

typedef enum _SerialType_
{
	SERIAL_TYPE_RS232 = 0,
	SERIAL_TYPE_RS485,
	SERIAL_TYPE_THREEG_AT,	// 3G AT指令集端口
	SERIAL_TYPE_GPS			// GPS端口
} SERIAL_TYPE_EN;

typedef enum _SerialBaudrate_
{
	SERIAL_BAUDRATE_110 = 0,
	SERIAL_BAUDRATE_300,
	SERIAL_BAUDRATE_600,
	SERIAL_BAUDRATE_1200,
	SERIAL_BAUDRATE_2400,
	SERIAL_BAUDRATE_4800,
	SERIAL_BAUDRATE_9600,
	SERIAL_BAUDRATE_19200,
	SERIAL_BAUDRATE_38400,
	SERIAL_BAUDRATE_57600,
	SERIAL_BAUDRATE_115200	
}SERIAL_BAUDRATE_ST;

typedef enum _SerialDatabits_
{
	SERIAL_DATABITS_5 = 0,
	SERIAL_DATABITS_6,
	SERIAL_DATABITS_7,
	SERIAL_DATABITS_8	
}SERIAL_DATABITS_ST;

typedef enum _SerialStopbits_
{
	SERIAL_STOPBITS_1 = 0,
	SERIAL_STOPBITS_2
}SERIAL_STOPBITS_ST;

typedef enum _SerialParity_
{
	SERIAL_PARITY_NONE = 0,
	SERIAL_PARITY_ODD,
	SERIAL_PARITY_EVEN
}SERIAL_PARITY_ST;

// 串口参数 rs232 rs485
typedef struct RsParam
{
	unsigned int	baudRate;			// 波特率
	unsigned char	dataBits;			// 数据位
	unsigned char	stopBits;			// 停止位
	unsigned char	parity;				// 校验位
}RS_PARAM;


typedef struct _SerialDev_
{
	int handle;
	int openCount;
}SERIAL_DEV_ST;

static SERIAL_DEV_ST g_RS232;
//static SERIAL_DEV_ST g_RS485;
//static SERIAL_DEV_ST g_RSTHREEG;
//static SERIAL_DEV_ST g_RSGPS;

const unsigned int baudRate[] = {
    110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};
const SERIAL_BAUDRATE_ST serialBuadRate[]=
{
	SERIAL_BAUDRATE_110 ,
	SERIAL_BAUDRATE_300,
	SERIAL_BAUDRATE_600,
	SERIAL_BAUDRATE_1200,
	SERIAL_BAUDRATE_2400,
	SERIAL_BAUDRATE_4800,
	SERIAL_BAUDRATE_9600,
	SERIAL_BAUDRATE_19200,
	SERIAL_BAUDRATE_38400,
	SERIAL_BAUDRATE_57600,
	SERIAL_BAUDRATE_115200	
};


int UartOpenDev( char *dev );
int UartWrite( int fd, void *buf, int len );
int UartRead( int fd, void *buf, int len );
void UartClose( int fd );

int UartSetParam( int handle, SERIAL_BAUDRATE_ST baudrate, 
								  SERIAL_DATABITS_ST databits, 
								  SERIAL_STOPBITS_ST stopbits,
								  SERIAL_PARITY_ST parity );
int UartSetParamEx( int handle, SERIAL_BAUDRATE_ST baudrate, 
								  SERIAL_DATABITS_ST databits, 
								  SERIAL_STOPBITS_ST stopbits,
								  SERIAL_PARITY_ST parity );

	
#endif

#ifndef _TBA_3093_H
#define	_TBA_3093_H
#include "stm32f2xx.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"

// 定义NORMAL工作模式接受标识
#define COM_RX_HEAD			0x01
#define COM_RX_LEN				0x02
#define COM_RX_TYPE			0x03
#define COM_RX_NUM				0x04
#define COM_RX_PARA			0x05
#define COM_RX_CHKSUM			0x06

#define NORMAL			0x00
#define ATS_MODE            0x01
#define CLOCK			0x02
#define ISCS_MODE          0X03    //也就是MODBUS模式
#define GONGFANG           0x04
#define DATA_OUT   1
#define DATA_IN	    0

//定义综合监控系统状态机状态
#define ISCS_SLAVE_ADDR			0x01    //接收从机地址
#define ISCS_FUNCTION_CODE      		0x02    //接收功能码
#define ISCS_REG_ADDR_H     			0x03    //接收寄存器高字节
#define ISCS_REG_ADDR_L     			0x04    //接收寄存器低字节
#define ISCS_VALUE_H				0x05    //接收寄存器内容高字节
#define ISCS_VALUE_L				0x06    //接收寄存器内容低字节
#define ISCS_CRC_L			    		0x07
#define ISCS_CRC_H			    		0x08
#define ISCS_ADDR               			0x5a

#define ISCS_CMD_OFFS   0X01    //功能码在缓冲区中的偏移地址
#define ISCS_DATA_OFFS  0X03    //转发数据在缓冲区中的偏移地址

// 定义时钟系统状态机状态
#define CLOCK_RX_STX			0x01
#define CLOCK_RX_STX1			0x02
#define CLOCK_RX_DATA			0x03
#define CLOCK_RX_ETX			0x04
#define CLOCK_HEAD1			0XEB
#define CLOCK_HEAD2			0X90

//定义信号系统状态机状态
#define ATS_HEAD_STAT           		0X01
#define ATS_TARGET_ADDR_STAT    	0X02
#define ATS_SOURCE_ADDR_STAT   	0X03
#define ATS_SERIAL_NUM_STAT     		0X04
#define ATS_CONTROL_STAT       		0X05
#define ATS_DATA_LEN1_STAT     		0X06    //接收数据段数据长度字节
#define ATS_DATA_LEN2_STAT      		0X07    //接收数据段数据长度字节
#define ATS_DATA_OTHER_STAT     		0X08    //接收数据段中其他信息
#define ATS_CRC_L_STAT          		0X09
#define ATS_CRC_H_STAT          		0X0A

//定义信号系统相关的宏
#define ATS_CD2_CMD             	0X12
#define ATS_CD3_CMD             	0X13
#define ATS_ACK_CMD             	0X06
#define ATS_NACK_CMD            	0X15
#define ATS_STX_CMD             	0X02
#define ATS_RESEND_CMD       		 0XFF    //该命令是本人添加的，用于重发上一条命令
#define ATS_BS_ADDR             		0X0A
#define ATS_ATS_ADDR            	0XA0
#define ATS_TRANSCODE           	0x7E
#define ATS_BS_TRAIN_INFO       	0x21

//-------------------以下是与信号系统接口相关的数据结构---------------------------------
#define ATS_DATA_MAX        		1152//2400    //数据段最大2400字节
//缓冲中各数据段的基址
#define ATS_TARGET_BASE     		0X00
#define ATS_SOURCE_BASE     		(ATS_TARGET_BASE+1)
#define ATS_SERIAL_BASE     		(ATS_SOURCE_BASE+1)
#define ATS_CONTROL_BASE    	(ATS_SERIAL_BASE+1)
#define ATS_DATA_BASE       		(ATS_CONTROL_BASE+1)
#define ATS_CRC_L_BASE      		(ATS_DATA_BASE+ATS_DATA_MAX+2)//+2是因为ATS_DATA_MAX并不包含占2个字节“length”
#define ATS_CRC_H_BASE      		(ATS_CRC_L_BASE+1)
//以下是数据段内各种信息的偏移
#define LENGTH_L_OFFS       		0X00    //LENGTH_L在 Domain中的偏移地址
#define LENGTH_H_OFFS       		(LENGTH_L_OFFS+1)
#define CONTROL_STATUS_OFFS 	(LENGTH_H_OFFS+1)
#define MESSAGE_FLAG_OFFS   	(CONTROL_STATUS_OFFS+1)    //MESSAGE_FLAG在Data Domain中的偏移地址
#define PLATFORM_CNT_OFFS   	(MESSAGE_FLAG_OFFS+1)
//下面开始的是与每一车站相关的信息
#define STATION_ID_OFFS     		(PLATFORM_CNT_OFFS+1)
#define PLATFORM_ID_OFFS    		(STATION_ID_OFFS+1)
#define VALIDITY_FIELD_OFFS 	(PLATFORM_ID_OFFS+2)
#define UNIT_NUM_OFFS       		(VALIDITY_FIELD_OFFS+1)
#define SERVER_NUM_OFFS     		(UNIT_NUM_OFFS+2)
#define DESTINATION_ID_OFFS 	(SERVER_NUM_OFFS+2)
#define CURRENT_TIME_OFFS   	(DESTINATION_ID_OFFS+2)
#define ARRIVAL_TIME_OFFS   		(CURRENT_TIME_OFFS+4)
#define DEPARTURE_TIME_OFFS 	(ARRIVAL_TIME_OFFS+4)
#define ARRIVAL_STAT_OFFS   		(DEPARTURE_TIME_OFFS+4)
#define DEPARTURE_STAT_OFFS 	(ARRIVAL_STAT_OFFS+1)
#define HOLD_STAT_OFFS      		(DEPARTURE_STAT_OFFS+1)
#define SKIP_STAT_OFFS      		(HOLD_STAT_OFFS+1)
#define OUT_SERVICE_OFFS    		(SKIP_STAT_OFFS+1)
#define LAST_TRAIN_OFFS     		(OUT_SERVICE_OFFS+1)
#define STATION_DATA_LEN    		(LAST_TRAIN_OFFS-STATION_ID_OFFS+1) //这是一个车站信息的长度


#define LINE_NUM    0x02;       // 号线 

#define CONFG_INIT_422_1 		uart2_config
#define CONFG_INIT_422_2 		uart3_config
#define SEND_BUF_422_1    		uart2_write_buf
#define SEND_BUF_422_2    		uart3_write_buf
#define SEND_CHAR_422_1    		uart2_write_char
#define SEND_CHAR_422_2    		uart3_write_char
#define READ_CHAR_422_1  		uart2_read_char
#define READ_CHAR_422_2  		uart3_read_char
#define COM_READ_CHAR 			uart1_read_char
#define COM_WRITE_BUF 			uart1_write_buf
#define COM_WRITE_CHAR		uart1_write_char

//#define COM_READ_CHAR 			uart3_read_char
//#define COM_WRITE_BUF 			uart3_write_buf
//#define COM_WRITE_CHAR		uart3_write_char

extern uint8_t com422_1_work_mode;
extern uint8_t com422_2_work_mode;
extern uint8_t ATS[ATS_CRC_H_BASE + 1];
extern uint8_t tx_seq;    //用在与ATS接口上，用于保存发送序号
extern uint8_t rx_seq;    //用在与ATS接口上，用于保存接收序号
extern uint32_t fas_timer;
extern uint8_t fas_flag;

extern void device_addr_init(void);
extern void get_device_addr(void);
extern uint8_t get_mode_flag(void);
extern void proc_com_data(void);
extern void proc_422_1_data(void);
extern void proc_422_2_data(void);
extern void prc_422_1_normal(uint8_t tmp);
extern void prc_422_1_iscs(uint8_t ch);
extern unsigned int check_modbus_crc(unsigned char* nData, unsigned int wLength);
extern void prc_422_1_clock(uint8_t tmp);
extern void prc_422_1_ats(uint8_t ch);
extern void prc_clock_data(unsigned char *buf);
extern unsigned int check_ats_crc(unsigned char* nData, unsigned int len);
extern uint8_t get_connect_ats_1(void);
extern void broadcast_init(void);
extern void send_broadcast_msg(uint8_t *buf, uint8_t length);

extern void send_to_com(uint8_t dev_type, uint8_t dev_num, uint8_t cmd_key, uint8_t n, uint8_t *buf);
extern void send_to_net(uint8_t dev_type, uint8_t dev_num, uint8_t cmd_key, uint8_t n, uint8_t *buf);
extern void send_com_fifo(uint8_t *buf, uint16_t length);
#endif

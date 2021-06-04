/******************************************************************************
*文件名称：tba_3093.c		       		         
*作          者：hk           							 
*描          述：通信扩展主处理文件
*创建日期：2010-10-11
*修改记录：无								
******************************************************************************/
#include "tba_3093.h"
#include "main.h"
#include "stm32f2x7_eth.h"
#include "netconf.h"
#include "stm32f2xx_usart.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DEVICE_TYPE			0x08
#define IRQ_STATUS_PIN          GPIO_Pin_3
#define IRQ_OUT_PIN               GPIO_Pin_4
#define IRQ_GPIO                     GPIOD           

#define COM_FIFO_MAX_LENGTH   128//256

uint8_t device_addr;
uint8_t com422_1_work_mode;
uint8_t com422_2_work_mode;
uint8_t mode_flag = 0;
uint32_t  baud_rate[] = {300, 600, 1200, 2400, 4800, 9600, 19200};
uint16_t parity_check[] = {USART_Parity_No, USART_Parity_Even, USART_Parity_Odd};


uint8_t fas_flag = 0;
uint8_t ATS[ATS_CRC_H_BASE + 1];

uint8_t tx_seq;    //用在与ATS接口上，用于保存发送序号
uint8_t rx_seq;    //用在与ATS接口上，用于保存接收序号
uint32_t fas_timer;

uint8_t com_fifo_buf[COM_FIFO_MAX_LENGTH];
uint8_t com_fifo_index = 0;

struct udp_pcb *udppcb;
struct pbuf *p;

//CRC-16:(美国二进制同步系统中采用)G(X) = X16 + X15 + X2 + 1
unsigned int  MODBUS_CRC_TBL[]={         
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
unsigned int ats_crc_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, //7
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, //15
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, //23
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, //31
    
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, //63
    
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, //95
    
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


/****************************************************
*函数名：device_addr_init			       		         
*作者：hk             							 
*描述：GPIO口初始化	
*输入：无										
*输出：设备地址	
*返回值：无
*修改记录：无								
****************************************************/
void device_addr_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //IRQ_STATUS 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = IRQ_STATUS_PIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
     /* Configure the IRQ_OUT pin */
    GPIO_InitStructure.GPIO_Pin = IRQ_OUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD, IRQ_OUT_PIN);
}

/****************************************************
*函数名：get_device_addr			       		         
*作者：hk             							 
*描述：获取设备地址			
*输入：无										
*输出：
*返回值：无
*修改记录：无								
****************************************************/
void get_device_addr()
{
    device_addr = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15);
    device_addr = (device_addr<<1)|GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
    device_addr = (device_addr<<1)|GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13);
    device_addr = (device_addr<<1)|GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);

	//device_addr = 0x02;
}

unsigned char chksum(unsigned char *buf, unsigned char n)
{
	unsigned char  i;
	unsigned char  tmp = 0;

	for(i = 0; i < n; i++)
		tmp += buf[i];

	return(tmp);
}

/****************************************************
*函数名：send_to_com			       		         
*作者：hk             							 
*描述：往内部串口发送一包数据		
*输入：dev_type  :指设备类型
*                dev_num   :设备编号
*                cmd_key   ：命令字,若cmd_key == NULL，表示命令字为空
*                n         ：指buf中保持的数据长度
*输出：
*返回值：无
*修改记录：无								
****************************************************/
void send_to_com(uint8_t dev_type, uint8_t dev_num, uint8_t cmd_key, uint8_t n, uint8_t *buf)
{
    uint8_t  i;
    uint8_t txbuf[256+6];
    uint8_t index;

    index = 0;
	txbuf[index++] = 0x7e;
    if(cmd_key == 0)
    {
        txbuf[index++] = n + 2;
    }else
    {
        txbuf[index++] = n + 3;
    }
	txbuf[index++] = dev_type;
	txbuf[index++] = dev_num;
    if(cmd_key != 0)
    {
        txbuf[index++] = cmd_key;        
    }

	for(i = 0; i < n; i++)
		txbuf[index++] = buf[i];
	txbuf[index++] = chksum(&txbuf[2], index - 2);
	send_com_fifo(txbuf, index);
	fill_sd_data_fifo(txbuf, index, DATA_OUT, 1);
}

/****************************************************
*函数名：send_to_net			       		         
*作者：hk             							 
*描述：往内部以太网广播一包数据		
*输入：dev_type  :指设备类型
*                dev_num   :设备编号
*                cmd_key   ：命令字,若cmd_key == NULL，表示命令字为空
*                n         ：指buf中保持的数据长度
*输出：
*返回值：无
*修改记录：无								
****************************************************/
void send_to_net(uint8_t dev_type, uint8_t dev_num, uint8_t cmd_key, uint8_t n, uint8_t *buf)
{
    uint8_t  i;
    uint8_t txbuf[256+6];
    uint8_t index;

    index = 0;
    txbuf[index++] = 0x7e;
    if(cmd_key == 0)
    {
        txbuf[index++] = n + 2;
    }else
    {
        txbuf[index++] = n + 3;
    }
	txbuf[index++] = dev_type;
	txbuf[index++] = dev_num;
    if(cmd_key != 0)
    {
        txbuf[index++] = cmd_key;        
    }

	for(i = 0; i < n; i++)
		txbuf[index++] = buf[i];
	txbuf[index++] = chksum(&txbuf[2], index - 2);
	send_broadcast_msg(txbuf, index);
	fill_sd_data_fifo(txbuf, index, DATA_OUT, 6);
}

/****************************************************
*函数名：dispose_para			       		         
*作者：hk             							 
*描述：内部串口数据解析		
*输入：buf：输入数据的缓冲区指针
                 len: 缓冲区中数据的长度
*输出：
*返回值：无
*修改记录：无								
****************************************************/
void dispose_para(uint8_t *buf, uint8_t len)
{
	uint32_t baud;
	uint8_t parity;
	switch(buf[0])
	{
	case 0x01:  //设置工作模式命令
	com422_1_work_mode = buf[1];
	com422_2_work_mode = buf[2];
	
	mode_flag = 1;
	break;

	case 0x02:				//转发内部串口收到数据
		switch(buf[1])
		{
            case 0x01:			//422口1
                switch(com422_1_work_mode)	//422口1工作模式
                {
                    case NORMAL:
                        SEND_BUF_422_1(&buf[2], (len - 2));
			     //fill_sd_data_fifo(&buf[2], (len - 2));
                    break;

                    case GONGFANG:
                        SEND_BUF_422_1(&buf[2], (len - 2));
			     //fill_sd_data_fifo(&buf[2], (len - 2));
                    break;
                    
                    default:    //其他工作模式不用转发
                    break;
                }
            break;

            case 0x02:			//422口2
                switch(com422_2_work_mode)	//422口2工作模式
                {
                    case NORMAL:
                        SEND_BUF_422_2(&buf[2], (len - 2));
			     //fill_sd_data_fifo(&buf[2], (len - 2));
                    break;
                        
                    case GONGFANG:
                        SEND_BUF_422_2(&buf[2], (len - 2));
			     //fill_sd_data_fifo(&buf[2], (len - 2));
                    break;
                    
                    default:    //其他工作模式不用转发
                    break;
                }
            break;
        
            default:
            break;
		}
		break;

	case 0x03:
		send_to_com(DEVICE_TYPE, device_addr, 0x03, 0, 0);
      break;
	case 0x04://设置波特率和奇偶校验
	    baud = buf[1] & 0x3f;
	    parity = (buf[1] >> 6) & 0x03; 
	    CONFG_INIT_422_1(baud_rate[baud],parity_check[parity]);
	    baud = buf[2] & 0x3f;
	    parity = (buf[3] >> 6) & 0x03; 
	    CONFG_INIT_422_2(baud_rate[baud],parity_check[parity]);
      break;
	case 0xa4://火灾确认
		if(fas_flag)
			send_to_com(0xe0, 0x01, 0xa4, 0, 0);
		break;
	case 0xaa:
		while(1);
		//break;
	case 0x89:
		if(buf[1] >99 || buf[2] > 12 || buf[3] > 31 || buf[4] > 23 || buf[5] > 59 || buf[6] > 59)
			break;
		set_rtc_date(buf[1], buf[2], buf[3]);
		set_rtc_time(buf[4], buf[5], buf[6]);
      break;
	}	
}

/****************************************************
*函数名：proc_com_data			       		         
*作者：hk             							 
*描述：内部串口数据处理	
*输入：无										
*输出：设备地址	
*返回值：无
*修改记录：无								
****************************************************/
void proc_com_data()
{
      static unsigned char para_len = 0;
      static unsigned char  rx_chksum;
      uint8_t  tmp;
      static uint8_t index;
      static uint8_t com_rx_status = COM_RX_HEAD;
      uint8_t dat[128];
      static uint32_t mode_timer = 0;
      uint32_t time_out = 0;
	
	//如果没有设置模式，则请求设置模式
      if((get_mode_flag() == 0) && check_timer(&mode_timer, 1500))
      	{
      	    send_to_com(DEVICE_TYPE, device_addr, 0x01, 0, 0);
      	}

      //发送缓冲区中有数据需要发送
	if(com_fifo_index > 0)
	{
	    //空闲状态，则拉低，发送数据
	    if(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN))
	    {
	        time_out = get_time();
	        while(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN))
	        {
	            GPIO_ResetBits(IRQ_GPIO, IRQ_OUT_PIN);
		      if(check_timer(&time_out, 30))
			  break;
	        }
		 if(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN) == 0)
		 {
		     COM_WRITE_BUF(com_fifo_buf, com_fifo_index);
		     GPIO_SetBits(IRQ_GPIO, IRQ_OUT_PIN);
		     com_fifo_index = 0;
		     //fill_sd_data_fifo(com_fifo_buf, com_fifo_index);
		 }
          }
	}

	if(COM_READ_CHAR(&tmp) == 0)
	{
		return;
	}

	switch(com_rx_status)
	{
	case COM_RX_HEAD:
		if(tmp == 0x7e)
		{
			rx_chksum = 0;
			index = 0;
			com_rx_status = COM_RX_LEN;
			dat[index++] = tmp;
		}
		else
		{
			com_rx_status = COM_RX_HEAD;
		}
		break;

	case COM_RX_LEN:
		if(tmp < 3)
		{
			com_rx_status = COM_RX_HEAD;
		}
		else
		{
			para_len = tmp - 2;
			com_rx_status = COM_RX_TYPE;	
			dat[index++] = tmp;
		}		
		break;

	case COM_RX_TYPE:
		if(tmp == DEVICE_TYPE)
		{
			rx_chksum += tmp;
			com_rx_status = COM_RX_NUM;
			dat[index++] = tmp;
		}
		else
		{
			com_rx_status = COM_RX_HEAD;
		}
		break;

	case COM_RX_NUM:
		if(tmp == device_addr)
		{
			rx_chksum += tmp;
			com_rx_status = COM_RX_PARA;
			dat[index++] = tmp;
		}
		else
		{
			com_rx_status = COM_RX_HEAD;
		}
		break;	

	case COM_RX_PARA:
		dat[index++] = tmp;
		rx_chksum += tmp;
		if(--para_len > 0)
			com_rx_status = COM_RX_PARA;
		else
			com_rx_status = COM_RX_CHKSUM;
		break;

	case COM_RX_CHKSUM:
		if(tmp == rx_chksum)
		{
			if(index > 0)
			{
				dispose_para(&dat[4], index-4);
				dat[index++] = tmp;
				fill_sd_data_fifo(dat, index, DATA_IN, 1);
				index = 0;
			}
		}
		com_rx_status = COM_RX_HEAD;
		break;
	}	
}


/****************************************************
*函数名：get_mode_flag			       		         
*作者：hk             							 
*描述：获取设置模式标识	
*输入：无										
*输出：1:已经处理了设置模式命令
*返回值：无
*修改记录：无								
****************************************************/
uint8_t get_mode_flag()
{
    return mode_flag;
}


//CRC-16:(美国二进制同步系统中采用)G(X) = X16 + X15 + X2 + 1
unsigned int check_modbus_crc(unsigned char* nData, unsigned int wLength)
{
	unsigned char nTemp;
	unsigned int wCRCWord=0xffff;
    while (wLength--)
    {
        nTemp = *nData ^ wCRCWord;
		nData++;
        wCRCWord >>= 8;
        wCRCWord ^= MODBUS_CRC_TBL[nTemp];
    }
    return wCRCWord;
}


void prc_clock_data(unsigned char *buf)
{
	unsigned char dat[6];

	dat[0] = (buf[0]*1000+buf[1]*100+buf[2]*10+buf[3])%100;//十进制年份的低2位
	dat[1] = buf[4]*10+buf[5];//月
	dat[2] = buf[6]*10+buf[7];//日
	dat[3] = buf[9]*10+buf[10];//时
	dat[4] = buf[11]*10+buf[12];//分
	dat[5] = buf[13]*10+buf[14];//秒

      send_to_com(0xe1, 0x01, 0, 6, dat);
}

//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
unsigned int check_ats_crc(unsigned char* nData, unsigned int len)
{
    unsigned int crc = 0;
    unsigned char crc_H8;
    unsigned char *ptr = nData;

    while( len-- )
    {
        crc_H8 = (unsigned char)(crc >> 8);
        crc = crc << 8;
        crc = crc ^ ats_crc_table[crc_H8 ^ *ptr];
        ptr++;
    }
    return crc;
}

void broadcast_init()
{
     udppcb = udp_new();
     udp_bind(udppcb, IP_ADDR_ANY, 3001);
}

void send_broadcast_msg(uint8_t *buf, uint8_t length)
{
    p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    memcpy (p->payload, buf, length);
    udp_sendto(udppcb, p, IP_ADDR_BROADCAST, 3002);
    pbuf_free(p); //De-allocate packet buffer
}

void send_com_fifo(uint8_t *buf, uint16_t length)
{
    uint32_t time_out = 0;
    uint8_t tmp = 0;

    //空闲状态，则拉低，发送数据
    if(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN))
    {
        time_out = get_time();
        while(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN))
        {
	      GPIO_ResetBits(IRQ_GPIO, IRQ_OUT_PIN);
	      if(check_timer(&time_out, 30))
		  break;
        }
	 if(GPIO_ReadInputDataBit(IRQ_GPIO, IRQ_STATUS_PIN) == 0)
	 {
	     COM_WRITE_BUF(buf, length);
	     GPIO_SetBits(IRQ_GPIO, IRQ_OUT_PIN);
	    // fill_sd_data_fifo(buf, length);
	     return;
	 }
    }
   if(length + com_fifo_index > COM_FIFO_MAX_LENGTH)
   	com_fifo_index = 0;

   for(tmp = 0; tmp < length; tmp++)
   {
       com_fifo_buf[com_fifo_index++] = buf[tmp];
   }
   GPIO_SetBits(IRQ_GPIO, IRQ_OUT_PIN);
 }	

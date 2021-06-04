/************************************************************
  Copyright (C), 1988-1999, beihai Tech. Co., Ltd.
  FileName: usb.c
  Author:        hk
  Description:     usb 数据的读取
  Version:         // 版本信息
  History:         
      <author>  <time>   <version >   <desc>
      hk           2012-1-4    1.0     build this moudle  
***********************************************************/

#include "usbd_hid_core.h"
#include "usbd_conf.h"
#include "usb.h"
#include "flash_if.h"
#include "common.h"
#include "config.h"
#include "httpserver.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at45db161.h"

uint8_t USB_Rx_Buffer[USB_BUF_LENGTH];
uint8_t USB_Tx_Buffer[USB_BUF_LENGTH];
uint16_t usb_recv_fifo_in = 0;
uint16_t usb_recv_fifo_out = 0;
uint16_t usb_send_fifo_in = 0;
uint16_t usb_send_fifo_out = 0;

extern USB_OTG_CORE_HANDLE           USB_OTG_dev;


static uint8_t ptu_recv_buf[PTU_BUF_SIZE];
static uint16_t ptu_recv_length = 0;

static __IO uint32_t flash_write_address;

void upgrade(uint8_t * packet);

/*************************************************
  Function:       usb_read_char
  Description:    查询usb的接收fifo 如果有数据读取一个字节
  Input:          ch---读取字符存放地址
  Output:         none
  Return:         如果有数据返回1
  Others:         
*************************************************/
uint8_t usb_read_char(uint8_t *ch)
{
	if(usb_recv_fifo_out == usb_recv_fifo_in)
		return 0;
	*ch=USB_Rx_Buffer[usb_recv_fifo_out];//得到当前要读取的字符
	usb_recv_fifo_out++;//指向下一个要读取的字符
	if(usb_recv_fifo_out == USB_BUF_LENGTH)
		usb_recv_fifo_out = 0;//如果到了fifo末尾 则重从头开始
	return 1;
}

/*************************************************
  Function:       usb_write_char
  Description:    向usb数据端点发送一个字节
  Input:          ch---发送的数据
  Output:          无
  Return:         无
  Others:         
*************************************************/
void usb_write_char(uint8_t ch)
{
      uint8_t tmp_buf[10] = {0};
      tmp_buf[0] = 8;
	tmp_buf[2] = ch;
	USBD_HID_SendBuf(&USB_OTG_dev, tmp_buf, 10);
    //USBD_HID_SendBuf(&USB_OTG_dev, &ch, 1);
    //USB_Tx_Buffer[usb_send_fifo_in++] = ch;
    //if(usb_send_fifo_in >= USB_BUF_LENGTH)
    {
     // USBD_HID_SendBuf(&USB_OTG_dev, 0, 0);
	//usb_send_fifo_in = 0;
    }
}

/*************************************************
  Function:       usb_write_buf
  Description:    向usb数据端点发送一串数据
  Input:          buf---发送的数据
                    length---发送数据的长度
  Output:          无
  Return:         无
  Others:         
*************************************************/
void usb_write_buf(uint8_t *buf, uint16_t length)
{
    uint16_t tmp = 0;
    uint8_t tmp_buf[10];
#if 1
    while((length -tmp) > 8)
    { 
        memset(tmp_buf, 0x00, sizeof(tmp_buf));
	  tmp_buf[0] = 8;
	  memcpy(&tmp_buf[2], &buf[tmp], 8);
        USBD_HID_SendBuf(&USB_OTG_dev, tmp_buf, 10);
	  tmp = tmp + 8;
    }
    if((length - tmp) > 0)
    {
      memset(tmp_buf, 0x00, sizeof(tmp_buf));
      tmp_buf[0] = 8;
	memcpy(&tmp_buf[2], &buf[tmp], (length - tmp));
	USBD_HID_SendBuf(&USB_OTG_dev, tmp_buf, 10);
    }
 #endif   
  //  for(tmp = 0; tmp < length; tmp++)
    {
   //     usb_write_char(buf[tmp]);
    }
}

static uint8_t ptu_get_packet(void)
{
	static uint8_t start_flag = 0;
	uint8_t temp;
		
	while(usb_read_char(&temp))
	{	
		//自定义命令
		if( !start_flag )
		{
			//读到分隔符 判断是否有数据
			if(temp == '\r')
			{
				if(ptu_recv_length)
				{
					ptu_recv_buf[ptu_recv_length]=0;//添加结束符
					ptu_recv_length=0;//缓冲区指针清0
					return 2;//得到一个完整命令
				}
			}
			else if(temp == '\n')
			{
				
			}
			else if(temp == TBA_PACKET_TAB)
			{
				start_flag = 1;
				ptu_recv_length = 0;
			}
			else if(temp)//注意过滤 \0 字符
			{
				ptu_recv_buf[ptu_recv_length++] = temp;//向缓存送数据
				if(ptu_recv_length >= PTU_BUF_SIZE-1)
					ptu_recv_length=0;//判断是否超长	
			}
		}
		//总线命令
		else
		{
			//读到分隔符 判断是否有数据 并执行逆转码 然后判断转码以后总长度和校验和
			if(temp == TBA_PACKET_TAB)
			{
				//判断长度 判断是否有数据
				if(ptu_recv_length >= 3)
				{
					start_flag=0;
					return 1;
				}
				ptu_recv_length=0;
			}
			//其他字符串直接送缓冲区
			else
			{
				ptu_recv_buf[ptu_recv_length] = temp;
				if(++ptu_recv_length >= PTU_BUF_SIZE)
					ptu_recv_length = 0;
			}
		}
	}
	return 0;
}

static void exce_ptu_cmd(uint8_t *packet, uint8_t length)
{
	uint8_t recv_flag=0;
	uint8_t checksum=0;
	uint16_t src_length=0;//逆转码之前的长度
	uint16_t dst_length=0;//逆转码以后的长度
	uint16_t ptu_recv_data_len = 0;
#if 1
	//执行逆转码 并计算校验和
	for(src_length=0; src_length<ptu_recv_length; src_length++)
	{
		//拷贝数据
		packet[dst_length]=packet[src_length];
		//判断刚刚拷贝的是否是转码符
		if(packet[src_length]==TBA_PACKET_DLE && src_length<ptu_recv_length-1)
		{
			//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_TAB 并跳过下一个字符
			if(packet[src_length+1]==TBA_PACKET_DLE_TAB)
			{
				packet[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TBA_PACKET_TAB
				src_length++;//并跳过下一个字符
			}
			//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TBA_PACKET_DLE 并跳过下一个字符
			else if(packet[src_length+1]==TBA_PACKET_DLE_DLE)
			{
				packet[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TBA_PACKET_DLE
				src_length++;//并跳过下一个字符
			}
			else
			{
				//print_line("ptu 7f error");
	//			//数据错误----7f没有被正确转码
				dst_length=0;//清除转码以后的长度
				break;
			}
		}
		checksum+=packet[dst_length];
		dst_length++;
	}
	if((checksum == 0x55) && ((dst_length - 2) == packet[0]))
	{
		recv_flag=1;
		//ptu_recv_length=0;
		//print_line("PTU checksum ok");
	}
	else
	{
		recv_flag=0;
		//ptu_recv_length=0;
		//print_int("%xPTU checksum error",checksum);
		
	}
	if(recv_flag)
	{	
            if(packet[1] == 0x01) //usb 升级数据包
            	{
            		gUpdateBmpSt.tick = gSystemTick;
            	    upgrade(packet);
            	}
		else if(packet[1] == 0x02) //版本查询
		{
		    usb_write_buf(GREETING, strlen(GREETING));
		    usb_write_buf(VERSION, strlen(VERSION));
			UsbPrintConfig();
		}
		else if(packet[1] == 0x03)
		{
		    char buf[32] = {0};
           	    snprintf(buf, sizeof(buf), "ip:%d.%d.%d.%d\r\n", saveparm.ipaddr[0], saveparm.ipaddr[1], saveparm.ipaddr[2], saveparm.ipaddr[3]);
	          usb_write_buf(buf, strlen(buf));
	          memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "netmask:%d.%d.%d.%d\r\n", saveparm.net_mask[0], saveparm.net_mask[1], saveparm.net_mask[2], saveparm.net_mask[3]);
                usb_write_buf(buf, strlen(buf));

                memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "gateway:%d.%d.%d.%d\r\n", saveparm.gw_addr[0], saveparm.gw_addr[1], saveparm.gw_addr[2], saveparm.gw_addr[3]);
	          usb_write_buf(buf, strlen(buf));

		    memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "mac:%x-%x-%x-%x-%x-%x\r\n", saveparm.mac[0], saveparm.mac[1], saveparm.mac[2], saveparm.mac[3], saveparm.mac[4], saveparm.mac[5]);
	          usb_write_buf(buf, strlen(buf));
		}
		else if(packet[1] == 0x04)
		{
		    saveparm.ipaddr[0] = packet[2];
		    saveparm.ipaddr[1] = packet[3];
		    saveparm.ipaddr[2] = packet[4];
		    saveparm.ipaddr[3] = packet[5];
		    saveparm.net_mask[0] = packet[6];
		    saveparm.net_mask[1] = packet[7];
		    saveparm.net_mask[2] = packet[8];
		    saveparm.net_mask[3] = packet[9];
		    saveparm.gw_addr[0] = packet[10];
		    saveparm.gw_addr[1] = packet[11];
		    saveparm.gw_addr[2] = packet[12];
		    saveparm.gw_addr[3] = packet[13];
		    save_parm();
		    usb_write_buf("set ip success\r\n", strlen("set ip success\r\n"));
		}
		else if(packet[1] == 0x05)
		{
		    saveparm.mac[0] = packet[2];
		    saveparm.mac[1] = packet[3];
		    saveparm.mac[2] = packet[4];
		    saveparm.mac[3] = packet[5];
		    saveparm.mac[4] = packet[6];
		    saveparm.mac[5] = packet[7];
		    save_parm();
		    usb_write_buf("set mac success\r\n", strlen("set mac success\r\n"));
		}
	}
#endif
}

void upgrade(uint8_t * packet)
{
    static uint32_t total_size = 0;
    static uint32_t recv_size = 0;
    uint16_t index;
    uint8_t buf[32] = {0};
    if(packet[2] == 0x02) //usb 升级数据
    {
        recv_size = recv_size + packet[0] - 4;
	  FLASH_If_Write(&flash_write_address, (uint32_t*) &packet[5], (uint16_t) (packet[0] - 4)/4);
	  index = packet[3];
	  index = index << 8 | packet[4];
	  snprintf(buf, sizeof(buf), "wirte %d ok\r\n\0", index);
	  usb_write_buf(buf, strlen(buf));
     }
    else if(packet[2] == 0x01)//usb 升级开始命令
    {
		write_version_flag(0x55);
		DispTypeInit(DISP_TYPE_DOWNING);
		total_size = 0;
		recv_size = 0;
		total_size = packet[5];
		total_size = total_size<<8 | packet[6];
		total_size = total_size<<8 | packet[7];
		total_size = total_size<<8 | packet[8];

		FLASH_If_Init();
		// erase user flash area 
		FLASH_If_Erase_Prog_Bak(PROG_BAK_ADDRESS);
		flash_write_address = PROG_BAK_ADDRESS;
		usb_write_buf("Begin to upgrade\r\n", strlen("Begin to upgrade\r\n"));
    }
    else if(packet[2] == 0x03)//usb 升级结束命令
    {
        if(total_size == recv_size)
        {
            usb_write_buf("End upgrade\r\n", strlen("End upgrade\r\n"));
            USB_OTG_BSP_DisableInterrupt ();
   	      exec_iap_fun();
        }
    }
}

void ptu_proc(void)
{
	uint8_t type;
	
	//接收命令
	type = ptu_get_packet();	
	//分析命令
	if(type == 1)
	{
            exce_ptu_cmd(ptu_recv_buf, ptu_recv_length);
            ptu_recv_length = 0;
	}
	else if(type == 2)
	{
	    
	}
}


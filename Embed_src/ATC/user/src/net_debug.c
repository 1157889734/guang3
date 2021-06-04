#include "main.h"
#include "net_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_TO_NET_SIZE 960

static uint8_t send_data_to_net_buf[DATA_TO_NET_SIZE];
static uint16_t send_data_to_net_length = 0;

uint8_t send_data_to_net_flag = 0;  // 1:获取atc数据 2:获取tms数据3:获取atc/tms数据 4:获取pis数据


void fill_net_buf(uint8_t *buf, uint16_t length) // modified by Lison on 2012-8-31 8:46:09 
{
    int i=0;

    if((send_data_to_net_length + length) >= DATA_TO_NET_SIZE)
    {
	  if(send_data_to_net_flag)
	  {
	       send_msg_to_telnet(send_data_to_net_buf, send_data_to_net_length);
	  }
         send_data_to_net_length = 0;
    }
	
    for(i = 0; i < length; i++)
    {
        if(send_data_to_net_length  >= DATA_TO_NET_SIZE)
	 {
	      return;
        }
        send_data_to_net_buf[send_data_to_net_length++] = buf[i];
    }
	
#if 0
    uint8_t asiic_buf[]="0123456789ABCDEF";
    int i=0;

    if((send_data_to_net_length + length *3) >= DATA_TO_NET_SIZE)
    {
	  if(send_data_to_net_flag)
	       send_msg_to_telnet(send_data_to_net_buf, send_data_to_net_length);
         send_data_to_net_length = 0;
    }
	
    for(i = 0; i < length; i++)
    {
        if(send_data_to_net_length  >= DATA_TO_NET_SIZE)
	 {
	      return;
        }
        send_data_to_net_buf[send_data_to_net_length++] = asiic_buf[buf[i]>>4];
        send_data_to_net_buf[send_data_to_net_length++] = asiic_buf[buf[i]&0x0F];
        send_data_to_net_buf[send_data_to_net_length++] = ' ';
    }
#endif
}

void net_debug_proc(void)
{
    if(send_data_to_net_flag)
	send_msg_to_telnet(send_data_to_net_buf, send_data_to_net_length);
    send_data_to_net_length = 0;
}

void set_data_flag(char *buf)
{
    char *p = NULL;
    p = strstr(buf, "atc");
    if(p != NULL)
    {
        send_data_to_net_flag |= GET_DATA_FROM_ATC;
    }
    p = strstr(buf, "tms");
    if(p != NULL)
    {
        send_data_to_net_flag |= GET_DATA_FROM_TMS;
    }
    p = strstr(buf, "pis");
    if(p != NULL)
    {
        send_data_to_net_flag |= GET_DATA_FROM_PIS;
    }
}

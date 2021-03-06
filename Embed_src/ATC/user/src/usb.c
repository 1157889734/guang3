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
#include "usb_bsp.h"
#include "flash_if.h"
#include "common.h"
#include "config.h"
#include "httpserver.h"
#include "sd_data.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t USB_Rx_Buffer[USB_BUF_LENGTH];
uint8_t USB_Tx_Buffer[USB_BUF_LENGTH];
uint16_t usb_recv_fifo_in = 0;
uint16_t usb_recv_fifo_out = 0;
uint16_t usb_send_fifo_in = 0;
uint16_t usb_send_fifo_out = 0;

uint8 tcms_debug_flag = 0;
uint8 pisc_debug_flag = 0;
uint8 tba_debug_flag = 0;


extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
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
    if (usb_recv_fifo_out == usb_recv_fifo_in)
    {
        return 0;
    }
    *ch = USB_Rx_Buffer[usb_recv_fifo_out]; //得到当前要读取的字符
    usb_recv_fifo_out++;//指向下一个要读取的字符
    
    if (usb_recv_fifo_out == USB_BUF_LENGTH)
    {
        usb_recv_fifo_out = 0;//如果到了fifo末尾 则重从头开始
    }
    return 1;
}

/************************************************* 0000
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
    
    while ((length - tmp) > 8)
    {
        memset(tmp_buf, 0x00, sizeof(tmp_buf));
        tmp_buf[0] = 8;
        memcpy(&tmp_buf[2], &buf[tmp], 8);
        USBD_HID_SendBuf(&USB_OTG_dev, tmp_buf, 10);
        tmp = tmp + 8;
    }
    
    if ((length - tmp) > 0)
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

/*************************************************
  Function:       set_tcms_usb_debug
  Description:    设置tcms  usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:
*************************************************/
void set_tcms_usb_debug(void)
{
    if (tcms_debug_flag)
    {
        tcms_debug_flag = 0;
    }
    else
    {
        tcms_debug_flag = 1;
    }
}

/*************************************************
  Function:       set_pisc_usb_debug
  Description:    设置pisc usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:
*************************************************/
void set_pisc_usb_debug(void)
{
    if (pisc_debug_flag)
    {
        pisc_debug_flag = 0;
    }
    else
    {
        pisc_debug_flag = 1;
    }
}
/*************************************************
  Function:       set_tba_usb_debug
  Description:    设置tba usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:
*************************************************/
void set_tba_usb_debug(void)
{
    if (tba_debug_flag)
    {
        tba_debug_flag = 0;
    }
    else
    {
        tba_debug_flag = 1;
    }
}
/*************************************************
  Function:       tcms_usb_debug
  Description:    usb 打印tcms信息
  Input:            type 数据类型
                     buf 要打印的数据
                      len  数据长度
  Output:          无
  Return:         无
  Others:
*************************************************/
void tcms_usb_debug(uint8 type, uint8 *buf, uint16 len)
{
    char cdata[256] = {0};
    
    if (tcms_debug_flag)
    {
        if (HEX == type)
        {
            myitoa(buf, cdata, len);
            usb_write_buf((uint8 *)cdata, sizeof(cdata));
        }
        else
        {
            usb_write_buf(buf, len);
        }
    }
}
/*************************************************
  Function:       pisc_usb_debug
  Description:    usb 打印tcms信息
  Input:          type 数据类型
                     buf 要打印的数据
                      len  数据长度
  Output:          无
  Return:         无
  Others:
*************************************************/
void pisc_usb_debug(uint8 type, uint8 *buf, uint16 len)
{
    char cdata[256] = {0};
    
    if (pisc_debug_flag)
    {
        if (HEX == type)
        {
            myitoa(buf, cdata, len);
            usb_write_buf((uint8 *)cdata, sizeof(cdata));
        }
        else
        {
            usb_write_buf(buf, len);
        }
    }
}
/*************************************************
  Function:       tba_usb_debug
  Description:    usb 打印tcms信息
  Input:           type 数据类型
                        buf 要打印的数据
                      len  数据长度
  Output:          无
  Return:         无
  Others:
*************************************************/
void tba_usb_debug(uint8 type, uint8 *buf, uint16 len)
{
    char cdata[256] = {0};
    
    if (tba_debug_flag)
    {
        if (HEX == type)
        {
            myitoa(buf, cdata, len);
            usb_write_buf((uint8 *)cdata, sizeof(cdata));
        }
        else
        {
            usb_write_buf(buf, len);
        }
    }
}



#ifndef USB_H
#define USB_H

#include "config.h"

#define USB_BUF_LENGTH         512//128  
#define PTU_BUF_SIZE           256//64

#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符

#define HEX    1
#define CCHAR   0

extern uint8_t USB_Rx_Buffer[USB_BUF_LENGTH];
extern uint8_t USB_Tx_Buffer[USB_BUF_LENGTH];

extern uint16_t usb_recv_fifo_in;
extern uint16_t usb_recv_fifo_out;
extern uint16_t usb_send_fifo_in;
extern uint16_t usb_send_fifo_out;
extern uint8_t usb_read_char(uint8_t *ch);
extern void usb_write_buf(uint8_t *buf, uint16_t length);
extern void ptu_proc(void);

/*************************************************
  Function:       set_tcms_usb_debug
  Description:    设置tcms  usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:         
*************************************************/
void set_tcms_usb_debug(void);

/*************************************************
  Function:       set_pisc_usb_debug
  Description:    设置pisc usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:         
*************************************************/
void set_pisc_usb_debug(void);

/*************************************************
  Function:       set_tba_usb_debug
  Description:    设置tba usb打印功能
  Input:           debug  1 使能打印 0  禁用打印
  Output:          无
  Return:         无
  Others:         
*************************************************/
void set_tba_usb_debug(void);
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
void tcms_usb_debug(uint8 type, uint8 *buf, uint16 len);


/*************************************************
  Function:       pisc_usb_debug
  Description:    usb 打印tcms信息
  Input:             type 数据类型
                    buf 要打印的数据
                      len  数据长度
  Output:          无
  Return:         无
  Others:         
*************************************************/
void pisc_usb_debug(uint8 type,uint8 *buf, uint16 len);
/*************************************************
  Function:       tba_usb_debug
  Description:    usb 打印tcms信息
  Input:            type 数据类型
                    buf 要打印的数据
                      len  数据长度
  Output:          无
  Return:         无
  Others:         
*************************************************/
void tba_usb_debug(uint8 type,uint8 *buf, uint16 len);


#endif

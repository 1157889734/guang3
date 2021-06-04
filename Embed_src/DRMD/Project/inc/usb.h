#ifndef USB_H
#define USB_H

#define USB_BUF_LENGTH    512//128  
#define PTU_BUF_SIZE           256//64

#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符

extern uint8_t USB_Rx_Buffer[USB_BUF_LENGTH];
extern uint8_t USB_Tx_Buffer[USB_BUF_LENGTH];

extern uint16_t usb_recv_fifo_in;
extern uint16_t usb_recv_fifo_out;
extern uint16_t usb_send_fifo_in;
extern uint16_t usb_send_fifo_out;
extern uint8_t usb_read_char(uint8_t *ch);
extern void usb_write_buf(uint8_t *buf, uint16_t length);
extern void ptu_proc(void);
#endif

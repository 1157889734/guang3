#ifndef USB_H
#define USB_H

#define USB_BUF_LENGTH    512//128  
#define PTU_BUF_SIZE           256//64

#define TBA_PACKET_TAB			0x7E	//���ݰ��ָ��
#define TBA_PACKET_DLE			0x7F	//���ݰ�ȫ��ת���
#define TBA_PACKET_DLE_TAB		0x80	//���ݰ��ָ����ת���
#define TBA_PACKET_DLE_DLE		0x81	//���ݰ�ת�����ת���

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

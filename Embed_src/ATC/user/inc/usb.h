#ifndef USB_H
#define USB_H

#include "config.h"

#define USB_BUF_LENGTH         512//128  
#define PTU_BUF_SIZE           256//64

#define TBA_PACKET_TAB			0x7E	//���ݰ��ָ��
#define TBA_PACKET_DLE			0x7F	//���ݰ�ȫ��ת���
#define TBA_PACKET_DLE_TAB		0x80	//���ݰ��ָ����ת���
#define TBA_PACKET_DLE_DLE		0x81	//���ݰ�ת�����ת���

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
  Description:    ����tcms  usb��ӡ����
  Input:           debug  1 ʹ�ܴ�ӡ 0  ���ô�ӡ
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void set_tcms_usb_debug(void);

/*************************************************
  Function:       set_pisc_usb_debug
  Description:    ����pisc usb��ӡ����
  Input:           debug  1 ʹ�ܴ�ӡ 0  ���ô�ӡ
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void set_pisc_usb_debug(void);

/*************************************************
  Function:       set_tba_usb_debug
  Description:    ����tba usb��ӡ����
  Input:           debug  1 ʹ�ܴ�ӡ 0  ���ô�ӡ
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void set_tba_usb_debug(void);
/*************************************************
  Function:       tcms_usb_debug
  Description:    usb ��ӡtcms��Ϣ
  Input:            type ��������
                     buf Ҫ��ӡ������
                      len  ���ݳ���
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void tcms_usb_debug(uint8 type, uint8 *buf, uint16 len);


/*************************************************
  Function:       pisc_usb_debug
  Description:    usb ��ӡtcms��Ϣ
  Input:             type ��������
                    buf Ҫ��ӡ������
                      len  ���ݳ���
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void pisc_usb_debug(uint8 type,uint8 *buf, uint16 len);
/*************************************************
  Function:       tba_usb_debug
  Description:    usb ��ӡtcms��Ϣ
  Input:            type ��������
                    buf Ҫ��ӡ������
                      len  ���ݳ���
  Output:          ��
  Return:         ��
  Others:         
*************************************************/
void tba_usb_debug(uint8 type,uint8 *buf, uint16 len);


#endif

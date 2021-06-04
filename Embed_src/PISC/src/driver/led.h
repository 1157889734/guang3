//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ledָʾ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:led.h

//*�ļ�����:ledָʾ��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef LED_H
#define LED_H

#define LED_CTRL_REG		GPIO1_PD
#define LED1_CTRL_BIT		0	//����
#define LED2_CTRL_BIT		1	//����
#define LED3_CTRL_BIT		2	//TMSͨ��
#define LED4_CTRL_BIT		3	//TBA_BUSͨ��
#define LED5_CTRL_BIT		4	//OCC
#define LED6_CTRL_BIT		5	//MEDIAͨ��
#define LED7_CTRL_BIT		6	//USBͨ��
#define LED8_CTRL_BIT		7	//OCC��Դ
#define LED9_CTRL_BIT		8	//DVA��Դ
#define LED10_CTRL_BIT		9	//MEDIA��Դ	


#define MARST_LED_Pin			(1 << 0)
#define SLAVE_LED_Pin			(1 << 1)
#define TCMS_LED_Pin			(1 << 2)
#define TRAIN_LED_Pin			(1 << 3)
#define OCC_LED_Pin				(1 << 4)
#define MEDIA_LED_Pin			(1 << 5)
#define USB_LED_Pin				(1 << 6)
#define OCC_S_LED_Pin			(1 << 7)
#define DVA_S_LED_Pin			(1 << 8)
#define MEDIA_S_LED_Pin			(1 << 9)
//led����
#define LED_NUM		10

#define MARST_light				(GPIO1_PD |= (1 << 0))
#define MARST_dark				(GPIO1_PD &= ~(1 << 0))

#define SLAVE_light				(GPIO1_PD |= (1 << 1))
#define SLAVE_dark				(GPIO1_PD &= ~(1 << 1))

#define TCMS_light				(GPIO1_PD |= (1 << 2))
#define TCMS_dark				(GPIO1_PD &= ~(1 << 2))

#define TRAIN_light				(GPIO1_PD |= (1 << 3))
#define TRAIN_dark				(GPIO1_PD &= ~(1 << 3))

#define OCC_light					(GPIO1_PD |= (1 << 4))
#define OCC_dark					(GPIO1_PD &= ~(1 << 4))

#define MEDIA_light				(GPIO1_PD |= (1 << 5))
#define MEDIA_dark				(GPIO1_PD &= ~(1 << 5))

#define USB_light					(GPIO1_PD |= (1 << 6))
#define USB_dark					(GPIO1_PD &= ~(1 << 6))

#define OCC_SRC_light			(GPIO1_PD |= (1 << 7))
#define OCC_SRC_dark			(GPIO1_PD &= ~(1 << 7))

#define MP3_SRC_light 			(GPIO1_PD |= (1 << 8))
#define MP3_SRC_dark 			(GPIO1_PD &=~ (1 << 8))

#define MEDIA_SRC_light			(GPIO1_PD |= (1 << 9))
#define MEDIA_SRC_dark			(GPIO1_PD &= ~(1 << 9))

extern void led_init(void);
extern void led_onoff(uint8 led_no, uint8 onoff);
extern void led_toggle(uint8 led_no);
extern void led_blink(uint8 led_no, uint16 on_time, uint16 off_time);



#endif



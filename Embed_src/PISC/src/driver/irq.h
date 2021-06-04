//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&ȱʡ�жϴ���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:irq.c

//*�ļ�����:�������жϴ���

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef IRQ_H
#define IRQ_H



//�ж�������
//#define IRQ_T0TIMI		0
#define IRQ_FLASH		1
#define IRQ_RCCU		2
#define IRQ_RTC			3
#define IRQ_WDG			4
//#define IRQ_XTI		5
#define IRQ_USBHP		6
//#define IRQ_I2C0ITERR		7
//#define IRQ_I2C1ITERR		8
//#define IRQ_UART0		9
//#define IRQ_UART1		10
//#define IRQ_UART2		11
//#define IRQ_UART3		12
#define IRQ_SPI0		13
#define IRQ_SPI1		14
//#define IRQ_I2C0		15
//#define IRQ_I2C1		16
#define IRQ_CAN			17
#define IRQ_ADC			18
//#define IRQ_T1TIMI		19
#define IRQ_T2TIMI		20
#define IRQ_T3TIMI		21
#define IRQ_HDLC		25
//#define IRQ_USBLP		26
#define IRQ_T0TOI		29
#define IRQ_T0OC1		30
#define IRQ_T0OC2		31



extern void irq_init(void);



#endif



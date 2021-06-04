//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&��ʱ��0��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:timer0.h

//*�ļ�����:��ʱ��0��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TIMER0_H
#define TIMER0_H



//ȱʡ����
#define TIMER0_DEFAULT_MS	20



//�жϿ������Ĳ�
#define TIMER0_IRQ_SLOT			EIC_SIR0
//�ж�������
#define TIMER0_IRQ_VECTOR		0
//�ж����ȼ�
#define TIMER0_IRQ_PRIORITY		5	//���15



extern void timer0_init(uint16 ms);
extern uint8 timer0_check_timer(uint32 *timer, uint32 interval);
extern void timer0_mdelay(uint16 count);
extern void timer0_delay(uint16 count);
extern uint32 timer0_get_clock(void);
extern void timer0_dog_feed(void);



#endif



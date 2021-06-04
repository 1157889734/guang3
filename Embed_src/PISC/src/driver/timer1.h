//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&��ʱ��1��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:timer1.h

//*�ļ�����:��ʱ��1��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TIMER1_H
#define TIMER1_H



//�жϿ������Ĳ�
#define TIMER1_IRQ_SLOT			EIC_SIR19
//�ж�������
#define TIMER1_IRQ_VECTOR		19
//�ж����ȼ�
#define TIMER1_IRQ_PRIORITY		6	//���15



extern void timer1_init(uint16 ms);
extern void timer1_onoff(uint8 onoff);
extern void timer1_set_hook(void(*hook)(void));



#endif



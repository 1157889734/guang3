//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&定时器1驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:timer1.h

//*文件作用:定时器1驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TIMER1_H
#define TIMER1_H



//中断控制器的槽
#define TIMER1_IRQ_SLOT			EIC_SIR19
//中断向量号
#define TIMER1_IRQ_VECTOR		19
//中断优先级
#define TIMER1_IRQ_PRIORITY		6	//最高15



extern void timer1_init(uint16 ms);
extern void timer1_onoff(uint8 onoff);
extern void timer1_set_hook(void(*hook)(void));



#endif



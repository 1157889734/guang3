//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&定时器0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:timer0.h

//*文件作用:定时器0驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TIMER0_H
#define TIMER0_H



//缺省周期
#define TIMER0_DEFAULT_MS	20



//中断控制器的槽
#define TIMER0_IRQ_SLOT			EIC_SIR0
//中断向量号
#define TIMER0_IRQ_VECTOR		0
//中断优先级
#define TIMER0_IRQ_PRIORITY		5	//最高15



extern void timer0_init(uint16 ms);
extern uint8 timer0_check_timer(uint32 *timer, uint32 interval);
extern void timer0_mdelay(uint16 count);
extern void timer0_delay(uint16 count);
extern uint32 timer0_get_clock(void);
extern void timer0_dog_feed(void);



#endif



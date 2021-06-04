//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&串口1驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:uart1.c

//*文件作用:串口1驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo定义
static uint8 uart1_recv_fifo[UART1_FIFO_SIZE];
static uint16 uart1_recv_fifo_in=0;
static uint16 uart1_recv_fifo_out=0;
static uint8 uart1_send_fifo[UART1_FIFO_SIZE];
static uint16 uart1_send_fifo_in=0;
static uint16 uart1_send_fifo_out=0;
static uint16 uart1_recv_fifo_out_backup=0;
//硬件测试命令
static uint8 uart1_hardware_test_flag=0;
//extern uint32 bus_timer;
extern uint32 bus_timer_key ;
extern uint32 bus_timer_recon ;

extern uint32 timer0_clock;
uint8 test_uart1_irq_flag = 0;
uint16 MAX485_CTRL_REG_D = 0; 
//uint8 recv_data_flag_hd = 0;

//***********************************************************************************************************************
//函数作用:串口1中断服务程序
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void UART1_IRQHandler(void)
{
	uint16 status=UART1_SR;
	uint16 send_delay = 480;
	//接收FIFO半满(8个字节)---满8个字节触发中断提高接收的效率
	if(status&(0x0100|0x40))
	{
		test_uart1_irq_flag = 1;
		//15个波特率没有数据 就根据FIFO中剩下的数据触发中断 用于非8的整数倍的数据接收
		UART1_TOR=15;
		//把硬件FIFO读空 一边读一边送软件FIFO
		while(UART1_SR&0x01)
		{
			uart1_recv_fifo[uart1_recv_fifo_in++]=UART1_RxBUFR;
			if(uart1_recv_fifo_in>=UART1_FIFO_SIZE)
				uart1_recv_fifo_in=0;
		}
	}
	//数据发送完毕 继续向硬件FIFO装载数据
	if(status&(0x02)) //数据发送为空
	{
	test_uart1_irq_flag = 2;
		//没有数据要发送 关发送中断
		if(uart1_send_fifo_out==uart1_send_fifo_in)
		{
			test_uart1_irq_flag = 8;
			//sh9_tba_bus_dev.tba_send = 0;   //没有数据要发送
			
			UART1_IER&=~(0x02);
			while(send_delay--);
			//MAX485听状态
			//UART1_MAX485_CTRL_REG&=~(0x0001<<UART1_MAX485_CTRL_BIT);
			UART1_MAX485_CTRL_REG |= (0x0001<<UART1_MAX485_CTRL_BIT);
			//MAX485_CTRL_REG_D = UART1_MAX485_CTRL_REG;
			
			//MAX485_CTRL_REG_D |= (0x0001<<UART1_MAX485_CTRL_BIT);
			//UART1_MAX485_CTRL_REG = MAX485_CTRL_REG_D ;
			//UART1_MAX485_CTRL_REG = MAX485_CTRL_REG_D ;
			
			MAX485_CTRL_REG_D = UART1_MAX485_CTRL_REG;
				test_uart1_irq_flag = 9;
		}
		//有数据要发送 把硬件FIFO装满
		else
		{
		test_uart1_irq_flag = 6;
			while(UART1_SR&0x02)
			{
			test_uart1_irq_flag = 7;
				UART1_TxBUFR=uart1_send_fifo[uart1_send_fifo_out++];
				if(uart1_send_fifo_out>=UART1_FIFO_SIZE)
					uart1_send_fifo_out=0;
				if(uart1_send_fifo_out==uart1_send_fifo_in)
					break;//判断是否还有数据
			}
		}
	}
}

//***********************************************************************************************************************
//函数作用:串口初始化
//参数说明:波特率
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart1_init(uint32 bps)
{
	//注册串口中断
	//UART1_IRQ_SLOT=(uint32)UART1_HANDLER;//设置中断处理句柄地址---在IRQ.S中已经完成
	UART1_IRQ_SLOT=(UART1_IRQ_SLOT&0xFFFF0000)|UART1_IRQ_PRIORITY;//设置优先级--最高15
	EIC_IER|=1<<UART1_IRQ_VECTOR;//使能中断控制器相应的槽
	
	//设置管脚属性为UART1
	GPIO0_PC0|=(0x0001<<11);
	GPIO0_PC1|=(0x0001<<11);
	GPIO0_PC2|=(0x0001<<11);
	GPIO0_PC0&=~(0x0001<<10);
	GPIO0_PC1|=(0x0001<<10);
	GPIO0_PC2&=~(0x0001<<10);
	//配置MAX485控制管脚属性为输出+上拉
/*	GPIO1_PC0|=(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO1_PC1&=~(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO1_PC2|=(0x0001<<UART1_MAX485_CTRL_BIT);
*/
			
	GPIO0_PC0 |= (0x0001<<UART1_MAX485_CTRL_BIT);	// 初始化EN1483B，使其上电后为低电平
	GPIO0_PC1 &=~(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO0_PC2 |= (0x0001<<UART1_MAX485_CTRL_BIT);
	
	//MAX485听状态
	UART1_MAX485_CTRL_REG |= (0x0001<<UART1_MAX485_CTRL_BIT);		
	//清除所有寄存器
	UART1_CR=0x0000;
	UART1_IER=0x0000;
	(void)UART1_RxBUFR;
	UART1_RxRSTR=0xFFFF;
	UART1_TxRSTR=0xFFFF;
	UART1_TOR=0x0000;
	UART1_GTR=0x0000;
	//设置寄存器
	UART1_CR|=0x0080;//开串口
	UART1_CR|=0x0400;//使用硬件FIFO
	UART1_RxRSTR=0xFFFF;//清硬件接收FIFO
	UART1_TxRSTR=0xFFFF;//清硬件发送FIFO
	UART1_CR&=~0x0040;//关闭本地回环
	UART1_TOR=15;//15个波特率没有数据 就根据FIFO中剩下的数据触发中断 用于非8的整数倍的数据接收
	UART1_BR=(uint16)(Fpcl/(16*bps));//波特率
	UART1_CR=(UART1_CR&0xFFDF)|0;//校验 N
	UART1_CR=(UART1_CR&0xFFF8)|0x01;//数据位 8
	UART1_CR=(UART1_CR&0xFFE7)|0x08;//停止位 1
	UART1_CR|=0x0100;//启动接收
	//开串口中断
	UART1_IER=0x0100|0x40;//满8个字节触发中断提高接收的效率 15个波特率没有数据 就根据FIFO中剩下的数据触发中断
	UART1_IER|=(0x02);//硬件FIFO发送一半的时候或发送完毕的时候---触发中断继续向硬件FIFO装载数据
}

//***********************************************************************************************************************
//函数作用:查询串口 如果有数据读取一个字节
//参数说明:ch---读取字符存放地址
//注意事项:
//返回说明:如果有数据返回1
//***********************************************************************************************************************
uint8 uart1_read_char(uint8 *ch)
{
	if(uart1_recv_fifo_out==uart1_recv_fifo_in)
		return 0;
	*ch=uart1_recv_fifo[uart1_recv_fifo_out];//得到当前要读取的字符
	uart1_recv_fifo_out++;//指向下一个要读取的字符
	if(uart1_recv_fifo_out==UART1_FIFO_SIZE)
		uart1_recv_fifo_out=0;//如果到了fifo末尾 则重从头开始
	
	//判断硬件测试命令
	if(*ch==HARDWARE_TEST_CMD)
	{
		uart1_hardware_test_flag++;
		if(uart1_hardware_test_flag>=200)
			uart1_hardware_test_flag=200;
	}
	else
	{
		uart1_hardware_test_flag=0;
	}	
	return 1;
}

//***********************************************************************************************************************
//函数作用:串口发送一个字节
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart1_write_char(uint8 ch)
{	
	//测试缓冲区是否满
	uint16 fifo_in_test=uart1_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=UART1_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==uart1_send_fifo_out)
		return;
	//向FIFO填充数据
	uart1_send_fifo[uart1_send_fifo_in++]=ch;
	if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
		uart1_send_fifo_in=0;
	test_uart1_irq_flag = 5;
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART1_SR&0x02) 就会进入中断自动发送
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485发送状态
	UART1_IER|=(0x02);
}

//***********************************************************************************************************************
//函数作用:串口发送定长数据
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 uart1_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=uart1_send_fifo_in;
	while(length--)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=uart1_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART1_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart1_send_fifo_out)
		{
			//缓冲区满恢复发送之前的指针
			uart1_send_fifo_in=send_fifo_in_backup;
			UART1_IER|=(0x02);
			return 0;
		}
		//向FIFO填充数据
		uart1_send_fifo[uart1_send_fifo_in++]=*buf++;
		if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
			uart1_send_fifo_in=0;
	}
	test_uart1_irq_flag = 4;

	if(sh9_tba_bus_dev.tba_send)
		return 0;
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART1_SR&0x02) 就会进入中断自动发送
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485发送状态//MAX485发送状态	
	UART1_IER|=(0x02);
	
	return 1;
}

//***********************************************************************************************************************
//函数作用:串口发送字符串
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart1_write_str(char *str)
{
	while(*str)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=uart1_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART1_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart1_send_fifo_out)
			break;
		//向FIFO填充数据
		uart1_send_fifo[uart1_send_fifo_in++]=*str++;
		if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
			uart1_send_fifo_in=0;
	}
	test_uart1_irq_flag = 3;
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART1_SR&0x02) 就会进入中断自动发送
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485发送状态//MAX485发送状态
	UART1_IER|=(0x02);
}

//***********************************************************************************************************************
//函数作用:获取硬件测试命令
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 uart1_get_hardware_test(void)
{
	if(uart1_hardware_test_flag==60)
	{
		uart1_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}


//***********************************************************************************************************************
//函数作用:保存fifo读指针
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart1_fifo_out_backup(void)
{
	uart1_recv_fifo_out_backup=uart1_recv_fifo_out;
}

//***********************************************************************************************************************
//函数作用:恢复fifo读指针
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart1_fifo_out_resume(void)
{
	uart1_recv_fifo_out=uart1_recv_fifo_out_backup;
}

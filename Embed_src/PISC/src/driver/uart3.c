//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&串口3驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:uart3.c

//*文件作用:串口3驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo定义
static uint8 uart3_recv_fifo[UART3_FIFO_SIZE];
static uint16 uart3_recv_fifo_in=0;
static uint16 uart3_recv_fifo_out=0;
static uint8 uart3_send_fifo[UART3_FIFO_SIZE];
static uint16 uart3_send_fifo_in=0;
static uint16 uart3_send_fifo_out=0;

static uint16 uart3_recv_fifo_out_backup=0;

//硬件测试命令
static uint8 uart3_hardware_test_flag=0;


//***********************************************************************************************************************
//函数作用:串口3中断服务程序
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void UART3_IRQHandler(void)
{
	uint16 status=UART3_SR;
	
	//接收FIFO半满(8个字节)---满8个字节触发中断提高接收的效率
	if(status& (0x0100|0x40))
	{
		//30个波特率没有数据 就根据FIFO中剩下的数据触发中断 用于非8的整数倍的数据接收
		UART3_TOR=30;
		//把硬件FIFO读空 一边读一边送软件FIFO
		while(UART3_SR&0x01)
		{
			uart3_recv_fifo[uart3_recv_fifo_in++]=UART3_RxBUFR;
			if(uart3_recv_fifo_in>=UART3_FIFO_SIZE)
				uart3_recv_fifo_in=0;
		}
	}
	//数据发送完毕 继续向硬件FIFO装载数据	
	if(status&(0x02))
	{
		//没有数据要发送 关发送中断
		if(uart3_send_fifo_out==uart3_send_fifo_in)
		{
			UART3_IER&=~(0x02);
			//MAX485听状态
			#ifdef UART3_MAX485_CTRL_REG
			UART3_MAX485_CTRL_REG&=~(0x0001<<UART3_MAX485_CTRL_BIT);
			#endif
		}
		//有数据要发送 把硬件FIFO装满
		else
		{
			while(UART3_SR&0x02)
			{
				UART3_TxBUFR=uart3_send_fifo[uart3_send_fifo_out++];
				if(uart3_send_fifo_out>=UART3_FIFO_SIZE)
					uart3_send_fifo_out=0;
				if(uart3_send_fifo_out==uart3_send_fifo_in)
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
void uart3_init(uint32 bps)
{
	//注册串口中断
	//UART3_IRQ_SLOT=(uint32)UART3_HANDLER;//设置中断处理句柄地址---在IRQ.S中已经完成
	UART3_IRQ_SLOT=(UART3_IRQ_SLOT&0xFFFF0000)|UART3_IRQ_PRIORITY;//设置优先级--最高15
	EIC_IER|=1<<UART3_IRQ_VECTOR;//使能中断控制器相应的槽
	
	//设置管脚属性为UART3
	GPIO0_PC0|=(0x0001<<0);
	GPIO0_PC1|=(0x0001<<0);
	GPIO0_PC2|=(0x0001<<0);
	GPIO0_PC0&=~(0x0001<<1);
	GPIO0_PC1|=(0x0001<<1);
	GPIO0_PC2&=~(0x0001<<1);
	//配置MAX485控制管脚属性为输出+上拉
	#ifdef UART3_MAX485_CTRL_REG
	GPIO2_PC0|=(0x0001<<UART3_MAX485_CTRL_BIT);
	GPIO2_PC1&=~(0x0001<<UART3_MAX485_CTRL_BIT);
	GPIO2_PC2|=(0x0001<<UART3_MAX485_CTRL_BIT);
	#endif
	
	//清除所有寄存器
	UART3_CR=0x0000;
	UART3_IER=0x0000;
	(void)UART3_RxBUFR;
	UART3_RxRSTR=0xFFFF;
	UART3_TxRSTR=0xFFFF;
	UART3_TOR=0x0000;
	UART3_GTR=0x0000;
	//设置寄存器
	UART3_CR|=0x0080;//开串口
	UART3_CR|=0x0400;//使用硬件FIFO
	UART3_RxRSTR=0xFFFF;//清硬件接收FIFO
	UART3_TxRSTR=0xFFFF;//清硬件发送FIFO
	UART3_CR&=~0x0040;//关闭本地回环
	UART3_TOR=30;//30个波特率没有数据 就根据FIFO中剩下的数据触发中断 用于非8的整数倍的数据接收
	UART3_BR=(uint16)(Fpcl/(16*bps));//波特率
	UART3_CR=(UART3_CR&0xFFDF)|0;//校验 N
	UART3_CR=(UART3_CR&0xFFF8)|0x01;//数据位 8
	UART3_CR=(UART3_CR&0xFFE7)|0x08;//停止位 1
	UART3_CR|=0x0100;//启动接收
	//开串口中断
	UART3_IER=0x0100|0x40;//满8个字节触发中断提高接收的效率 30个波特率没有数据 就根据FIFO中剩下的数据触发中断
	UART3_IER|=(0x02);//硬件FIFO发送一半的时候或发送完毕的时候---触发中断继续向硬件FIFO装载数据
	
	//MAX485听状态
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG&=~(0x0001<<UART3_MAX485_CTRL_BIT);
	#endif
	uart3_recv_fifo_out=uart3_recv_fifo_in=0;
}

//***********************************************************************************************************************
//函数作用:查询串口 如果有数据读取一个字节
//参数说明:ch---读取字符存放地址
//注意事项:
//返回说明:如果有数据返回1
//***********************************************************************************************************************
uint8 uart3_read_char(uint8 *ch)
{
	if(uart3_recv_fifo_out==uart3_recv_fifo_in)
		return 0;
	*ch=uart3_recv_fifo[uart3_recv_fifo_out];//得到当前要读取的字符
	uart3_recv_fifo_out++;//指向下一个要读取的字符
	if(uart3_recv_fifo_out==UART3_FIFO_SIZE)
		uart3_recv_fifo_out=0;//如果到了fifo末尾 则重从头开始
	
	//判断硬件测试命令
	if(*ch==HARDWARE_TEST_CMD)
	{
		uart3_hardware_test_flag++;
		if(uart3_hardware_test_flag>=200)
			uart3_hardware_test_flag=200;
	}
	else
	{
		uart3_hardware_test_flag=0;
	}	
	return 1;
}

//***********************************************************************************************************************
//函数作用:串口发送一个字节
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart3_write_char(uint8 ch)
{	
	//测试缓冲区是否满
	uint16 fifo_in_test=uart3_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=UART3_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==uart3_send_fifo_out)
		return;
	//向FIFO填充数据
	uart3_send_fifo[uart3_send_fifo_in++]=ch;
	if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
		uart3_send_fifo_in=0;
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART3_SR&0x02) 就会进入中断自动发送
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485发送状态
	#endif
	UART3_IER|=(0x02);
}

//***********************************************************************************************************************
//函数作用:串口发送定长数据
//参数说明:
//注意事项:
//返回说明:缓冲区满返回0
//***********************************************************************************************************************
uint8 uart3_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=uart3_send_fifo_in;
	
	while(length--)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=uart3_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART3_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart3_send_fifo_out)
		{
			//缓冲区满恢复发送之前的指针
			uart3_send_fifo_in=send_fifo_in_backup;
			UART3_IER|=(0x02);
			return 0;
		}
		//向FIFO填充数据
		uart3_send_fifo[uart3_send_fifo_in++]=*buf++;
		if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
			uart3_send_fifo_in=0;
	}
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART3_SR&0x02) 就会进入中断自动发送
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485发送状态	
	#endif
	UART3_IER|=(0x02);
	
	return 1;
}

//***********************************************************************************************************************
//函数作用:串口发送字符串
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart3_write_str(char *str)
{
	while(*str)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=uart3_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART3_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart3_send_fifo_out)
			break;
		//向FIFO填充数据
		uart3_send_fifo[uart3_send_fifo_in++]=*str++;
		if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
			uart3_send_fifo_in=0;
	}
	//开发送中断 利用中断发送 如果这时候硬件发送FIFO有空位置(UART3_SR&0x02) 就会进入中断自动发送
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485发送状态	
	#endif
	UART3_IER|=(0x02);
}

//***********************************************************************************************************************
//函数作用:保存fifo读指针
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart3_fifo_out_backup(void)
{
	uart3_recv_fifo_out_backup=uart3_recv_fifo_out;
}

//***********************************************************************************************************************
//函数作用:恢复fifo读指针
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void uart3_fifo_out_resume(void)
{
	uart3_recv_fifo_out=uart3_recv_fifo_out_backup;
}

//***********************************************************************************************************************
//函数作用:获取硬件测试命令
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 uart3_get_hardware_test(void)
{
	if(uart3_hardware_test_flag==60)
	{
		uart3_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}



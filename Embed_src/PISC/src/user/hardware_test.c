//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 硬件测试处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:hardware_test.c

//*文件作用:硬件测试处理

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"




//硬件测试
static void io_set(uint8 index, uint8 level)
{
	if(index==0)//D1
	{
		
		GPIO1_PC0|=(1<<0);GPIO1_PC1&=~(1<<0);GPIO1_PC2|=(1<<0);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<0);
		else GPIO1_PD&=~(1<<0);
	}
	else if(index==1)//D2
	{
		
		GPIO1_PC0|=(1<<1);GPIO1_PC1&=~(1<<1);GPIO1_PC2|=(1<<1);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<1);
		else GPIO1_PD&=~(1<<1);
	}
	else if(index==2)//D3
	{
		
		GPIO1_PC0|=(1<<2);GPIO1_PC1&=~(1<<2);GPIO1_PC2|=(1<<2);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<2);
		else GPIO1_PD&=~(1<<2);
	}
	else if(index==3)//D4
	{
		
		GPIO1_PC0|=(1<<3);GPIO1_PC1&=~(1<<3);GPIO1_PC2|=(1<<3);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<3);
		else GPIO1_PD&=~(1<<3);
	}
	
	else if(index==4)//D5
	{
		
		GPIO1_PC0|=(1<<4);GPIO1_PC1&=~(1<<4);GPIO1_PC2|=(1<<4);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<4);
		else GPIO1_PD&=~(1<<4);
	}
	else if(index==5)//D6
	{
		
		GPIO1_PC0|=(1<<5);GPIO1_PC1&=~(1<<5);GPIO1_PC2|=(1<<5);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<5);
		else GPIO1_PD&=~(1<<5);
	}
	else if(index==6)//D7
	{
		
		GPIO1_PC0|=(1<<6);GPIO1_PC1&=~(1<<6);GPIO1_PC2|=(1<<6);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<6);
		else GPIO1_PD&=~(1<<6);
	}
	else if(index==7)//D8
	{
		
		GPIO1_PC0|=(1<<7);GPIO1_PC1&=~(1<<7);GPIO1_PC2|=(1<<7);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<7);
		else GPIO1_PD&=~(1<<7);
	}
	else if(index==8)//D9
	{
		
		GPIO1_PC0|=(1<<11);GPIO1_PC1&=~(1<<11);GPIO1_PC2|=(1<<11);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<11);
		else GPIO1_PD&=~(1<<11);
	}
	else if(index==9)//D10
	{
		
		GPIO1_PC0|=(1<<9);GPIO1_PC1&=~(1<<9);GPIO1_PC2|=(1<<9);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<9);
		else GPIO1_PD&=~(1<<9);
	}
	else if(index==10)//UPDOWN
	{
		
		GPIO2_PC0|=(1<<7);GPIO2_PC1&=~(1<<7);GPIO2_PC2|=(1<<7);//设置管脚属性为输出
		if(level)GPIO2_PD|=(1<<7);
		else GPIO2_PD&=~(1<<7);
	}
	else if(index==11)//CHECK
	{
		
		GPIO2_PC0|=(1<<11);GPIO2_PC1&=~(1<<11);GPIO2_PC2|=(1<<11);//设置管脚属性为输出
		if(level)GPIO2_PD|=(1<<11);
		else GPIO2_PD&=~(1<<11);
	}
	else if(index==12)//PLAY
	{
		
		GPIO2_PC0|=(1<<9);GPIO2_PC1&=~(1<<9);GPIO2_PC2|=(1<<9);//设置管脚属性为输出
		if(level)GPIO2_PD|=(1<<9);
		else GPIO2_PD&=~(1<<9);
	}
	else if(index==13)//STOP
	{
		
		GPIO2_PC0|=(1<<10);GPIO2_PC1&=~(1<<10);GPIO2_PC2|=(1<<10);//设置管脚属性为输出
		if(level)GPIO2_PD|=(1<<10);
		else GPIO2_PD&=~(1<<10);
	}
	else if(index==14)//PA_CTRL
	{
		
		GPIO1_PC0|=(1<<15);GPIO1_PC1&=~(1<<15);GPIO1_PC2|=(1<<15);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<15);
		else GPIO1_PD&=~(1<<15);
	}
	else if(index==15)//PA_ON
	{
		
		GPIO1_PC0|=(1<<12);GPIO1_PC1&=~(1<<12);GPIO1_PC2|=(1<<12);//设置管脚属性为输出
		if(level)GPIO1_PD|=(1<<12);
		else GPIO1_PD&=~(1<<12);
	}
}
static uint8 io_get(uint8 index)
{
	if(index==0)//T/W
	{
		GPIO2_PC0|=(1<<1);GPIO2_PC1|=(1<<1);GPIO2_PC2&=~(1<<1);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<1))return 1;
		return 0;
	}
	else if(index==1)//PLAYING
	{
		GPIO2_PC0|=(1<<6);GPIO2_PC1|=(1<<6);GPIO2_PC2&=~(1<<6);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<6))return 1;
		return 0;
	}
	else if(index==2)//NOFILE
	{
		GPIO2_PC0|=(1<<5);GPIO2_PC1|=(1<<5);GPIO2_PC2&=~(1<<5);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<5))return 1;
		return 0;
	}
	else if(index==3)//OPEN_DOOR1
	{
		GPIO2_PC0|=(1<<2);GPIO2_PC1|=(1<<2);GPIO2_PC2&=~(1<<2);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<2))return 1;
		return 0;
	}
	else if(index==4)//CLOSE_DOOR
	{
		GPIO2_PC0|=(1<<3);GPIO2_PC1|=(1<<3);GPIO2_PC2&=~(1<<3);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<3))return 1;
		return 0;
	}
	else if(index==5)//OCC_IN
	{
		GPIO2_PC0|=(1<<4);GPIO2_PC1|=(1<<4);GPIO2_PC2&=~(1<<4);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<4))return 1;
		return 0;
	}		
	else if(index==6)//OPEN_DOOR2
	{
		GPIO0_PC0|=(1<<6);GPIO0_PC1|=(1<<6);GPIO0_PC2&=~(1<<6);//配置管脚属性为输入+弱上拉
		if(GPIO0_PD&(1<<6))return 1;
		return 0;
	}
	else if(index==7)//0KM
	{
		GPIO0_PC0|=(1<<7);GPIO0_PC1|=(1<<7);GPIO0_PC2&=~(1<<7);//配置管脚属性为输入+弱上拉
		if(GPIO0_PD&(1<<7))return 1;
		return 0;
	}
	else if(index==8)//PA_LINE
	{
		GPIO2_PC0|=(1<<0);GPIO2_PC1|=(1<<0);GPIO2_PC2&=~(1<<0);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<0))return 1;
		return 0;
	}
	else if(index==9)//APU_CHECK
	{
		GPIO0_PC0|=(1<<4);GPIO0_PC1|=(1<<4);GPIO0_PC2&=~(1<<4);//配置管脚属性为输入+弱上拉
		if(GPIO0_PD&(1<<4))return 1;
		return 0;
	}	
	else if(index==10)//DVA_CHECK
	{
		GPIO0_PC0|=(1<<5);GPIO0_PC1|=(1<<5);GPIO0_PC2&=~(1<<5);//配置管脚属性为输入+弱上拉
		if(GPIO0_PD&(1<<5))return 1;
		return 0;
	}			
	else if(index==11)//DVA_PLAYING
	{
		GPIO2_PC0|=(1<<12);GPIO2_PC1|=(1<<12);GPIO2_PC2&=~(1<<12);//配置管脚属性为输入+弱上拉
		if(GPIO2_PD&(1<<12))return 1;
		return 0;
	}
	
					
	return 0;	
}
#define CMD_RS232_1		0x01
#define CMD_RS485_1		0x02
#define CMD_RS232_2		0x03
#define CMD_RS485_2		0x04
#define CMD_I2C			0x05
#define CMD_USB			0x06
//static uint8 code cmd_out_high[]={0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0};
//static uint8 code cmd_out_low []={0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0};
//static uint8 code cmd_in_high[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0};
//static uint8 code cmd_in_low []={0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0};
static void test_command(uint8 ch, uint8 uart_no)
{	
	//I2C通信
	 if(ch==CMD_I2C)
	 {	
	 	//设置一个输出一个输入 并且初始化为高电平
		GPIO0_PC0|=(1<<I2C1_SCL_BIT);GPIO0_PC1&=~(1<<I2C1_SCL_BIT);GPIO0_PC2|=(1<<I2C1_SCL_BIT);//设置管脚属性为输出
		GPIO0_PC0|=(1<<I2C1_SDA_BIT);GPIO0_PC1|=(1<<I2C1_SDA_BIT);GPIO0_PC2&=~(1<<I2C1_SDA_BIT);//配置管脚属性为输入+弱上拉
		I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);//送高电平
		I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);//送高电平
		
		//电平自环测试
		I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);//送低电平
		if((I2C1_SDA_REG&(1<<I2C1_SDA_BIT))==0)//检查是否读到低电平
		{
			I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);//送高电平
			if(I2C1_SDA_REG&(1<<I2C1_SDA_BIT))//检查是否读到高电平
			{
				if(uart_no==0)uart0_write_char(CMD_I2C|0x80);
				else if(uart_no==1)uart1_write_char(CMD_I2C|0x80);
				else if(uart_no==2)uart2_write_char(CMD_I2C|0x80);
				else if(uart_no==3)uart3_write_char(CMD_I2C|0x80);				
			}
		}
		//重新初始化ic21
		i2c1_init(APU_BAUDRATE);
	}
	//USB通信
	else if(ch==CMD_USB)
	{
		if(usb_get_connect_state())
		{
			if(uart_no==0)uart0_write_char(CMD_USB|0x80);
			else if(uart_no==1)uart1_write_char(CMD_USB|0x80);
			else if(uart_no==2)uart2_write_char(CMD_USB|0x80);
			else if(uart_no==3)uart3_write_char(CMD_USB|0x80);
		}
	}
	//I/O口输出高
	else if(ch>=0x10 && ch<0x20)
	{
		io_set(ch-0x10,1);
	}
	//I/O口输出低
	else if(ch>=0x20 && ch<0x30)
	{
		io_set(ch-0x20,0);
	}
	
	//I/O口输入高
	else if(ch>=0x30 && ch<0x40)
	{
		if(io_get(ch-0x30))
		{
			if(uart_no==0)uart0_write_char(ch|0x80);
			else if(uart_no==1)uart1_write_char(ch|0x80);
			else if(uart_no==2)uart2_write_char(ch|0x80);
			else if(uart_no==3)uart3_write_char(ch|0x80);	
		}
	}
	//I/O口输入低
	else if(ch>=0x40 && ch<0x50)
	{
		if(io_get(ch-0x40)==0)
		{
			if(uart_no==0)uart0_write_char(ch|0x80);
			else if(uart_no==1)uart1_write_char(ch|0x80);
			else if(uart_no==2)uart2_write_char(ch|0x80);
			else if(uart_no==3)uart3_write_char(ch|0x80);
		}
	}	
}
static void hardtest(void)
{
	uint32 test_time=6000000;
	uint8 temp;
	
	print_line("HARDWARE TEST");
	
	led_init();
	while(test_time--)
	{
		dog();
		
		//接收4个串口的命令
		if(uart0_read_char(&temp))
		{
			//串口0通信
			if(temp==CMD_RS232_1)uart0_write_char(CMD_RS232_1|0x80);
			//串口0其他命令
			else test_command(temp,0);
		}			
		if(uart1_read_char(&temp))
		{
			//串口1通信
			if(temp==CMD_RS485_2)uart1_write_char(CMD_RS485_2|0x80);
			//串口1其他命令
			else test_command(temp,1);
		}
 		if(uart2_read_char(&temp))
		{
			//串口2通信
			if(temp==CMD_RS485_1)uart2_write_char(CMD_RS485_1|0x80);
			//串口2其他命令
			else test_command(temp,2);
		}
 		if(uart3_read_char(&temp))
		{
			//串口3通信
			if(temp==CMD_RS232_2)uart3_write_char(CMD_RS232_2|0x80);
			//串口3其他命令
			else test_command(temp,3);
		}
		
		//面板指示灯
		if((test_time%100000)==0)led_toggle(0);
		else if((test_time%100000)==10000)led_toggle(1);
		else if((test_time%100000)==20000)led_toggle(2);
		else if((test_time%100000)==30000)led_toggle(3);
		else if((test_time%100000)==40000)led_toggle(4);
		else if((test_time%100000)==50000)led_toggle(5);
		else if((test_time%100000)==60000)led_toggle(6);
		else if((test_time%100000)==70000)led_toggle(7);
		else if((test_time%100000)==80000)led_toggle(8);
		else if((test_time%100000)==90000)led_toggle(9);
	}
	print_line("EXIT TEST");
}

//***********************************************************************************************************************
//函数作用:硬件测试处理
//参数说明:无
//注意事项:
//返回说明:
//***********************************************************************************************************************
void hardware_test_proc(void)
{
	if(uart0_get_hardware_test() || uart1_get_hardware_test() 
	|| uart2_get_hardware_test() || uart3_get_hardware_test())
	{
		hardtest();
	}	
}



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&i2c0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:i2c0.c

//*文件作用:i2c0驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"


#define I2C0_WAIT_TIME		40


//&&&&&&&&&&&&&&&&&&&&&&&&I2C专用函数&&&&&&&&&&&&&&&&&&&&&&&&&&

//*****************************************
//启动I2C
//*****************************************
static void I2C0_Start(void)
{
	uint16 wait;
	
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	
	I2C0_SDA_REG&=~(1<<I2C0_SDA_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	
	I2C0_SCL_REG&=~(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME/3;wait++);
}

//*****************************************
//停止I2C
//*****************************************
static void I2C0_Stop(void)
{
	uint16 wait;
	
	I2C0_SDA_REG&=~(1<<I2C0_SDA_BIT);
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	
	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
}

//*******************************************
//I2C发送一个字节
//*******************************************
void I2C0_SendByte(uint8 ch)
{
	uint16 wait;
	uint8 i;
	
	for(i=0;i<8;i++)
	{
		I2C0_SCL_REG&=~(1<<I2C0_SCL_BIT);
		for(wait=0;wait<I2C0_WAIT_TIME/3;wait++);
		if(ch&0x80)I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
		else I2C0_SDA_REG&=~(1<<I2C0_SDA_BIT);
		ch<<=1;
		for(wait=0;wait<I2C0_WAIT_TIME;wait++);
		I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
		for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	}
	I2C0_SCL_REG&=~(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME/3;wait++);
}

//****************************************
//I2C主器件等待应答
//****************************************
uint8 WaitAtk_i2c0(void)
{
       uint8 ack1;
	 uint8 ack2;
	uint16 wait;//因故障接收方无ACK，超时值为255。

	GPIO1_PC0|=(1<<I2C0_SDA_BIT);
	GPIO1_PC1|=(1<<I2C0_SDA_BIT);
	GPIO1_PC2&=~(1<<I2C0_SDA_BIT);

	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);

	ack1 = (I2C0_SDA_REG >> I2C0_SDA_BIT)&0x01;
	//等待应答
	for(wait=0;wait<200;wait++);
	ack2 = (I2C0_SDA_REG >> I2C0_SDA_BIT)&0x01;
	if((ack1 == 1) && (ack1 == ack2))
	{
	    ptu_print_debug("no ack");
	    apu_error++;
	    if(apu_error >= 40)
		apu_error = 40;
	}
	else
	{
	    apu_error = 0; 
	}
	//wait=2500;
	//while(SDA)
	//{
	//	wait--; 
	//	if(!wait)
	//	{
	//		I2C_Stop();
	//		return 0;
	//	}
	//}
	I2C0_SCL_REG&=~(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME/3;wait++);

	GPIO1_PC0|=(1<<I2C0_SDA_BIT);
	GPIO1_PC1&=~(1<<I2C0_SDA_BIT);
	GPIO1_PC2|=(1<<I2C0_SDA_BIT);
	return 1;
}

//***********************************************************************************************************************
//函数作用:i2c1初始化
//参数说明:波特率
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void i2c0_init(uint32 bps)
{
	//设置管脚属性为输出
	GPIO1_PC0|=(1<<I2C0_SCL_BIT);
	GPIO1_PC1&=~(1<<I2C0_SCL_BIT);
	GPIO1_PC2|=(1<<I2C0_SCL_BIT);
	GPIO1_PC0|=(1<<I2C0_SDA_BIT);
	GPIO1_PC1&=~(1<<I2C0_SDA_BIT);
	GPIO1_PC2|=(1<<I2C0_SDA_BIT);
	//送高电平
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
}

//***********************************************************************************************************************
//函数作用:i2c1发送定长数据
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void i2c0_write_buf(uint8 *buf, uint16 length)
{	
	I2C0_Start();//启动总线
	
	I2C0_SendByte(I2C0_DEVICE_ADDRESS);//送从器件地址
	WaitAtk_i2c0();
	//发送数据
	while(length--)
	{
		I2C0_SendByte(*buf++);
		WaitAtk_i2c0();		
	}
	
	I2C0_Stop();//停止总线
	
	//return 1;
}




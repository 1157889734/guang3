//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&i2c1驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:i2c1.c

//*文件作用:i2c1驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



#define I2C1_WAIT_TIME		20


//&&&&&&&&&&&&&&&&&&&&&&&&I2C专用函数&&&&&&&&&&&&&&&&&&&&&&&&&&

//*****************************************
//启动I2C
//*****************************************
void I2C_Start(void)
{
	uint16 wait;
	
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	
	I2C1_SDA_REG&=~(1<<I2C1_SDA_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	
	I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME/3;wait++);
}

//*****************************************
//停止I2C
//*****************************************
void I2C_Stop(void)
{
	uint16 wait;
	
	I2C1_SDA_REG&=~(1<<I2C1_SDA_BIT);
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	
	I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
}

//*******************************************
//I2C发送一个字节
//*******************************************
void I2C_SendByte(uint8 ch)
{
	uint16 wait;
	uint8 i;
	
	for(i=0;i<8;i++)
	{
		I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);
		for(wait=0;wait<I2C1_WAIT_TIME/3;wait++);
		if(ch&0x80)
			I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
		else 
			I2C1_SDA_REG&=~(1<<I2C1_SDA_BIT);
		ch<<=1;
		for(wait=0;wait<I2C1_WAIT_TIME;wait++);

		I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);

		for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	}
	I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME/3;wait++);
}

//****************************************
//I2C主器件等待应答
//****************************************
uint8 WaitAtk(uint8 tmp)
{
	uint16 wait;//因故障接收方无ACK，超时值为255。

      if(tmp)
		I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
	else
		I2C1_SDA_REG&=~(1<<I2C1_SDA_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	
	//等待应答
	for(wait=0;wait<1000;wait++);
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
	I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME/3;wait++);
	return 1;
}

//***********************************************************************************************************************
//函数作用:i2c1初始化
//参数说明:波特率
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void i2c1_init(uint32 bps)
{
	//设置管脚属性为输出
/*	GPIO0_PC0&=~(1<<I2C1_SCL_BIT);
	GPIO0_PC1|=(1<<I2C1_SCL_BIT);
	GPIO0_PC2|=(1<<I2C1_SCL_BIT);
	
	GPIO0_PC0&=~(1<<I2C1_SDA_BIT);
	GPIO0_PC1|=(1<<I2C1_SDA_BIT);
	GPIO0_PC2|=(1<<I2C1_SDA_BIT);
*/

	GPIO0_PC0|=(1<<I2C1_SCL_BIT);
	GPIO0_PC1&=~(1<<I2C1_SCL_BIT);
	GPIO0_PC2|=(1<<I2C1_SCL_BIT);
	
	GPIO0_PC0|=(1<<I2C1_SDA_BIT);  //out put
	GPIO0_PC1&=~(1<<I2C1_SDA_BIT);
	GPIO0_PC2|=(1<<I2C1_SDA_BIT);


	
	//送高电平
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
}

//***********************************************************************************************************************
//函数作用:i2c1发送定长数据
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 i2c1_write_buf(uint8 *buf, uint16 length)
{	
	I2C_Start();//启动总线
	
	I2C_SendByte(I2C1_DEVICE_ADDRESS);//送从器件地址
	WaitAtk(1);
	//发送数据
	while(length--)
	{
		I2C_SendByte(*buf++);
		WaitAtk(1);		
	}
	
	I2C_Stop();//停止总线
	
	return 1;
}

//***********************************************************************************************************************
//函数作用:i2c1读取一个字节
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 i2c1_read_byte(uint8* ch)
{
    uint8 data1 = 0;
    uint8 i = 0;
    uint8 wait;
    I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);

    for(i = 0; i < 8; i++)
    {
        I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);
        for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	 
	 I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
       for(wait=0;wait<I2C1_WAIT_TIME/2;wait++);
	   
	 data1 = data1<<1;
	 if(I2C1_SDA_REG&(1<<I2C1_SDA_BIT))
	 	data1 = data1 + 1;

	 for(wait=0;wait<I2C1_WAIT_TIME;wait++);
    }
    I2C1_SCL_REG&=~(1<<I2C1_SCL_BIT);

    *ch = data1;
    return data1;
}

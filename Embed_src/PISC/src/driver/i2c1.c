//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&i2c1��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:i2c1.c

//*�ļ�����:i2c1��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



#define I2C1_WAIT_TIME		20


//&&&&&&&&&&&&&&&&&&&&&&&&I2Cר�ú���&&&&&&&&&&&&&&&&&&&&&&&&&&

//*****************************************
//����I2C
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
//ֹͣI2C
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
//I2C����һ���ֽ�
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
//I2C�������ȴ�Ӧ��
//****************************************
uint8 WaitAtk(uint8 tmp)
{
	uint16 wait;//����Ͻ��շ���ACK����ʱֵΪ255��

      if(tmp)
		I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
	else
		I2C1_SDA_REG&=~(1<<I2C1_SDA_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	for(wait=0;wait<I2C1_WAIT_TIME;wait++);
	
	//�ȴ�Ӧ��
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
//��������:i2c1��ʼ��
//����˵��:������
//ע������:
//����˵��:��
//***********************************************************************************************************************
void i2c1_init(uint32 bps)
{
	//���ùܽ�����Ϊ���
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


	
	//�͸ߵ�ƽ
	I2C1_SCL_REG|=(1<<I2C1_SCL_BIT);
	I2C1_SDA_REG|=(1<<I2C1_SDA_BIT);
}

//***********************************************************************************************************************
//��������:i2c1���Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 i2c1_write_buf(uint8 *buf, uint16 length)
{	
	I2C_Start();//��������
	
	I2C_SendByte(I2C1_DEVICE_ADDRESS);//�ʹ�������ַ
	WaitAtk(1);
	//��������
	while(length--)
	{
		I2C_SendByte(*buf++);
		WaitAtk(1);		
	}
	
	I2C_Stop();//ֹͣ����
	
	return 1;
}

//***********************************************************************************************************************
//��������:i2c1��ȡһ���ֽ�
//����˵��:
//ע������:
//����˵��:��
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

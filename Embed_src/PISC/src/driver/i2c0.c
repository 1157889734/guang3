//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&i2c0��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:i2c0.c

//*�ļ�����:i2c0��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"


#define I2C0_WAIT_TIME		40


//&&&&&&&&&&&&&&&&&&&&&&&&I2Cר�ú���&&&&&&&&&&&&&&&&&&&&&&&&&&

//*****************************************
//����I2C
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
//ֹͣI2C
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
//I2C����һ���ֽ�
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
//I2C�������ȴ�Ӧ��
//****************************************
uint8 WaitAtk_i2c0(void)
{
       uint8 ack1;
	 uint8 ack2;
	uint16 wait;//����Ͻ��շ���ACK����ʱֵΪ255��

	GPIO1_PC0|=(1<<I2C0_SDA_BIT);
	GPIO1_PC1|=(1<<I2C0_SDA_BIT);
	GPIO1_PC2&=~(1<<I2C0_SDA_BIT);

	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	for(wait=0;wait<I2C0_WAIT_TIME;wait++);

	ack1 = (I2C0_SDA_REG >> I2C0_SDA_BIT)&0x01;
	//�ȴ�Ӧ��
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
//��������:i2c1��ʼ��
//����˵��:������
//ע������:
//����˵��:��
//***********************************************************************************************************************
void i2c0_init(uint32 bps)
{
	//���ùܽ�����Ϊ���
	GPIO1_PC0|=(1<<I2C0_SCL_BIT);
	GPIO1_PC1&=~(1<<I2C0_SCL_BIT);
	GPIO1_PC2|=(1<<I2C0_SCL_BIT);
	GPIO1_PC0|=(1<<I2C0_SDA_BIT);
	GPIO1_PC1&=~(1<<I2C0_SDA_BIT);
	GPIO1_PC2|=(1<<I2C0_SDA_BIT);
	//�͸ߵ�ƽ
	I2C0_SCL_REG|=(1<<I2C0_SCL_BIT);
	I2C0_SDA_REG|=(1<<I2C0_SDA_BIT);
}

//***********************************************************************************************************************
//��������:i2c1���Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void i2c0_write_buf(uint8 *buf, uint16 length)
{	
	I2C0_Start();//��������
	
	I2C0_SendByte(I2C0_DEVICE_ADDRESS);//�ʹ�������ַ
	WaitAtk_i2c0();
	//��������
	while(length--)
	{
		I2C0_SendByte(*buf++);
		WaitAtk_i2c0();		
	}
	
	I2C0_Stop();//ֹͣ����
	
	//return 1;
}




//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&I2C1��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:i2c1.h

//*�ļ�����:I2C1��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef I2C1_H
#define I2C1_H


//Ӳ������
#define I2C1_SCL_REG		GPIO0_PD
#define I2C1_SCL_BIT		2
#define I2C1_SDA_REG		GPIO0_PD
#define I2C1_SDA_BIT		3



//�豸��ַ
#define I2C1_DEVICE_ADDRESS		0x80





//�ӿں���
extern void  i2c1_init(uint32 bps);
extern uint8 i2c1_write_buf(uint8 *buf, uint16 length);

extern uint8 i2c1_read_byte(uint8* ch);
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern uint8 WaitAtk(uint8);
extern void I2C_SendByte(uint8 ch);


#endif



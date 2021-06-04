//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&I2C0��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:i2c0.h

//*�ļ�����:I2C0��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#if 0
#ifndef I2C0_H
#define I2C0_H



//�豸��ַ
#define DEVICE_ADDRESS		0x80



//����������
#define I2C0_FIFO_SIZE			512


#define I2C0_DEFAULT_BAUDRATE		100000


//�жϿ������Ĳ�
#define I2C0ITERR_IRQ_SLOT		EIC_SIR7
#define I2C0_IRQ_SLOT			EIC_SIR15
//�ж�������
#define I2C0ITERR_IRQ_VECTOR		7
#define I2C0_IRQ_VECTOR			15
//�ж����ȼ�
#define I2C0ITERR_IRQ_PRIORITY		8	//���15
#define I2C0_IRQ_PRIORITY		8	//���15



//�ӿں���
extern void  i2c0_init(uint32 bps);
extern uint8 i2c0_read_char(uint8 *ch);
extern void i2c0_write_char(uint8 ch);
extern uint8 i2c0_write_buf(uint8 *buf, uint16 length);
extern void i2c0_write_str(char *str);



#endif
#endif
#ifndef I2C0_H
#define I2C0_H


//Ӳ������
#define I2C0_SCL_REG		GPIO1_PD
#define I2C0_SCL_BIT			13
#define I2C0_SDA_REG		GPIO1_PD
#define I2C0_SDA_BIT		14



//�豸��ַ
#define I2C0_DEVICE_ADDRESS		0x80





//�ӿں���
extern void  i2c0_init(uint32 bps);
extern void  i2c0_write_buf(uint8 *buf, uint16 length);




#endif


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&I2C0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:i2c0.h

//*文件作用:I2C0驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#if 0
#ifndef I2C0_H
#define I2C0_H



//设备地址
#define DEVICE_ADDRESS		0x80



//缓冲区定义
#define I2C0_FIFO_SIZE			512


#define I2C0_DEFAULT_BAUDRATE		100000


//中断控制器的槽
#define I2C0ITERR_IRQ_SLOT		EIC_SIR7
#define I2C0_IRQ_SLOT			EIC_SIR15
//中断向量号
#define I2C0ITERR_IRQ_VECTOR		7
#define I2C0_IRQ_VECTOR			15
//中断优先级
#define I2C0ITERR_IRQ_PRIORITY		8	//最高15
#define I2C0_IRQ_PRIORITY		8	//最高15



//接口函数
extern void  i2c0_init(uint32 bps);
extern uint8 i2c0_read_char(uint8 *ch);
extern void i2c0_write_char(uint8 ch);
extern uint8 i2c0_write_buf(uint8 *buf, uint16 length);
extern void i2c0_write_str(char *str);



#endif
#endif
#ifndef I2C0_H
#define I2C0_H


//硬件定义
#define I2C0_SCL_REG		GPIO1_PD
#define I2C0_SCL_BIT			13
#define I2C0_SDA_REG		GPIO1_PD
#define I2C0_SDA_BIT		14



//设备地址
#define I2C0_DEVICE_ADDRESS		0x80





//接口函数
extern void  i2c0_init(uint32 bps);
extern void  i2c0_write_buf(uint8 *buf, uint16 length);




#endif


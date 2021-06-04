//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&I2C1驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:i2c1.h

//*文件作用:I2C1驱动程序

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef I2C1_H
#define I2C1_H


//硬件定义
#define I2C1_SCL_REG		GPIO0_PD
#define I2C1_SCL_BIT		2
#define I2C1_SDA_REG		GPIO0_PD
#define I2C1_SDA_BIT		3



//设备地址
#define I2C1_DEVICE_ADDRESS		0x80





//接口函数
extern void  i2c1_init(uint32 bps);
extern uint8 i2c1_write_buf(uint8 *buf, uint16 length);

extern uint8 i2c1_read_byte(uint8* ch);
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern uint8 WaitAtk(uint8);
extern void I2C_SendByte(uint8 ch);


#endif



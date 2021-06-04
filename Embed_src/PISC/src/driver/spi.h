#ifndef _SPI_H__
#define _SPI_H__

//#include "Device.h"
//#include "Type.h"
//#include "regARM7.h"
#define SPI_CS	(2)		//AT45DB081B OUT
#define SPI_CLK	(6)		//OUT
#define SPI_IN	(4)
#define SPI_OUTPUT	(5)	//OUT
#define FLASH_WP (7)
#define HIGH 1
#define LOW 0
void SPI_Init(void);
//void SPI_write_byte(uint8 data);
//void SPI_read_byte(uint8 *data);
//void SPI_enable(void);
//void SPI_disable(void);

extern void AT45161_Serial_Input_byte(unsigned char sdata);
extern unsigned char AT45161_Serial_Output(void);

extern void SPI_delay(void);
extern void SPI_gpio_set_out(uint8 gpio,uint8 tmp_cs);

extern void SPI_disable1(void);
extern void SPI_enable1(void);

#endif

/******************************************************************************
*文件名称：at45db161d.h		       		         
*作          者：hk           							 
*描          述：异常数据存储
*创建日期：2010-5-18
*修改记录：无								
******************************************************************************/

#ifndef _AT45DB161_H
#define _AT45DB161_H

#define       FLASH_CS       (2)
#define       FLASH_OUT     (4)    //flash输出
#define       FLASH_IN        (5)   //flash输入
#define       FLASH_CLK     (6)
#define	    FLASH_WP      (7)


#define	Startaddr		0x00000000
#define	Endaddr			0x00210000
#define   PAGE_SIZE         (512)

#define      READ_STATUS_REGISTER     0xd7    //Status Register
#define	   CMD_CONTINUOUS_ARRAY_READ		0xe8
#define	   CMD_CONTINUOUS_ARRAY_WRITE	0x82


extern void read_flash_id(void);
extern void read_flash(uint32 addr, uint8 * buf, uint16 length);
extern void write_flash(uint32 addr, uint8 * buf, uint16 length);
#endif

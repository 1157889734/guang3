/******************************************************************************
*文件名称：at45db161d.c		       		         
*作          者：hk           							 
*描          述：异常数据存储
*创建日期：2010-5-18
*修改记录：无								
******************************************************************************/

#include "include.h"
/****************************************************
*函数名：	write_bit 		         
*作者：hk             							 
*描述：写一位
*输入： gpio  ----io口
                  bit     ----位数据
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
#if 0
static void write_bit(uint8 gpio, uint8 bit1)
{
    if(bit1)
	{
		GPIO0_PD |= (0x0001<<gpio);
	}
	else
	{
		GPIO0_PD &=~ (0x0001<<gpio);
	}
}
#endif
/****************************************************
*函数名：	read_byte		         
*作者：hk             							 
*描述：从flash读取一位
*输入：无
*输出：	无
*返回值：读取的数据
*修改记录：无								
****************************************************/
#if 0
static uint8 read_byte(void)
{
    uint8 rdata = 0x00;
    uint8 wait = 0;
    uint8 cnt = 8;
    while(cnt--)
    {
        write_bit(FLASH_CLK, 1);
	  for(wait =0; wait < 20; wait++);
	  rdata <<= 1;
	  write_bit(FLASH_CLK, 0);

	  if(GPIO0_PD & FLASH_OUT)
	  {
	      rdata++;
	  }
    }
    return rdata;
}
#endif
/****************************************************
*函数名：	write_byte		         
*作者：hk             							 
*描述：写一位
*输入：sdata     ----数据
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
#if 0
static void write_byte(uint8 sdata)
{
    uint8 cnt = 8;
    uint8 wait = 0;
    write_bit(FLASH_CLK, 0);
    while(cnt--)
    {
        if(sdata&0x80)
		write_bit(FLASH_IN, 1);
        else
		write_bit(FLASH_IN, 0);

        write_bit(FLASH_CLK, 1);
	  for(wait =0; wait < 20; wait++);
	  sdata <<= 1;
	  write_bit(FLASH_CLK, 0);
    }
}
#endif

/****************************************************
*函数名：	flash_enable 		         
*作者：hk             							 
*描述：片选
*输入：cs     ----0 允许操作flash
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
static void flash_enable(uint8 cs)
{
    if(cs)
	{
		GPIO2_PD |= (0x0001<<FLASH_CS);
	}
	else
	{
		GPIO2_PD &=~ (0x0001<<FLASH_CS);
	}
}



/****************************************************
*函数名：	flash_busy 		         
*作者：hk             							 
*描述：检查状态寄存器最高位是否为忙，等待空闲
*输入：无
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
#if 1
static void flash_busy(void)
{
      uint8 status = 0x00;
      AT45161_Serial_Input_byte(READ_STATUS_REGISTER);
	while((status & 0x80) == 0)
	{
	    status = AT45161_Serial_Output();
	}
}
#endif
/****************************************************
*函数名：	read_flash 		         
*作者：hk             							 
*描述：从flash的addr位置顺序读取length长度数据
*输入：addr   ---- 数据存储地址
                 buf     ----保存数据的缓存
                 length  ----读取数据的长度
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
void read_flash(uint32 addr, uint8 *buf, uint16 length)
{

	uint16 page = 0;
	uint16 start_offset = 0;
	uint32 i= 0;
	
	uint8 addr_data[7] = {0,0,0,0,0,0,0};
	uint8 tmp_data;

	if((addr + (uint32)length - 1) > Endaddr)
	{
		return;
	}

//****************at45DB161d*************8	
	page = addr / PAGE_SIZE;	
	start_offset = addr %PAGE_SIZE;
#if 0
	addr_data[0] = (uint8)(page >> 6);
	addr_data[1] = (uint8)(page <<2);
	addr_data[1] |= (uint8)(start_offset >> 8);
	addr_data[2] = (uint8)start_offset;
//****************************************************
     //512
     #else
      addr_data[0] = (uint8)(page >> 5);
	addr_data[1] = (uint8)(page <<3);
	addr_data[1] |= (uint8)(start_offset >> 8);
	addr_data[2] = (uint8)start_offset;
      #endif 
	
	flash_enable(0);	//CS选中

	AT45161_Serial_Input_byte(CMD_CONTINUOUS_ARRAY_READ);
      for(i = 0; i < 7; i++)
	{
		AT45161_Serial_Input_byte(addr_data[i]);
	}
	  for(i = 0; i < length; i++)
	{
		tmp_data = AT45161_Serial_Output();
		*buf = tmp_data;
		buf++;
	}
	flash_enable(1);
}


/****************************************************
*函数名：	write_flash 		         
*作者：hk             							 
*描述：在flash的addr位置顺序写入length长度数据
*输入：addr   ---- 数据存储地址
                 buf     ----需保存的数据
                 length  ---- 数据的长度
*输出：	无
*返回值：无
*修改记录：无								
****************************************************/
void write_flash(uint32 addr, uint8 *buf, uint16 length)
{
	uint16 page = 0;
	uint16 start_offset = 0;
	uint32 i= 0;
	
	uint8 addr_data[7] = {0,0,0,0,0,0,0};
	//uint8 tmp_data;

	if((addr + (uint32)length - 1) > Endaddr)
	{
		//return;
		//从头开始写
		addr = 0x00;
	}

//****************at45DB161d*************8	
	page = addr / PAGE_SIZE;	
	start_offset = addr %PAGE_SIZE;
#if 0
	addr_data[0] = (uint8)(page >> 6);
	addr_data[1] = (uint8)(page <<2);
	addr_data[1] |= (uint8)(start_offset >> 8);
	addr_data[2] = (uint8)start_offset;
//****************************************************
      //512
     #else
      addr_data[0] = (uint8)(page >> 5);
	addr_data[1] = (uint8)(page <<3);
	addr_data[1] |= (uint8)(start_offset >> 8);
	addr_data[2] = (uint8)start_offset;
      #endif 
	
	flash_enable(0);	//CS选中
	//write_bit(FLASH_WP, 1);  //高电平允许写
	//flash_busy();
	AT45161_Serial_Input_byte(CMD_CONTINUOUS_ARRAY_WRITE);	//opcode  通过缓存的主存页编写
	
	for(i = 0; i < 3; i++)
	{
		AT45161_Serial_Input_byte(addr_data[i]);
	}
	for(i = 0; i < length; i++)
	{
	       //flash_busy();
		AT45161_Serial_Input_byte(*buf);
		buf++;
	}
	
	flash_enable(1);
	//flash_busy();
	
}


void read_flash_id(void)
{
       uint8 length = 4;
	uint8 buf[4] = {0};
	uint8 tmp_data = 0;
	int i =0;

	flash_enable(0);	//CS选中
	AT45161_Serial_Input_byte(0x9f);	//opcode  通过缓存的主存页编写
	
	for(i = 0; i < length; i++)
	{
		tmp_data = AT45161_Serial_Output();
		buf[i] = tmp_data;
	}
	flash_enable(1);
      print_line("flash id");
	print_buf(buf, length);

	print_line("flash id end");
	
}

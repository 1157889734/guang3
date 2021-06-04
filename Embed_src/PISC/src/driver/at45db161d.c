/******************************************************************************
*�ļ����ƣ�at45db161d.c		       		         
*��          �ߣ�hk           							 
*��          �����쳣���ݴ洢
*�������ڣ�2010-5-18
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"
/****************************************************
*��������	write_bit 		         
*���ߣ�hk             							 
*������дһλ
*���룺 gpio  ----io��
                  bit     ----λ����
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
*��������	read_byte		         
*���ߣ�hk             							 
*��������flash��ȡһλ
*���룺��
*�����	��
*����ֵ����ȡ������
*�޸ļ�¼����								
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
*��������	write_byte		         
*���ߣ�hk             							 
*������дһλ
*���룺sdata     ----����
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
*��������	flash_enable 		         
*���ߣ�hk             							 
*������Ƭѡ
*���룺cs     ----0 �������flash
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
*��������	flash_busy 		         
*���ߣ�hk             							 
*���������״̬�Ĵ������λ�Ƿ�Ϊæ���ȴ�����
*���룺��
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
*��������	read_flash 		         
*���ߣ�hk             							 
*��������flash��addrλ��˳���ȡlength��������
*���룺addr   ---- ���ݴ洢��ַ
                 buf     ----�������ݵĻ���
                 length  ----��ȡ���ݵĳ���
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
	
	flash_enable(0);	//CSѡ��

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
*��������	write_flash 		         
*���ߣ�hk             							 
*��������flash��addrλ��˳��д��length��������
*���룺addr   ---- ���ݴ洢��ַ
                 buf     ----�豣�������
                 length  ---- ���ݵĳ���
*�����	��
*����ֵ����
*�޸ļ�¼����								
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
		//��ͷ��ʼд
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
	
	flash_enable(0);	//CSѡ��
	//write_bit(FLASH_WP, 1);  //�ߵ�ƽ����д
	//flash_busy();
	AT45161_Serial_Input_byte(CMD_CONTINUOUS_ARRAY_WRITE);	//opcode  ͨ�����������ҳ��д
	
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

	flash_enable(0);	//CSѡ��
	AT45161_Serial_Input_byte(0x9f);	//opcode  ͨ�����������ҳ��д
	
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

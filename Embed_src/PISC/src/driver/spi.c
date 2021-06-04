#include "include.h"

void SPI_delay(void)
{
	//LED_SHOW_ST_ADD(LED_CTL2_ADD) = out_st2;
	//LED_SHOW_ST_ADD(LED_CTL1_ADD) = out_st1;
	uint8 i=5;//20;
	while(i--);
	return;
}

void SPI_gpio_set_out(uint8 gpio,uint8 tmp_cs)
{
	if(tmp_cs)
	{
		GPIO0_PD |= (0x0001<<gpio);
	}
	else
	{
		GPIO0_PD &=~ (0x0001<<gpio);
	}
}
void SPI_gpio2_set_out(uint8 gpio,uint8 tmp_cs)
{
	if(tmp_cs)
	{
		GPIO2_PD |= (0x0001<<gpio);
	}
	else
	{
		GPIO2_PD &=~ (0x0001<<gpio);
	}
}
void SPI_Init(void)
{
	//SPI_gpio_set_out(SPI_CS,HIGH);
	SPI_gpio2_set_out(SPI_CS,HIGH);
	SPI_gpio_set_out(SPI_CLK,LOW);
	SPI_gpio_set_out(SPI_OUTPUT,HIGH);
	
}

void SPI_enable1(void)
{
	uint8 counter_tmp = 0;
	SPI_gpio2_set_out(SPI_CS,0);
	for(; counter_tmp < 02; counter_tmp++)
	{
		SPI_delay();
	}
	return;	
}

void SPI_disable1(void)
{
	SPI_gpio2_set_out(SPI_CS,1);
	SPI_delay();
	return;
}


void AT45161_Serial_Input_byte(unsigned char sdata)  //write a byte to Flash
{   
   unsigned char i=8;
   SPI_delay();
   while(i--)
   { 
   SPI_gpio_set_out(SPI_CLK,0);
    SPI_delay();
    if(sdata & 0x80)
    {
    	SPI_gpio_set_out(SPI_OUTPUT,1);
    }
    else
    {
    	SPI_gpio_set_out(SPI_OUTPUT,0);
    }
    SPI_delay();
    //mdelay(1);
    SPI_gpio_set_out(SPI_CLK,1);
    SPI_delay();
    sdata <<= 1;
   // SPI_gpio_set_out(SPI_CLK,0);
    //SPI_delay();
   }
}

unsigned char AT45161_Serial_Output(void)  //read one byte
{
	unsigned char i=8;
   	unsigned char sdata=0x00;
	SPI_delay();
   while(i--)
   { 
    	 SPI_gpio_set_out(SPI_CLK,1);
    	 SPI_delay();
    	sdata <<= 1;
    	 SPI_gpio_set_out(SPI_CLK,0);
   	//SPI_delay();
    	 //SPI_delay();
    	if(GPIO0_PD & (0x0010))
    	{
      		sdata++;
      	}
      	SPI_delay();
   }
   return(sdata);
}




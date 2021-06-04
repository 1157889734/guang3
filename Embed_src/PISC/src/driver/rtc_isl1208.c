/******************************************************************************
*文件名称：rtc_isl1208.c		       		         
*作          者：hk           							 
*描          述：实时时钟处理
*创建日期：2010-5-18
*修改记录：无								
******************************************************************************/

#include "include.h"

#define  I2C_READ_BYTE           i2c1_read_byte
#define  I2C_WRITE_BYTE         I2C_SendByte
#define  I2C_START                   I2C_Start
#define  I2C_STOP                    I2C_Stop
#define  I2C_ACK                      WaitAtk
#define  I2C_INIT                     i2c1_init
#define  I2C_WRITE_BUF         i2c1_write_buf


/*#define  I2C_READ_BYTE           i2c1_read_char
#define  I2C_WRITE_BYTE         i2c1_write_char
//#define  I2C_START                   I2C_Start
//#define  I2C_STOP                    I2C_Stop
//#define  I2C_ACK                      WaitAtk
#define  I2C_INIT                     i2c1_init
*/
#define ISL1208_WRITE_ADDR    0xde
#define ISL1208_READ_ADDR      0xdf

/****************************************************
*函数名：	write_rtc 		         
*作者：hk             							 
*描述：往时钟写一个字节
*输入：无                 
*输出：	
*返回值：无
*修改记录：无								
****************************************************/
#if 0
static void write_rtc(uint8 addr, uint8 data1)
{
   // I2C_START();
   // I2C_WRITE_BYTE(ISL1208_WRITE_ADDR);
   // I2C_ACK(1);
 //   I2C_WRITE_BYTE(addr);
   // I2C_ACK(1);
  //  I2C_WRITE_BYTE(data1);
   // I2C_ACK(1);
    //I2C_STOP();
}
#endif
/****************************************************
*函数名：	rtc_get_time 		         
*作者：hk             							 
*描述：读取时钟时间
*输入：time  ------       时间
*输出：	
*返回值：无
*修改记录：无								
****************************************************/
static void rtc_get_time(uint8* time)
{
    uint8 tmp = 0;

     
     if((sh9_tms_dev.month>12) || (sh9_tms_dev.date ==0)
	 	||(sh9_tms_dev.date>31) || (sh9_tms_dev.hour>24)||(sh9_tms_dev.minute>60))
     	{
     	}
	 else{
	     //tms 日期正常用tms数据
	     time[0] = (((sh9_tms_dev.year/10)<<4) |(sh9_tms_dev.year%10));
	     time[1] = (((sh9_tms_dev.month/10)<<4) |(sh9_tms_dev.month%10));
	     time[2] = (((sh9_tms_dev.date/10)<<4) |(sh9_tms_dev.date%10));
	     time[3] = (((sh9_tms_dev.hour/10)<<4) |(sh9_tms_dev.hour%10));
	     time[4] = (((sh9_tms_dev.minute/10)<<4) |(sh9_tms_dev.minute%10));
	     return;
	 }

    for(tmp = 0; tmp < 6; tmp++)
    	{
	    	GPIO0_PC0|=(1<<I2C1_SDA_BIT);  //out put
		GPIO0_PC1&=~(1<<I2C1_SDA_BIT);
		GPIO0_PC2|=(1<<I2C1_SDA_BIT);
	// for(cnt =0; cnt < 10; cnt++);
    	    I2C_START();
	    I2C_WRITE_BYTE(ISL1208_WRITE_ADDR);
	    I2C_ACK(1);

	    I2C_WRITE_BYTE(tmp);
	    I2C_ACK(1);

	    I2C_START();
	    I2C_WRITE_BYTE(ISL1208_READ_ADDR);
	    I2C_ACK(1);
		 
	    	{
			GPIO0_PC0 |= (1<<I2C1_SDA_BIT); 	//input
			GPIO0_PC1 &= ~(1<<I2C1_SDA_BIT);
			GPIO0_PC2 &= ~(1<<I2C1_SDA_BIT);
	    	}

	    //  for(cnt =0; cnt < 10; cnt++);

	     I2C_READ_BYTE(&time[5-tmp] );   
	     I2C_ACK(0);
	     I2C_STOP();
    // print_int("%d", time[5-tmp]);


    	}
}

/****************************************************
*函数名：	rtc_set_time 		         
*作者：hk             							 
*描述：设置时钟时间
*输入：time  ------       时间
*输出：	
*返回值：无
*修改记录：无								
****************************************************/
static void rtc_set_time(uint8 *time)
{
    uint8 tmp = 0;
  //  I2C_WRITE_BYTE(ISL1208_WRITE_ADDR, 0x00, 8);

    //I2C_WRITE_BYTE(ISL1208_WRITE_ADDR, 0x01, time[4]);
   // I2C_WRITE_BYTE(ISL1208_WRITE_ADDR, 0x02, time[3]);

    for(tmp = 1; tmp <6; tmp++)
    {
	    I2C_START();
	    I2C_WRITE_BYTE(ISL1208_WRITE_ADDR);
	    I2C_ACK(1);

	    I2C_WRITE_BYTE(tmp);
	    I2C_ACK(1);

	    I2C_WRITE_BYTE(time[5-tmp]);
	    I2C_ACK(0);
	    I2C_STOP();
    	}

}


/****************************************************
*函数名：	rtc_init 		         
*作者：hk             							 
*描述：实时时钟初始化
*输入：无                 
*输出：	
*返回值：无
*修改记录：无								
****************************************************/
void rtc_init(void)
{
    uint8 wait= 0;
    I2C_INIT(10000);
//I2C_WRITE_BYTE(ISL1208_WRITE_ADDR,0x07,0xd0);

   I2C_START();
 I2C_WRITE_BYTE(ISL1208_WRITE_ADDR);
  I2C_ACK(1);
  while(wait<5)wait++;

   I2C_WRITE_BYTE(0x07);
   I2C_ACK(1);
   while(wait<5)wait++;

   I2C_WRITE_BYTE(0x12);
	while(wait<5)wait++;
   I2C_ACK(0);
   I2C_STOP();
   //write_rtc(0x07, 0xd0);   //允许外部32768Hz的时钟，   WRTC 置1，XTOSCB置1

//for test 
    //write_rtc(0x00,0x00);
    print_line("rtc init");
}

rtc_dev_t rtc_dev =
{
    rtc_init,
    rtc_get_time,
    rtc_set_time,
};

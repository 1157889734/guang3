/******************************************************************************
*�ļ����ƣ�rtc_isl1208.c		       		         
*��          �ߣ�hk           							 
*��          ����ʵʱʱ�Ӵ���
*�������ڣ�2010-5-18
*�޸ļ�¼����								
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
*��������	write_rtc 		         
*���ߣ�hk             							 
*��������ʱ��дһ���ֽ�
*���룺��                 
*�����	
*����ֵ����
*�޸ļ�¼����								
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
*��������	rtc_get_time 		         
*���ߣ�hk             							 
*��������ȡʱ��ʱ��
*���룺time  ------       ʱ��
*�����	
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void rtc_get_time(uint8* time)
{
    uint8 tmp = 0;

     
     if((sh9_tms_dev.month>12) || (sh9_tms_dev.date ==0)
	 	||(sh9_tms_dev.date>31) || (sh9_tms_dev.hour>24)||(sh9_tms_dev.minute>60))
     	{
     	}
	 else{
	     //tms ����������tms����
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
*��������	rtc_set_time 		         
*���ߣ�hk             							 
*����������ʱ��ʱ��
*���룺time  ------       ʱ��
*�����	
*����ֵ����
*�޸ļ�¼����								
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
*��������	rtc_init 		         
*���ߣ�hk             							 
*������ʵʱʱ�ӳ�ʼ��
*���룺��                 
*�����	
*����ֵ����
*�޸ļ�¼����								
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
   //write_rtc(0x07, 0xd0);   //�����ⲿ32768Hz��ʱ�ӣ�   WRTC ��1��XTOSCB��1

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

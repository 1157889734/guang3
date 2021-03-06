#ifndef __GPIO_H__
#define __GPIO_H__

#include <sys/ioctl.h>
#include <fcntl.h>


typedef enum GPIO_DIR{
	GPIO_DIR_IN = 0,
	GPIO_DIR_OUT
}GPIO_DIR_EN;

typedef enum GPIO_GROUP{
	GPIO0 = 0,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO4,
	GPIO5,
//qixiaohui
	GPIO6,
	GPIO7,
	GPIO8,
	GPIO9,	
}GPIO_GROUP_EN;

typedef enum GPIO_BIT{
	GPIO_BIT0 = 0,
	GPIO_BIT1,
	GPIO_BIT2,
	GPIO_BIT3,
	GPIO_BIT4,
	GPIO_BIT5,
	GPIO_BIT6,
	GPIO_BIT7,
}GPIO_BIT_EN;


typedef enum GPIO_BIT_VALUE{
	GPIO_BIT_LOW = 0,
	GPIO_BIT_HIGH
}GPIO_BIT_VALUE_EN;




typedef struct {
unsigned int  group;
unsigned int  bit;
unsigned int  value;
}GPIO_GROUPBIT_INFO_T;



/*ó??§??±eμ?   IO   ?ú?¨ò?*/

//·??ù?÷
#define GPIO_BUZZER_GROUP			GPIO0
#define GPIO_BUZZER_BIT			GPIO_BIT5

//RECORD LED
#define GPIO_RECORD_LED_GROUP				GPIO5
#define GPIO_RECORD_LED_BIT				GPIO_BIT7

//ALARM IO 
#define GPIO_ALARM0_GROUP					GPIO1
#define GPIO_ALARM0_BIT					GPIO_BIT6
#define GPIO_ALARM1_GROUP					GPIO5
#define GPIO_ALARM1_BIT					GPIO_BIT3

//???io
#define GPIO_POWER_OFF_GROUP					GPIO9
#define GPIO_POWER_OFF_BIT					GPIO_BIT4


#define GPIO_SET_DIR 0x1
#define GPIO_GET_DIR 0x2
#define GPIO_READ_BIT 0x3
#define GPIO_WRITE_BIT 0x4

#define DDR2_REG_BASE   (0x20000000)
#define MAP_SIZE        4096UL
#define MAP_MASK        (MAP_SIZE - 1)




int GpioInit(void);
int  GpioDeinit(void);
int  GpioSetDir( GPIO_GROUP_EN gpioGroup, GPIO_BIT_EN gpioBit, GPIO_DIR_EN gpioDir );
int  GpioSetBit(GPIO_GROUP_EN gpioGroup, GPIO_BIT_EN gpioBit, GPIO_BIT_VALUE_EN bitValue);
int  GpioGetBit(GPIO_GROUP_EN gpioGroup, GPIO_BIT_EN gpioBit);
void GpioBuzzerEnable(void);
void GpioBuzzerDisable(void);
void GpioRecordLedEnable(void);
void GpioRecordLedDisable(void);
int memr_get_value(unsigned long ulAddr);
int memr_set_value(unsigned long ulAddr,unsigned long ulNew);

int StartGpio();
void GpioWorkingUartSetUnSend();
void GpioWorkingUartSetSend();


int openMem();
void mmapUartDate(int dev_fd,int reg,unsigned char **pMem);
void ChangeDateToSend(unsigned char *pMem);
void ChangeDateToUnSend(unsigned char *pMem);

void ummapUartDate();





#endif  // __GPIO_H__


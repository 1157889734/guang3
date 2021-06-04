/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司, All rights reserved.
**  作者：李军
**  当前版本：v1.0
**  创建日期: 2013.05.03
**  文件说明: IO口控制
*******************************************************************************
*/

#include "include.h"


#define GPIO6_REG_BASEADDR		(0x201b0000)
#define GPIO6_REG_DATA			(0x201b03FC)	//数据寄存器,bit2-bit9表示数据的bit0-bit7是否可写，1 可写，0 不可写  
#define GPIO6_REG_DIR				(0x201b0400)	//输入输出控制

#define GPIO9_REG_BASEADDR		(0x201e0000)
#define GPIO9_REG_DATA			(0x201e03FC)	//数据寄存器,bit2-bit9表示数据的bit0-bit7是否可写，1 可写，0 不可写  
#define GPIO9_REG_DIR				(0x201e0400)	//输入输出控制 方向控制寄存器


//复用管脚
#define POWER_OFF_REG_REUSE_CTRL		(0x200f00e8) //管脚复用，控制地址
#define WORKING_REG_REUSE_CTRL		(0x200f00e8) //管脚复用，控制地址

#define UART_SEND_REG_REUSE_CTRL        (0x200f0150)//管脚服用，控制地址

//GPIO寄存器
#define POWER_OFF_REG_BASEADDR		GPIO6_REG_BASEADDR
#define POWER_OFF_BIT		(0)
#define POWER_OFF_REG_DATA		GPIO6_REG_DATA
#define POWER_OFF_REG_DIR		GPIO6_REG_DIR

#define UART_SEND_REG_BASEADDR          GPIO9_REG_BASEADDR
#define UART_SEND_BIT		(0)
#define UART_SEND_REG_DATA		GPIO9_REG_DATA //数据寄存器
#define UART_SEND_REG_DIR		GPIO9_REG_DIR //方向控制寄存器


#define WORKING_REG_BASEADDR		GPIO6_REG_BASEADDR
#define WORKING_BIT		(0)
#define WORKING_REG_DATA		GPIO6_REG_BASEADDR
#define WORKING_REG_DIR		GPIO6_REG_DIR

unsigned char *pTestRegBase = NULL;


static signed int fd = -1;

void GpioDirInit(void);


/*
* fn: GPIO 初始化，打开GPIO设备
* return : 0:成功    -1:失败
*/
int GpioInit(void)
{
	GpioDirInit(); //qixiaohui

    return 0;
}

int  GpioDeinit(void)
{

    return 0;
}

int  GpioSetDir( int reg, unsigned char bit, GPIO_DIR_EN gpioDir )
{
	int val_tmp=0;
	//读取管脚值
	val_tmp=memr_get_value(reg);
	if(1==gpioDir)
	{
		val_tmp|=(1<<bit);// 第一位为1输出 不影响其他位
	}
	else if(0==gpioDir)
	{
		val_tmp&=~(1<<bit);//第一位为0 输入 不影响其他位
	}
	//设置mem值
	memr_set_value(reg,val_tmp);

	return 1;
}

int  GpioGetBit(int reg, unsigned char bit)
{
	int val_tmp=0;
	//读取管脚值
	val_tmp=memr_get_value(reg);
	val_tmp=((val_tmp>>bit)&1);
	
	return val_tmp;
}

void GpioSetBit(int reg,unsigned char bit,unsigned char val)
{
	int val_tmp=0;
	//读取管脚值
	val_tmp=memr_get_value(reg);
	if(1==val)
	{
		val_tmp|=(1<<bit);
	}
	else if(0==val)
	{
		val_tmp&=~(1<<bit);
	}
	//设置mem值
	memr_set_value(reg,val_tmp);
}

/*===========================================================================================================
*  以上为 GPIO  通用接口   以下为用户GPIO初始化
*============================================================================================================*/

static void GpioPowerOffInit(void)
{
	//设置复用管脚为gpio模式
//	memr_set_value(POWER_OFF_REG_REUSE_CTRL,0x00);
 	char memrSet[30];
	sprintf(memrSet,"himm 0x%x 0x00",POWER_OFF_REG_REUSE_CTRL );
	system(memrSet);

	//掉电检测
	GpioSetDir(POWER_OFF_REG_DIR, POWER_OFF_BIT, GPIO_DIR_IN);
}
static void GpioWorkingInit(void)
{
	//设置复用管脚为gpio模式
	//memr_set_value(GPIO9_REG_REUSE_CTRL,GPIO9_VAL_REUSE_CTRL);
	//掉电检测
	GpioSetDir(WORKING_REG_DIR, WORKING_BIT, GPIO_DIR_IN);
}
static void GpioWorkingSet(unsigned char onoff)
{
	//设置
	GpioSetBit(WORKING_REG_DATA,WORKING_BIT,onoff);
}
static void GpioWorkingToggle()
{
	static unsigned char onoff_tmp=0;
	//printf("onoff_tmp: %d\r\n",onoff_tmp);
	//设置
	GpioSetBit(WORKING_REG_DATA,WORKING_BIT,onoff_tmp);
	onoff_tmp=(1-onoff_tmp);
}
/*===========================================================================================================
*  UART GPIO初始化
*============================================================================================================*/
int openMem()
{
	int dev_fd;
	dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);       
    if (dev_fd < 0) 
	{
        perror("open(/dev/mem) failed.");     
        return -1;
    }
	return dev_fd;
}
void mmapUartDate(int dev_fd,int reg,unsigned char **pMem)
{
	unsigned int ulOld;
    pTestRegBase = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, reg & ~MAP_MASK);
    if (MAP_FAILED == pTestRegBase) 
	{
        printf("mmap failed. fd(%d), addr(0x%x), size(%d)\n", dev_fd, reg, MAP_SIZE); 
    }
	else
	{
		printf("mmap ok. fd(%d), addr(0x%x), size(%d)\n", dev_fd, reg, MAP_SIZE); 
		*pMem = pTestRegBase + (reg & MAP_MASK);
		//ulOld = *((volatile unsigned int *)pMem);
    }
}
void ummapUartDate()
{
	munmap((void*)pTestRegBase, MAP_SIZE);
	pTestRegBase = NULL;
}
//立即发送
void ChangeDateToSend(unsigned char *pMem)
{
	int ulNew = 1;
	int val_tmp=0;
	val_tmp|=(1<<0);
	if(pMem == NULL)
	{
		printf("pMem error\n");
	}
	*(volatile unsigned long*)pMem = val_tmp;
}

//立即不发送
void ChangeDateToUnSend(unsigned char *pMem)
{
	int ulNew = 1;
	int val_tmp=0;
	val_tmp&=~(1<<0);
	if(pMem == NULL)
	{
		printf("pMem error\n");
	}
	*(volatile unsigned long*)pMem = val_tmp;
}


//初始化不发送
#if 1
void GpioWorkingUartSetUnSend()
{
	 //设置
	unsigned char Send_tmp=0;
	GpioSetBit(UART_SEND_REG_DATA,UART_SEND_BIT,Send_tmp);//设置data寄存器
  // printf("close*---------------------------------------\n");
}
#endif
//初始化发送
void GpioWorkingUartSetSend()
{
    //设置
   unsigned char Send_tmp=1;
   GpioSetBit(UART_SEND_REG_DATA,UART_SEND_BIT,Send_tmp);//设置data寄存器
  // printf("send*---------------------------------------\n");
}


static void GpioUartSendInit()
{
    //设置复用管脚为gpio模式
    char memrSet[30];
	sprintf(memrSet,"himm 0x%x 0x00",UART_SEND_REG_REUSE_CTRL );
	system(memrSet);
   //memr_set_value(UART_SEND_REG_REUSE_CTRL,0x00);
   
   //设置dir寄存器为1输出 
   GpioSetDir(UART_SEND_REG_DIR, UART_SEND_BIT, GPIO_DIR_OUT);
   //初始化data寄存器为0不发送
   GpioWorkingUartSetSend();

}


/*
* fn: 初始化系统所有用到的GPIO
*/
void GpioDirInit(void)
{
       //uart控制send的管脚初始化
       GpioUartSendInit();
	//工作指示灯
//	GpioWorkingInit();
	//掉电检测
	//GpioPowerOffInit();
}

void *GpioThread( void *args )
{
	printf("start GpioThread check powerDown-----------------------------lyq\n");
	int val_tmp=0;
	
	while(1)
	{
		//读取掉电检测管脚值
		val_tmp=GpioGetBit(POWER_OFF_REG_DATA,POWER_OFF_BIT);
		//printf("val_tmp  is : %d\r\n",val_tmp);
#if 1
		if(!(val_tmp & 0x00000001))
		{
			printf("reboot\n");
			system("reboot");
		}
#endif

		sleep(1);
	}
	return NULL;
}

int StartGpio()
{
	GpioInit();
	//pthread_t tidGpio;
	//pthread_create( &tidGpio, NULL, GpioThread, NULL);
	return 1;
}



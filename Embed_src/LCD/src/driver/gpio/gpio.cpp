/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾, All rights reserved.
**  ���ߣ����
**  ��ǰ�汾��v1.0
**  ��������: 2013.05.03
**  �ļ�˵��: IO�ڿ���
*******************************************************************************
*/

#include "include.h"


#define GPIO6_REG_BASEADDR		(0x201b0000)
#define GPIO6_REG_DATA			(0x201b03FC)	//���ݼĴ���,bit2-bit9��ʾ���ݵ�bit0-bit7�Ƿ��д��1 ��д��0 ����д  
#define GPIO6_REG_DIR				(0x201b0400)	//�����������

#define GPIO9_REG_BASEADDR		(0x201e0000)
#define GPIO9_REG_DATA			(0x201e03FC)	//���ݼĴ���,bit2-bit9��ʾ���ݵ�bit0-bit7�Ƿ��д��1 ��д��0 ����д  
#define GPIO9_REG_DIR				(0x201e0400)	//����������� ������ƼĴ���


//���ùܽ�
#define POWER_OFF_REG_REUSE_CTRL		(0x200f00e8) //�ܽŸ��ã����Ƶ�ַ
#define WORKING_REG_REUSE_CTRL		(0x200f00e8) //�ܽŸ��ã����Ƶ�ַ

#define UART_SEND_REG_REUSE_CTRL        (0x200f0150)//�ܽŷ��ã����Ƶ�ַ

//GPIO�Ĵ���
#define POWER_OFF_REG_BASEADDR		GPIO6_REG_BASEADDR
#define POWER_OFF_BIT		(0)
#define POWER_OFF_REG_DATA		GPIO6_REG_DATA
#define POWER_OFF_REG_DIR		GPIO6_REG_DIR

#define UART_SEND_REG_BASEADDR          GPIO9_REG_BASEADDR
#define UART_SEND_BIT		(0)
#define UART_SEND_REG_DATA		GPIO9_REG_DATA //���ݼĴ���
#define UART_SEND_REG_DIR		GPIO9_REG_DIR //������ƼĴ���


#define WORKING_REG_BASEADDR		GPIO6_REG_BASEADDR
#define WORKING_BIT		(0)
#define WORKING_REG_DATA		GPIO6_REG_BASEADDR
#define WORKING_REG_DIR		GPIO6_REG_DIR

unsigned char *pTestRegBase = NULL;


static signed int fd = -1;

void GpioDirInit(void);


/*
* fn: GPIO ��ʼ������GPIO�豸
* return : 0:�ɹ�    -1:ʧ��
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
	//��ȡ�ܽ�ֵ
	val_tmp=memr_get_value(reg);
	if(1==gpioDir)
	{
		val_tmp|=(1<<bit);// ��һλΪ1��� ��Ӱ������λ
	}
	else if(0==gpioDir)
	{
		val_tmp&=~(1<<bit);//��һλΪ0 ���� ��Ӱ������λ
	}
	//����memֵ
	memr_set_value(reg,val_tmp);

	return 1;
}

int  GpioGetBit(int reg, unsigned char bit)
{
	int val_tmp=0;
	//��ȡ�ܽ�ֵ
	val_tmp=memr_get_value(reg);
	val_tmp=((val_tmp>>bit)&1);
	
	return val_tmp;
}

void GpioSetBit(int reg,unsigned char bit,unsigned char val)
{
	int val_tmp=0;
	//��ȡ�ܽ�ֵ
	val_tmp=memr_get_value(reg);
	if(1==val)
	{
		val_tmp|=(1<<bit);
	}
	else if(0==val)
	{
		val_tmp&=~(1<<bit);
	}
	//����memֵ
	memr_set_value(reg,val_tmp);
}

/*===========================================================================================================
*  ����Ϊ GPIO  ͨ�ýӿ�   ����Ϊ�û�GPIO��ʼ��
*============================================================================================================*/

static void GpioPowerOffInit(void)
{
	//���ø��ùܽ�Ϊgpioģʽ
//	memr_set_value(POWER_OFF_REG_REUSE_CTRL,0x00);
 	char memrSet[30];
	sprintf(memrSet,"himm 0x%x 0x00",POWER_OFF_REG_REUSE_CTRL );
	system(memrSet);

	//������
	GpioSetDir(POWER_OFF_REG_DIR, POWER_OFF_BIT, GPIO_DIR_IN);
}
static void GpioWorkingInit(void)
{
	//���ø��ùܽ�Ϊgpioģʽ
	//memr_set_value(GPIO9_REG_REUSE_CTRL,GPIO9_VAL_REUSE_CTRL);
	//������
	GpioSetDir(WORKING_REG_DIR, WORKING_BIT, GPIO_DIR_IN);
}
static void GpioWorkingSet(unsigned char onoff)
{
	//����
	GpioSetBit(WORKING_REG_DATA,WORKING_BIT,onoff);
}
static void GpioWorkingToggle()
{
	static unsigned char onoff_tmp=0;
	//printf("onoff_tmp: %d\r\n",onoff_tmp);
	//����
	GpioSetBit(WORKING_REG_DATA,WORKING_BIT,onoff_tmp);
	onoff_tmp=(1-onoff_tmp);
}
/*===========================================================================================================
*  UART GPIO��ʼ��
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
//��������
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

//����������
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


//��ʼ��������
#if 1
void GpioWorkingUartSetUnSend()
{
	 //����
	unsigned char Send_tmp=0;
	GpioSetBit(UART_SEND_REG_DATA,UART_SEND_BIT,Send_tmp);//����data�Ĵ���
  // printf("close*---------------------------------------\n");
}
#endif
//��ʼ������
void GpioWorkingUartSetSend()
{
    //����
   unsigned char Send_tmp=1;
   GpioSetBit(UART_SEND_REG_DATA,UART_SEND_BIT,Send_tmp);//����data�Ĵ���
  // printf("send*---------------------------------------\n");
}


static void GpioUartSendInit()
{
    //���ø��ùܽ�Ϊgpioģʽ
    char memrSet[30];
	sprintf(memrSet,"himm 0x%x 0x00",UART_SEND_REG_REUSE_CTRL );
	system(memrSet);
   //memr_set_value(UART_SEND_REG_REUSE_CTRL,0x00);
   
   //����dir�Ĵ���Ϊ1��� 
   GpioSetDir(UART_SEND_REG_DIR, UART_SEND_BIT, GPIO_DIR_OUT);
   //��ʼ��data�Ĵ���Ϊ0������
   GpioWorkingUartSetSend();

}


/*
* fn: ��ʼ��ϵͳ�����õ���GPIO
*/
void GpioDirInit(void)
{
       //uart����send�Ĺܽų�ʼ��
       GpioUartSendInit();
	//����ָʾ��
//	GpioWorkingInit();
	//������
	//GpioPowerOffInit();
}

void *GpioThread( void *args )
{
	printf("start GpioThread check powerDown-----------------------------lyq\n");
	int val_tmp=0;
	
	while(1)
	{
		//��ȡ������ܽ�ֵ
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



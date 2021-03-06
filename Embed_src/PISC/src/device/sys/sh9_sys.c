//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:sys.c

//*文件作用:系统模块

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//***********************************************************************************************************************
//函数作用:系统模块处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void sys_proc(void)
{
	//喂狗
	dog();
	softdog();

	//参数保存处理
	save_proc();
	
	//USB连接指示灯
	if( usb_get_connect_state() )
		led_blink(USB_LED_NO, 400, 600);
	else
		led_onoff(USB_LED_NO, 1);
}

//***********************************************************************************************************************
//函数作用:系统模块初始化
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void sys_init(void)
{	
	mcu_init();//MCU基本设置
	irq_init();//总中断初始化
	flash_init();//FLASH存储器读写
	dog_init();//看门狗初始化
	timer0_init(TIMER0_DEFAULT_MS);//系统时钟初始化
	led_init();//面板指示灯初始化
	save_init();//参数保存初始化
	init_gpio();	//initial 8 ports(p2.8~p2.15) to output and set GPIO0 pin 5 to output
#if 0	
	//配置设备号管脚属性为输入+弱上拉
	GPIO0_PC0|=(0x0001<<DEV_ID_DET_BIT);
	GPIO0_PC1|=(0x0001<<DEV_ID_DET_BIT);
	GPIO0_PC2&=~(0x0001<<DEV_ID_DET_BIT);
	
	//配置司机室钥匙管脚属性为输入+弱上拉
	GPIO2_PC0|=(0x0001<<CAB_KEY_BIT);
	GPIO2_PC1|=(0x0001<<CAB_KEY_BIT);
	GPIO2_PC2&=~(0x0001<<CAB_KEY_BIT);
#endif
}



//输出虚拟设备类
sys_dev_t sh9_sys_dev=
{
	sys_init,
	sys_proc,
};



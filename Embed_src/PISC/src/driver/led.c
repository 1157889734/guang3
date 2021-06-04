//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& led指示灯 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:led.c

//*文件作用:led指示灯

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//LED控制位
static uint8 const led_ctrl_bit[]=
{
	LED1_CTRL_BIT,
	LED2_CTRL_BIT,
	LED3_CTRL_BIT,
	LED4_CTRL_BIT,
	LED5_CTRL_BIT,
	LED6_CTRL_BIT,
	LED7_CTRL_BIT,
	LED8_CTRL_BIT,
	LED9_CTRL_BIT,
	LED10_CTRL_BIT,
};
//LED亮灭状态
static uint16 led_state=0;
//闪烁定时器
static uint32 led_blink_timer[LED_NUM]={0};



//***********************************************************************************************************************
//函数作用:led指示灯初始化
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void led_init(void)
{
	uint8 led_index;
	unsigned short tmp;
	// Set GPIO1 pin 0, 1. 2, 3, 4, 5, 6, 7, 8, 9 is GPIO_OUT_PP
	tmp = MARST_LED_Pin | SLAVE_LED_Pin | TCMS_LED_Pin | TRAIN_LED_Pin | OCC_LED_Pin | MEDIA_LED_Pin | USB_LED_Pin | OCC_S_LED_Pin | DVA_S_LED_Pin | MEDIA_S_LED_Pin;
	GPIO1_PC0 |= tmp;
	GPIO1_PC1 &= ~tmp;
	GPIO1_PC2 |= tmp;
	GPIO1_PD &= ~tmp;
	
	for(led_index=0; led_index<LED_NUM; led_index++)
		led_onoff(led_index, 1);	
}

//***********************************************************************************************************************
//函数作用:led指示灯开关
//参数说明:无
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void led_onoff(uint8 led_no, uint8 onoff)
{
#if 1 	
       led_state=LED_CTRL_REG;
	if(led_no>=LED_NUM)
		return;

//硬件版本1
#if(HARDWARE_VERSION==1)
	if(onoff)
		led_state |= (1<<led_ctrl_bit[led_no]);
	else
		led_state &= ~(1<<led_ctrl_bit[led_no]);
//硬件版本2
#elif(HARDWARE_VERSION==2)
	if(onoff)
		led_state &= ~(1<<led_ctrl_bit[led_no]);
	else
		led_state |= (1<<led_ctrl_bit[led_no]);
#endif
	LED_CTRL_REG=led_state;
#endif
}

//***********************************************************************************************************************
//函数作用:led指示灯开关
//参数说明:无
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void led_toggle(uint8 led_no)
{
//硬件版本1
#if(HARDWARE_VERSION==1)
	//如果当前是亮熄灭
	if( led_state & (1<<led_ctrl_bit[led_no]) )
	{
		led_onoff(led_no,0);
	}
	else
	{
		led_onoff(led_no,1);
	}
//硬件版本2
#elif(HARDWARE_VERSION==2)
	//如果当前是亮熄灭
	if( ( led_state & (1<<led_ctrl_bit[led_no]) ) == 0 )
	{
		led_onoff(led_no,0);
	}
	else
	{
		led_onoff(led_no,1);
	}
#endif
}

//***********************************************************************************************************************
//函数作用:led指示灯开关
//参数说明:无
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void led_blink(uint8 led_no, uint16 on_time, uint16 off_time)
{
	if( led_no>=LED_NUM )
		return;

//硬件版本1
#if( HARDWARE_VERSION == 1 )
	//如果当前是亮 判断时间到了以后熄灭
	if(led_state & (1<<led_ctrl_bit[led_no]))
	{
		if( checktimer(&led_blink_timer[led_no],on_time) )
		{
			led_onoff(led_no, 0);
		}
	}
	//如果当前熄灭 判断时间到了以后点亮
	else
	{
		if( checktimer(&led_blink_timer[led_no],off_time) )
		{
			led_onoff(led_no, 1);
		}		
	}
//硬件版本2
#elif(HARDWARE_VERSION == 2)
	//如果当前是亮 判断时间到了以后熄灭
	if((led_state & (1<<led_ctrl_bit[led_no]))==0)
	{
		if( checktimer(&led_blink_timer[led_no],on_time) )
		{
			led_onoff(led_no, 0);
		}
	}
	//如果当前熄灭 判断时间到了以后点亮
	else
	{
		if( checktimer(&led_blink_timer[led_no],off_time) )
		{
			led_onoff(led_no, 1);
		}		
	}
#endif
	

}



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 音频控制单元 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:apu.c

//*文件作用:音频控制单元

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//通信协议关键字
#define APU_PACKET_VOLUME	0x00	//音量调节
#define APU_PACKET_FFR		0x01	//前右音量
#define APU_PACKET_FFL		0x02	//前左音量
#define APU_PACKET_FRR		0x03	//后右音量
#define APU_PACKET_FRL		0x04	//后左音量
#define APU_PACKET_BASS		0x05	//重低音
#define APU_PACKET_TREBLE	0x06	//高音
#define APU_PACKET_SWITCH	0x07	//切换通道

//通道选择控制字
#define APU_CHANNEL_A		0x07
#define APU_CHANNEL_B		0x06
#define APU_CHANNEL_C		0x05
#define APU_CHANNEL_D		0x04
#define APU_CHANNEL_NULL	0x00



static uint8 apu_channel=APU_CHANNEL_NONE;//当前选择的通道
static uint8 apu_pa_onoff_flag=0;//当前PA的状态
static uint32 apu_send_timer=0;//发送定时器

uint8 apu_error = 0;
static uint8 mute_flag = 0;
//***********************************************************************************************************************
//函数作用:音频控制单元控制平衡
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_set_balance(uint8 left, uint8 right)
{
	uint8 buf[2];
	
	//前左
	buf[0]=APU_PACKET_FFL;
	buf[1]=left;
	APU_SEND_BUF(buf,2);
	//后左
	buf[0]=APU_PACKET_FRL;
	buf[1]=left;
	APU_SEND_BUF(buf,2);
	//前右
	buf[0]=APU_PACKET_FFR;
	buf[1]=right;
	APU_SEND_BUF(buf,2);
	//后右
	buf[0]=APU_PACKET_FRR;
	buf[1]=right;
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//函数作用:音频控制各种音量
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_set_volume(uint8 volume_cmd, uint8 volume)
{
	uint8 buf[2];
	
	buf[0]=volume_cmd;
	buf[1]=volume;
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//函数作用:音频控制单元切换通道
//参数说明:0---不选择任何通道
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_switch(void)
{
	uint8 buf[2];
	
	if(apu_channel==APU_CHANNEL_NONE)
	{
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_NULL;	
	}
	else if(apu_channel==APU_CHANNEL_OCC)
	{
		apu_set_balance(0x00,saveparam.occ_volume);//关闭左声道 设置右通道音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_A;
	}
	else if(apu_channel==APU_CHANNEL_DVA)
	{
		apu_set_balance(0x00,saveparam.dva_volume);//关闭左声道 设置右通道音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_MEDIA)
	{
		apu_set_balance(saveparam.media_volume,0x00);//关闭右声道 设置左通道音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_MEDIA_DOOR)
	{
		apu_set_balance(saveparam.door_volume,0x00);//关闭右声道 设置左通道音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_DVA_DOOR)
	{
		apu_set_balance(0x00,saveparam.door_volume);//关闭左声道 设置右通道音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_OCC_MEDIA)
	{
		apu_set_balance(saveparam.media_volume-8,saveparam.occ_volume);//设置左通道和右通道的音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_A;
	}
	else if(apu_channel==APU_CHANNEL_DVA_MEDIA)
	{
		apu_set_balance(saveparam.media_volume-8,saveparam.dva_volume);//设置左通道和右通道的音量
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//函数作用:音频控制单元数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_proc(void)
{
	if(apu_error >= 40)
    	{
	  	led_onoff(BROADCAST_OCC_LED_NO, 0);
		led_onoff(BROADCAST_DVA_LED_NO, 0);
		led_onoff(BROADCAST_MEDIA_LED_NO, 0);
		my_default_data.cab_status.apu_error = 1;
    	}
	else
	{
	    	my_default_data.cab_status.apu_error = 0;
	}

	//定时发送
	if(checktimer(&apu_send_timer,1000))
	{		
		//设置PA线
		if(apu_pa_onoff_flag==0)
		{
			APU_PAOUT_CTRL_REG &= ~(1<<APU_PAOUT_CTRL_BIT1);
			APU_PAOUT_CTRL_REG &= ~(1<<APU_PAOUT_CTRL_BIT2);
		}
		else
		{
			APU_PAOUT_CTRL_REG |= (1<<APU_PAOUT_CTRL_BIT1);
			APU_PAOUT_CTRL_REG |= (1<<APU_PAOUT_CTRL_BIT2);
		}
	
		//设置总音量
		apu_set_volume(APU_PACKET_VOLUME,saveparam.global_volume);
		//设置重低音
		apu_set_volume(APU_PACKET_BASS,saveparam.apu_bass);
		//设置高音
		apu_set_volume(APU_PACKET_TREBLE,saveparam.apu_treble);
		//切换通道
		apu_switch();
	}
}

//***********************************************************************************************************************
//函数作用:音频控制单元设置PA_ON
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_set_pa_out(uint8 onoff)
{
	apu_pa_onoff_flag=onoff;
	if(apu_pa_onoff_flag==0)
	{
		APU_PAOUT_CTRL_REG&=~(1<<APU_PAOUT_CTRL_BIT1);
		APU_PAOUT_CTRL_REG&=~(1<<APU_PAOUT_CTRL_BIT2);
	}
	else
	{
		APU_PAOUT_CTRL_REG|=(1<<APU_PAOUT_CTRL_BIT1);
		APU_PAOUT_CTRL_REG|=(1<<APU_PAOUT_CTRL_BIT2);
	}
}


static void apu_set_door_out(uint8 onoff)
{
	if(onoff==0)
	{
		APU_DOOROUT_CTRL_REG&=~(1<<APU_DOOROUT_CTRL_BIT);		
	}
	else
	{
		APU_DOOROUT_CTRL_REG|=(1<<APU_DOOROUT_CTRL_BIT);		
	}
}

//***********************************************************************************************************************
//函数作用:音频控制单元获取PA_ON
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 apu_get_pa_out(void)
{
	return apu_pa_onoff_flag;
}

//***********************************************************************************************************************
//函数作用:音频控制单元获取PA_IN
//参数说明:
//注意事项:
//返回说明:在播放返回1
//***********************************************************************************************************************
static uint8 apu_get_pa_in(void)
{
	if(APU_PA_IN_REG&(1<<APU_PA_IN_BIT))
		return 0;
	else 
		return 1;
}

static uint8 apu_get_door_in(void)
{
	if(APU_DOOR_IN_REG&(1<<APU_DOOR_IN_BIT))
		return 0; // 读到高电平是播放完
	else 
		return 1;
}

static uint8 apu_get_test_in(void)
{
	if(APU_DOOR_IN_REG&(1<<APU_TEST_IN_BIT))
		return 0; // 读到高电平没播
	else 
		return 1;
}

//***********************************************************************************************************************
//函数作用:音频控制单元获取OCC_IN
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 apu_get_occ_in(void)
{
	if(APU_OCC_IN_REG&(1<<APU_OCC_IN_BIT))
		return 0;
	else 
		return 1;
}

//***********************************************************************************************************************
//函数作用:音频控制单元设置通道
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_set_channel(uint8 channel)
{
       if(mute_flag) //静音处理
       {
           apu_channel = APU_CHANNEL_NONE;
	     apu_dev->set_pa_out(0);//释放PA线
       }
	if(apu_channel!=channel)
	{		
		apu_channel=channel;
		apu_switch();
		apu_switch();
		apu_switch();
	}
}

//***********************************************************************************************************************
//函数作用:音频控制单元初始化函数
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static void apu_init(void)
{	
	//设置管脚属性为输出
	GPIO2_PC0|=(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC1&=~(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC2|=(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC0|=(1<<APU_PAOUT_CTRL_BIT2);
	GPIO2_PC1&=~(1<<APU_PAOUT_CTRL_BIT2);
	GPIO2_PC2|=(1<<APU_PAOUT_CTRL_BIT2);
/*
	//配置PALINE检测管脚属性为输入+弱上拉
	GPIO2_PC0|=(0x0001<<APU_PA_IN_BIT);
	GPIO2_PC1|=(0x0001<<APU_PA_IN_BIT);
	GPIO2_PC2&=~(0x0001<<APU_PA_IN_BIT);
	//配置OCC检测管脚属性为输入+弱上拉
	GPIO2_PC0|=(0x0001<<APU_OCC_IN_BIT);
	GPIO2_PC1|=(0x0001<<APU_OCC_IN_BIT);
	GPIO2_PC2&=~(0x0001<<APU_OCC_IN_BIT);
*/			
	//初始化通信端口
	APU_PORTINIT(APU_BAUDRATE);
}

static void apu_set_mute_flag(uint8 flag)
{
    mute_flag = flag;
}
static uint8 apu_get_mute_flag()
{
    return mute_flag;
}

//输出虚拟设备类
apu_dev_t sh9_apu_dev=
{
	apu_init,
	apu_proc,
	apu_set_pa_out,
	apu_get_pa_out,
	apu_get_pa_in,
	apu_get_occ_in,
	apu_set_channel,
	apu_set_mute_flag,
	apu_get_mute_flag,
	apu_set_door_out,
	apu_get_door_in,
	apu_get_test_in,
};



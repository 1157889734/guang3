//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 音频控制单元 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:apu.h

//*文件作用:音频控制单元

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef APU_H
#define APU_H



//硬件定义
//#define APU_PAOUT_CTRL_REG		GPIO1_PD 	//shenzhen  No.1 24 cars
//#define APU_PAOUT_CTRL_BIT1		12
//#define APU_PAOUT_CTRL_BIT2		15

#define APU_PAOUT_CTRL_REG		GPIO2_PD 	//xi'an Line No.2 
#define APU_PAOUT_CTRL_BIT1		10
#define APU_PAOUT_CTRL_BIT2		14

#define APU_DOOROUT_CTRL_REG		GPIO2_PD 	//xi'an Line No.2 
#define APU_DOOROUT_CTRL_BIT		15


//#define APU_PA_IN_REG			GPIO2_PD	//shenzhen  No.1 24 cars
//#define APU_PA_IN_BIT			0
#define APU_PA_IN_REG			RD_EXT_IO_STAUTS	//xi'an Line No.2 
#define APU_PA_IN_BIT			(15)
#define APU_OCC_IN_REG			RD_EXT_IO_STAUTS
#define APU_OCC_IN_BIT			(11)
#define APU_DOOR_IN_REG			RD_EXT_IO_STAUTS	//xi'an Line No.2 
#define APU_DOOR_IN_BIT			(4)
#define APU_TEST_IN_BIT			(13)

//输入输出端口定义--根据硬件接口改变
#define APU_BAUDRATE		100000			//波特率
#define APU_PORTINIT		i2c0_init		//通信端口初始化
//#define APU_RECEIVE_CHAR	i2c0_read_char		//通信端口接收
#define APU_SEND_BUF		i2c0_write_buf		//通信端口发送



//通道号参数
#define APU_CHANNEL_NONE		0x00
#define APU_CHANNEL_OCC			0x01
#define APU_CHANNEL_DVA			0x02
#define APU_CHANNEL_MEDIA		0x03
#define APU_CHANNEL_OCC_MEDIA		0x04
#define APU_CHANNEL_DVA_MEDIA		0x05
#define APU_CHANNEL_MEDIA_DOOR		0x06	//关门提示音通道--虚拟通道 其实就是APU_CHANNEL_MEDIA
#define APU_CHANNEL_DVA_DOOR		0x07	//关门提示音通道--虚拟通道 其实就是APU_CHANNEL_MEDIA

extern uint8 apu_error;

//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	void (*set_pa_out)(uint8);
	uint8 (*get_pa_out)(void);
	uint8 (*get_pa_in)(void);
	uint8 (*get_occ_in)(void);
	void (*set_channel)(uint8);
	void(*set_mute_flag)(uint8);
	uint8(*get_mute_flag)(void);
	void (*set_door_out)(uint8);
	uint8 (*get_door_in)(void);
	uint8 (*get_test_in)(void);	
}apu_dev_t;
extern apu_dev_t sh9_apu_dev;



#endif



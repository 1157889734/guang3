//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 多媒体播放器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:media.h

//*文件作用:多媒体播放器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef MEDIA_H
#define MEDIA_H



//硬件定义
#define MEDIA_PLAY_IN_REG		GPIO2_PD
#define MEDIA_PLAY_IN_BIT		(6)



//媒体播放通信指示灯号
#define MEDIA_LED_NO		5


//输入输出端口定义--根据硬件接口改变
#define MEDIA_BAUDRATE		9600			//波特率
#define MEDIA_PORTINIT		uart0_init		//通信端口初始化
#define MEDIA_RECEIVE_CHAR	uart0_read_char		//通信端口接收
#define MEDIA_SEND_BUF		uart0_write_buf		//通信端口发送
#define MEDIA_SEND_CHAR		uart0_write_char	//通信端口发送
#define MEDIA_FIFO_BACKUP	uart0_fifo_out_backup	//底层指针保存
#define MEDIA_FIFO_RESUME	uart0_fifo_out_resume	//底层指针回读



//缓冲区长度定义--根据具体应用改变
#define MEDIA_BUF_SIZE		255


typedef struct
{
    __packed struct
	{
		uint8 cctv_encode_err:1;// 司机室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_encode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}cab1; //司机室1
	__packed struct
	{
		uint8 cctv_encode_err:1;// 司机室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_encode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}cab2; //司机室1
	__packed struct
	{
		uint8 cctv_encode_err:1;// 客室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_decode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}car1; //客室1
	__packed struct
	{
		uint8 cctv_encode_err:1;// 客室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_decode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}car2; //客室2
	__packed struct
	{
		uint8 cctv_encode_err:1;// 客室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_decode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}car3; //客室3
	__packed struct
	{
		uint8 cctv_encode_err:1;// 客室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_decode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}car4; //客室4
	__packed struct
	{
		uint8 cctv_encode_err:1;// 客室监控编码板
		uint8 camera1:1;//摄像头1
		uint8 camera2:1;// 摄像头1
		uint8 media_decode_err:1;// 媒体编码板
		uint8 reserve1:4;// 
	}car5; //客室5
	uint8 reserve[2];
}device_state_t;

//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	void (*send_data)();
	uint8 (*get_playing)(void);
	
	uint16 comm_error;//串口通信错误
	
	device_state_t device_state;
	
}media_dev_t;
extern media_dev_t sh9_media_dev;

typedef struct
{
    __packed struct
	{
		uint8 emergency:1;// 1:正在播放紧急广播
		uint8 door_sides:2; //10 左侧  01 右侧11 两侧
		//uint8 open_right_door:1;// 开右门
		//uint8 open_left_door:1;// 开左门
		uint8 up_dir:1;// 上行
		uint8 down_dir:1;// 下行
		uint8 close_delay:1;//  延迟关机
		uint8 leave_broadcast:1;//  离站 
		uint8 arrived_broadcast:1;
	}status;
	__packed struct
	{
		uint8 master_slave:1;//  1: master  0:slave
		uint8 key:1;//钥匙
		uint8 reserve:1;
	}cab;
	uint8 start_station_id;
	uint8 destination_station_id;
	uint8 current_station_id;
	uint8 next_station_id;   //6
	uint8 emergency_id;
	uint8 city_id;
	uint8 line_id;
	uint8 velocity_h; //10
	uint8 velocity_l;
	
	uint8 year;
	uint8 month;
	uint8 date;
	uint8 hour;
	uint8 minute;  //16
	uint8 second;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}tc1;
	*/
	uint8 tc1_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}mp1;
	*/
	uint8 mp1_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}m;
	*/
	uint8 m_doors_unlock;  //20
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}mp2;                   
	*/
	uint8 mp2_doors_unlock;
/*
	__packed struct
	{
		uint8 door1:1; 
		uint8 door2:1; 
		uint8 door3:1; 
		uint8 door4:1; 
		uint8 door5:1; 
		uint8 door6:1; 
		uint8 door7:1;// 
		uint8 door8:1;
	}tc2;
	*/
	uint8 tc2_doors_unlock;
      uint8 reserve_for_door;   //23
	__packed struct
	{
		uint8 dva:1; 
		uint8 mvb:1; 
		uint8 atc:1; 
		uint8 fdu:1; //终点站屏 
		uint8 fire_alarm_tc1_cab:1;
		uint8 apu:1;
		uint8 dcp:1;
		uint8 reserve:1;
	}pcu1;
	__packed struct
	{
		uint8 dva:1; 
		uint8 mvb:1; 
		uint8 atc:1; 
		uint8 fdu:1; //终点站屏 
		uint8 fire_alarm_tc2_cab:1;
		uint8 apu:1;
		uint8 dcp:1;
		uint8 reserve:1;
	}pcu2;                        //25
	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//对讲控制器 
		uint8 amp1:1; //功放
		uint8 amp2:1; //终点站屏 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_tc1_saloon:1;
		uint8 reserve:1;
	}scu1_1;
	__packed struct
	{
		uint8 idu1:1; //内部屏
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //侧部屏
		uint8 reserve:4; 
	}scu1_2;                 //27
	__packed struct
	{
		uint8 drmd1:1;  //动态地图 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu1_3;
	uint8 reserve_1;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//对讲控制器 
		uint8 amp1:1; //功放
		uint8 amp2:1; //终点站屏 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_mp1_saloon:1;
		uint8 reserve:1;
	}scu2_1;
	__packed struct
	{
		uint8 idu1:1; //内部屏
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //侧部屏
		uint8 reserve:4; 
	}scu2_2;              //31
	__packed struct
	{
		uint8 drmd1:1;  //动态地图 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu2_3;
	uint8 reserve_2;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//对讲控制器 
		uint8 amp1:1; //功放
		uint8 amp2:1; //终点站屏 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_m_saloon:1;
		uint8 reserve:1;
	}scu3_1;
	__packed struct
	{
		uint8 idu1:1; //内部屏
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //侧部屏
		uint8 reserve:4; 
	}scu3_2;
	__packed struct
	{
		uint8 drmd1:1;  //动态地图 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu3_3;            //36
	uint8 reserve_3;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//对讲控制器 
		uint8 amp1:1; //功放
		uint8 amp2:1; //终点站屏 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_mp2_saloon:1;
		uint8 reserve:1;
	}scu4_1;
	__packed struct
	{
		uint8 idu1:1; //内部屏
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //侧部屏
		uint8 reserve:4; 
	}scu4_2;                             //39
	__packed struct
	{
		uint8 drmd1:1;  //动态地图 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu4_3;
	uint8 reserve_4;

	__packed struct
	{
		uint8 scu:1; 
		uint8 intercom:1;//对讲控制器 
		uint8 amp1:1; //功放
		uint8 amp2:1; //终点站屏 
		uint8 pea1:1;
		uint8 pea2:1;
		uint8 fire_alarm_tc2_saloon:1;
		uint8 reserve:1;
	}scu5_1;
	__packed struct
	{
		uint8 idu1:1; //内部屏
		uint8 idu2:1; 
		uint8 sdu1:1; 
		uint8 sdu2:1; //侧部屏
		uint8 reserve:4; 
	}scu5_2;                          //43
	__packed struct
	{
		uint8 drmd1:1;  //动态地图 
		uint8 drmd2:1; 
		uint8 drmd3:1; 
		uint8 drmd4:1; 
		uint8 drmd5:1; 
		uint8 drmd6:1; 
		uint8 drmd7:1;// 
		uint8 drmd8:1;
	}scu5_3;
	uint8 reserve_5;
	uint8 reverse[2];
}media_send_packet_t;

extern media_send_packet_t media_send_packet;
#endif



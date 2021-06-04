#ifndef Com_proc_H_
#define Com_proc_H_



//设备类型定义
#define PISC					0x01	//中央控制器
#define BCB						0x02	//广播控制盒
#define LCU						0x03	//本地控制器
#define DMP						0x04
#define	EHP						0x05	//对讲控制器
#define IDU						0x06	//内部屏
#define FDU						0x07    //前部屏
#define SDU						0x08    //侧部屏

#define PROCESS_PACKET			0x00	//过程数据包
#define P2P_PACKET				0x01	//点对点命令包
#define TOKEN_PACKET			0x02	//令牌包
#define CLR_TOKEN				0x03	//清除总线上的令牌包
#define DOWNLOAD_PACKET		0x04	//下载包
#define keep_bus_alone			0x08	//独占总线
#define giveup_bus				0x0C	//放弃独占总线
#define get_devic_statu  			0x0E   	//回读某设备



__packed typedef struct
{
	//触发标志---低位在前
	__packed struct
	{
		u8 leave_broadcast:1;
		u8 arrived_broadcast:1;
		u8 jump:1;
		u8 urgent_broadcast:1;
		u8 closedoor:1;
		u8 opendoor:1;
		u8 leave:1;
		u8 reach_stop:1;
	}trigger_hbit;

	u8 start_station;//起始站
	u8 end_station;//终点站
	u8 current_station;//当前站
	u8 next_station;//下一站
	u8 jump_broadcast_no;//越站广播代码
	u8 urgent_broadcast_no;//紧急广播代码
	
	//状态标志---低位在前
	__packed struct
	{
		u8 jump_valid:1;
		u8 active:1;
		u8 master:1;
		u8 dir_valid:1;
		u8 door_right_sides:1;
		u8 door_left_sides:1;
		u8 dir_down:1;
		u8 dir_up:1;
	}status_hbit;
	

	u8 line_no;//城市号

	u8 year;
	u8 month;
	u8 date;
	
	
	u32 broadcast_pre;
	
	//广播信源---低位在前17
	__packed struct
	{
		u8 broadcast_req:1;
		u8 broadcasting:1;
		u8 broadcast_type:6;
	}broadcast_signal;

	//24个报警器状态        18-25
	u8 esu_status[8];


	u8 setup_data_val;
__packed struct
	{
		u8 other:6;
		u8 display_welcome:1;
		u8 dir_up:1;
	}   setup_data_type;          //27 setup_data_type.display_welcome
		


		__packed struct
	{
		u8 interverse:6;
		u8 mode:2;
	}mode_run;
				//28

	__packed struct
	{
		u8 reserve:2;
		u8 mp3_busying:1;
		u8 mp3_trigger:1;
		u8 A_car_active:1;
		u8 F_car_active:1;
		u8 occ_trigger:1;
		u8 time_valid:1;
	}timevalid_linenum;

	u8 hour;
	u8 statu_reserve;
	u8 media_error_reserve;
	
}pisc_data_t;

__packed typedef struct {
	u8 dest_netnum;
	//低位在前
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;
	}src_eqnum;
	u8 cmd;
	u8 len;
	pisc_data_t data_;
}tFrameHeader;




extern void com1_proc(void);






//广4 动态地图老协议
__packed typedef struct {
	u8 receiverType;
	u8 receiverId;
	u8 transmitType;
	u8 transmitId;
	u8 tokenControl;
	u8 frameNo;
	u8 frameLength;
	u8 receiverKeyword;
	u8 lineId;
	u8 stationH;
	u8 stationL;
	u8 tmsDoor;
	u8 doorSignal;
	u8 announceActivation;
}tFrameHeaderOld;

//广4 动态地图新协议
__packed typedef struct {
	u8 receiverType;
	u8 receiverId;
	u8 transmitType;
	u8 transmitId;
	u8 tokenControl;
	u8 frameNo;
	u8 frameLength;
	u8 receiverKeyword;
	//触发标志---低位在前
	__packed struct
	{
		u8 leave_broadcast:1;
		u8 arrived_broadcast:1;
		u8 jump:1;
		u8 urgent_broadcast:1;
		u8 closedoor:1;
		u8 opendoor:1;
		u8 leave:1;
		u8 reach_stop:1;
	}trigger_hbit;

	u8 start_station;//起始站
	u8 end_station;//终点站
	u8 current_station;//当前站
	u8 urgent_broadcast_no;//紧急广播代码

	//状态标志---低位在前
	__packed struct
	{
		u8 active_a:1;
		u8 active_f:1;
		u8 master:1;
		u8 dir_valid:1;
		u8 door_right_sides:1;
		u8 door_left_sides:1;
		u8 dir_down:1;
		u8 dir_up:1;
	}status_hbit;

}tFrameHeaderNew;









#endif



//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆参数保存◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//◆文件名称:save.h
//◆文件说明:
//◆文件作者:翟  鹏
//◆创建日期:2007年5月
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆

#include "error.h"

#ifndef SAVE_H
#define SAVE_H



//参数保存地址
#define PARAM_ADDR	FLASH_PARAM_BASE

#define POLL_NUM_MAX   12     //个数为saveparam_t结构体中巡检表中个数

#define STATION_NUM_MAX		66	//最多支持的站数
#define BROADCAST_NUM_MAX	9	//最多支持的广播通道
#define LINE_NUM_MAX         3// 支持3条线


//参数保存数据结构类型
typedef struct
{
	uint32 version;//版本号

	//终点站提示语播放列表
	uint8 endstation_mind_list[30][5];
	
	//令牌巡检表
	uint8 token_table_dev_id[TBA_M_TABLE_NUM];
	uint16 token_table_interval[TBA_M_TABLE_NUM];
	
	uint16 stations_code_table_up[STATION_NUM_MAX];//站的代码表
	uint16 stations_code_table_down[STATION_NUM_MAX];//站的代码表
	uint8 station_total_num;//站总数		
	
	uint16 door_sides_distance;
	
	//车辆开门侧指示
	uint8 door_sides_up[STATION_NUM_MAX];
	uint8 door_sides_down[STATION_NUM_MAX];

	//触发离站广播的距离
	uint16  leave_broc_distance[STATION_NUM_MAX]; 
	//触发到站广播的距离
	uint16 arrive_broc_distance[STATION_NUM_MAX]; 

       //两站之间距离
	uint16 distance[LINE_NUM_MAX][STATION_NUM_MAX];

	//终点站预报站广播
	//uint16 dva_pre_dest_list[STATION_NUM_MAX][DVA_LIST_MAX];
	uint32   broadcast_delay_time[STATION_NUM_MAX];  //手动模式下延迟触发报站时间

	//站名
	uint8 station_name[STATION_NUM_MAX][DVA_LIST_MAX*2];
	
	//关门提示音
	uint16 dva_door_list[DVA_LIST_MAX];
	//关门提示音播放次数
	uint8 close_door_playtimes;
	//特殊广播次数
	uint8 special_broadcast_times[30];
	//广播优先级 广播优先级:OCC、  乘客对讲、 司机对讲、 人工广播、
	// 关门、DVA、 MEDIA 、紧急广播、测试语音
	uint8 broadcast_priority[BROADCAST_NUM_MAX];
	
	//音量
	uint8 global_volume;//总音量
	uint8 occ_volume;//通道1音量
	uint8 dva_volume;//通道2音量
	uint8 media_volume;//通道3音量
	uint8 door_volume;//关门提示音 音量
	uint8 apu_bass;//重低音
	uint8 apu_treble;//高音
	uint8 global_volume_pcent;//总音量百分比
	uint8 occ_volume_pcent;//通道1音量百分比
	uint8 dva_volume_pcent;//通道2音量百分比
	uint8 media_volume_pcent;//通道3音量百分比
	uint8 door_volume_pcent;//关门提示音 音量百分比
	uint8 apu_bass_pcent;//重低音百分比
	uint8 apu_treble_pcent;//高音百分比
	uint8 media_sametime;//是否同时媒体播放
	uint8 door_broadcast_flag;//是否播放关门提示音

	//广播盒控制音量
	uint8 dcp_pa_listen_volume;  // 广播监听音量
	uint8 dcp_cc_listen_volume;  // 司机对讲监听音量
	uint8 dcp_pc_listen_volume; // 乘客对讲监听音量
	uint8 dcp_pa_mic_volume;  // 广播麦克音量
	uint8 dcp_cc_mic_volume;  // 司机对讲麦克音量
	uint8 dcp_pc_mic_volume;  // 乘客对讲麦克音量
	
		
	uint8 panel_volume_param;//客室功放参数 MODE_PRESET<<4|VOLUME

	//begin add by hk 2010-5-24 for 异常存储
	uint32 error_flash_addr;   //最后一次保存地址
	uint8  re_write_flash;      //flash写完一遍，从首地址从头开始标志
	error_packet_t pre_error_pack;//上一次存储的数据 2010-9-9
	//end add by hk 2010-5-24
	
	//校验和			
	uint8 crc;
}saveparam_t;
extern saveparam_t saveparam;




//接口函数
extern void save_init(void);
extern void save_proc(void);
extern void save_set_task(void);
extern void save_set_erase(void);
extern void save_set_update(void);
extern void save_load_default(void);


#endif



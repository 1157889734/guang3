#ifndef TypeDef_H_
#define TypeDef_H_
#include "include.h"



typedef enum{
	BMP_FAIL = 0,
	BMP_OK = 1
}tBmpUpdateFlag;




typedef enum{
	LED_COLOR_RED = 1,
	LED_COLOR_GREEN = 2,
	LED_COLOR_RG = 3,
	LED_COLOR_DARK = 4
}tLedColor;




typedef enum {
	DOOR_SIDE_DARK,
	DOOR_SIDE_LIGHT,
	DOOR_SIDE_FLASH
}eDoorSideMode;



typedef struct {
	tLedColor mDoorSideColor;
	eDoorSideMode mDoorSideStat;
}tDoorStat;


typedef enum{
	BMP_TYPE_LINE1 = 1,
	BMP_TYPE_LINE2 = 2,
	BMP_TYPE_LINE3 = 3,
	BMP_TYPE_MSG = 4,
	BMP_TYPE_AD = 5,
	BMP_TYPE_EME = 6,
	CONFIG_LINE1 = 7,
	CONFIG_LINE2 = 8,
	CONFIG_LINE3 = 9,
}tBmpType;

typedef enum{
	LINE1_CONFIG = 1,
	LINE2_CONFIG = 2,
	LINE3_CONFIG = 3,
}tConfigPram;	

typedef enum{
	LANG_ENGLISH = 0,
	LANG_CHINESE = 1
}tLanguage;

typedef struct{
	tBmpType type;
	u8 index;  //序号   下标从0开始，
	tLanguage langusge;
	
}tBmpParam;




typedef enum{
	DISP_TYPE_NORMAL,
	DISP_TYPE_DOWNING,
	DISP_TYPE_ERROR,
	DISP_TYPE_ERROR_DOWN,
	DISP_TYPE_DARK,
	DISP_TYPE_NO_PROCESS,
	DISP_TYPE_VERSION,
	DISP_TYPE_TEST_ALL_LIGHT,
	DISP_TYPE_TEST_LOG,
	DISP_TYPE_LED_TEST,  //过程数据测试
}tDispType;

typedef enum{
	SHIFT_NONE,
	SHIFT_LEFT,
	SHIFT_RIGHT,
}tShiftType;

typedef struct  {
	tDispType disp_type;
	u8 row_index;
	u16 column_index;
	tShiftType shift_type;
	u16 width;
	u32 delay_cnt;
	u32 scr_sav_tick;
	u32 switch_disp_tick;
	u8 change_lang_flag;
	tLanguage curr_lang;
	tBmpType curr_line;
	u8 curr_station;
}tDispStat;

typedef enum{
	DISP_LANG_ENGLISH_ONLY = 1,
	DISP_LANG_CHINESE_ONLY = 2,
	DISP_LANG_BOTH = 3,
}tDispLanguageType;


typedef struct{
	u8 ad_num;					//广告总数
	tDispLanguageType disp_type;       //显示语言种类
	u8 scr_saver;                  //屏保时间
}tSysConfig;



__packed typedef struct {
	__packed struct {
		u8 total_start:1;  //本次升级开始标志
		u8 total_end:1;    //本次升级结束标志
		u8 bmp_start:1;    //本张图片第1包标志
		u8 bmp_end:1;    //本张图片最后1包标志
		u8 language:1;	//语言 0:英文    1:波斯文
		u8 reserve:3;   
	}flag_bit;
	u8 packet_current;
	u8 type;
	u8 index;
	u16 width;
	u8 color;
	
}tBmpHeader;


typedef struct{
	tBmpUpdateFlag success_flag;
	u8 packet_old;
	u8 *pt;
	u16 offset;
	u32 tick;
}tUpdateBmpSt;


typedef enum{
	TRIGER_PRE_BROADCAST= 0,	 
	TRIGER_ARRIVE_BROADCAST = 1,   // 没有连接显示功能
	TRIGER_DOOR_OPEN  =2,
	TRIGER_DOOR_CLOSE,
	TRIGER_START_STATION,  //起始站广播
	TRIGER_NEXT_TERMINAL,  //终点站广播
	TRIGER_PRE_BROADCAST_1,        //全自动模式下的预到站信息
}tTrigerType;

typedef enum{
	DOOR_SIDE_LEFT = 0,
	DOOR_SIDE_RIGHT,
	DOOR_SIDE_BOTH,
	DOOR_SIDE_NONE,
}tDoorSide;

typedef enum{
	CONFIG_TYPE_PRE = 0,
	CONFIG_TYPE_ARRIVE = 1,
	CONFIG_TYPE_ALL = 2,
	CONFIG_TYPE_PRE_COLOR = 3,
	CONFIG_TYPE_ARRIVE_COLOR = 4,
}tConfigType;


//触发信号相关变量
typedef struct{
	u8 eme_flag;
	u8 welcome_flag;
	u8 eme_index;   //下标从1开始
	tTrigerType triger_type;
	u8 triger_flag;
	u8 line_no;			//下标从1开始
	u8 station_next;	 	//下标从1开始
	u8 station_current;	//下标从1开始
	u8 station_start;		//下标从1开始
	u8 station_end;		//下标从1开始
	tDoorSide door;	
	u8 config_buf[CONFIG_TOTAL_LEN];
	u8 english_config[CONFIG_TOTAL_LEN];
	u8 chinese_config[CONFIG_TOTAL_LEN];
	u8 ad_english_buf[CONFIG_TOTAL_LEN];
	u8 ad_chinese_buf[CONFIG_TOTAL_LEN];
	u8 ad_english_num;
	u8 ad_english_index;
	u8 ad_chinese_num;
	u8 ad_chinese_index;
	char station_msg_disp_e_time;
	char station_msg_disp_c_time;
	char station_msg_disp_time;

	tLedColor disp_color;
	tLedColor prev_english_color;
	tLedColor prev_chinese_color;
	tLedColor arrive_english_color;
	tLedColor arrive_chinese_color;

	/*****************
	//关门
	tLedColor door_open_english_color;
	tLedColor door_open_chinese_color;
	//开门
	tLedColor door_close_english_color;
	tLedColor door_close_chinese_color;
	//越站
	

	
	******************/
}tTrigerValue;



#endif


#ifndef __DEF_H__
#define __DEF_H__

#define VERSION "1.0.11.32"

#define CONFIG_PATH		"/root/usr_exe/config.ini"
#define UPDATE_FILE_NAME	"setTempBroName.ini"
#define UPDATE_FILE_NAME_PRO	"src"
#define DOWNLOAD_FILE_PATH  "/usr/setTempBroName.ini"
#define DOWNLOAD_FILE_PRO  "/usr/src"
#define UPDATE_FILE_PATH	"/udisk/rootfs_uclibc_jffs2.img"
#define NVR_FILE_DIR 		"/hdd/sda1/"

#define RTSP_URL_HD_CAB 		"rtsp://%s:554/video0.sdp"
#define RTSP_URL_SD_CAB 		"rtsp://%s:554/video1.sdp"
#define RTSP_URL_HD_CARRIAGE 	"rtsp://%s:554/video0.sdp"
#define RTSP_URL_SD_CARRIAGE 	"rtsp://%s:554/video1.sdp"

#define CAMERA_COUNT			24
#define CAB_HEAD_CAMERA_ID		0
#define CAB_TAIL_CAMERA_ID		CAMERA_COUNT-1
#define CARRIAGE_CAMERA_COUNT	3

#define ADDR_COUNT					30
#define TEMP_BROADCAST_COUNT       100    // 每页9条紧急广播
#define EMERGENCY_BROADCAST_COUNT 100
#define EMERGENCY_CLIENTTALKBACK_COUNT  18
#define LINE_COUNT						15
#define LINE1_COUNT						16
#define LINE2_COUNT						16
#define LINE3_COUNT						14
#define LINE9_COUNT						25

#define LOG_COUNT 						105
#define ERRORLIST_COUNT					105
#define ERROR_COUNT						23


#define PASSENGER_ALARM_COUNT		24  	//客室1(4个)+ ... + 客室6(4个)
#define DOOR_OPEN_ALARM_COUNT		48	//客室1(8个)+ ... + 客室6(8个)
#define FIRE_ALARM_COUNT				48	//客室1(8个)+ ... + 客室6(8个)

#define RTSP_CHANNEL_COUNT		6
#define VO_CHANNEL_COUNT		6
#define VPSS_GRP_COUNT			6

#define VDEC_CHN_HD_CARRIAGE		0
#define VDEC_CHN_SD_CARRIAGE1		1
#define VDEC_CHN_SD_CARRIAGE2		2
#define VDEC_CHN_SD_CARRIAGE3		3
#define VDEC_CHN_SD_CARRIAGE4		4
#define VDEC_CHN_HD_CAB			5
#define VDEC_CHN_SD_CAB1			6
#define VDEC_CHN_SD_CAB2			7
#define VDEC_CHN_PLAYBACK			8

#define VO_CHN_PLAYBACK			VO_CHANNEL_COUNT
#define VPSS_GRP_CHN_PLAYBACK		VPSS_GRP_COUNT

#define FRAME_WIDTH_CAB_HD		1280
#define FRAME_HEIGHT_CAB_HD		720
#define FRAME_WIDTH_CAB_SD		640
#define FRAME_HEIGHT_CAB_SD		480
#define FRAME_WIDTH_CARRIAGE_HD	1280
#define FRAME_HEIGHT_CARRIAGE_HD	720
#define FRAME_WIDTH_CARRIAGE_SD	640
#define FRAME_HEIGHT_CARRIAGE_SD	480

#define FRAME_WIDTH_CAB_HD_PLAYBACK  	1280
#define FRAME_HEIGHT_CAB_HD_PLAYBACK	720
#define FRAME_WIDTH_CARRIAGE_HD_PLAYBACK  	1280
#define FRAME_HEIGHT_CARRIAGE_HD_PLAYBACK	720

#define VO_RECT_PLAYBACK_X			0
#define VO_RECT_PLAYBACK_Y			0
#define VO_RECT_PLAYBACK_WIDTH		1024
#define VO_RECT_PLAYBACK_HEIGHT		680

#define VO_RECT_HD_X			0
#define VO_RECT_HD_Y			0
#define VO_RECT_HD_WIDTH		1024
#define VO_RECT_HD_HEIGHT		640

#define VO_RECT_4D1_1_X		0
#define VO_RECT_4D1_1_Y		0
#define VO_RECT_4D1_1_WIDTH	510
#define VO_RECT_4D1_1_HEIGHT	310

#define VO_RECT_4D1_2_X		510
#define VO_RECT_4D1_2_Y		0
#define VO_RECT_4D1_2_WIDTH	510
#define VO_RECT_4D1_2_HEIGHT	310

#define VO_RECT_4D1_3_X		0
#define VO_RECT_4D1_3_Y		340
#define VO_RECT_4D1_3_WIDTH	510
#define VO_RECT_4D1_3_HEIGHT	310

#define VO_RECT_4D1_4_X		510
#define VO_RECT_4D1_4_Y		340
#define VO_RECT_4D1_4_WIDTH	510
#define VO_RECT_4D1_4_HEIGHT	310

#define TAB_PLAY_REALTIME_4D1	0
#define TAB_PLAY_REALTIME_HD	1

#define TAB_PLAYBACK_SELECT_FILE 	0
#define TAB_PLAYBACK_FILE_PLAY		1

#define TAB_BROADCAST 							0
#define TAB_ADDR_NAME							1
#define TAB_TEMP_BROADCAST 					2
//#define TAB_URGENCY_BROADCAST 				3
//#define TAB_PASSGENER_INTERCOM_BROADCAST 	4
#define TAB_CHOOSE_LINE						3
//#define TAB_LINE1_ADDR							6
//#define TAB_LINE2_ADDR							7
//#define TAB_LINE3_ADDR							8
#define TAB_LINE9_ADDR							4
#define TAB_LOG								5
#define TAB_SYSSET								6
#define TAB_ERROR								7
#define TAB_BALCK								8
#define TAB_RECV_DATE								9
#define TAB_VOLUME 							10
#define TAB_SYSTEM_MESSAGE 							11




#define MIN_SPELL_INTERVAL  10
#define MAX_SPELL_INTERVAL  60
#define MIN_WARNNING_SPELL_INTERVAL  3
#define MAX_WARNNING_SPELL_INTERVAL  10

#define PLAYBACK_NVR_NORMAL	0	//播放正常录像
#define PLAYBACK_NVR_REPLACE	1	//播放冗余录像

#endif

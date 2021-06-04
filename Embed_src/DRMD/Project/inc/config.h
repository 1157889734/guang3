#ifndef __CONFIG_H
#define __CONFIG_H

#include "include.h"


#define IDU_COMPILE					1
#define FDU_COMPILE					0
#define SDU_COMPILE					0

#define AUTO  						 3
#define MAUAL						 4

/***********************************************************************
	选择参数:
	 LIGHT_SIDE : 1    和 CHOISE_PTU :0                
	 
**********************************************************************
*功能选择
*  1:  门侧灯
*  2: PTU版本选择
*  	2.1:  KIND_RUN 动态地图模式还是IDU模式
*  3:  WATCH DOG
*  4:  换线功能
*  5:  测试所有灯亮
*  6: 半自动和自动运行切换
*  7:显示内容方式==>把2种语言分成2次独立显示，可以分颜色和静态显示/ 把2种语言合成一整图片显示，不存在静止图片和分颜色显示
**********************************************************************
注:
	广3、广4 PTU (CHOISE_PTU ==1 )决定(KIND_RUN = 1) ,如果(CHOISE_PTU ==0 ) ==> (KIND_RUN = 0) 
	
***********************************************************************/
#define LIGHT_SIDE                			1           //门侧灯
#define CHOISE_PTU                               0           // 1: 广3 广4         //0         伊朗PTU
#define WATCH_DOG 					1
#define LINE_CHANGE			 		0		//换线功能
#define DEBUG_ALL_LIGHT_ON			0		//测试用，点亮全部LED
#define MODE_CHANGE				0		
#define DISPLAY_KIND				0         // 0:语言分开独立显示，这样存在图片静止  
										    // 1: 2种语言合成一张图片显示，这样不存在静止图片
/*2.1**********************************************************************/

		#define KIND_RUN					1            // 1 : 动态地图的运行模式=> (预到站触发，停止触发，开门触发 +门侧灯提示开门侧信息)    0: IDU运行模式(预到站触发，到站触发+文字提示开门侧信息)

/*********************************************************************
*触发类型选择
*
*1:预到站触发
*2:到站触发
*3:越站触发 
*4:紧急广播
*5:关门触发
*6:开门触发
*7:离站触发
*8:到站停稳

选择的后面跟对应的数字，不选择则填0


***************************************************************/
	#define PRE_BROADCAST    		1
	#define ARRIVE_BROADCAST		0
	#define JUMP 					0
	#define EME_FUCTION				1
	#define DOOR_CLOSE  				1
	#define DOOR_OPEN   				1
	#define LEAVING					0
	#define STOP						1

/*********************************************************************/
/**********************************************
*没有门侧灯的时候,且带了紧急广播，可以文字提示开门侧
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		2
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			0
*#define DOOR_OPEN   			0
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*没有门侧灯的时候,不带紧急广播，可以文字提示开门侧
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		2
*#define JUMP					0
*#define EME_FUCTION				0
*#define DOOR_CLOSE  			0
*#define DOOR_OPEN   			0
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*有门侧灯的时候,且带了紧急广播，半自动运行
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		0
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			1
*#define DOOR_OPEN   			1
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*有门侧灯的时候,且带了紧急广播，自动运行
*#define PRE_BROADCAST    		0
*#define ARRIVE_BROADCAST		1
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			1
*#define DOOR_OPEN   			1
*#define LEAVING					0
*#define STOP					0
*******************************************/


#define CONFIG_MSG_LEN			5
#define CONFIG_AD_LEN			5
#define CONFIG_TOTAL_LEN		(2+CONFIG_MSG_LEN*2+CONFIG_AD_LEN*2+3+3)



#define GREETING "          Shenzhen BeiHai \r\n    "
#define NO_PROCESS_TIME                                1000         //多少秒


#if IDU_COMPILE
#define LOCAL_DEVICE_TYPE					IDU
#define LED_PHYSICS_WIDTH					(24)		//数码管模块横排个数

#define SHIFT_LEFT_NULL_WIDTH				32          //移动完成后，等待XX时间再重新开始移
#define VERSION "IDU LED   2013-06-07 V1.9\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						9

#define SHIFT_SPEED							0			//扫描n  屏移动1列
#define STAY_TIME							650			//更换显示内容间隔时间    //这里时间太长，当显示的图片内容小于这个时间，这个图片会显示2次
#define KEEP_TIME							0			//屏幕不滚动的时候，显示的最长时间    16*SYSTICK 约为 20ms
#endif

#if FDU_COMPILE
#define LOCAL_DEVICE_TYPE					FDU
#define LED_PHYSICS_WIDTH					(14)		//数码管模块横排个数
#define SHIFT_LEFT_NULL_WIDTH				8          //移动完成后，等待XX时间再重新开始移
#define VERSION "FDU LED   2013-06-17 V1.7\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						7

#define SHIFT_SPEED							10			//扫描n  屏移动1列
#define STAY_TIME							1200		//更换显示内容时间
#endif

#if SDU_COMPILE
#define LOCAL_DEVICE_TYPE					SDU
#define LED_PHYSICS_WIDTH					(14)		//数码管模块横排个数
#define SHIFT_LEFT_NULL_WIDTH				8			//移动完成后，等待XX时间再重新开始移
#define VERSION "SDU LED   2013-06-17 V1.7\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						7

#define SHIFT_SPEED							0			//扫描n  屏移动1列
#define STAY_TIME							1200		//更换显示内容时间
#endif



#define IMAGE_NO_PROCESS_WIDTH				 ((LED_PHYSICS_WIDTH)+2)
#define IMAGE_LED_TEST_WIDTH					((LED_PHYSICS_WIDTH)+2)





#endif

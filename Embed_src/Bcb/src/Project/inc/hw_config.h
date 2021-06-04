/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.0.0
* Date               : 04/06/2009
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DOWN            1
//#define LEFT            2
//#define RIGHT           3
#define UP              4
#define CURSOR_STEP     30




#define TRAIN485_CTRL_GPIO					GPIOA
#define TRAIN485_CTRL_BIT					GPIO_Pin_1
#define TRAIN485_OUTPUT()					GPIO_SetBits(TRAIN485_CTRL_GPIO, TRAIN485_CTRL_BIT)
#define TRAIN485_INPUT()					GPIO_ResetBits(TRAIN485_CTRL_GPIO, TRAIN485_CTRL_BIT)


// 控制接口板继电器将人工广播的音频信号直接送到列车音频总线
#define PA_DIRECT_CONN_CTRL_GPIO			GPIOB
#define PA_DIRECT_CONN_CTRL_BIT			GPIO_Pin_9
#define PA_DIRECT_CONN_ENABLE()			GPIO_SetBits(PA_DIRECT_CONN_CTRL_GPIO, PA_DIRECT_CONN_CTRL_BIT)
#define PA_DIRECT_CONN_DISABLE()			GPIO_ResetBits(PA_DIRECT_CONN_CTRL_GPIO, PA_DIRECT_CONN_CTRL_BIT)

//控制PA总线中心抽头
#define PA_LEVEL_CTRL_GPIO					GPIOB
#define PA_LEVEL_CTRL_BIT					GPIO_Pin_8
#define PA_LEVEL_PULL_DOWN()				GPIO_SetBits(PA_LEVEL_CTRL_GPIO, PA_LEVEL_CTRL_BIT)
#define PA_LEVEL_FREE()						GPIO_ResetBits(PA_LEVEL_CTRL_GPIO, PA_LEVEL_CTRL_BIT)

//控制PC总线中心抽头
#define PC_LEVEL_CTRL_GPIO					GPIOB
#define PC_LEVEL_CTRL_BIT					GPIO_Pin_0
#define PC_LEVEL_PULL_DOWN()				GPIO_SetBits(PC_LEVEL_CTRL_GPIO, PC_LEVEL_CTRL_BIT)
#define PC_LEVEL_FREE()						GPIO_ResetBits(PC_LEVEL_CTRL_GPIO, PC_LEVEL_CTRL_BIT)


//控制音量 M62429P  
#define VOLUME_CLK_CTRL_GPIO				GPIOB
#define VOLUME_CLK_CTRL_BIT				GPIO_Pin_6
#define VOLUME_CLK_LOW()					GPIO_ResetBits(VOLUME_CLK_CTRL_GPIO, VOLUME_CLK_CTRL_BIT)
#define VOLUME_CLK_HIGH()					GPIO_SetBits(VOLUME_CLK_CTRL_GPIO, VOLUME_CLK_CTRL_BIT)

//控制音量 M62429P  
#define VOLUME_DATA_CTRL_GPIO			GPIOB
#define VOLUME_DATA_CTRL_BIT				GPIO_Pin_7
#define VOLUME_DATA_LOW()					GPIO_ResetBits(VOLUME_DATA_CTRL_GPIO, VOLUME_DATA_CTRL_BIT)
#define VOLUME_DATA_HIGH()					GPIO_SetBits(VOLUME_DATA_CTRL_GPIO, VOLUME_DATA_CTRL_BIT)

// 广播控制盒占用了PA总线，需要点亮指示灯
#define LED_PC_CTRL_GPIO					GPIOB
#define LED_PC_CTRL_BIT					GPIO_Pin_5
#define LED_PC_OPEN()						GPIO_SetBits(LED_PC_CTRL_GPIO, LED_PC_CTRL_BIT)
#define LED_PC_CLOSE()						GPIO_ResetBits(LED_PC_CTRL_GPIO, LED_PC_CTRL_BIT)

#define LED_BOX_COM_GPIO					GPIOB
#define LED_BOX_COM_BIT					GPIO_Pin_1
#define LED_BOX_COM_OPEN()				GPIO_SetBits(LED_BOX_COM_GPIO, LED_BOX_COM_BIT)
#define LED_BOX_COM_CLOSE()				GPIO_ResetBits(LED_BOX_COM_GPIO, LED_BOX_COM_BIT)

#define LED_TRAIN_COM_GPIO			GPIOA
#define LED_TRAIN_COM_BIT				GPIO_Pin_11
#define LED_TRAIN_COM_OPEN()			GPIO_SetBits(LED_TRAIN_COM_GPIO, LED_TRAIN_COM_BIT)
#define LED_TRAIN_COM_CLOSE()			GPIO_ResetBits(LED_TRAIN_COM_GPIO, LED_TRAIN_COM_BIT)

#define LED_PA_GPIO						GPIOA
#define LED_PA_BIT						GPIO_Pin_12
#define LED_PA_OPEN()					GPIO_SetBits(LED_PA_GPIO, LED_PA_BIT)
#define LED_PA_CLOSE()					GPIO_ResetBits(LED_PA_GPIO, LED_PA_BIT)


//播放报警声音
#define PLAY_VOICE_CTRL_GPIO			GPIOB
#define PLAY_VOICE_CTRL_BIT			GPIO_Pin_15
#define PLAY_VOICE_ENABLE()				GPIO_ResetBits(PLAY_VOICE_CTRL_GPIO, PLAY_VOICE_CTRL_BIT)
#define PLAY_VOICE_DISABLE()			GPIO_SetBits(PLAY_VOICE_CTRL_GPIO, PLAY_VOICE_CTRL_BIT)

//麦克风出来的音频送到PC OR  PA  继电器选择  JQ_OUT_C5
#define MIC_AUDIO_CTRL_GPIO			GPIOB
#define MIC_AUDIO_CTRL_BIT				GPIO_Pin_14
#define MIC_AUDIO_TO_PA()				GPIO_ResetBits(MIC_AUDIO_CTRL_GPIO, MIC_AUDIO_CTRL_BIT)
#define MIC_AUDIO_TO_PC()				GPIO_SetBits(MIC_AUDIO_CTRL_GPIO, MIC_AUDIO_CTRL_BIT)

//麦克风音频送到PA  OR  PC  音频总线的通断继电器  JQ_OUT_C4
#define MIC_CONN_GPIO					GPIOB
#define MIC_CONN_BIT					GPIO_Pin_13
#define MIC_CONN_ENABLE()				GPIO_ResetBits(MIC_CONN_GPIO, MIC_CONN_BIT)
#define MIC_CONN_DISABLE()				GPIO_SetBits(MIC_CONN_GPIO, MIC_CONN_BIT)

//监听扬声器是否接通 JQ_OUT_C3
#define MONITOR_CONN_GPIO				GPIOB
#define MONITOR_CONN_BIT					GPIO_Pin_12
#define MONITOR_CONN_ENABLE()				GPIO_ResetBits(MONITOR_CONN_GPIO, MONITOR_CONN_BIT)
#define MONITOR_CONN_DISABLE()			GPIO_SetBits(MONITOR_CONN_GPIO, MONITOR_CONN_BIT)


//监听总线音频(包括PA OR PC) 或者 报警音  JQ_OUT_C2
#define MONI_SELECT_CTRL_GPIO			GPIOB
#define MONI_SELECT_CTRL_BIT			GPIO_Pin_11
#define MONI_SELECT_PAPC()				GPIO_ResetBits(MONI_SELECT_CTRL_GPIO, MONI_SELECT_CTRL_BIT)
#define MONI_SELECT_RING()				GPIO_SetBits(MONI_SELECT_CTRL_GPIO, MONI_SELECT_CTRL_BIT)


//监听PA OR PC              JQ_OUT_C1
#define MONI_SELECT_PAORPC_CTRL_GPIO			GPIOB
#define MONI_SELECT_PAORPC_CTRL_BIT			GPIO_Pin_10
#define MONI_SELECT_PA()				GPIO_ResetBits(MONI_SELECT_PAORPC_CTRL_GPIO, MONI_SELECT_PAORPC_CTRL_BIT)
#define MONI_SELECT_PC()				GPIO_SetBits(MONI_SELECT_PAORPC_CTRL_GPIO, MONI_SELECT_PAORPC_CTRL_BIT)



// 检测PA总线中心抽头电平
#define PA_CHK_GPIO					GPIOA
#define PA_CHK_BIT						GPIO_Pin_8
#define PA_CHK()							GPIO_ReadInputDataBit(PA_CHK_GPIO, PA_CHK_BIT)

// 检测PC总线中心抽头电平
#define PC_CHK_GPIO					GPIOA
#define PC_CHK_BIT						GPIO_Pin_7
#define PC_CHK()							GPIO_ReadInputDataBit(PC_CHK_GPIO, PC_CHK_BIT)



#define ADDR0_GPIO						GPIOA
#define ADDR0_BIT						GPIO_Pin_4
#define ADDR0_READ()					GPIO_ReadInputDataBit(ADDR0_GPIO, ADDR0_BIT)

#define ADDR1_GPIO						GPIOA
#define ADDR1_BIT						GPIO_Pin_6
#define ADDR1_READ()					GPIO_ReadInputDataBit(ADDR1_GPIO, ADDR1_BIT)

#define KEY_SIGNAL_GPIO				GPIOA
#define KEY_SIGNAL_BIT					GPIO_Pin_5
#define KEY_SIGNAL_READ()				GPIO_ReadInputDataBit(KEY_SIGNAL_GPIO, KEY_SIGNAL_BIT)

//
#define PLAY_STAT_GPIO					GPIOA
#define PLAY_STAT_BIT					GPIO_Pin_0
#define PLAY_STAT_READ()				GPIO_ReadInputDataBit(PLAY_STAT_GPIO, PLAY_STAT_BIT)




/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Joystick_Send(uint8_t Keys);
uint8_t JoyState(void);
void Get_SerialNum(void);

extern void SetLocalPaLineStat(u8 stat);
extern void SetLocalPcLineStat(u8 stat);
extern u8 GetLocalPaLineStat(void);
extern u8 GetLocalPcLineStat(void);


#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

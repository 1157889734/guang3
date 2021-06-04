#ifndef LedDrv_H_
#define LedDrv_H_

#include "TypeDef.h"

#include "at45db161.h"
#include "include.h"


#define MAX_LINE_NUM                    3

#define LINE1_STATION_NUM               18
#define LINE2_STATION_NUM               21
#define LINE3_STATION_NUM               21



#define LEVELE_LOW                  0
#define LEVELE_HIGH                 1



#define LED_DRV_LATCH(n)            (n) ? (GPIO_SetBits(GPIOF, GPIO_Pin_11)) : (GPIO_ResetBits(GPIOF, GPIO_Pin_11))
#define LED_DRV_OE(n)               (n) ? (GPIO_SetBits(GPIOG, GPIO_Pin_4)) : (GPIO_ResetBits(GPIOG, GPIO_Pin_4))
#define LED_DRV_R(n)                (n) ? (GPIO_SetBits(GPIOG, GPIO_Pin_3)) : (GPIO_ResetBits(GPIOG, GPIO_Pin_3))
#define LED_DRV_G(n)                (n) ? (GPIO_SetBits(GPIOG, GPIO_Pin_2)) : (GPIO_ResetBits(GPIOG, GPIO_Pin_2))
#define LED_DRV_CLK(n)              (n) ? (GPIO_SetBits(GPIOA, GPIO_Pin_5)) : (GPIO_ResetBits(GPIOA, GPIO_Pin_5))
#define LED_DRV_DATA(n)             (n) ? (GPIO_SetBits(GPIOB, GPIO_Pin_5)) : (GPIO_ResetBits(GPIOB, GPIO_Pin_5))




extern tDispStat gDispStat;
extern tSysConfig gSysConfg;
extern tTrigerValue gTrigerValue;


extern u8 gOldTerminal;

extern void SpiDmaInit(void);
extern void LoadRowData(void);
extern void LedSetRow(u8 index);
extern void DispTypeInit(tDispType type);
extern void LedSetColour(tLedColor color);
extern tDoorStat GetDoorStat( void );
extern void do_get_station_config_arrive(void);
extern void do_get_station_config_pre(void);
extern void ProcessDataOperat(u8 *buf);
extern void DispFun_DS(void);

extern void PorcessDataNormalSystem(u8 *buf);

extern void DispFun_NDS(void);  //“¡¿ PTU

#endif

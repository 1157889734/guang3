/**
  ******************************************************************************
  * @file    stm32f2xx_rtc.h
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file contains all the functions prototypes for the RTC firmware 
  *          library.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_RTC_H
#define __STM32F2xx_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup RTC
  * @{
  */ 

/** @defgroup RTC_Exported_Types
  * @{
  */ 

/** 
  * @brief  RTC Init structures definition  
  */ 
typedef struct
{
  uint32_t RTC_HourFormat;   /*!< Specifies the RTC Hour Format.
                             This parameter can be a value of @ref RTC_Hour_Formats */
  
  uint32_t RTC_AsynchPrediv; /*!< Specifies the RTC Asynchronous Predivider value.
                             This parameter must be set to a value lower than 0x7F */
  
  uint32_t RTC_SynchPrediv;  /*!< Specifies the RTC Synchronous Predivider value.
                             This parameter must be set to a value lower than 0x1FFF */ 
}RTC_InitTypeDef;

/** 
  * @brief  RTC Time structure definition  
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  
  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef; 

/** 
  * @brief  RTC Date structure definition  
  */
typedef struct
{
  uint32_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
  
  uint32_t RTC_Month;   /*!< Specifies the RTC Date Month.
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
  
  uint8_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

/** 
  * @brief  RTC Alarm structure definition  
  */
typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;     /*!< Specifies the RTC Alarm Time members. */

  uint32_t RTC_AlarmMask;            /*!< Specifies the RTC Alarm Masks.
                                     This parameter can be a value of @ref RTC_AlarmMask_Definitions */

  uint32_t RTC_AlarmDateWeekDaySel;  /*!< Specifies the RTC Alarm is on Date or WeekDay.
                                     This parameter can be a value of @ref RTC_AlarmDateWeekDay_Definitions */
  
  uint8_t RTC_AlarmDateWeekDay;      /*!< Specifies the RTC Alarm Date/WeekDay.
                                     This parameter must be set to a value in the 1-31 range 
                                     if the Alarm Date is selected.
                                     This parameter can be a value of @ref RTC_WeekDay_Definitions 
                                     if the Alarm WeekDay is selected. */
}RTC_AlarmTypeDef;

/**
  * @}
  */ 

/** @defgroup RTC_Exported_Constants
  * @{
  */ 

 
/** @defgroup RTC_Hour_Formats 
  * @{
  */ 
#define RTC_HourFormat_24              ((uint32_t)0x00000000)
#define RTC_HourFormat_12              ((uint32_t)0x00000040)
#define IS_RTC_HOUR_FORMAT(FORMAT)     (((FORMAT) == RTC_HourFormat_12) || \
                                        ((FORMAT) == RTC_HourFormat_24))
/**
  * @}
  */ 

/** @defgroup RTC_Asynchronous_Predivider 
  * @{
  */ 
#define IS_RTC_ASYNCH_PREDIV(PREDIV)   ((PREDIV) <= 0x7F)
 
/**
  * @}
  */ 


/** @defgroup RTC_Synchronous_Predivider 
  * @{
  */ 
#define IS_RTC_SYNCH_PREDIV(PREDIV)    ((PREDIV) <= 0x1FFF)

/**
  * @}
  */ 

/** @defgroup RTC_Time_Definitions 
  * @{
  */ 
#define IS_RTC_HOUR12(HOUR)            (((HOUR) > 0) && ((HOUR) <= 12))
#define IS_RTC_HOUR24(HOUR)            ((HOUR) <= 23)
#define IS_RTC_MINUTES(MINUTES)        ((MINUTES) <= 59)
#define IS_RTC_SECONDS(SECONDS)        ((SECONDS) <= 59)

/**
  * @}
  */ 

/** @defgroup RTC_AM_PM_Definitions 
  * @{
  */ 
#define RTC_H12_AM                     ((uint8_t)0x00)
#define RTC_H12_PM                     ((uint8_t)0x40)
#define IS_RTC_H12(PM) (((PM) == RTC_H12_AM) || ((PM) == RTC_H12_PM))

/**
  * @}
  */ 

/** @defgroup RTC_Year_Date_Definitions 
  * @{
  */ 
#define IS_RTC_YEAR(YEAR)              ((YEAR) <= 99)

/**
  * @}
  */ 

/** @defgroup RTC_Month_Date_Definitions 
  * @{
  */ 
#define RTC_Month_January              ((uint32_t)0x00000001)
#define RTC_Month_February             ((uint32_t)0x00000002)
#define RTC_Month_March                ((uint32_t)0x00000003)
#define RTC_Month_April                ((uint32_t)0x00000004)
#define RTC_Month_May                  ((uint32_t)0x00000005)
#define RTC_Month_June                 ((uint32_t)0x00000006)
#define RTC_Month_July                 ((uint32_t)0x00000007)
#define RTC_Month_August               ((uint32_t)0x00000008)
#define RTC_Month_September            ((uint32_t)0x00000009)
#define RTC_Month_October              ((uint32_t)0x00000010)
#define RTC_Month_November             ((uint32_t)0x00000011)
#define RTC_Month_December             ((uint32_t)0x00000012)
#define IS_RTC_MONTH(MONTH) (((MONTH) >= 1) && ((MONTH) <= 12))
#define IS_RTC_DATE(DATE) (((DATE) >= 1) && ((DATE) <= 31))

/**
  * @}
  */ 

/** @defgroup RTC_WeekDay_Definitions 
  * @{
  */ 
  
#define	RTC_Weekday_Monday             ((uint32_t)0x00000001)
#define	RTC_Weekday_Tuesday            ((uint32_t)0x00000002)
#define	RTC_Weekday_Wednesday          ((uint32_t)0x00000003)
#define	RTC_Weekday_Thursday           ((uint32_t)0x00000004)
#define	RTC_Weekday_Friday             ((uint32_t)0x00000005)
#define	RTC_Weekday_Saturday           ((uint32_t)0x00000006)
#define	RTC_Weekday_Sunday             ((uint32_t)0x00000007)
#define IS_RTC_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_Weekday_Monday) || \
                                 ((WEEKDAY) == RTC_Weekday_Tuesday) || \
                                 ((WEEKDAY) == RTC_Weekday_Wednesday) || \
                                 ((WEEKDAY) == RTC_Weekday_Thursday) || \
                                 ((WEEKDAY) == RTC_Weekday_Friday) || \
                                 ((WEEKDAY) == RTC_Weekday_Saturday) || \
                                 ((WEEKDAY) == RTC_Weekday_Sunday))
/**
  * @}
  */ 


/** @defgroup RTC_Alarm_Definitions 
  * @{
  */ 
#define IS_RTC_ALARM_DATE_WEEKDAY_DATE(DATE) (((DATE) > 0) && ((DATE) <= 31))
#define IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_Weekday_Monday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Tuesday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Wednesday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Thursday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Friday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Saturday) || \
                                                    ((WEEKDAY) == RTC_Weekday_Sunday))

/**
  * @}
  */ 


/** @defgroup RTC_AlarmDateWeekDay_Definitions 
  * @{
  */ 
#define RTC_AlarmDateWeekDaySel_Date      ((uint32_t)0x00000000)  
#define RTC_AlarmDateWeekDaySel_WeekDay   ((uint32_t)0x40000000)  

#define IS_RTC_ALARM_DATE_WEEKDAY_SEL(SEL) (((SEL) == RTC_AlarmDateWeekDaySel_Date) || \
                                            ((SEL) == RTC_AlarmDateWeekDaySel_WeekDay))

/**
  * @}
  */ 


/** @defgroup RTC_AlarmMask_Definitions 
  * @{
  */ 
#define RTC_AlarmMask_None                ((uint32_t)0x00000000)
#define RTC_AlarmMask_DateWeekDay         ((uint32_t)0x80000000)  
#define RTC_AlarmMask_Hours               ((uint32_t)0x00800000)
#define RTC_AlarmMask_Minutes             ((uint32_t)0x00008000)
#define RTC_AlarmMask_Seconds             ((uint32_t)0x00000080)
#define RTC_AlarmMask_All                 ((uint32_t)0x80808080)
#define IS_ALARM_MASK(MASK)  (((MASK) & 0x7F7F7F7F) == (uint32_t)RESET)

/**
  * @}
  */ 

/** @defgroup RTC_Alarms_Definitions 
  * @{
  */ 
#define RTC_Alarm_A                       ((uint32_t)0x00000100)
#define RTC_Alarm_B                       ((uint32_t)0x00000200)
#define IS_RTC_ALARM(ALARM) (((ALARM) == RTC_Alarm_A) || ((ALARM) == RTC_Alarm_B))
#define IS_RTC_CMD_ALARM(ALARM) (((ALARM) & (RTC_Alarm_A | RTC_Alarm_B)) != (uint32_t)RESET)

/**
  * @}
  */ 

/** @defgroup RTC_Wakeup_Timer_Definitions 
  * @{
  */ 
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
#define IS_RTC_WAKEUP_CLOCK(CLOCK) (((CLOCK) == RTC_WakeUpClock_RTCCLK_Div16) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div8) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div4) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div2) || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_16bits) || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_17bits))
#define IS_RTC_WAKEUP_COUNTER(COUNTER)  ((COUNTER) <= 0xFFFF)
/**
  * @}
  */ 

/** @defgroup RTC_Time_Stamp_Edges_definitions 
  * @{
  */ 
#define RTC_TimeStampEdge_Rising          ((uint32_t)0x00000000)
#define RTC_TimeStampEdge_Falling         ((uint32_t)0x00000008)
#define IS_RTC_TIMESTAMP_EDGE(EDGE) (((EDGE) == RTC_TimeStampEdge_Rising) || \
                                     ((EDGE) == RTC_TimeStampEdge_Falling))
/**
  * @}
  */ 

/** @defgroup RTC_Output_selection_Definitons 
  * @{
  */ 
#define RTC_Output_Disable             ((uint32_t)0x00000000)
#define RTC_Output_AlarmA              ((uint32_t)0x00200000)
#define RTC_Output_AlarmB              ((uint32_t)0x00400000)
#define RTC_Output_WakeUp              ((uint32_t)0x00600000)
 
#define IS_RTC_OUTPUT(OUTPUT) (((OUTPUT) == RTC_Output_Disable) || \
                               ((OUTPUT) == RTC_Output_AlarmA) || \
                               ((OUTPUT) == RTC_Output_AlarmB) || \
                               ((OUTPUT) == RTC_Output_WakeUp))

/**
  * @}
  */ 

/** @defgroup RTC_Output_Polarity_Definitons 
  * @{
  */ 
#define RTC_OutputPolarity_High           ((uint32_t)0x00000000)
#define RTC_OutputPolarity_Low            ((uint32_t)0x00100000)
#define IS_RTC_OUTPUT_POL(POL) (((POL) == RTC_OutputPolarity_High) || \
                                ((POL) == RTC_OutputPolarity_Low))
/**
  * @}
  */ 


/** @defgroup RTC_Digital_Calibration_Definitions 
  * @{
  */ 
#define RTC_CalibSign_Positive            ((uint32_t)0x00000000) 
#define RTC_CalibSign_Negative            ((uint32_t)0x00000080)
#define IS_RTC_CALIB_SIGN(SIGN) (((SIGN) == RTC_CalibSign_Positive) || \
                                 ((SIGN) == RTC_CalibSign_Negative))
#define IS_RTC_CALIB_VALUE(VALUE) ((VALUE) < 0x20)

/**
  * @}
  */ 


/** @defgroup RTC_DayLightSaving_Definitions 
  * @{
  */ 
#define RTC_DayLightSaving_SUB1H   ((uint32_t)0x00020000)
#define RTC_DayLightSaving_ADD1H   ((uint32_t)0x00010000)
#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) == RTC_DayLightSaving_SUB1H) || \
                                      ((SAVE) == RTC_DayLightSaving_ADD1H))

#define RTC_StoreOperation_Reset        ((uint32_t)0x00000000)
#define RTC_StoreOperation_Set          ((uint32_t)0x00040000)
#define IS_RTC_STORE_OPERATION(OPERATION) (((OPERATION) == RTC_StoreOperation_Reset) || \
                                           ((OPERATION) == RTC_StoreOperation_Set))
/**
  * @}
  */ 

/** @defgroup RTC_Tamper_Edges_Definitions 
  * @{
  */ 
#define RTC_TamperEdge_Rising             ((uint32_t)0x00000000)
#define RTC_TamperEdge_Falling            ((uint32_t)0x00000002)
#define IS_RTC_TAMPER_EDGE(EDGE) (((EDGE) == RTC_TamperEdge_Rising) || \
                                  ((EDGE) == RTC_TamperEdge_Falling))
                              
/**
  * @}
  */ 

/** @defgroup RTC_Tamper_Pin_Selection 
  * @{
  */ 
#define RTC_TamperPin_PC13                 ((uint32_t)0x00000000)
#define RTC_TamperPin_PI8                  ((uint32_t)0x00010000)
#define IS_RTC_TAMPER_PIN(PIN) (((PIN) == RTC_TamperPin_PC13) || \
                                ((PIN) == RTC_TamperPin_PI8))
/**
  * @}
  */ 


/** @defgroup RTC_TimeStamp_Pin_Selection 
  * @{
  */ 
#define RTC_TimeStampPin_PC13              ((uint32_t)0x00000000)
#define RTC_TimeStampPin_PI8               ((uint32_t)0x00020000)
#define IS_RTC_TIMESTAMP_PIN(PIN) (((PIN) == RTC_TimeStampPin_PC13) || \
                                   ((PIN) == RTC_TimeStampPin_PI8))                                                                
/**
  * @}
  */ 


/** @defgroup RTC_Output_Type_ALARM_OUT 
  * @{
  */ 
#define RTC_OutputType_OpenDrain           ((uint32_t)0x00000000)
#define RTC_OutputType_PushPull            ((uint32_t)0x00040000)
#define IS_RTC_OUTPUT_TYPE(TYPE) (((TYPE) == RTC_OutputType_OpenDrain) || \
                                  ((TYPE) == RTC_OutputType_PushPull))   

/**
  * @}
  */ 

/** @defgroup RTC_Backup_Registers_Definitions 
  * @{
  */                                                                     

#define RTC_BkpRegister_0                 ((uint32_t)0x00000000)
#define RTC_BkpRegister_1                 ((uint32_t)0x00000001)
#define RTC_BkpRegister_2                 ((uint32_t)0x00000002)
#define RTC_BkpRegister_3                 ((uint32_t)0x00000003)
#define RTC_BkpRegister_4                 ((uint32_t)0x00000004)
#define RTC_BkpRegister_5                 ((uint32_t)0x00000005)
#define RTC_BkpRegister_6                 ((uint32_t)0x00000006)
#define RTC_BkpRegister_7                 ((uint32_t)0x00000007)
#define RTC_BkpRegister_8                 ((uint32_t)0x00000008)
#define RTC_BkpRegister_9                 ((uint32_t)0x00000009)
#define RTC_BkpRegister_10                ((uint32_t)0x0000000A)
#define RTC_BkpRegister_11                ((uint32_t)0x0000000B)
#define RTC_BkpRegister_12                ((uint32_t)0x0000000C)
#define RTC_BkpRegister_13                ((uint32_t)0x0000000D)
#define RTC_BkpRegister_14                ((uint32_t)0x0000000E)
#define RTC_BkpRegister_15                ((uint32_t)0x0000000F)
#define RTC_BkpRegister_16                ((uint32_t)0x00000010)
#define RTC_BkpRegister_17                ((uint32_t)0x00000011)
#define RTC_BkpRegister_18                ((uint32_t)0x00000012)
#define RTC_BkpRegister_19                ((uint32_t)0x00000013)
#define IS_RTC_BACKUP_REGISTER(REGISTER) (((REGISTER) == RTC_BkpRegister_0) || \
                                          ((REGISTER) == RTC_BkpRegister_1) || \
                                          ((REGISTER) == RTC_BkpRegister_2) || \
                                          ((REGISTER) == RTC_BkpRegister_3) || \
                                          ((REGISTER) == RTC_BkpRegister_4) || \
                                          ((REGISTER) == RTC_BkpRegister_5) || \
                                          ((REGISTER) == RTC_BkpRegister_6) || \
                                          ((REGISTER) == RTC_BkpRegister_7) || \
                                          ((REGISTER) == RTC_BkpRegister_8) || \
                                          ((REGISTER) == RTC_BkpRegister_9) || \
                                          ((REGISTER) == RTC_BkpRegister_10) || \
                                          ((REGISTER) == RTC_BkpRegister_11) || \
                                          ((REGISTER) == RTC_BkpRegister_12) || \
                                          ((REGISTER) == RTC_BkpRegister_13) || \
                                          ((REGISTER) == RTC_BkpRegister_14) || \
                                          ((REGISTER) == RTC_BkpRegister_15) || \
                                          ((REGISTER) == RTC_BkpRegister_16) || \
                                          ((REGISTER) == RTC_BkpRegister_17) || \
                                          ((REGISTER) == RTC_BkpRegister_18) || \
                                          ((REGISTER) == RTC_BkpRegister_19))
/**
  * @}
  */ 

/** @defgroup RTC_Input_parameter_format_definitions 
  * @{
  */ 
#define RTC_Format_BIN                    ((uint32_t)0x000000000)
#define RTC_Format_BCD                    ((uint32_t)0x000000001)
#define IS_RTC_FORMAT(FORMAT) (((FORMAT) == RTC_Format_BIN) || ((FORMAT) == RTC_Format_BCD))

/**
  * @}
  */ 

/** @defgroup RTC_Flags_Definitions 
  * @{
  */ 
#define RTC_FLAG_TAMPF                    ((uint32_t)0x00002000)
#define RTC_FLAG_TSOVF                    ((uint32_t)0x00001000)
#define RTC_FLAG_TSF                      ((uint32_t)0x00000800)
#define RTC_FLAG_WUTF                     ((uint32_t)0x00000400)
#define RTC_FLAG_ALRBF                    ((uint32_t)0x00000200)
#define RTC_FLAG_ALRAF                    ((uint32_t)0x00000100)
#define RTC_FLAG_INITF                    ((uint32_t)0x00000040)
#define RTC_FLAG_RSF                      ((uint32_t)0x00000020)
#define RTC_FLAG_INITS                    ((uint32_t)0x00000010)
#define RTC_FLAG_WUTWF                    ((uint32_t)0x00000004)
#define RTC_FLAG_ALRBWF                   ((uint32_t)0x00000002)
#define RTC_FLAG_ALRAWF                   ((uint32_t)0x00000001)
#define IS_RTC_GET_FLAG(FLAG) (((FLAG) == RTC_FLAG_TSOVF) || ((FLAG) == RTC_FLAG_TSF) || \
                               ((FLAG) == RTC_FLAG_WUTF) || ((FLAG) == RTC_FLAG_ALRBF) || \
                               ((FLAG) == RTC_FLAG_ALRAF) || ((FLAG) == RTC_FLAG_INITF) || \
                               ((FLAG) == RTC_FLAG_RSF) || ((FLAG) == RTC_FLAG_WUTWF) || \
                               ((FLAG) == RTC_FLAG_ALRBWF) || ((FLAG) == RTC_FLAG_ALRAWF) || \
                               ((FLAG) == RTC_FLAG_TAMPF))
#define IS_RTC_CLEAR_FLAG(FLAG) (((FLAG) != (uint32_t)RESET) && (((FLAG) & 0xFFFFC0DF) == (uint32_t)RESET))

/**
  * @}
  */ 

/** @defgroup RTC_Interrupts_Definitions 
  * @{
  */ 
#define RTC_IT_TS                         ((uint32_t)0x00008000)
#define RTC_IT_WUT                        ((uint32_t)0x00004000)
#define RTC_IT_ALRB                       ((uint32_t)0x00002000)
#define RTC_IT_ALRA                       ((uint32_t)0x00001000)
#define RTC_IT_TAMP                       ((uint32_t)0x00000004)
#define IS_RTC_CONFIG_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFF0FFB) == (uint32_t)RESET))
#define IS_RTC_GET_IT(IT) (((IT) == RTC_IT_TS) || ((IT) == RTC_IT_WUT) || \
                           ((IT) == RTC_IT_ALRB) || ((IT) == RTC_IT_ALRA) || \
                           ((IT) == RTC_IT_TAMP))
#define IS_RTC_CLEAR_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFF0FFB) == (uint32_t)RESET))

/**
  * @}
  */ 

/**
  * @}
  */ 


/** @defgroup RTC_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup RTC_Exported_Functions
  * @{
  */ 

/** 
  * @brief  RTC Initialization functions  
  */ 
ErrorStatus RTC_DeInit(void);
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState);
ErrorStatus RTC_RefClockCmd(FunctionalState NewState);
ErrorStatus RTC_EnterInitMode(void);
void RTC_ExitInitMode(void);
void RTC_WriteProtectionCmd(FunctionalState NewState);
ErrorStatus RTC_WaitForSynchro(void);

/** 
  * @brief  RTC Time and Date Functions  
  */ 
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

/** 
  * @brief  RTC Alarms Functions  
  */ 
void RTC_SetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);
ErrorStatus RTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState);

/** 
  * @brief  RTC WakeUp Timer Functions  
  */ 
void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);
void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);
uint32_t RTC_GetWakeUpCounter(void);
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);

/** 
  * @brief  RTC Daylight Saving Function  
  */ 
void RTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation);
uint32_t RTC_GetStoreOperation(void);

/** 
  * @brief  RTC Output pin Configuration Function  
  */ 
void RTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity);

/** 
  * @brief  RTC Digital Calibration Functions  
  */ 
ErrorStatus RTC_DigitalCalibConfig(uint32_t RTC_CalibSign, uint32_t Value);
ErrorStatus RTC_DigitalCalibCmd(FunctionalState NewState);
void RTC_CalibOutputCmd(FunctionalState NewState);

/** 
  * @brief  RTC TimeStamp Functions  
  */ 
void RTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState);
void RTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct, 
                                      RTC_DateTypeDef* RTC_StampDateStruct);
                                  

/** 
  * @brief  RTC Tamper Functions  
  */ 
void RTC_TamperConfig(uint32_t RTC_TamperEdge);
void RTC_TamperCmd(FunctionalState NewState);

/** 
  * @brief  RTC Backup Registers Functions  
  */ 
void RTC_WriteBackupRegister(uint32_t RTC_BkpRegister, uint32_t Value);
uint32_t RTC_ReadBackupRegister(uint32_t RTC_BkpRegister);

/** 
  * @brief  RTC Tamper, TimeStamp Pins Selection Functions  
  */ 
void RTC_OutputTypeConfig(uint32_t RTC_OutputType);
void RTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin);
void RTC_TamperPinSelection(uint32_t RTC_TamperPin);


/** 
  * @brief  RTC Flags ans IT functions  
  */ 
FlagStatus RTC_GetFlagStatus(uint32_t RTC_FLAG);
void RTC_ClearFlag(uint32_t RTC_FLAG);
ITStatus RTC_GetITStatus(uint32_t RTC_IT);
void RTC_ClearITPendingBit(uint32_t RTC_IT);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F2xx_RTC_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

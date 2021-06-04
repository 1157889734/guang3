
/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-May-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f2xx_iwdg.h"
#include "main.h"

#include "uart.h"

#include "stm32_eval_sdio_sd.h"
#include "sd_data.h"
#include "flash_if.h"
#include "ff.h"
#include "mem.h"
#include "memp.h"

#include "common.h"
#include "tcms.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "stm32_eval.h"
#include "usb.h"

#include "rtc_1337.h"
#include "cmd.h"


/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
__IO uint32_t AsynchPrediv, SynchPrediv = 0 ;

USB_OTG_CORE_HANDLE  USB_OTG_dev ;

uint8 ATC_addr = 0;

uint8 ATC_id = 0;



void NVIC_Configuration()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //USART1�ж�����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#if CHOISE_COM
    //USART3�ж�����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#else
    //USART2�ж�����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    //SD
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_Configuration(void)
{
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
    /* LSI used as RTC source clock*/
    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);
    
    /* Wait till LSI is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }
    
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
}
/***********************************************************************************************************************
*Function Nane    :  device_addr_init
*Description      :  �豸��ַ��ʼ��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void  device_addr_init(void)
{
    static GPIO_InitTypeDef  my_GPIO_InitStructure;//for device address// Added by Lison on 2013-2-26 9:10:30
    //init map_address connect pin���豸��ַ,PD12 & PD13 // Added by Lison on 2013-2-26 9:13:41
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    // config the device address pins for input pins
    my_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    my_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    my_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    my_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    my_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &my_GPIO_InitStructure);
}
/***********************************************************************************************************************
*Function Nane    :  power_up_init
*Description      :  �ϵ��ʼ��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void  power_up_init(void)
{
    //led ��ʼ��
    STM_EVAL_LEDInit(LED1);
    //�豸��ַ��ʼ��
    device_addr_init();
    //RTCʱ��оƬ��ʼ��
    rtc_1337_init();
    //���������ʼ��
    save_init();
    // RTC_Configuration();
    //broadcast_init();
    //device_addr_init();
    //get_device_addr();
    //485��ʼ��
    gpio_485_en_init();
    //USB��ʼ��
#if 1
    USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
              USB_OTG_HS_CORE_ID,
#else
              USB_OTG_FS_CORE_ID,
#endif
              &USR_desc,
              &USBD_HID_cb,
              &USR_cb);
#endif
    //���Ź���ʼ��
#if IWDG_FLAG
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_128);
    IWDG_SetReload(0xaaa);
    IWDG_Enable();
    IWDG_ReloadCounter();
#endif
}
/***********************************************************************************************************************
*Function Nane    :  system_init
*Description      :  ϵͳ��ʼ��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void system_init(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    /*���ڳ�ʼ��*/
    uart_init(COM1);      /* UART1  ��PISͨ��*/
    uart_init(COM3);      /* UART3  ��TMSͨ��*/
    uart_config(UART3, 19200, USART_Parity_No);
#if CHOISE_COM
    uart_init(COM3);      /* UART3  ��TMSͨ��*/
    uart_config(UART3, 19200, USART_Parity_No);
#else
    uart_init(COM2);      /* UART2  �����豸�л�ʱͨ��485����һ�˵�PISͨ��*/
    uart_config(UART2, 19200, USART_Parity_No);
#endif
    uart_config(UART1, 19200, USART_Parity_No);
    NVIC_Configuration();
    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    /* SystTick configuration: an interrupt every 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
    /////////////////////////////////////////////
    /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
    mem_init();
    /* Initializes the memory pools defined by MEMP_NUM_x.*/
    memp_init();
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured to
         120 MHz, this is done through SystemInit() function which is called from
         startup file (startup_stm32f2xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f2xx.c file
       */
    static uint32 run_timer = 0;
    static uint32 rtc_timer = 0;
    static uint8  num = 0;
    //NVIC_InitTypeDef NVIC_InitStructure;
    //MCUϵͳ��ʼ��
    system_init();
    //�豸�ϵ��ʼ����
    power_up_init();
    //���µ�ǰʱ��
    update_cur_time();
    //SD����ʼ��
    sd_init();
    //��¼����ʱ��
    save_start_time();
    tcms_init();
    //��ȡ�豸��ַ
    ATC_addr = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13);
    ATC_addr = (ATC_addr << 1) | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);
    ATC_addr = ATC_addr & 0x03;
    ATC_id   = (0xA0 | ATC_addr);
#if 1
    USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
              USB_OTG_HS_CORE_ID,
#else
              USB_OTG_FS_CORE_ID,
#endif
              &USR_desc,
              &USBD_HID_cb,
              &USR_cb);
#endif
              
    //update_dir();
    //check_sd_state();
    //check_sd_space();
    while (1)
    {
#if 1
    
        if (check_timer(&rtc_timer, 1000))
        {
            update_cur_time();
            //ÿһ���Ӽ��ʱ�䣬�Ƿ��0��
            //��0�㣬��ı䵱ǰ�洢·
            check_sd_state();
            
            if (60 < num) //60
            {
                //update_dir();
                update_rec_data_dir();
                //check_sd_state();
                num = 0;
            }
            
            num++;
        }
        
#endif
        
        if (check_timer(&run_timer, 400)) //���е�
        {
            STM_EVAL_LEDToggle(LED1);
        }
        
        SD_LED();
        ptu_proc();         //usb ���� ��PTUͨ��
        //ι��
#if IWDG_FLAG
        IWDG_ReloadCounter();
#endif
        tms_proc();         //uart3, �����г���������TMS �ź�
        pis_proc();          //uart1, û�з��������л�ʱ��ͨ��RS232 �ͱ���PISC ͨ��
        //atc_proc_tms();
        //ι��
#if IWDG_FLAG
        IWDG_ReloadCounter();
#endif
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    /* Capture the current local time */
    timingdelay = LocalTime + nCount;
    
    /* wait until the desired delay finish */
    while (timingdelay > LocalTime)
    {
#if IWDG_FLAG
        //ι��
        IWDG_ReloadCounter();
#endif
    }
}


/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
    LocalTime += SYSTEMTICK_PERIOD_MS;
}

//***********************************************************************************************************************
//��������:���Ӧ�ò��Ƿ�ʱ�䵽��
//����˵��:timer---Ӧ�ò㶨��ļ�����   interval---���м��
//ע������:��ȷ��10ms
//����˵��:��
//***********************************************************************************************************************
uint8_t check_timer(uint32_t *timer, uint32_t interval)
{
    if (interval)
    {
        //�ж����
        if (LocalTime < (*timer))
        {
            *timer = 0;
        }
        
        //�Ƚ��Ƿ�ʱ�䵽
        if (LocalTime - (*timer) >= interval)
        {
            *timer = LocalTime;
            return 1;
        }
    }
    
    return 0;
}

uint32_t get_time(void)
{
    return LocalTime;
}

uint8 get_device_addr(void)
{
    return ATC_addr;
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


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
#include "stm32f2x7_eth.h"
#include "stm32f2xx_iwdg.h"
#include "netconf.h"
#include "main.h"
//#include "tftpserver.h"
#include "httpserver.h"
#include "telnet.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "tba_3093.h"
#include "stm32_eval_sdio_sd.h"
#include "sd_data.h"
#include "flash_if.h"
//#include "udp_server.h"
#include "ff.h"

#include "common.h"
//#include "menu.h"
#include  "usbd_hid_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include  "stm32_eval.h"
#include  "usb.h"
#include "include.h"
#include "TypeDef.h"
#include "LedDrv.h"
#include "at45db161.h"

/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1
extern u32 gScrSavTime;
extern u32 gErrorTime;
extern u8   gDownFirmware ;
extern tDoorStat gDoorStat;
u8 FIRST = 0;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
__IO uint32_t AsynchPrediv, SynchPrediv = 0 ;

USB_OTG_CORE_HANDLE  USB_OTG_dev ;



#define PHY_RESET(n)        (n) ? (GPIO_SetBits(GPIOF, GPIO_Pin_0)) : (GPIO_ResetBits(GPIOF, GPIO_Pin_0))


void NVIC_Configuration()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //USART1????????
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //USART2????????
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#if 0  //for test
    //USART3????????
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    /*
      //SD
      NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
    */
    /* Enable DMA2 channel3 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
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
    /* Enable GPIOs clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                           RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI |
                           RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |
                           RCC_AHB1Periph_GPIOF, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Enable DMA clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_DeInit(DMA2_Stream3);
    /* Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}




void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 485 ENABLE */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* ADDR 0 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    /* ADDR 1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    /* ADDR 2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    /* ADDR 3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    //SEG LED
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // spi flash
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* LED DRIVER */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_11;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
#if 0
    /* Configure SPI1 pins: SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure SPI1 pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
#else
    /* Configure SPI1 pins: SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure SPI1 pins: SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
    LED_DRV_OE(LEVELE_HIGH);
}


void SysDelay(u32 ms)
{
    u32 tick;
    tick = gSystemTick;
    
    while (((gSystemTick - tick) < (ms / 10)))
    {
#if WATCH_DOG
        IWDG_ReloadCounter();
#endif
    }
}

void eth_init(void)
{
    static char LwipInitFlag = 0;
    ETH_BSP_Config(1);
    
    if (LwipInitFlag == 0)
    {
        LwipInitFlag = 1;
        LwIP_Init();
        telnet_init();
        // udp_server_init();
        broadcast_init();
        IAP_httpd_init();
    }
}

extern void updateTest(void);

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
    //  static uint32_t gongfang_timer = 0;
    static uint32_t run_timer = 0;
    int *test;
    int i;
    RCC_ClocksTypeDef RCC_Clocks;
    char eth_flag = 0;
    char LwipInitFlag = 0;
    uint16_t net_tick = 0;
    //gLedTest.test_flag = 0;
    DispTypeInit(DISP_TYPE_NO_PROCESS);
    //  RTC_InitTypeDef RTC_InitStructure;
    STM_EVAL_LEDInit(LED1);
    //  uart_init(COM3);
    //  uart_init(COM2);
    //  uart_init(COM1);
    //  uart3_config(9600,USART_Parity_No);
    //  uart1_config(9600,USART_Parity_No);
    //  uart2_config(9600,USART_Parity_No);
    NVIC_Configuration();
    //////////////////////////////////////////
    RTC_Configuration();
    GPIO_Configuration();
    uart_init(COM1);
    uart1_config(9600, USART_Parity_No);
    //  gLedImageBuf = (u8*)mem_calloc(16*(LED_WIDTH/8), 1);
    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    gDispStat.disp_type = DISP_TYPE_DARK;
    /* SystTick configuration: an interrupt every 1ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / SYS_TICK_TIME);
    /////////////////////////////////////////////
    //  test_bmp();
#if 1  //jerry
    /* configure ethernet (GPIOs, clocks, MAC, DMA) */
    ETH_BSP_Config(0);
#endif
    save_init();
    //SD_Init();
    /* Initialize the LwIP stack */
#if 0 //jerry
    LwIP_Init();
    telnet_init();
    // udp_server_init();
    broadcast_init();
#endif
    //    device_addr_init();
    //    get_device_addr();
    /*
        RTC_InitStructure.RTC_AsynchPrediv    = AsynchPrediv;
        RTC_InitStructure.RTC_SynchPrediv   =  SynchPrediv;
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStructure);
    */
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
#if 0 //jerry
    IAP_httpd_init();
#endif
    ////////////////////////////////////////
    //updateTest();
    /////////////////////////////////////////////////
    //////////////////////////////////////////////////
    //Delay(9000);
    //set_rtc_date(10, 11, 11);
    //set_rtc_time(5,5,5);
#if WATCH_DOG
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_128);
    IWDG_SetReload(0xaaa);
    IWDG_Enable();
    IWDG_ReloadCounter();
#endif
    com422_1_work_mode = NORMAL;
    com422_2_work_mode = NORMAL;
    GetBmpUpdateFlag();
    GetConfig();
#if 0
    
    for (i = LINE1_CONFIG_ADDR; i < VERSION_FLAG_ADDR; i++)
    {
        page_erase(i);
        IWDG_ReloadCounter();
    }
    
#endif
#if 1
    PHY_RESET(0);
    SysDelay(500);
    PHY_RESET(1);
    SysDelay(500);
#endif
    //SysDelay(3000);
#if 0
    
    //??????????????????
    if (get_version_flag())
    {
        DispTypeInit(DISP_TYPE_VERSION);
        write_version_flag(0);
    }
    else
    {
        DispTypeInit(DISP_TYPE_NO_PROCESS);
    }
    
#endif
    gDispStat.scr_sav_tick = gSystemTick;
    gScrSavTick = 0;
    gUpdateBmpSt.success_flag = BMP_OK;
    //gLedTest.test_flag = 0;
    //gLedTest.test_index = 0;
    //gLedTest.chg_flag = 0;
    //gLedTest.test_tick = gSystemTick;
    DispTypeInit(DISP_TYPE_NO_PROCESS);
    //DispTypeInit(DISP_TYPE_LED_TEST);
    //updateTest();
    
    /*
    gTrigerValue.triger_flag = 1;
    gTrigerValue.triger_type = TRIGER_DOOR_CLOSE;
    gTrigerValue.station_start= 1;
    gTrigerValue.station_end= 16;
    gTrigerValue.station_current= 2;
    gTrigerValue.station_next= 2;
    gTrigerValue.door= DOOR_SIDE_LEFT;
    */
    while (1)
    {
        GetDeviceId();
        
        if (check_timer(&run_timer, 300)) //??????
        {
            STM_EVAL_LEDToggle(LED1);
            
            if (eth_flag == 0)
            {
#if 1
            
                if (ETH_ReadPHYRegister(1, PHY_BSR) & PHY_Linked_Status)
                {
                    net_tick ++;
                    
                    if (net_tick > 3)
                    {
                        eth_flag = 1;
                        PHY_RESET(0);
                        SysDelay(20);
                        PHY_RESET(1);
                        eth_init();
                    }
                }
                else
                {
                    net_tick = 0;
                }
                
#endif
            }
        }
        
        com1_proc();
        ptu_proc();//usb ????
#if WATCH_DOG
        IWDG_ReloadCounter();
#endif
#if CHOISE_PTU
        DispFun_DS();   //??3 ??4 PTU       //????????????????????????????????????????
#else
        DispFun_NDS();  //????PTU         ??LED??????????????????????   ????????PTU????????????????
#endif
#if 1
        
        if (eth_flag == 1)
        {
            /* check if any packet received */
            if (ETH_CheckFrameReceived())
            {
                /* process received ethernet packet */
                LwIP_Pkt_Handle();
            }
            
            /* handle periodic timers for LwIP */
            LwIP_Periodic_Handle(LocalTime);
        }
        
#endif
        
        if (gScrSavTime > (NO_PROCESS_TIME ) * 20 || (gScrSavTick > (NO_PROCESS_TIME ) * 300))                          /* ??20????*/
        {
            if (gDispStat.disp_type != DISP_TYPE_DOWNING)
            {
                //DispTypeInit(DISP_TYPE_NO_PROCESS);
                if (FIRST == 0)
                {
                    FIRST = 1;
                    gDispStat.change_lang_flag = 1;
                    gDispStat.curr_lang == LANG_ENGLISH;
                }
                
                if (gDispStat.change_lang_flag == 1)
                {
                    gDispStat.change_lang_flag = 0;
                    
                    if (1)
                    {
                        if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
                        else gDispStat.curr_lang = LANG_ENGLISH;
                    }
                    
                    GetBmp(BMP_TYPE_MSG, 7, gDispStat.curr_lang);
                    gTrigerValue.triger_flag = 0;
                }
                
                //gScrSavTick = 0;
                gOldTerminal = 0xff;
#if LIGHT_SIDE
                gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
#endif
            }
        }
        
        if (gErrorTime > ((NO_PROCESS_TIME ) * 150) )                           /*??3????*/
        {
            gErrorTime = 0;
            LedSetColour(LED_COLOR_RED);
            gDispStat.disp_type = DISP_TYPE_NO_PROCESS;
            gDispStat.column_index = 0;
            gDispStat.row_index = 16;
            gDispStat.width = IMAGE_NO_PROCESS_WIDTH * 8;
            gScrSavTick = 0;
            gOldTerminal = 0xff;
            gDownFirmware = 0;
        }
        
#if WATCH_DOG
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
#if WATCH_DOG
        //????
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
//????????:??????????????????????
//????????:timer---??????????????????   interval---????????
//????????:??????10ms
//????????:??
//***********************************************************************************************************************
uint8_t check_timer(uint32_t *timer, uint32_t interval)
{
    if (interval)
    {
        //????????
        if (LocalTime < (*timer))
            *timer = 0;
            
        //??????????????
        if (LocalTime - (*timer) >= interval)
        {
            *timer = LocalTime;
            return 1;
        }
    }
    
    return 0;
}

uint32_t get_time()
{
    return LocalTime;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

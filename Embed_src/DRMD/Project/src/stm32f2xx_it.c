/**
  ******************************************************************************
  * @file    stm32f2xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-May-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f2xx_it.h"
#include "main.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "stm32_eval.h"
#include  "usbd_hid_core.h"
#include "stm32_eval_sdio_sd.h"
#ifdef USE_STM3210C_EVAL
 #include "stm3210c_eval_ioe.h"
#else
 #include "stm322xg_eval_ioe.h"
#endif
#include "LedDrv.h"
#include "include.h"

u32 gSystemTick;
u32 gScrSavTick;
u32 gErrorTime;
u32 gScrSavTime;
extern u8 SpiDmaBuf[LED_PHYSICS_WIDTH];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define CURSOR_STEP     10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
//static uint8_t *USBD_HID_GetPos (void);
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}




#if 0
static u16 doorSideShapeLBuf[16] = {
	0x07e0,
	0x07e0,
	0x07e0,
	0x07e0,
	0x0fF0,
	0x07E0,
	0x03C0,
	0x0180,
	0xFfff,
	0xFfff,
	0xFfff,
	0xFfff,
	0xFfff,
	0xFfff,
	0xFfff,
	0xFfff,
};

static u16 doorSideShapeRBuf[16] = {
	0x07e0,
	0x07e0,
	0x07e0,
	0x07e0,
	0x1fF0,
	0x0fE0,
	0x07C0,
	0x0380,
	0xfffF,
	0xfffF,
	0xfffF,
	0xfffF,
	0xfffF,
	0xffff,
	0xfffF,
	0xfffF,
};


void FillDoorSide( char side )
{
	tDoorStat doorStat;
	u8 i, bit;
	u16 temp;
	u8 darkFlag = 0;
	static u32 tick;

	temp = (side==0)? (doorSideShapeLBuf[gDispStat.row_index]) : (doorSideShapeRBuf[gDispStat.row_index]);
	
	//temp = doorSideShapeBuf[gDispStat.row_index];
	//temp = 0xfffE;
	doorStat = GetDoorStat();
	doorStat.mDoorSideColor = LED_COLOR_GREEN; //固定绿色显示

	darkFlag = 0;
	if (doorStat.mDoorSideStat == DOOR_SIDE_DARK) darkFlag = 1;
	else if (doorStat.mDoorSideStat == DOOR_SIDE_FLASH)
	{
		tick ++;
		if (tick < 600) darkFlag = 1;
		else if (tick < 1200) darkFlag = 0;
		else tick = 0;
	}
	
	LedSetColour( doorStat.mDoorSideColor );
	for (i=0; i<16; i++)
	{
		LED_DRV_CLK(0);
		if (temp&0x8000) 
		{
			LED_DRV_DATA(1);
			LED_DRV_DATA(1);
			LED_DRV_DATA(1);
			LED_DRV_DATA(1);
		}
		else 
		{
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
		}
		if (darkFlag) LED_DRV_DATA(0);
		LED_DRV_CLK(1);
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
		temp <<= 1;
	}
}

#else 

void FillDoorSide( char side )
{
	tDoorStat doorStat;
	u8 i, bit;
	u8 darkFlag = 0;
	static u32 tick;
	
	//temp = doorSideShapeBuf[gDispStat.row_index];
	//temp = 0xfffE;
	doorStat = GetDoorStat();
	doorStat.mDoorSideColor = LED_COLOR_RG;  // 
//doorStat.mDoorSideStat = DOOR_SIDE_FLASH;

	darkFlag = 0;
	if (doorStat.mDoorSideStat == DOOR_SIDE_DARK) darkFlag = 1;
	else if (doorStat.mDoorSideStat == DOOR_SIDE_FLASH)
	{
		tick ++;
		if (tick < 1000) darkFlag = 1;
		else if (tick <2000) darkFlag = 0;
		else tick = 0;
	}
	
	//LedSetColour( doorStat.mDoorSideColor );
	LED_DRV_CLK(0);
	LED_DRV_DATA(0);
	LED_DRV_DATA(0);
	LED_DRV_DATA(0);
	LED_DRV_DATA(0);
	if (darkFlag) LED_DRV_DATA(1);
	if (darkFlag) LED_DRV_DATA(1);
	if (darkFlag) LED_DRV_DATA(1);
	LED_DRV_CLK(1);
	LED_DRV_CLK(1);	
	LED_DRV_CLK(1);	
	LED_DRV_CLK(1);	
}

#endif



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
	static u32 tick;
  	u8 i;
	u8 bit;
	static u8 dma_flag =0;

	tick ++;
	if (tick >= 7) {
		gSystemTick ++;	
		tick = 0;
	}
	gScrSavTick ++;
	gScrSavTime ++;
	if((gDispStat.disp_type == DISP_TYPE_ERROR)||(gDispStat.disp_type == DISP_TYPE_ERROR_DOWN))
		gErrorTime++;
	else
		gErrorTime = 0;
	//GetDeviceId();
	Time_Update();
	LedDispId();
	LoadRowData();

	//SpiDmaInit();

#if 1
	#if LIGHT_SIDE

		FillDoorSide( 0 );
		FillDoorSide( 0 );		 

	#endif

/*
	for (i=0; i<16; i++)
	{
		LED_DRV_CLK(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
		
		LED_DRV_CLK(1);
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
	}
*/
	

	LedSetColour( gTrigerValue.disp_color );
	for (i=0; i<LED_PHYSICS_WIDTH; i++) {
		for (bit=0; bit<8; bit++) {
			LED_DRV_CLK(0);
			if (SpiDmaBuf[i] & (0x80 >> bit)) 
			{
				LED_DRV_DATA(1);
				LED_DRV_DATA(1);
				LED_DRV_DATA(1);
				LED_DRV_DATA(1);
			}
			else 
			{
				LED_DRV_DATA(0);
				LED_DRV_DATA(0);
				LED_DRV_DATA(0);
				LED_DRV_DATA(0);
			}
			
			LED_DRV_CLK(1);
			LED_DRV_CLK(1);			
			LED_DRV_CLK(1);			
			LED_DRV_CLK(1);	
		}
	}

/*
	for (i=0; i<16; i++)
	{
		LED_DRV_CLK(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
			LED_DRV_DATA(0);
		
		LED_DRV_CLK(1);
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
		LED_DRV_CLK(1);	
	}
*/

	
	//FillDoorSide( 1 );
	
	LED_DRV_OE(LEVELE_HIGH);
	for (i=0; i<100; i++);
	LED_DRV_LATCH(LEVELE_HIGH);
	LedSetRow(gDispStat.row_index);
	for (i=0; i<50; i++);
	LED_DRV_LATCH(LEVELE_LOW);	
	LED_DRV_OE(LEVELE_LOW);
#endif
	
}

void USART1_IRQHandler(void)
{
 
   u16 i; 
    /*处理接收到的数据*/ 
    if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET) 
    {               
        i = USART_ReceiveData(USART1); 
	 uart1_recv_buf[uart1_recv_fifo_in++] = (u8)i;
	 if(uart1_recv_fifo_in >= UART1_BUF_LEN)
	 	uart1_recv_fifo_in = 0;
    } 
}
void USART2_IRQHandler(void)
{
 
   u16 i; 
    /*处理接收到的数据*/ 
    if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==SET) 
    {               
        i = USART_ReceiveData(USART2); 
	 uart2_recv_buf[uart2_recv_fifo_in++] = (u8)i;
	 if(uart2_recv_fifo_in >= UART2_BUF_LEN)
	 	uart2_recv_fifo_in = 0;
    } 
}
void USART3_IRQHandler(void)
{
 
   u16 i; 
    /*处理接收到的数据*/ 
    if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)==SET) 
    {               
        i = USART_ReceiveData(USART3); 
	 uart3_recv_buf[uart3_recv_fifo_in++] = (u8)i;
	 if(uart3_recv_fifo_in >= UART3_BUF_LEN)
	 	uart3_recv_fifo_in = 0;
    } 
}


void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_STM3210C_EVAL 
void EXTI9_5_IRQHandler(void)
#else
void EXTI15_10_IRQHandler(void)
#endif
{
  if (EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
  {
 
    USB_OTG_ActiveRemoteWakeup(&USB_OTG_dev);
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS  
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
	/* Reset SLEEPDEEP and SLEEPONEXIT bits */
	SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

	/* After wake-up from sleep mode, reconfigure the system clock */
	SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    /* Reset SLEEPDEEP and SLEEPONEXIT bits */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

	/* After wake-up from sleep mode, reconfigure the system clock */
	SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif


u32 gEndCnt;


/**
  * @brief  This function handles DMA0 Stream0 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream3_IRQHandler(void)
{
	u32 i;
	
  /* Test on DMA2_Stream0 channel0 Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))
  {
    /* Get Current Data Counter value after complete transfer */
    gEndCnt = DMA_GetCurrDataCounter(DMA2_Stream3);

    /* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
    DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);

	for (i=0; i<400; i++);
	SPI_Cmd(SPI1, DISABLE);  
	DMA_Cmd(DMA2_Stream3, DISABLE);

	LED_DRV_OE(LEVELE_HIGH);
	for (i=0; i<100; i++);
	LED_DRV_LATCH(LEVELE_HIGH);
	LedSetRow(gDispStat.row_index);
	for (i=0; i<50; i++);
	LED_DRV_LATCH(LEVELE_LOW);	
	LED_DRV_OE(LEVELE_LOW);
  }
}



/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

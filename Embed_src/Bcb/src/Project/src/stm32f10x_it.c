/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V3.0.0
* Date               : 04/06/2009
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "platform_config.h"
#include "systick.h"
#include "uart1.h"
#include "uart2.h"
#include "stm32f10x_usart.h"
#include "include.h"
#include "hw_config.h"



static u32 gPcTick;
static u32 gPaChkTick;
extern void PcLineStatUpdate(u8 stat);
extern void PaLineStatUpdate(u8 stat);


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
	gSystemTick ++;
	gPaChkTick ++;
	if (0 == PA_CHK())
	{
		if (gPaChkTick > 500) PaLineStatUpdate(0);
	}
	else 
	{
		PaLineStatUpdate(1);
		gPaChkTick = 0;
	}

	gPcTick ++;
	if (0 == PC_CHK())
	{
		if (gPcTick > 500) PcLineStatUpdate(0);
	}
	else 
	{
		PcLineStatUpdate(1);
		gPcTick = 0;
	}
	
    TimingDelay_Decrement();
}



/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(GPIO_KEY_EXTI_Line) != RESET)
  {

    /* Clear the EXTI line 9 pending bit */
    EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);
  }
}



void USART1_IRQHandler(void)
{
 
   u16 i; 
    /*∩|角赤?車那?米?米?那y?Y*/ 
    if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET) 
    {               
        i = USART_ReceiveData(USART1); 
	 uart1_recv_buf[uart1_recv_fifo_in++] = (u8)i;
	 if(uart1_recv_fifo_in >= UART1_BUF_LEN)
	 	uart1_recv_fifo_in = 0;
    }
	
	if (USART_GetFlagStatus( USART1,USART_FLAG_TXE ) == SET) {
		USART_ClearFlag( USART1,USART_FLAG_TXE );
		if (uart1_send_cnt > 0) {
			USART_SendData(USART1, uart1_send_buf[uart1_send_index++]);
			uart1_send_cnt --;
			if (0 == uart1_send_cnt)
			{
				USART_ITConfig(USART1, USART_IT_TC, ENABLE);
				//uart1_clr_busy();
			}
		}
		else {
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}	
	if (USART_GetFlagStatus( USART1,USART_FLAG_TC ) == SET) 
	{
		uart1_clr_busy();
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	}	
	
}

void USART2_IRQHandler(void)
{
 
   u16 i; 
    /*∩|角赤?車那?米?米?那y?Y*/ 
    if(USART_GetFlagStatus(USART2,USART_IT_RXNE)==SET) 
    {               
         i = USART_ReceiveData(USART2); 
		 uart2_recv_buf[uart2_recv_fifo_in++] = (u8)i;
		 if(uart2_recv_fifo_in >= UART2_BUF_LEN) uart2_recv_fifo_in = 0;
    } 

	if (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == SET) {
		if (uart2_send_cnt > 0) {
			USART_SendData(USART2, uart2_send_buf[uart2_send_index++]);
			uart2_send_cnt --;
			if (0 == uart2_send_cnt)
			{
				USART_ITConfig(USART2, USART_IT_TC, ENABLE);
				/*
				uart2_clr_busy();
				TRAIN485_INPUT();
				LED_TRAIN_COM_CLOSE();
				*/
			}
		}
		else {
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
	}	

	if (USART_GetFlagStatus( USART2,USART_FLAG_TC ) == SET) 
	{
		USART_ClearFlag(USART2,USART_FLAG_TC);
		uart2_clr_busy();
		TRAIN485_INPUT();
		LED_TRAIN_COM_CLOSE();
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	}

}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

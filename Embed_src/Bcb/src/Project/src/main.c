/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �������������ݣ�����ģ���ȡ�����������ݶ���������
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/


#include "include.h"
#include "uart1.h"
#include "uart2.h"
#include "stm32f10x_dbgmcu.h"
#include "led_control.h"
#include "data_train.h"
#include "data_box.h"
#include "led_control.h"
#include "set_vlum.h"
#include "stat_switch.h"

void Delay(__IO uint32_t nCount);

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void startWatchdog(void)
{
	DBGMCU_Config( DBGMCU_CR_DBG_IWDG_STOP, DISABLE);

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);	
	IWDG_SetReload(0xfff);	
	IWDG_ReloadCounter();	
	IWDG_Enable();	
	//IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);

	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	
}

void resetWatchDog(void)
{
	IWDG_ReloadCounter();
}


//////////////////////////////////////////////////////////////////
void NVIC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;


#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


  //USART1�ж�����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  //USART2�ж�����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  


  //EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);
}


void RCC_Configuration(void)
{	
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

	//����ADCʱ��Ƶ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
    
  //Ϊ����ģ�鿪��ʱ��
  //GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF, ENABLE);
  //AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  //USART1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //USART2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

}


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure USART1 Tx (PA.09) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  // Configure USART1 Rx (PA.10) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART2 Tx (PA.2) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  // Configure USART2 Rx (PA.3) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

 
  GPIO_InitStructure.GPIO_Pin = PA_DIRECT_CONN_CTRL_BIT | PA_LEVEL_CTRL_BIT | PC_LEVEL_CTRL_BIT |  \
  	                                        VOLUME_CLK_CTRL_BIT | VOLUME_DATA_CTRL_BIT | LED_PC_CTRL_BIT | \
  	                                        LED_BOX_COM_BIT | PLAY_VOICE_CTRL_BIT | MIC_AUDIO_CTRL_BIT | MIC_CONN_BIT | \
  	                                        MONITOR_CONN_BIT | MONI_SELECT_CTRL_BIT | MONI_SELECT_PAORPC_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = TRAIN485_CTRL_BIT | LED_TRAIN_COM_BIT | LED_PA_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


//------------------------------------------------------------------------------------------------
// below input 

  GPIO_InitStructure.GPIO_Pin = PA_CHK_BIT | PC_CHK_BIT | ADDR0_BIT | ADDR1_BIT | KEY_SIGNAL_BIT | PLAY_STAT_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	TRAIN485_INPUT();
	PA_DIRECT_CONN_DISABLE();
	SetLocalPaLineStat(1);
	SetLocalPcLineStat(1);
	VOLUME_CLK_LOW();
	VOLUME_DATA_LOW();
	LED_PC_CLOSE();
	LED_BOX_COM_CLOSE();
	LED_TRAIN_COM_CLOSE();
	LED_PA_CLOSE();
	PLAY_VOICE_DISABLE();
	MIC_AUDIO_TO_PC();
	MIC_CONN_DISABLE();
	MONITOR_CONN_DISABLE();  //��ֹ�ϵ�ʱ������ž��
	MONI_SELECT_PAPC();
	MONI_SELECT_PA();

  
}




void SysStatInit()
{
	SetDeviceId();
	TalkStatInit();
	VolumeInit();

	TRAIN485_INPUT();
	VOLUME_CLK_LOW();
	VOLUME_DATA_LOW();
	LED_PA_CLOSE();
	LED_BOX_COM_CLOSE();
	LED_TRAIN_COM_CLOSE();
	LED_PC_CLOSE();
	MIC_AUDIO_TO_PC();


	SaveVolume(1, 9);
	SaveVolume(2, 13);
	SaveVolume(3, 13);
	SaveVolume(4, 13);
	SaveVolume(5, 13);
	SaveVolume(6, 13);
}


extern void ptu_proc(void);
////////////////////////////////////////////////////////////////////
int main(void)
{
//  ע��: ������޸ĳ�ʼ��˳�򡣺������ǰ���Ϊ����

	static unsigned long tick;

	RCC_Configuration();

	NVIC_Configuration();
	SysTick_Configuration();
	GPIO_Configuration();

	tick = gSystemTick;
	while(gSystemTick-tick<500);
	SysStatInit();

	//box
	uart1_config();
	
	//train
	uart2_config();
	
	TrainDataInit();
	BoxDataInit();
	
	LedDispDeviceId();

	startWatchdog();
	while (1)
	{
		KeySignalChk();
		
		TrainDataProc();
		BoxDataProc();


		//˾���Խ����˿ͶԽ����˹��㲥״̬�л�
		TalkStatSwitch();

		LedProc();
		resetWatchDog();
	}
}



/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0;nCount--);
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

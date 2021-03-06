/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
//static uint32_t s_u32ExtClockKHz = 12000;
static uint32_t s_u32ExtClockKHz = 32;

////static PWRWU_CALLBACK PWRWU_callbackFn;

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               None	                                                         						   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               The DrvSYS version number                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to get the DrvSYS version number                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_GetVersion(void)
{
	return DRVSYS_VERSION_NUM;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_ReadProductID				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None    				   										   						   */
/*                     		                           	   												   */
/*               									      		                           				   */
/* Returns:                                                                                                */
/*               Product ID																		   		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Read Product ID                             	                       					   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_ReadProductID(void)
{	
	return SYS->PDID;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_UnlockKeyAddr				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None    				   										   						   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               0 		Success																		   	   */
/*              <0		Failed					                                                           */
/* Description:                                                                                            */
/*               Unlock protected registers                             	                   			   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_UnlockKeyAddr(void)
{
	uint32_t lockbit;
	
	SYS->REGLOCK = 0x59;
	SYS->REGLOCK = 0x16;
	SYS->REGLOCK = 0x88;

	lockbit = SYS->REGLOCK & 0x01;
	return (lockbit)? 0:E_DRVSYS_ERR_UNLOCK_FAIL;
} 
 
/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_LockKeyAddr				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None    				   										   						   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               0 		Success																		   	   */
/*              <0		Failed					                                                           */
/* Description:                                                                                            */
/*               Lock protected registers                             	                   			   	   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_LockKeyAddr(void)
{
	uint32_t lockbit;
	
	SYS->REGLOCK = 0x00;

	lockbit = SYS->REGLOCK & 0x01;
	return (lockbit)? E_DRVSYS_ERR_LOCK_FAIL:0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_Delay                                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               us     - [in]      Delay time. The Max value is 335000                                    */
/*                                                                                                         */
/* Returns:                                                                                                */
/*      None                                                                                               */
/*                                                                                                         */
/* Description:                                                                                            */
/*      Use the SysTick to generate the delay time and the UNIT is in us.                                  */
/*      The SysTick clock source is from HCLK.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_Delay(uint32_t us)
{
    SysTick->LOAD = us * 48;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_GetRstSrc				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None    				   										   						   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               Reset source																		   	   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Get reset source from last operation                             	           		   	   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_GetRstSrc(void)
{
	return *((__IO uint32_t *) &SYS->RSTSRC);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_ClearRstSrc				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32Src    	- [in]    The corresponding bit of reset source			   					   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               0																		   	   			   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Clear reset source by write 1                             	           		   	   		   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_ClearRstSrc(uint32_t u32Src)
{
	*((__IO uint32_t *) &SYS->RSTSRC) = u32Src;
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_ResetIP				                                               	   		   	   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               eIpRst  - [in]  	E_SYS_GPIO_RST  / E_SYS_TMR0_RST / E_SYS_TMR1_RST / E_SYS_TMR2_RST	   */
/*	 					    		E_SYS_TMR3_RST  / E_SYS_I2C0_RST / E_SYS_I2C1_RST / E_SYS_SPI0_RST	   */
/*									E_SYS_SPI1_RST  / E_SYS_SPI2_RST / E_SYS_SPI3_RST / E_SYS_UART0_RST	   */
/*									E_SYS_UART1_RST	/ E_SYS_UART2_RST/ E_SYS_PWM03_RST/ E_SYS_PWM47_RST	   */
/*									E_SYS_ACMP_RST  / E_SYS_PS2_RST  / E_SYS_CAN0_RST / E_SYS_USBD_RST 	   */
/*									E_SYS_ADC_RST   / E_SYS_I2S_RST  / E_SYS_PDMA_RST			   		   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Reset coresponding IP                            	           		   	   		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_ResetIP(E_SYS_IP_RST eIpRst)
{	
	uint32_t u32Reg;

	if (eIpRst == E_SYS_PDMA_RST)
	{
		SYS->IPRSTC1.PDMA_RST = 1;
		SYS->IPRSTC1.PDMA_RST = 0;
	}
	else
	{
		u32Reg = *((__IO uint32_t *) &SYS->IPRSTC2);
		*((__IO uint32_t *) &SYS->IPRSTC2) = u32Reg | (1<<eIpRst);
		*((__IO uint32_t *) &SYS->IPRSTC2) = u32Reg & ~(1<<eIpRst);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_ResetCPU				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None			   						   												   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Reset CPU                           	           		   	   		   					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_ResetCPU(void)
{
	SYS->IPRSTC1.CPU_RST = 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_ResetChip				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None			   						   												   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Reset whole chip                           	           		   	   		   			   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_ResetChip(void)
{
	SYS->IPRSTC1.CHIP_RST = 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetRCAdjValue				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32Adj		- [in]	RC adjustment value  						   					       */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               None 										 											   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Set RC adjustment value            		   	   			       						   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_SetRCAdjValue(uint32_t u32Adj)
{
	SYS->OSCTRIM[0].TRIM = u32Adj;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetIPClock				                                        	       	  	   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               eIpClk    	- [in]  E_SYS_WDG_CLK   / E_SYS_RTC_CLK   / E_SYS_TMR0_CLK  / E_SYS_TMR1_CLK   */
/*	 					    		E_SYS_I2C0_CLK  / E_SYS_SPI0_CLK  / E_SYS_DPWM_CLK	/ E_SYS_UART0_CLK  */
/*									E_SYS_BIQ_CLK   / E_SYS_CRC_CLK   / E_SYS_PWM01_CLK / E_SYS_ACMP_CLK   */
/*									E_SYS_SBRAM_CLK / E_SYS_ADC_CLK   / E_SYS_I2S_CLK   / E_SYS_ANA_CLK    */
/*									E_SYS_PDMA_CLK  / E_SYS_ISP_CLK   									   */
/*              					                                                           			   */
/* 				 i32Enable	- [in] 	1 : Enable / 0 : Disable                    						   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable/Disable coresponding IP clock                           	   	   		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_SetIPClock(E_SYS_IP_CLK eIpClk, int32_t i32Enable)
{	
	uint32_t u32Reg;

	if (eIpClk == E_SYS_PDMA_CLK)
	{
		SYSCLK->AHBCLK.PDMA_EN = i32Enable; //0x5000_0204[1]
	}
	else if (eIpClk == E_SYS_ISP_CLK)
	{
		SYSCLK->AHBCLK.ISP_EN = i32Enable; //0x5000_0204[2]
	}
	else
	{
		u32Reg = *((__IO uint32_t *) &SYSCLK->APBCLK);
		
		if (i32Enable)
		{
			*((__IO uint32_t *) &SYSCLK->APBCLK) = u32Reg | (1<<eIpClk); //0x5000_0208
		}
		else
		{
			*((__IO uint32_t *) &SYSCLK->APBCLK) = u32Reg & ~(1<<eIpClk);
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetHCLKSource				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*           u8ClkSrcSel       - [in] 	0: Internal 48M clock                      	   					   */
/*          							1: External 32K clock                      	   					   */
/*            							2: Internal 10K clock              	   							   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*                 0:  Success																			   */
/*               < 0:  Wrong value											                    		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to select HCLK clock source                                   	   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_SetHCLKSource(uint8_t u8ClkSrcSel)
{
	if (u8ClkSrcSel > 2)
	{
		return E_DRVSYS_ERR_ARGUMENT;
	}
	else
	{
		SYSCLK->CLKSEL0.HCLK_S = u8ClkSrcSel; //0x5000_0210[2:0]
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetSysTickSource				                                              		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*           u8ClkSrcSel       - [in] 	0: Internal 10K clock                      	   					   */
/*           						 	1: External 32K clock                      	   					   */
/*          							2: Internal 10K clock / 2                      	   				   */
/*            							3: Internal 48M clock            	   							   */
/*            							4~7: HCLK / 2        		                     	   			   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*                 0:  Success																			   */
/*               < 0:  Wrong value											                    		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to select sysyem tick clock source                              	   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_SetSysTickSource(uint8_t u8ClkSrcSel)
{
	if (u8ClkSrcSel > 7)
	{
		return E_DRVSYS_ERR_ARGUMENT;
	}
	else
	{
		SYSCLK->CLKSEL0.STCLK_S = u8ClkSrcSel;  //0x5000_0210[5:3]
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetIPClockSource	     		                                               	   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               eIpClkSrc	 - [in]     E_SYS_I2S_CLKSRC   / E_SYS_PWM01_CLKSRC/ E_SYS_TMR1_CLKSRC 	   	   */
/*               	     		 		E_SYS_TMR0_CLKSRC  / E_SYS_DPWM_CLKSRC / E_SYS_WDG_CLKSRC		   */
/*                      																			       */
/*               u8ClkSrcSel - [in]    Corresponding clock source                     	 				   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*                 0:  Success																			   */
/*               < 0:  Wrong value											                    		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               This function is used to select IP clock source	       		   	   		   			   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_SetIPClockSource(E_SYS_IP_CLKSRC eIpClkSrc, uint8_t u8ClkSrcSel)
{
	
	switch(eIpClkSrc)
	{
		case E_SYS_I2S_CLKSRC:
			SYSCLK->CLKSEL2.I2S_S = u8ClkSrcSel;  //0x5000_021C[1:0]
			break;
		case E_SYS_PWM01_CLKSRC:
			SYSCLK->CLKSEL1.PWM01_S = u8ClkSrcSel;//0x5000_0214[29:28]
			break;
		case E_SYS_TMR1_CLKSRC:
		    SYSCLK->CLKSEL1.TMR1_S = u8ClkSrcSel;//0x5000_0214[14:12]
			break;				
		case E_SYS_TMR0_CLKSRC:
		    SYSCLK->CLKSEL1.TMR0_S = u8ClkSrcSel;//0x5000_0214[10:8]
			break;				
		case E_SYS_DPWM_CLKSRC:
		    SYSCLK->CLKSEL1.DPWM_S = u8ClkSrcSel;//0x5000_0214[4]
			break;				
		case E_SYS_WDG_CLKSRC:
		    SYSCLK->CLKSEL1.WDG_S = u8ClkSrcSel;//0x5000_0214[1:0]
			break;				
		default:
			return E_DRVSYS_ERR_IPSRC;
	}
	
	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetClockDivider 		                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*           eIpDiv   - [in] E_SYS_ADC_DIV / E_SYS_UART_DIV /E_SYS_HCLK_DIV*/
/*           i32value - [in] 0 ~ 255 for ADC, 0 ~ 15 for other IP or HCLK  								   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*           0:  Success																			   	   */
/*           < 0:  Wrong value											                    		       */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               This function is used to set IP divider number from the corresponding clock source	   	   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_SetClockDivider(E_SYS_IP_DIV eIpDiv , int32_t i32value)
{
	if (eIpDiv == E_SYS_ADC_DIV)
	{
	 	SYSCLK->CLKDIV.ADC_N = i32value; //0x5000_0218[23:16]
	}
//	else if(eIpDiv == E_SYS_CAN_DIV)
//	{
//	 	SYSCLK->CLKDIV.CAN_N = i32value;
//	}
	else if(eIpDiv == E_SYS_UART_DIV) 
	{
	 	SYSCLK->CLKDIV.UART_N = i32value;//0x5000_0218[11:8]
	}
//	else if(eIpDiv == E_SYS_USB_DIV)
//	{
//	 	SYSCLK->CLKDIV.USB_N = i32value;
//	}
	else if(eIpDiv == E_SYS_HCLK_DIV)
	{
	 	SYSCLK->CLKDIV.HCLK_N = i32value; //0x5000_0218[3:0]
	}
	else
		return E_DRVSYS_ERR_IPDIV;

	return E_SUCCESS;
	
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_SetOscCtrl	     		                                               	   	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               eOscCtrl	 - [in]    E_SYS_XTL32K  / E_SYS_OSC49M / E_SYS_OSC10K					 	   */
/*               i32Enable   - [in]    1: Enable / 0: Disable                     	 					   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*                 0:  Success																			   */
/*               < 0:  Wrong value											                    		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               This function is used to enable/disable Oscillator	       		   	   		   			   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvSYS_SetOscCtrl(E_SYS_OSC_CTRL eOscCtrl, int32_t i32Enable)
{	
	switch(eOscCtrl)
	{
		case E_SYS_XTL32K:
			SYSCLK->PWRCON.XTL32K_EN = i32Enable;
			break;
		case E_SYS_OSC49M:
		    SYSCLK->PWRCON.OSC49M_EN = i32Enable;
			break;
		case E_SYS_OSC10K:
		    SYSCLK->PWRCON.OSC10K_EN = i32Enable;
			break;				
		default:
			return E_DRVSYS_ERR_ARGUMENT;
	}
	
	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_EnableDeepPowerDown				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               i32Enable	- [in]	 1: Enable / 0: Disable 						   					   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Request a Deep Power Down on next WFI or WFE           		   	   		   			   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSYS_EnableDeepPowerDown(int32_t i32Enable)
{
	SYSCLK->PWRCON.DEEP_PD = i32Enable;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_GetEXTClock                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*      None                                                                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*      The external clock frequency.                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get external clock frequency. The clock UNIT is in kHz.                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_GetEXTClock(void)
{	
	return s_u32ExtClockKHz;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_GetHCLK                                                                            	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*      None                                                                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*      HCLK clock frequency in kHz.                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*      Get HCLK clock and the UNIT is in kHz.                                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvSYS_GetHCLK(void)
{
	
	uint32_t u32Freqout = 0, u32AHBDivider;

	if(SYSCLK->CLKSEL0.HCLK_S == 1)     	/* external 32KHz crystal clock */ 
	{
		u32Freqout = 32;
	}
	else if(SYSCLK->CLKSEL0.HCLK_S == 2)	/* internal 10KHz oscillator clock */
	{
	 	u32Freqout = 10;
	}
	else									/* internal 49MHz oscillator clock */
	{
	 	if(SYSCLK->CLKSEL0.OSCFSel)
		u32Freqout = 32768;
		else
		u32Freqout = 49152;
	
	}
	u32AHBDivider = SYSCLK->CLKDIV.HCLK_N + 1 ;
	
	return (u32Freqout/u32AHBDivider);	
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSYS_EnableFreqDividerOutput 		                                       	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               i32Enable	- [in]		  1: Enable / 0: Disable 								   		   */
/*               u8Divider	- [in]		  The divider of output frequency (0~15)					   	   */
/*                     		                           	   												   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*                 0:  Success																			   */
/*               < 0:  Wrong value											                    		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable frequency clock output and set its divider value. 								   */
/*				 The output frequency = Fin/2^(N+1), Fin is the input clock frequency, N is 4-bit value	   */
/*---------------------------------------------------------------------------------------------------------*/
/*
int32_t DrvSYS_EnableFreqDividerOutput(int32_t i32Enable, uint8_t u8Divider)
{
	if (u8Divider > 15)
		return E_DRVSYS_ERR_ARGUMENT;

	if (i32Enable)
	{
		SYSCLK->FRQDIV.FSEL = u8Divider;
	}
	SYSCLK->FRQDIV.FDIV_EN = i32Enable; 		
	DrvSYS_SetIPClock(E_SYS_FDIV_CLK, i32Enable);

	return E_SUCCESS;
}
*/



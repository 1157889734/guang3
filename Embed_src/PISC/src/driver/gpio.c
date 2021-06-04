/*******************************************************************************
* File Name          : gpio.c
* Author             : Song Wenli
* Date First Issued  : 20/02/2008
* Description        : This file provides all the GPIO software functions
********************************************************************************
* History:
*	20/02/2008 : Created
*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "include.h"

void init_gpio(void)
{
	unsigned short tmp;

	// Set GPIO2 pin 12, 13, 14, 15 is GPIO_OUT_PP
	tmp = OUT0 | OUT1 |OUT2 | OUT3 | OUT4 | OUT5| OUT6 | OUT7| OUT8;
	GPIO2_PC0 |= tmp;
	GPIO2_PC1 &= ~tmp;
	GPIO2_PC2 |= tmp;
	
	//GPIO2_PD |= tmp;	//(EN1483A_Pin | EN1483B_Pin | EN1483C_Pin);
						//GPIO2_PD &= ~LED_Pin;
	GPIO2_PD &= ~tmp;
/*
	// Set GPIO1 pin 0, 1. 2, 3, 4, 5, 6, 7, 8, 9 is GPIO_OUT_PP
	tmp = MARST_LED_Pin | SLAVE_LED_Pin | TCMS_LED_Pin | TRAIN_LED_Pin | OCC_LED_Pin | MEDIA_LED_Pin | USB_LED_Pin | OCC_S_LED_Pin | DVA_S_LED_Pin | MEDIA_S_LED_Pin;
	GPIO1_PC0 |= tmp;
	GPIO1_PC1 &= ~tmp;
	GPIO1_PC2 |= tmp;
	
	GPIO1_PD &= ~tmp;	
*/	
	// add by hk 2010-5-28
	//tmp |= (1 << 5) | (1 <<6) | (1<<7) ;		//output
	GPIO0_PC0 |= ((1 << 5) | (1 <<6) | (1<<7));
	GPIO0_PC1 &= ~((1 << 5) | (1 <<6) | (1<<7));
	GPIO0_PC2 |= ((1 << 5) | (1 <<6) | (1<<7));
	
	//GPIO0_PD |= tmp;

       GPIO0_PC0 |= (1<<4);		//input
	GPIO0_PC1 &= ~(1<<4);
	GPIO0_PC2 &= ~(1<<4);
	
	//GPIO0_PD |= (1<<4);

}


/*******************************************************************************
* File Name          : gpio.h
* Author             : Song Wenli
* Date First Issued  : 20/02/2008
* Description        : This file contains all the functions prototypes for the
*                      GPIO software library.
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
#ifndef __GPIO_H__
#define __GPIO_H__



#define EN1483A_Pin				(1 << 4)
#define EN1483B_Pin				(1 << 5)
#define EN1483C_Pin				(1 << 6)

#define LED_Pin					(1 << 15)
/*
#define MARST_LED_Pin			(1 << 0)
#define SLAVE_LED_Pin			(1 << 1)
#define TCMS_LED_Pin			(1 << 2)
#define TRAIN_LED_Pin			(1 << 3)
#define OCC_LED_Pin				(1 << 4)
#define MEDIA_LED_Pin			(1 << 5)
#define USB_LED_Pin				(1 << 6)
#define OCC_S_LED_Pin			(1 << 7)
#define DVA_S_LED_Pin			(1 << 8)
#define MEDIA_S_LED_Pin			(1 << 9)
*/
#define OUT0					(1 << 8)
#define OUT1					(1 << 9)
#define OUT2					(1 << 10)
#define OUT3					(1 << 11)
#define OUT4					(1 << 12)
#define OUT5					(1 << 13)
#define OUT6					(1 << 14)
#define OUT7					(1 << 15)
#define OUT8	                                                (1<<2)

extern void init_gpio(void);


#endif	/*	__GPIO_H__	*/
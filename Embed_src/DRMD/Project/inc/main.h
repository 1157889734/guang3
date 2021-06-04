/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-May-2011
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm32f2x7_eth_bsp.h"
#include "telnet.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32_eval.h"
#include "sd_data.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* IAP options selection ******************************************************/
//#define USE_IAP_TFTP   /* enable IAP using TFTP */
#define USE_IAP_HTTP   /* enable IAP using HTTP */
//#define USE_LCD        /* enable LCD  */  
//#define USE_DHCP       /* enable DHCP, if disabled static address is used */

/* Flash user area definition *************************************************/   
/* 
   IMPORTANT NOTE:
   ==============
   Be sure that USER_FLASH_FIRST_PAGE_ADDRESS do not overlap with IAP code.
   For example, with all option enabled the total readonly memory size using EWARM
   compiler v6.10, with high optimization for size, is 39928 bytes (32894 bytes
   of readonly code memory and 7034 bytes of readonly data memory).
   However, with TrueSTUDIO, RIDE and TASKING toolchains the total readonly memory
   size exceeds 48 Kbytes, with this result four sectors of 16 Kbytes each will
   be used to store the IAP code, so the user Flash address will start from Sector4. 

   In this example the define USER_FLASH_FIRST_PAGE_ADDRESS is set to 64K boundary,
   but it can be changed to any other address outside the 1st 64 Kbytes of the Flash.
   */
#define USER_FLASH_FIRST_PAGE_ADDRESS 0x080c0000 /* Only as example see comment */
#define USER_FLASH_LAST_PAGE_ADDRESS  0x080E0000
#define USER_FLASH_END_ADDRESS        0x080FFFFF  
   
/* UserID and Password definition *********************************************/
#define USERID       "user"
#define PASSWORD     "stm32"
#define LOGIN_SIZE   (15+ sizeof(USERID) + sizeof(PASSWORD))

/* MAC Address definition *****************************************************/
#define MAC_ADDR0   3
#define MAC_ADDR1   4
#define MAC_ADDR2   5
#define MAC_ADDR3   6 
#define MAC_ADDR4   7
#define MAC_ADDR5   2
 
/* Static IP Address definition ***********************************************/
#define IP_ADDR0   10
#define IP_ADDR1   10
#define IP_ADDR2   30
#define IP_ADDR3   24
   
/* NETMASK definition *********************************************************/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/* Gateway Address definition *************************************************/
#define GW_ADDR0   10
#define GW_ADDR1   10
#define GW_ADDR2   30
#define GW_ADDR3   1  

/* MII and RMII mode selection, for STM322xG-EVAL Board(MB786) RevB ***********/
#define RMII_MODE  // In this case the System clock frequency is configured
                     // to 100 MHz, for more details refer to system_stm32f2xx.c      
                                     
//#define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
 #define PHY_CLOCK_MCO
#endif

/* STM322xG-EVAL jumpers setting
    +==========================================================================================+
    +  Jumper |       MII mode configuration            |      RMII mode configuration         +
    +==========================================================================================+
    +  JP5    | 2-3 provide 25MHz clock by MCO(PA8)     |  2-3 provide 50MHz clock by MCO(PA8) +
    +         | 1-2 provide 25MHz clock by ext. Crystal |                                      +
    + -----------------------------------------------------------------------------------------+
    +  JP6    |          2-3                            |  1-2                                 +
    + -----------------------------------------------------------------------------------------+
    +  JP8    |          Open                           |  Close                               +
    +==========================================================================================+
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */  
/* Exported function prototypes ----------------------------------------------*/
void Time_Update(void);
void Delay(uint32_t nCount);
extern uint32_t get_time(void);
extern uint8_t check_timer(uint32_t *timer, uint32_t interval);
void LCD_LOG_SetFooter(uint8_t *Status);
void LCD_LOG_SetHeader (uint8_t *Title);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


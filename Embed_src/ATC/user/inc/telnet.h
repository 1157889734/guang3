/**
******************************************************************************
* @file    telnet.h
* @author  MCD Application Team
* @version V1.0.0
* @date    11/20/2009
* @brief   This file contains all the functions prototypes for the helloworld.c
*          file.
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
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TELNET_H
#define __TELNET_H

#ifdef __cplusplus
extern "C"
{
#endif

//#define GREETING "          Shenzhen BeiHai \r\n    "
//#define VERSION " TBA_3093   2011-12-29  V1.1\r\n"

/* Includes ------------------------------------------------------------------*/


/** @defgroup helloworld_Exported_Functions
  * @{
  */
extern struct tcp_pcb *pcb23;
void telnet_init(void);
void send_msg_to_telnet(unsigned char *buf, int length);
void send_int_buf_to_telnet(unsigned char *buf, int length);
void parse_input_parm(char *buf, int length);
/**
  * @}
  */



#ifdef __cplusplus
}
#endif

#endif /* __HELLOWERLOD_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


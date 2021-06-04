/**
  ******************************************************************************
  * @file    STM32F2xx_IAP/src/flash_if.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-May-2011
  * @brief   This file provides all the memory related operation functions.
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

/** @addtogroup STM32F2xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


pFunction Jump_To_Application;
uint32_t  JumpAddress;


saveparm_t saveparm;
saveparm_t defaultparm =
{
    {10, 10, 30, 24},      //ip address
    {255, 255, 255, 0},    //net mask
    {10, 10, 30, 1},       //gateway
    {0, 0, 0, 0, 0, 2},    //mac
};
static uint32_t GetSector(uint32_t Address);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{
    FLASH_Unlock();
    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase_Save_Parm(uint32_t StartSector)
{
    uint32_t UserStartSector = FLASH_Sector_1, i = 0;
    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(StartSector);
    
    for (i = UserStartSector; i < (UserStartSector + 8); i += 8) //use sector 7 for save parm
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */
        if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
        {
            /* Error occurred while page erase */
            return (1);
        }
    }
    
    return (0);
}


/**
  * 擦除程序备份区
  */
uint32_t FLASH_If_Erase_Prog_Bak(uint32_t StartSector)
{
    uint32_t UserStartSector = FLASH_Sector_1, i = 0;
    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(StartSector);
    
    for (i = UserStartSector; i < (UserStartSector + 8); i += 8) //sector 6 用做程序备份
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */
        if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
        {
            /* Error occurred while page erase */
            return (1);
        }
    }
    
    return (0);
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(__IO uint32_t *FlashAddress, uint32_t *Data , uint16_t DataLength)
{
    uint32_t i = 0;
    
    for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */
        if (FLASH_ProgramWord(*FlashAddress, *(uint32_t *)(Data + i)) == FLASH_COMPLETE)
        {
            /* Check the written value */
            if (*(uint32_t *)*FlashAddress != *(uint32_t *)(Data + i))
            {
                /* Flash content doesn't match SRAM content */
                return (2);
            }
            
            /* Increment FLASH destination address */
            *FlashAddress += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return (1);
        }
    }
    
    return (0);
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
    uint32_t UserStartSector = FLASH_Sector_1;
    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(SAVE_PARM_ADDRESS);
    
    /* Check if there are write protected sectors inside the user flash area */
    if ((FLASH_OB_GetWRP() >> (UserStartSector / 8)) == (0xFFF >> (UserStartSector / 8)))
    {
        /* No write protected sectors inside the user flash area */
        return 1;
    }
    else
    {
        /* Some sectors inside the user flash area are write protected */
        return 0;
    }
}

/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 1: Write Protection successfully disabled
  *         2: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_DisableWriteProtection(void)
{
    __IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;
    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(SAVE_PARM_ADDRESS);
    /* Mark all sectors inside the user flash area as non protected */
    UserWrpSectors = 0xFFF - ((1 << (UserStartSector / 8)) - 1);
    /* Unlock the Option Bytes */
    FLASH_OB_Unlock();
    /* Disable the write protection for all sectors inside the user flash area */
    FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);
    
    /* Start the Option Bytes programming process. */
    if (FLASH_OB_Launch() != FLASH_COMPLETE)
    {
        /* Error: Flash write unprotection failed */
        return (2);
    }
    
    /* Write Protection successfully disabled */
    return (1);
}

/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
    
    if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;
    }
    else if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;
    }
    else if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;
    }
    else if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;
    }
    else if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;
    }
    else if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;
    }
    else if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;
    }
    else if ((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;
    }
    else if ((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;
    }
    else if ((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;
    }
    else if ((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }
    
    return sector;
}


void save_init(void)
{
    uint32_t i;
    uint8_t sum = 0;
    uint8_t *p = (uint8_t *)&saveparm;
    FLASH_If_Init();
    memcpy((uint8_t *)&saveparm, (uint8_t *)SAVE_PARM_ADDRESS, sizeof(saveparm_t));
    
    //核对校验和
    for (i = 0; i < sizeof(saveparm_t); i++)
        sum += p[i];
        
    if (0x55 != sum)
    {
        load_default_parm();
    }
}

void load_default_parm(void)
{
    uint32_t i;
    uint8_t *p = (uint8_t *)&saveparm;
    uint8_t const *p_default = (uint8_t const *)&defaultparm;
    
    for (i = 0; i < sizeof(saveparm_t) - 1; i++)
    {
        p[i] = p_default[i];
    }
    
    save_parm();
}
void save_parm(void)
{
    uint32_t i;
    uint8_t sum = 0;
    uint8_t *p = (uint8_t *)&saveparm;
    uint32_t save_addr = SAVE_PARM_ADDRESS;
    
    for (i = 0; i < sizeof(saveparm_t) - 1; i++)
    {
        sum += p[i];
    }
    
    p[i] = 0x55 - sum;
    FLASH_If_Erase_Save_Parm(SAVE_PARM_ADDRESS);
    FLASH_If_Write((uint32_t *)&save_addr, (uint32_t *)&saveparm, sizeof(saveparm_t) / 4);
}


void exec_iap_fun(void)
{
    JumpAddress = *(__IO uint32_t *) (PROG_IAP_ADDRESS + 4);
    /* Jump to user application */
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *) PROG_IAP_ADDRESS);
    Jump_To_Application();
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

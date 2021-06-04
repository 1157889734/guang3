
//ifdef suven
#if 1
#ifndef __PROGUPDATE_H__
#define __PROGUPDATE_H__

#include "stm32f10x.h"


#define PROG_BAK_ADDRESS     (uint32_t)0x08003C00    //升级时程序备份区  
#define PARAM_ADDRESS  	   (uint32_t)0x08007C00    //升级时程序备份区 4K



#define TEMP_FLASH_ADDR  0x08003C00//Start address for program shift
#define BOOT_FLASH_ADDR  0x08000000//Internal flash base address


typedef enum
{
    update_ok = 0,
    update_param,
    update_uncontinuous,
    update_erase_flash,
}tUpdateErrNum;

u8 update_init();
u8 update_save_program(u8 *program_data,u32 program_total_len,u16 program_cur_num,u32 cur_len);
extern void ProgramUpdate(u8 *cmd_buf);
extern void UsbProgramUpdate(u8 *cmd_buf);

#endif

#else
#ifndef __PROGUPDATE_H__
#define __PROGUPDATE_H__

#include "stm32f10x.h"


#define PROG_BAK_ADDRESS     (uint32_t)0x08006000    //升级时程序备份区  
#define PARAM_ADDRESS  	   (uint32_t)0x08005000    //升级时程序备份区 4K



#define TEMP_FLASH_ADDR  0x08006000//Start address for program shift
#define BOOT_FLASH_ADDR  0x08000000//Internal flash base address


typedef enum
{
    update_ok = 0,
    update_param,
    update_uncontinuous,
    update_erase_flash,
}tUpdateErrNum;

u8 update_init();
u8 update_save_program(u8 *program_data,u32 program_total_len,u16 program_cur_num,u32 cur_len);
extern void ProgramUpdate(u8 *cmd_buf);
extern void UsbProgramUpdate(u8 *cmd_buf);

#endif

#endif


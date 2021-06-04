
//#ifdef suven
#if 1

/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �������� ��FLASH�Ĳ���
**  ��������: 2013.12.24
**
**  ��ǰ�汾��1.1
**  �޸�stm32f103xx6 cpu ����
**  ���ߣ�suven
*******************************************************************************
*/
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "core_cm3.h"
#include "progupdate.h"
#include "string.h"
#include "data_public.h"
#include "data_train.h"
#include "hw_config.h"
#include "led_control.h"

#include <stdint.h>

#define ERASEATSTART
#define FLASH_PAGE_LEN 1024

/*
void SoftwareReset(void)
{
	__asm("LDR R0,=0XE000ED0C");
	__asm("LDR R1,=0X05FA0001");
	__asm("STR R1, [R0]");

}
*/

static void UpdateDisableAllNVIC(void)
{
  for(int i=19; i < 59; i++)
  {
    NVIC->ICER[i >> 0x05] =(unsigned int )0x01 << (i & (unsigned char)0x1F);
  }
}



////////////////////////////////////////////////////////////////////////////////
#define UPDATE_DATA_BUFFER_LEN 512
#define UPDATE_FUNC_BUFFER_LEN 2048
#define UPDATE_CLI()  __set_PRIMASK(1)

void map_flash_copy_func(u32 length);

static u8  update_copy_func[UPDATE_FUNC_BUFFER_LEN];          //��ſ�������
static u32 update_func_addr;
static u8  update_flash2flash_buffer[UPDATE_DATA_BUFFER_LEN]; // ���ݻ�����


void update_flash_init(void)
{
	update_func_addr=(u32)(update_copy_func+1);//(u32)&
}

void map_flash_copy_func_new(u32 length);
void update_flash_copy_run(u32 length)
{	
    update_copy_func[0] = 0x2d;
    memcpy((u8 *)(update_copy_func+1),(u8 *)&map_flash_copy_func,\
        UPDATE_FUNC_BUFFER_LEN-1);
    UPDATE_CLI();	
    UpdateDisableAllNVIC();
    //UpdateDisableAllGPIO();
        
    ((void(*)(u32 length0))update_func_addr)(length);//run copy
}



void map_flash_copy_func(u32 length)
{
    u32 i = 0,k = 0;
    u32 addr1 = TEMP_FLASH_ADDR,addr2 = BOOT_FLASH_ADDR; 
    u32 idx = 0;
    u32 *p = (u32*)update_flash2flash_buffer;
	FLASH_Status flashstatus;
	u8 led_flag;
    

	for(k = 0; k < 15; k++)
	{
    		//������ʾ�����ڰ���
		if (0 == led_flag)
		{
			led_flag = 1;
			GPIOB->BSRR = GPIO_Pin_1;
			GPIOB->BSRR = GPIO_Pin_5;
			GPIOA->BSRR = GPIO_Pin_12;
			GPIOA->BSRR = GPIO_Pin_11;
		}
		else 
		{
			led_flag = 0;
			GPIOB->BRR = GPIO_Pin_1;
			GPIOB->BRR = GPIO_Pin_5;
			GPIOA->BRR = GPIO_Pin_12;
			GPIOA->BRR = GPIO_Pin_11;
		}	
		// flash unlock
		FLASH->KEYR =  ((u32)0x45670123);
		FLASH->KEYR = ((u32)0xCDEF89AB);

		FLASH->SR = FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR;		
		FLASH->CR|= ((u32)0x00000002);
		FLASH->AR = ((u32)(0x8000000 + (k * 0x400))); 
		FLASH->CR|= ((u32)0x00000040);
		IWDG->KR = ((u16)0xAAAA);

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
		
		FLASH->CR &= ((u32)0x00001FFD);
	}

  
    while(length)
    {
   		//������ʾ�����ڰ���
    		led_flag ++;
		if (led_flag < 2)
		{
			GPIOB->BSRR = GPIO_Pin_1;
			GPIOB->BSRR = GPIO_Pin_5;
			GPIOA->BSRR = GPIO_Pin_12;
			GPIOA->BSRR = GPIO_Pin_11;
		}
		else if (led_flag < 10)
		{
			led_flag = 0;
			GPIOB->BRR = GPIO_Pin_1;
			GPIOB->BRR = GPIO_Pin_5;
			GPIOA->BRR = GPIO_Pin_12;
			GPIOA->BRR = GPIO_Pin_11;
		}
	
        FLASH->KEYR =  ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        for(i=0;i<sizeof(update_flash2flash_buffer);i++)
        {
            update_flash2flash_buffer[i]=(*((volatile u8*)(addr1+i)));	
        }

        for (idx = 0; idx <  128; idx = idx + 1)
        {
				//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait

		
		FLASH->CR |= ((u32)0x00000001);
		*(__IO u16*)addr2 = (u16)p[idx];
 
		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
    

		*(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
     

            addr2 += 4;
		FLASH->CR &= ((u32)0x00001FFE);
        }
        addr1+=512;

	 if(length>512)
        	length-=512;
	 else
	 	break;
    }

	GPIOB->BSRR = GPIO_Pin_1;
	GPIOB->BSRR = GPIO_Pin_5;
	GPIOA->BSRR = GPIO_Pin_12;
	GPIOA->BSRR = GPIO_Pin_11;

	IWDG->KR = ((u16)0xAAAA);
	IWDG->KR = 0xCCCC;

	//NVIC_SystemReset();
	//SoftwareReset();
    while(1)
    {
    }
}

/*******************************************************************************
*��������:�����������
*����˵��:program_data: ָ�����ݵ�ָ�룬program_total_len:��������
                                             ���ܳ���
                        program_cur_num:��ǰ�������ݰ��ı��
                        cur_len:��ǰ���ݰ��ĳ���.
*ע������:���ݰ��ı�Ŵ�1��ʼ��������Ű�1������ÿ��
                        �ĳ��ȱ���Ϊ4�ı���,�����ֽ�Ϊ��λ��С�ˡ�
                        ÿ���������ΪFLASHҳ���ȷ���˳������1��ʼ
                        һֱ������һ��,�������˳�򷢰���������
                        ���װһ�η���һ�����ݵ�֮ǰ����һ��
                        update_init()���������ã����ϴγ���ʧ�ܵ�����£�
                        �ô�������ʧ�ܡ�
*******************************************************************************/

static u32 program_rcv_len = 0;
static u16 last_pack_num = 0;

u8 update_init()
{
    program_rcv_len   = 0;
    last_pack_num     = 0;
    return update_ok;
}


void FLASH_If_Init(void)
{
   FLASH_Unlock();
   FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}


void FLASH_If_Erase_Prog_Bak(void)
{
	u32 i;

	for(i = PROG_BAK_ADDRESS; i <= (PROG_BAK_ADDRESS+14*1024); i += FLASH_PAGE_LEN )
	{
   		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(i);		
	}

}
void update_flash_erase(void)
{

    FLASH_If_Init();
    FLASH_If_Erase_Prog_Bak();
	
}
//void exec_iap_fun();
u8 update_save_program(u8 *program_data,u32 program_total_len,
    u16 program_cur_num,u32 cur_len)
{
    u32 addr2,idx;
    u32 *p = (u32*)program_data;

    if(NULL == program_data||!program_total_len||!program_cur_num||!cur_len)
    {
        update_init();
        return update_param;
    }

    if(program_cur_num == 1)
    {
        update_init();
    }
    
    if(program_cur_num - last_pack_num != 1)
    {
        update_init();
        return update_uncontinuous;
    }

    last_pack_num = program_cur_num;

#ifndef ERASEATSTART
    if(FLASH_PAGE_LEN - packet_use_len < cur_len||!flash_first_erase)
    {
        FLASH->KEYR = ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        if(FLASH_ErasePage(TEMP_FLASH_ADDR+(pagecounter*FLASH_PAGE_LEN)) != \
            FLASH_COMPLETE)
        {
            update_init();
            return update_erase_flash;
        }
        pagecounter++;
        
        if(!flash_first_erase)
        {
            flash_first_erase = 1;
            packet_use_len += cur_len;
        }
        else
        {
            packet_use_len = (packet_use_len+cur_len) % FLASH_PAGE_LEN;
        }
    }
    else
    {
        packet_use_len += cur_len;
    }
#endif

    FLASH->KEYR =  ((u32)0x45670123);
    FLASH->KEYR = ((u32)0xCDEF89AB);
    addr2 = TEMP_FLASH_ADDR + program_rcv_len;

#if 0
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        } 
        FLASH->CR |= ((u32)0x00000001);
        *(__IO u16*)addr2 = (u16)p[idx];
        
        /* Wait for last operation to be completed */
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }
        *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }  
        /* Disable the PG Bit */
        addr2 += 4;
        FLASH->CR &= ((u32)0x00001FFE);
    }
#else
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        //can be better 
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR |= ((u32)0x00000001);
            *(__IO u16*)addr2 = (u16)p[idx];
            break;
        }
        
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
            break;
        }
        
        addr2 += 4;
        while(FLASH_BUSY != FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR &= ((u32)0x00001FFE);
            break;
        }
        
    }
#endif
    program_rcv_len += cur_len;

    if(program_rcv_len >= program_total_len)
    {
//#ifdef UPDATEUSERAM
        update_flash_init();
		update_flash_copy_run(program_rcv_len);
//#else
/*
        u16 tmp;
        for(tmp = 0; tmp < 150; tmp++);
        __set_FAULTMASK(1);
		exec_iap_fun();
*/		
//#endif
    }

    return update_ok;
}


typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;



static u32 rcv_len               = 0;
static u32 programmlen           = 0;
static u8  updat_rcv_packet_flag = 0;
static u16 cur_pack_num          = 0;

void ProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u16 update_lastpack = 0;
    tUpdateFrame *update_packet;
    //u32 vector_num;
    
    update_packet = (tUpdateFrame *)cmd_buf;
    if((update_packet->data.header.update_type== 1)||\
        (update_packet->data.header.update_type == 0x7d)) //���³���
	{
	    if(update_packet->data.header.update_flag == 0x01)
        {
            //��ʼ�����ı�־
            update_init();
       	//LedAlarmStatSet( LED_ALARM_UPDATE_LOSE_PAKET, LED_ALARM_IDLE );
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
            if(!updat_rcv_packet_flag)
            {
                programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

                //vector_num = (programmlen%FLASH_PAGE_LEN)?(1+((programmlen/FLASH_PAGE_LEN))):(programmlen/FLASH_PAGE_LEN);
                
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        if(updat_rcv_packet_flag)
        {
            programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

            cur_pack_num = \
                (((u16)(update_packet->data.header.high_byte_cur_packet_num)<<8)|\
                ((u16)(update_packet->data.header.low_byte_cur_packet_num)));

            if((cur_pack_num - update_lastpack) != 1)
            {
            	   //LedAlarmStatSet( LED_ALARM_IDLE, LED_ALARM_UPDATE_LOSE_PAKET );
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
		   LED_PA_OPEN();
		   LED_PC_OPEN();
                return;
            }
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->header.len - \
                sizeof(update_packet->data.header);
            rcv_len += cur_packet_len;
		LED_PA_OPEN();
		LED_PC_OPEN();
            update_save_program(&(update_packet->data.data[0]),programmlen,\
                cur_pack_num,\
                cur_packet_len);
		LED_PA_CLOSE();
		LED_PC_CLOSE();
        }

        if(update_packet->data.header.update_flag == 0x03)
        {
            //���������ı�־
            updat_rcv_packet_flag = 0;
            return;
        }
    }
}


/*
void UsbProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u32 total_len = 1024*1024*1;
    static u16 update_lastpack = 0;
    tUsbUpdatePacket *update_packet;
    
    
    update_packet = (tUsbUpdatePacket *)cmd_buf;
   
	  if(update_packet->step == 0x01)
        {
            //��ʼ�����ı�־
            LedAlarmStatSet( LED_ALARM_UPDATE_LOSE_PAKET, LED_ALARM_IDLE );
            update_init();
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
            if(!updat_rcv_packet_flag)
            {
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        else if(update_packet->step == 0x02)
        {
            cur_pack_num = \
                (((u16)(update_packet->packet_num_h)<<8)|\
                ((u16)(update_packet->packet_num_l)));

            if((cur_pack_num - update_lastpack) != 1)
            {
                LedAlarmStatSet( LED_ALARM_IDLE, LED_ALARM_UPDATE_LOSE_PAKET );
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
                return;
            }
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->len-4;
            rcv_len += cur_packet_len;
            update_save_program(&(update_packet->data_area[0]),total_len,\
                cur_pack_num,\
                cur_packet_len);
        }

        else if(update_packet->step == 0x03)
        {
            //���������ı�־
            updat_rcv_packet_flag = 0;
	       update_flash_init();
		update_flash_copy_run(program_rcv_len);
            return;
        }
  
}
*/
#else

/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: �������� ��FLASH�Ĳ���
**  ��������: 2013.12.24
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "core_cm3.h"
#include "progupdate.h"
#include "string.h"
#include "data_public.h"
#include "data_train.h"
#include "hw_config.h"
#include "led_control.h"

#include <stdint.h>

#define ERASEATSTART
#define FLASH_PAGE_LEN 1024

/*
void SoftwareReset(void)
{
	__asm("LDR R0,=0XE000ED0C");
	__asm("LDR R1,=0X05FA0001");
	__asm("STR R1, [R0]");

}
*/

static void UpdateDisableAllNVIC(void)
{
  for(int i=19; i < 59; i++)
  {
    NVIC->ICER[i >> 0x05] =(unsigned int )0x01 << (i & (unsigned char)0x1F);
  }
}



////////////////////////////////////////////////////////////////////////////////
#define UPDATE_DATA_BUFFER_LEN 512
#define UPDATE_FUNC_BUFFER_LEN 2048
#define UPDATE_CLI()  __set_PRIMASK(1)

void map_flash_copy_func(u32 length);

static u8  update_copy_func[UPDATE_FUNC_BUFFER_LEN];          //��ſ�������
static u32 update_func_addr;
static u8  update_flash2flash_buffer[UPDATE_DATA_BUFFER_LEN]; // ���ݻ�����


void update_flash_init(void)
{
	update_func_addr=(u32)(update_copy_func+1);//(u32)&
}

void map_flash_copy_func_new(u32 length);
void update_flash_copy_run(u32 length)
{	
    update_copy_func[0] = 0x2d;
    memcpy((u8 *)(update_copy_func+1),(u8 *)&map_flash_copy_func,\
        UPDATE_FUNC_BUFFER_LEN-1);
    UPDATE_CLI();	
    UpdateDisableAllNVIC();
    //UpdateDisableAllGPIO();
        
    ((void(*)(u32 length0))update_func_addr)(length);//run copy
}



void map_flash_copy_func(u32 length)
{
    u32 i = 0,k = 0;
    u32 addr1 = TEMP_FLASH_ADDR,addr2 = BOOT_FLASH_ADDR; 
    u32 idx = 0;
    u32 *p = (u32*)update_flash2flash_buffer;
	FLASH_Status flashstatus;
	u8 led_flag;
    

	for(k = 0; k < 20; k++)
	{
    		//������ʾ�����ڰ���
		if (0 == led_flag)
		{
			led_flag = 1;
			GPIOB->BSRR = GPIO_Pin_1;
			GPIOB->BSRR = GPIO_Pin_5;
			GPIOA->BSRR = GPIO_Pin_12;
			GPIOA->BSRR = GPIO_Pin_11;
		}
		else 
		{
			led_flag = 0;
			GPIOB->BRR = GPIO_Pin_1;
			GPIOB->BRR = GPIO_Pin_5;
			GPIOA->BRR = GPIO_Pin_12;
			GPIOA->BRR = GPIO_Pin_11;
		}	
		// flash unlock
		FLASH->KEYR =  ((u32)0x45670123);
		FLASH->KEYR = ((u32)0xCDEF89AB);

		FLASH->SR = FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR;		
		FLASH->CR|= ((u32)0x00000002);
		FLASH->AR = ((u32)(0x8000000 + (k * 0x400))); 
		FLASH->CR|= ((u32)0x00000040);
		IWDG->KR = ((u16)0xAAAA);

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
		
		FLASH->CR &= ((u32)0x00001FFD);
	}

  
    while(length)
    {
   		//������ʾ�����ڰ���
    		led_flag ++;
		if (led_flag < 2)
		{
			GPIOB->BSRR = GPIO_Pin_1;
			GPIOB->BSRR = GPIO_Pin_5;
			GPIOA->BSRR = GPIO_Pin_12;
			GPIOA->BSRR = GPIO_Pin_11;
		}
		else if (led_flag < 10)
		{
			led_flag = 0;
			GPIOB->BRR = GPIO_Pin_1;
			GPIOB->BRR = GPIO_Pin_5;
			GPIOA->BRR = GPIO_Pin_12;
			GPIOA->BRR = GPIO_Pin_11;
		}
	
        FLASH->KEYR =  ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        for(i=0;i<sizeof(update_flash2flash_buffer);i++)
        {
            update_flash2flash_buffer[i]=(*((volatile u8*)(addr1+i)));	
        }

        for (idx = 0; idx <  128; idx = idx + 1)
        {
				//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait

		
		FLASH->CR |= ((u32)0x00000001);
		*(__IO u16*)addr2 = (u16)p[idx];
 
		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
    

		*(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
     

            addr2 += 4;
		FLASH->CR &= ((u32)0x00001FFE);
        }
        addr1+=512;
        length-=512;
    }

	GPIOB->BSRR = GPIO_Pin_1;
	GPIOB->BSRR = GPIO_Pin_5;
	GPIOA->BSRR = GPIO_Pin_12;
	GPIOA->BSRR = GPIO_Pin_11;

	IWDG->KR = 0xCCCC;

	//NVIC_SystemReset();
	//SoftwareReset();
    while(1)
    {
    }
}

/*******************************************************************************
*��������:�����������
*����˵��:program_data: ָ�����ݵ�ָ�룬program_total_len:��������
                                             ���ܳ���
                        program_cur_num:��ǰ�������ݰ��ı��
                        cur_len:��ǰ���ݰ��ĳ���.
*ע������:���ݰ��ı�Ŵ�1��ʼ��������Ű�1������ÿ��
                        �ĳ��ȱ���Ϊ4�ı���,�����ֽ�Ϊ��λ��С�ˡ�
                        ÿ���������ΪFLASHҳ���ȷ���˳������1��ʼ
                        һֱ������һ��,�������˳�򷢰���������
                        ���װһ�η���һ�����ݵ�֮ǰ����һ��
                        update_init()���������ã����ϴγ���ʧ�ܵ�����£�
                        �ô�������ʧ�ܡ�
*******************************************************************************/

static u32 program_rcv_len = 0;
static u16 last_pack_num = 0;

u8 update_init()
{
    program_rcv_len   = 0;
    last_pack_num     = 0;
    return update_ok;
}


void FLASH_If_Init(void)
{
   FLASH_Unlock();
   FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}


void FLASH_If_Erase_Prog_Bak(void)
{
	u32 i;

	for(i = PROG_BAK_ADDRESS; i <= (PROG_BAK_ADDRESS+20*1024); i += FLASH_PAGE_LEN )
	{
   		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(i);		
	}

}
void update_flash_erase(void)
{

    FLASH_If_Init();
    FLASH_If_Erase_Prog_Bak();
	
}
//void exec_iap_fun();
u8 update_save_program(u8 *program_data,u32 program_total_len,
    u16 program_cur_num,u32 cur_len)
{
    u32 addr2,idx;
    u32 *p = (u32*)program_data;

    if(NULL == program_data||!program_total_len||!program_cur_num||!cur_len)
    {
        update_init();
        return update_param;
    }

    if(program_cur_num == 1)
    {
        update_init();
    }
    
    if(program_cur_num - last_pack_num != 1)
    {
        update_init();
        return update_uncontinuous;
    }

    last_pack_num = program_cur_num;

#ifndef ERASEATSTART
    if(FLASH_PAGE_LEN - packet_use_len < cur_len||!flash_first_erase)
    {
        FLASH->KEYR = ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        if(FLASH_ErasePage(TEMP_FLASH_ADDR+(pagecounter*FLASH_PAGE_LEN)) != \
            FLASH_COMPLETE)
        {
            update_init();
            return update_erase_flash;
        }
        pagecounter++;
        
        if(!flash_first_erase)
        {
            flash_first_erase = 1;
            packet_use_len += cur_len;
        }
        else
        {
            packet_use_len = (packet_use_len+cur_len) % FLASH_PAGE_LEN;
        }
    }
    else
    {
        packet_use_len += cur_len;
    }
#endif

    FLASH->KEYR =  ((u32)0x45670123);
    FLASH->KEYR = ((u32)0xCDEF89AB);
    addr2 = TEMP_FLASH_ADDR + program_rcv_len;

#if 0
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        } 
        FLASH->CR |= ((u32)0x00000001);
        *(__IO u16*)addr2 = (u16)p[idx];
        
        /* Wait for last operation to be completed */
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }
        *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }  
        /* Disable the PG Bit */
        addr2 += 4;
        FLASH->CR &= ((u32)0x00001FFE);
    }
#else
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        //can be better 
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR |= ((u32)0x00000001);
            *(__IO u16*)addr2 = (u16)p[idx];
            break;
        }
        
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
            break;
        }
        
        addr2 += 4;
        while(FLASH_BUSY != FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR &= ((u32)0x00001FFE);
            break;
        }
        
    }
#endif
    program_rcv_len += cur_len;

    if(program_rcv_len >= program_total_len)
    {
//#ifdef UPDATEUSERAM
        update_flash_init();
		update_flash_copy_run(program_rcv_len);
//#else
/*
        u16 tmp;
        for(tmp = 0; tmp < 150; tmp++);
        __set_FAULTMASK(1);
		exec_iap_fun();
*/		
//#endif
    }

    return update_ok;
}


typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;



static u32 rcv_len               = 0;
static u32 programmlen           = 0;
static u8  updat_rcv_packet_flag = 0;
static u16 cur_pack_num          = 0;

void ProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u16 update_lastpack = 0;
    tUpdateFrame *update_packet;
    //u32 vector_num;
    
    update_packet = (tUpdateFrame *)cmd_buf;
    if((update_packet->data.header.update_type== 1)||\
        (update_packet->data.header.update_type == 0x7d)) //���³���
	{
	    if(update_packet->data.header.update_flag == 0x01)
        {
            //��ʼ�����ı�־
            update_init();
       	//LedAlarmStatSet( LED_ALARM_UPDATE_LOSE_PAKET, LED_ALARM_IDLE );
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
            if(!updat_rcv_packet_flag)
            {
                programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

                //vector_num = (programmlen%FLASH_PAGE_LEN)?(1+((programmlen/FLASH_PAGE_LEN))):(programmlen/FLASH_PAGE_LEN);
                
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        if(updat_rcv_packet_flag)
        {
            programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

            cur_pack_num = \
                (((u16)(update_packet->data.header.high_byte_cur_packet_num)<<8)|\
                ((u16)(update_packet->data.header.low_byte_cur_packet_num)));

            if((cur_pack_num - update_lastpack) != 1)
            {
            	   //LedAlarmStatSet( LED_ALARM_IDLE, LED_ALARM_UPDATE_LOSE_PAKET );
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
		   LED_PA_OPEN();
		   LED_PC_OPEN();
                return;
            }
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->header.len - \
                sizeof(update_packet->data.header);
            rcv_len += cur_packet_len;
		LED_PA_OPEN();
		LED_PC_OPEN();
            update_save_program(&(update_packet->data.data[0]),programmlen,\
                cur_pack_num,\
                cur_packet_len);
		LED_PA_CLOSE();
		LED_PC_CLOSE();
        }

        if(update_packet->data.header.update_flag == 0x03)
        {
            //���������ı�־
            updat_rcv_packet_flag = 0;
            return;
        }
    }
}


/*
void UsbProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u32 total_len = 1024*1024*1;
    static u16 update_lastpack = 0;
    tUsbUpdatePacket *update_packet;
    
    
    update_packet = (tUsbUpdatePacket *)cmd_buf;
   
	  if(update_packet->step == 0x01)
        {
            //��ʼ�����ı�־
            LedAlarmStatSet( LED_ALARM_UPDATE_LOSE_PAKET, LED_ALARM_IDLE );
            update_init();
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
            if(!updat_rcv_packet_flag)
            {
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        else if(update_packet->step == 0x02)
        {
            cur_pack_num = \
                (((u16)(update_packet->packet_num_h)<<8)|\
                ((u16)(update_packet->packet_num_l)));

            if((cur_pack_num - update_lastpack) != 1)
            {
                LedAlarmStatSet( LED_ALARM_IDLE, LED_ALARM_UPDATE_LOSE_PAKET );
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
                return;
            }
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->len-4;
            rcv_len += cur_packet_len;
            update_save_program(&(update_packet->data_area[0]),total_len,\
                cur_pack_num,\
                cur_packet_len);
        }

        else if(update_packet->step == 0x03)
        {
            //���������ı�־
            updat_rcv_packet_flag = 0;
	       update_flash_init();
		update_flash_copy_run(program_rcv_len);
            return;
        }
  
}
*/
#endif



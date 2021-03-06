#include "include.h"

#include "at45db161.h"

uint8_t gDownFirmware = 0;


/*
buf: 程序数据包，从总长度开始的数据
lenth: 本包数据应用层总长度，这里需要减去多余的长度
*/
void UpdateFirmware(u8 *data_buf, u16 lenth)
{
    static uint32_t total_size = 0;
    static uint32_t recv_size = 0;
    uint16_t index;
    uint8_t buf[32] = {0};
    static u32 flash_write_address;
    static uint16_t packet_old = 0;
    uint16_t temp;
    RCC_ClocksTypeDef RCC_Clocks;
    uint32_t i;
    static u8 try_num_erro;
    gUpdateBmpSt.tick = gSystemTick;
    
    if (data_buf[0] == 0x02) //uart 升级数据
    {
        //gDownFirmware = 1;
        /******************/
        if (gDispStat.disp_type == DISP_TYPE_ERROR_DOWN)
            return;
            
        /*******************/
        DispTypeInit(DISP_TYPE_DOWNING);
        temp = data_buf[6];
        temp <<= 8;
        temp |= data_buf[5];
        
        if (packet_old < temp)
        {
            try_num_erro  += 1;
            
            if (try_num_erro  == 2)
            {
                try_num_erro = 0;
                DispTypeInit(DISP_TYPE_ERROR_DOWN);
                return;
            }
            
            return;
        }
        else if (packet_old > temp)
        {
            return;
        }
        
        try_num_erro = 0;
        
        if (lenth > 255)
        {
            DispTypeInit(DISP_TYPE_ERROR);
            return;
        }
        
        if ((lenth - 8) % 4 != 0)
        {
            DispTypeInit(DISP_TYPE_ERROR);
            return;
        }
        
        packet_old = temp + 1;
        recv_size = recv_size + lenth - 8;
        //SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->CTRL  &= ~SysTick_CTRL_CLKSOURCE_Msk |
                          ~SysTick_CTRL_TICKINT_Msk   |
                          ~SysTick_CTRL_ENABLE_Msk;
        //SysTick_ITConfig(DISABLE);
        Delay(2);
        FLASH_If_Write(&flash_write_address, (u32 *) &data_buf[7], (u16) (lenth - 8) / 4);
        RCC_GetClocksFreq(&RCC_Clocks);
        SysTick_Config(RCC_Clocks.HCLK_Frequency / SYS_TICK_TIME);
        Delay(5);
        index = data_buf[6];
        index = index << 8 | data_buf[5];
        //      snprintf(buf, sizeof(buf), "w %d ok\r\n\0", index);
        //      usb_write_buf(buf, strlen(buf));
    }
    else if (data_buf[0] == 0x01) //uart 升级开始命令
    {
        gDownFirmware = 1;
        write_version_flag(0x55);
        DispTypeInit(DISP_TYPE_DOWNING);
        packet_old = 1;
        total_size = 0;
        recv_size = 0;
        total_size = data_buf[4];
        total_size = total_size << 8 | data_buf[3];
        total_size = total_size << 8 | data_buf[2];
        total_size = total_size << 8 | data_buf[1];
        //SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;
        //SysTick->CTRL  &= ~SysTick_CTRL_CLKSOURCE_Msk;
        FLASH_If_Init();
        // erase user flash area
        FLASH_If_Erase_Prog_Bak(PROG_BAK_ADDRESS);
        //SysTick_Config(RCC_Clocks.HCLK_Frequency / SYS_TICK_TIME);
        flash_write_address = PROG_BAK_ADDRESS;
        //      usb_write_buf("Begin to upgrade\r\n", strlen("Begin to upgrade\r\n"));
    }
    else if (data_buf[0] == 0x03) //uart 升级结束命令
    {
        //      print_int("%d", total_size);
        //      print_int("%d", recv_size);
        if (total_size == recv_size)
        {
            gOldTerminal = 0xff;
            //usb_write_buf("End upgrade\r\n", strlen("End upgrade\r\n"));
            USB_OTG_BSP_DisableInterrupt();
            /*
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
            USART_Cmd(USART1, DISABLE);
            SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
            SysTick_CTRL_TICKINT_Msk   |
            SysTick_CTRL_ENABLE_Msk;
            */
            __set_FAULTMASK(1);
            exec_iap_fun();
        }
        else
        {
            usb_write_buf("upgrade error\r\n", strlen("upgrade error\r\n"));
            DispTypeInit(DISP_TYPE_ERROR);
        }
    }
}



/*
buf: 图片数据包，从升级类型字段开始的数据
lenth: 本包数据应用层总长度，
*/
void UpdateFunc(u8 *buf, u16 lenth)
{
    if (buf[0] == 1)
    {
        //print_line("down code");
        gUpdateBmpSt.tick = gSystemTick;
        UpdateFirmware(&buf[1], lenth);
    }
    else if (buf[0] == 2)
    {
        UpdateBmp(&buf[1], lenth);
    }
    else if (buf[0] == 3)
    {
        UpdateConfig(&buf[1], lenth);
    }
    else if (buf[0] == 4)
    {
        UpdateStationConfig(&buf[1], lenth);
    }
    //复位信号
    else if (buf[0] == 5)
    {
        while (1);
    }
}





//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//*文件名称:AT45DB161.c

//*文件作用:对外扩串行存储器at45db161的操作

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include "at45db161.h"
#include "include.h"



u8 gLedImageBuf[16][(LED_IMAGE_MAX_WIDTH / 8) + 36];
//u8 gLedImageBuf[16][(LED_IMAGE_MAX_WIDTH/8) + ];

u16 gBmpWidth;
tLedColor gLedColor;
tBmpUpdateFlag gBmpUpdateFlag;
tUpdateBmpSt gUpdateBmpSt;


static u8 gxor;
static u16 gsum;


extern u8 run_mode ; //全自动模式还是半自动模式
void down_print_line(char *str)
{
    return;
    //  print_line(str);
}

void down_print_int(char *args, uint32 n)
{
    return;
    //print_int(args, n);
}


void SpiDelay(void)
{
    char tick;
    
    for (tick = 0; tick < 2; tick++);
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//              写入一个字节             //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
void write_byte(uint8_t wByte)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        if ((wByte << i) & 0x80)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_12);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_12);
        }
        
        SpiDelay();
        CLR_SPI_CLK();
        SpiDelay();
        SET_SPI_CLK();
        SpiDelay();
    }
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//              读1个字节                //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
uint8_t read_byte(void)
{
    uint8_t i, rByte = 0;
    
    for (i = 0; i < 8; i++)
    {
        CLR_SPI_CLK();
        SpiDelay();
        SET_SPI_CLK();
        SpiDelay();
        rByte <<= 1;
        
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET)
        {
            rByte++;
        }
        
        SpiDelay();
    }
    
    return rByte;
}




//检查状态寄存器最高位是否为忙，并等待空闲
void wait_busy(void)
{
    u32 tick = 0;
    uint8_t state_reg = 0x00;
    CLR_SPI_CS();
    write_byte(READ_STATE_REGISTER);
    state_reg = read_byte();
    
    while ((state_reg & 0x80) == 0)
    {
        tick ++;
        
        if (tick > 0xffffff) break;
        
        state_reg = read_byte();
    }
    
    SET_SPI_CS();
}


/*
page:开始页地址
lenth:数据总长度，包含前3字节的宽度和颜色
buf:数据
*/
void write_bmp(u16 page0, u32 lenth0, u8 *buf)
{
    u16 len;
    u16 i;
    u32 leng;
    u16 page;
    u8 *data;
    page = page0;
    leng = lenth0;
    data = buf;
    wait_busy();    //等待，flsah直到空闲
    
    while (leng)
    {
        CLR_SPI_CS();
        write_byte(BUFFER_1_WRITE); //选择缓冲区
        write_byte(0x00);
        write_byte(0x00);
        write_byte(0x00);
        len = min(leng, PAGE_LEN);
        
        for (i = 0; i < len; i++)
        {
            write_byte(*data);
            data++;
        }
        
        SET_SPI_CS();     //启动芯片写数据动作
        
        if (page < 4096)
        {
            CLR_SPI_CS();
            write_byte(B1_TO_MM_PAGE_PROG_WITH_ERASE);
            write_byte((u8)(page >> 6));
            write_byte((u8)(page << 2));
            write_byte(0x00);
            SET_SPI_CS();     //启动芯片写数据动作
            wait_busy();
            leng -= len;
        }
        else
        {
            return;
        }
        
        page += 1;  //换页
    }
}

/*
page0:图片数据开始地址
return: 0 成功       1不成功

*/
u8 read_bmp(u16 page0)
{
    u16 page;
    u16 len;
    u16 i;
    u8 temp;
    u8 h;
    u16 sum = 0;
    u8 xor = 0;
    page = page0;
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(page >> 6));
    write_byte((u8)(page << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    temp = read_byte();     //取出点阵长度
    len = read_byte();
    len = len << 8;
    len |= temp;
    gBmpWidth = len;
    
    if (len % 8 == 0) len /= 8;
    else
    {
        len /= 8;
        len += 1;
    }
    
    if (len > LED_IMAGE_MAX_WIDTH / 8)
    {
        //return 1;
        len = LED_IMAGE_MAX_WIDTH / 8;  //限定最大缓冲区长度，防止溢出
    }
    
    temp = read_byte();
    gLedColor = (tLedColor)temp;
    
    for (h = 0; h < 16; h ++)
    {
        for (i = 0; i < len; i++)
        {
            gLedImageBuf[h][i] = read_byte();
            xor ^= gLedImageBuf[h][i];
            sum += gLedImageBuf[h][i];
        }
    }
    
    for (h = 0; h < 16; h ++)
    {
        for (i = len; i < LED_PHYSICS_WIDTH; i++)
        {
            gLedImageBuf[h][i] = 0;
        }
    }
    
    //校验才会用到
    gxor = read_byte();
    temp = read_byte();
    gsum = read_byte();
    gsum <<= 8;
    gsum += temp;
    SET_SPI_CS();
    
    if ((xor == gxor) && (sum == gsum)) return 0;
    else return 1;
}


void page_erase(u16 page)
{
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(PAGE_ERASE); //选择缓冲区
    write_byte((u8)(page >> 6));
    write_byte((u8)(page << 2));
    write_byte(0x00);
    SET_SPI_CS();
}

void write_ok_flag(void)
{
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(BUFFER_1_WRITE); //选择缓冲区
    write_byte(0x00);
    write_byte(0x00);
    write_byte(0x00);
    write_byte((u8)BMP_OK);
    SET_SPI_CS();     //启动芯片写数据动作
    SpiDelay();
    CLR_SPI_CS();
    write_byte(B1_TO_MM_PAGE_PROG_WITH_ERASE);
    write_byte((u8)(UPDATE_STATUS_ADDR >> 6));
    write_byte((u8)(UPDATE_STATUS_ADDR << 2));
    write_byte(0x00);
    SET_SPI_CS();     //启动芯片写数据动作
    wait_busy();
}


/*
  flag: 0   普通
          0x55  刚更新程序标志
*/
void write_version_flag(u8 flag)
{
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(BUFFER_1_WRITE); //选择缓冲区
    write_byte(0x00);
    write_byte(0x00);
    write_byte(0x00);
    write_byte((u8)flag);
    SET_SPI_CS();     //启动芯片写数据动作
    SpiDelay();
    CLR_SPI_CS();
    write_byte(B1_TO_MM_PAGE_PROG_WITH_ERASE);
    write_byte((u8)(VERSION_FLAG_ADDR >> 6));
    write_byte((u8)(VERSION_FLAG_ADDR << 2));
    write_byte(0x00);
    SET_SPI_CS();     //启动芯片写数据动作
    wait_busy();
}


/*
return: 0    不需要显示版本号
           1   显示版本号
*/
u8 get_version_flag(void)
{
    u8 value;
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(VERSION_FLAG_ADDR >> 6));
    write_byte((u8)(VERSION_FLAG_ADDR << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    value = read_byte();        //取出点阵长度
    SET_SPI_CS();
    
    if ((value == 0)) return 0;
    else return 1;
}

/*
传入图片属性，获得该图片起始页
返回起始页
*/
u16 GetBmpStartPage(tBmpParam param)
{
    u16 page;
    
    //page = SpiSpageAddrBuf[param.type];
    switch (param.type)
    {
        case BMP_TYPE_LINE1:
            page = LINE1_BASE_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
            
        case BMP_TYPE_LINE2:
            page = LINE2_BASE_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
            
        case BMP_TYPE_LINE3:
            page = LINE3_BASE_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
            
        case BMP_TYPE_MSG:
            page = MSG_BASE_ADDR + param.index * 8;
            
            if (param.langusge == LANG_CHINESE) page += 4;
            
            break;
            
        case BMP_TYPE_AD:
            page = AD_BASE_ADDR + param.index * 32;
            
            if (param.langusge == LANG_CHINESE) page += 16;
            
            break;
            
        case BMP_TYPE_EME:
            page = EME_BASE_ADDR + param.index * 32;
            
            if (param.langusge == LANG_CHINESE) page += 16;
            
            break;
            
        case CONFIG_LINE1:
            page = LINE1_CONFIG_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
            
        case CONFIG_LINE2:
            page = LINE2_CONFIG_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
            
        case CONFIG_LINE3:
            page = LINE3_CONFIG_ADDR + param.index * 2;
            
            if (param.langusge == LANG_CHINESE) page += 1;
            
            break;
    }
    
    return page;
}


/*
    fun: 先把所有配置信息全部取出
    index: 下标从0开始
    得到指定站的配置信息

    return : 0 成功    1失败
*/
u8 GetStationConfig1(u8 *buf, u8 index, u8 line_no)
{
    u16 page;
    tBmpParam param;
    u8 temp;
    u8 j;
    u8 flag = 0;
    param.type = CONFIG_LINE1 + line_no;
    param.index = index;
    param.langusge = LANG_ENGLISH;                //语言是混淆在一起 组成一张图片的
    page = GetBmpStartPage(param);
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(page >> 6));
    write_byte((u8)(page << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    temp = 0;
    j = 0;
    
    while (temp != 0xff)
    {
        temp = read_byte();
        buf[j++] = temp;
        
        if (j > CONFIG_TOTAL_LEN)
        {
            flag = 1;
            break;
        }
    }
    
    SET_SPI_CS();
    
    if (flag == 0) return 0;
    else
    {
        return 1;
    }
}




/*
    index: 下标从0开始
    得到指定站的配置信息

    return : 0 成功    1失败
*/
u8 GetStationConfig(u8 *buf, tBmpType line_no, u8 index, tLanguage lang, tConfigType type)
{
    u16 page;
    tBmpParam param;
    u8 temp;
    u8 i, j;
    u8 flag = 0;
    u8 arrive_flag;
    param.type = line_no;
    param.index = index;
    param.langusge = lang;
    page = GetBmpStartPage(param);
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(page >> 6));
    write_byte((u8)(page << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    temp = 0;
    i = 0;
    j = 0;
    arrive_flag = 0;
    
    while (temp != 0xff)
    {
        temp = read_byte();
        
        if (type == CONFIG_TYPE_ALL)
        {
            buf[j] = temp;
            j ++;
            
            if ((j == 3) && (buf[2] != 0xfd))
            {
                flag = 1;
                break;
            }
        }
        else if (type == CONFIG_TYPE_PRE)
        {
            if (j >= 2)
            {
                buf[i] = temp;
                i ++;
                
                if (temp == 0xfc)
                {
                    buf[i - 1] = 0xff;
                    break;
                }
                
                if (buf[0] != 0xfd)
                {
                    flag = 1;
                    break;
                }
            }
            
            j ++;
        }
        else if (type == CONFIG_TYPE_ARRIVE)
        {
            if (j >= 2)
            {
                if (arrive_flag == 1)
                {
                    buf[i] = temp;
                    i ++;
                    
                    if (buf[0] != 0xfd)
                    {
                        flag = 1;
                        break;
                    }
                }
                else
                {
                    if (temp == 0xfc) arrive_flag = 1;
                }
            }
            
            j ++;
        }
        else if (type == CONFIG_TYPE_PRE_COLOR)
        {
            buf[0] = temp;
            break;
        }
        else if (type == CONFIG_TYPE_ARRIVE_COLOR)
        {
            if (j >= 1)
            {
                buf[0] = temp;
                break;
            }
            
            j ++;
        }
        
        if (j > CONFIG_TOTAL_LEN)
        {
            flag = 1;
            break;
        }
    }
    
    SET_SPI_CS();
    
    if (flag == 0) return 0;
    else
    {
        down_print_line("config error :");
        down_print_int("%d", line_no);
        down_print_int("%d", index);
        DispTypeInit(DISP_TYPE_ERROR);
        return 1;
    }
}


/*
return: 0 ok ;    1 fail
*/
u8 CheckBmpData(u16 page)
{
    if (0 == read_bmp(page)) return 0;
    else return 1;
    
#if 0
    u16 sum;
    u8 xor;
    u16 h;
    u16 len;
    u16 i;
    sum = 0;
    xor = 0;
    read_bmp(page);
    /*
        xor ^= (gBmpWidth & 0x00ff);
        xor ^= (gBmpWidth >> 8);
        xor ^= gLedColor;
        sum += (gBmpWidth & 0x00ff);
        sum += (gBmpWidth >> 8);
        sum += gLedColor;
    */
    len = gBmpWidth;
    
    if (len % 8 == 0) len /= 8;
    else
    {
        len /= 8;
        len += 1;
    }
    
    print_int("%d", len);
    
    if (len > 512)
    {
        len = 512;  //限定最大缓冲区长度，防止溢出
    }
    
    for (h = 0; h < 16; h ++)
    {
        for (i = 0; i < len; i++)
        {
            xor ^= gLedImageBuf[h][i];
            sum += gLedImageBuf[h][i];
        }
    }
    
    if ((xor == gxor) && (sum == gsum)) return 0;
    else return 1;
    
#endif
}


/*
buf: 图片数据包，从标志位开始的数据
lenth: 本包数据应用层总长度，这里需要减去多余的长度
*/
void UpdateBmp(u8 *buf, u16 lenth)  //收到的BUF中包含7个字节的图片信息
{
    tBmpHeader *header;
    u8 head_len;
    u16 len;
    tBmpParam param;
    u16 page;
    u8 ret;
    u32 tick;
    head_len = sizeof(tBmpHeader);
    header = (tBmpHeader *)buf;
    gUpdateBmpSt.tick = gSystemTick;
    
    //如果是开始升级，需要先将升级状态标志擦除
    if (header->flag_bit.total_start)
    {
        DispTypeInit(DISP_TYPE_DOWNING);
        down_print_line("total start");
        page_erase(UPDATE_STATUS_ADDR);
        gUpdateBmpSt.packet_old = 0;
        gUpdateBmpSt.success_flag = BMP_OK;
        gBmpUpdateFlag = BMP_FAIL;
        gUpdateBmpSt.pt = (u8 *)gLedImageBuf;
    }
    
    if (header->flag_bit.bmp_start)
    {
        down_print_line("bmp start");
        down_print_int("%d", header->type);
        down_print_int("%d", header->index);
        gUpdateBmpSt.packet_old = 0;
        gUpdateBmpSt.pt = (u8 *)gLedImageBuf;
        gUpdateBmpSt.pt[0] = (header->width);
        gUpdateBmpSt.pt[1] = (header->width) >> 8;
        gUpdateBmpSt.pt[2] = header->color;
        gUpdateBmpSt.offset = 3;
    }
    
    //判断上1包包数是否与本包连续。
    if ((gUpdateBmpSt.packet_old + 1) != header->packet_current)
    {
        gUpdateBmpSt.success_flag = BMP_FAIL;
        down_print_line("packet error");
        DispTypeInit(DISP_TYPE_ERROR);
    }
    else
    {
        gUpdateBmpSt.packet_old = header->packet_current;
        down_print_int("%d", header->packet_current);
    }
    
    len = lenth - head_len - 1;
    memcpy(&gUpdateBmpSt.pt[gUpdateBmpSt.offset], &buf[head_len], len);
    gUpdateBmpSt.offset += len;
    //if (gUpdateBmpSt.offset >= (LED_WIDTH/8)*LED_HEIGHT) gUpdateBmpSt.offset -= len;
    
    if (header->flag_bit.bmp_end)
    {
        down_print_line("bmp end");
        
        if (gUpdateBmpSt.success_flag == BMP_FAIL) return;
        
        param.type = header->type;
        param.index = header->index - 1;
        
        if (header->flag_bit.language)
        {
            param.langusge = LANG_CHINESE;
        }
        else
        {
            param.langusge = LANG_ENGLISH;
        }
        
        page = GetBmpStartPage(param);
        write_bmp(page, gUpdateBmpSt.offset, (u8 *)gLedImageBuf);  //
        ret = CheckBmpData(page);
        
        if (ret)
        {
            gUpdateBmpSt.success_flag = BMP_FAIL;
            down_print_line("data check fail");
            DispTypeInit(DISP_TYPE_ERROR);
        }
        else
        {
            //gOldTerminal = 0xff;
        }
    }
    
    //结束升级
    if (header->flag_bit.total_end)
    {
        if ((gUpdateBmpSt.success_flag == BMP_OK))
        {
            down_print_line("total end ok");
            write_ok_flag();
            //for (tick=0; tick<0xfffff; tick++);
            GetBmpUpdateFlag();
            DispTypeInit(DISP_TYPE_NO_PROCESS);
        }
        else
        {
            down_print_line("total end fail");
            DispTypeInit(DISP_TYPE_ERROR);
        }
    }
}

void GetBmpUpdateFlag(void)
{
    u8 value;
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(UPDATE_STATUS_ADDR >> 6));
    write_byte((u8)(UPDATE_STATUS_ADDR << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    value = read_byte();        //取出点阵长度
    SET_SPI_CS();
    
    if ((value != BMP_OK))
    {
        gBmpUpdateFlag = BMP_FAIL;
        down_print_line("get update flag error");
        DispTypeInit(DISP_TYPE_ERROR);
    }
    else
    {
        gBmpUpdateFlag = (tBmpUpdateFlag)value;
        //gDispStat.disp_type = DISP_TYPE_NORMAL;
    }
}

/*
index: 下标从0开始
return: 0成功       1失败
*/
u8 GetBmp(tBmpType type, u16 index, tLanguage language)
{
    u16 page;
    tBmpParam param;
    u8 h;
    u16 i, j;
    u16 len;
    u16 offset;
    DispTypeInit(DISP_TYPE_DARK);
    gDispStat.curr_lang = language;
    gDispStat.curr_line = type;
    gDispStat.curr_station = index;
    param.type = type;
    param.index = index;
    param.langusge = language;
    page = GetBmpStartPage(param);
    
    if (1 == read_bmp(page))     // 2个字节的长度 ，1个字节的颜色
    {
        gBmpUpdateFlag = BMP_FAIL;
        down_print_line("read bmp error");
        DispTypeInit(DISP_TYPE_ERROR);
        return 1;
    }
    
#if 1
    
    if (gBmpWidth > LED_PHYSICS_WIDTH * 8)
    {
        if (gBmpWidth % 8 == 0) len = gBmpWidth / 8;
        else
        {
            len = gBmpWidth / 8;
            len += 1;
        }
        
        for (h = 0; h < 16; h ++)
        {
            offset = len + LED_PHYSICS_WIDTH - 1;
            
            for (i = 0, j = (offset); i < len; i++, j--)
            {
                gLedImageBuf[h][j] = gLedImageBuf[h][j - LED_PHYSICS_WIDTH];
            }
        }
        
        //清除最开始的
        for (h = 0; h < 16; h ++)
        {
            for (i = 0; i < LED_PHYSICS_WIDTH; i++)
            {
                gLedImageBuf[h][i] = 0;
            }
        }
        
        //清除最末尾的
        for (h = 0; h < 16; h ++)
        {
            offset = len + LED_PHYSICS_WIDTH;
            
            for (i = offset; i < offset + LED_PHYSICS_WIDTH; i++)
            {
                gLedImageBuf[h][i] = 0;
            }
        }
        
        gBmpWidth += LED_PHYSICS_WIDTH * 8 + SHIFT_LEFT_NULL_WIDTH;
        gDispStat.shift_type = (language == LANG_ENGLISH) ? (SHIFT_LEFT) : (SHIFT_LEFT);
    }
    else
    {
        gDispStat.shift_type = SHIFT_NONE;
    }
    
#endif
    DispTypeInit(DISP_TYPE_NORMAL);
    return 0;
}


/*
buf:  存放页首地址，已0xffff 结束
return: 0成功       1失败
*/
u8 ConnectBmp(u16 *buf)
{
    u8 k;
    u16 offset;
    u16 page;
    u16 len;
    u16 i;
    u8 temp;
    u8 h;
    u16 sum = 0;
    u8 xor = 0;
    DispTypeInit(DISP_TYPE_DARK);
    offset = LED_PHYSICS_WIDTH;
    k = 0;
    gBmpWidth = 0;
    
    for (h = 0; h < 16; h ++)
    {
        for (i = 0; i < LED_PHYSICS_WIDTH; i++)
        {
            gLedImageBuf[h][i] = 0x00;
        }
    }
    
    while (buf[k] != 0xffff)
    {
        page = buf[k];
        
        if (page > VERSION_FLAG_ADDR) return 1;
        
        sum = 0;
        xor = 0;
        k ++;
        wait_busy();    //等待，flsah直到空闲
        CLR_SPI_CS();
        write_byte(CONTINUOUS_ARRAY_READ);
        write_byte((u8)(page >> 6));
        write_byte((u8)(page << 2));
        write_byte(0x00);
        write_byte(0x00);           //写一个dummy字节
        temp = read_byte();     //取出点阵长度
        len = read_byte();
        len = len << 8;
        len |= temp;
        
        if (len % 8 == 0) len /= 8;
        else
        {
            len /= 8;
            len += 1;
        }
        
        if (len > LED_IMAGE_MAX_WIDTH) return 1;
        
        temp = read_byte();
        
        //gLedColor = temp;
        for (h = 0; h < 16; h ++)
        {
            for (i = 0; i < len; i++)
            {
                gLedImageBuf[h][i + offset] = read_byte();
                xor ^= gLedImageBuf[h][i + offset];
                sum += gLedImageBuf[h][i + offset];
            }
        }
        
        offset += len;
        //校验才会用到
        gxor = read_byte();
        temp = read_byte();
        gsum = read_byte();
        gsum <<= 8;
        gsum += temp;
        SET_SPI_CS();
        
        if ((xor != gxor) && (sum != gsum)) return 1;
    }
    
    gBmpWidth = offset * 8;
    
    //清除最末尾的
    for (h = 0; h < 16; h ++)
    {
        for (i = offset; i < offset + LED_PHYSICS_WIDTH; i++)
        {
            gLedImageBuf[h][i] = 0;
        }
    }
    
    gBmpWidth += SHIFT_LEFT_NULL_WIDTH;
    
    if (gBmpWidth > LED_IMAGE_MAX_WIDTH) return 1;
    
    //2012-12-21 addby Jerry
    len = gBmpWidth - SHIFT_LEFT_NULL_WIDTH;
    len = len - (LED_PHYSICS_WIDTH * 8);
    
    if (len <= LED_PHYSICS_WIDTH * 8)
    {
        gBmpWidth = len;
        offset -= LED_PHYSICS_WIDTH;
        
        for (h = 0; h < 16; h ++)
        {
            for (i = 0; i < offset; i++)
            {
                gLedImageBuf[h][i] = gLedImageBuf[h][i + LED_PHYSICS_WIDTH];
            }
        }
    }
    
    return 0;
}








/*
    buf :   传入到站信息的序号 值，下标从0 开始,以0xff 结束
*/
void ConnectMsg(u8 *buf, u8 line_no, tLanguage lang, u8 *station_index, u16 *page_buf)
{
    u8 i;
    u16 temp;
    //u16 page_buf[20];
    tBmpParam param;
    i = 0;
    
    while (buf[i] != 0xff)
    {
        if ((i == station_index[0]) || (i == station_index[1]))
        {
            param.type = (tBmpType)line_no;
        }
        else
        {
            param.type = (tBmpType)BMP_TYPE_MSG;
        }
        
        param.langusge = lang;
        param.index = buf[i];
        page_buf[i] = GetBmpStartPage(param);
        i ++;
    }
    
    page_buf[i] = 0xffff;
    /*如果是波斯语语序需要反转
    if (lang == LANG_CHINESE) {
        for (j=0,k=i-1; j<(i)/2; j++, k--) {
            temp = page_buf[k];
            page_buf[k] = page_buf[j];
            page_buf[j] = temp;
        }
    }
    */
}


/***********************************
确定便宜方向和图片链接
***********************************/
void Do_ConnectBmp(u16 *page_buf)
{
    if (1 == ConnectBmp(page_buf))
    {
        down_print_line("connect bmp error");
        DispTypeInit(DISP_TYPE_ERROR);
    }
    else
    {
        if (gBmpWidth <= (LED_PHYSICS_WIDTH * 8))
        {
            gDispStat.shift_type = SHIFT_NONE;
        }
        else
        {
            //if (lang == LANG_ENGLISH) gDispStat.shift_type = SHIFT_LEFT;
            //else gDispStat.shift_type = SHIFT_RIGHT;
            gDispStat.shift_type = SHIFT_LEFT;
        }
        
#if DISPLAY_KIND
        
        if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
        {
            do_get_station_config_arrive();
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
                gLedColor = gTrigerValue.arrive_chinese_color;
            else
            {
                if (gDispStat.curr_lang == LANG_ENGLISH) gLedColor = gTrigerValue.arrive_english_color;
                else gLedColor = gTrigerValue.arrive_chinese_color;
            }
        }
        else
        {
            do_get_station_config_pre();
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
                gLedColor = gTrigerValue.prev_chinese_color;
            else
            {
                if (gDispStat.curr_lang == LANG_ENGLISH) gLedColor = gTrigerValue.prev_english_color;
                else gLedColor = gTrigerValue.prev_chinese_color;
            }
        }
        
#else
        
        if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
        {
            if (gDispStat.curr_lang == LANG_ENGLISH) gLedColor = gTrigerValue.arrive_english_color;
            else gLedColor = gTrigerValue.arrive_chinese_color;
        }
        else
        {
            do_get_station_config_pre();
        
            if (gDispStat.curr_lang == LANG_ENGLISH) gLedColor = gTrigerValue.prev_english_color;
            else gLedColor = gTrigerValue.prev_chinese_color;
        }
        
#endif
        DispTypeInit(DISP_TYPE_NORMAL);
    }
}


/**********************************************
0xfe:  需要加入下一站
0xfd:  需要加入开门侧
0xfc:  需要加入终点站
0xff:  结束标志
数组下标从 1  开始

***********************************************
TIP:   必须设定成以下格式，因为在软件代码里面已经把显示的内容你顺序定了
1，本站:
2，终点站：
3，下一站：
4，下一站是本次列车的终点站:
5，左边车门
6，右边车门下车
7，两边车门下车
8，欢迎乘坐广州地铁3号线
9，广州地铁欢迎您
10，本次列车开往
11，逗号
12，到了
13,18个8*8模块的空格
14，前方到站:
15 ,即将到达终点站
16 3个 8*8个空格



在custom栏写
17,   可换乘一号线
18，  可换乘二号线
19,   可换乘五号线
20，  可换乘八号线
21，，可换乘APM号线



1，This station is,
2，The destination of this train is
3，The next station is
4，our terminal station
5，left side
6，right side
7，both side
8，Welcome to GuangZhou Metro
9,    Welcome to the 3rd line of GuangZhou
10，The destination of this train is
11，逗号
12、We're now at the terminal station
13,18个8*8模块的空格
14，Approaching station is
15，Approaching terminal station
16 3个 8*8个空格


在custom栏写
17,   可换乘一号线
18，  可换乘二号线
19,   可换乘五号线
20，  可换乘八号线
21，，可换乘APM号线

18，，You can transfer to Line two
19，，You can transfer to Line five
20，，You can transfer to Line eight
21，，You can transfer to APM Line



用伊朗PTU的时候有个怪现象，就是当图片长度不够的时候，它会显示2次，然后再换语言显示
***********************************************
*/
#if DISPLAY_KIND
/*没有门侧灯的时候*/
const u8 pre_e_msg_normal_buf0[] = {13, 10, 0xfc, 11, 3, 0xfe, 11, 8, 0xfd, 0xff}; //本次列车开往终点站XX ，下一站XX
const u8 pre_e_msg_terminal_buf0[] = {13, 4, 0xfc, 11, 8, 0xfd, 0xff};     //下一站是本次列车的终点站xx，请带齐你的物品从哪个门下车
const u8 pre_c_msg_normal_buf0[] = {10, 0xfc, 16, 3, 0xfe, 16, 8, 0xfd, 0xff};
const u8 pre_c_msg_terminal_buf0[] = {4, 0xfc, 16, 8, 0xfd, 0xff};

const u8 arrive_e_msg_normal_buf0[] = {13, 1, 0xfe, 16, 8, 0xfd, 0xff};
const u8 arrive_e_msg_terminal_buf0[] = {13, 12, 0xfe, 16, 8, 0xfd, 0xff};
const u8 arrive_c_msg_normal_buf0[] = {0xfe, 12, 16, 8, 0xfd, 0xff};
const u8 arrive_c_msg_terminal_buf0[] = {0xfe, 12, 16, 8, 0xfd, 0xff}; // XX站马上到了,请带齐你的物品从指定门下车

/*有门侧灯的时候*/
const u8 pre_e_msg_normal_buf[] = {13, 3, 0xfe, 0xff};
const u8 pre_e_msg_terminal_buf[] = { 13, 4, 0xfc, 0xff};        //下一站
const u8 pre_c_msg_normal_buf[] = {3, 0xfe, 16, 0xff};
const u8 pre_c_msg_terminal_buf[] = {4, 0xfc, 16, 0xff};
/////////////////////////////////////
const u8 arrive_e_msg_normal_buf[] = {13, 14, 0xfe, 0xff};
const u8 arrive_e_msg_terminal_buf[] = {13, 15, 0xfc, 0xff};
const u8 arrive_c_msg_normal_buf[] = {14, 0xfe, 16, 0xff}; //即将到达终点站
const u8 arrive_c_msg_terminal_buf[] = {15, 0xfc, 16, 0xff}; // 前方到站，

const u8 close_e_msg_normal_buf[] = {13, 2, 0xfc, 0xff};
const u8 close_e_msg_terminal_buf[] = {13, 2, 0xfc, 0xff};
const u8 close_c_msg_normal_buf[] = {2, 0xfc, 16, 0xff}; //不显示内容
const u8 close_c_msg_terminal_buf[] = {2, 0xfc, 16, 0xff}; // 终点站:XX

const u8 open_e_msg_normal_buf[] = {13, 1, 0xfe, 0xff};
const u8 open_e_msg_start_buf[] = {13, 9, 11, 2, 0xfc, 0xff};
const u8 open_e_msg_terminal_buf[] = {13, 9, 11, 12, 0xfc, 0xff};
const u8 open_c_msg_normal_buf[] = {1, 0xfe, 16, 0xff}; // 本站:xx
const u8 open_c_msg_start_buf[] = {9, 11, 10, 0xfc, 16, 0xff}; //终点站XX 到了
const u8 open_c_msg_terminal_buf[] = {9, 11, 2, 0xfc, 12, 16, 0Xff};

#else
//中文换成信息前面加空格
const u8 pre_e_msg_normal_buf0[] = {10, 0xfc, 11, 3, 0xfe, 11, 8, 0xfd, 0xff}; //本次列车开往终点站XX ，下一站XX
const u8 pre_e_msg_terminal_buf0[] = {4, 0xfc, 11, 8, 0xfd, 0xff};      //下一站是本次列车的终点站xx，请带齐你的物品从哪个门下车
const u8 pre_c_msg_normal_buf0[] = {10, 0xfc, 16, 3, 0xfe, 16, 8, 0xfd, 0xff};
const u8 pre_c_msg_terminal_buf0[] = {4, 0xfc, 16, 8, 0xfd, 0xff};

const u8 arrive_e_msg_normal_buf0[] = {1, 0xfe, 16, 8, 0xfd, 0xff};
const u8 arrive_e_msg_terminal_buf0[] = {12, 0xfe, 16, 8, 0xfd, 0xff};
const u8 arrive_c_msg_normal_buf0[] = {0xfe, 12, 16, 8, 0xfd, 0xff};
const u8 arrive_c_msg_terminal_buf0[] = {0xfe, 12, 16, 8, 0xfd, 0xff}; // XX站马上到了,请带齐你的物品从指定门下车

/*有门侧灯的时候*/
////////////////////////////////////
const u8 pre_e_msg_normal_buf[] = {3, 0xfe, 0xff};
const u8 pre_e_msg_terminal_buf[] = { 3, 0xfc, 11, 4, 0xff};       //下一站
const u8 pre_c_msg_normal_buf[] = {3, 0xfe, 0xff};
const u8 pre_c_msg_terminal_buf[] = {4, 0xfc, 0xff};

const u8 arrive_e_msg_normal_buf[] = {14, 0xfe, 0xff};
const u8 arrive_e_msg_terminal_buf[] = {15, 0xfc, 0xff};
const u8 arrive_c_msg_normal_buf[] = {14, 0xfe, 0xff}; //即将到达终点站
const u8 arrive_c_msg_terminal_buf[] = {15, 0xfc, 0xff}; // 前方到站，

const u8 close_e_msg_normal_buf[] = {2, 0xfc, 0xff};
const u8 close_e_msg_terminal_buf[] = {2, 0xfc, 0xff};
const u8 close_c_msg_normal_buf[] = {2, 0xfc, 0xff};  //不显示内容
const u8 close_c_msg_terminal_buf[] = {2, 0xfc, 0xff}; // 终点站:XX

const u8 open_e_msg_normal_buf[] = {1, 0xfe, 0xff};
const u8 open_e_msg_start_buf[] = {1, 0xfe, 0xff};
const u8 open_e_msg_terminal_buf[] = {1, 0xfe, 0xff};
const u8 open_c_msg_normal_buf[] = {1, 0xfe, 0xff}; // 本站:xx
const u8 open_c_msg_start_buf[] = {1, 0xfe, 0xff}; //终点站XX 到了
const u8 open_c_msg_terminal_buf[] = {1, 0xfe, 0Xff};

#endif

/*
得到某种类型的配置信息以0xff结尾
*/
u8 GetConfigOfType( u8 *buf,  tTrigerType type )
{
    u8 i, j;
    u8 start_flag;
    u8 start_byte;
    u8 temp;
    
    if (type == TRIGER_PRE_BROADCAST) start_byte = 0xfc;
    //else if (type == TRIGER_PRE_BROADCAST) start_byte = 0xf9;
    else if (type == TRIGER_DOOR_OPEN) start_byte = 0xfe;
    else if (type == TRIGER_DOOR_CLOSE) start_byte = 0xfd;
    else if (type == TRIGER_START_STATION) start_byte = 0xfb;
    else if (type == TRIGER_NEXT_TERMINAL) start_byte = 0xfa;
    else return 1;
    
    start_flag = 0;
    i = 3;
    j = 0;
    
    while (buf[i] != 0xff)
    {
        temp = buf[i];
        
        if (temp == start_byte)
        {
            start_flag = 1;
            i ++;
            continue;
        }
        
        if (start_flag == 1)
        {
            if (temp > 0xf0)
            {
                break;
            }
            
            buf[j] = temp;
            j ++;
        }
        
        i ++;
        
        if (i > CONFIG_TOTAL_LEN) return 1;
    }
    
    buf[j] = 0xff;
    return 0;
}



/***************************************
这里就是将显示配置图片连成一张图片，语言也弄在一起! 显示。适合广3的PTU，什么东西都可以在PTU里面配置

***************************************/

void LoadBmpAndWriteToImageBuf(u8 line_no, u8 *buf, u8 station_current, u8 station_next, u8 station_end)
{
    u8 i, j, k;
    u16 page_buf[CONFIG_TOTAL_LEN - 10], temp;
    tBmpParam param;
    i = 0;
    
    while (buf[i] != 0xff)
    {
        if (buf[i] == 0xe0)
        {
            param.type = line_no;
            param.index = station_next;
            param.langusge = LANG_CHINESE;
        }
        else if (buf[i] == 0xe1)
        {
            param.type = line_no;
            param.index = station_current;
            param.langusge = LANG_CHINESE;
        }
        else if (buf[i] == 0xe2)
        {
            param.type = line_no;
            param.index = station_end;
            param.langusge = LANG_CHINESE;
        }
        else if (buf[i] == 0xe3)
        {
            param.type = line_no;
            param.index = station_next;
            param.langusge = LANG_ENGLISH;
        }
        else if (buf[i] == 0xe4)
        {
            param.type = line_no;
            param.index = station_current;
            param.langusge = LANG_ENGLISH;
        }
        else if (buf[i] == 0xe5)
        {
            param.type = line_no;
            param.index = station_end;
            param.langusge = LANG_ENGLISH;
        }
        else
        {
            param.type = line_no;
            param.index = buf[i] - 1;
            param.langusge = LANG_ENGLISH;
        }
        
        page_buf[i] = GetBmpStartPage(param);
        i ++;
    }
    
    page_buf[i] = 0xffff;
    
    if (1 == ConnectBmp(page_buf))
    {
        down_print_line("connect bmp error");
        DispTypeInit(DISP_TYPE_ERROR);
    }
    else
    {
        if (gBmpWidth <= (LED_PHYSICS_WIDTH * 8))
        {
            gDispStat.shift_type = SHIFT_NONE;
        }
        else
        {
            gDispStat.shift_type = SHIFT_LEFT;
        }
        
        DispTypeInit(DISP_TYPE_NORMAL);
    }
}



/*

    next : 下一站，  下标从0 开始
    end :   下标从0 开始
*/
//  开门侧          左   右   两侧
const u8 door_side_buf[] = {5, 6, 7};


/*
    next : 下一站，  下标从0 开始
    end :   下标从0 开始
*/

/*******************************************
这里适合伊朗的PTU，语言分开显示，每一个阶段每一种语言分开显示，伊朗的PTU决定要手工配置显示信息，广告和换乘信息可以再PTU里面配置
这里还需在调整:显示的内容显示2次后再换语言
*******************************************/

void LoadConnect(tLanguage lang, u16 *page_buf)
{
    const u8 *pt;
    u8 i, j;
    u8 buf[CONFIG_TOTAL_LEN + 10] = {0};
    u8 *config_pt;
    u8 station_index[2];
    u8 index;
    static u8 station_num = 0;
    //用于检验预报站所取得的配置信息对应的站是否改变，开门和关门要用到它的配置信息
#if KIND_RUN     //动态地图运行模式，有门侧灯
    
    if (MAUAL == run_mode) //半自动模式
    {
        if ((gTrigerValue.triger_type == TRIGER_PRE_BROADCAST) )
        {
            station_num = gTrigerValue.station_next;
            
            if (lang == LANG_ENGLISH)
            {
                config_pt = gTrigerValue.english_config;
                
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt = pre_e_msg_terminal_buf;
                else pt = pre_e_msg_normal_buf;
            }
            else if (lang == LANG_CHINESE)
            {
                config_pt = gTrigerValue.chinese_config;
                
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt = pre_c_msg_terminal_buf;
                else pt = pre_c_msg_normal_buf;
            }
        }
        else if ((gTrigerValue.triger_type == TRIGER_DOOR_CLOSE ) )
        {
            if (lang == LANG_ENGLISH)
            {
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt = close_e_msg_terminal_buf;
                else pt = close_e_msg_normal_buf;
            }
            else if (lang == LANG_CHINESE)
            {
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt =  close_c_msg_terminal_buf;
                else pt =  close_c_msg_normal_buf;
            }
        }
        else if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
        {}
    }
    else if (AUTO == run_mode) //全自动模式
    {
        if (gTrigerValue.triger_type == TRIGER_PRE_BROADCAST_1)
        {
            station_num = gTrigerValue.station_next;
            
            if (lang == LANG_ENGLISH)
            {
                config_pt = gTrigerValue.english_config;
                
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt = arrive_e_msg_terminal_buf;
                else pt = arrive_e_msg_normal_buf;
            }
            else if (lang == LANG_CHINESE)
            {
                config_pt = gTrigerValue.chinese_config;
                
                if (gTrigerValue.station_next == gTrigerValue.station_end) pt = arrive_c_msg_terminal_buf;
                else pt = arrive_c_msg_normal_buf;
            }
        }
        
        //只有在到站触发的时候才会显示内容，其他时候都是黑屏
    }
    
#else   //没有开门侧，IDU运行模式，靠文字显示开门侧信息
    
    if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
    {
        station_num = gTrigerValue.station_next;
    
        if (lang == LANG_ENGLISH)
        {
            config_pt = gTrigerValue.english_config;
    
            if (gTrigerValue.station_next == gTrigerValue.station_end) pt = arrive_e_msg_terminal_buf0;
            else pt = arrive_e_msg_normal_buf0;
        }
        else if (lang == LANG_CHINESE)
        {
            config_pt = gTrigerValue.chinese_config;
    
            if (gTrigerValue.station_next == gTrigerValue.station_end) pt = arrive_c_msg_terminal_buf0;
            else pt = arrive_c_msg_normal_buf0;
        }
    }
    else if ((gTrigerValue.triger_type == TRIGER_PRE_BROADCAST) )
    {
        station_num = gTrigerValue.station_next;
    
        if (lang == LANG_ENGLISH)
        {
            config_pt = gTrigerValue.english_config;
    
            if (gTrigerValue.station_next == gTrigerValue.station_end) pt = pre_e_msg_terminal_buf0;
            else pt = pre_e_msg_normal_buf0;
        }
        else if (lang == LANG_CHINESE)
        {
            config_pt = gTrigerValue.chinese_config;
    
            if (gTrigerValue.station_next == gTrigerValue.station_end) pt = pre_c_msg_terminal_buf0;
            else pt = pre_c_msg_normal_buf0;
        }
    }
    else
    {
        config_pt = 0;
        pt = 0;
    }
    
#endif
    
    if ((gTrigerValue.triger_type == TRIGER_DOOR_OPEN ))
    {
        if (station_num != gTrigerValue.station_next)
        {
            do_get_station_config_pre();
            /*#if LIGHT_SIDE
                            if(run_mode ==AUTO)
                                do_get_station_config_arrive();
                            else if( run_mode ==MAUAL)
                                do_get_station_config_pre();         //尽量在预报站这一栏填写换乘信息//
            #else
                            do_get_station_config_pre();
            #endif
            */
        }
        
        if (lang == LANG_ENGLISH)
        {
            config_pt = gTrigerValue.english_config;
            
            if (gTrigerValue.station_next == gTrigerValue.station_start)
                pt = open_e_msg_start_buf;
            else if (gTrigerValue.station_next == gTrigerValue.station_end)
                pt = open_e_msg_terminal_buf;
            else
                pt = open_e_msg_normal_buf;
        }
        else if (lang == LANG_CHINESE)
        {
            config_pt = gTrigerValue.chinese_config;
            
            if (gTrigerValue.station_next == gTrigerValue.station_start)
                pt =  open_c_msg_start_buf;
            else if (gTrigerValue.station_next == gTrigerValue.station_end)
                pt = open_c_msg_terminal_buf;
            else
                pt = open_c_msg_normal_buf;
        }
    }
    
    index = 0;
    station_index[0] = 0xff;
    station_index[1] = 0xff;
    i = 0;
    j = 0;
    
    while (pt[i] != 0xff)
    {
        if (pt[i] == 0xfe)
        {
            // 下一站
            buf[j] = gTrigerValue.station_next - 1;
            station_index[index ++] = j;
        }
        else if (pt[i] == 0xfd)
        {
            //开门侧
            if (gTrigerValue.door != DOOR_SIDE_NONE) //change for no door open side by sven at 2013年6月5日 10:35:28
            {
                buf[j] = door_side_buf[gTrigerValue.door] - 1;
            }
            else
            {
                j--;
            }
        }
        else if (pt[i] == 0xfc)
        {
            // 终点站
            buf[j] = gTrigerValue.station_end - 1;
            station_index[index ++] = j;
        }
        else
        {
            buf[j] = pt[i] - 1;
        }
        
        j ++;
        i ++;
        
        if (i > 100)
            break;
    }
    
    if (gTrigerValue.triger_type == TRIGER_DOOR_CLOSE) {}
    else
    {
        i = 1;
        
        while (config_pt[i] != 0xfe)
        {
            buf[j] = config_pt[i] - 1;
            j ++;
            i ++;
            
            if (i > (CONFIG_MSG_LEN + 1))
            {
                down_print_line("config error connect");
                DispTypeInit(DISP_TYPE_ERROR);
                break;
            }
        }
    }
    
    buf[j] = 0xff;
    ConnectMsg(buf, gTrigerValue.line_no, lang, station_index, page_buf);
}


extern uint8_t gDownFirmware;


void UpdateConfig(u8 *buf, u16 lenth)
{
    u8 *pt;
    gDownFirmware = 1;
    gUpdateBmpSt.success_flag = BMP_OK;
    gUpdateBmpSt.tick = gSystemTick;
    DispTypeInit(DISP_TYPE_DOWNING);
    pt = (u8 *)gLedImageBuf;
    pt[0] = buf[0];
    write_bmp(CONFIG_ADDR, 1, pt);
    GetConfig();
    
    if (buf[0] == gSysConfg.disp_type)
    {
        gDownFirmware = 0;
        down_print_line("down config ok");
        DispTypeInit(DISP_TYPE_NO_PROCESS);
    }
    else
    {
        gUpdateBmpSt.success_flag = BMP_FAIL;
        down_print_line("down config fail");
        DispTypeInit(DISP_TYPE_ERROR);
    }
}

void UpdateStationConfig(u8 *buf, u16 lenth)
{
    u8 *pt;
    u16 page;
    tBmpParam param;
    u8 len;
    u8 i, j;
    u8 buf1[30];
    u8 flag;
    gDownFirmware = 1;
    gUpdateBmpSt.success_flag = BMP_OK;
    gUpdateBmpSt.tick = gSystemTick;
    DispTypeInit(DISP_TYPE_DOWNING);
    pt = (u8 *)gLedImageBuf;
    param.type = (buf[0] - 1) + CONFIG_LINE1;
    param.index = buf[1] - 1;
    param.langusge = buf[2];
    page = GetBmpStartPage(param);
    len = 0;
    i = 3;
    flag = 0;
    j = 0;
    
    while (buf[i] != 0xff)
    {
        pt[j] = buf[i];
        j ++;
        len ++;
        i ++;
        
        if (len > CONFIG_TOTAL_LEN)
        {
            flag = 1;
            break;
        }
    }
    
    pt[j] = 0xff;
    len ++;
    
    if (flag == 0)
    {
        write_bmp(page, len, pt);
        GetStationConfig(buf1, param.type, param.index, param.langusge, CONFIG_TYPE_ALL);
        
        for (i = 0, j = 0; j < len; i++, j++)
        {
            if (pt[i] != buf1[j])
            {
                flag = 1;
                break;
            }
        }
    }
    
    down_print_line("down config:");
    down_print_int("%d", param.type);
    down_print_int("%d", param.index);
    
    if ((flag == 0))
    {
        gDownFirmware = 0;
        down_print_line("ok");
        //      DispTypeInit(DISP_TYPE_NO_PROCESS);
    }
    else
    {
        //gDownFirmware = 0;
        gUpdateBmpSt.success_flag = BMP_FAIL;
        down_print_line("fail");
        DispTypeInit(DISP_TYPE_ERROR);
    }
}


void GetConfig(void)
{
    u16 page;
    page = CONFIG_ADDR;
    wait_busy();    //等待，flsah直到空闲
    CLR_SPI_CS();
    write_byte(CONTINUOUS_ARRAY_READ);
    write_byte((u8)(page >> 6));
    write_byte((u8)(page << 2));
    write_byte(0x00);
    write_byte(0x00);           //写一个dummy字节
    gSysConfg.disp_type = read_byte();
    
    if (gSysConfg.disp_type > DISP_LANG_BOTH) gSysConfg.disp_type = DISP_LANG_ENGLISH_ONLY;
    
    if (gSysConfg.disp_type == 0) gSysConfg.disp_type = DISP_LANG_ENGLISH_ONLY;
    
    SET_SPI_CS();
}

void UsbPrintInt(char *src_buf)
{
    u8 i;
    char buf[50];
    u8 j;
    i = 0;
    
    while (src_buf[i] != 0xff)
    {
        snprintf(buf, sizeof(buf), "%d+", src_buf[i]);
        usb_write_buf((u8 *)buf, strlen(buf));
        i ++;
        
        if (i >= 50) break;
    }
    
    snprintf(buf, sizeof(buf), "%d", 255);
    usb_write_buf((u8 *)buf, strlen(buf));
}

void UsbPrintConfig(void)
{
    u8 station;
    tLanguage lang;
    tBmpType line_no;
    char buf[50];
    u8 i;
#if (!IDU_COMPILE)
    return;
#endif
    GetConfig();
    snprintf(buf, sizeof(buf), "disp type: %d", gSysConfg.disp_type);
    down_print_line(buf);
    
    for (line_no = CONFIG_LINE1; line_no <= CONFIG_LINE3; line_no++)
    {
        snprintf(buf, sizeof(buf), "line%d:", line_no - CONFIG_LINE1 + 1);
        down_print_line(buf);
        
        for (station = 0; station < 10; station++)
        {
            snprintf(buf, sizeof(buf), "s%d:", station);
            down_print_line(buf);
            GetStationConfig((u8 *)buf, line_no, station, LANG_ENGLISH, CONFIG_TYPE_ALL);
            UsbPrintInt(buf);
            snprintf(buf, sizeof(buf), "\r\n");
            usb_write_buf(buf, strlen(buf));
            GetStationConfig((u8 *)buf, line_no, station, LANG_CHINESE, CONFIG_TYPE_ALL);
            UsbPrintInt(buf);
            SysDelay(400);
        }
    }
}

void debug_bmp(void)
{
    u16 page;
    tBmpParam param;
    param.type = BMP_TYPE_LINE1;
    param.index = 0;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    read_bmp(page);
    param.type = BMP_TYPE_LINE1;
    param.index = 0;
    param.langusge = LANG_CHINESE;
    page = GetBmpStartPage(param);
    read_bmp(page);
    param.type = BMP_TYPE_LINE2;
    param.index = 4;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    read_bmp(page);
    param.type = BMP_TYPE_LINE2;
    param.index = 4;
    param.langusge = LANG_CHINESE;
    page = GetBmpStartPage(param);
    read_bmp(page);
}

#if 0
u16 page;
void test_bmp(void)
{
    u16 h, w;
    u8 *pt;
    u8 i;
    u8 temp;
    tBmpParam param;
    u8 index;
    index = 0;
    param.type = BMP_TYPE_LINE1;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_LINE2;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_LINE3;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_MSG;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_NEXT_BEFORE;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_NEXT_AFTER;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_ARRIVE_BEFORE;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_ARRIVE_AFTER;
    param.index = index;
    param.langusge = LANG_ENGLISH;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_LINE1;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_LINE2;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_LINE3;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_MSG;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_NEXT_BEFORE;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_NEXT_AFTER;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_ARRIVE_BEFORE;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    param.type = BMP_TYPE_ARRIVE_AFTER;
    param.index = index;
    param.langusge = LANG_FARSI;
    page = GetBmpStartPage(param);
    temp = read_status();
    write_ok_flag();
    temp = read_status();
    page_erase(UPDATE_STATUS_ADDR);
    temp = read_status();
    temp = read_status();
    pt = (u8 *)gLedImageBuf;
    *(pt + 0) = 0x0;
    *(pt + 1) = 0x10;
    *(pt + 2) = 2;
    pt += 3;
    i = 0;
    
    for (h = 0; h < 16; h++)
    {
        for (w = 0; w < 512; w++)
        {
            *pt = i;
            i ++;
            pt ++;
        }
    }
    
    write_bmp(1, 4099, (u8 *)gLedImageBuf);
    
    for (h = 0; h < 16; h++)
    {
        for (w = 0; w < 512; w++)
        {
            gLedImageBuf[h][w] = 0;
        }
    }
    
    read_bmp(1);
}
#endif

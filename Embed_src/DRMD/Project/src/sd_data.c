#include "stm32f2xx.h"
#include "stm32_eval_sdio_sd.h"
#include "sd_data.h"
#include "udp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32f2x7_eth.h"
#include "netconf.h"
#include "udp_server.h"
#include "ff.h"
#include "mem.h"

#include "uart1.h"
#include "uart2.h"
#include "uart3.h"

#define     DATA_START_ADDR      0x0200000//0x0200000   //数据起始位置 前2M为索引区
#define     INDEX_START_ADDR     0x000200    //前512个字节做sd标识用
#define     INDEX_END_ADDR        0x1c2000
#define     BLOCK_SIZE                512
#define     SD_SIZE                      0x72000000 //0x80000000   // 1824MB  --实际有效空间没有2G
#define     ONE_INDEX_AREA        0x080000      //每个索引表示的范围(512KB)               512*1024

uint32_t sd_write_addr_cur;  //当前写sd卡的位置
uint32_t sd_write_index_addr_cur;//当前写sd卡索引的位置
uint32_t sd_write_index_addr_next;//下一次写sd卡索引位置
index_data_t last_index; //最后一次写的索引
//uint8_t   block_rx[BLOCK_SIZE];
uint8_t   buf_block_tx[BLOCK_SIZE];
//uint8_t   *buf_block_tx;
uint16_t  block_tx_index = 0;
SD_Error status = SD_OK;
uint8_t   sd_first_flag = 0;   //  1:表示sd卡从没有写过
uint8_t   set_rtc_flag = 0;   // 1:设置了rtc时间

RTC_TimeTypeDef  rtc_time;
RTC_DateTypeDef  rtc_date;
/****************************************************
*函数名：sd_data_init
*作者：hk
*描述：初始化当前写sd卡的位置
*输入：无
*输出：无
*返回值：无
*修改记录：无
****************************************************/
#if 0
void sd_data_init()
{
    uint32_t read_addr;
    index_data_t  *index_data;
    uint16_t index = 0;
    uint8_t find_flag = 0;
    static uint8_t init_flag = 0;
    
    if ((set_rtc_flag == 0) || init_flag) //中央控制器没有设置系统时间
        return;
        
    init_flag = 1;
    status = SD_ReadBlock(block_rx, 0x00, BLOCK_SIZE);
    
    if (status != SD_OK)
        return;
        
    if ((block_rx[0] == 0x55) && (block_rx[1] == 0x66) && (block_rx[2] == 0x77)) //不是新卡
    {
        sd_first_flag = 0;
        
        if (set_rtc_flag)
        {
            read_addr = INDEX_START_ADDR;
            sd_write_addr_cur = DATA_START_ADDR;
            
            while (read_addr < INDEX_END_ADDR)
            {
                SD_ReadBlock(block_rx, read_addr, BLOCK_SIZE);
                sd_write_index_addr_cur = read_addr;
                read_addr += BLOCK_SIZE;
                index_data = (index_data_t *)block_rx;
                
                if ((index_data->month == 0xaa) && (index_data->date == 0xbb)) //表示下一次写的索引区
                {
                    find_flag = 1;
                    break;
                }
                else if ((index_data->month == 0x00) && (index_data->date == 0x00)) //防止在擦除后，写之前断电
                {
                    find_flag = 1;
                    break;
                }
                else
                {
                    sd_write_addr_cur = index_data->start_addr;
                    memcpy(&last_index, index_data, sizeof(index_data_t));
                }
            }
            
            if (find_flag)
            {
                //找到索引位置，再找对应数据区位置
                for (index = 0; index < 1024; index++)  //512k包含1204个512字节
                {
                    SD_ReadBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE);
                    
                    if ((block_rx[0] == 0xaa) && (block_rx[1] == 0xbb) && (block_rx[2] == 0xcc)) //找到结尾标识
                    {
                        break;
                    }
                    else if ((block_rx[0] == 0x00) && (block_rx[1] == 0x00) && (block_rx[2] == 0x00)) //防止在擦除后，写之前断电
                    {
                        break;
                    }
                    
                    sd_write_addr_cur += BLOCK_SIZE;
                    
                    if (sd_write_addr_cur >= SD_SIZE) //已到达卡结尾
                        sd_write_addr_cur = DATA_START_ADDR;
                }
            }
        }
    }
    else
    {
        sd_first_flag = 1;
        read_addr = 0x00;
        
        while (read_addr < SD_SIZE)
        {
            SD_Erase(read_addr, BLOCK_SIZE);
            read_addr += read_addr + BLOCK_SIZE;
        }
    }
}

#endif
/****************************************************
*函数名：save_data_to_sd
*作者：hk
*描述：写数据到sd卡
*输入：无
*输出：无
*返回值：无
*修改记录：无
****************************************************/
#if 1
void save_data_to_sd()
{
    FATFS *fs, fatfs;
    FIL fsrc;
    int br;
    FRESULT res;
    RTC_DateTypeDef  rtc_date;
    char file_name[12] = {0};
    //uint8_t asiic_buf[]="0123456789ABCDEF";
    // uint8_t *buf = (uint8_t *)mem_calloc(512, 1);;
    //int i=0;
    //int index = 0;
    fs = &fatfs;
    fatfs.win = (uint8_t *)mem_calloc(512, 1);
    fsrc.buf = (uint8_t *)mem_calloc(512, 1);
    RTC_GetDate(RTC_Format_BIN, &rtc_date);
    sprintf(file_name, "%d%d%d.txt", rtc_date.RTC_Year + 2000, rtc_date.RTC_Month, rtc_date.RTC_Date);
    f_mount(0, fs);
    res = f_open(&fsrc, file_name, FA_OPEN_ALWAYS | FA_WRITE);
    
    if (res != FR_OK)
        res = f_open(&fsrc, file_name, FA_CREATE_NEW | FA_WRITE);
        
    if (res != FR_OK)
        return;
        
    f_lseek(&fsrc, fsrc.fsize);
    
    if (fsrc.fs->free_clust == 0) //磁盘已经写满
    {
        f_close(&fsrc);
        f_mount(0, NULL);
        mem_free(fatfs.win);
        mem_free(fsrc.buf );
        return;
    }
    
#if 0
    
    for (i = 0; i < block_tx_index; i++)
    {
        buf[index++] = asiic_buf[buf_block_tx[i] >> 4];
        
        if (index == 511)
        {
            f_write(&fsrc, buf, index, &br);
            index = 0;
        }
        
        buf[index++] = asiic_buf[buf_block_tx[i] & 0x0F];
        
        if (index == 511)
        {
            f_write(&fsrc, buf, index, &br);
            index = 0;
        }
        
        buf[index++] = ' ';
        
        if (index == 511)
        {
            f_write(&fsrc, buf, index, &br);
            index = 0;
        }
    }
    
    if (index != 0)
        f_write(&fsrc, buf, index, &br);
        
#endif
    f_write(&fsrc, buf_block_tx, block_tx_index, &br);
    f_close(&fsrc);
    f_mount(0, NULL);
    mem_free(fatfs.win);
    mem_free(fsrc.buf );
    //mem_free(buf);
}
#endif
#if 0
void save_data_to_sd()
{
    RTC_DateTypeDef  rtc_date;
    index_data_t index_data;
    RTC_GetDate(RTC_Format_BIN, &rtc_date);
    
    if (sd_first_flag) //新卡
    {
        //初始化
        memset(block_rx , 0x00 , BLOCK_SIZE);
        block_rx[0] = 0x55;
        block_rx[1] = 0x66;
        block_rx[2] = 0x77;
        SD_Erase(0x00, BLOCK_SIZE);
        status = SD_WriteBlock(block_rx, 0x00, BLOCK_SIZE);
        sd_first_flag = 0;
        sd_write_addr_cur = DATA_START_ADDR;
        sd_write_index_addr_cur = INDEX_START_ADDR;
        //建立索引
        memset(&last_index, 0x00, sizeof(index_data_t));
        last_index.date = rtc_date.RTC_Date;
        last_index.month = rtc_date.RTC_Month;
        last_index.year = rtc_date.RTC_Year;
        last_index.start_addr = sd_write_addr_cur;
        SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE * 2);
        SD_WriteBlock((uint8_t *)&last_index, sd_write_index_addr_cur, BLOCK_SIZE);
        memset(&index_data, 0x00, sizeof(index_data_t));
        index_data.date = 0xbb;
        index_data.month = 0xaa;
        sd_write_index_addr_cur += BLOCK_SIZE;
        SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //填写结束标识
        //保存数据
        SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE * 2);
        SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
        sd_write_addr_cur += BLOCK_SIZE;
        memset(block_rx, 0x00, BLOCK_SIZE);
        block_rx[0] = 0xaa;
        block_rx[1] = 0xbb;
        block_rx[2] = 0xcc;
        SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //填写数据结束标识
    }
    else   //旧卡
    {
        if ((rtc_date.RTC_Date != last_index.date) || (rtc_date.RTC_Month != last_index.month)
            || (rtc_date.RTC_Year != last_index.year))  //当前时间与上个索引时间不同，重开一个索引
        {
            //建立索引
            memset(&last_index, 0x00, sizeof(index_data_t));
            last_index.date = rtc_date.RTC_Date;
            last_index.month = rtc_date.RTC_Month;
            last_index.year = rtc_date.RTC_Year;
            sd_write_addr_cur += BLOCK_SIZE;    //跳过上一个数据结束标识
            
            if (sd_write_addr_cur >= SD_SIZE) //卡已写满
                sd_write_addr_cur = DATA_START_ADDR;
                
            last_index.start_addr = sd_write_addr_cur;
            SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
            SD_WriteBlock((uint8_t *)&last_index, sd_write_index_addr_cur, BLOCK_SIZE);
            memset(&index_data, 0x00, sizeof(index_data_t));
            index_data.date = 0xbb;
            index_data.month = 0xaa;
            sd_write_index_addr_cur += BLOCK_SIZE;
            
            if (sd_write_index_addr_cur >= INDEX_END_ADDR) //已经到达结尾
                sd_write_index_addr_cur = INDEX_START_ADDR;
                
            SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
            SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //填写结束标识
            //保存数据
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
            sd_write_addr_cur += BLOCK_SIZE;
            memset(block_rx, 0x00, BLOCK_SIZE);
            block_rx[0] = 0xaa;
            block_rx[1] = 0xbb;
            block_rx[2] = 0xcc;
            
            if (sd_write_addr_cur >= SD_SIZE) //卡已写满
                sd_write_addr_cur = DATA_START_ADDR;
                
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //填写数据结束标识
        }
        else   //当前时间与上个索引区表示的时间相同
        {
            if (sd_write_addr_cur >= (last_index.start_addr + ONE_INDEX_AREA)) //到一个索引指示的最大范围
            {
                //新开一个索引
                memset(&last_index, 0x00, sizeof(index_data_t));
                last_index.date = rtc_date.RTC_Date;
                last_index.month = rtc_date.RTC_Month;
                last_index.year = rtc_date.RTC_Year;
                sd_write_addr_cur += BLOCK_SIZE;    //跳过上一个数据结束标识
                
                if (sd_write_addr_cur >= SD_SIZE) //卡已写满
                    sd_write_addr_cur = DATA_START_ADDR;
                    
                last_index.start_addr = sd_write_addr_cur;
                SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
                SD_WriteBlock((uint8_t *)&last_index, sd_write_index_addr_cur, BLOCK_SIZE);
                memset(&index_data, 0x00, sizeof(index_data_t));
                index_data.date = 0xbb;
                index_data.month = 0xaa;
                sd_write_index_addr_cur += BLOCK_SIZE;
                
                if (sd_write_index_addr_cur >= INDEX_END_ADDR) //已经到达结尾
                    sd_write_index_addr_cur = INDEX_START_ADDR;
                    
                SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
                SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //填写结束标识
            }
            
            //保存数据
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
            sd_write_addr_cur += BLOCK_SIZE;
            memset(block_rx, 0x00, BLOCK_SIZE);
            block_rx[0] = 0xaa;
            block_rx[1] = 0xbb;
            block_rx[2] = 0xcc;
            
            if (sd_write_addr_cur >= SD_SIZE) //卡已写满
                sd_write_addr_cur = DATA_START_ADDR;
                
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //填写数据结束标识
        }
    }
}
#endif

/****************************************************
*函数名：fill_sd_data_fifo
*作者：hk
*描述：写入数据缓冲区
*输入：buf---保存的数据   length---数据长度
                 in_out---1:输出0:输入  indx:端口1-5表示对应串口1-5， 6表示网口
*输出：无
*返回值：无
*修改记录：无
****************************************************/
#if 1
void fill_sd_data_fifo(uint8_t *buf, uint16_t length, uint8_t in_out, uint8_t indx)
{
    static RTC_TimeTypeDef  rtc_time;
    uint8_t asiic_buf[] = "0123456789ABCDEF";
    static uint32_t timer = 0;
    uint16_t i = 0;
    //  if(set_rtc_flag == 0)//中央控制器没有设置系统时间
    //  return;
    
    if (check_timer(&timer, 1000))
        RTC_GetTime(RTC_Format_BIN, &rtc_time);
        
    if ((block_tx_index + length * 3 + 20) > BLOCK_SIZE)
    {
        save_data_to_sd();
        memset(buf_block_tx, 0x00, BLOCK_SIZE);
        block_tx_index = 0;
        
        if (length * 3 + 20 > BLOCK_SIZE)
            return ;
    }
    
    buf_block_tx[block_tx_index++] = '7';
    buf_block_tx[block_tx_index++] = 'e';
    buf_block_tx[block_tx_index++] = ' ';
    buf_block_tx[block_tx_index++] = '0' + in_out;
    buf_block_tx[block_tx_index++] = '0' + indx;
    buf_block_tx[block_tx_index++] = ' ';
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Hours >> 4];
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Hours & 0x0F];
    buf_block_tx[block_tx_index++] = ' ';
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Minutes >> 4];
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Minutes & 0x0F];
    buf_block_tx[block_tx_index++] = ' ';
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Seconds >> 4];
    buf_block_tx[block_tx_index++] = asiic_buf[rtc_time.RTC_Seconds & 0x0F];
    buf_block_tx[block_tx_index++] = ' ';
    
    for (i = 0; i < length; i++)
    {
        if (buf[i] == 0x7e)
        {
            buf_block_tx[block_tx_index++] = '7';
            buf_block_tx[block_tx_index++] = 'f';
            buf_block_tx[block_tx_index++] = ' ';
            buf_block_tx[block_tx_index++] = '8';
            buf_block_tx[block_tx_index++] = '0';
            buf_block_tx[block_tx_index++] = ' ';
        }
        else if (buf[i] == 0x7f)
        {
            buf_block_tx[block_tx_index++] = '7';
            buf_block_tx[block_tx_index++] = 'f';
            buf_block_tx[block_tx_index++] = ' ';
            buf_block_tx[block_tx_index++] = '8';
            buf_block_tx[block_tx_index++] = '1';
            buf_block_tx[block_tx_index++] = ' ';
        }
        else
        {
            buf_block_tx[block_tx_index++] = asiic_buf[buf[i] >> 4];
            buf_block_tx[block_tx_index++] = asiic_buf[buf[i] & 0x0F];
            buf_block_tx[block_tx_index++] = ' ';
        }
    }
    
    //buf_block_tx[block_tx_index++] = 0x7e;
    buf_block_tx[block_tx_index++] = '7';
    buf_block_tx[block_tx_index++] = 'e';
    buf_block_tx[block_tx_index++] = ' ';
}
#endif
#if 0
void fill_sd_data_fifo(uint8_t *buf, uint16_t length)
{
    static RTC_TimeTypeDef  rtc_time;
    static uint32_t timer = 0;
    uint16_t i = 0;
    //  if(set_rtc_flag == 0)//中央控制器没有设置系统时间
    //  return;
    
    if (check_timer(&timer, 1000))
        RTC_GetTime(RTC_Format_BIN, &rtc_time);
        
    if ((block_tx_index + length + 8) > BLOCK_SIZE)
    {
        save_data_to_sd();
        memset(buf_block_tx, 0x00, BLOCK_SIZE);
        block_tx_index = 0;
        
        if (length > BLOCK_SIZE - 3)
            return ;
    }
    
    buf_block_tx[block_tx_index++] = 0x7e;
    buf_block_tx[block_tx_index++] = 0x76;
    buf_block_tx[block_tx_index++] = rtc_time.RTC_Hours;
    buf_block_tx[block_tx_index++] = rtc_time.RTC_Minutes;
    buf_block_tx[block_tx_index++] = rtc_time.RTC_Seconds;
    
    for (i = 0; i < length; i++)
    {
        if (buf[i] == 0x7e)
        {
            buf_block_tx[block_tx_index++] = 0x7f;
            buf_block_tx[block_tx_index++] = 0x80;
        }
        else if (buf[i] == 0x7f)
        {
            buf_block_tx[block_tx_index++] = 0x7f;
            buf_block_tx[block_tx_index++] = 0x81;
        }
        else
        {
            buf_block_tx[block_tx_index++] = buf[i];
        }
    }
    
    buf_block_tx[block_tx_index++] = 0x7e;
}
#endif
void set_rtc_date(uint8_t year, uint8_t month, uint8_t date)
{
    RTC_DateTypeDef  rtc_date;
    set_rtc_flag = 1;
    rtc_date.RTC_Date = date;
    rtc_date.RTC_Month = month;
    rtc_date.RTC_Year = year;
    rtc_date.RTC_WeekDay = 0;
    RTC_SetDate(RTC_Format_BIN, &rtc_date);
}

void set_rtc_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_TimeTypeDef  rtc_time;
    rtc_time.RTC_Hours = hour;
    rtc_time.RTC_Minutes = min;
    rtc_time.RTC_Seconds = sec;
    rtc_time.RTC_H12 = RTC_H12_AM;
    
    if (hour > 12)
        rtc_time.RTC_H12 = RTC_H12_PM;
        
    RTC_SetTime(RTC_Format_BIN, &rtc_time);
}
#if 0
uint8_t date_cmp(index_data_t *date1, index_data_t *date2)
{
    //date1>= date2 return 1 or return 0
    if (date1->year > date2->year)
        return 1;
    else if (date1->year == date2->year)
    {
        if (date1->month > date2->month)
            return 1;
        else if (date1->month == date2->month)
        {
            if (date1->date >= date2->date)
                return 1;
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
}

void get_sd_data(index_data_t *from, index_data_t *to)
{
    uint32_t read_addr = INDEX_START_ADDR;
    uint32_t read_data_addr;
    index_data_t *index_data;
    uint32_t cnt;
    
    while (read_addr < INDEX_END_ADDR)
    {
        SD_ReadBlock(block_rx, read_addr, BLOCK_SIZE);
        read_addr += BLOCK_SIZE;
        index_data = (index_data_t *)block_rx;
        
        if (index_data->year > 99 || index_data->month > 12 || index_data->date > 31)
            continue;
            
        if ((index_data->month == 0xaa) && (index_data->date == 0xbb)) //表示下一次写的索引区
        {
            continue;
        }
        
        if (date_cmp(index_data, from) == 0) //数据时间< from
        {
            continue;
        }
        
        if (date_cmp(index_data, to))  //数据时间> to
        {
            continue;
        }
        
        read_data_addr = index_data->start_addr;
        block_rx[0] = 0x7e;
        block_rx[1] = 0x77;//命令标识，表示时间
        block_rx[2] = index_data->year;
        block_rx[3] = index_data->month;
        block_rx[4] = index_data->date;
        block_rx[5] = 0x7e;
        //send_int_buf_to_telnet(block_rx, 6);
        send_int_buf_by_udp(block_rx, 6);
        uart3_write_buf(block_rx, 6);
        cnt = 0;
        
        while (1)
        {
            SD_ReadBlock(block_rx, read_data_addr, BLOCK_SIZE);
            
            if ((block_rx[0] == 0xaa) && (block_rx[1] == 0xbb) && (block_rx[2] == 0xcc)) //找到结尾标识
            {
                break;
            }
            
            cnt++;
            
            if (cnt > 1024) //防止数据出问题了一直在读
                break;
                
            //把数据发送出去
            uart3_write_buf(block_rx, BLOCK_SIZE);
            //send_int_buf_to_telnet(block_rx, BLOCK_SIZE);
            send_int_buf_by_udp(block_rx, BLOCK_SIZE);
            read_data_addr += BLOCK_SIZE;
            
            if (read_data_addr >= SD_SIZE) //卡已写满
                read_data_addr = DATA_START_ADDR;
                
#if 1
                
            /* check if any packet received */
            if (ETH_CheckFrameReceived())
            {
                /* process received ethernet packet */
                LwIP_Pkt_Handle();
            }
            
            /* handle periodic timers for LwIP */
            //LwIP_Periodic_Handle(LocalTime);
#endif
        }
        
        //需要喂狗
    }
}
#endif

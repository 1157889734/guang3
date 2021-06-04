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

#define     DATA_START_ADDR      0x0200000//0x0200000   //������ʼλ�� ǰ2MΪ������
#define     INDEX_START_ADDR     0x000200    //ǰ512���ֽ���sd��ʶ��
#define     INDEX_END_ADDR        0x1c2000
#define     BLOCK_SIZE                512
#define     SD_SIZE                      0x72000000 //0x80000000   // 1824MB  --ʵ����Ч�ռ�û��2G
#define     ONE_INDEX_AREA        0x080000      //ÿ��������ʾ�ķ�Χ(512KB)               512*1024

uint32_t sd_write_addr_cur;  //��ǰдsd����λ��
uint32_t sd_write_index_addr_cur;//��ǰдsd��������λ��
uint32_t sd_write_index_addr_next;//��һ��дsd������λ��
index_data_t last_index; //���һ��д������
//uint8_t   block_rx[BLOCK_SIZE];
uint8_t   buf_block_tx[BLOCK_SIZE];
//uint8_t   *buf_block_tx;
uint16_t  block_tx_index = 0;
SD_Error status = SD_OK;
uint8_t   sd_first_flag = 0;   //  1:��ʾsd����û��д��
uint8_t   set_rtc_flag = 0;   // 1:������rtcʱ��

RTC_TimeTypeDef  rtc_time;
RTC_DateTypeDef  rtc_date;
/****************************************************
*��������sd_data_init
*���ߣ�hk
*��������ʼ����ǰдsd����λ��
*���룺��
*�������
*����ֵ����
*�޸ļ�¼����
****************************************************/
#if 0
void sd_data_init()
{
    uint32_t read_addr;
    index_data_t  *index_data;
    uint16_t index = 0;
    uint8_t find_flag = 0;
    static uint8_t init_flag = 0;
    
    if ((set_rtc_flag == 0) || init_flag) //���������û������ϵͳʱ��
        return;
        
    init_flag = 1;
    status = SD_ReadBlock(block_rx, 0x00, BLOCK_SIZE);
    
    if (status != SD_OK)
        return;
        
    if ((block_rx[0] == 0x55) && (block_rx[1] == 0x66) && (block_rx[2] == 0x77)) //�����¿�
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
                
                if ((index_data->month == 0xaa) && (index_data->date == 0xbb)) //��ʾ��һ��д��������
                {
                    find_flag = 1;
                    break;
                }
                else if ((index_data->month == 0x00) && (index_data->date == 0x00)) //��ֹ�ڲ�����д֮ǰ�ϵ�
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
                //�ҵ�����λ�ã����Ҷ�Ӧ������λ��
                for (index = 0; index < 1024; index++)  //512k����1204��512�ֽ�
                {
                    SD_ReadBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE);
                    
                    if ((block_rx[0] == 0xaa) && (block_rx[1] == 0xbb) && (block_rx[2] == 0xcc)) //�ҵ���β��ʶ
                    {
                        break;
                    }
                    else if ((block_rx[0] == 0x00) && (block_rx[1] == 0x00) && (block_rx[2] == 0x00)) //��ֹ�ڲ�����д֮ǰ�ϵ�
                    {
                        break;
                    }
                    
                    sd_write_addr_cur += BLOCK_SIZE;
                    
                    if (sd_write_addr_cur >= SD_SIZE) //�ѵ��￨��β
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
*��������save_data_to_sd
*���ߣ�hk
*������д���ݵ�sd��
*���룺��
*�������
*����ֵ����
*�޸ļ�¼����
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
    
    if (fsrc.fs->free_clust == 0) //�����Ѿ�д��
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
    
    if (sd_first_flag) //�¿�
    {
        //��ʼ��
        memset(block_rx , 0x00 , BLOCK_SIZE);
        block_rx[0] = 0x55;
        block_rx[1] = 0x66;
        block_rx[2] = 0x77;
        SD_Erase(0x00, BLOCK_SIZE);
        status = SD_WriteBlock(block_rx, 0x00, BLOCK_SIZE);
        sd_first_flag = 0;
        sd_write_addr_cur = DATA_START_ADDR;
        sd_write_index_addr_cur = INDEX_START_ADDR;
        //��������
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
        SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //��д������ʶ
        //��������
        SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE * 2);
        SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
        sd_write_addr_cur += BLOCK_SIZE;
        memset(block_rx, 0x00, BLOCK_SIZE);
        block_rx[0] = 0xaa;
        block_rx[1] = 0xbb;
        block_rx[2] = 0xcc;
        SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //��д���ݽ�����ʶ
    }
    else   //�ɿ�
    {
        if ((rtc_date.RTC_Date != last_index.date) || (rtc_date.RTC_Month != last_index.month)
            || (rtc_date.RTC_Year != last_index.year))  //��ǰʱ�����ϸ�����ʱ�䲻ͬ���ؿ�һ������
        {
            //��������
            memset(&last_index, 0x00, sizeof(index_data_t));
            last_index.date = rtc_date.RTC_Date;
            last_index.month = rtc_date.RTC_Month;
            last_index.year = rtc_date.RTC_Year;
            sd_write_addr_cur += BLOCK_SIZE;    //������һ�����ݽ�����ʶ
            
            if (sd_write_addr_cur >= SD_SIZE) //����д��
                sd_write_addr_cur = DATA_START_ADDR;
                
            last_index.start_addr = sd_write_addr_cur;
            SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
            SD_WriteBlock((uint8_t *)&last_index, sd_write_index_addr_cur, BLOCK_SIZE);
            memset(&index_data, 0x00, sizeof(index_data_t));
            index_data.date = 0xbb;
            index_data.month = 0xaa;
            sd_write_index_addr_cur += BLOCK_SIZE;
            
            if (sd_write_index_addr_cur >= INDEX_END_ADDR) //�Ѿ������β
                sd_write_index_addr_cur = INDEX_START_ADDR;
                
            SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
            SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //��д������ʶ
            //��������
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
            sd_write_addr_cur += BLOCK_SIZE;
            memset(block_rx, 0x00, BLOCK_SIZE);
            block_rx[0] = 0xaa;
            block_rx[1] = 0xbb;
            block_rx[2] = 0xcc;
            
            if (sd_write_addr_cur >= SD_SIZE) //����д��
                sd_write_addr_cur = DATA_START_ADDR;
                
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //��д���ݽ�����ʶ
        }
        else   //��ǰʱ�����ϸ���������ʾ��ʱ����ͬ
        {
            if (sd_write_addr_cur >= (last_index.start_addr + ONE_INDEX_AREA)) //��һ������ָʾ�����Χ
            {
                //�¿�һ������
                memset(&last_index, 0x00, sizeof(index_data_t));
                last_index.date = rtc_date.RTC_Date;
                last_index.month = rtc_date.RTC_Month;
                last_index.year = rtc_date.RTC_Year;
                sd_write_addr_cur += BLOCK_SIZE;    //������һ�����ݽ�����ʶ
                
                if (sd_write_addr_cur >= SD_SIZE) //����д��
                    sd_write_addr_cur = DATA_START_ADDR;
                    
                last_index.start_addr = sd_write_addr_cur;
                SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
                SD_WriteBlock((uint8_t *)&last_index, sd_write_index_addr_cur, BLOCK_SIZE);
                memset(&index_data, 0x00, sizeof(index_data_t));
                index_data.date = 0xbb;
                index_data.month = 0xaa;
                sd_write_index_addr_cur += BLOCK_SIZE;
                
                if (sd_write_index_addr_cur >= INDEX_END_ADDR) //�Ѿ������β
                    sd_write_index_addr_cur = INDEX_START_ADDR;
                    
                SD_Erase(sd_write_index_addr_cur, sd_write_index_addr_cur + BLOCK_SIZE);
                SD_WriteBlock((uint8_t *)&index_data, sd_write_index_addr_cur, BLOCK_SIZE); //��д������ʶ
            }
            
            //��������
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(buf_block_tx, sd_write_addr_cur, BLOCK_SIZE);
            sd_write_addr_cur += BLOCK_SIZE;
            memset(block_rx, 0x00, BLOCK_SIZE);
            block_rx[0] = 0xaa;
            block_rx[1] = 0xbb;
            block_rx[2] = 0xcc;
            
            if (sd_write_addr_cur >= SD_SIZE) //����д��
                sd_write_addr_cur = DATA_START_ADDR;
                
            SD_Erase(sd_write_addr_cur, sd_write_addr_cur + BLOCK_SIZE);
            SD_WriteBlock(block_rx, sd_write_addr_cur, BLOCK_SIZE); //��д���ݽ�����ʶ
        }
    }
}
#endif

/****************************************************
*��������fill_sd_data_fifo
*���ߣ�hk
*������д�����ݻ�����
*���룺buf---���������   length---���ݳ���
                 in_out---1:���0:����  indx:�˿�1-5��ʾ��Ӧ����1-5�� 6��ʾ����
*�������
*����ֵ����
*�޸ļ�¼����
****************************************************/
#if 1
void fill_sd_data_fifo(uint8_t *buf, uint16_t length, uint8_t in_out, uint8_t indx)
{
    static RTC_TimeTypeDef  rtc_time;
    uint8_t asiic_buf[] = "0123456789ABCDEF";
    static uint32_t timer = 0;
    uint16_t i = 0;
    //  if(set_rtc_flag == 0)//���������û������ϵͳʱ��
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
    //  if(set_rtc_flag == 0)//���������û������ϵͳʱ��
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
            
        if ((index_data->month == 0xaa) && (index_data->date == 0xbb)) //��ʾ��һ��д��������
        {
            continue;
        }
        
        if (date_cmp(index_data, from) == 0) //����ʱ��< from
        {
            continue;
        }
        
        if (date_cmp(index_data, to))  //����ʱ��> to
        {
            continue;
        }
        
        read_data_addr = index_data->start_addr;
        block_rx[0] = 0x7e;
        block_rx[1] = 0x77;//�����ʶ����ʾʱ��
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
            
            if ((block_rx[0] == 0xaa) && (block_rx[1] == 0xbb) && (block_rx[2] == 0xcc)) //�ҵ���β��ʶ
            {
                break;
            }
            
            cnt++;
            
            if (cnt > 1024) //��ֹ���ݳ�������һֱ�ڶ�
                break;
                
            //�����ݷ��ͳ�ȥ
            uart3_write_buf(block_rx, BLOCK_SIZE);
            //send_int_buf_to_telnet(block_rx, BLOCK_SIZE);
            send_int_buf_by_udp(block_rx, BLOCK_SIZE);
            read_data_addr += BLOCK_SIZE;
            
            if (read_data_addr >= SD_SIZE) //����д��
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
        
        //��Ҫι��
    }
}
#endif

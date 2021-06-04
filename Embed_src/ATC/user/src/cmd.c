/************************************************************
  Copyright (C), 1988-1999, beihai Tech. Co., Ltd.
  FileName: info_save
  Author:
  Description:     ʵ����λ������
  Version:
  History:
    <author>  <time>   <version >   <desc>
      bluck         2013-3-22    1.0     build this moudle
***********************************************************/

#include "main.h"
#include "stm32f2xx_iwdg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <def.h>
#include "sd_data.h"
#include "ff.h"
#include "rtc_1337.h"
#include "usb.h"
#include "cmd.h"
#include "flash_if.h"
#include "httpserver.h"
#include "tcms.h"
#include "tba_bus.h"
#include "usb_bsp.h"
#include "mem.h"
#include "uart.h"


uint32 upgrade_total_size = 0; //�����ļ���С
uint32 upgrade_recv_size = 0; //���ν��յ����ֽ���
uint8  upgrade_flag      = 0; //������ʶ
extern tstate      atc_tms;             //atc��tmsͨ��״̬
extern tstate      atc_pis;             //atc��pisͨ��״̬
extern tstate   atc_tba;
tptu_info   ptu_recv;        //����ptu��������Ϣ
tframe_info ptu_send;        //����ptu��������Ϣ

tptu_states  ptu;            //PTU��Ϣ

FIL   fsrc;                  //SD���ļ���Ϣ

static __IO uint32_t flash_write_address; //�����ļ��洢��ַ

/***********************************************************************************************************************
*Function Nane     :  get_cs
*Description       :  �������ݵ�У���
*Input             :  pdata   ָ������
                      length  ���ݳ���
*Output            :  cs ָ��У���
*Return            :  0 ʧ�� 1�ɹ�
*Others            :  ��
***********************************************************************************************************************/
uint8 get_cs(uint8 *pdata,  uint16  length,  uint8 *cs)
{
    uint16  i = 0;
    uint8  checksum = 0;
    
    if (NULL == pdata)
    {
        return 0;
    }
    
    for (i = 0; i < length; i++)
    {
        checksum += pdata[i];
    }
    
    *cs = 0x55 - checksum;
    return 1;
}
/***********************************************************************************************************************
*Function Nane    :  cmd_upgrade
*Description      :  ��������
*Input            :  type      : �������� 1 ��ʼ���� 2 �������� 3 ��������
                     packet    : ָ��Ҫд������ָ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void cmd_upgrade(uint8 type, uint8 *packet)
{
    static uint16 index;
    static uint16 oldindex;
    static uint8  flag  = 0;
    uint8 buf[8] = {0x7e, 0x04, 0x01, 0x01, 0x5a, 0x5a, 0x9b, 0x7e};
    
    if (1 == type)
    {
        //�������������ݴ�С
        upgrade_total_size = 0;
        upgrade_recv_size  = 0;
        upgrade_total_size = packet[5];
        upgrade_total_size = upgrade_total_size << 8 | packet[6];
        upgrade_total_size = upgrade_total_size << 8 | packet[7];
        upgrade_total_size = upgrade_total_size << 8 | packet[8];
        FLASH_If_Init();
        // erase user flash area
        FLASH_If_Erase_Prog_Bak(PROG_BAK_ADDRESS);
        flash_write_address = PROG_BAK_ADDRESS;
        get_cs(&buf[1], buf[1] + 1, &buf[6]);
        usb_write_buf(buf, 8);
        oldindex  = 0;
        index     = 0;
        flag      = 1;
    }
    else if (2 == type)
    {
        //�м����ݣ�ֱ��д�뱸����
        upgrade_recv_size += packet[0] - 4;
        FLASH_If_Write(&flash_write_address, (uint32_t *) &packet[5], (uint16_t) (packet[0] - 4) / 4);
        index = packet[3];
        index = index << 8 | packet[4];
        
        //�ж��Ƿ�������
        if (oldindex != (index - 1))
        {
            flag = 0;
        }
        
        oldindex = index;
        buf[3] = 0x02;
        buf[4] = packet[3];
        buf[5] = packet[4];
        get_cs(&buf[1], buf[1] + 1, &buf[6]);
        usb_write_buf(buf, 8);
    }
    else if (3 == type)
    {
        Delay(1);
        
        if ((upgrade_recv_size == upgrade_recv_size) && (flag))
        {
            //�����ɹ�
            buf[3] = 0x03;
            buf[4] = 0x5a;
            buf[5] = 0x5a;
            get_cs(&buf[1], buf[1] + 1, &buf[6]);
            usb_write_buf(buf, 8);
            USB_OTG_BSP_DisableInterrupt ();
            exec_iap_fun();
        }
        else
        {
            //����ʧ��
            buf[3] = 0x03;
            buf[4] = 0x55;
            buf[5] = 0x55;
            get_cs(&buf[1], buf[1] + 1, &buf[6]);
            usb_write_buf(buf, 8);
        }
        
        upgrade_flag = 1;
    }
}
/***********************************************************************************************************************
*Function Nane     :  cmd_version
*Description       :  �汾��ѯ
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_version(void)
{
    uint8 buf[100] = {0};
    char buf1[30];
    uint8 len = 0;
    buf[0] = 0x7e;
    buf[2] = 0x02;
    //������
    len = strlen(GREETING);
    memcpy(&buf[3], GREETING, len);
    buf[1] = 1 + len;
    get_cs(&buf[1], 2 + len, &buf[buf[1] + 2]);
    buf[buf[1] + 3] = 0x7e;
    usb_write_buf(buf, buf[1] + 4);
    //�汾��Ϣ
    len = strlen(VERSION);
    memcpy(&buf[3], VERSION, len);
    buf[1] = 1 + len;
    get_cs(&buf[1], 2 + len, &buf[buf[1] + 2]);
    buf[buf[1] + 3] = 0x7e;
    usb_write_buf(buf, buf[1] + 4);
    snprintf(buf1, sizeof(buf1), "address of device = %d \r\n", get_device_addr());
    //�豸��ַ
    len = strlen(buf1);
    memcpy(&buf[3], buf1, len);
    buf[1] = 1 + len;
    get_cs(&buf[1], 2 + len, &buf[buf[1] + 2]);
    buf[buf[1] + 3] = 0x7e;
    usb_write_buf(buf, buf[1] + 4);
}
/***********************************************************************************************************************
*Function Nane     :  cmd_set_time
*Description       :  ����rtcʱ��
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_set_time(uint8 *time)
{
    uint8 buf[6] = {0x7e, 0x02, 0x06, 0x5a, 0xf3, 0x7e};
    tRtcTimer ttimer;
    tRtcTimer oldtimer;
    ttimer.sec =    time[0];
    ttimer.min =    time[1];
    ttimer.hour =   time[2];
    ttimer.date =   time[3];
    ttimer.month =  time[4];
    ttimer.year =   time[5];
    get_rtc_time(&oldtimer);
    
    if (0 == set_rtc_time(ttimer))
    {
        //����ʧ��
        buf[3] = 0x55;
        buf[4] = 0xf8;
        usb_write_buf(buf, 6);
    }
    else
    {
        Delay(1);
        //���óɹ�
        update_cur_time();
        update_dir();
        save_change_time(oldtimer, ttimer);
        usb_write_buf(buf, 6);
    }
}
/***********************************************************************************************************************
*Function Nane   :  cmd_get_time
*Description     :  ��ȡrtcʱ��
*Input           :  ��
*Output          :  ��
*Return          :  ��
*Others          :  ��
***********************************************************************************************************************/
void cmd_get_time(void)
{
    tRtcTimer ttimer;
    uint8 time_buf[11] = {0x7e, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    get_rtc_time(&ttimer);
    time_buf[3] = ttimer.sec;
    time_buf[4] = ttimer.min;
    time_buf[5] = ttimer.hour;
    time_buf[6] = ttimer.date;
    time_buf[7] = ttimer.month;
    time_buf[8] = ttimer.year;
    get_cs(&time_buf[1], 8, &time_buf[9]);
    usb_write_buf(time_buf, sizeof(time_buf));
}
/***********************************************************************************************************************
*Function Nane   :  get_comn_state
*Description     :  ��ȡͨ��״̬
*Input           :  ��
*Output          :  ��
*Return          :  ��
*Others          :  ��
***********************************************************************************************************************/
void get_comn_state(void)
{
    uint8 state_buf[9] = {0x7e, 0x05, 0x08, 0X01, 0x00, 0x00, 0x00, 0x00, 0x7e};
    
    if (get_tms_comm())
    {
        state_buf[4] = 0x5a;
    }
    
    if (get_pis_comm())
    {
        state_buf[5] = 0x5a;
    }
    
    if (get_train485_comm())
    {
        state_buf[6] = 0x5a;
    }
    
    get_cs(&state_buf[1], 6, &state_buf[7]);
    usb_write_buf(state_buf, 9);
}
/***********************************************************************************************************************
*Function Nane    :  cmd_get_state
*Description      :  ATCģ��״̬
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void cmd_get_state(void)
{
    switch (ptu_recv.buf[3])
    {
        case SENDSTATE:
        {
            get_comn_state(); //��ȡͨ��״̬
        }
        break;
        
        case SENDTMS:
        {
            set_tcms_debug(1);
        }
        break;
        
        case SENDPIS:
        {
            set_pis_debug(1);
        }
        break;
        
        case CANCELTMS:
        {
            set_tcms_debug(0);
        }
        break;
        
        case CANCELPIS:
        {
            set_pis_debug(0);
        }
        break;
        
        default:
            break;
    }
}
/***************************************************************************************
*Function Nane    : cmd_format_sd
*Description      :  ��ʽ��sd��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
*****************************************************************************************/
void cmd_format_sd(void)
{
    DIR dir;
    FILINFO dirinfo;
    
    if (FR_OK == f_opendir(&dir, "0:"))
    {
        while (FR_OK == f_readdir(&dir, &dirinfo))
        {
            if (!(strcmp(dirinfo.fname, ".")) || (!(strcmp(dirinfo.fname, ".."))))
            {
                continue;
            }
            
            if (!dirinfo.fname[0])
            {
                return ;
            }
            
            sd_remove_file(dirinfo.fname);
        }
    }
}
/***********************************************************************************************************************
*Function Nane    :  send_sd_space
*Description      :  ����sd�Ŀռ���Ϣ
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
static void send_sd_space(void)
{
    uint8    sd_buf[14] = {0x7e, 0x0a, 0x09, 0x02, 0x00, 0x00, 0x00, 0x00
                           , 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e
                          };
    uint32   total = 0;
    uint32   free = 0;
    
    //SD����������ʣ��ռ�
    if (0 == get_sd_space(&total, &free))
    {
        total = 0;
        free = 0;
    }
    
    sd_buf[4] = (total >> 24) & 0xff;
    sd_buf[5] = (total >> 16) & 0xff;
    sd_buf[6] = (total >> 8) & 0xff;
    sd_buf[7] = (total) & 0xff;
    sd_buf[8] = (free >> 24) & 0xff;
    sd_buf[9] = (free >> 16) & 0xff;
    sd_buf[10] = (free >> 8) & 0xff;
    sd_buf[11] = (free) & 0xff;
    get_cs(&sd_buf[1], 11, &sd_buf[12]);
    usb_write_buf(sd_buf, 14);
}



/***************************************************************************************
*Function Nane   :  get_file_info
*Description     :  ��ȡ�ļ���Ϣ
*Input           :  path �ļ�·��
*Output          :  ��
*Return          :  ��
*Others          :  ��
*****************************************************************************************/
void file_info(char *path, uint32 fsize)
{
    char  cbuf[MAXPATH] = {0};
    uint8 sd_buf[100] = {0};
    uint8 len = 0;
    sd_buf[0] = 0x7e;
    sd_buf[2] = CMD_SDINFO;
    sd_buf[3] = 0x04;     //��ͨ�ļ�
    //�ļ���С
    sd_buf[4] = (fsize >> 24) & 0xff;
    sd_buf[5] = (fsize >> 16) & 0xff;
    sd_buf[6] = (fsize >> 8) & 0xff;
    sd_buf[7] = (fsize) & 0xff;
    //�ļ�·��
    f_getcwd(cbuf, MAXPATH);
    
    if (3 == strlen(cbuf))
    {
        //��Ŀ¼�����ü���"/"
    }
    else
    {
        strcat(cbuf, "/");
    }
    
    strcat(cbuf, path);
    //�ļ�·������
    len = strlen(cbuf);
    //�ļ�·��
    strncpy((char *)&sd_buf[8], cbuf, len);
    //�ֳ�
    sd_buf[1] = 6 + len;
    //У����
    get_cs(&sd_buf[1], sd_buf[1] + 1, &sd_buf[sd_buf[1] + 2]);
    sd_buf[sd_buf[1] + 3] = 0x7e;
    //��������
    usb_write_buf(sd_buf, sd_buf[1] + 4);
}
/***************************************************************************************
*Function Nane   :  get_file_info
*Description     :  ��ȡ�ļ���Ϣ
*Input           :  path �ļ�·��
*Output          :  ��
*Return          :  ��
*Others          :  ��
*****************************************************************************************/
void get_file_info(char *path)
{
    DIR dir;
    FILINFO dirinfo;
    char  cbuf[MAXPATH] = {0};
    uint8 sd_buf[100];
    uint8 len = 0;
    uint32 size = 0;
    sd_buf[0] = 0x7e;
    sd_buf[2] = CMD_SDINFO;
    
    //��ȡ�ļ���Ϣ
    if (FR_OK != f_opendir(&dir, path))
    {
        return ;
    }
    
    while (FR_OK == f_readdir(&dir, &dirinfo))
    {
        if (!dirinfo.fname[0])
        {
            break;
        }
        
        if (!(strcmp(dirinfo.fname, ".")) || (!(strcmp(dirinfo.fname, ".."))))
        {
            continue;
        }
        
        //Ŀ¼�ļ�
        if (AM_DIR == dirinfo.fattrib)
        {
            sd_buf[3] = 0x03;
            //Ŀ¼�ļ���С
            size = get_dir_size(dirinfo.fname);
            sd_buf[4] = (size >> 24) & 0xff;
            sd_buf[5] = (size >> 16) & 0xff;
            sd_buf[6] = (size >> 8) & 0xff;
            sd_buf[7] = (size) & 0xff;
            //Ŀ¼·��
            f_getcwd(cbuf, MAXPATH);
            strcat(cbuf, "/");
            strcat(cbuf, dirinfo.fname);
            len = strlen(cbuf);  //Ŀ¼·������
            strncpy((char *)&sd_buf[8], cbuf, len);
            sd_buf[1] = 6 + len;
            //У����
            get_cs(&sd_buf[1], sd_buf[1] + 1, &sd_buf[sd_buf[1] + 2]);
            sd_buf[sd_buf[1] + 3] = 0x7e;
            //��������
            usb_write_buf(sd_buf, sd_buf[1] + 4);
            //�����Ŀ¼
            f_chdir(dirinfo.fname);
            get_file_info(".");
            //������һ��Ŀ¼
            f_chdir("..");
        }
        else
        {
            file_info(dirinfo.fname, dirinfo.fsize);
        }
    }
}
/***********************************************************************************************************************
*Function Nane    :  cmd_get_sd_info
*Description      :  ��ȡsd��Ϣ
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void cmd_get_sd_info(void)
{
    uint8    sd_buf[8] = {0x7e, 0x04, 0x09, 0x01, 0x00, 0x00, 0x00, 0x7e};
    
    //���SD����д����״̬
    if (get_sd_wp_state())
    {
        sd_buf[5] = 0x5a;
    }
    
    //����Ƿ���SD��
    if (get_sd_state())
    {
        sd_buf[4] = 0x5c;
        get_cs(&sd_buf[1], 5, &sd_buf[6]);
        usb_write_buf(sd_buf, 8);
        goto NO_SD;
    }
    
    if (get_sd_ready())
    {
        //׼������
        sd_buf[4] = 0x5a;
    }
    else
    {
        //δ׼������
        sd_buf[4] = 0x5b;
        get_cs(&sd_buf[1], 5, &sd_buf[6]);
        usb_write_buf(sd_buf, 8);
        goto NO_SD;
    }
    
    get_cs(&sd_buf[1], 5, &sd_buf[6]);
    usb_write_buf(sd_buf, 8);
    send_sd_space();
    //��ѯSD�����ļ���Ϣ
    //  f_chdir("0:");
    get_file_info("0:");
    //  f_chdir("..");
    //���Ͳ�ѯ����֡
NO_SD:
    sd_buf[0] = 0x7e;
    sd_buf[2] = 0x09;
    sd_buf[1] = 0x02;
    sd_buf[3] = 0x05;
    sd_buf[4] = 0x45;
    sd_buf[5] = 0X7E;
    usb_write_buf(sd_buf, 6);
}
/***********************************************************************************************************************
*Function Nane     :  cmd_get_sd_file
*Description       :  ��ȡsd�ļ�����
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_get_sd_file(char *path)
{
    uint32 br;
    uint8 ret = 0;
    uint32 leftszie = 0;
    uint8  pathlen = 0;
    uint8 num = 0 ;
    uint8 buf[6] = {0x7E, 0x02, 0x0A, 0x04, 0X45, 0X7E};
    static uint8 flag = 0;
    
    if (NULL == path)
    {
        return ;
    }
    
    if (READY  == ptu.getfile.flag)
    {
        close_uart_irq();
        fsrc.buf = (uint8 *)mem_calloc(1, 512);
        //���ļ�
        ret = f_open(&fsrc, path, FA_READ | FA_OPEN_EXISTING);
        
        if (FR_OK != ret)
        {
            mem_free(fsrc.buf);
            usb_write_buf(buf, 6);
            return ;
        }
        
        flag = 1;
        //���͵�һ֡
        ptu_send.head  = 0x7e;
        ptu_send.cmd   = 0x0a;
        ptu_send.buf[0] = 0x01;
        ptu_send.buf[1] = 0x00;
        ptu_send.buf[2] = 0x00;
        //��ȫ�ļ�·��
        // f_getcwd(ptu.getfile.path, MAXPATH);
        //  strcat(ptu.getfile.path, path);
        pathlen = strlen(ptu.getfile.path);
        memcpy(&ptu_send.buf[3], ptu.getfile.path, pathlen);
        ptu_send.len   = 4 + pathlen;//�ֳ�
        //Ч����
        get_cs(&ptu_send.len, ptu_send.len + 1, &ptu_send.buf[ptu_send.len - 1]);
        ptu_send.buf[ptu_send.len] = 0x7e;
        usb_write_buf(&ptu_send.head, ptu_send.len + 4);
        ptu.getfile.flag  = GOING;
        ptu.getfile.index = 0;
        ptu.getfile.get_size = 0;
    }
    else if (GOING == ptu.getfile.flag)
    {
        //�����ļ�����֡
        ptu_send.head  = 0x7e;
        ptu_send.cmd   = 0x0a;
        ptu_send.buf[0] = 0x02;
        f_lseek(&fsrc, ptu.getfile.get_size);
        
        while (num < 10)
        {
            num ++;
            
            if (ptu.getfile.get_size < fsrc.fsize)
            {
                leftszie = fsrc.fsize - ptu.getfile.get_size;  //ʣ��������
                ptu.getfile.index++;                           //�����
                
                if (245 <= leftszie)
                {
                    f_read(&fsrc, &ptu_send.buf[3], 245, &br);
                    ptu.getfile.get_size += 245;
                    ptu_send.len = 4 + 245;
                }
                else
                {
                    f_read(&fsrc, &ptu_send.buf[3], leftszie, &br);
                    ptu.getfile.get_size = fsrc.fsize;
                    ptu_send.len = 4 + leftszie;
                }
                
                //���ݰ����
                ptu_send.buf[1] = ptu.getfile.index >> 8; //��λ
                ptu_send.buf[2] = ptu.getfile.index & 0xff; //��λ
                get_cs(&ptu_send.len, ptu_send.len + 1, &ptu_send.buf[ptu_send.len - 1]);
                ptu_send.buf[ptu_send.len] = 0x7e;
                usb_write_buf(&ptu_send.head, ptu_send.len + 4);
            }
            else
            {
                ptu.getfile.flag = FINISH;
                break;
            }
        }
    }
    else if (FINISH == ptu.getfile.flag)
    {
        f_close(&fsrc);
        mem_free(fsrc.buf);
        flag = 0;
        //�������ݽ���֡
        ptu_send.head  = 0x7e;
        ptu_send.cmd   = 0x0a;
        ptu_send.buf[0] = 0x03;
        ptu_send.buf[1] = 0x00;
        ptu_send.buf[2] = 0x00;
        pathlen = strlen(ptu.getfile.path);
        memcpy(&ptu_send.buf[3], ptu.getfile.path, pathlen);
        ptu_send.len   = 4 + pathlen;           //�ֳ�
        get_cs(&ptu_send.len, ptu_send.len + 1, &ptu_send.buf[ptu_send.len - 1]); //Ч����
        ptu_send.buf[ptu_send.len] = 0x7e;
        usb_write_buf(&ptu_send.head, ptu_send.len + 4);
        ptu.states = PTU_NORMAL;
        open_uart_irq();
    }
    else if (CANCEL == ptu.getfile.flag)
    {
        //ȡ����ȡ�ļ�
        if (1 == flag )
        {
            f_close(&fsrc);
            mem_free(fsrc.buf);
            flag = 0;
        }
        
        ptu.getfile.index = 0;
        ptu.getfile.get_size = 0;
        ptu.states = PTU_NORMAL;
        open_uart_irq();
        ptu_send.head = 0x7e;
        ptu_send.len = 2;
        ptu_send.cmd = CMD_CANCEL;
        ptu_send.buf[0] = 0x5a;
        ptu_send.buf[1] = 0xEE;
        ptu_send.buf[2] = 0x7e;
        usb_write_buf(&ptu_send.head, ptu_send.len + 4);
    }
}
/***********************************************************************************************************************
*Function Nane     : cmd_remove_file
*Description       :  ɾ���ļ�
*Input             :   path �ļ�·��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_remove_file(char *path)
{
    sd_remove_file(path);
}
/***********************************************************************************************************************
*Function Nane     :  cmd_reboot_system
*Description       :  ��λϵͳ
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_reboot_system(void)
{
    uint8 buf[6] = {0x7e, 0x01, 0x0e, 0x46, 0x7e};
    usb_write_buf(buf, 5);
    
    while (1)
    {
    }
}
/***********************************************************************************************************************
*Function Nane     :  cmd_usb_debug
*Description       :  ����usb��ӡ��Ϣ
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_usb_debug(uint8 type)
{
    switch (type)
    {
        case TCMS_DEBUG:
        {
            set_tcms_usb_debug();
        }
        break;
        
        case PISC_DEBUG:
        {
            set_pisc_usb_debug();
        }
        break;
        
        case TBA_DEBUG:
        {
            set_tba_usb_debug();
        }
        break;
        
        default:
            break;
    }
}
/***********************************************************************************************************************
*Function Nane     : cmd_heartbeat
*Description       :    ptu�����ź�
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void cmd_heartbeat(void)
{
    ptu.time = get_time();
}
/***********************************************************************************************************************
*Function Nane     :  send_error_info
*Description       :  ���ʹ�����Ϣ
*Input             :  type  ERR_NO_FILE ·������    ERR_USE_FILE �ļ�ʹ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void send_error_info(uint8 type)
{
    uint8 buf[2][6] = {0x7e, 0x02, 0x0a, 0x04, 0x45, 0x7e,
                       0x7e, 0x02, 0x0a, 0x05, 0x44, 0x7e
                      };
                      
    if (ERR_NO_FILE)
    {
        usb_write_buf(buf[0], 6);
    }
    else if (ERR_USE_FILE)
    {
        usb_write_buf(buf[1], 6);
    }
}
/***********************************************************************************************************************
*Function Nane     :  ptu_do_cmd
*Description       :  ʵ��PTU����
*Input             :  type  ��������
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
static void ptu_do_cmd(u8 type)
{
    // char  path[40]={0};
    uint8 buf[6] = {0x7e, 0x02, 0x0a, 0x06, 0x43, 0x7e};
    
    switch (type)
    {
        case CMD_UPGRADE:
        {
            //�����������
            cmd_upgrade(ptu_recv.buf[3], &ptu_recv.buf[1]);
        }
        break;
        
        case CMD_VERSION :
        {
            //��ѯ�汾
            cmd_version();
        }
        break;
        
        case CMD_SHOWNET:
        case CMD_SETNET:
        case CMD_SETMAC :
        {
        } break;
        
        case CMD_SETTIME:
        {
            //����ʱ��
            cmd_set_time(&ptu_recv.buf[3]);
        }
        break;
        
        case CMD_GETTIME:
        {
            //��ѯʱ��
            cmd_get_time() ;
        }
        break;
        
        case CMD_GETSTATE:
        {
            //��ѯ״̬
            cmd_get_state();
        }
        break;
        
        case CMD_SDINFO:
        {
            //��ѯsd����Ϣ
            cmd_get_sd_info();
        }
        break;
        
        case CMD_GETFILE:
        {
            //��ȡSD����ĳ���ļ�����
            if (get_sd_ready())
            {
                memset(ptu.getfile.path, '\0', 50);
                memcpy(ptu.getfile.path, &ptu_recv.buf[3], ptu_recv.buf[1] - 1);
                atc_tba.comm_flag = 0;
                atc_tms.comm_flag = 0;
                atc_pis.comm_flag = 0;
                
                if (get_tms_comm())
                {
                    usb_write_buf(buf, 6);
                    return;
                }
                
                if (0 == check_use_file(&ptu.getfile.path[3]))
                {
                    ptu.states = PTU_GETFILE;
                    ptu.getfile.flag = READY;
                    cmd_get_sd_file(ptu.getfile.path);
                }
                else
                {
                    send_error_info(ERR_USE_FILE);
                }
            }
            else
            {
            }
        } break;
        
        case CMD_CANCEL:
        {
            //ȡ����ȡ�ļ�����
            if (PTU_GETFILE == ptu.states)
            {
                ptu.getfile.flag = CANCEL;
            }
            else
            {
                ptu_send.head  = 0x7e;
                ptu_send.len   = 0x02;
                ptu_send.cmd   = 0x0b;
                ptu_send.buf[0] = 0x5b;
                ptu_send.buf[1] = 0xED;
                ptu_send.buf[2] = 0x7e;
                usb_write_buf(&ptu_send.head, 6);
            }
        }
        break;
#if 0
        
        case CMD_FORMATSD:
        {
            //��ʽ��SD��
            if (get_sd_ready())
            {
                cmd_format_sd();
                ptu_send.head  = 0x7e;
                ptu_send.len   = 0x01;
                ptu_send.cmd   = 0x0C;
                ptu_send.buf[0] = 0x48;
                ptu_send.buf[1] = 0x7E;
                usb_write_buf(&ptu_send.head, 5);
            }
        }
        break;
        
        case  CMD_ERMOVEFILE:
        {
            //ɾ���ļ�
            if (get_sd_ready())
            {
                memset(path, '\0', 40);
                memcpy(path, &ptu_recv.buf[3], ptu_recv.buf[1] - 1);
                
                if (0 == check_use_file(&path[3]))
                {
                    cmd_remove_file(path);
                }
                else
                {
                    send_error_info(ERR_USE_FILE);
                }
            }
        }
        break;
#endif
        
        case CMD_REBOOT:
        {
            //��λģ��
            open_uart_irq();
            cmd_reboot_system();
        }
        break;
        
        case CMD_USB_DEBUG:
        {
            cmd_usb_debug(ptu_recv.buf[3]);
        }
        break;
        
        case CMD_HEARTBEAT:
        {
            //����PTU�����ź�
            cmd_heartbeat();
        }
        break;
        
        default :
        {
            break;
        }
    }
}
/***********************************************************************************************************************
*Function Nane    :  ptu_transcod
*Description      :  ptu������ת�벢���У����
*Input            :  ��
*Output           :  ��
*Return           :  1:�ɹ� 0 :����
*Others           :  ��
***********************************************************************************************************************/
uint8 ptu_transcod(void)
{
    uint16 i = 0;
    uint16 j = 0;
    uint8 cs = 0;
    
    for (j = 1, i = 1; i <  ptu_recv.len - 1; i++)
    {
        ptu_recv.buf[j] = ptu_recv.buf[i];
        
        //ת��
        if (PTU_PACKET_DLE ==  ptu_recv.buf[i])
        {
            if (PTU_PACKET_DLE_TAB ==  ptu_recv.buf[i + 1])
            {
                ptu_recv.buf[j] = PTU_PACKET_TAB;
                i++;
            }
            else if (PTU_PACKET_DLE_DLE ==  ptu_recv.buf[i + 1])
            {
                ptu_recv.buf[j] = PTU_PACKET_DLE;
                i++;
            }
            else
            {
                ptu_recv.len = 0;
                return 0;
            }
        }
        
        j++;
    }
    
    ptu_recv.len = j;
    //�ж�У�����Ƿ���ȷ
    get_cs(&ptu_recv.buf[1],  ptu_recv.len - 2, &cs);
    
    if (cs ==  ptu_recv.buf[ ptu_recv.len - 1])
    {
        ptu_recv.len = 0;
        return 1;
    }
    else
    {
        ptu_recv.len = 0;
        return 0;
    }
}
/***********************************************************************************************************************
*Function Nane     :  ptu_get_packet
*Description       :  ����PTU���ݰ���������
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
static uint8 ptu_get_packet(void)
{
    static uint8 start_flag = 1;
    uint8  tmp = 0;
    static uint32 over_time = 0;
    
    while (usb_read_char(&tmp))
    {
        if (start_flag)
        {
            //�յ��ָ�������Ϊ֡ͷ
            if (tmp == PTU_PACKET_TAB)
            {
                ptu_recv.buf[0] = tmp;
                start_flag = 0;
                ptu_recv.len = 1;
                over_time = get_time();
            }
            else
            {
                ptu_recv.len = 0 ;
                start_flag = 1;
            }
        }
        else
        {
            //��������ֱ���ͻ�����
            ptu_recv.buf[ptu_recv.len ++] = tmp;
            
            if (PTU_BUF_LEN <= ptu_recv.len )
            {
                ptu_recv.len = 0;
                start_flag = 1;
                return 0;
            }
            
            //�����ָ��� �ж�
            if ((PTU_PACKET_TAB == tmp))
            {
                if (check_timer(&over_time, 1000))
                {
                    ptu_recv.buf[0] = tmp;
                    start_flag = 0;
                    ptu_recv.len = 1;
                }
                else
                {
                    start_flag = 1;
                    //�յ���������ת�벢���У����
                    return ptu_transcod();
                }
            }
        }
    }
    
    return 0;
}
/***********************************************************************************************************************
*Function Nane     :  ptu_proc
*Description       :  ����PTU����������
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
*********************************************************************************/
void ptu_proc(void)
{
    uint32 time = 0;
    
    //    uint16 i=0;
    if (1 == ptu_get_packet())
    {
        ptu_do_cmd(ptu_recv.buf[2]);
        time = get_time();
        ptu.comm_flag = 1;
        
        if (CMD_UPGRADE == ptu_recv.buf[2])
        {
            //�������ر�UART����
            close_uart_irq();
            upgrade_flag  = 0;
            
            while ((0 == upgrade_flag) && (0 == check_timer(&time, 300000)))
            {
                if (1 == ptu_get_packet())
                {
                    close_uart_irq();
                    time = get_time();
                    ptu_do_cmd(ptu_recv.buf[2]);
                }
                
                Delay(3);
            }
            
            //������ɣ���UART����
            
            if (ptu.states != PTU_GETFILE)
            {
                open_uart_irq();
                Delay(1);
            }
        }
    }
    
#if 0
    
    if (1 == ptu.comm_flag)
    {
        //usb��ͨ��
        if (check_timer(&ptu.time, 10000))
        {
            open_uart_irq();
            ptu.comm_flag = 0 ;
            set_tcms_debug(0);
            set_pis_debug(0);
            ptu.getfile.flag = CANCEL;
        }
    }
    
#endif
    
    switch (ptu.states)
    {
        case PTU_GETFILE:
        {
            sd_fflush(PIS, FRAMEERR);
            Delay(1);
            sd_fflush(PIS, FRAMEOK);
            Delay(1);
            sd_fflush(TMS, FRAMEERR);
            Delay(1);
            sd_fflush(TMS, FRAMEOK);
            Delay(1);
            sd_fflush(TBA, FRAMEERR);
            Delay(1);
            sd_fflush(TBA, FRAMEOK);
            Delay(1);
            cmd_get_sd_file(ptu.getfile.path);
        }
        break;
        
        default:
            break ;
    }
}


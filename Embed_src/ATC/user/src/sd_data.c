/************************************************************
  Copyright (C), 1988-1999, beihai Tech. Co., Ltd.
  FileName: info_save
  Author:
  Description:     SD���洢
  Version:
  History:
    <author>  <time>   <version >   <desc>
      bluck         2013-3-22    1.0     build this moudle
***********************************************************/

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <def.h>
#include "ff.h"
#include "mem.h"
#include "sd_data.h"
#include "usb.h"
#include "tcms.h"
#include "tba_bus.h"
//��������֡�洢·��
char filename[3][30] =
{
    {"140101/TCMS.TXT"},
    {"140101/PISC.TXT"},
    {"140101/TBA.TXT"},
};
//��������֡�洢·��
char errorfilename[3][30] =
{
    {"140101/ERRTCMS.TXT"},
    {"140101/ERRPISC.TXT"},
    {"140101/ERRTBA.TXT"},
};

extern tstate      atc_pis;
_tsd_file  sd_info[3];       //sd����֡�����ļ�����
_tsd_mode  sd_form[3];       //sd������֡���ݻ���

_tsd_file  sd_error_info[3]; //sd������֡�����ļ�����
_tsd_mode  sd_error_form[3]; //sd������֡���ݻ���
_tsd_mode_pis   sd_pis[3];
tRtcTimer old_time;          //��ʱ��
char      cur_dir[7];        //��ǰ�ļ���

char  file_buf[CHARFILEBUFLEN];//�ļ��ַ�������

FATFS fs;
uint8 sd_ready_flag = 0;     //sd׼��������־λ
void myitoa(uint8 *src, char *dest, uint16 len)
{
    uint16 i = 0;
    uint8  tmp = 0;
    
    for (i = 0; i < len; i++)
    {
        tmp = src[i] >> 4;
        
        if (tmp < 10)
        {
            *dest = tmp  + '0';
        }
        else
        {
            *dest = tmp - 10  + 'a';
        }
        
        dest++;
        tmp =  src[i] & 0xf;
        
        if (tmp < 10)
        {
            *dest = tmp  + '0';
        }
        else
        {
            *dest = tmp - 10  + 'a';
        }
        
        dest++;
    }
}

void myatoi(char *src, uint8 *dest, uint16 len)
{
    uint16 i = 0;
    uint8 tmp = 0;
    
    for (i = 0; i < len; i++)
    {
        if (('0' <= src[i]) && (src[i] <= '9'))
        {
            tmp = ((uint8)src[i] - '0') << 4;
        }
        else if (('a' <= src[i]) && (src[i] <= 'z'))
        {
            tmp = ((src[i] - 'a') + 10) << 4;
        }
        else if (('A' <= src[i]) && (src[i] <= 'Z'))
        {
            tmp = ((src[i] - 'A') + 10) << 4;
        }
        
        i++;
        
        if (('0' <= src[i]) && (src[i] <= '9'))
        {
            *dest = tmp + (src[i] - '0');
        }
        else if (('a' <= src[i]) && (src[i] <= 'z'))
        {
            *dest = tmp +  ((src[i] - 'a') + 10);
        }
        else if (('A' <= src[i]) && (src[i] <= 'Z'))
        {
            *dest = tmp + ((src[i] - 'A') + 10);
        }
        
        dest++;
    }
}

/***********************************************************************************************************************
*Function Nane    : get_use_file
*Description      : ����ļ��Ƿ���ʹ����
*Input            :  path �ļ�·��
*Output           :  ��
*Return           :  1 ʹ���� 0  ������
*Others           :  ��
***********************************************************************************************************************/
uint8  check_use_file(char *path)
{
    if ((0 == strcmp(filename[0], path)) && (get_tms_comm()))
    {
        return 1;
    }
    
    if ((0 == strcmp(errorfilename[0], path)) && (get_tms_comm()))
    {
        return 1;
    }
    
    if ((0 == strcmp(filename[1], path)) && (get_pis_comm()))
    {
        return 1;
    }
    
    if ((0 == strcmp(errorfilename[1], path)) && (get_pis_comm()))
    {
        return 1;
    }
    
    if ((0 == strcmp(filename[2], path)) && (get_train485_comm()))
    {
        return 1;
    }
    
    if ((0 == strcmp(errorfilename[2], path)) && (get_train485_comm()))
    {
        return 1;
    }
    
    return 0;
}
/***********************************************************************************************************************
*Function Nane    :  sd_hw_init
*Description      :  sd��Ӳ���ڳ�ʼ��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
static  void  sd_hw_init(void)
{
    static GPIO_InitTypeDef  my_GPIO_InitStructure;//for device address// Added by Lison on 2013-2-26 9:10:30
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    my_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    my_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    my_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    my_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    my_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &my_GPIO_InitStructure);
    my_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    my_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &my_GPIO_InitStructure);
}
/***********************************************************************************************************************
*Function Nane     :  get_sd_wp_state
*Description       :  ��ȡsd��д����״̬
*Input             :  ��
*Output            :  ��
*Return            :  0 �ޱ��� 1 д����
*Others            :  ��
***********************************************************************************************************************/
uint8 get_sd_wp_state(void)
{
    return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1);
}
/***********************************************************************************************************************
*Function Nane     :  get_sd_state
*Description       :  ��ȡsd���Ƿ����
*Input             :  ��
*Output            :  ��
*Return            :  0  ���� 1 û�в���
*Others            :  ��
***********************************************************************************************************************/
uint8 get_sd_state(void)
{
    return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0);
}
/***********************************************************************************************************************
*Function Nane    :  get_sd_ready
*Description      :  ��ȡsd�Ƿ�׼������
*Input            :  ��
*Output           :  ��
*Return           :  0  û��׼������ 1 ׼������
*Others           :  ��
***********************************************************************************************************************/
uint8 get_sd_ready(void)
{
    return sd_ready_flag;
}
/***********************************************************************************************************************
*Function Nane    :  update_dir
*Description      :  ���´洢Ŀ¼
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void update_dir(void)
{
    DIR dir;
    tRtcTimer time;
    char path[6] = {0};
    uint8 i = 0;
    uint8 ret = 0;
    
    if (0 == get_sd_ready())
    {
        return ;
    }
    
    get_cur_time(&time);
    path[0] = time.year / 10 + '0';
    path[1] = time.year % 10 + '0';
    path[2] = time.month / 10 + '0';
    path[3] = time.month % 10 + '0';
    path[4] = time.date / 10 + '0';
    path[5] = time.date % 10 + '0';
    
    /*���ʱ����գ�������ӦĿ¼�������ĵ�ǰ�洢·��*/
    if (old_time.date != time.date)
    {
        ret = f_opendir(&dir, path);
        
        if (FR_NO_PATH == ret)
        {
            if (FR_OK == f_mkdir(path))
            {
                memcpy(cur_dir, path, 6);
                memcpy(&old_time.sec, &time.sec, sizeof(tRtcTimer));
                
                for (i = 0; i < 3; i++)
                {
                    strncpy(filename[i], cur_dir, 6);
                    strncpy(errorfilename[i], cur_dir, 6);
                }
            }
        }
        else if (FR_OK == ret)
        {
            memcpy(cur_dir, path, 6);
            memcpy(&old_time.sec, &time.sec, sizeof(tRtcTimer));
            
            for (i = 0; i < 3; i++)
            {
                strncpy(filename[i], cur_dir, 6);
                strncpy(errorfilename[i], cur_dir, 6);
            }
        }
    }
}
/***************************************************************************************
*Function Nane    : check_cur_dir
*Description      : ��鵱ǰĿ¼
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
*****************************************************************************************/
void check_cur_dir(void)
{
    DIR dir;
    tRtcTimer time;
    char path[6] = {0};
    uint8 i = 0;
    uint8 ret = 0;
    get_cur_time(&time);
    path[0] = time.year / 10 + '0';
    path[1] = time.year % 10 + '0';
    path[2] = time.month / 10 + '0';
    path[3] = time.month % 10 + '0';
    path[4] = time.date / 10 + '0';
    path[5] = time.date % 10 + '0';
    memcpy(&old_time.sec, &time.sec, sizeof(tRtcTimer));
    ret = f_opendir(&dir, path);
    
    if (FR_NO_PATH == ret)
    {
        if (FR_OK == f_mkdir(path))
        {
            memcpy(cur_dir, path, 6);
            
            for (i = 0; i < 3; i++)
            {
                strncpy(filename[i], cur_dir, 6);
                strncpy(errorfilename[i], cur_dir, 6);
            }
        }
    }
    else if (FR_OK == ret)
    {
        memcpy(cur_dir, path, 6);
        
        for (i = 0; i < 3; i++)
        {
            strncpy(filename[i], cur_dir, 6);
            strncpy(errorfilename[i], cur_dir, 6);
        }
    }
}
/***********************************************************************************************************************
*Function Nane    :  sd_init
*Description      :  sd����ʼ��
*Input            :  ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void sd_init(void)
{
    u8 ret = 0;
    //sd��Ӳ����ʼ��
    sd_hw_init();
    Delay(1);
    strcpy(cur_dir, "140201");
    sd_ready_flag = 0;
    
    //���sd���Ƿ����
    if (get_sd_state())
    {
        return ;
    }
    
    //ע�Ṥ����
    fs.win = (uint8 *)mem_calloc(1, 1024);
    ret = f_mount(0, &fs);
    
    if (FR_OK != ret)
    {
        mem_free(fs.win);
        return ;
    }
    
    check_cur_dir();
    sd_ready_flag = 1; //sd��׼������
}
/***************************************************************************************
*Function Nane   : check_sd_state
*Description     : ���sd ��״̬
*Input           :  path �ļ�·��
*Output          :  ��
*Return          :  ��
*Others          :  ��
*****************************************************************************************/
void check_sd_state(void)
{
    uint8 ret = 0;
    
    if (get_sd_state())
    {
        if (sd_ready_flag)
        {
            //sd�γ����ͷſռ�
            f_mount(0, NULL);
            mem_free(fs.win);
            sd_ready_flag = 0;
        }
    }
    else
    {
        if (0 == sd_ready_flag)
        {
            sd_init();//֧���Ȳ��
            //sd���룬����ռ䣬ע�Ṥ����
            fs.win = (uint8 *)mem_calloc(1, 1024);
            ret = f_mount(0, &fs);
            
            if (FR_OK != ret)
            {
                mem_free(fs.win);
                return ;
            }
            
            check_cur_dir();
            sd_ready_flag = 1;
        }
    }
}
/***************************************************************************************
*Function Nane     : remove_file
*Description       : ɾ���ļ�
*Input             :  path �ļ�·��
*Output            :  ��
*Return            :  ��
*Others            :  ��
*****************************************************************************************/
static void remove_file(char *path)
{
    DIR dir;
    FILINFO dirinfo;
    
    if (FR_OK == f_opendir(&dir, path))
    {
        while (FR_OK == f_readdir(&dir, &dirinfo))
        {
            if (!dirinfo.fname[0])
            {
                return ;
            }
            
            if (!(strcmp(dirinfo.fname, ".")) || (!(strcmp(dirinfo.fname, ".."))))
            {
                continue;
            }
            
            //Ŀ¼�ļ�
            if (AM_DIR == dirinfo.fattrib)
            {
                if (!(strcmp(dirinfo.fname, ".")) || (!(strcmp(dirinfo.fname, ".."))))
                {
                    continue ;
                }
                
                //�����Ŀ¼
                f_chdir(dirinfo.fname);
                remove_file(".");
                //������һ��Ŀ¼
                f_chdir("..");
            }
            
            f_unlink(dirinfo.fname);
        }
    }
}
/***************************************************************************************
*Function Nane  : sd_remove_file
*Description    : ɾ���ļ�
*Input          :  path �ļ�·��
*Output         :  ��
*Return         :  1�ɹ� 0 ʧ��
*Others         :  ��
*****************************************************************************************/
uint8 sd_remove_file(char *path)
{
    FILINFO dirinfo;
    uint8 ret = 0;
    ret = f_stat(path, &dirinfo);
    
    if (FR_OK != ret)
    {
        return 0;
    }
    
    //Ŀ¼�ļ��� ����Ŀ¼��ɾ����Ŀ¼�������ļ�������ɾ����Ŀ¼
    if (AM_DIR == dirinfo.fattrib)
    {
        if (FR_OK == f_chdir(path))
        {
            remove_file(".");
            f_chdir("..");
            f_unlink(path);
        }
    }
    else
    {
        f_unlink(path); //��ͨ�ļ���ֱ��ɾ��
    }
    
    return 0;
}
/***************************************************************************************
*Function Nane   :  get_sd_space
*Description     :  ��ȡSD���ռ���Ϣ
*Input           :  ��
*Output          :  total �ܿռ�         ��λ(KB)
                    free   ʣ��ռ�   ��λ(KB)
*Return          :  1 �ɹ� 0  ʧ��
*Others          :  ��
*****************************************************************************************/
uint8 get_sd_space(uint32 *total, uint32 *free)
{
    unsigned long num_clues = 0;
    FATFS  *pfs = NULL;
    FATFS  fs;
    uint8 ret = 0;
    pfs = &fs;
    fs.win = (uint8 *) mem_calloc(1, 256);
    ret = f_getfree("0:", &num_clues, &pfs);
    
    if (FR_OK != ret)
    {
        mem_free(fs.win);
        return 0;
    }
    
    // ʣ��ռ�=
    // ʣ�����* ÿ��������*ÿ�����ֽ���
    *free   = num_clues * pfs->csize / 2;
    *total = (pfs->n_fatent - 2) * pfs->csize / 2;
    mem_free(fs.win);
    return 1;
}
/***************************************************************************************
*Function Nane   :  get_dir_size
*Description     :  ��ȡĿ¼�ļ��Ĵ�С
*Input           :  path ·��
*Output          :  ��
*Return          :  �ļ���С
*Others          :  ��
*****************************************************************************************/
uint32 get_dir_size(char *path)
{
    DIR dir;
    FILINFO dirinfo;
    uint32 size = 0;
    
    //��Ŀ¼
    if (FR_OK != f_opendir(&dir, path))
    {
        return 0;
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
        
        if (AM_DIR == dirinfo.fattrib)
        {
            f_chdir(dirinfo.fname);
            size += get_dir_size(".");
            f_chdir("..");
        }
        else
        {
            size += dirinfo.fsize;
        }
    }
    
    return size;
}
/***************************************************************************************
*Function Nane   :  delete_file_format
*Description     :  ɾ���ļ���ǰ����
*Input           :  ��
*Output          :  path ·��
                        pos  ɾ����ֹλ��(��λ:�ֽ�)
*Return          :  ��
*Others          :  ��
*****************************************************************************************/
static void delete_file_format(char *path, uint32 pos)
{
    FIL fsrc;
    FIL ftmp;
    uint32 sbr = 0;
    uint32 tbr = 0;
    uint8  buf[512] = {0};
    uint8 ret = 0 ;
    
    if (0 == pos)
    {
        return;
    }
    
    fsrc.buf = (uint8 *)mem_calloc(1, 1024);
    ftmp.buf = (uint8 *)mem_calloc(1, 1024);
    //��ԭ�ļ�
    ret = f_open(&fsrc, path, FA_OPEN_EXISTING | FA_READ );
    
    if (FR_OK != ret)
    {
        mem_free(ftmp.buf);
        mem_free(fsrc.buf);
        return ;
    }
    
    //���������ļ�
    ret = f_open(&ftmp, "backup.txt", FA_CREATE_ALWAYS | FA_WRITE);
    
    if (FR_OK != ret)
    {
        f_close(&fsrc);
        mem_free(ftmp.buf);
        mem_free(fsrc.buf);
        return ;
    }
    
    //ƫ���ļ�λ��
    ret  = f_lseek(&fsrc, pos);
    ret  = f_lseek(&ftmp, 0);
    sbr = 512;
    
    //��ȡԭ�ļ� д�뱸���ļ�
    while (sbr == 512)
    {
        if (FR_OK != f_read(&fsrc, buf, 512, &sbr))
        {
            break;
        }
        
        if (FR_OK != f_write(&ftmp, buf, sbr,  &tbr))
        {
            break;
        }
    }
    
    f_close(&ftmp);
    f_close(&fsrc);
    mem_free(ftmp.buf);
    mem_free(fsrc.buf);
    //ɾ��ԭ�ļ����ѱ����ļ����ָĳ�ԭ�ļ���
    sd_remove_file(path);
    f_rename("backup.txt", path);
}

int ascii_to_integer( char *string )
{
    int value = 0;
    
    // ������ַ������ַ�ת��Ϊ���֡�
    while (*string >= '0' && *string <= '9')
    {
        value *= 10;
        value += *string - '0';
        string++;
    }
    
    //�����飺�����������һ���������ַ�����ֹ���ѽ������Ϊ0
    if (*string != '\0')
    {
        value = 0;
    }
    
    return value;
}




/***********************************************************************************************************************
*Function Nane    :  sd_clear_space
*Description      :  ����SD�ռ�
*Input            :   free sd��ʣ��ռ�(��λ:kb)
*Output           :  ��
*Return           :  ��
*Others           :  ��
*************************************************************************/
#if 1    //��Ϊ��ʱ��ɾ����ֻ����һ�����ڵ�����
static void sd_clear_space(uint32 free)
{
    DIR dir;
    FILINFO dirinfo;
    FILINFO olddirinfo;
    uint32 size = 0;
    uint8  flag = 0;
    uint8  num = 0;
    tRtcTimer time;
    char time_name[13] = {0}; //ʱ���ļ���
    char file_name[13] = {0}; //Ŀ¼�ļ���
    int  time_new_num = 0;
    int  time_old_num = 0;
    int tmp_num = 0;
    //strcpy(olddirinfo.fname, "999999");
    get_cur_time(&time);
    time_name[0] = time.year / 10 + '0';
    time_name[1] = time.year % 10 + '0';
    time_name[2] = time.month / 10 + '0';
    time_name[3] = time.month % 10 + '0';
    time_name[4] = time.date / 10 + '0';
    time_name[5] = time.date % 10 + '0';
    time_new_num = ascii_to_integer(time_name);
    
    //��ȡ�ļ���Ϣ
    if (FR_OK != f_opendir(&dir, "0:"))
    {
        return ;
    }
    
    //��ȡ������С���ļ���
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
            for (num = 0; num < 6; num++)
            {
                file_name[num] = dirinfo.fname[num];
            }
            
            time_old_num = ascii_to_integer(file_name);
            
            //strcpy(file_name,dirinfo.fname);
            if (0x07 < time_new_num - time_old_num)
            {
                memcpy(&olddirinfo, &dirinfo, sizeof(FILINFO));
                flag = 1;
            }
        }
    }
    
    if (0 == flag)
    {
        return ;
    }
    
    size = get_dir_size(olddirinfo.fname);
    
    //��Ŀ¼С��ʣ��ռ���Сֵ ,ֱ��ɾ��
    if (size < (FREESPACE * 1024))
    {
        sd_remove_file(olddirinfo.fname);
    }
    else
    {
        //��Ŀ¼����ʣ��ռ���Сֵ��ɾ����Ŀ¼����ļ���ǰ����
        f_chdir(olddirinfo.fname);
        f_opendir(&dir, ".");
        olddirinfo.fsize = 0;
        
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
            
            if (AM_DIR != dirinfo.fattrib)
            {
                //��¼�ļ������ļ���Ϣ
                if (olddirinfo.fsize < dirinfo.fsize)
                {
                    memcpy(&olddirinfo, &dirinfo, sizeof(FILINFO));
                }
            }
        }
        
        //ɾ�����ļ���ǰ�벿��
        size = olddirinfo.fsize / 2 + 1;
        
        if (free < (size / 1024))
        {
            size = (olddirinfo.fsize / 1024 - free + 1) * 1024 ;
        }
        
        delete_file_format(olddirinfo.fname, size);
        f_chdir("..");
    }
}
#else

static void sd_clear_space(uint32 free)
{
    DIR dir;
    FILINFO dirinfo;
    FILINFO olddirinfo;
    uint32 size = 0;
    uint8  flag = 0;
    strcpy(olddirinfo.fname, "999999");

    //��ȡ�ļ���Ϣ
    if (FR_OK != f_opendir(&dir, "0:"))
    {
        return ;
    }

    //��ȡ������С���ļ���
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
            if (0 < strcmp(olddirinfo.fname, dirinfo.fname))
            {
                memcpy(&olddirinfo, &dirinfo, sizeof(FILINFO));
                flag = 1;
            }
        }
    }

    if (0 == flag)
    {
        return ;
    }

    size = get_dir_size(olddirinfo.fname);

    //��Ŀ¼С��ʣ��ռ���Сֵ ,ֱ��ɾ��
    if (size < (FREESPACE * 1024))
    {
        sd_remove_file(olddirinfo.fname);
    }
    else
    {
        //��Ŀ¼����ʣ��ռ���Сֵ��ɾ����Ŀ¼����ļ���ǰ����
        f_chdir(olddirinfo.fname);
        f_opendir(&dir, ".");
        olddirinfo.fsize = 0;

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

            if (AM_DIR != dirinfo.fattrib)
            {
                //��¼�ļ������ļ���Ϣ
                if (olddirinfo.fsize < dirinfo.fsize)
                {
                    memcpy(&olddirinfo, &dirinfo, sizeof(FILINFO));
                }
            }
        }

        //ɾ�����ļ���ǰ�벿��
        size = olddirinfo.fsize / 2 + 1;

        if (free < (size / 1024))
        {
            size = (olddirinfo.fsize / 1024 - free + 1) * 1024 ;
        }

        delete_file_format(olddirinfo.fname, size);
        f_chdir("..");
    }
}



#endif
/***********************************************************************************************************************
*Function Nane    : check_file_size
*Description      :  ����ļ��Ĵ�С��������ɾ���ļ�ͷ������
*Input            :  path �ļ�·��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
static void check_file_size(char *path)
{
    uint32 size = 0;
    FILINFO fileinfo;
    uint32 total = 0;
    uint32 free = 0;
    
    if (FR_OK != f_stat(path, &fileinfo))
    {
        return ;
    }
    
    if (0 == get_sd_space(&total, &free))
    {
        return ;
    }
    
    //�ļ�������ɾ���ļ�ͷ������
    if ((FILEMAXSIZE * 1024) < fileinfo.fsize)
    {
        size = fileinfo.fsize - FILEMAXSIZE * 1024;
        
        if (free < FILEMAXSIZE)
        {
            size = ((fileinfo.fsize / 1024) - free + 1) * 1024;
        }
        
        delete_file_format(path, size);
    }
}
/***********************************************************************************************************************
*Function Nane    :  check_sd_space
*Description      :  ���sd���������Ƿ�����Ҫ��
*Input            : ��
*Output           :  ��
*Return           :  ��
*Others           :  ��
***********************************************************************************************************************/
void check_sd_space(void)
{
    uint32 total = 0;
    uint32 free = 0;
    //uint8 num =0;
    
    if (0 == get_sd_space(&total, &free))
    {
        return ;
    }
    
#if 0
    
    //�ռ䲻�㣬 �����ϵ����ݽ��д���
    while ((free < FREESPACE) && (num < 20))
    {
        sd_clear_space(free);
        get_sd_space(&total, &free);
        num++;
    }
    
#else
    
    while (free < FREESPACE)
    {
        sd_clear_space(free);
    }
    
#endif
}
/***********************************************************************************************************************
*Function Nane      :  save_data_to_sd
*Description        :  ��SD���洢��Ϣ
*Input              :  filp  �ļ���
                       buf  ����
                       len  ���ݳ���
*Output             :  ��
*Return             :  ��
*Others             :  ��
***********************************************************************************************************************/
static  void save_data_to_sd(char *path, uint8 *buf, uint16 len)
{
#if 1
    FIL fsrc;
    uint32 br;
    FRESULT res;
    uint16 size  = 0;
    
    if ((0 == len) || (0 == sd_ready_flag))
    {
        return;
    }
    
    //check_file_size(path);
    //���sd��ʣ��ռ�
    check_sd_space();
    fsrc.buf = (uint8 *)mem_calloc(1, FILEBUFLEN);
    //���ļ�
    res = f_open(&fsrc, path, FA_OPEN_ALWAYS | FA_WRITE);
    
    if (res != FR_OK)
    {
        res = f_open(&fsrc, path, FA_CREATE_NEW | FA_WRITE);
    }
    if (res != FR_OK)
    {
        mem_free(fsrc.buf );
        f_mount(0, NULL);
        mem_free(fs.win); //sd�γ����ͷſռ�
        sd_ready_flag = 0;
        return;
    }
    
    //ƫ���ļ�λ��
    res = f_lseek(&fsrc, fsrc.fsize);
    
    if (res != FR_OK)
    {
        mem_free(fsrc.buf );
        f_mount(0, NULL);
        mem_free(fs.win); //sd�γ����ͷſռ�
        sd_ready_flag = 0;
        return;
    }
    
    //д����
    while (size < len)
    {
        if (512 <= len - size)
        {
            f_write(&fsrc, buf, 512, &br);
            buf += 512;
            size += 512;
        }
        else
        {
            f_write(&fsrc, buf, (len - size), &br);
            size = len;
        }
    }
    
    if (res != FR_OK)
    {
        mem_free(fsrc.buf );
        f_mount(0, NULL);
        mem_free(fs.win); //sd�γ����ͷſռ�
        sd_ready_flag = 0;
        return;
    }
    
    //�ر��ļ�
    res = f_close(&fsrc);
    
    if (res != FR_OK)
    {
        mem_free(fsrc.buf );
        f_mount(0, NULL);
        mem_free(fs.win); //sd�γ����ͷſռ�
        sd_ready_flag = 0;
        return;
    }
    
    mem_free(fsrc.buf);
#endif
}

void SD_LED(void)
{
#if 1
    static uint32 led_timer = 0;
    static uint8  led_num = 0;
    
    if ((get_sd_state() || sd_ready_flag == 0))
    {
        gpio_485_disable(LED4);//�޿�
    }
    
    if (get_sd_state() == 0 && (atc_pis.comm_flag == 1) && sd_ready_flag == 1)
    {
        if (check_timer(&led_timer, 100))//���е�
        {
            gpio_485_enable(LED4);
            
            if (2 < led_num )
            {
                gpio_485_disable(LED4);
                led_num = 0;
            }
            
            led_num++;
        }
    }
    
    if (atc_pis.comm_flag == 0)
    {
        if (check_timer(&led_timer, 500))//���е�
        {
            gpio_485_enable(LED4);
            
            if (2 < led_num )
            {
                gpio_485_disable(LED4);
                led_num = 0;
            }
            
            led_num++;
        }
    }
    
#endif
}


#if SD_SAVE_WAY
static char mvb_data_path[100] = "0:141009\\mvb08.txt";
static char pis_data_path[100] = "0:141009\\pis08.txt";
static char oplog_data_path[100] = "0:141009\\log.txt";
void update_rec_data_dir(void)
{
    DIR dir;
    tRtcTimer timer, *time = NULL;
    char path[6] = {0};
    uint8 i = 0;
    uint8 ret = 0;
    
    if (0 == get_sd_ready())
    {
        return ;
    }
    
    time = &timer;
    get_cur_time(&timer);
    path[0] = time->year / 10 + '0';
    path[1] = time->year % 10 + '0';
    path[2] = time->month / 10 + '0';
    path[3] = time->month % 10 + '0';
    path[4] = time->date / 10 + '0';
    path[5] = time->date % 10 + '0';
    
    for (i = 0; i < 6; i++)
    {
        mvb_data_path[2 + i] = path[i];
        pis_data_path[2 + i] = path[i];
        oplog_data_path[2 + i] = path[i];
    }
    
    mvb_data_path[12] = time->hour / 10 + '0';
    pis_data_path[12] = time->hour / 10 + '0';
    mvb_data_path[13] = time->hour % 10 + '0';
    pis_data_path[13] = time->hour % 10 + '0';
    
    /*���ʱ����գ�������ӦĿ¼�������ĵ�ǰ�洢·��*/
    if (old_time.hour != time->hour)
    {
        ret = f_opendir(&dir, path);
        
        if (FR_NO_PATH == ret)
        {
            if (FR_OK == f_mkdir(path))
            {
                memcpy(cur_dir, path, 6);
                memcpy(&old_time.sec, &time->sec, sizeof(tRtcTimer));
                
                for (i = 0; i < 3; i++)
                {
                    strncpy(filename[i], cur_dir, 6);
                    strncpy(errorfilename[i], cur_dir, 6);
                }
            }
        }
        else if (FR_OK == ret)
        {
            memcpy(cur_dir, path, 6);
            memcpy(&old_time.sec, &time->sec, sizeof(tRtcTimer));
            
            for (i = 0; i < 3; i++)
            {
                strncpy(filename[i], cur_dir, 6);
                strncpy(errorfilename[i], cur_dir, 6);
            }
        }
    }
}

uint16 hex_to_ascii(uint8 *par_des, uint8 *par_src, uint16 par_length)
{
    uint16 i;
    uint16 tmp_length = 0;
    const uint8 code_table[] = "0123456789ABCDEF";
    
    for (i = 0; i < par_length; i++, tmp_length += 3)
    {
        *par_des = code_table[(*par_src >> 4) & 0x0f];
        par_des++;
        *par_des = code_table[*par_src & 0x0f];
        par_des++;
        *par_des = ' ';
        par_des++;
        par_src++;
    }
    
    *par_des = '\r';
    par_des++;
    *par_des = '\n';
    par_des++;
    *par_des = '\r';
    par_des++;
    *par_des = '\n';
    par_des++;
    tmp_length += 4;
    return tmp_length;
}

static char const rec_head[][30] =
{
    "[REC MVB 2014-10-14 12:43:59] ",
    "[SND MVB 2014-10-14 12:43:59] ",
    "[REC PIS 2014-10-14 12:43:59] ",
    "[SND PIS 2014-10-14 12:43:59] ",
    "[OPR LOG 2014-10-14 12:43:59] "
};
void save_log_data(uint8 par_type, uint8 *par_data, uint16 par_length)
{
    uint8 i;
    tRtcTimer timer;
    uint16 tmp_length;
    uint8  tmp_buf[1000];
    get_cur_time(&timer);
    
    for (i = 0; i < 30; i++)
    {
        tmp_buf[i] = rec_head[par_type][i];
    }
    
    tmp_length = 30;
    tmp_buf[11] = 0x30 + timer.year / 10;
    tmp_buf[12] = 0x30 + timer.year % 10;
    tmp_buf[14] = 0x30 + timer.month / 10;
    tmp_buf[15] = 0x30 + timer.month % 10;
    tmp_buf[17] = 0x30 + timer.date / 10;
    tmp_buf[18] = 0x30 + timer.date % 10;
    tmp_buf[20] = 0x30 + timer.hour / 10;
    tmp_buf[21] = 0x30 + timer.hour % 10;
    tmp_buf[23] = 0x30 + timer.min / 10;
    tmp_buf[24] = 0x30 + timer.min % 10;
    tmp_buf[26] = 0x30 + timer.sec / 10;
    tmp_buf[27] = 0x30 + timer.sec % 10;
    tmp_length += hex_to_ascii(&tmp_buf[30], par_data, par_length);
    
    if ( (par_type == 0) || (par_type == 1) )
    {
        save_data_to_sd(mvb_data_path, tmp_buf, tmp_length);
    }
    else if ( (par_type == 2) || (par_type == 3) )
    {
    	save_data_to_sd(pis_data_path, tmp_buf, tmp_length);
    }
    else if (par_type == 4)
    {
    	save_data_to_sd(oplog_data_path, tmp_buf, tmp_length);
    }
}
#endif

/***********************************************************************************************************************
*Function Nane     :  save_start_time
*Description       :  ��¼ϵͳ����ʱ��
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
#if SD_SAVE_WAY
void save_start_time(void)
{
    tRtcTimer timer;
    uint8 tmp_buf = 0;
    get_cur_time(&timer);
    update_rec_data_dir();
    save_log_data(OP_LOG, &tmp_buf, 0);
}
#else
void save_start_time(void)
{
    uint8 start_buf[8] = {0x68, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    tRtcTimer timer;
    get_cur_time(&timer);
    memcpy(&start_buf[2], &timer.sec, 6);
    save_data_to_sd("0:Time.txt", start_buf, 8);
}
#endif

/***********************************************************************************************************************
*Function Nane     : save_change_time
*Description       :  ��¼ϵͳ�޸�ʱ��
*Input             :  oldtimer   ��ʱ��
                         newtimer ��ʱ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void save_change_time(tRtcTimer oldtimer, tRtcTimer newtimer)
{
    uint8 start_buf[16] = {0x68, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x68, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                          };
    memcpy(&start_buf[2], &oldtimer.sec, 6);
    memcpy(&start_buf[10], &newtimer.sec, 6);
    save_data_to_sd("0:Time.txt", start_buf, 16);
}

/***********************************************************************************************************************
*Function Nane     :  save_normal_data
*Description       :  ��¼���յ�������
*Input             :   type ���� 0 : tms  1: pis  2:train485
                           buf  ����
                           len  ���ݳ���
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
static void save_normal_data(uint8 type, uint8 *buf, uint16 len)
{
    uint16 size = 0;
    
    if (( 0 == len) || (0 == sd_ready_flag))
    {
        return ;
    }
    
    size = sd_info[type].len + len;
#if 1
    
    if (FILEBUFLEN <= size)
    {
        size = FILEBUFLEN - sd_info[type].len;
        memcpy(&sd_info[type].buf[sd_info[type].len], buf, size);
        //save_data_to_sd(filename[type], sd_info[type].buf, FILEBUFLEN);
#if 1
        
        if (type == 0)
        {
            save_data_to_sd(mvb_data_path, sd_info[type].buf, FILEBUFLEN);
        }
        
        if (type == 1)
        {
            save_data_to_sd(pis_data_path, sd_info[type].buf, FILEBUFLEN);
        }
        
#endif
        sd_info[type].len = len - size;
        memcpy(&sd_info[type].buf[0], &buf[size], sd_info[type].len);
    }
    else
    {
        memcpy(&sd_info[type].buf[sd_info[type].len], buf, len);
        sd_info[type].len += len ;
    }
    
#endif
}









static void save_normal_data_pis(uint8 type, uint8 *buf, uint16 len)
{
    uint16 size = 0;
    
    if (( 0 == len) || (0 == sd_ready_flag))
    {
        return ;
    }
    
    size = sd_info[type].len + len;
    
    if (FILEBUFLEN <= size)
    {
        size = FILEBUFLEN - sd_info[type].len;
        memcpy(&sd_info[type].buf[sd_info[type].len], buf, size);
        save_data_to_sd(filename[type], sd_info[type].buf, FILEBUFLEN);
        sd_info[type].len = len - size;
        memcpy(&sd_info[type].buf[0], &buf[size], sd_info[type].len);
    }
    else
    {
        memcpy(&sd_info[type].buf[sd_info[type].len], buf, len);
        sd_info[type].len += len ;
    }
}

/***********************************************************************************************************************
*Function Nane     :  form_normal_frame
*Description       :  ��֯д��SD��������
*Input             :   type ����         0 : tms       1: pis      2:train485
                      mode ʱ���  1 : ��ʼ  �м�  2:����
                      buf  ����
                      len  ���ݳ���
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
/*void form_normal_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len)
{
    if(0 == sd_ready_flag)
    {
        return;
    }

    if (1 == mode)
    {
        sd_form[type].head = 0x68;
        sd_form[type].star_time.sec=  time.sec;
        sd_form[type].star_time.min=  time.min;
        sd_form[type].star_time.hour= time.hour;
        sd_form[type].rhead = 0x68;
        memcpy(sd_form[type].buf, buf, len);
        sd_form[type].len = len;
    }
    else if(2 == mode)
    {
        sd_form[type].stop_time.sec=  time.sec;
        sd_form[type].stop_time.min=  time.min;
        sd_form[type].stop_time.hour= time.hour;
        sd_form[type].num= len;
        //save_normal_data(type, &sd_form[type].head , 10);
        //save_normal_data(type, sd_form[type].buf , sd_form[type].len);
    memcpy(sd_info[type].buf,&sd_form[type].head,10);
    //memcpy(&sd_info[type].buf[10],sd_form[type].buf,TMS_FRAME_LEN);
    //sd_info[type].len = 10+TMS_FRAME_LEN;
//if(type == PIS ){
//      memcpy(&sd_info[type].buf[10],sd_form[type].buf,PIS_FRAME_LEN_CHANGE);
    //  sd_info[type].len = 10+ PIS_FRAME_LEN_CHANGE;
    //  sd_fflush(PIS, FRAMEOK);
//  }
    //else if(type == TMS ){
        if(type == TMS ){
        memcpy(&sd_info[type].buf[10],sd_form[type].buf,TMS_FRAME_LEN);
        sd_info[type].len = 10+TMS_FRAME_LEN;
        sd_fflush(TMS, FRAMEOK);}


    }
}

*/


void form_normal_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len)
{
    if (0 == sd_ready_flag)
    {
        return;
    }
    
    if (1 == mode)
    {
        sd_form[type].head = 0x68;
        sd_form[type].star_time.sec =  time.sec;
        sd_form[type].star_time.min =  time.min;
        sd_form[type].star_time.hour = time.hour;
        sd_form[type].rhead = 0x68;
        memcpy(sd_form[type].buf, buf, len);
        sd_form[type].len = len;
    }
    else if (2 == mode)
    {
        sd_form[type].stop_time.sec =  time.sec;
        sd_form[type].stop_time.min =  time.min;
        sd_form[type].stop_time.hour = time.hour;
        sd_form[type].num = len;
        save_normal_data(type, &sd_form[type].head , 10);
        save_normal_data(type, sd_form[type].buf , sd_form[type].len);
    }
    else if (3 == mode)
    {
#if 1
        save_data_to_sd(filename[type], buf, len);
#endif
#if 0
        sd_form[type].head = 0x68;
        sd_form[type].star_time.sec =  time.sec;
        sd_form[type].star_time.min =  time.min;
        sd_form[type].star_time.hour = time.hour;
        sd_form[type].rhead = 0x68;
        sd_form[type].stop_time.sec =  time.sec;
        sd_form[type].stop_time.min =  time.min;
        sd_form[type].stop_time.hour = time.hour;
        save_normal_data(type, &sd_form[type].head , 10);
        
        for (; len > FILEBUFLEN; len -= FILEBUFLEN)
        {
            memcpy(sd_form[type].buf, buf, FILEBUFLEN);
            buf += FILEBUFLEN;
            sd_form[type].len = FILEBUFLEN;
            save_normal_data(type, sd_form[type].buf , sd_form[type].len);
        }
        
        memcpy(sd_form[type].buf, buf, len);
        sd_form[type].len = len;
        save_normal_data(type, sd_form[type].buf , sd_form[type].len);
#endif
    }
    else if (4 == mode)
    {
#if 1
        save_data_to_sd(filename[type], buf, len);
#endif
    }
}






/*
void form_normal_frame_pis(uint8 type,  tRtcTimer time, uint8 *buf, uint16 len)
{
    if(0 == sd_ready_flag)
    {
        return;
    }

        sd_pis[type].head = 0x68;
        sd_pis[type].star_time.sec=  time.sec;
        sd_pis[type].star_time.min=  time.min;
        sd_pis[type].star_time.hour= time.hour;
     sd_pis[type].len = len;
        sd_pis[type].rhead = 0x68;
        memcpy(sd_pis[type].buf, buf, len);

    memcpy(sd_info[type].buf,&sd_pis[type].head,6);

    if(type == PIS ){
        memcpy(&sd_info[type].buf[6],sd_pis[type].buf,len);
        sd_info[type].len = 6+ PIS_FRAME_LEN_CHANGE;
        sd_fflush(PIS, FRAMEOK);
    }
}
*/
/***********************************************************************************************************************
*Function Nane     :  save_error_data
*Description       :  ��¼���յ�������
*Input             :   type ���� 0 : tms  1: pis  2:train485
                       buf  ����
                       len  ���ݳ���
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
static void save_error_data(uint8 type, uint8 *buf, uint16 len)
{
    uint16 size = 0;
    
    if (( 0 == len) || (0 == sd_ready_flag))
    {
        return ;
    }
    
    //�ļ�������
    if (FILEBUFLEN <= sd_error_info[type].len + len)
    {
        size = FILEBUFLEN - sd_error_info[type].len;
        memcpy(&sd_error_info[type].buf[sd_error_info[type].len], buf, size);
        save_data_to_sd(errorfilename[type], sd_error_info[type].buf, FILEBUFLEN);
        sd_error_info[type].len = len - size;
        memcpy(&sd_error_info[type].buf[0], &buf[size], sd_error_info[type].len);
    }
    else
    {
        memcpy(&sd_error_info[type].buf[sd_error_info[type].len], buf, len);
        sd_error_info[type].len += len;
    }
}
/***********************************************************************************************************************
*Function Nane     :  form_error_frame
*Description       :  ��֯д��SD��������
*Input             :   type ����         0 : tms       1: pis      2:tba
                           mode ʱ���  0 : ��ʼ 1 �м�  2:����
                           time  ʱ��
                           buf  ����
                           len  ���ݳ���
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void form_error_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len)
{
    uint16 total = 0;
    //    uint16 sendlen =0;
    uint16 pos = 0;
    //    uint8  prelen=0;
    
    if (0 == sd_ready_flag)
    {
        return;
    }
    
    if (0 == mode)         //֡ͷ
    {
        sd_error_form[type].head =  0x68;
        sd_error_form[type].star_time.sec =  time.sec;
        sd_error_form[type].star_time.min =  time.min;
        sd_error_form[type].star_time.hour = time.hour;
        sd_error_form[type].rhead = 0x68;
        sd_error_form[type].len = 0;
        total = len;
        
        while ((0xf6) <= total)  //256 - 10
        {
            //���ݳ��ȴ��ڻ����255
            //�����ļ���������ֱ�����ݳ���С��255
            sd_error_form[type].stop_time.sec =  time.sec;
            sd_error_form[type].stop_time.min =  time.min;
            sd_error_form[type].stop_time.hour = time.hour;
            sd_error_form[type].num = 1;
            sd_error_form[type].len = 0xf6;
            save_error_data(type, &sd_error_form[type].head, 10);
            save_error_data(type, &buf[pos] , 0xf6);
            total -= 0xf6;
            pos   += 0xf6;
        }
        
        // �������ݻ�����
        memcpy(sd_error_form[type].buf, &buf[pos], total);
        sd_error_form[type].len = total;
        sd_error_form[type].stop_time.sec =  time.sec;
        sd_error_form[type].stop_time.min =  time.min;
        sd_error_form[type].stop_time.hour = time.hour;
        sd_error_form[type].num = 1;
        save_error_data(type, &sd_error_form[type].head, 10);
        save_error_data(type, &buf[pos] , total);
    }
    else if (1 == mode)   //�м�����
    {
        total    = len;
        
        while ((0xf6) <= total)  //256 - 10
        {
            //���ݳ��ȴ��ڻ����255
            //�����ļ���������ֱ�����ݳ���С��255
            sd_error_form[type].stop_time.sec =  time.sec;
            sd_error_form[type].stop_time.min =  time.min;
            sd_error_form[type].stop_time.hour = time.hour;
            sd_error_form[type].num = 1;
            sd_error_form[type].len = 0xf6;
            save_error_data(type, &sd_error_form[type].head, 10);
            save_error_data(type, &buf[pos] , 0xf6);
            total -= 0xf6;
            pos   += 0xf6;
        }
        
        memcpy(sd_error_form[type].buf, &buf[pos], total);
        sd_error_form[type].len = total;
        sd_error_form[type].stop_time.sec =  time.sec;
        sd_error_form[type].stop_time.min =  time.min;
        sd_error_form[type].stop_time.hour = time.hour;
        sd_error_form[type].num = 1;
        save_error_data(type, &sd_error_form[type].head, 10);
        save_error_data(type, &buf[pos] , total);
    }
    else if (2 == mode)   //֡������ȡ�ý���ʱ�� �������д洢
    {
        sd_error_form[type].num = 1;
        sd_error_form[type].stop_time.sec =  time.sec;
        sd_error_form[type].stop_time.min =  time.min;
        sd_error_form[type].stop_time.hour = time.hour;
        save_error_data(type, &sd_error_form[type].head, 10);
        save_error_data(type, sd_error_form[type].buf , sd_error_form[type].len);
        sd_error_form[type].len = 0;
    }
    else if (3 == mode)
    {
#if 1
        save_data_to_sd(errorfilename[type], buf, len);
#endif
    }
    else if (4 == mode)
    {
        save_data_to_sd(errorfilename[type], buf, len);
    }
}




/*
void form_error_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len)
{
    uint16 total = 0;
    uint16 sendlen =0;
    uint16 pos =0;
    uint8  prelen=0;

    if(0 == sd_ready_flag)
    {
        return;
    }

    if (0 == mode)         //֡ͷ
    {
        sd_error_form[type].head =  0x68;
        sd_error_form[type].star_time.sec=  time.sec;
        sd_error_form[type].star_time.min=  time.min;
        sd_error_form[type].star_time.hour= time.hour;
        sd_error_form[type].rhead = 0x68;
        sd_error_form[type].len = 0;
        total = len;
        while (0xff <= total)
        {
            //���ݳ��ȴ��ڻ����255
            //�����ļ���������ֱ�����ݳ���С��255
            sd_error_form[type].stop_time.sec=  time.sec;
            sd_error_form[type].stop_time.min=  time.min;
            sd_error_form[type].stop_time.hour= time.hour;
            sd_error_form[type].num = 1;
            sd_error_form[type].len = 0xff;
            save_error_data(type, &sd_error_form[type].head, 10);
            save_error_data(type, &buf[pos] , 0xff);
            total -= 0xff;
            pos   += 0xff;
        }
        // �������ݻ�����
        memcpy(sd_error_form[type].buf, &buf[pos], total);
        sd_error_form[type].len = total;
    }
    else if (1 == mode)   //�м�����
    {
        total    = len;
        prelen  = sd_error_form[type].len;  //֮ǰBUFû����ĳ���

        if(0xff <= sd_error_form[type].len + len)
        {
            total = sd_error_form[type].len + len;

            sd_error_form[type].stop_time.sec=  time.sec;
            sd_error_form[type].stop_time.min=  time.min;
            sd_error_form[type].stop_time.hour= time.hour;
            sd_error_form[type].num = 1;
            sd_error_form[type].len = 0xff;

            save_error_data(type, &sd_error_form[type].head, 10 + prelen);
            sendlen = 0xff - prelen;
            save_error_data(type, buf , sendlen);

            total -= 0xff;
            pos  =  sendlen;
            sd_error_form[type].star_time.sec=  time.sec;
            sd_error_form[type].star_time.min=  time.min;
            sd_error_form[type].star_time.hour= time.hour;

            prelen = 0;
            while(0xff <= total)
            {
                sd_error_form[type].len = 0xff;
                save_error_data(type, &sd_error_form[type].head, 10);
                save_error_data(type, &buf[pos] , 0xff);
                total -= 0xff;
                pos   += 0xff;
            }
        }

        memcpy(&sd_error_form[type].buf[prelen], &buf[pos], total);
        sd_error_form[type].len = prelen + total;
    }
    else if (2 == mode)   //֡������ȡ�ý���ʱ�� �������д洢
    {
        sd_error_form[type].num = 1;
        sd_error_form[type].stop_time.sec=  time.sec;
        sd_error_form[type].stop_time.min=  time.min;
        sd_error_form[type].stop_time.hour= time.hour;
        save_error_data(type, &sd_error_form[type].head, 10);
        save_error_data(type, sd_error_form[type].buf , sd_error_form[type].len);
        sd_error_form[type].len=0;
    }
}*/
/***********************************************************************************************************************
*Function Nane     :  sd_fflush
*Description       :  ǿ�ư��ļ�������������д���ļ�
*Input             :   type  �ļ�����  0 : tms       1: pis      2:tba
                           mode ��������  FRAMEOK : ���� FRAMEERR ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void sd_fflush(uint8 type, uint8 mode)
{
#if 1

    if (0 == sd_ready_flag)
    {
        return;
    }
    
    if (FRAMEOK == mode)
    {
        save_data_to_sd(filename[type], sd_info[type].buf, sd_info[type].len);
        sd_info[type].len = 0;
    }
    else if (FRAMEERR == mode)
    {
        save_data_to_sd(errorfilename[type], sd_error_info[type].buf, sd_error_info[type].len);
        sd_error_info[type].len = 0;
    }
    
#endif
}

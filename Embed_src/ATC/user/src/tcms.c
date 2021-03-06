/******************************************************************************
*文件名称：tcms.c
*作          者：hk
*描          述：TCMS数据处理
*创建日期：2012-04-26
*修改记录：无
******************************************************************************/
//#include "tba_3093.h"
#include "main.h"
#include "stm32f2x7_eth.h"
#include "netconf.h"
#include "net_debug.h"
#include "stm32f2xx_usart.h"
#include "lwip/mem.h"
#include "lwip/udp.h"

#include "uart.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcms.h>
#include "rtc_1337.h"
#include "usb.h"
#include "tba_bus.h"

#include "rtc_1337.h"

#include "sd_data.h"

#include "cmd.h"
#include <time.h>


#define TMS2PIS_LENGTH 64
//#define TMS2PIS_LENGTH 28

#define SEND_PIS_CS_INTER     0 /*选择中断发送还是轮询发送*/
#define SEND_TMS_CS_INTER    0
//#define SEND_TMS_CS_INTER    0
#if 0
static uint8       g_old_tms_buf[TMS_FRAME_LEN + 1] = {0};
static uint8       g_data_for_sd[TMS_FRAME_LEN + 1] = {0};
static uint8       g_data_error_buf[256];
#endif
#if 1
//ATC接收PIS数据结构
#define PIS_RECV_BUFF_SIZE 1500 /*pis数据缓存大小*/
#define PIS_RECV_PBUFF g_recv_pis_buf  /*pis数据缓存指针*/
#define PIS_FRAM_BEGIN 0xFE    /*pis数据包头*/
#define PIS_FRAM_END 0xFF      /*pis数据包尾*/
#if 0
#define PIS_FRAM_LEN PIS_FRAME_LEN  /*pis数据包长度*/
#else
#define PIS_FRAM_LEN PIS_FRAME_LEN_CHANGE
#endif
#define PIS_DATA_BUFF pis_recv_buf  /*pis数据包指针*/
#define PIS_READ_UART UART_PIS /*串口选择*/
/*校验和计算方法可能有不同，需要修改*/
typedef struct PIS_RECV_BUFF
{
    //长度
    uint16 len;
    //接收口
    uint16 in;
    //读口
    uint16 out;
    //数据buff
    uint8   buf[PIS_RECV_BUFF_SIZE];
} PIS_BUFF;
static PIS_BUFF g_recv_pis_buf =
{
    0,
    0,
    0,
    {0,},
};
//写SD数据
#define PIS_SD_BUFF_SIZE  1500
#define PIS_BUFF_LEN          g_pis_to_sd_len
#define PIS_BUFF_BUFF       g_pis_to_sd_buf
static uint8 g_pis_to_sd_buf[PIS_SD_BUFF_SIZE] = 0;
static uint32 g_pis_to_sd_len = 0;

#if 1
//错误数据
#define __PIS_FRAM_ERROR_BUFF     PIS_FRAM_ERROR_BUFF   /*错误数据缓存*/
#define PIS_ERROR_SIZE 1500    /*错误数据缓存大小*/
#define PIS_FRAM_ERROR_BUFF     g_pis_data_error_buff   /*错误数据缓存*/
#define PIS_FRAM_ERROR_LENTH  g_pis_data_error_length  /*错误数据长度*/
static uint8 g_pis_data_error_buff[PIS_ERROR_SIZE] = {0};
static uint32 g_pis_data_error_length = 0;
#endif
#endif

#if 1
//ATC接收TMS数据结构
#define TMS_RECV_BUFF_SIZE 1500 /*tms数据缓存大小*/
#define TMS_RECV_PBUFF g_recv_tms_buf  /*tms数据缓存指针*/
#define TMS_FRAM_BEGIN 0xFE    /*tms数据包头*/
#define TMS_FRAM_END 0xFF      /*tms数据包尾*/
#define TMS_FRAM_LEN TMS_FRAME_LEN  /*tms数据包长度*/
#define TMS_DATA_BUFF tms_recv_buf  /*tms数据包指针*/
//#define TMS_READ_UART UART_TMS /*串口选择*/
#if 0
#define TMS_READ_UART UART3 /*串口选择*/
#else
#define TMS_READ_UART UART2 /*串口选择*/
#endif
/*校验和计算方法可能有不同，需要修改*/
typedef struct TMS_RECV_BUFF
{
    //长度
    uint16 len;
    //接收口
    uint16 in;
    //读口
    uint16 out;
    //数据buff
    uint8   buf[TMS_RECV_BUFF_SIZE];
} TMS_BUFF;
static TMS_BUFF g_recv_tms_buf =
{
    0,
    0,
    0,
    {0,},
};
//写SD数据
#define TMS_SD_BUFF_SIZE 1500
#define TMS_BUFF_LEN g_tms_to_sd_len
#define TMS_BUFF_BUFF g_tms_to_sd_buf
static uint8 g_tms_to_sd_buf[TMS_SD_BUFF_SIZE] = 0;
static uint32 g_tms_to_sd_len = 0;

#if 1
//错误数据
#define __TMS_FRAM_ERROR_BUFF     TMS_FRAM_ERROR_BUFF   /*错误数据缓存*/
#define TMS_ERROR_SIZE 1500    /*错误数据缓存大小*/
#define TMS_FRAM_ERROR_BUFF     g_tms_data_error_buff   /*错误数据缓存*/
#define TMS_FRAM_ERROR_LENTH  g_tms_data_error_length  /*错误数据长度*/
static uint8 g_tms_data_error_buff[TMS_ERROR_SIZE] = {0};
static uint32 g_tms_data_error_length = 0;
#endif
#endif
//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
unsigned int ats_crc_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, //7
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, //15
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, //23
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, //31
    
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, //63
    
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, //95
    
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

#define DLE 0xFE
#define STX 0x00
#define ETX 0x23
#define TRAINPD 0xFF
#define TRAINPDA 0xcc

uint32 timeout_to_pis = 0;
u8 flag_to_pis = 0;                 //当uart3 收到个正常包 置位

tATC_INFO   atc_recv_from_tms;   /* ATC  收到tms的信息*/

tATC_INFO   atc_recv_from_pis;   /* ATC  收到pis的信息*/
tTMS_INFO    tms_to_pis;         /*  tms发送，要发往pis的数据*/
tPIS_INFO    pis_to_tms;         /*  pis发送，要发往tms的数据*/

tstate      atc_tms;             //atc与tms通信状态
tstate      atc_pis;             //atc与pis通信状态
/********************************/

uint8       num_repeat_send = 0;     //数据重发次数
/*没有钥匙时 默认发往tms的数据*/
uint8 slave_tms_to_pis_buf[TMS_FRAME_LEN ] = {0};
/*
= {0xff, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x00, 0xeb, 0x6b, 0x00,
     0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x58, 0xc3};
*/
uint8       old_tms_buf[TMS_FRAME_LEN] = {0};
uint8       old_pis_buf[PIS_FRAME_LEN_CHANGE] = {0};

extern uint8 atc_recv_train485_flag;     /*ATC 是否收到485总线上的数据标志位*/
extern uint8 ATC_addr;                   /*ATC 设备地址*/

extern uint8 sd_ready_flag;

uint8 _gtmp_buf[256] = {0};

u8 tms_get_flag = 0;
u8 tms_recv_length = 0;
u8 tms_recv_buf[TMS_FRAME_LEN + 1] = {0};


u8 pis_get_flag = 0;
u8 pis_recv_length = 0;
u8 pis_recv_buf[PIS_FRAME_LEN] = {0};


u8 pis_to_tms_data[PIS_FRAME_LEN] = {0};


static u8 atc_com_conter = 0;
//CRC-CCITT:(由欧洲CCITT推荐)G(X) = X16 + X12 + X5 + 1
unsigned int check_ccitt_crc(unsigned char *nData, unsigned int len)
{
    unsigned int crc = 0;
    unsigned char crc_H8;
    unsigned char *ptr = nData;
    
    while ( len-- )
    {
        crc_H8 = (unsigned char)(crc >> 8);
        crc = crc << 8;
        crc = crc ^ ats_crc_table[crc_H8 ^ *ptr];
        ptr++;
    }
    
    return crc;
}
/***********************************************************************************************************************
*Function Nane    :  send_tms_ptu
*Description      :  发送tms数据到ptu
*Input            :  无
*Output           :  无
*Return           :  无
*Others           :  无
***********************************************************************************************************************/
void send_tms_ptu(uint16 len)
{
    uint8 buf[4] = {0x7e, 0x37, 0x08, 0x02};
    buf[1] = len + 2;
    memcpy(_gtmp_buf, buf, 4);
    memcpy(&_gtmp_buf[4], atc_recv_from_tms.buf, len);
    get_cs(&_gtmp_buf[1], len + 3, &_gtmp_buf[len + 4]);
    _gtmp_buf[len + 5] = 0x7e;
    usb_write_buf(_gtmp_buf, len + 6);
}
/***********************************************************************************************************************
*Function Nane    :  send_pis_ptu
*Description      :  发送pis数据到ptu
*Input            :  无
*Output           :  无
*Return           :  无
*Others           :  无
***********************************************************************************************************************/
void send_pis_ptu(uint16 len)
{
    uint8 buf[4] = {0x7e, 0x37, 0x08, 0x03};
    buf[1] = len + 2;
    memcpy(_gtmp_buf, buf, 4);
    memcpy(&_gtmp_buf[4], atc_recv_from_pis.buf, len);
    get_cs(&_gtmp_buf[1], len + 3, &_gtmp_buf[len + 4]);
    _gtmp_buf[len + 5] = 0x7e;
    usb_write_buf(_gtmp_buf, len + 6);
}
/****************************************************
*函数名：train_485_proc
*作者：hk
*描述：485数据  处理
*输入：无
*输出：无
*返回值：无
*修改记录：无
****************************************************/
void train_485_proc(void)
{
    /*当atc与pis通信超时时， 发生主备切换*/
    proc_deal_packet();
}
/****************************************************
*函数名：send_data_to_pis
*作者：hk
*描述：往PIS发送数据
*输入：无
*输出：无
*返回值：无
*修改记录：无
****************************************************/
/**************************************************************
void send_data_to_pis(void)
{
static uint32 timeout = 0;
//    uint16 my_crc = 0;
u8 data_pis_cnt = 0;
u8 data_pis_xor = 0;
u8 data_crc_cnt = 0;
u8 to_pis_buf[200] = {0};







if(check_timer(&timeout, 256))     //这里要改下   觉得timeout 每次进入到这里都为0，得重新延时
{
    //data_pis_xor = tms_to_pis.buf[2];
    data_pis_xor = TRAINPD;
    for(data_pis_cnt = 3 ;data_pis_cnt<TMS_FRAME_LEN-3;data_pis_cnt ++)
        data_pis_xor ^= tms_to_pis.buf[data_pis_cnt];
    tms_to_pis.buf[TMS_FRAME_LEN-1]= data_pis_xor;

    to_pis_buf[data_crc_cnt++] = DLE;
    to_pis_buf[data_crc_cnt++] = STX;
    to_pis_buf[data_crc_cnt++] = TRAINPD ;
    for(data_pis_cnt = 3 ;data_pis_cnt<TMS_FRAME_LEN-3;data_pis_cnt ++)
    {
        if(DLE == tms_to_pis.buf[data_pis_cnt])
        {
            to_pis_buf[data_crc_cnt++] = tms_to_pis.buf[data_pis_cnt];
            to_pis_buf[data_crc_cnt++] = tms_to_pis.buf[data_pis_cnt];
        }
    else
        to_pis_buf[data_crc_cnt++] = tms_to_pis.buf[data_pis_cnt];
}
to_pis_buf[data_crc_cnt++] = DLE;
to_pis_buf[data_crc_cnt++] = ETX;
to_pis_buf[data_crc_cnt++] = data_pis_xor;
uart_write(UART_PIS,  to_pis_buf, data_crc_cnt);
    //uart_write(UART_PIS,  tms_to_pis.buf, tms_to_pis.len);
    if(atc_com_conter>5)
        memset(tms_to_pis.buf, 0xff, TMS_FRAME_LEN);
//pisc_usb_debug(HEX,tms_to_pis.buf, tms_to_pis.len);
//memset(tms_to_pis.buf+3, 0x00, TMS_FRAME_DATA_LEN);
    if(atc_tms.comm_rigth)
    {
     //   *转发给PISC*
        if (num_repeat_send)
        {
            //uart_write(UART_PIS,  tms_to_pis.buf, tms_to_pis.len);
            num_repeat_send--;
        }
    }
    else
    {
        //uart_write(UART_PIS, slave_tms_to_pis_buf, TMS_FRAME_LEN);
    }
}
}
************************************************************/

void send_data_to_pis(void)
{
    u8 i;
    u8 to_pis_buf[82] = {0};
    u8 src_tmp = 0;
    memcpy(to_pis_buf, tms_recv_buf, 79);
    to_pis_buf[0] = 0xfe;
    
    for (i = 1; i < 79; i++)
    {
        src_tmp += to_pis_buf[i];  //0-78
    }
    
    to_pis_buf[i++] = src_tmp;  //79
    to_pis_buf[i] = 0xff;   //80
#if SEND_PIS_CS_INTER
    uart_write(UART1, to_pis_buf , TMS_FRAME_LEN_TMP);
    uart_write(UART1, to_pis_buf , TMS_FRAME_LEN_TMP);
#else
    
    for (i = 0; i < TMS_FRAME_LEN_TMP; i++)
        uart1_write_char(to_pis_buf[i]);
    
    for (i = 0; i < TMS_FRAME_LEN_TMP; i++)
        uart1_write_char(to_pis_buf[i]);
    
#endif
    
    if (atc_com_conter > 5)
    {
        //memset(tms_to_pis.buf, 0xff, TMS_FRAME_LEN_TMP);
        return ;
    }
}

/***********************************************************************************************************************
*Function Nane     :  save_pis_data_sd
*Description       :  存储pis的数据帧至sd卡
*Input             :  type  TMSOK 正确帧 TMSERR 错误帧
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/

void save_pis_data_sd(uint8 type, uint16 len)
{
#if 1
    u8 data_for_sd[6] = {0};
    tRtcTimer time;
    uint8 tmp = 0;
    get_cur_time(&time);
    
    if (PISOK == type)
    {
#if 1
        memcpy(data_for_sd, pis_recv_buf, len);
        
        //存储错误数据帧
        if ( 1 == atc_pis.error)
        {
            atc_pis.error = 0;
            form_error_frame(PIS, 2, time, 0, 0);
            return ;
        }
        
        //收到的数据帧与上一帧数据不同，记录信息
        if (memcmp(old_pis_buf, &data_for_sd[0],  len))
        {
            memcpy(old_pis_buf, &data_for_sd[0], len);
            
            if (atc_pis.normal)
            {
                form_normal_frame(PIS,  2, time, NULL, atc_pis.num_repeat);
                atc_pis.normal = 0;
            }
            
            form_normal_frame(PIS,  1, time, data_for_sd, len);
            atc_pis.normal = 1;
            atc_pis.num_repeat = 0;
        }
        
        //if ( 0xff!=atc_pis.num_repeat)
        {
            atc_pis.num_repeat++;
            
            if ((atc_pis.num_repeat % 40) == 0)
            {
                if (atc_pis.normal)
                {
                    form_normal_frame(PIS,  2, time, NULL, atc_pis.num_repeat);
                }
                
                form_normal_frame(PIS,  1, time, data_for_sd, len);
                atc_pis.num_repeat = 0;
            }
        }
#endif
    }
    else if (PISERR == type)
    {
#if 1
        memcpy(data_for_sd, pis_recv_buf, len);
        
        if (0 == atc_pis.error)
        {
            atc_pis.error = 1;
            form_error_frame(PIS, 0, time, data_for_sd, len);
        }
        else
        {
            form_error_frame(PIS, 1, time, data_for_sd, len);
        }
        
#endif
    }
    else if (TMSOK3 == type)
    {
        form_normal_frame(PIS,  3, time, (uint8 *)&PIS_BUFF_BUFF[0], len);
        //form_normal_frame(TMS,  2, time, NULL, TMS_BUFF_LEN);
        PIS_BUFF_LEN = 0;
    }
    else if (PISERROR4 == type)
    {
        form_error_frame(PIS,  4, time, (uint8 *)&PIS_FRAM_ERROR_BUFF[0], len);
        //form_normal_frame(TMS,  2, time, NULL, TMS_BUFF_LEN);
        PIS_FRAM_ERROR_LENTH = 0;
    }
    
#endif
}



void tcms_init(void)
{
    atc_recv_from_pis.len = 0;
    atc_recv_from_tms.len = 0;
}



/*

static uint8 pis_get_packet(void)
{
    uint8 temp;

    while(uart_read(UART_PIS, &temp, 1))   //FE 00 23
    {
        //数据分隔符
        if(temp==0xFE)
        {
            if(pis_get_flag != 1)
            {
                pis_get_flag=1;
                pis_recv_length=0;
                pis_recv_buf[pis_recv_length++] = 0xFE;
            }
        }


        //数据部分处理
        else if (pis_get_flag==1)
        {
            if(pis_recv_length == PIS_FRAME_LEN -1)
            {
                //uart_read(UART_PIS, &temp, 1);
                if(temp != 0xFF)
                {
                    pis_get_flag = 0;
                    pis_recv_length = 0;
                    return 0;
                }
                else
                {
                    pis_recv_buf[pis_recv_length++] = temp;
                    return 1;
                }

            }
            else
            {
                //uart_read(UART_PIS, &temp, 1);
                pis_recv_buf[pis_recv_length++] = temp;
            }
        }
    }

    return 0;
}
*/









/*
static uint8 pis_get_packet(void)
{
    uint8 temp;
    static u8 mySys = 0;
    static u8 otherSys = 0;
    static u32 tick;
    u8 myBuf[20]={0};
    u8 myLen = 0;

    //pis_recv_length = 0;
    while(uart_read(UART_PIS, &temp, 1))
    {
        if(temp == TBA_PACKET_TAB)
        {
            //判断长度 判断是否有数据
            if(pis_recv_length >= 3)
            {
                memcpy(pis_recv_buf, myBuf,myLen);
                pis_recv_length = myLen;
                myLen = 0;
                return 1;
            }
            myLen=0;
        }
        //其他字符串直接送缓冲区
        else
        {
            myBuf[myLen] = temp;
            if( ++myLen >= 20 )
                myLen = 0;
        }

    }
    return 0;
}
*/


//把所有pis数据读到g_recv_pis_buf
static uint8 read_pis_to_buf(void)
{
    uint8 temp;
    uint16 len = 0;
    
    while (uart_read(PIS_READ_UART, &temp, 1))
    {
        PIS_BUFF_BUFF[(PIS_BUFF_LEN++) % PIS_SD_BUFF_SIZE] = temp;
        PIS_RECV_PBUFF.buf[PIS_RECV_PBUFF.in++] = temp;
        PIS_RECV_PBUFF.in %= PIS_RECV_BUFF_SIZE;
        
        if (PIS_RECV_PBUFF.in == PIS_RECV_PBUFF.out)
        {
            PIS_RECV_PBUFF.out++;
        }
        else
        {
            PIS_RECV_PBUFF.len++;
        }
        
        PIS_RECV_PBUFF.out %= PIS_RECV_BUFF_SIZE;
        len++;
        
        if (len >= PIS_FRAM_LEN)
        {
            break;
        }
    }
    
    return 0;
}
//计算g_recv_pis_buf 的长度
static uint16 get_pis_len(void)
{
    return PIS_RECV_PBUFF.len;
}
//从g_recv_pis_buf 读出指定长度数据到pbuf，返回读出的长度
static uint16 read_pis_buf(uint8 *pbuf, uint16 len)
{
    uint16 ret = 0;
    uint16 i;
    
    if (PIS_RECV_PBUFF.len < len && NULL == pbuf)
    {
        ret = 0;
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            pbuf[i] = PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out + i) % PIS_RECV_BUFF_SIZE];
        }
        
        PIS_RECV_PBUFF.out = (PIS_RECV_PBUFF.out + len) % PIS_RECV_BUFF_SIZE;
        PIS_RECV_PBUFF.len -= len;
        ret = len;
    }
    
    return ret;
}
//从g_recv_pis_buf 删除len个字节
static void delete_pis_buf(uint16 len)
{
    uint16 i;
    
    if (PIS_RECV_PBUFF.len < len)
    {
        PIS_RECV_PBUFF.in = 0;
        PIS_RECV_PBUFF.out = 0;
        PIS_RECV_PBUFF.len = 0;
        return;
    }
    
    for (i = 0; i < len; i++)
    {
        PIS_FRAM_ERROR_BUFF[(PIS_FRAM_ERROR_LENTH++) % PIS_ERROR_SIZE] = PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out + i) % PIS_RECV_BUFF_SIZE];
    }
    
    PIS_RECV_PBUFF.out = (PIS_RECV_PBUFF.out + len) % PIS_RECV_BUFF_SIZE;
    PIS_RECV_PBUFF.len -= len;
    return;
}
#if 1
static uint8 pis_get_packet(void)
{
    uint8 temp;
    u8 checksum = 0;
    uint16 i, len;
    uint8 ret = 0;
    read_pis_to_buf();
    
    while (get_pis_len() >= PIS_FRAM_LEN) //FE 00 23
    {
        //数据分隔符
        temp = PIS_RECV_PBUFF.buf[PIS_RECV_PBUFF.out];
        
        if (PIS_FRAM_BEGIN == temp && PIS_FRAM_END == PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out + PIS_FRAM_LEN - 1) % PIS_RECV_BUFF_SIZE])
        {
            checksum = 0;
            
            for (i = 1; i < PIS_FRAM_LEN - 2; i++)
            {
                checksum += PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out + i) % PIS_RECV_BUFF_SIZE];
            }
            
            if (checksum == PIS_RECV_PBUFF.buf[(PIS_RECV_PBUFF.out + PIS_FRAM_LEN - 2) % PIS_RECV_BUFF_SIZE])
            {
                len = read_pis_buf(PIS_DATA_BUFF, PIS_FRAM_LEN);
                
                if (PIS_FRAM_LEN == len)
                {
#if 0
                
                    if (PIS_FRAM_ERROR_LENTH > 0)
                    {
                        //save_pis_data_sd(PISERR, PIS_FRAM_ERROR_LENTH);
                        PIS_FRAM_ERROR_LENTH = 0;
                    }
                    
#endif
                    ret = 1;
                    break;
                }
            }
        }
        
#if 0
        
        if (PIS_FRAM_ERROR_LENTH >= PIS_ERROR_SIZE)
        {
            //save_pis_data_sd(PISERR, PIS_FRAM_ERROR_LENTH);
            PIS_FRAM_ERROR_LENTH = 0;
        }
        
        read_pis_buf(&PIS_FRAM_ERROR_BUFF[PIS_FRAM_ERROR_LENTH], 1);
        PIS_FRAM_ERROR_LENTH++;
#endif
        delete_pis_buf(1);
    }
    
#if 0
    
    if (PIS_FRAM_ERROR_LENTH > 0)
    {
        //save_pis_data_sd(PISERR, PIS_FRAM_ERROR_LENTH);
        PIS_FRAM_ERROR_LENTH = 0;
    }
    
#endif
    return ret;
}
#else
static uint8 pis_get_packet(void)
{
    uint8 temp;

    while (uart_read(UART_PIS, &temp, 1))  //FE 00 23
    {
        //数据分隔符
        if (temp == 0xFE)
        {
            if (pis_get_flag != 1)
            {
                pis_get_flag = 1;
                pis_recv_length = 0;
                memset(pis_recv_buf, 0, PIS_FRAME_LEN_CHANGE);
                pis_recv_buf[pis_recv_length++] = 0xFE;
            }
            else
                pis_recv_buf[pis_recv_length++] = temp;
        }
        //数据部分处理
        else if (pis_get_flag == 1)
        {
            //if((tms_recv_length == (TMS_FRAME_LEN -1)) ||(tms_recv_length == (TMS_FRAME_LEN-9)))
            if (pis_recv_length == (PIS_FRAME_LEN_CHANGE - 1))
            {
                if (temp != 0xFF)
                {
                    pis_get_flag = 0;
                    pis_recv_length = 0;
                    return 0;
                }
                else
                {
                    pis_recv_buf[pis_recv_length++] = temp;
                    return 1;
                }
            }
            else
            {
                //uart_read(UART_TMS, &temp, 1);
                pis_recv_buf[pis_recv_length++] = temp;
            }
        }
    }

    return 0;
}
#endif
/****************************************************
*函数名：pis_proc
*作者：hk
*描述：pis数据  处理
*输入：无
*输出：无
*返回值：1:收到完整的一条数据  0 :failed
*修改记录：无
****************************************************/

uint8 pis_proc(void)
{
    //    uint16 crc_data = 0;
    //   uint16 crc_head = 0;
    u8 i = 0;
    //  static u8 xor_crc = 0 ;
    //  static u8 data_cunt0 = 0 ;
    tRtcTimer time;
    // u8  *str;
    //  static u16 recv_pisc_len = 0;
    //  u8  proc_num=0;
    u16 len = 0;
    //  u16 waste_data_len=0;
    //  uint16 debug_len =0;
    //  static u8 recv_pisc_buf_temp[20] = {0};
    //  static uint16 recv_pisc_tmp_len = 0 ;
    //  static u8 recv_pisc_head_flag =0 ;
    //  static u8 recv_pisc_end_flag =0 ;
    //  u8 src_length;
    //  u8 dst_length;
    u8 checksum = 0;
    get_cur_time(&time);
    
    if (atc_pis.comm_flag)
    {
        if (check_timer(&atc_pis.comm_time, 6000))
        {
            //存储错误数据帧
            if ( 1 == atc_pis.error)
            {
                atc_pis.error = 0;
                //form_error_frame(PIS, 2, time, 0, 0);
            }
            
            //sd_fflush(PIS, FRAMEERR);
            atc_pis.comm_flag = 0;
        }
    }
    
    /*
      if (atc_pis.comm_rigth)
      {
          if(check_timer(&atc_pis.comm_rigth_time, 5000))
          {
              //记录上一帧的结束时间
              if (atc_pis.normal)
              {
                  form_normal_frame_pis(PIS,  time,"0XFF,0XFF,0XFF,0XFF,0XFF,0XFF" , 6);
                  atc_pis.normal = 0;
                  memset(old_pis_buf,0x00, PIS_FRAME_LEN_CHANGE);
              }
              sd_fflush(PIS, FRAMEOK);
              atc_pis.comm_rigth = 0;//用于上报ATC与PISC之间是否通
          }
      }
    */
    
    if (atc_pis.comm_rigth)
    {
        if (check_timer(&atc_pis.comm_rigth_time, 5000))
        {
            //记录上一帧的结束时间
            if (atc_pis.normal)
            {
                //form_normal_frame(PIS,  2, time, NULL, atc_pis.num_repeat);
                atc_pis.normal = 0;
                memset(old_pis_buf, 0x00, PIS_FRAME_LEN_CHANGE);
            }
            
            //sd_fflush(PIS, FRAMEOK);
            atc_pis.comm_rigth = 0;/*用于上报ATC与PISC之间是否通*/
        }
    }
    
    /*读取判断ATC是否接受到pis的数据*/
    len =  uart_recv_len(UART_PIS);
    
    if (len > 12)
    {
        atc_pis.comm_flag = 1;
        atc_pis.comm_time = get_time();
        
        if (pis_get_packet() == 1)
        {
            /*执行逆转码 并计算校验和
            for(src_length=0; src_length<pis_recv_length; src_length++)
              {
              //拷贝数据
              pis_recv_buf[dst_length]=pis_recv_buf[src_length];
              //判断刚刚拷贝的是否是转码符
              if(pis_recv_buf[src_length]==TBA_PACKET_DLE && src_length<pis_recv_length-1)
              {
                  //判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_TAB 并跳过下一个字符
                  if(pis_recv_buf[src_length+1]==TBA_PACKET_DLE_TAB)
                  {
                      pis_recv_buf[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TMS_PACKET_TAB
                      src_length++;//并跳过下一个字符
                  }
                  //判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_DLE 并跳过下一个字符
                  else if(pis_recv_buf[src_length+1]==TBA_PACKET_DLE_DLE)
                  {
                      pis_recv_buf[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TMS_PACKET_DLE
                      src_length++;//并跳过下一个字符
                  }
                  else
                  {
                  //print_line("tms 7f error");
                      //数据错误----7f没有被正确转码
                      dst_length=0;//清除转码以后的长度
                      break;
                  }
              }
              //checksum+=pis_recv_buf[dst_length];
              dst_length++;
            }
            */
            for (i = 1; i < 4; i++)
                checksum += pis_recv_buf[i];
                
            if (checksum == pis_recv_buf[i])
            {
                atc_pis.comm_rigth = 1;
                atc_pis.comm_rigth_time = get_time();
                //form_normal_frame(PIS,  1,time, pis_recv_buf, 6);
                save_pis_data_sd(PISOK, PIS_FRAME_LEN_CHANGE);
            }
        }
    }
    
    return 0;
}
/***********************************************************************************************************************
*Function Nane     :  save_tms_data_sd
*Description       :  存储tms的数据帧至sd卡
*Input             :  type  TMSOK 正确帧 TMSERR 错误帧
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/

void save_tms_data_sd(uint8 type, uint16 len)
{
#if 1
    u8 data_for_sd[TMS_FRAME_LEN] = {0};
    tRtcTimer time;
    uint8 tmp = 0;
    u8 buf[5] = {0};
    memcpy(data_for_sd, tms_recv_buf, TMS_FRAME_LEN);
    get_cur_time(&time);
    
    if (TMSOK == type)
    {
#if 1
    
        //存储错误数据帧
        if ( 1 == atc_tms.error)
        {
            atc_tms.error = 0;
            form_error_frame(TMS, 2, time, 0, 0);
        }
        
        buf[0] = data_for_sd[17];
        buf[1] = data_for_sd[18];
        buf[2] = data_for_sd[19];
        buf[3] = data_for_sd[20];
        buf[4] = data_for_sd[151];
        data_for_sd[17] = 0;
        data_for_sd[18] = 0;
        data_for_sd[19] = 0;
        data_for_sd[20] = 0;
        data_for_sd[151] = 0;
        
        //收到的数据帧与上一帧数据不同，记录信息
        if (memcmp(old_tms_buf, &data_for_sd[0], TMS_FRAME_LEN))
        {
            memcpy(old_tms_buf, &data_for_sd[0], TMS_FRAME_LEN);
            //num_repeat_send = 3;
            
            if (atc_tms.normal)
            {
                form_normal_frame(TMS,  2, time, NULL, atc_tms.num_repeat);
                atc_tms.normal = 0;
            }
            
            data_for_sd[17] = buf[0];
            data_for_sd[18] = buf[1];
            data_for_sd[19] = buf[2];
            data_for_sd[20] = buf[3];
            data_for_sd[151] = buf[4];
            form_normal_frame(TMS,  1, time, data_for_sd, TMS_FRAME_LEN);
            atc_tms.normal = 1;
            atc_tms.num_repeat = 0;
        }
        
        if ( 0xff != atc_tms.num_repeat)
        {
            atc_tms.num_repeat++;
        }
        
#endif
    }
    else if (TMSERR == type)
    {
#if 1
    
        if (0 == atc_tms.error)
        {
            atc_tms.error = 1;
            form_error_frame(TMS, 0, time, data_for_sd, len);
        }
        else
        {
            form_error_frame(TMS, 1, time, data_for_sd, len);
        }
        
#endif
    }
    else if (TMSOK3 == type)
    {
        form_normal_frame(TMS,  3, time, (uint8 *)&TMS_BUFF_BUFF[0], len);
        //form_normal_frame(TMS,  2, time, NULL, TMS_BUFF_LEN);
        TMS_BUFF_LEN = 0;
    }
    else if (TMSERROR4 == type)
    {
        form_error_frame(TMS,  4, time, (uint8 *)&TMS_FRAM_ERROR_BUFF[0], len);
        //form_normal_frame(TMS,  2, time, NULL, TMS_BUFF_LEN);
        TMS_FRAM_ERROR_LENTH = 0;
    }
    
#endif
}
/***********************************************************************************************************************
*Function Nane     :  atc_proc_tms
*Description       :  处理tms发过来的数据
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
#if 1
void  atc_proc_tms(void)
{
    //    uint16 my_crc = 0;
    u8 i;
    u8 to_tms_buf[PIS_FRAME_LEN + 1] = {0};
    //memcpy(to_tms_buf,pis_to_tms.buf,PIS_FRAME_LEN);
    //memset(pis_to_tms.buf,0,PIS_FRAME_LEN);
    /*
    if(to_tms_buf[0] != 0xfe)
        return;
    */
    /************************
    写死部分
    *********************/
    to_tms_buf[0] = 0xFE;
    to_tms_buf[33] = 0x01;
    to_tms_buf[35] = 0x01;
    to_tms_buf[37] = 0x01;
    to_tms_buf[39] = 0x01;
    to_tms_buf[41] = 0x01;
    to_tms_buf[43] = 0x01;
    to_tms_buf[45] = 0x01;
    to_tms_buf[46] = 0x03;
    to_tms_buf[65] = 0x0A;
    to_tms_buf[66] = 0xFF;
    to_tms_buf[67] = 0x0;
#if SEND_TMS_CS_INTER
    uart_write(UART2, to_tms_buf, PIS_FRAME_LEN + 1);
#else
    gpio_485_enable(EN_485_TMS);
    
    for (i = 0; i <= PIS_FRAME_LEN; i++)
        uart2_write_char(to_tms_buf[i]);
    
#if 0
    uart2_write_char(1);
    uart2_write_char(2);
    uart2_write_char(3);
    uart2_write_char(4);
    uart2_write_char(5);
    uart2_write_char(1);
    uart2_write_char(2);
    uart2_write_char(3);
    uart2_write_char(4);
#endif
    //while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    gpio_485_disable(EN_485_TMS);
    //  USART_ClearFlag(USART2, USART_FLAG_TC);
#endif
}
#endif
#if 0
void  atc_proc_tms(void)
{
    //    uint16 my_crc = 0;
    u8 i;
    u8 to_tms_buf[PIS_FRAME_LEN + 1] = {0};
    //memcpy(to_tms_buf,pis_to_tms.buf,PIS_FRAME_LEN);
    //memset(pis_to_tms.buf,0,PIS_FRAME_LEN);
    /*
    if(to_tms_buf[0] != 0xfe)
        return;
    */
    /************************
    写死部分
    *********************/
    to_tms_buf[0] = 0xFE;
    to_tms_buf[33] = 0x01;
    to_tms_buf[35] = 0x01;
    to_tms_buf[37] = 0x01;
    to_tms_buf[39] = 0x01;
    to_tms_buf[41] = 0x01;
    to_tms_buf[43] = 0x01;
    to_tms_buf[45] = 0x01;
    to_tms_buf[46] = 0x03;
    to_tms_buf[65] = 0x0A;
    to_tms_buf[66] = 0xFF;
    to_tms_buf[67] = 0x0;
#if SEND_TMS_CS_INTER
    uart_write(UART3, to_tms_buf, PIS_FRAME_LEN + 1);
#else
    gpio_485_enable(EN_485_ATC);
    
    for (i = 0; i <= PIS_FRAME_LEN; i++)
        uart3_write_char(to_tms_buf[i]);
    
#if 0
    uart3_write_char(1);
    uart3_write_char(2);
    uart3_write_char(3);
    uart3_write_char(4);
    uart3_write_char(5);
    uart3_write_char(6);
#endif
    
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    
    gpio_485_disable(EN_485_ATC);
    USART_ClearFlag(USART3, USART_FLAG_TC);
#endif
}
#endif



/*************************
static uint8 tms_get_packet(void)
{
    uint8 temp;

    while(uart_read(UART_TMS, &temp, 1))   //FE 00 23
    {
        //数据分隔符
        if(temp==0x10)
        {
            if(tms_get_flag==0)//数据头判断
            {
                uart_read(UART_TMS, &temp, 1);
                if(temp==0x00)
                {
                    uart_read(UART_TMS, &temp, 1);
                    if(temp==0x23)
                    {
                        tms_get_flag=1;
                        tms_recv_length=0;
                        tms_recv_buf[tms_recv_length++] = 0x10;
                        tms_recv_buf[tms_recv_length++] = 0x02;
                        tms_recv_buf[tms_recv_length++] = 0x33;
                    }
                    else
                        continue;
                }
                else
                    continue;
            }
            else if(tms_get_flag==1) //数据判断
            {
                uart_read(UART_TMS, &temp, 1);
                if(temp==0x03)//数据结尾判断
                {
                    //ptu_print_debug("f3");
                    uart_read(UART_TMS, &temp, 1);//校验码
                    tms_recv_buf[tms_recv_length++] = 0x10;
                    tms_recv_buf[tms_recv_length++] = 0x03;
                    tms_recv_buf[tms_recv_length++] = temp;
                    tms_get_flag =0;

                    if (tms_recv_length==TMS_FRAME_LEN)
                    {
                        //ptu_print_debug("tms_recv_ok");
                        return 1;
                    }
                    else
                    {}
                        //print_int("%x L:", tms_recv_length);
                }
                else  //0x10 为数据部分处理
                {
                    //tms_recv_buf[tms_recv_length++] = 0X10; //当不是结束时保存10数据
                    tms_recv_buf[tms_recv_length++] = temp;

                }
            }
        }
        //数据部分处理
        else if (tms_get_flag==1)
        {
            tms_recv_buf[tms_recv_length++] = temp;
            if( tms_recv_length >= TMS_BUF_SIZE )
                tms_recv_length = 0;
        }
    }
    return 0;
}


******************************************/

//把所有tms数据读到g_recv_tms_buf
static uint8 read_tms_to_buf(void)
{
    uint8 temp;
    uint16 len = 0;
    
    while (uart_read(TMS_READ_UART, &temp, 1))
    {
        TMS_BUFF_BUFF[(TMS_BUFF_LEN++) % TMS_SD_BUFF_SIZE] = temp;
        TMS_RECV_PBUFF.buf[TMS_RECV_PBUFF.in++] = temp;
        TMS_RECV_PBUFF.in %= TMS_RECV_BUFF_SIZE;
        
        if (TMS_RECV_PBUFF.in == TMS_RECV_PBUFF.out)
        {
            TMS_RECV_PBUFF.out++;
        }
        else
        {
            TMS_RECV_PBUFF.len++;
        }
        
        TMS_RECV_PBUFF.out %= TMS_RECV_BUFF_SIZE;
        len++;
        
        if (len >= TMS_FRAM_LEN)
        {
            break;
        }
    }
    
    return 0;
}
//计算g_recv_pis_buf 的长度
static uint16 get_tms_len(void)
{
    return TMS_RECV_PBUFF.len;
}
//从g_recv_pis_buf 读出指定长度数据到pbuf，返回读出的长度
static uint16 read_tms_buf(uint8 *pbuf, uint16 len)
{
    uint16 ret = 0;
    uint16 i;
    
    if (TMS_RECV_PBUFF.len < len && NULL == pbuf)
    {
        ret = 0;
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            pbuf[i] = TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out + i) % TMS_RECV_BUFF_SIZE];
            //TMS_BUFF_BUFF[(TMS_BUFF_LEN++)%TMS_SD_BUFF_SIZE]=TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out+i)%TMS_RECV_BUFF_SIZE];
        }
        
        TMS_RECV_PBUFF.out = (TMS_RECV_PBUFF.out + len) % TMS_RECV_BUFF_SIZE;
        TMS_RECV_PBUFF.len -= len;
        ret = len;
    }
    
    return ret;
}
//从g_recv_pis_buf 删除len个字节
static void delete_tms_buf(uint16 len)
{
    uint16 i;
    
    if (TMS_RECV_PBUFF.len < len)
    {
        TMS_RECV_PBUFF.in = 0;
        TMS_RECV_PBUFF.out = 0;
        TMS_RECV_PBUFF.len = 0;
        return;
    }
    
    for (i = 0; i < len; i++)
    {
        TMS_FRAM_ERROR_BUFF[(TMS_FRAM_ERROR_LENTH++) % TMS_ERROR_SIZE] = TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out + i) % TMS_RECV_BUFF_SIZE];
    }
    
    TMS_RECV_PBUFF.out = (TMS_RECV_PBUFF.out + len) % TMS_RECV_BUFF_SIZE;
    TMS_RECV_PBUFF.len -= len;
    return;
}
#if 1
static uint8 tms_get_packet(void)
{
    uint8 temp;
    uint8 checksum = 0;
    uint16 i, len;
    uint8 ret = 0;
    read_tms_to_buf();
    
    //  TMS_FRAM_ERROR_LENTH = 0;
    while (get_tms_len() >= TMS_FRAM_LEN) //FE 00 23
    {
        //数据分隔符
        temp = TMS_RECV_PBUFF.buf[TMS_RECV_PBUFF.out];
        
        if ((TMS_FRAM_BEGIN == temp) && (TMS_FRAM_END == TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out + TMS_FRAM_LEN - 1) % TMS_RECV_BUFF_SIZE]))
        {
            checksum = 0;
            
            for (i = 1; i < TMS_FRAM_LEN - 2; i++)
            {
                checksum += TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out + i) % TMS_RECV_BUFF_SIZE];
            }
            
            if (checksum == TMS_RECV_PBUFF.buf[(TMS_RECV_PBUFF.out + TMS_FRAM_LEN - 2) % TMS_RECV_BUFF_SIZE])
            {
                len = 0;
                len = read_tms_buf(TMS_DATA_BUFF, TMS_FRAM_LEN);
                
                if (TMS_FRAM_LEN == len)
                {
#if 0
                
                    if (TMS_FRAM_ERROR_LENTH > 0)
                    {
                        atc_tms.comm_flag = 1;
                        atc_tms.comm_time = get_time();
                        //save_tms_data_sd(TMSERR, TMS_FRAM_ERROR_LENTH);
                        TMS_FRAM_ERROR_LENTH = 0;
                    }
                    
#endif
                    ret = 1;
                    break;
                }
                
                //return 1;
            }
        }
        
#if 0
        
        if (TMS_FRAM_ERROR_LENTH >= TMS_ERROR_SIZE)
        {
            atc_tms.comm_flag = 1;
            atc_tms.comm_time = get_time();
            //save_tms_data_sd(TMSERR, TMS_FRAM_ERROR_LENTH);
            TMS_FRAM_ERROR_LENTH = 0;
        }
        
        read_tms_buf((uint8 *)&TMS_FRAM_ERROR_BUFF[TMS_FRAM_ERROR_LENTH], 1);
        TMS_FRAM_ERROR_LENTH++;
#endif
        //从pis_recv_buf 删除一个字节
        delete_tms_buf(1);
    }
    
#if 0
    
    if (TMS_FRAM_ERROR_LENTH > 0)
    {
        atc_tms.comm_flag = 1;
        atc_tms.comm_time = get_time();
        //save_tms_data_sd(TMSERR, TMS_FRAM_ERROR_LENTH);
        TMS_FRAM_ERROR_LENTH = 0;
    }
    
#endif
    return ret;
}
#else
static uint8 tms_get_packet(void)
{
    uint8 temp;

    while (uart_read(UART_TMS, &temp, 1))  //FE 00 23
    {
        //数据分隔符
        if (temp == 0xFE)
        {
            if (tms_get_flag != 1)
            {
                tms_get_flag = 1;
                tms_recv_length = 0;
                memset(tms_recv_buf, 0, TMS_FRAME_LEN);
                tms_recv_buf[tms_recv_length++] = 0xFE;
            }
            else
                tms_recv_buf[tms_recv_length++] = temp;
        }
        //数据部分处理
        else if (tms_get_flag == 1)
        {
            //if((tms_recv_length == (TMS_FRAME_LEN -1)) ||(tms_recv_length == (TMS_FRAME_LEN-9)))
            if (tms_recv_length == (TMS_FRAME_LEN - 1))
            {
                if (temp != 0xFF)
                {
                    tms_get_flag = 0;
                    tms_recv_length = 0;
                    return 0;
                }
                else
                {
                    tms_recv_buf[tms_recv_length++] = temp;
                    return 1;
                }
            }
            else
            {
                //uart_read(UART_TMS, &temp, 1);
                tms_recv_buf[tms_recv_length++] = temp;
            }
        }
    }

    return 0;
}
#endif
/****************************************************
*函数名：tms_proc
*作者：hk
*描述：ATC数据  处理
*输入：无
*输出：无
*返回值：1:收到完整的一条数据  0 :failed
*修改记录：无
****************************************************/
//static u8 recv_tcms_flag_send =0;
//static uint32 delay_recv_tms_timeout = 0;
static uint8 g_tms_to_sd_flag = 0;
uint8 tms_proc(void)
{
    u8 xor_crc = 0 ;
    u8 data_cunt0 = 0 ;
    tRtcTimer time;
    u16 len = 0;
    time_t timep;
    struct tm *p = NULL;
    static uint8 adjust_time_flag = 0;//时间校正
    static u32 atc_com_timer = 0;
    get_cur_time(&time);
    
    if ((check_timer(&atc_com_timer, 512)))
    {
        atc_com_conter++;
    }
    if (atc_tms.comm_flag)
    {
        if ((check_timer(&atc_tms.comm_time, 5000)))
        {
            //存储错误数据帧
            if ( 1 == atc_tms.error)
            {
                //form_error_frame(TMS, 2, time, 0, 0);
                //atc_tms.error=0;
            }
            
            //sd_fflush(TMS, FRAMEERR);
            atc_tms.comm_flag = 0;
            /***********************************/
#if CHOISE_COM
            RS485_UART3_reboot();
#else
            RS485_UART3_reboot();
            RS485_UART2_reboot();
#endif
        }
    }
    
    if (atc_tms.comm_rigth)
    {
        if (check_timer(&atc_tms.comm_rigth_time, 5000))
        {
            if (atc_tms.normal)
            {
                //form_normal_frame(TMS,  2, time, NULL, atc_tms.num_repeat);
                //atc_tms.normal = 0;
                /****************************/
                //memset(old_tms_buf, 0x0, TMS_FRAME_LEN);
            }
            
            //sd_fflush(TMS, FRAMEOK);
            atc_tms.comm_rigth = 0; /*用于记录ATC与TMS之间是否通*/
            //memset(tms_to_pis.buf, 0x0, TMS_FRAME_LEN);
        }
    }
    
    if ((TMS_BUFF_LEN + 400 > TMS_SD_BUFF_SIZE) || (PIS_BUFF_LEN + 400 > PIS_SD_BUFF_SIZE))
    {
        //if (0 != get_sd_ready())
        {
#if 1
            //RS485_UART3_reboot();
            gpio_485_enable(EN_485_ATC);
            uart3_write_char(0x11);
            uart3_write_char(TMS_BUFF_LEN >> 8);
            uart3_write_char(TMS_BUFF_LEN);
            uart3_write_char(0x12);
            //while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
            gpio_485_disable(EN_485_ATC);
#endif
            //save_tms_data_sd(TMSOK3,TMS_BUFF_LEN);
            //save_pis_data_sd(PISOK3,PIS_BUFF_LEN);
            //save_tms_data_sd(TMSERROR4,TMS_FRAM_ERROR_LENTH);
            //save_pis_data_sd(PISERROR4,PIS_FRAM_ERROR_LENTH);
            //save_log_data(MVB_REC, tms_recv_buf, TMS_FRAME_LEN);
            //save_log_data(PIS_REC, pis_recv_buf, PIS_FRAME_LEN);
            TMS_BUFF_LEN = 0;
            PIS_BUFF_LEN = 0;
            TMS_FRAM_ERROR_LENTH = 0;
            PIS_FRAM_ERROR_LENTH = 0;
            //sd_fflush(TMS, FRAMEOK);
            g_tms_to_sd_flag = 1;
        }
        //else
        {
            //g_tms_to_sd_flag = 0;
        }
        //form_normal_frame(TMS,  1, time, (uint8 *)&TMS_BUFF_BUFF[0], TMS_BUFF_LEN);
        //TMS_BUFF_LEN = 0;
        //g_tms_to_sd_flag = 1;
    }
    else
    {
        //g_tms_to_sd_flag = 0;
    }
    
    /*读取判断ATC是否接受到TMS的数据*/
#if CHOISE_COM
    len = uart3_recv_len();
#else
    len = uart2_recv_len();
#endif
    
    if (len > 2 * TMS_FRAME_LEN)
    {
        //atc_tms.comm_flag =1;
        //atc_tms.comm_time = get_time();
        if (tms_get_packet() == 1)
        {
            tms_get_flag = 0;
            tms_recv_length = TMS_FRAM_LEN;
            xor_crc = tms_recv_buf[1];
            
            for (data_cunt0 = 2; data_cunt0 < tms_recv_length - 2; data_cunt0++)
                xor_crc  += tms_recv_buf[data_cunt0];
                
            if (xor_crc == tms_recv_buf[tms_recv_length - 2])
            {
                atc_tms.comm_rigth = 1;
                atc_tms.comm_rigth_time = get_time();
                atc_com_timer = get_time();
                atc_com_conter = 0;
                
                if ((0 == adjust_time_flag)) //校验时间
                {
                    timep = 0;
                    timep   = tms_recv_buf[19];
                    timep |= tms_recv_buf[20] << 8;
                    timep |= tms_recv_buf[17] << 16;
                    timep |= tms_recv_buf[18] << 24;
                    //time(timep);
                    p = localtime(&timep);
                    time.year = (uint8) ((1900 + p->tm_year) % 100);
                    time.month = (uint8)(1 + p->tm_mon);
                    time.date = (uint8)p->tm_mday;
                    time.hour = (uint8)p->tm_hour;
                    time.min = (uint8)p->tm_min;
                    time.sec = (uint8)p->tm_sec;
                    
                    if (set_rtc_time(time))
                    {
                        adjust_time_flag = 1;
                    }
                }
                
                //num_repeat_send = 3;
#if 0
                send_data_to_pis();
                
                if (1 == g_tms_to_sd_flag)
                {
                    //save_tms_data_sd(TMSOK3,TMS_BUFF_LEN);
                    //form_normal_frame(TMS,  1, time, (uint8 *)&TMS_BUFF_BUFF[0], TMS_BUFF_LEN);
                    //TMS_BUFF_LEN = 0;
                    g_tms_to_sd_flag = 0;
                }
                else
                {
                    atc_proc_tms();
                }
                
#endif                  //20190611
                //check_sd_state();
                //if (0 != get_sd_ready())
                {
                    save_tms_data_sd(TMSOK, TMS_FRAME_LEN);
                    //sd_fflush(TMS, FRAMEOK);
                    atc_tms.comm_rigth = 0; /*用于记录ATC与TMS之间是否通*/
                }
            }
            else
            {
                if (0 != get_sd_ready())
                {
                    //save_tms_data_sd(TMSERR, 3);
                }
            }
        }
    }
    
    if (atc_tms.debug)
    {
        if (check_timer(&atc_tms.debug_time, 2000))
        {
            send_tms_ptu(TMS_FRAME_LEN);  //设置输出TMS数据
        }
    }
    
    return 0;
}
/***********************************************************************************************************************
*Function Nane   :  get_pis_state
*Description     :  取得pis的状态
*Input           :  无
*Output          :  无
*Return          :  1 正常 0 异常
*Others          :  无
***********************************************************************************************************************/
uint8 get_pis_state(void)
{
    return atc_pis.comm_rigth;
}
/***********************************************************************************************************************
*Function Nane   :  get_tms_state
*Description     :  取得pis的状态
*Input           :  无
*Output          :  无
*Return          :  !0 : 正常 0 异常
*Others          :  无
***********************************************************************************************************************/
uint8 get_tms_state(void)
{
    return atc_tms.comm_rigth;
}

/***********************************************************************************************************************
*Function Nane   :  get_pis_comm
*Description     :  取得pis的通信状态
*Input           :  无
*Output          :  无
*Return          :  !0 : 通信中 0 非通信状态
*Others          :  无
***********************************************************************************************************************/
uint8 get_pis_comm(void)
{
    return atc_pis.comm_flag;
}
/***********************************************************************************************************************
*Function Nane   :  get_tms_comm
*Description     :  取得tms的通信状态
*Input           :  无
*Output          :  无
*Return          :  !0 : 通信中 0 非通信状态
*Others          :  无
***********************************************************************************************************************/
uint8 get_tms_comm(void)
{
    return atc_tms.comm_flag;
}

/***********************************************************************************************************************
*Function Nane  :  save_pis_data
*Description      :  保存PIS的数据
*Input              :  buf 数据
                          len 长度
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void save_pis_data(uint8 *buf, uint16 len)
{
    if (TMS_BUF_SIZE <= len)
    {
        return;
    }
    
    memcpy(pis_to_tms.buf, buf, len);
    pis_to_tms.len = len;
}


/***********************************************************************************************************************
*Function Nane  : set_tcms_debug
*Description      : 设置debug  tcms数据
*Input              :   type   0 :发送1 不发送
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_tcms_debug(uint8 type)
{
    atc_tms.debug = type;
}
/***********************************************************************************************************************
*Function Nane  : set_pis_debug
*Description      : 设置debug  pis数据
*Input              :  type   0 :发送1 不发送
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_pis_debug(uint8 type)
{
    atc_pis.debug = type;
}

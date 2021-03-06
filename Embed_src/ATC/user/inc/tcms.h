#ifndef _TCMS_H
#define _TCMS_H

#include "stm32f2xx.h"
#include "config.h"



#define TMS_BUF_SIZE  512
#define PIS_BUF_SIZE  512
#define ATC_BUF_SIZE  512              //问题与这里无关

#define  TMS_FRAME_DATA_LEN     150  /*  140 TMS数据帧的数据长度*/
#define  TMS_FRAME_LEN              153   /*  TMS数据帧长度(1+160+2)*/
#define  TMS_FRAME_LEN_TMP              82  /*  TMS数据帧长度(1+160+2)*/
#define  TMS_FRAME_START        0x00  /*  TMS数据帧的帧头*/


#define  PIS_FRAME_DATA_LEN     64  /*   PIS数据帧的数据长度*/
#define  PIS_FRAME_LEN          67    /*  PIS数据帧长度(1+64+2)*/
#define  PIS_FRAME_LEN_CHANGE        6    /* 协议商量好3个字节的数据，FE XX XX XX SRCFE*/

#define  PIS_FRAME_START        0x00  /*  PIS数据帧的帧头*/


#define UART_ATC    UART3
#define UART_TMS    UART2
#define UART_PIS    UART1

#define TMSOK     1
#define TMSERR    2
#define TMSOK3  3
#define TMSERROR4 4
#define PISOK     1
#define PISERR    2
#define PISOK3  3
#define PISERROR4  4

/*ATC接收信息结构体*/
typedef struct ATC_RECV_INFO
{
    uint16 len;
    uint8   buf[ATC_BUF_SIZE];
} tATC_INFO;





/*TMS接收信息结构体*/
typedef struct TMS_RECV_INFO
{
    uint16 len;
    uint8  buf[TMS_BUF_SIZE];
} tTMS_INFO;

/*PIS接收信息结构体*/
typedef struct PIS_RECV_INFO
{
    uint16 len;
    uint8  buf[PIS_BUF_SIZE];
} tPIS_INFO;


typedef struct STATE
{
    uint8  normal;
    uint8  error;
    uint8  num_repeat;
    uint8  comm_flag;
    uint8  comm_rigth;
    uint8  debug;
    uint32  debug_time;
    uint32 timer;
    uint32 comm_time;
    uint32 comm_rigth_time;
} tstate;

extern void train_485_proc(void);
extern uint8 pis_proc(void);
extern uint8 tms_proc(void);
extern void tcms_init(void);

/***********************************************************************************************************************
*Function Nane  :  get_pis_state
*Description      :  取得pis的状态
*Input              :  无
*Output            :  无
*Return            :  1 正常 0 异常
*Others            :  无
***********************************************************************************************************************/
uint8 get_pis_state(void);

/***********************************************************************************************************************
*Function Nane  :  get_tms_state
*Description      :  取得pis的状态
*Input              :  无
*Output            :  无
*Return            :  !0 : 正常 0 异常
*Others            :  无
***********************************************************************************************************************/
uint8 get_tms_state(void);

/***********************************************************************************************************************
*Function Nane   :  get_pis_comm
*Description     :  取得pis的通信状态
*Input           :  无
*Output          :  无
*Return          :  !0 : 通信中 0 非通信状态
*Others          :  无
***********************************************************************************************************************/
uint8 get_pis_comm(void);

/***********************************************************************************************************************
*Function Nane   :  get_tms_comm
*Description     :  取得pis的通信状态
*Input           :  无
*Output          :  无
*Return          :  !0 : 通信中 0 非通信状态
*Others          :  无
***********************************************************************************************************************/
uint8 get_tms_comm(void);


/***********************************************************************************************************************
*Function Nane  :  save_pis_data
*Description      :  保存   pis的数据
*Input              :  buf 数据
                          len 长度
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void save_pis_data(uint8 *buf, uint16 len);

/***********************************************************************************************************************
*Function Nane  : set_tcms_debug
*Description      : 设置debug  tcms数据
*Input              :   type   0 :发送1 不发送
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_tcms_debug(uint8 type);

/***********************************************************************************************************************
*Function Nane  : set_pis_debug
*Description      : 设置debug  pis数据
*Input              :  type   0 :发送1 不发送
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_pis_debug(uint8 type);

#endif

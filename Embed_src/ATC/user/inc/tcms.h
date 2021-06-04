#ifndef _TCMS_H
#define _TCMS_H

#include "stm32f2xx.h"
#include "config.h"



#define TMS_BUF_SIZE  512
#define PIS_BUF_SIZE  512
#define ATC_BUF_SIZE  512              //�����������޹�

#define  TMS_FRAME_DATA_LEN     150  /*  140 TMS����֡�����ݳ���*/
#define  TMS_FRAME_LEN              153   /*  TMS����֡����(1+160+2)*/
#define  TMS_FRAME_LEN_TMP              82  /*  TMS����֡����(1+160+2)*/
#define  TMS_FRAME_START        0x00  /*  TMS����֡��֡ͷ*/


#define  PIS_FRAME_DATA_LEN     64  /*   PIS����֡�����ݳ���*/
#define  PIS_FRAME_LEN          67    /*  PIS����֡����(1+64+2)*/
#define  PIS_FRAME_LEN_CHANGE        6    /* Э��������3���ֽڵ����ݣ�FE XX XX XX SRCFE*/

#define  PIS_FRAME_START        0x00  /*  PIS����֡��֡ͷ*/


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

/*ATC������Ϣ�ṹ��*/
typedef struct ATC_RECV_INFO
{
    uint16 len;
    uint8   buf[ATC_BUF_SIZE];
} tATC_INFO;





/*TMS������Ϣ�ṹ��*/
typedef struct TMS_RECV_INFO
{
    uint16 len;
    uint8  buf[TMS_BUF_SIZE];
} tTMS_INFO;

/*PIS������Ϣ�ṹ��*/
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
*Description      :  ȡ��pis��״̬
*Input              :  ��
*Output            :  ��
*Return            :  1 ���� 0 �쳣
*Others            :  ��
***********************************************************************************************************************/
uint8 get_pis_state(void);

/***********************************************************************************************************************
*Function Nane  :  get_tms_state
*Description      :  ȡ��pis��״̬
*Input              :  ��
*Output            :  ��
*Return            :  !0 : ���� 0 �쳣
*Others            :  ��
***********************************************************************************************************************/
uint8 get_tms_state(void);

/***********************************************************************************************************************
*Function Nane   :  get_pis_comm
*Description     :  ȡ��pis��ͨ��״̬
*Input           :  ��
*Output          :  ��
*Return          :  !0 : ͨ���� 0 ��ͨ��״̬
*Others          :  ��
***********************************************************************************************************************/
uint8 get_pis_comm(void);

/***********************************************************************************************************************
*Function Nane   :  get_tms_comm
*Description     :  ȡ��pis��ͨ��״̬
*Input           :  ��
*Output          :  ��
*Return          :  !0 : ͨ���� 0 ��ͨ��״̬
*Others          :  ��
***********************************************************************************************************************/
uint8 get_tms_comm(void);


/***********************************************************************************************************************
*Function Nane  :  save_pis_data
*Description      :  ����   pis������
*Input              :  buf ����
                          len ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void save_pis_data(uint8 *buf, uint16 len);

/***********************************************************************************************************************
*Function Nane  : set_tcms_debug
*Description      : ����debug  tcms����
*Input              :   type   0 :����1 ������
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void set_tcms_debug(uint8 type);

/***********************************************************************************************************************
*Function Nane  : set_pis_debug
*Description      : ����debug  pis����
*Input              :  type   0 :����1 ������
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void set_pis_debug(uint8 type);

#endif
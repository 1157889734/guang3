#ifndef UART_H
#define UART_H


#include "stm32f2xx.h"
#include "config.h"


#define EN_485_PIS 0  //UART1
#define EN_485_TMS 1  //UART2
#define EN_485_ATC 2  //UART3
#define EN_SD_LED 3  //SD_LED


#define UART1_BUF_LEN 1536
#define UART2_BUF_LEN 1536
#define UART3_BUF_LEN 1536

#define UART1  0
#define UART2  1
#define UART3  2

#define MAX_DELAY   40


//uart1��Ϣ�ṹ��by   bluck on 2013.3.13
typedef struct UART1_INFO
{
    uint8 send_flag;
    u16 in;
    u16 out;
    u8    buf[UART1_BUF_LEN ];
    
} tUART1_INFO;

//uart2��Ϣ�ṹ��
typedef struct UART2_INFO
{
    uint8 send_flag;
    u16 in;
    u16 out;
    u8    buf[UART2_BUF_LEN ];
} tUART2_INFO;


//uart3��Ϣ�ṹ��
typedef struct UART3_INFO
{
    uint8 send_flag;
    u16 in;
    u16 out;
    u8    buf[UART3_BUF_LEN ];
} tUART3_INFO;



extern void gpio_485_en_init(void);
extern void gpio_485_enable(uint8 num);
extern void gpio_485_disable(uint8 num);


/***********************************************************************************************************************
*Function Nane  :  uart_config
*Description    :  ���� uart
*Input          :   uart_id  :����id��
                    baudrate :������
                    parity   :У��λ
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void uart_config(u8 uart_id, uint32_t baudrate, uint16_t parity);

/***********************************************************************************************************************
*Function Nane  :  uart1_irq_recv_byte
*Description    :  uart1�жϽ��յ��ֽ�����
*Input          :  data     :���յĵ��ֽ�����
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
extern void uart1_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart2_irq_recv_byte
*Description    :  uart2�жϽ��յ��ֽ�����
*Input          :  data     :���յĵ��ֽ�����
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
extern void uart2_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart3_irq_recv_byte
*Description    :  uart3�жϽ��յ��ֽ�����
*Input          :  data     :���յĵ��ֽ�����
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
extern void uart3_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart_irq_recv_byte
*Description    :  uart�жϽ��յ��ֽ�����
*Input          :  uart_id  :����id��
                   data     :���յĵ��ֽ�����
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void uart_irq_recv_byte(u8 uart_id, u8 data);


/***********************************************************************************************************************
*Function Nane  : uart1_irq_send_byte
*Description    :  uart1�жϷ��͵��ֽ�����
*Input          :  ��
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void uart1_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  : uart2_irq_send_byte
*Description    :  uart2�жϷ��͵��ֽ�����
*Input          :  ��
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void uart2_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  : uart3_irq_send_byte
*Description    :  uart3�жϷ��͵��ֽ�����
*Input          :  ��
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void uart3_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  :  uart_recv_len
*Description    :  ��ȡuart���ջ�������ݳ���
*Input          :  uart_id  :����id��
*Output         :  ��
*Return         :  ���ݳ���
*Others         :  ��
***********************************************************************************************************************/
u16 uart_recv_len(u8 uart_id);

/***********************************************************************************************************************
*Function Nane    :  uart_read
*Description      :  ��ȡuart���ջ��������
*Input            :  uart_id  :����id��
                     szie     :Ҫ��ȡ���ֽ���
*Output           :  pdata    :ָ���ȡ��ָ��
*Return           :  ʵ�ʶ�ȡ�������ݳ���
*Others           :  ��
***********************************************************************************************************************/
u16 uart_read(u8 uart_id, u8  *pdata,  u16 size);

/***********************************************************************************************************************
*Function Nane  :  uart_write
*Description    :  ��uart д����
*Input          :  uart_id : ����id��
                   length  : Ҫд�����ݳ���
                   buf     : ָ��Ҫд������ָ��
*Output         :  ��
*Return         :  ʵ��д�����ݳ���
*Others         :  ��
***********************************************************************************************************************/
extern u16 uart_write(u8 uart_id, u8 *buf, u16 length);


/***********************************************************************************************************************
*Function Nane     : close_uart_irq
*Description       : �ر�uart�ж�
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
************************************************************************/
void close_uart_irq(void);

/***********************************************************************************************************************
*Function Nane     : open_uart_irq
*Description       :  ��uart�ж�
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
************************************************************************/
void open_uart_irq(void);


/***********************************************************************
*Function Nane     : Uart_Update
*Description       :  ����UART״̬
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
************************************************************************/
void Uart_Update(void);

/***********************************************************************
*Function Nane     : RS485_UART3_reboot
*Description       :  ����3����
*Input             :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
************************************************************************/
void RS485_UART3_reboot(void);
void RS485_UART2_reboot(void);
extern void uart1_write_char(u8 ch);
extern void uart2_write_char(u8 ch);
extern u16 uart2_recv_len(void);
#endif

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


//uart1信息结构体by   bluck on 2013.3.13
typedef struct UART1_INFO
{
    uint8 send_flag;
    u16 in;
    u16 out;
    u8    buf[UART1_BUF_LEN ];
    
} tUART1_INFO;

//uart2信息结构体
typedef struct UART2_INFO
{
    uint8 send_flag;
    u16 in;
    u16 out;
    u8    buf[UART2_BUF_LEN ];
} tUART2_INFO;


//uart3信息结构体
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
*Description    :  配置 uart
*Input          :   uart_id  :串口id号
                    baudrate :波特率
                    parity   :校验位
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart_config(u8 uart_id, uint32_t baudrate, uint16_t parity);

/***********************************************************************************************************************
*Function Nane  :  uart1_irq_recv_byte
*Description    :  uart1中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
extern void uart1_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart2_irq_recv_byte
*Description    :  uart2中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
extern void uart2_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart3_irq_recv_byte
*Description    :  uart3中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
extern void uart3_irq_recv_byte(u8 data);
/***********************************************************************************************************************
*Function Nane  :  uart_irq_recv_byte
*Description    :  uart中断接收单字节数据
*Input          :  uart_id  :串口id号
                   data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart_irq_recv_byte(u8 uart_id, u8 data);


/***********************************************************************************************************************
*Function Nane  : uart1_irq_send_byte
*Description    :  uart1中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart1_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  : uart2_irq_send_byte
*Description    :  uart2中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart2_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  : uart3_irq_send_byte
*Description    :  uart3中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart3_irq_send_byte(void);

/***********************************************************************************************************************
*Function Nane  :  uart_recv_len
*Description    :  读取uart接收缓存的数据长度
*Input          :  uart_id  :串口id号
*Output         :  无
*Return         :  数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart_recv_len(u8 uart_id);

/***********************************************************************************************************************
*Function Nane    :  uart_read
*Description      :  读取uart接收缓存的数据
*Input            :  uart_id  :串口id号
                     szie     :要读取的字节数
*Output           :  pdata    :指向存取的指针
*Return           :  实际读取到的数据长度
*Others           :  无
***********************************************************************************************************************/
u16 uart_read(u8 uart_id, u8  *pdata,  u16 size);

/***********************************************************************************************************************
*Function Nane  :  uart_write
*Description    :  向uart 写数据
*Input          :  uart_id : 串口id号
                   length  : 要写的数据长度
                   buf     : 指向要写的数据指针
*Output         :  无
*Return         :  实际写的数据长度
*Others         :  无
***********************************************************************************************************************/
extern u16 uart_write(u8 uart_id, u8 *buf, u16 length);


/***********************************************************************************************************************
*Function Nane     : close_uart_irq
*Description       : 关闭uart中断
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void close_uart_irq(void);

/***********************************************************************************************************************
*Function Nane     : open_uart_irq
*Description       :  打开uart中断
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void open_uart_irq(void);


/***********************************************************************
*Function Nane     : Uart_Update
*Description       :  更新UART状态
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void Uart_Update(void);

/***********************************************************************
*Function Nane     : RS485_UART3_reboot
*Description       :  串口3重启
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void RS485_UART3_reboot(void);
void RS485_UART2_reboot(void);
extern void uart1_write_char(u8 ch);
extern void uart2_write_char(u8 ch);
extern u16 uart2_recv_len(void);
#endif


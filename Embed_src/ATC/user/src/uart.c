
#include "uart.h"
#include <string.h>
#include <def.h>
#include "stm32f2xx_usart.h"
#include "main.h"
//by bluck on 213.3.13
#include "usb.h"

tUART1_INFO  uart1_recv;
tUART2_INFO  uart2_recv;
tUART3_INFO  uart3_recv;

tUART1_INFO  uart1_send;
tUART2_INFO  uart2_send;
tUART3_INFO  uart3_send;

static uint8 uart2_delay = 0;
static uint8 uart3_delay = 0;

uint8 uart2_max_delay = 0;
uint8 uart3_max_delay = 0;

//GPIOB PING6  PIS

static GPIO_TypeDef *GPIO_PORT[4] = {GPIOE, GPIOD, GPIOD, GPIOB};
//static uint16 GPIO_PIN[4] = {GPIO_Pin_0, GPIO_Pin_7, GPIO_Pin_10};

static uint16 GPIO_PIN[4] = {GPIO_Pin_0, GPIO_Pin_7, GPIO_Pin_10, GPIO_Pin_6};




void Delayus(uint16 us)
{
    uint16 i = 0;
    
    while (i < us)
    {
        i++;
    }
}
void gpio_485_enable(uint8 num)
{
    GPIO_PORT[num]->BSRRL = GPIO_PIN[num];
    GPIO_PORT[num]->BSRRL = GPIO_PIN[num];
}
void gpio_485_disable(uint8 num)
{
    GPIO_PORT[num]->BSRRH = GPIO_PIN[num];
    GPIO_PORT[num]->BSRRH = GPIO_PIN[num];
}

/****************************************************
*函数名：gpio_485_en_init
*作者：hk
*描述：485使能线初始化
*输入：无
*输出：无
*返回值：无
*修改记录：无
****************************************************/
void gpio_485_en_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL   ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    gpio_485_disable(EN_485_PIS);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    gpio_485_disable(EN_485_ATC);
    gpio_485_disable(EN_485_TMS);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;       //SD LED
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    gpio_485_disable(EN_SD_LED);
}

/***********************************************************************************************************************
*Function Nane  :  uart1_config
*Description    :  配置 uart1
*Input          :   baudrate :波特率
                    parity   :校验位
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart1_config(uint32_t baudrate, uint16_t parity)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#if SEND_PIS_CS_INTER
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TXE);
#else
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ClearFlag(USART1, USART_FLAG_TXE);
#endif
    uart1_send.send_flag = 0;
}
/***********************************************************************************************************************
*Function Nane  :  uart2_config
*Description    :  配置 uart2
*Input          :   baudrate :波特率
                    parity   :校验位
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart2_config(uint32_t baudrate, uint16_t parity)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#if SEND_TMS_CS_INTER
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TXE);
#else
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ClearFlag(USART2, USART_FLAG_TXE);
#endif
    uart2_send.send_flag = 0;
}
/***********************************************************************************************************************
*Function Nane  :  uart3_config
*Description    :  配置 uart3
*Input          :   baudrate :波特率
                    parity   :校验位
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart3_config(uint32_t baudrate, uint16_t parity)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
#if SEND_TMS_CS_INTER
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TXE);
#else
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    USART_ClearFlag(USART3, USART_FLAG_TXE);
#endif
    uart3_send.send_flag = 0;
}
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
void uart_config(u8 uart_id, uint32_t baudrate, uint16_t parity)
{
    switch (uart_id)
    {
        case UART1:
        {
            uart1_config(baudrate, parity);
        }
        break;
        
        case UART2:
        {
            uart2_config(baudrate,  parity);
        }
        break;
        
        case UART3:
        {
            uart3_config(baudrate, parity);
        }
        break;
        
        default:
            break;
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart1_irq_recv_byte
*Description    :  uart1中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart1_irq_recv_byte(u8 data)
{
    uart1_recv.buf[uart1_recv.in++] = data;
    
    if ( UART1_BUF_LEN <= uart1_recv.in )
    {
        uart1_recv.in = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart2_irq_recv_byte
*Description    :  uart2中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart2_irq_recv_byte(u8 data)
{
    uart2_recv.buf[uart2_recv.in++] = data;
    
    if ( UART2_BUF_LEN <= uart2_recv.in )
    {
        uart2_recv.in = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart3_irq_recv_byte
*Description    :  uart3中断接收单字节数据
*Input          :  data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart3_irq_recv_byte(u8 data)
{
    uart3_recv.buf[uart3_recv.in++] = data;
    
    if ( UART3_BUF_LEN <= uart3_recv.in )
    {
        uart3_recv.in = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  : uart1_irq_send_byte
*Description    :  uart1中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart1_irq_send_byte(void)
{
    u32 i = 0;
    u32 tmp = 0;
    
    if (uart1_send.out != uart1_send.in )
    {
        USART_SendData(USART1, uart1_send.buf[uart1_send.out++]);
        
        if (UART1_BUF_LEN <= uart1_send.out)
        {
            uart1_send.out = 0;
        }
        
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
    else
    {
        uart1_send.send_flag = 0;
        
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
}
/***********************************************************************************************************************
*Function Nane  : uart2_irq_send_byte
*Description    :  uart2中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart2_irq_send_byte(void)
{
    if (uart2_send.out != uart2_send.in )
    {
        USART_SendData(USART2, uart2_send.buf[uart2_send.out++]);
        
        if (UART2_BUF_LEN <= uart2_send.out)
        {
            uart2_send.out = 0;
        }
    }
    else
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        
        USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        uart2_send.send_flag = 0;
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
        gpio_485_disable(EN_485_TMS);
        Delayus(1500);
        USART_ClearFlag(USART2, USART_FLAG_TC);
        //485接收使能
        uart2_delay = 0;
    }
}

/***********************************************************************************************************************
*Function Nane  : uart3_irq_send_byte
*Description    :  uart3中断发送单字节数据
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart3_irq_send_byte(void)
{
    if (uart3_send.out != uart3_send.in )
    {
        USART_SendData(USART3, uart3_send.buf[uart3_send.out++]);
        
        if (UART3_BUF_LEN <= uart3_send.out)
        {
            uart3_send.out = 0;
        }
    }
    else
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        uart3_send.send_flag = 0;
        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
        gpio_485_disable(EN_485_ATC);       //485接收使能
        Delayus(1500);
        USART_ClearFlag(USART3, USART_FLAG_TC);
        uart3_delay = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart_irq_recv_byte
*Description    :  uart中断接收单字节数据
*Input          :  uart_id  :串口id号
                   data     :接收的单字节数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart_irq_recv_byte(u8 uart_id, u8 data)
{
    switch (uart_id)
    {
        case UART1:
        {
            uart1_irq_recv_byte(data);
        }
        break;
        
        case UART2:
        {
            uart2_irq_recv_byte(data);
        }
        break;
        
        case UART3:
        {
            uart3_irq_recv_byte(data);
        }
        break;
        
        default:
            break;
    }
}

/***********************************************************************************************************************
*Function Nane  :  uart1_recv_len
*Description    :  读取uart1接收缓存的数据长度
*Input          :  无
*Output         :  无
*Return         :  数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart1_recv_len()
{
    if (uart1_recv.out <= uart1_recv.in)
    {
        return (uart1_recv.in - uart1_recv.out);
    }
    else
    {
        return (UART1_BUF_LEN + uart1_recv.in - uart1_recv.out);
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart2_recv_len
*Description    :  读取uart2接收缓存的数据长度
*Input          :  无
*Output         :  无
*Return         :  数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart2_recv_len(void)
{
    if (uart2_recv.out <= uart2_recv.in)
    {
        return (uart2_recv.in - uart2_recv.out);
    }
    else
    {
        return (UART2_BUF_LEN + uart2_recv.in - uart2_recv.out);
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart3_recv_len
*Description    :  读取uart3接收缓存的数据长度
*Input          :  无
*Output         :  无
*Return         :  数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart3_recv_len(void)
{
    if (uart3_recv.out <= uart3_recv.in)
    {
        return (uart3_recv.in - uart3_recv.out);
    }
    else
    {
        return (UART3_BUF_LEN + uart3_recv.in - uart3_recv.out);
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart_recv_len
*Description    :  读取uart接收缓存的数据长度
*Input          :  uart_id  :串口id号
*Output         :  无
*Return         :  数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart_recv_len(u8 uart_id)
{
    u16 len = 0;
    
    switch (uart_id)
    {
        case UART1:
        {
            len = uart1_recv_len();
        }
        break;
        
        case UART2:
        {
            len = uart2_recv_len();
        }
        break;
        
        case UART3:
        {
            len = uart3_recv_len();
        }
        break;
        
        default:
            break;
    }
    
    return len;
}
/***********************************************************************************************************************
*Function Nane    :  uart1_read
*Description      :  读取uart1接收缓存的数据
*Input            :  szie  :要读取的字节数
*Output           :  pdata  指向存取的指针
*Return           :  实际读取到的数据长度
*Others           :  无
***********************************************************************************************************************/
u16 uart1_read(u8  *pdata,  u16 size)
{
    u16 len = 0;
    
    if (NULL == pdata)
    {
        return 0;
    }
    
    while ((uart1_recv.out != uart1_recv.in) && (len < size))
    {
        pdata[len++] = uart1_recv.buf[uart1_recv.out++];
        
        if (UART1_BUF_LEN <= uart1_recv.out)
        {
            uart1_recv.out = 0;
        }
    }
    
    return len;
}
/***********************************************************************************************************************
*Function Nane    :  uart2_read
*Description      :  读取uart2接收缓存的数据
*Input            :  szie  :要读取的字节数
*Output           :  pdata  指向存取的指针
*Return           :  实际读取到的数据长度
*Others           :  无
***********************************************************************************************************************/
u16 uart2_read(u8  *pdata,  u16 size)
{
    u16 len = 0;
    
    if (NULL == pdata)
    {
        return 0;
    }
    
    while ((uart2_recv.out != uart2_recv.in) && (len < size))
    {
        pdata[len++] = uart2_recv.buf[uart2_recv.out++];
        
        if (UART2_BUF_LEN <= uart2_recv.out)
        {
            uart2_recv.out = 0;
        }
    }
    
    return len;
}
/***********************************************************************************************************************
*Function Nane    :  uart3_read
*Description      :  读取uart3接收缓存的数据
*Input            :  szie  :要读取的字节数
*Output           :  pdata  指向存取的指针
*Return           :  实际读取到的数据长度
*Others           :  无
***********************************************************************************************************************/
u16 uart3_read(u8  *pdata,  u16 size)
{
    u16 len = 0;
    
    if (NULL == pdata)
    {
        return 0;
    }
    
    while ((uart3_recv.out != uart3_recv.in) && (len < size))
    {
        pdata[len++] = uart3_recv.buf[uart3_recv.out++];
        
        if (UART3_BUF_LEN <= uart3_recv.out)
        {
            uart3_recv.out = 0;
        }
    }
    
    return len;
}
/***********************************************************************************************************************
*Function Nane    :  uart_read
*Description      :  读取uart接收缓存的数据
*Input            :  uart_id  :串口id号
                     szie     :要读取的字节数
*Output           :  pdata    :指向存取的指针
*Return           :  实际读取到的数据长度
*Others           :  无
***********************************************************************************************************************/
u16 uart_read(u8 uart_id, u8  *pdata,  u16 size)
{
    u16 len = 0;
    //u16 test_len = 0;
    
    switch (uart_id)
    {
        case UART1:
        {
            len = uart1_read(pdata,   size);
        }
        break;
        
        case UART2:
        {
            len = uart2_read(pdata,  size) ;
        }
        break;
        
        case UART3:
        {
            len = uart3_read(pdata,  size) ;
            //test_len = len;
            //  tcms_usb_debug(HEX,pdata, test_len);
            //usb_write_buf(pdata,test_len);
        }
        break;
        
        default:
            break;
    }
    
    return len;
}

/***********************************************************************************************************************
*Function Nane  :  uart1_write_char
*Description    :  向uart1 写数据
*Input          :  ch  :要写的数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart1_write_char(u8 ch)
{
    USART_SendData(USART1, ch);
    
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart2_write_char
*Description    :  向uart2 写数据
*Input          :  ch  :要写的数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart2_write_char(u8 ch)
{
    USART_SendData(USART2, ch);
    
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart3_write_char
*Description    :  向uart3 写数据
*Input          :  ch  :要写的数据
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void uart3_write_char(u8 ch)
{
    USART_SendData(USART3, ch);
    
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        //while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    {
    }
}
/***********************************************************************************************************************
*Function Nane  :  uart1_write
*Description    :  向uart1 写数据
*Input          :  length  : 要写的数据长度
                   buf     : 指向要写的数据指针
*Output         :  无
*Return         :  实际写的数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart1_write( u8 *buf, u16 length)
{
    u16 cnt = 0;
    
    if ((NULL == buf) || (0 == length))
    {
        return 0;
    }
    
    if (UART1_BUF_LEN < length)
    {
        length = UART1_BUF_LEN;
    }
    
    uart1_send.send_flag = 1;
    
    while (cnt < (length))
    {
        uart1_send.buf[uart1_send.in++] = buf[cnt++];
        
        if (UART1_BUF_LEN <= uart1_send.in)
        {
            uart1_send.in = 0;
        }
    }
    
    //UART1发送中断使能,开始发送
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    return cnt;
}
/***********************************************************************************************************************
*Function Nane  :  uart2_write
*Description    :  向uart2 写数据
*Input          :  length  : 要写的数据长度
                   buf     : 指向要写的数据指针
*Output         :  无
*Return         :  实际写的数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart2_write( u8 *buf, u16 length)
{
    u16 cnt = 0;
    
    if ((NULL == buf) || (0 == length))
    {
        return 0;
    }
    
    if (UART2_BUF_LEN <= length)
    {
        length = UART2_BUF_LEN;
    }
    
    while (cnt < length-1)
    {
        uart2_send.buf[uart2_send.in++] = buf[cnt++];
        
        if (UART2_BUF_LEN <= uart2_send.in)
        {
            uart2_send.in = 0;
        }
    }
    
    //uart2_delay=0;
    uart2_send.send_flag = 1;
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    gpio_485_enable(EN_485_TMS); //485发送使能
    Delayus(1200);
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    return 0;
}
/***********************************************************************************************************************
*Function Nane  :  uart3_write
*Description    :  向uart3 写数据
*Input          :  length  : 要写的数据长度
                   buf     : 指向要写的数据指针
*Output         :  无
*Return         :  实际写的数据长度
*Others         :  无
***********************************************************************************************************************/
u16 uart3_write( u8 *buf, u16 length)
{
    u16 cnt = 0;
    
    if ((NULL == buf) || (0 == length))
    {
        return 0;
    }
    
    //while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
    if (UART3_BUF_LEN <= length)
    {
        length = UART3_BUF_LEN;
    }
    
    while (cnt < length)
    {
        uart3_send.buf[uart3_send.in++] = buf[cnt++];
        
        if (UART3_BUF_LEN <= uart3_send.in)
        {
            uart3_send.in = 0;
        }
    }
    
    uart3_delay = 0;
    uart3_send.send_flag = 1;
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    gpio_485_enable(EN_485_ATC); //485发送使能
    Delayus(1200);
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    return cnt;
}
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
u16 uart_write(u8 uart_id, u8 *buf, u16 length)
{
    u16 cnt = 0;
    
    if (NULL == buf)
    {
        return 0;
    }
    
    switch (uart_id)
    {
        case UART1:
        {
            cnt = uart1_write(buf,   length);
        }
        break;
        
        case UART2:
        {
            cnt = uart2_write(buf,   length) ;
        }
        break;
        
        case UART3:
        {
            cnt = uart3_write(buf,   length) ;
        }
        break;
        
        default:
            break;
    }
    
    return cnt;
}
/***********************************************************************************************************************
*Function Nane     : close_uart_irq
*Description       : 关闭uart中断
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void close_uart_irq(void)
{
    USART_Cmd(USART1, DISABLE);
    USART_Cmd(USART2, DISABLE);
    USART_Cmd(USART3, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
}
/***********************************************************************
*Function Nane     : open_uart_irq
*Description       :  打开uart中断
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void open_uart_irq(void)
{
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TXE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TXE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TXE);
}
/***********************************************************************
*Function Nane     : Uart_Update
*Description       :  更新UART状态
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void Uart_Update(void)
{
    if (1 == uart3_send.send_flag)
    {
        //UART3发送400ms后关闭485发送使能
        uart3_delay++;
        
        if (MAX_DELAY < uart3_delay)
        {
            uart3_delay = 0;
            uart3_send.send_flag = 0;
            gpio_485_disable(EN_485_ATC);
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
        }
    }
}
/***********************************************************************
*Function Nane     : RS485_UART3_reboot
*Description       :  串口3重启
*Input             :  无
*Output            :  无
*Return            :  无
*Others            :  无
************************************************************************/
void RS485_UART3_reboot(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    gpio_485_disable(EN_485_ATC);
}

void RS485_UART2_reboot(void)
{
    uart_init(COM2);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    gpio_485_disable(EN_485_TMS);
}






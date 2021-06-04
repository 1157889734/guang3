#include "uart2.h"
#include "stm32f2xx_usart.h"
u8 uart2_recv_buf[UART2_BUF_LEN];
u8 uart2_send_buf[UART2_BUF_LEN];
u16 uart2_recv_fifo_in = 0;
u16 uart2_recv_fifo_out = 0;

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
}

void uart2_write_char(u8 ch)
{
   USART_SendData(USART2, ch);
   while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
   {}
}

void uart2_write_buf(u8 *buf, u16 length)
{
  u16 cnt = 0;
  while(cnt < length)
  {
    uart2_write_char(buf[cnt++]);
  }
}

u8 uart2_read_char(u8 *ch)
{
    if(uart2_recv_fifo_out == uart2_recv_fifo_in)
	return 0;
    *ch = uart2_recv_buf[uart2_recv_fifo_out++];
    if(uart2_recv_fifo_out == UART2_BUF_LEN)
	uart2_recv_fifo_out = 0;
    return 1;
}

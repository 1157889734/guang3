#include "uart1.h"
#include "stm32f2xx_usart.h"
uint8_t uart1_recv_buf[UART1_BUF_LEN];
uint8_t uart1_send_buf[64];
uint16_t uart1_recv_fifo_in = 0;
uint16_t uart1_recv_fifo_out =0;

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

  UART1_DIRECT(UART1_IN);
}

void uart1_write_char(uint8_t ch)
{
   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
   USART_SendData(USART1, ch);
   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
   {}
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void uart1_write_buf(uint8_t *buf, uint16_t length)
{
  uint16_t cnt = 0;
  u32 tick;

	UART1_DIRECT(UART1_OUT);
  for (tick=0; tick<0x3FF; tick++);
  
  while(cnt < length)
  {
    uart1_write_char(buf[cnt++]);
	IWDG_ReloadCounter();
  }
  for (tick=0; tick<0xffff; tick++);
  UART1_DIRECT(UART1_IN);
}

uint8_t uart1_read_char(uint8_t *ch)
{ 
    if(uart1_recv_fifo_out == uart1_recv_fifo_in)
	return 0;
    *ch = uart1_recv_buf[uart1_recv_fifo_out++];
    if(uart1_recv_fifo_out == UART1_BUF_LEN)
	uart1_recv_fifo_out = 0;
    return 1;
}

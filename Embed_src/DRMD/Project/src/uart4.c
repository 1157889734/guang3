#include "uart4.h"
#include "stm32f2xx_usart.h"
u8 uart4_recv_buf[UART4_BUF_LEN];
u8 uart4_send_buf[UART4_BUF_LEN];
u16 uart4_recv_fifo_in = 0;
u16 uart4_recv_fifo_out = 0;

void uart4_config(uint32_t baudrate, uint16_t parity)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = parity;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure); 
  USART_Cmd(UART4, ENABLE);
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void uart4_write_char(u8 ch)
{
   USART_SendData(UART4, ch);
   while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
   {}
}

void uart4_write_buf(u8 *buf, u16 length)
{
  u16 cnt = 0;
  //485使能
 // GPIO_SetBits(GPIOB, GPIO_Pin_5);
  while(cnt < length)
  {
    uart4_write_char(buf[cnt++]);
  }
  //485使能
  //GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

u8 uart4_read_char(u8 *ch)
{
    if(uart4_recv_fifo_out == uart4_recv_fifo_in)
	return 0;
    *ch = uart4_recv_buf[uart4_recv_fifo_out++];
    if(uart4_recv_fifo_out == UART4_BUF_LEN)
	uart4_recv_fifo_out = 0;
    return 1;
}

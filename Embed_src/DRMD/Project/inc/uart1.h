#ifndef UART1_H
#define UART1_H
#include "stm32f2xx.h"

#include "Com_proc.h"


#define UART1_BUF_LEN 512//256

extern uint8_t uart1_recv_buf[UART1_BUF_LEN];
extern uint8_t uart1_send_buf[64];
extern uint16_t uart1_recv_fifo_in;
extern uint16_t uart1_recv_fifo_out;


#define UART1_IN						1
#define UART1_OUT						0


#define UART1_DIRECT(n)				(n) ? (GPIO_ResetBits(GPIOB, GPIO_Pin_9)) : (GPIO_SetBits(GPIOB, GPIO_Pin_9))

extern void uart1_config(uint32_t baudrate, uint16_t parity);
extern void uart1_write_char(uint8_t ch);
extern void uart1_write_buf(uint8_t *buf, uint16_t length);
extern uint8_t uart1_read_char(uint8_t *ch);
#endif

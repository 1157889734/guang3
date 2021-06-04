#ifndef UART2_H
#define UART2_H
#include "stm32f2xx.h"
#define UART2_BUF_LEN 256

extern uint8_t uart2_recv_buf[UART2_BUF_LEN];
extern uint8_t uart2_send_buf[UART2_BUF_LEN];
extern uint16_t uart2_recv_fifo_in;
extern uint16_t uart2_recv_fifo_out;

extern void uart2_config(uint32_t baudrate, uint16_t parity);
extern void uart2_write_char(uint8_t ch);
extern void uart2_write_buf(uint8_t *buf, uint16_t length);
extern uint8_t uart2_read_char(uint8_t *ch);
#endif

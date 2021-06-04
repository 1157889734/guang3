#ifndef UART4_H
#define UART4_H
#include "stm32f2xx.h"
#define UART4_BUF_LEN 256

extern u8 uart4_recv_buf[UART4_BUF_LEN];
extern u8 uart4_send_buf[UART4_BUF_LEN];
extern u16 uart4_recv_fifo_in;
extern u16 uart4_recv_fifo_out;

extern void uart4_config(uint32_t baudrate, uint16_t parity);
extern void uart4_write_char(u8 ch);
extern void uart4_write_buf(u8 *buf, u16 length);
extern u8 uart4_read_char(u8 *ch);
#endif

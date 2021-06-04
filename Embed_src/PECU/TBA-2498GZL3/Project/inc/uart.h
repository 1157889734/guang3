#ifndef __UART_H
#define __UART_H

#include "ISD9xx.h"

extern vu8 uart_tx_delay_time;

void UART_Configuration(void);
void put_n_char(uint8_t *buf, uint16_t n);
uint8_t get_char(uint8_t *pchar);

#endif	/* __UART_H	*/

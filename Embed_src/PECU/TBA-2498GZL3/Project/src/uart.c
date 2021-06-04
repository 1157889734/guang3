#include "uart.h"
#include "DrvUART.h"
#include "DrvGPIO.h"

#include "pecu.h"

#define UART_RXBUF_SIZE			768
uint8_t uart_rxbuf[UART_RXBUF_SIZE];

vu16 uart_rx_wr_index = 0;
vu16 uart_rx_rd_index = 0;
vu16 uart_rx_cnt = 0;

vu8 uart_tx_delay_time = 0;

static void UART_INT_HANDLE(uint32_t u32IntStatus);

uint8_t get_char(uint8_t *pchar)
{
	if(0 == uart_rx_cnt)
		return(0);

	*pchar = uart_rxbuf[uart_rx_rd_index++];
	if(uart_rx_rd_index >= UART_RXBUF_SIZE)
		uart_rx_rd_index = 0;
	
	UNLOCKREG();
	uart_rx_cnt--;
	LOCKREG();

	return(1);
}

void put_n_char(uint8_t *buf, uint16_t n)
{
	EN1483;

	DrvGPIO_ClrBit(PECU_CTRL_PORT, COM_LED_PIN);	// 点亮通讯指示灯

	DrvUART_Write(UART_PORT0, buf, n);

	uart_tx_delay_time = 2;					// 此芯片没有移位寄存器空标志，所以需要延时保证最后一个字节通过RS485正确传输
	while(uart_tx_delay_time);

	DIS1483;

	DrvGPIO_SetBit(PECU_CTRL_PORT, COM_LED_PIN);	// 灭掉通讯指示灯
} 

static void UART_INT_HANDLE(uint32_t u32IntStatus)
{
	uint8_t tmp;

	if(u32IntStatus & RDAIE)
	{
		while(UART0->ISR.RDA_IF == 1)
		{
			DrvUART_Read(UART_PORT0, &tmp, 1);
			uart_rxbuf[uart_rx_wr_index++] = tmp;
			if(uart_rx_wr_index >= UART_RXBUF_SIZE)
				uart_rx_wr_index = 0;

		   	UNLOCKREG();
			uart_rx_cnt++;
			LOCKREG();				  
		}		
	}
}

void UART_Configuration(void)
{
	STR_UART_T sParam;
	
	sParam.u32BaudRate = 9600;
	sParam.u8cDataBits = DRVUART_DATABITS_8;
	sParam.u8cStopBits = DRVUART_STOPBITS_1;
	sParam.u8cParity = DRVUART_PARITY_NONE;
	sParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;
	
	DrvUART_Open(UART_PORT0, &sParam);
	
	DrvUART_EnableInt(UART_PORT0, DRVUART_RDAINT, UART_INT_HANDLE); 			
}

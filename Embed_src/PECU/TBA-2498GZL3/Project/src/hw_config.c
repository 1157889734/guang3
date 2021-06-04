#include "hw_config.h"
#include "DrvGPIO.h"
#include "uart.h"
#include "timer.h"

#include "pecu.h"

static void GPIO_Configuration(void);
static void RCC_Configuration(void);

void board_init(void)
{
	RCC_Configuration();

	DrvGPIO_InitFunction(FUNC_UART0);	// configure PA8 to UART0 TXD, PA9 to RXD
	
	UART_Configuration();
	
	TMR0_Configuration();
	
	GPIO_Configuration();		 
}

static void GPIO_Configuration(void)
{
	DrvGPIO_Open(GPA, 6, IO_OUTPUT);

	DrvGPIO_ClrBit(GPA, 6);

	DrvGPIO_Open(EN1483_PORT, EN1483_PIN, IO_OUTPUT);
	DIS1483;

}

static void RCC_Configuration(void)
{
	UNLOCKREG();
	
	SYSCLK->PWRCON.OSC49M_EN = 1;
	SYSCLK->PWRCON.OSC10K_EN = 1;
	SYSCLK->PWRCON.XTL32K_EN = 1;

	SYSCLK->CLKSEL0.STCLK_S = 3;		// MCU Cortex_M0 systick clock source, 011= clock source from OSC49M internal oscillator divided by 2

	SYSCLK->CLKSEL0.HCLK_S = 0;			// HCLK clock source select, 000 = clock source from internal OSC48M oscillator
	SYSCLK->CLKDIV.HCLK_N = 0 ;			// HCLK clock divide number from HCLK clock source, The HCLK clock frequency = (HCLK clock source frequency) / (HCLK_N + 1)
										// APB PCLK = CLK48M / (HCLK_N + 1)
	SYSCLK->CLKSEL0.OSCFSel = 0;		// OSC48M frequency select
	
	LOCKREG();			
}

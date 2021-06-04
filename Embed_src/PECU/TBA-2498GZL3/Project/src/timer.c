#include "timer.h"
#include "DrvTimer.h"
#include "DrvSYS.h"
#include "main.h"
#include "pecu.h"
#include "uart.h"

static void TMR0_Callback(void);

static void TMR0_Callback(void)		// 10msÖÐ¶Ï
{
	alarm_led_flash_interval++;
	busy_led_flash_interval++;
	com_led_flash_interval++;

	if(uart_tx_delay_time > 0)
		uart_tx_delay_time--;

	if(pecu_com_timeout < PECU_COM_TIMEOUT)
		pecu_com_timeout++;

	if(alarm_timeout < ALARM_TIMEOUT)
		alarm_timeout++;

	if(ptt_timeout < PTT_TIMEOUT)
		ptt_timeout++;

	if(com_delay_time > 0)
		com_delay_time--;

	playback_time++;
		
	test_time++;
}

void TMR0_Configuration(void)
{
	DrvTIMER_Init();
	DrvSYS_SetIPClockSource(E_SYS_TMR0_CLKSRC, 2);		// TIMER0 clock source select6 010 = clock source from HCLK
	DrvTIMER_Open(TMR0, 200, PERIODIC_MODE);				// Using TIMER0 PERIODIC_MODE, 200 tick / SEC
	DrvTIMER_SetTimerEvent(TMR0, 2, (TIMER_CALLBACK)TMR0_Callback, 0);	// Install Callback function "call back" when Interrupt happen twice time
	DrvTIMER_EnableInt(TMR0);							// Enable TIMER0 Interrupt
}




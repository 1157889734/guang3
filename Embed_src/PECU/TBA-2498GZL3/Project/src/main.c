#include "ISD9xx.h"
#include "hw_config.h"
#include "DrvGPIO.h"
#include "uart.h"
#include "timer.h"
#include "play_ctrl.h"
#include "playPCM.h"

#include "pecu.h"

vu16 test_time = 0;
uint16_t i = 0;
uint8_t tmp;
uint8_t tmpbuf[256];

int32_t main(void)
{
	board_init();

	DPWM_init();

	play_port_init();

	PECU_port_init();

	pecu_eqnum = get_addr();

	while(1)
	{
		PECU_run();
	}
}

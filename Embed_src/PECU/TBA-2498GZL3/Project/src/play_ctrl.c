#include "play_ctrl.h"
#include "DrvGPIO.h"
#include "playPCM.h"

int32_t play_ctrl_now = 1;
int32_t play_ctrl_old = 1;

void play_ctrl(void)
{
	play_ctrl_now = DrvGPIO_GetBit(PLAY_CTRL_PORT, PLAY_CTRL_PIN);
	if((play_ctrl_now == 0) && (play_ctrl_old == 1))				// PLAY控制引脚为下降沿
	{
		DrvGPIO_ClrBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);
		playback_start();		
	}
	else if((play_ctrl_now == 1) && (play_ctrl_old == 0))			// PLAY控制引脚为上升沿
	{
		DrvGPIO_SetBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);
		playback_stop();	
	}
	
	play_ctrl_old = play_ctrl_now;	
}

void play_port_init(void)
{
	DrvGPIO_Open(PLAY_STATUS_PORT, PLAY_STATUS_PIN, IO_OUTPUT);		// 设置PLAY状态引脚为输出
	DrvGPIO_SetBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);				// 设置PLAY状态引脚电平为高

	DrvGPIO_Open(PLAY_CTRL_PORT, PLAY_CTRL_PIN, IO_INPUT);			// 设置PLAY控制引脚为输入
	DrvGPIO_SetBit(PLAY_CTRL_PORT, PLAY_CTRL_PIN);					// 设置PLAY控制引脚电平为高
}

#include "play_ctrl.h"
#include "DrvGPIO.h"
#include "playPCM.h"

int32_t play_ctrl_now = 1;
int32_t play_ctrl_old = 1;

void play_ctrl(void)
{
	play_ctrl_now = DrvGPIO_GetBit(PLAY_CTRL_PORT, PLAY_CTRL_PIN);
	if((play_ctrl_now == 0) && (play_ctrl_old == 1))				// PLAY��������Ϊ�½���
	{
		DrvGPIO_ClrBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);
		playback_start();		
	}
	else if((play_ctrl_now == 1) && (play_ctrl_old == 0))			// PLAY��������Ϊ������
	{
		DrvGPIO_SetBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);
		playback_stop();	
	}
	
	play_ctrl_old = play_ctrl_now;	
}

void play_port_init(void)
{
	DrvGPIO_Open(PLAY_STATUS_PORT, PLAY_STATUS_PIN, IO_OUTPUT);		// ����PLAY״̬����Ϊ���
	DrvGPIO_SetBit(PLAY_STATUS_PORT, PLAY_STATUS_PIN);				// ����PLAY״̬���ŵ�ƽΪ��

	DrvGPIO_Open(PLAY_CTRL_PORT, PLAY_CTRL_PIN, IO_INPUT);			// ����PLAY��������Ϊ����
	DrvGPIO_SetBit(PLAY_CTRL_PORT, PLAY_CTRL_PIN);					// ����PLAY�������ŵ�ƽΪ��
}

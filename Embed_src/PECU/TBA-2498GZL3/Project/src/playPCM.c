#include "ISD9xx.h"
#include "DrvGPIO.h"
#include "playPCM.h"
#include "DrvPDMA.h"
#include "DrvDPWM.h"

#include "pecu.h"

#include "audio_file.h"

#define BUFFER_SAMPLECOUNT					0x30

uint32_t BufferSampleCount;
uint32_t BufferReadyAddr;
uint32_t PDMA1Counter;
uint32_t audiotack_index;

__align(4) int16_t audio_buf[2][BUFFER_SAMPLECOUNT];

static void PDMA1forDPWM(void);
static void PDMA1_Callback(void);

void playback_start(void)
{
	BufferSampleCount = BUFFER_SAMPLECOUNT;	
	BufferReadyAddr = (uint32_t)(&audio_buf[0][0]);

	PDMA1forDPWM();
}

void playback_stop(void)
{
	PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.PDMACEN = 0;
	PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.TRIG_EN = 0;

	DrvDPWM_DisablePDMA();

	PDMA1Counter = 0;
	audiotack_index = 0; 	
}

void DPWM_init(void)
{
	DrvPDMA_Init();

	if(!SYSCLK->APBCLK.ANA_EN)
	{
		SYSCLK->APBCLK.ANA_EN = 1;
		SYS->IPRSTC2.ANA_RST = 1;
		SYS->IPRSTC2.ANA_RST = 0;
	}

	DrvDPWM_Open();
	DrvDPWM_SetDPWMClk(E_DRVDPWM_DPWMCLK_HCLKX2);
	
	DrvDPWM_SetSampleRate(16000);
	DrvDPWM_Enable();	
}

static void PDMA1forDPWM(void)
{
	STR_PDMA_T sPDMA;

	sPDMA.sSrcAddr.u32Addr = BufferReadyAddr;
	sPDMA.sDestAddr.u32Addr = (uint32_t)&DPWM->FIFO;
	sPDMA.u8Mode = eDRVPDMA_MODE_MEM2APB;
	sPDMA.u8TransWidth = eDRVPDMA_WIDTH_16BITS;
	sPDMA.sSrcAddr.eAddrDirection = eDRVPDMA_DIRECTION_WRAPAROUND;
	sPDMA.sDestAddr.eAddrDirection = eDRVPDMA_DIRECTION_FIXED;
	sPDMA.u8WrapBcr = eDRVPDMA_WRA_WRAP_HALF_INT;			// Interrupt condition set for Half buffer & buffer end		// For WRAPROUND
	sPDMA.i32ByteCnt = BufferSampleCount * 4;
	DrvPDMA_Open(eDRVPDMA_CHANNEL_1, &sPDMA);

	DrvPDMA_SetCHForAPBDevice(eDRVPDMA_CHANNEL_1, eDRVPDMA_DPWM, eDRVPDMA_WRITE_APB);

	DrvPDMA_EnableInt(eDRVPDMA_CHANNEL_1, eDRVPDMA_WAR);		// For WARPROUND

	DrvPDMA_InstallCallBack(eDRVPDMA_CHANNEL_1, eDRVPDMA_WAR, (PFN_DRVPDMA_CALLBACK)PDMA1_Callback);

	DrvDPWM_EnablePDMA();

	DrvPDMA_CHEnablelTransfer(eDRVPDMA_CHANNEL_1);
} 

static void PDMA1_Callback(void)
{
	uint8_t i;
	
	PDMA1Counter++;
	
	if(PDMA1Counter & 0x01)
	{
		BufferReadyAddr = (uint32_t)(&audio_buf[1][0]);
		for(i = 0; i < BUFFER_SAMPLECOUNT; i++)
		{
			audio_buf[0][i] = audio_table[audiotack_index++] << 8;
			audio_buf[0][i] |= audio_table[audiotack_index++];

			if(audiotack_index >= FILE_LEN)
			{
				audiotack_index = 0;
//				PDMA1Counter = 0;

//				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.PDMACEN = 0;
//				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.TRIG_EN = 0;

//				DrvDPWM_DisablePDMA();

				// 修改
				PDMA1Counter = 0;

				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.PDMACEN = 0;
				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.TRIG_EN = 0;

				DrvDPWM_DisablePDMA();

				playback_over_flag = 1;			// “忙”提示语音循环播放完毕标志置1
			}
		}
	}
	else
	{
		BufferReadyAddr = (uint32_t)(&audio_buf[0][0]);
		for(i = 0; i < BUFFER_SAMPLECOUNT; i++)
		{
			audio_buf[1][i] = audio_table[audiotack_index++] << 8;
			audio_buf[1][i] |= audio_table[audiotack_index++];
			if(audiotack_index >= FILE_LEN)
			{
				audiotack_index = 0;
//				PDMA1Counter = 0;

//				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.PDMACEN = 0;
//				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.TRIG_EN = 0;

//				DrvDPWM_DisablePDMA();

				// 修改
				PDMA1Counter = 0;

				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.PDMACEN = 0;
				PDMA->channel[eDRVPDMA_CHANNEL_1].CSR.TRIG_EN = 0;

				DrvDPWM_DisablePDMA();

				playback_over_flag = 1;			// “忙”提示语音循环播放完毕标志置1
			}
		}
	}	
}

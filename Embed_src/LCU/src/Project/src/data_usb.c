/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: ����USB����
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "uart4.h"
#include "utils.h"
#include "include.h"
#include "data_public.h"
#include "device_info.h"
#include "progupdate.h"
#include "led_control.h"


static u8 gUsbRecvPacketBuf[256];
static u8 gUsbRecvPaketLength = 0;

static u8 UsbDataGetPacket(void)
{
	u8 temp;
	while(usb_read_char(&temp))
	{
		LedUsbOpen();
		if(temp == TBA_PACKET_TAB)
		{
			//�жϳ��� �ж��Ƿ�������
			if(gUsbRecvPaketLength >= 3)
			{
				return 1;
			}
			gUsbRecvPaketLength=0;
		}
		//�����ַ���ֱ���ͻ�����
		else
		{
			gUsbRecvPacketBuf[gUsbRecvPaketLength] = temp;
			if( ++gUsbRecvPaketLength >= 255 )
				gUsbRecvPaketLength = 0;
		}
	}
	return 0;
}



void UsbDataProc()
{
	u8 len;

	if (UsbDataGetPacket()) 
	{
		len = VerifyPaket( gUsbRecvPacketBuf, gUsbRecvPaketLength );
		if ( len > 0 ) 
		{
			if ( gUsbRecvPacketBuf[1] == 2 ) //�汾��ѯ
			{
				PrintDeviceInfo();
			}
			else if ( gUsbRecvPacketBuf[1] == 1 ) //�������
			{
				UsbProgramUpdate( gUsbRecvPacketBuf );
			}
		}
		gUsbRecvPaketLength = 0;
	}

}



/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 设备的相关信息
**  创建日期: 2013.12.20
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "utils.h"
#include "device_info.h"
#include "data_public.h"
#include "include.h"
#include "string.h"




static u8 gDeviceId = 1;

//0:no key     1:have key
static u8 gKeySignal;

u8 GetDeviceId( )
{
	return gDeviceId;
}

void GetSoftwareVersion(u8 *vh, u8 *vl)
{
	*vh = (u8)(DEVICE_SOFTWARE_VERSION_HIGHT);
	*vl = (u8)(DEVICE_SOFTWARE_VERSION_LOW);
}


void SetDeviceId()
{
	u8 bit0;
	u8 temp;
	u16 cnt = 0;


addr_next:
	bit0 = ADDR0_READ();
//	bit1 = ADDR1_READ();

	if ( 0==bit0 )
	{
		temp = 1;
	}
	else 
	{
		temp = 2;
	}

	if (temp == gDeviceId)
	{
		cnt ++;
	}
	else 
	{
		cnt = 0;
	}
	gDeviceId = temp;

	if (cnt < 50)
	{
		goto addr_next;
	}
	
	gDeviceId = temp;
	
	gKeySignal = 0;
}

void PrintDeviceInfo()
{
	
	
//	usb_write_str(buf);
//	print_line(char * str)
}

u8 GetKeySignal()
{
	return gKeySignal;
}

void KeySignalChk(void)
{
	if (KEY_SIGNAL_READ())
	{
		gKeySignal = 0;
	}
	else 
	{
		gKeySignal = 1;
	}
}



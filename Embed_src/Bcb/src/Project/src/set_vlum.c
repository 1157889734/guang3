/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 音量设置子程序set_volume.c  硬件型号M62429P，音频衰减
**  创建日期: 2013.12.31
**
**  当前版本：1.0
**  作者：李军
注意:   操作完成后，保持两个控制脚为低电平
*******************************************************************************
*/


#include "include.h"
#include "hw_config.h"
#include "set_vlum.h"
#include "progupdate.h"

static tVolumeValue gVolumeValue;


u8 VolumeGetMoniBroadcast(void)
{
	return gVolumeValue.moniBroadcast;
}

u8 VolumeGetMoniDriverTalk(void)
{
	return gVolumeValue.moniDriverTalk;
}

u8 VolumeGetMoniPasTalk(void)
{
	return gVolumeValue.moniPasTalk;
}

u8 VolumeGetMicBroadcast(void)
{
	return gVolumeValue.micBroadcast;
}

u8 VolumeGetMicDriverTalk(void)
{
	return gVolumeValue.micDriverTalk;
}

u8 VolumeGetMicPasTalk(void)
{
	return gVolumeValue.micPasTalk;
}


/*
(0db)~(-80db)分成16级
          只取高7bit     0bit不要
-76db: 0x46
-68db: 0x26
-60db: 0x66
-52db: 0x16
-44db: 0x56
-40db: 0xd6
-36db: 0x36
-32db: 0xb6
-28db: 0x76
-24db: 0xf6
-20db: 0x0e
-16db: 0x8e
-12db: 0x4e
-08db: 0xce
-04db: 0x2e
-00db: 0xae

*/
const u8 volume_table[16]= {0x46,0x26,0x66,0x16,0x56,0xd6,0x36,0xb6,0x76,0xf6,0x0e,0x8e,0x4e,0xce,0x2e,0xae};


u8 WriteDataToFlash()
{
	u32 idx;
	 u32 *pWord;
	 u32 len;
	 u32 addr;

	addr = PARAM_ADDRESS;

	gVolumeValue.flag1 = 0x55;
	gVolumeValue.flag2 = 0xaa;

	pWord = (u32 *)&gVolumeValue;

	len = sizeof(gVolumeValue);
	len /=4;

	FLASH_Unlock();
   	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(addr);

    FLASH->KEYR =  ((u32)0x45670123);
    FLASH->KEYR = ((u32)0xCDEF89AB);

    for (idx = 0; idx <  len; idx ++)
    {
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR |= ((u32)0x00000001);
            *(__IO u16*)addr = (u16)pWord[idx];
            break;
        }
        
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            *(__IO u16*)(addr + 2) = (u32)pWord[idx]>>16;
            break;
        }
        
        addr += 4;
        while(FLASH_BUSY != FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR &= ((u32)0x00001FFE);
            break;
        }
        
    }
	return 0;
}


void VolumeInit(void)
{
	u32 i;
	u32 len;
	u8 buf[50];

	FLASH_Unlock();	//FLASH解锁
        FLASH->KEYR =  ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);

	len = sizeof(gVolumeValue);
	for (i=0; i<len; i++)
	{
		buf[i]=(*((volatile u8*)(PARAM_ADDRESS+i)));	
	}
	FLASH_Lock();

	memcpy(&gVolumeValue, buf, sizeof(gVolumeValue));
	if ((gVolumeValue.flag1!=0x55)||(gVolumeValue.flag2!=0xaa))
	{
		gVolumeValue.moniBroadcast = 15;
		gVolumeValue.moniDriverTalk = 15;
		gVolumeValue.moniPasTalk = 15;
		gVolumeValue.micBroadcast = 15;
		gVolumeValue.micDriverTalk = 15;
		gVolumeValue.micPasTalk = 15;			
		gVolumeValue.flag1 = 0x55;
		gVolumeValue.flag2 = 0xaa;
		WriteDataToFlash();
	}
	if (gVolumeValue.moniBroadcast>15) gVolumeValue.moniBroadcast = 15;	
	if (gVolumeValue.moniDriverTalk>15) gVolumeValue.moniDriverTalk = 15;	
	if (gVolumeValue.moniPasTalk>15) gVolumeValue.moniPasTalk = 15;	
	if (gVolumeValue.micBroadcast>15) gVolumeValue.micBroadcast = 15;	
	if (gVolumeValue.micDriverTalk>15) gVolumeValue.micDriverTalk = 15;	
	if (gVolumeValue.micPasTalk>15) gVolumeValue.micPasTalk = 15;
	
}


void SaveVolume(u8 mode, u8 volume)
{
	 u8 flag = 0;

	switch(mode)
	{
		case 0:
			return;
		case 1:
			if (volume > 15) return;
			if (volume == gVolumeValue.moniBroadcast) return;
			gVolumeValue.moniBroadcast = volume;
			break;
		case 2:
			if (volume > 15) return;
			if (volume == gVolumeValue.moniDriverTalk) return;
			gVolumeValue.moniDriverTalk = volume;
			break;
		case 3:
			if (volume > 15) return;
			if (volume == gVolumeValue.moniPasTalk) return;
			gVolumeValue.moniPasTalk = volume;
			break;
		case 4:
			if (volume > 15) return;
			if (volume == gVolumeValue.micBroadcast) return;
			gVolumeValue.micBroadcast = volume;
			break;
		case 5:
			if (volume > 15) return;
			if (volume == gVolumeValue.micDriverTalk) return;
			gVolumeValue.micDriverTalk = volume;
			break;
		case 6:
			if (volume > 15) return;
			if (volume == gVolumeValue.micPasTalk) return;
			gVolumeValue.micPasTalk = volume;
			break;
		case 7:
			if (volume > 15) return;
			if (volume != gVolumeValue.moniBroadcast) flag = 1;
			if (volume != gVolumeValue.moniDriverTalk) flag = 1;
			if (volume != gVolumeValue.moniPasTalk) flag = 1;
			if (volume != gVolumeValue.micBroadcast) flag = 1;
			if (volume != gVolumeValue.micDriverTalk) flag = 1;
			if (volume != gVolumeValue.micPasTalk) flag = 1;
			if (flag == 0) return;
			gVolumeValue.moniBroadcast = volume;
			gVolumeValue.moniDriverTalk = volume;
			gVolumeValue.moniPasTalk = volume;
			gVolumeValue.micBroadcast = volume;
			gVolumeValue.micDriverTalk = volume;
			gVolumeValue.micPasTalk = volume;			
			break;
		default:
			return;
	}

	WriteDataToFlash();

}


/*---------------------set_volume--------------------------*/


void SPI_delay(u32 tick)
{
	volatile u8 i;
	while(tick--){
		for (i=0; i<100; i++);
	}
}


void SetVolClk(u8 st)
{
	if (st) {
		VOLUME_CLK_HIGH();
	}
	else {
		VOLUME_CLK_LOW();
	}
}

void SetVolData(u8 st)
{
	if (st) {
		VOLUME_DATA_HIGH();
	}
	else {
		VOLUME_DATA_LOW();
	}
}

/*
只传输高7bit
*/
void SetVolSendByte(u8 byte_data)
{
	u8 i;
	u8 temp;

	temp = byte_data;
	for (i=0; i<7; i++) {
		if (temp & 0x80) {
			SetVolData(1);
		}
		else {
			SetVolData(0);
		}
		SPI_delay(5);
		SetVolClk(1);
		SPI_delay(5);
		SetVolData(0);
		SPI_delay(5);
		SetVolClk(0);
		SPI_delay(5);		
		temp <<= 1;
	}
}


/*
type:设置广播音量还是对讲音量
volume:设置音量值，分级别
*/
void SetVolume(eSetVolumeType type)	
{
	u8 vol_data;
	u8 ch;

	static eSetVolumeType moni_type_old;
	static u8 moni_value_old = 0xff;
	static eSetVolumeType mic_type_old;
	static u8 mic_value_old = 0xff;

	switch (type)
	{
		case VOLUME_TYPE_MONI_BROADCAST:
			vol_data = gVolumeValue.moniBroadcast;
			if ((moni_type_old == type)&&(moni_value_old==vol_data)) return;
			moni_type_old = type;
			moni_value_old = vol_data;
			ch = 1;
			break;
		case VOLUME_TYPE_MONI_DRIVER_TALK:
			vol_data = gVolumeValue.moniDriverTalk;
			if ((moni_type_old == type)&&(moni_value_old==vol_data)) return;
			moni_type_old = type;
			moni_value_old = vol_data;
			ch = 1;
			break;
		case VOLUME_TYPE_MONI_PAS_TALK:
			vol_data = gVolumeValue.moniPasTalk;
			if ((moni_type_old == type)&&(moni_value_old==vol_data)) return;
			moni_type_old = type;
			moni_value_old = vol_data;
			ch = 1;
			break;
		case VOLUME_TYPE_MIC_BROADCAST:
			vol_data = gVolumeValue.micBroadcast;
			if ((mic_type_old == type)&&(mic_value_old==vol_data)) return;
			mic_type_old = type;
			mic_value_old = vol_data;
			ch = 0;
			break;
		case VOLUME_TYPE_MIC_DRIVER_TALK:
			vol_data = gVolumeValue.micDriverTalk;
			if ((mic_type_old == type)&&(mic_value_old==vol_data)) return;
			mic_type_old = type;
			mic_value_old = vol_data;
			ch = 0;
			break;
		case VOLUME_TYPE_MIC_PAS_TALK:
			vol_data = gVolumeValue.micPasTalk;
			if ((mic_type_old == type)&&(mic_value_old==vol_data)) return;
			mic_type_old = type;
			mic_value_old = vol_data;
			ch = 0;
			break;
		default:
			return;
	}

	if (vol_data > 15) vol_data = 15;
	vol_data = volume_table[vol_data];
	
	//发送通道号
	SetVolClk(0);
	SPI_delay(50);
	SetVolData(ch);	
	SPI_delay(5);
	SetVolClk(1);
	SPI_delay(5);
	SetVolData(0);
	SPI_delay(5);
	SetVolClk(0);
	SPI_delay(5);

	//只设置一个通道
	SetVolData(1);	
	SPI_delay(5);
	SetVolClk(1);
	SPI_delay(5);
	SetVolData(0);
	SPI_delay(5);
	SetVolClk(0);
	SPI_delay(5);

	//发送衰减值	
	SetVolSendByte(vol_data);

	//发送D9
	SetVolData(1);	
	SPI_delay(5);
	SetVolClk(1);
	SPI_delay(5);
	SetVolData(0);
	SPI_delay(5);
	SetVolClk(0);
	SPI_delay(5);

	//发送D10
	SetVolData(1);	
	SPI_delay(5);
	SetVolClk(1);
	SPI_delay(5);
	SetVolData(1);
	SPI_delay(5);
	SetVolClk(0);
	SPI_delay(5);
	SetVolData(0);
	SPI_delay(5);
	
	return;
}  


extern u8 BoxDataSend( u8* buf, u8 length );
void DebugPrintVolume(void)
{
	u8 buf[100];

	buf[0] = gVolumeValue.moniBroadcast;
	buf[1] = gVolumeValue.moniDriverTalk;
	buf[2] = gVolumeValue.moniPasTalk;
	buf[3] = gVolumeValue.micBroadcast;
	buf[4] = gVolumeValue.micDriverTalk;
	buf[5] = gVolumeValue.micPasTalk;
	buf[6] = gVolumeValue.flag1;
	buf[7] = gVolumeValue.flag2;
	//BoxDataSend(buf, 8);
}





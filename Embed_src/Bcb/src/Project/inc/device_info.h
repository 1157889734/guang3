#ifndef DATA_DEVICE_INFO_H_
#define DATA_DEVICE_INFO_H_

#include "include.h"
#include "data_public.h"

#define DEVICE_SOFTWARE_VERSION_HIGHT			1
#define DEVICE_SOFTWARE_VERSION_LOW				6


extern u8 GetDeviceId( );
extern void GetSoftwareVersion(u8 *vh, u8 *vl);
extern void PrintDeviceInfo();
extern void SetDeviceId();
extern void KeySignalChk(void);
extern u8 GetKeySignal();

#endif



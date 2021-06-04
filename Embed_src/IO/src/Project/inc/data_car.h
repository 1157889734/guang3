#ifndef DATA_CAR_H_
#define DATA_CAR_H_

#include "data_public.h"



extern void CarDataProc(void);
extern void CarDataStartToSend(tStationStat stationStat);
extern void CarDataInit();
extern u8 CarDataSend( u8* buf, u8 length );
extern void MapInit(void);
extern void CarDataClearDmp();

#endif



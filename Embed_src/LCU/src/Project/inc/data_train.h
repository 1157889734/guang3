#ifndef DATA_TRAIN_H_
#define DATA_TRAIN_H_

#include "data_ehp.h"
#include "data_public.h"


#define LCU_MAX_NUM			6





extern void TrainDataProc(void);
extern eEhpBcbStat TrainDataGetEhpStat( u8 deviceId );
extern void TrainDataInit(void);
extern u8 TrainDataSend( u8* buf, u8 length );

#endif



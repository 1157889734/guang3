#ifndef STAT_SWITCH_H_
#define STAT_SWITCH_H_

#include "data_public.h"
#include "device_info.h"


//״̬
typedef enum{
	TALK_STAT_IDLE = 0,
	TALK_STAT_BROADCAST,
	TALK_STAT_REQUEST_BROADCAST,
	TALK_STAT_DRIVER_TALK,
	TALK_STAT_PAS_TALK,
}eTalkStat;



extern eTalkStat GetTalkStat(void);
extern void TalkStatSwitch(void);
extern void TalkStatInit(void);
extern u8 GetDriverTalkStat( void );
extern tLogicFlag GetRequestBroadcastFlag(  void );
extern tLogicFlag GetNowBroadcastFlag(  void );

#endif


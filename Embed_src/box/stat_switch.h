#ifndef STAT_SWITCH_H_
#define STAT_SWITCH_H_


#include "general.h"



//״̬
typedef enum{
	TALK_STAT_IDLE = 0,
	TALK_STAT_ALARM,
	TALK_STAT_CONN,
}eTalkStat;


extern void TalkStatSwitch(void);
extern void TalkStatInit(void);
extern void SetTalkStat(eTalkStat stat);
extern eTalkStat GetTalkStat();

#endif


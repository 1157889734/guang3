#ifndef DATA_TRAIN_H_
#define DATA_TRAIN_H_


#include "data_public.h"


#define LCU_MAX_NUM						6
#define EHP_MAX_NUM						3    //½ô¼±±¨¾¯Æ÷¸öÊý


//±¾µØ¿ØÖÆÆ÷Ñ²¼ì½ô¼±±¨¾¯Æ÷×´Ì¬
typedef enum {
	EHP_STAT_IDLE = 0,
	EHP_STAT_BUSY,
	EHP_STAT_ANSWER,
	EHP_STAT_HANGUP,
}eEhpBcbStat;


typedef enum{
	PA_TYPE_OCC = 0x10,
	PA_TYPE_MANUAL = 0x20,
	PA_TYPE_DVA = 0x08,
	PA_TYPE_MEDIA = 0x04,
	PA_TYPE_EME = 0x01,
	PA_TYPE_CLOSE_DOOR = 0x02,
	PA_TYPE_NULL = 0,
}tPaType;

typedef struct{
	tPaType type;
	u8 occ;
	u8 manual;
	u8 dva;
	u8 lcd_media;
	u8 eme;
	u8 close_door;
	u32 tick_485;
}tPaPriority;


extern void TrainDataProc(void);
extern void TrainDataInit(void);
extern u8 TrainDataSend( u8* buf, u8 length );
extern u8 GetOtherSidePasTalkStat( void );
extern u8 GetOtherSideDriverTalkStat( void );
extern u32 GetTrainComTick(void);
extern u8 PaArbitrate(void);
extern u8 GetOtherSidePaStat( void );
extern u8 GetOtherSideKeySignal( void );

#endif



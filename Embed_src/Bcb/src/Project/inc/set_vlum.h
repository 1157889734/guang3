#define VOICE_VALUE_OFF 0x50
#define MIC_ANN_DOWN_VOICE 0x14
#define TALK_MONI_VOLUM 0x00

#define MIN_MIC_VOLICE 40
#define MIN_MONI_VOLICE 40

typedef struct{
	u8 moniBroadcast;
	u8 moniDriverTalk;
	u8 moniPasTalk;
	u8 micBroadcast;
	u8 micDriverTalk;
	u8 micPasTalk;
	u8 flag1;
	u8 flag2;	
	u8 reserve[4];
}tVolumeValue;


typedef enum{
	VOLUME_TYPE_MONI_BROADCAST,
	VOLUME_TYPE_MONI_DRIVER_TALK,
	VOLUME_TYPE_MONI_PAS_TALK,
	VOLUME_TYPE_MIC_BROADCAST,
	VOLUME_TYPE_MIC_DRIVER_TALK,
	VOLUME_TYPE_MIC_PAS_TALK,
}eSetVolumeType;


extern void SetVolume(eSetVolumeType type);
extern void SaveVolume(u8 mode, u8 value);
extern void VolumeInit(void);
extern u8 VolumeGetMoniBroadcast(void);
extern u8 VolumeGetMoniDriverTalk(void);
extern u8 VolumeGetMoniPasTalk(void);
extern u8 VolumeGetMicBroadcast(void);
extern u8 VolumeGetMicDriverTalk(void);
extern u8 VolumeGetMicPasTalk(void);



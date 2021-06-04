#ifndef KEY_H_
#define KEY_H_

#define KEY_SCAN_DEBOUNCE				10  ///base 10 ms


#define KEY_QUEUE_NUM			10



typedef enum{
	KEY_TYPE_NULL=0,
	KEY_TYPE_IDLE=1,  //·ÇPA  ·ÇCC
	KEY_TYPE_CC=2,
	KEY_TYPE_PA=3,
	KEY_TYPE_PC_CONN=4,
	KEY_TYPE_PC_HANGUP=5,
	KEY_TYPE_PTT_DOWN=6,
	KEY_TYPE_PTT_UP=7,
	KEY_TYPE_PC=8,	
	KEY_TYPE_PCRST=9,
}eKeyType;


typedef struct {
	eKeyType key_queue[KEY_QUEUE_NUM];
	byte q_in;
	byte q_out;
}tKey;

extern byte ScanKey();
extern void KeyProc();
extern eKeyType GetKeyValue(void);
extern byte uart1_busy();

#endif


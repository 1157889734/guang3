#ifndef COM_H_
#define COM_H_

#include "key.h"
#include "general.h"


extern void Uart1StartSend(unsigned char  *buf,  unsigned char len);
extern void Uart2StartSend(unsigned char  *buf,  unsigned char len);
extern byte LcdSend( byte* buf, byte length );
extern byte BoxDataSend( byte* buf, byte length );
extern void BoxDataSendRepeat(void);
extern void BoxDataProc(void);
extern void LcdDataProc(void);
extern void BoxDataInit(void);
extern void BoxDataStartToSend( eKeyType keyType );
extern byte GetUartIfSend(void);
extern byte PackageData( byte *destBuf, byte *srcBuf, byte length );
extern byte VerifyPaket(byte *paketBuf, byte length);

extern void BoxDataUpdateKeyPa(byte stat);
extern void BoxDataUpdateKeyCc(byte stat);
extern void BoxDataUpdateKeyPc(byte stat);
extern void BoxDataUpdateKeyPtt(byte stat);
extern void BoxDataUpdateKeyPcRst(byte stat);
extern void BoxDataUpdateKeyLedTest(byte stat);

#endif


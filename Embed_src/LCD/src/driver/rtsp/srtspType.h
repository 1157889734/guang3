#ifndef __SRTSPTYPE_H__
#define __SRTSPTYPE_H__

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h> 	

#define USER_AGENT_HEADER  "User-Agent: simplertsp v20051123\r\n"
typedef struct _RtspSt_
{
	unsigned fCSeq;
	char *UserAgentHeaderStr;

	unsigned parseBufferSize;
	char *fBaseURL;
	char *fLastSessionId;

	unsigned long fMaxPlayEndTime;

	unsigned VTimestampFrequency;
	unsigned ATimestampFrequency;

	unsigned char VPayloadType;
	unsigned char APayloadType;
	int PotPos;
	unsigned long VFrameRate;
	uint32_t fixed_vop_rate;
	unsigned long vloopNum;
	
	uint32_t *bitstream;

	int rtpNumber;
	int rtcpNumber;
} RTSP_T;

struct ResultData
{
	unsigned char *buffer;
	int len;
	unsigned char fRTPPayloadFormat;
	unsigned long frtpTimestamp;
	int frtpMarkerBit;
};

typedef struct {
    long len;
    uint32_t timestamp;
    double time_sec;
    int fps;
    uint8_t pt;
    uint8_t *data;
    long curLen;
} rtp_frame;

#ifdef __cplusplus
extern "C"{
#endif

int Urltokenize(char *urlname, char **host, char **port, char **path);
int SelectReadRtspc( int socket, int maxMsec );

#ifdef __cplusplus
}
#endif


#endif // __SRTSPTYPE_H__


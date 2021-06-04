/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

**********/

#ifndef _RTSP_CLIENT_H
#define _RTSP_CLIENT_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include "srtspType.h"

#define MAX_READBUFSIZE 20000
#define MAX_PACKET_SIZE 10000

#define TIMEOUT_RECONNECT_TIME 5000 // 超时重连时间, 5 秒

struct MediaAttribute
{
	unsigned fVideoFrequency;
	unsigned char fVideoPayloadFormat;
	char fConfigAsc[100];
	unsigned char fConfigHex[50];
	int fConfigHexLen;
	unsigned fVideoFrameRate;
	unsigned fTimeIncBits;
	unsigned fixed_vop_rate;
	unsigned fVideoWidth;
	unsigned fVideoHeight; 
	unsigned fAudioFrequency;
	unsigned char fAudioPayloadFormat;
	unsigned fTrackNum;
};

struct MediaSubsession 
{
	char * fMediumName;
	char * fCodecName;
	char * fControlPath;// "a=control:<control-path>" 
	char * fConfig;
	unsigned short fClientPortNum;
	unsigned fNumChannels;
	unsigned frtpTimestampFrequency;
	unsigned char fRTPPayloadFormat;
	char * fSessionId;
	char *fFileName;
	FILE* fFid;
	struct MediaSubsession *fNext;
};




int blockUntilwriteable(int socket);
int blockUntilReadable(int socket);
char* parseSDPLine(char* inputLine);
				   
int parseResponseCode(char* line, unsigned int * responseCode);
int getResponse(int socketNum,char* responseBuffer,unsigned responseBufferSize) ;

char* getLine(char* startOfLine);

int parseRTSPURL(char* url,char* address,int* portNum, RTSP_T *pRtsp); 

int setupMediaSubsession(int socketNum,struct MediaSubsession* subsession, RTSP_T *pRtsp) ;

char* parseSDPAttribute_rtpmap( char* sdpLine,unsigned* rtpTimestampFrequency,unsigned *fnumChannels) ;//Check for a "a=rtpmap:<fmt> <codec>/<freq>line
char * parseSDPAttribute_control(char* sdpLine) ;//Check for a "a=control:<control-path>" line
int parseSDPAttribute_range(char* sdpLine, RTSP_T *pRtsp);//Check for a "a=range:npt=<startTime>-<endTime>" line
char * parseSDPAttribute_fmtp(char* sdpLine) ;//Check for a "a=fmtp:" line



int networkReadHandler(int socketNum,unsigned *NextTCPReadSize,struct ResultData* data, RTSP_T *pRtsp ) ;
unsigned char* skip(unsigned char * buffer,unsigned numBytes);
int handleRead(int socketNum,unsigned char* buffer,unsigned bufferMaxSize,unsigned* bytesRead,unsigned* NextTCPReadSize) ;


int getSDPDescriptionFromURL(int socketNum,char* url,char* Description, RTSP_T *pRtsp);
int openConnectionFromURL(char* url, RTSP_T *pRtsp);


struct MediaSubsession * initializeWithSDP(char* sdpDescription,int *SubsessionNum, RTSP_T *pRtsp); 
int GetMediaAttrbute(struct MediaAttribute *Attribute,struct MediaSubsession * subsession,int subsessionNum, RTSP_T *pRtsp);

int setupStreams(int socketNum,struct MediaSubsession *subsession,int subsessionNum, RTSP_T *pRtsp);
int startPlayingStreams(int socketNum,struct MediaSubsession *subsession,int subsessionNum, RTSP_T *pRtsp); 
int teardownMediaSession(int socketNum, RTSP_T *pRtsp);
void resumeStreams(int socketNum, RTSP_T *pRtsp) ;
int pauseMediaSession(int socketNum, RTSP_T *pRtsp);
int playMediaSession(int socketNum,int start,int end, RTSP_T *pRtsp);


char* strDup(char* str) ;
char* strDupSize(char* str);
int hex_a(unsigned char *hex, char *a,unsigned char length);
int a_hex( char *a,unsigned char *hex,unsigned char len);

extern int init_rtsp(char *url,struct MediaAttribute *Attribute, RTSP_T *pRtsp);
extern int RTP_ReadHandler(int socketNum,struct ResultData* data, RTSP_T *pRtsp );
extern void clearup(int socketNum, RTSP_T *pRtsp);


#endif


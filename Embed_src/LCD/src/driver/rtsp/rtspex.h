/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**********/

#ifndef _RTSP_H
#define _RTSP_H

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

#ifdef __cplusplus
extern "C"{
#endif

int init_rtsp(char *url,struct MediaAttribute *Attribute, RTSP_T *pRtsp);
int RTP_ReadHandler(int socketNum,struct ResultData* data, RTSP_T *pRtsp );
void clearup(int socketNum, RTSP_T *pRtsp);
int rtp_h264_parse( struct ResultData *packData, rtp_frame * fr );
int rtp_audio_parse( struct ResultData *packData, rtp_frame * fr );

#ifdef __cplusplus
}
#endif

#endif


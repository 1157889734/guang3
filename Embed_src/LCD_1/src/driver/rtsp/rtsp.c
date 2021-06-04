/**********
This library is FREE software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.
**********/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <semaphore.h>

#include "rtsp.h"
//#include "bitstream.h"
#include "fmtp.h"

#include "bh_malloc.h"

#ifdef NO_CHECK_MALLOC
#define MALLOC(len)  	malloc(len)
#define FREE(ptr)	     	free(ptr);ptr=NULL
#endif

void free_MediaSubsession(struct MediaSubsession *subsession)
{	
	struct MediaSubsession* subsession_tmp = subsession, *subsession_tmp1;
	
	while(subsession_tmp)
	{
		subsession_tmp1=subsession_tmp;
		if(subsession_tmp->fMediumName) FREE(subsession_tmp->fMediumName);
		if(subsession_tmp->fCodecName) FREE(subsession_tmp->fCodecName);
		if(subsession_tmp->fControlPath) FREE(subsession_tmp->fControlPath);
		if(subsession_tmp->fConfig) FREE(subsession_tmp->fConfig);
		if(subsession_tmp->fSessionId) FREE(subsession_tmp->fSessionId);
		if(subsession_tmp->fFileName) FREE(subsession_tmp->fFileName);
		subsession_tmp=subsession_tmp->fNext;
		if(subsession_tmp1) FREE(subsession_tmp1);
	}
}

int init_rtsp(char *url,struct MediaAttribute *Attribute, RTSP_T *pRtsp )
{
	char* sdpDescription = NULL;
	struct MediaSubsession *subsession = NULL;
	int subsessionNum = 0;

	int socketNum = -1;
	int result  = -1;

	pRtsp->fBaseURL = strDup(url);
	socketNum = openConnectionFromURL(pRtsp->fBaseURL, pRtsp);
	if(socketNum<0)
	{
		//fprintf(stderr,"failed to open the URL\n");
		//post_sem();
		return (-1);
	}
	sdpDescription = (char*)MALLOC(MAX_READBUFSIZE*sizeof(char));
	if(sdpDescription == NULL)
	{
		fprintf(stderr,"failed to alloc the memory\n");
		//post_sem();
		return (-1);
	}
	memset(sdpDescription,0,MAX_READBUFSIZE);
	result = getSDPDescriptionFromURL(socketNum,pRtsp->fBaseURL,sdpDescription,pRtsp);
	if (result<0) 
	{
		fprintf(stderr,"Failed to get a SDP description from URL\n");
		if(sdpDescription) FREE(sdpDescription);
		close(socketNum);
		//post_sem();
		return (-1);
	}
	subsession = initializeWithSDP(sdpDescription, &subsessionNum, pRtsp);
	if(subsession == NULL || (((int)(subsession->fCodecName))==1))
	{
		fprintf(stderr,"Failed to initialize a SDP description\n");
		if(sdpDescription) FREE(sdpDescription);
		close(socketNum);
		return (-1);
	}
	if(-1==GetMediaAttrbute(Attribute,subsession,subsessionNum, pRtsp))
	{
		if(sdpDescription) FREE(sdpDescription);
		free_MediaSubsession(subsession);
		close(socketNum);
		return (-1);
	}
	if(-1==setupStreams(socketNum,subsession,subsessionNum, pRtsp))
	{
		if(sdpDescription) FREE(sdpDescription);
		free_MediaSubsession(subsession);
		close(socketNum);
		return (-1);
	}
	if(startPlayingStreams(socketNum,subsession,subsessionNum, pRtsp)==0)
	{	
		if(sdpDescription) FREE(sdpDescription);
		free_MediaSubsession(subsession);
		close(socketNum);
		return (-1);
	}

	if(sdpDescription) FREE(sdpDescription);
	free_MediaSubsession(subsession);

	return socketNum;
}

int RTP_ReadHandler(int socketNum,struct ResultData* data, RTSP_T *pRtsp )
{
	unsigned char c;
	unsigned char streamChannelId;
	unsigned short size;
	unsigned fNextTCPReadSize;
	int result = 0;
	do 
	{
		do 
		{	
			if( 0 >= SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME ) )
			{
				return -1;
			}
			result = recv(socketNum, &c, 1, 0);
			
			if (result <=0) 
			{ 	
				// error reading TCP socket
				//fprintf(stderr,"pRtsp->vloopNum is : %lu\n",pRtsp->vloopNum);
				fprintf(stderr,"error reading TCP socket\n");
				return -1;
			}
									
		} while (c != '$');


   		if( 0 >= SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME ))
		{
			return -1;
		}
		if (recv(socketNum, &streamChannelId, 1, 0)!= 1) break;
    
		if( 0 >= SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME ))
		{
			return -1;
		}
		if (recv(socketNum, (unsigned char*)&size, 2,0) != 2) break;
		fNextTCPReadSize = ntohs(size);
		
		return networkReadHandler(socketNum,&fNextTCPReadSize,data, pRtsp ); 

	} while (0);
	return -1;
}



void clearup(int socketNum, RTSP_T *pRtsp)
{
	//sem_wait(&bin_sem);
	//teardownMediaSession(socketNum);
	
	if(pRtsp->fBaseURL !=NULL) 
	{
		FREE(pRtsp->fBaseURL);
		pRtsp->fBaseURL = NULL;
	}
	if(pRtsp->fLastSessionId !=NULL) 
	{
		FREE(pRtsp->fLastSessionId);
		pRtsp->fLastSessionId = NULL;
	}

	if(socketNum>=0)
	{
			close(socketNum);
	}
	//post_sem();
}

int startPlayingStreams(int socketNum,struct MediaSubsession *subsession,int subsessionNum, RTSP_T *pRtsp)
{

	if(playMediaSession(socketNum,0,-1,pRtsp))
	{
		fprintf(stderr,"Play MediaSession failed\n");
		return 0;
	}
	//fprintf(stderr,"Play MediaSession successful\n");
	return 1;
}


int setupStreams(int socketNum,struct MediaSubsession *subsession,int subsessionNum, RTSP_T *pRtsp) 
{
	struct MediaSubsession *mediasub;
	mediasub = subsession;
	while(subsessionNum>0)
	 {
			if(setupMediaSubsession(socketNum,mediasub, pRtsp)) 
			{	
				fprintf(stderr,"Setup MediaSubsession Failed\n");
				return -1;
			}
			mediasub = mediasub->fNext;
			//clearup_pRtsp_info(pRtsp);
			subsessionNum--;
	 }
	return 0;
	//fprintf(stderr,"Setup Streams successful\n");
}

int handleRead(int socketNum,unsigned char* buffer,
				unsigned bufferMaxSize,unsigned *bytesRead,unsigned* NextTCPReadSize) 
{
	int readSuccess = -1;
	unsigned totBytesToRead;
	unsigned curBytesToRead;
	unsigned curBytesRead = 0;
	int ret;

	if (socketNum < 0) 
	{
		fprintf(stderr,"no socket active\n");
		return -1;
	}
	else 
	{
		// Read from the TCP connection:
		*bytesRead = 0;
		totBytesToRead = *NextTCPReadSize;
		*NextTCPReadSize = 0;
    
		if (totBytesToRead > bufferMaxSize) totBytesToRead = bufferMaxSize; 
    
		curBytesToRead = totBytesToRead;
		#if 1
		while ( curBytesToRead > 0 )
		{
			ret = SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME );
			if( ret <= 0 )
			{
				return -1;
			}
			curBytesRead = recv(socketNum,&buffer[*bytesRead], curBytesToRead,0);
			if( curBytesRead > 0 )
			{
				(*bytesRead) += curBytesRead;
				if ((*bytesRead) >= totBytesToRead) break;
				curBytesToRead -= curBytesRead;
			}
			else
			{
				return -1;
			}
		}
		#else
		while ((curBytesRead = recv(socketNum,&buffer[*bytesRead], curBytesToRead,0)) > 0) 
		{
      			(*bytesRead) += curBytesRead;
			if ((*bytesRead) >= totBytesToRead) break;
			curBytesToRead -= curBytesRead;
			
		}
		#endif
		if (curBytesRead <= 0) 
		{
			*bytesRead = 0;
			readSuccess = -1;
		} 
		else 
		{
			readSuccess = 0;
		}
	}

	return readSuccess;
}


unsigned char* skip(unsigned char * buffer,unsigned numBytes) 
{
  buffer = buffer + numBytes;
	return buffer;
} 



int networkReadHandler(int socketNum,unsigned *NextTCPReadSize,
						struct ResultData* data, RTSP_T *pRtsp )
{
#if 0
	
	
	typedef struct {
#ifdef WORDS_BIGENDIAN
		uint32_t ver:2;
		uint32_t pad:1;
		uint32_t ext:1;
		uint32_t cc:4;
#else
		uint32_t cc:4;		  //!< source count
		uint32_t ext:1; 	  //!< extension flag
		uint32_t pad:1; 	  //!< padding flag
		uint32_t ver:2; 	  //!< version
#endif
#ifdef WORDS_BIGENDIAN
		uint32_t mark:1;
		uint32_t pt:7;
#else
		uint32_t pt:7;		  //!< payload type
		uint32_t mark:1;	  //!< marker flag 
#endif
		uint32_t seq:16;	  //!< sequence number
		uint32_t time;		  //!< timestamp
		uint32_t ssrc;		  //!< synchronization source identifier
	} RTP_HEAD;
	---

	unsigned int leftLen = *NextTCPReadSize;
	int ret;
	int hadRecvLen = 0;
	char buf[1024*1024];
	RTP_HEAD rtpHead;
	int playType;
	while( leftLen > 0 )
	{
		ret = recv( socketNum, &buf[hadRecvLen], leftLen, 0);
		hadRecvLen += ret;
		leftLen = leftLen - ret;		
		memcpy( &rtpHead, buf, sizeof(rtpHead) );
		if( rtpHead.mark == 1 )
		{
			mark = 1;
		}
		playType = rtpHead.pt;
	}
	fprintf(stderr, "playType is %d\n", playType);
	if( 96 == playType)
	{
		mark = 0;
	}
	return 100;
#else	
	unsigned bytesRead;
	unsigned datasize;
	unsigned rtpHdr;
	unsigned char *buffer;
	int rtpMarkerBit;
	unsigned long rtpTimestamp;
	
	unsigned rtpSSRC;
	unsigned cc;
	unsigned char payloadType = 0;

	unsigned extHdr;
	unsigned remExtSize;
	unsigned numPaddingBytes;
	unsigned short rtpSeqNo;	
	unsigned char *bufptr;//[MAX_PACKET_SIZE];

	unsigned char *cpptr;
	unsigned  cplen;

	int ret = -1;

	buffer = data->buffer;
	bufptr = (unsigned char *)MALLOC(MAX_PACKET_SIZE*sizeof(unsigned char));
	if(bufptr == NULL)
	{
		fprintf(stderr,"alloc failed\n");
		return -1;
	}
	//memset(bufptr,0,MAX_PACKET_SIZE);
	do 
	{
		if(handleRead(socketNum,buffer,MAX_PACKET_SIZE,&bytesRead,NextTCPReadSize)) break;
		memcpy(bufptr,buffer,bytesRead);
		datasize = bytesRead;   

		// Check for the 12-byte RTP header:
		if (datasize < 12) break;
		
		cpptr = bufptr + 12;
		cplen = datasize - 12;
		
		rtpHdr = ntohl(*(unsigned*)(buffer)); 
		buffer = skip(buffer,4);
		datasize -=4;
		rtpMarkerBit = (rtpHdr&0x00800000) >> 23;
		rtpTimestamp = ntohl(*(unsigned*)(buffer));
		buffer = skip(buffer,4);
		datasize -=4;
		rtpSSRC = ntohl(*(unsigned*)(buffer)); 
		buffer = skip(buffer,4);
		datasize -=4;
    
		if ((rtpHdr&0xC0000000) != 0x80000000) break;
    
		cc = (rtpHdr>>24)&0xF;
		if (datasize < cc) break;
		buffer = skip(buffer,cc);
		datasize-=cc;
		
		// Check for (& ignore) any RTP header extension
		if (rtpHdr&0x10000000) 
		{
		//	fprintf(stderr,"1 datasize is %u\n",datasize);
			if (datasize < 4) break;
			extHdr = ntohl(*(unsigned*)(buffer)); 
			buffer = skip(buffer,4);
			datasize -=4;
			remExtSize = 4*(extHdr&0xFFFF);
			if (datasize < remExtSize) break;
			buffer = skip(buffer,remExtSize);
			datasize -= remExtSize;
		//	fprintf(stderr,"2 datasize is %u\n",datasize);
      
		}
    
		// Discard any padding bytes:
		if (rtpHdr&0x20000000) 
		{
			if (datasize == 0) break;
			numPaddingBytes	= (unsigned)(buffer)[datasize-1];
			if (datasize < numPaddingBytes) break;
			if (numPaddingBytes > datasize) numPaddingBytes = datasize;
			datasize -= numPaddingBytes;
		}    

		// Check the Payload Type.
		payloadType = (unsigned char)((rtpHdr&0x007F0000)>>16);

		rtpSeqNo = (unsigned short)(rtpHdr&0xFFFF);

		if(payloadType >= 96 ) // 注意: 这个音视频界限只是临时的
		{				
			//memcpy(data->buffer,buffer,datasize);
			pRtsp->VPayloadType = payloadType;
			memcpy(data->buffer,cpptr,cplen);
			data->len = cplen;
			data->fRTPPayloadFormat = payloadType;
			data->frtpTimestamp = rtpTimestamp;
			data->frtpMarkerBit = rtpMarkerBit;
			ret = 0;			
		}
		else
		{
			pRtsp->APayloadType = payloadType;
			datasize -=4;
			memcpy(buffer,buffer+4,datasize);
			//memcpy(data->buffer,buffer,datasize);
			memcpy(data->buffer,cpptr,cplen);
			//data->len = datasize;
			data->len = cplen;
			data->fRTPPayloadFormat = payloadType;
			data->frtpTimestamp = rtpTimestamp;
			data->frtpMarkerBit = rtpMarkerBit;

			ret = 0;
		}
	} while (0);
	FREE(bufptr);
	
	return ret;
#endif
}

int parseResponseCode(char* line, unsigned int * responseCode) 
{
	if (sscanf(line, "%*s%u", responseCode) != 1) 
	{
		fprintf(stderr,"no response code in line\n");
		return -1;
	}

	return 0;
}

int setupMediaSubsession(int socketNum,struct MediaSubsession* subsession, RTSP_T *pRtsp) 
{
	int fSocketNum = socketNum;
	char* lineStart;
	char* cmd = NULL;
	unsigned int cmdSize; 
	unsigned const readBufSize = 10000;
	char *readBuffer;//[readBufSize+1]; 
	char* readBuf ;
	char cmdFmt[]= "SETUP %s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n"
			"%s\r\n";
	char* firstLine;
	char* nextLineStart;
	unsigned responseCode;
	int bytesRead;
	char *SessionId;
	char *Sessionstr;
	readBuffer = (char *)MALLOC((readBufSize+1)*sizeof(char));
	SessionId = (char *)MALLOC((readBufSize+1)*sizeof(char));
	if(fSocketNum<0) return -1;
	
	if(readBuffer == NULL||SessionId == NULL) return -1;
	memset(readBuffer,0,readBufSize+1);
	readBuf = readBuffer;
	do 
	{
		if (pRtsp->fLastSessionId != NULL) 
		{
			Sessionstr = (char *)MALLOC((20+strlen(pRtsp->fLastSessionId))*sizeof(char)); 
			if(Sessionstr == NULL)
			{
				fprintf(stderr,"Sessionstr failed to alloc the memory\n");
				FREE(readBuffer);
				FREE(SessionId);
				return -1;
			}
			sprintf(Sessionstr, "Session: %s\r\n", pRtsp->fLastSessionId);
		} 
		else 
		{
			Sessionstr = "";
		}
		
		pRtsp->rtcpNumber = pRtsp->rtpNumber + 1;

		//char *ptrt = "trackID=2";
		if( NULL == subsession->fControlPath )
		{
			subsession->fControlPath = MALLOC(10);
			strcpy(subsession->fControlPath,"trackID=2");
		}
		#if 0
		cmdSize = strlen(cmdFmt)
				+ strlen(pRtsp->fBaseURL) + strlen(subsession->fControlPath)
				+ 20 /* max int len */
				+ 2*5 /* max port len */
				+ strlen(Sessionstr)
				+ strlen(pRtsp->UserAgentHeaderStr);
		#endif
		cmdSize = 1024;
		cmd = (char *)MALLOC((cmdSize+1)*sizeof(char));
		if(cmd == NULL)
		{
			fprintf(stderr,"cmd failed to alloc the memory\n");
			if(Sessionstr[0]!='\0') FREE(Sessionstr);
			FREE(readBuffer);
			FREE(SessionId);
			return -1;
		}
		memset(cmd,0,cmdSize);
		sprintf(cmd, cmdFmt,
			pRtsp->fBaseURL, subsession->fControlPath,
			++pRtsp->fCSeq,
			pRtsp->rtpNumber, pRtsp->rtcpNumber,Sessionstr,pRtsp->UserAgentHeaderStr);
		pRtsp->rtpNumber +=2;

		//fprintf(stderr,"SETUP command-%d:\n%s\n",pRtsp->fCSeq,cmd);
		
		if (send(fSocketNum,cmd,strlen(cmd),0)<0) 
		{
			fprintf(stderr,"SETUP send() failed\n");
			break;
		}
		
		// Get the response from the server:
  
		bytesRead = getResponse(fSocketNum,readBuf, readBufSize);
		if (bytesRead <= 0) break;
    
		//fprintf(stderr,"SETUP response-%d:\n%s\n",pRtsp->fCSeq,readBuf);

		// Inspect the first line to check whether it's a result code 200
		firstLine = readBuf;
		nextLineStart = getLine(firstLine);
		if (parseResponseCode(firstLine, &responseCode)) break;
		if (responseCode != 200) 
		{
			fprintf(stderr,"cannot handle SETUP response\n");
			break;
		}
		while (1) 
		{
			lineStart = nextLineStart;
			if (lineStart == NULL||lineStart[0] == '\0') break;
			nextLineStart = getLine(lineStart);
			if (sscanf(lineStart, "Session: %s",SessionId ) == 1)
			{
				subsession->fSessionId = strDup(SessionId);
				if(pRtsp->fLastSessionId!=NULL) FREE(pRtsp->fLastSessionId);
				pRtsp->fLastSessionId = strDup(SessionId);
				break;
			}	////////////////
		}
		if (subsession->fSessionId == NULL) 
		{
			fprintf(stderr,"Session header is missing in the response\n");
			break;
		}
		if(Sessionstr[0]!='\0') FREE(Sessionstr);
		FREE(SessionId);
		FREE(cmd);
		FREE(readBuffer);
		return 0;
	} while (0);

	if(Sessionstr[0]!='\0') FREE(Sessionstr);
	FREE(SessionId);
	FREE(cmd);
	FREE(readBuffer);
	return -1;
}

////////////////////////////

int getResponse(int socketNum,char* responseBuffer,unsigned responseBufferSize) 
{
	int fSocketNum;
	char *lastToCheck=NULL;
	char* p = NULL;//responseBuffer;
	int bytesRead = 0; // because we've already read the first byte
	unsigned bytesReadNow  = 0;
	fSocketNum = socketNum;

	if (responseBufferSize == 0) return 0; // just in case...
	*(responseBuffer) = '\0';

	while (bytesRead < (int)responseBufferSize) 
	{
		lastToCheck = NULL;
	#if 0
	 	if(blockUntilReadable(fSocketNum)<=0)
		{
			fprintf(stderr,"socket is unreadable\n");
			break;
		}
	#else
		if( 0 >= SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME ) )
		{
			return -1;
		}
	#endif
				
		bytesReadNow = recv(fSocketNum,(unsigned char*)(responseBuffer+bytesRead),1, 0);
		if (bytesReadNow != 1) 
		{
			fprintf(stderr,"RTSP response was truncated\n");
			break;
		}
		bytesRead++;
    
  
		lastToCheck = responseBuffer+bytesRead-4;
		if (lastToCheck < responseBuffer) continue;
		p = lastToCheck;
		
		if (*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n') 
		{
			*(responseBuffer+bytesRead)= '\0';
			// Before returning, trim any \r or \n from the start:
			while (*responseBuffer == '\r' || *responseBuffer == '\n'||*responseBuffer!=0x52) 
			{
				++responseBuffer;
				--bytesRead;
			}
			if(*responseBuffer == 0x52)//&&responseBuffer[1]=='T'&&responseBuffer[2]=='S'&&responseBuffer[3]=='P')

			{
			return bytesRead;
				}

		}
		else
		{
		
		}
	}
	return bytesRead;
}
/////////////////////
char* parseSDPLine(char* inputLine)
{
	char *ptr;
	for (ptr = inputLine; *ptr != '\0'; ++ptr) 
	{
		if (*ptr == '\r' || *ptr == '\n') 
		{
			++ptr;
			while (*ptr == '\r' || *ptr == '\n') ++ptr;
			if (ptr[0] == '\0') ptr = NULL; // special case for end
			break;
		}
	}

	if (inputLine[0] == '\r' || inputLine[0] == '\n') return ptr;
	if (strlen(inputLine) < 2 || inputLine[1] != '='
		    || inputLine[0] < 'a' || inputLine[0] > 'z') 
	{
		fprintf(stderr,"Invalid SDP line \n");
		return NULL;
	}

	return ptr;
}

char* parseSDPAttribute_rtpmap(char* sdpLine,unsigned* rtpTimestampFrequency,unsigned *fnumChannels) 
{
	unsigned rtpmapPayloadFormat;
	unsigned numChannels = 1;
	char* codecName = strDupSize(sdpLine); // ensures we have enough space

/*
	char* codecName;
	unsigned int len;
	if (sdpLine == NULL) codecName= NULL;
	len = strlen(sdpLine) + 1;
	codecName = (char *)MALLOC(len*sizeof(char));
*/
	if(codecName == NULL)
	{
		return NULL;
	}
	
	if (sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u/%u",
			&rtpmapPayloadFormat, codecName, rtpTimestampFrequency,
			&numChannels) == 4
		|| sscanf(sdpLine, "a=rtpmap: %u %[^/]/%u",
			&rtpmapPayloadFormat, codecName, rtpTimestampFrequency) == 3
		|| sscanf(sdpLine, "a=rtpmap: %u %s",
			&rtpmapPayloadFormat, codecName) == 2) 
	{
		*fnumChannels = numChannels;  
		char* my_codecName=strDup(codecName);
		if(codecName) FREE(codecName);
		return my_codecName;

	}
	
	if(codecName) FREE(codecName);

	return NULL;
}

char * parseSDPAttribute_control(char* sdpLine) 
{
  
	char* controlPath = strDupSize(sdpLine); // ensures we have enough space

#if 0
	char* controlPath;
	unsigned int len;
	if (sdpLine == NULL) controlPath= NULL;
	len = strlen(sdpLine) + 1;
	controlPath = (char *)MALLOC(len*sizeof(char));
#endif
	if(controlPath == NULL)
	{
		return NULL;
	}

	if (sscanf(sdpLine, "a=control:%s", controlPath) == 1) 
	{
		char *my_controlPath=strDup(controlPath);
		FREE(controlPath);
		return my_controlPath;
	}
	FREE(controlPath);

	return NULL;
}

int parseSDPAttribute_range(char* sdpLine, RTSP_T *pRtsp) 
{
	int parseSuccess = -1;
	char playEndTime[32];
	char temp[32];
	int index = 0;
	int j = 0;
	unsigned long endtime = 0;
	char *endstr;
	int k;
	int i,num;
	if (sscanf(sdpLine, "a=range:npt=0-  %s", playEndTime)==1) 
	{
		parseSuccess = 0;
		for(index = 0;index<strlen(playEndTime);index++)
		{
			if(playEndTime[index]!='.')
			{
				temp[j] = playEndTime[index];
				j++;
			}
			else
			{
				for(k=index+1;k<strlen(playEndTime);k++)
				{
					temp[j] = playEndTime[k];
					j++;
					pRtsp->PotPos++;
				}
				break;
			}

		}
		temp[j] = '\0';
		
		
		endtime = strtoul(temp,&endstr,10);

		if (endtime > pRtsp->fMaxPlayEndTime) 
		{
			if(pRtsp->PotPos>=3)
			{
				num = 1;
				for(i = 0;i<pRtsp->PotPos-3;i++)
				{
					num = 10*num;
				}
				pRtsp->fMaxPlayEndTime = endtime/num;
			}
			else
			{
				num = 1;
				for(i = 0;i<3-pRtsp->PotPos;i++)
				{
					num = 10*num;
				}
				pRtsp->fMaxPlayEndTime = endtime*num;
			}
			pRtsp->PotPos = 1000;
			
		}
		//fprintf(stderr,"pRtsp->fMaxPlayEndTime is %lu\n",pRtsp->fMaxPlayEndTime);
	}

	return parseSuccess;
}


char * parseSDPAttribute_fmtp(char* sdpLine) 
{
#if 0
	char* lineCopy;
	char* line;
	char* valueStr;
	char* c;
	do 
	{
		if (strncmp(sdpLine, "a=fmtp:", 7) != 0) break; 
		sdpLine += 7;
		while (isdigit(*sdpLine)) ++sdpLine;

		lineCopy = strDup(sdpLine); 
		line = lineCopy;
		for (c = line; *c != '\0'; ++c) *c = tolower(*c);
    
		while (*line != '\0' && *line != '\r' && *line != '\n') 
		{
			valueStr = strDupSize(line);
			if (sscanf(line, " config = %[^; \t\r\n]", valueStr) == 1) 
			{
				FREE(lineCopy);
				
				return strDup(valueStr);
      			}
      			FREE(valueStr);

			while (*line != '\0' && *line != '\r' && *line != '\n'&& *line != ';') ++line;
			while (*line == ';') ++line;
		}
		FREE(lineCopy);
	} while (0);
	return NULL;
#else
	char* lineCopy;
	char valueStr[1024];
	char* c;
	int i;
	do 
	{
		if (strncmp(sdpLine, "a=fmtp:", 7) != 0) break; 
		sdpLine += 7;
		while (isdigit(*sdpLine)) ++sdpLine;

		lineCopy = strDup(sdpLine); 
		for (c = lineCopy, i = 0; *c != '\r' && *c != '\n'; ++c) 
		{
			valueStr[i++] = *c;
	    	}
	    	valueStr[i] = '\0';
		FREE(lineCopy);				
		return strDup(valueStr);
	
	} while (0);

	return NULL;
	
#endif
}


int GetMediaAttrbute(struct MediaAttribute *Attribute,struct MediaSubsession * subsession,int subsessionNum, RTSP_T *pRtsp)
{
	struct MediaSubsession *mediasub;
	int len = 0;
	mediasub = subsession;
	while(subsessionNum>0)
	{
		if( NULL != mediasub && mediasub->fCodecName != NULL )
		{
			//if(((int)(mediasub->fCodecName))<100) return -1;
			if(strcmp(mediasub->fCodecName,"MP4V-ES") == 0)
			{	
				Attribute->fVideoFrequency = mediasub->frtpTimestampFrequency;
				Attribute->fVideoPayloadFormat = mediasub->fRTPPayloadFormat;
				len = strlen(mediasub->fConfig);
				memcpy(Attribute->fConfigAsc,mediasub->fConfig,len);
				a_hex(mediasub->fConfig,Attribute->fConfigHex,len);
				Attribute->fConfigHexLen = len/2;
				//parse_vovod(Attribute->fConfigHex,Attribute->fConfigHexLen,
				//		&(Attribute->fVideoFrameRate),&(Attribute->fTimeIncBits),
				//		&(Attribute->fVideoWidth),&(Attribute->fVideoHeight), pRtsp);
				
				Attribute->fixed_vop_rate = pRtsp->fixed_vop_rate;
				pRtsp->VFrameRate = Attribute->fVideoFrameRate;
			}
			else if(strcmp(mediasub->fCodecName,"MPA") == 0||strcmp(mediasub->fCodecName,"mpeg4-generic") == 0)
			{	
				Attribute->fAudioFrequency = mediasub->frtpTimestampFrequency;
				Attribute->fAudioPayloadFormat = mediasub->fRTPPayloadFormat;
				Attribute->fTrackNum = mediasub->fNumChannels;
			}	
			else if( NULL != strstr(mediasub->fCodecName,"264") )
			{				
				fmtp_parser( mediasub->fConfig, Attribute->fConfigHex, &Attribute->fConfigHexLen );
			}
			mediasub = mediasub->fNext;
		}
		subsessionNum--;
	}
	return 0;
}

struct MediaSubsession * initializeWithSDP(char* sdpDescription,int *SubsessionNum, RTSP_T *pRtsp)
{
	int Num = 0;
	char*  sdpLine = sdpDescription;
	char* nextSDPLine = NULL;
	struct MediaSubsession *fSubsessionsHead;
	struct MediaSubsession *fSubsessionsTail;
	struct MediaSubsession* subsession;
	char* mediumName=NULL;
	char * CodecName=NULL;
	char * ControlPath=NULL;
	char *Config=NULL;
	unsigned payloadFormat;
	if (sdpDescription == NULL) return NULL;
	fSubsessionsHead = fSubsessionsTail = NULL;
	while (1) 
	{
		if ((nextSDPLine = parseSDPLine(sdpLine)) == NULL)
		{
			return NULL;		
		}
		if (sdpLine[0] == 'm') 
		{
			Num++;
			break;
		}
		sdpLine = nextSDPLine;
		if (sdpLine == NULL) break; // there are no m= lines at all 
		if (!parseSDPAttribute_range(sdpLine, pRtsp)) continue;//check a=range:npt=
	}
    
	while (sdpLine != NULL) 
	{
		subsession = (struct MediaSubsession*)MALLOC(sizeof(struct MediaSubsession));
		if (subsession == NULL) 
		{
			fprintf(stderr,"Unable to create new MediaSubsession\n");
			return NULL;
		}
		memset(subsession, 0x00, sizeof(struct MediaSubsession));

		if (fSubsessionsTail == NULL) 
		{
			fSubsessionsHead = fSubsessionsTail = subsession;
		} 
		else 
		{
			fSubsessionsTail->fNext = subsession;
			subsession->fNext = NULL;
			fSubsessionsTail = subsession;
			
		}

		mediumName = strDupSize(sdpLine);
    
		if (sscanf(sdpLine, "m=%s %hu RTP/AVP %u",mediumName, &subsession->fClientPortNum, &payloadFormat) != 3
			|| payloadFormat > 127) 
		{
			fprintf(stderr,"Bad SDP line\n");
			FREE(mediumName);
			return NULL;
		}
    
		subsession->fMediumName = strDup(mediumName);
		FREE(mediumName);
		subsession->fRTPPayloadFormat = payloadFormat;
		
		subsession->fControlPath = NULL;
		subsession->fConfig		 = NULL;
		while (1) 
		{
			sdpLine = nextSDPLine;//a=rtpmap...
			if (sdpLine == NULL) break; // we've reached the end
			if ((nextSDPLine = parseSDPLine(sdpLine)) == NULL)
			{
				*SubsessionNum = Num;
				return fSubsessionsHead;			
				//下移一行
			}

			if (sdpLine[0] == 'm') 
			{
				Num ++;
				break; // we've reached the next subsession
			}
			// Check for various special SDP lines that we understand:
			//CodecName = strDupSize(sdpLine);	
			if (( CodecName=parseSDPAttribute_rtpmap(sdpLine,&(subsession->frtpTimestampFrequency),&(subsession->fNumChannels)))!=NULL)
			{
				subsession->fCodecName = strDup(CodecName);
				if(strcmp(CodecName,"MP4V-ES") == 0)
				{	
					pRtsp->VTimestampFrequency = subsession->frtpTimestampFrequency;
					pRtsp->VPayloadType = subsession->fRTPPayloadFormat;
				}
				else if(strcmp(CodecName,"MPA") == 0||strcmp(CodecName,"mpeg4-generic") == 0)
				{	
					pRtsp->ATimestampFrequency = subsession->frtpTimestampFrequency;
					pRtsp->APayloadType = subsession->fRTPPayloadFormat;
				}
				if(CodecName)FREE(CodecName);
				continue;
			}
			if(CodecName) FREE(CodecName);

	
			//ControlPath = strDupSize(sdpLine);
			if ((ControlPath = parseSDPAttribute_control(sdpLine))!=NULL)
			{
				subsession->fControlPath = strDup(ControlPath);
				FREE(ControlPath);
				continue;
			}
			if(ControlPath)FREE(ControlPath);

			//Config = strDupSize(sdpLine);
			if ((Config = parseSDPAttribute_fmtp(sdpLine))!=NULL)
			{
				subsession->fConfig = strDup(Config);
				FREE(Config);
				continue;
			}
			if(Config) FREE(Config);
			
		}
		FREE(mediumName);
	}
	*SubsessionNum = Num;
	return fSubsessionsHead;
  
}
int blockUntilwriteable(int socket)//, struct timeval* timeout)
{
	int result = -1;
	unsigned numFds;
	fd_set wd_set;
	do 
	{
		FD_ZERO(&wd_set);
		if (socket < 0) break;
		FD_SET((unsigned) socket, &wd_set);
		numFds = socket+1;
		
		result = select(numFds, NULL, &wd_set, NULL,0);// timeout);
		if (result == 0)
		{
			break; // this is OK - timeout occurred
		}
		else if(result <= 0) 
		{
			fprintf(stderr, "select() error\n ");
			break;
		}

		if (!FD_ISSET(socket, &wd_set)) 
		{
			fprintf(stderr, "select() error - !FD_ISSET\n");
			break;
		}
	} while (0);
	return result;
};

int blockUntilReadable(int socket)//, struct timeval* timeout) 
{
	fd_set rd_set;
	int result = -1;
	unsigned numFds;
	
	do 
	{
		FD_ZERO(&rd_set);
		if (socket < 0) break;
		FD_SET((unsigned) socket, &rd_set);
		numFds = socket+1;
		result = select(numFds, &rd_set, NULL, NULL,0);
		if (result == 0) 
		{
			break; // this is OK - timeout occurred
		} 
		else if (result <= 0) 
		{
			fprintf(stderr, "select() error\n ");
			break;
		}
    
		if (!FD_ISSET(socket, &rd_set)) 
		{
			fprintf(stderr, "select() error - !FD_ISSET\n");
			break;
		}	
	} while (0);

	return result;
}


int getSDPDescriptionFromURL(int socketNum,char* url,char* Description, RTSP_T *pRtsp)
{
	int fSocketNum;
	char *readBuffer;//[MAX_READBUFSIZE+1]; 
	char* readBuf;

	int bytesRead;

	char* cmdFmt ="DESCRIBE %s RTSP/1.0\r\nCSeq: %d\r\nAccept: application/sdp\r\n%s\r\n";
	unsigned cmdSize = 0;										
  	char* cmd = NULL;
	char* firstLine;
	char* nextLineStart;
	unsigned responseCode;
	int contentLength = -1;
	char* lineStart;  
	unsigned numExtraBytesNeeded;
	char* ptr;
	int bytesRead2;
	
	readBuffer = (char *)MALLOC((MAX_READBUFSIZE+1)*sizeof(char));
	if(readBuffer == NULL)
	{
		fprintf(stderr,"failed to alloc the memory\n");
    		return -1;
	}
	memset(readBuffer,0,MAX_READBUFSIZE+1);
	readBuf =readBuffer;
	
	//fprintf(stderr,"start alloc cmd\n");
	cmdSize = strlen(cmdFmt) + strlen(url) + 20 /* max int len */+ strlen(pRtsp->UserAgentHeaderStr);
	cmd = (char *)MALLOC((cmdSize+1)*sizeof(char));
	if(cmd ==NULL)
	{
		fprintf(stderr,"failed to alloc the memory\n");
    		FREE(readBuffer);
    		return -1;
	}
	//fprintf(stderr,"alloc cmd successful\n");
	memset(cmd,0,cmdSize+1);
	fSocketNum = socketNum;
	do 
	{  
 
		// Send the DESCRIBE command:
		//fprintf(stderr,"Send the DESCRIBE command\n");
		sprintf(cmd, cmdFmt,url,++pRtsp->fCSeq,pRtsp->UserAgentHeaderStr);

    //发送description 请求
		
		//fprintf(stderr,"start blockUntilwriteable\n");
  		if(blockUntilwriteable(fSocketNum)<=0)
  		{ 
  			fprintf(stderr,"socket is unwriteable\n");
  			break;
  		}
  		//fprintf(stderr,"start send\n");
  		if (send(fSocketNum, cmd, strlen(cmd), 0)<0) 
		{
			fprintf(stderr,"DESCRIBE send() failed\n ");
			break;
		}
		//fprintf(stderr,"DESCRIBE send() successful\n");
			// Get the response from the server:
		//fprintf(stderr,"Get the response from the server\n");
		bytesRead = getResponse(fSocketNum,readBuf, MAX_READBUFSIZE);
		if (bytesRead <= 0) break;
		
		//fprintf(stderr,"DESCRIBE response-%d:\n%s\n",pRtsp->fCSeq,readBuf);
		//Received DESCRIBE response
    
		// Inspect the first line to check whether it's a result code that
		// we can handle.

		firstLine = readBuf;
		nextLineStart = getLine(firstLine);
		if (sscanf(firstLine, "%*s%u", &responseCode) != 1) 
		{
			fprintf(stderr,"no response code in line\n");
			break;
		}
		if (responseCode != 200) 
		{
			fprintf(stderr,"cannot handle DESCRIBE response\n");
			break;
		}

		// Skip every subsequent header line, until we see a blank line
		// The remaining data is assumed to be the SDP descriptor that we want.
		// We should really do some checking on the headers here - e.g., to
		// check for "Content-type: application/sdp", "Content-base",
		// "Content-location", "CSeq", etc. #####
		//fprintf(stderr,"start get Content-Length\n");
		while (1) 
		{
			lineStart = nextLineStart;
			if (lineStart == NULL||lineStart[0] == '\0') break;

			nextLineStart = getLine(lineStart);
			if (sscanf(lineStart, "Content-Length: %d", &contentLength) == 1
				|| sscanf(lineStart, "Content-length: %d", &contentLength) == 1) 
			if(contentLength>0)
			{
				if (contentLength < 0) 
				{
					fprintf(stderr,"Bad Content-length\n");
					break;
				}
				// Use the remaining data as the SDP descr, but first, check
				// the "Content-length:" header (if any) that we saw.  We may need to
				// read more data, or we may have extraneous data in the buffer.
				//bodyStart = nextLineStart;
				// We saw a "Content-length:" header
				
				//numBodyBytes = &readBuf[bytesRead] - bodyStart;
				//if (contentLength > (int)numBodyBytes) 
				//{
				// We need to read more data.  First, make sure we have enough
				// space for it:
				numExtraBytesNeeded = contentLength;// - numBodyBytes;
			
				// Keep reading more data until we have enough:
				bytesRead = 0;
				//fprintf(stderr,"start recv %d\n",contentLength);
				while (numExtraBytesNeeded > 0) 
				{
				#if 0
					if(blockUntilReadable(fSocketNum)<=0)
    					{
    						fprintf(stderr,"socket is unreadable\n");
    						break;
    					}
				#else
					if( 0 >= SelectReadRtspc( socketNum, TIMEOUT_RECONNECT_TIME ) )
					{
						break;
					}
				#endif
    					//fprintf(stderr,"start recv\n");
					ptr = Description+bytesRead;
					//ptr = &readBuf[bytesRead];
					bytesRead2 = recv(fSocketNum, (unsigned char*)ptr,numExtraBytesNeeded,0);
					if (bytesRead2 <0) break;
					ptr[bytesRead2] = '\0';
						

					bytesRead += bytesRead2;
					numExtraBytesNeeded -= bytesRead2;
				}
				if (numExtraBytesNeeded > 0) break; // one of the reads failed
			
				FREE(readBuffer);
				FREE(cmd);
				return 0;
			}
		}
	}while(0);
	FREE(readBuffer);
	FREE(cmd);
	Description = NULL;
	return -1;
}

char* getLine(char* startOfLine) 
{
	// returns the start of the next line, or NULL if none
	char* ptr;
	for (ptr = startOfLine; *ptr != '\0'; ++ptr) 
	{
		if (*ptr == '\r' || *ptr == '\n') 
		{
			// We found the end of the line
			*ptr++ = '\0';
			if (*ptr == '\n') ++ptr;
			return ptr;
		}
	}

	return NULL;
}

int openConnectionFromURL(char* url, RTSP_T *pRtsp)
{
	struct hostent *hp;
	char address[100];
	int destPortNum;
	struct sockaddr_in server;
	int fSocketNum = -1;
	if (url == NULL) return -1;
	memset(address,0,100);
	if (parseRTSPURL(url, address, &destPortNum, pRtsp)) return -1;
  
  
	fSocketNum = socket(AF_INET, SOCK_STREAM, 0);
  
	if (fSocketNum < 0) 
	{
		fprintf(stderr,"Client: Error Opening socket\n");
		return -1;
	}
	hp = gethostbyname(address);
	if (hp == NULL ) 
	{
		fprintf(stderr,"Client: Cannot resolve address [%s]\n",address);
		return -1;
	}
	
	memset(&server,0,sizeof(struct sockaddr_in));
	memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons((unsigned short)destPortNum);
	struct timeval tval;
	tval.tv_sec 	= 0;
	tval.tv_usec 	= 1000*100;
 	setsockopt( fSocketNum, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tval, sizeof(tval) );
	if (connect(fSocketNum, (struct sockaddr*)&server, sizeof(struct sockaddr_in))!= 0) 
	if (connect(fSocketNum, (struct sockaddr*)&server, sizeof(struct sockaddr_in))!= 0) 
	{
		//fprintf(stderr,"connect() failed\n");
		//closesocket(fSocketNum);
		close(fSocketNum);
		return -1;
	}
//fprintf(stderr,"connect() successful \n");
	return fSocketNum;
}

int parseRTSPURL(char* url,char * address,int* portNum, RTSP_T *pRtsp ) 
{
	
	char const* prefix = "rtsp://";
	unsigned const prefixLength = 7;
	char* from = NULL ;
	char* to = NULL;
	unsigned i;
	char nextChar;
	if (strncmp(url, prefix, prefixLength) != 0) 
	{
		fprintf(stderr,"URL is not of the form\n");
		return -1;
	}
		
	from = &url[prefixLength];
	to = &address[0];
	for (i = 0; i < pRtsp->parseBufferSize; ++i) 
	{
		if (*from == '\0' || *from == ':' || *from == '/') 
		{
	// We've completed parsing the address
			*to = '\0';
			break;
		}
		*to++ = *from++;
	}
	if (i == pRtsp->parseBufferSize) 
	{
		fprintf(stderr,"URL is too long\n");
		return -1;
	}

 	*portNum = 554; // default value
	nextChar = *from;
	if (nextChar == ':') 
	{
		int portNumInt;
		if (sscanf(++from, "%d", &portNumInt) != 1) 
		{
			fprintf(stderr,"No port number follows :%d\n",portNumInt);
			return -1;
		}
		if (portNumInt < 1 || portNumInt > 65535) 
		{
			fprintf(stderr,"Bad port number\n");
			return -1;
		}
		*portNum = portNumInt;
	}
	//fprintf(stderr,"address is %s;portNum is %d \n",address,*portNum);
	return 0;
}

////////////////////////
void resumeStreams(int socketNum, RTSP_T *pRtsp)
{
	//double start;
	//start = (double)(pRtsp->vloopNum*1000)/(double)(pRtsp->VFrameRate);
	long start;
	start = pRtsp->vloopNum*1000/pRtsp->VFrameRate;

	//if(playMediaSession(socketNum,start,(double)pRtsp->fMaxPlayEndTime/(double)1000)) 
	if(playMediaSession(socketNum,start,-1,pRtsp)) 
	{	
			fprintf(stderr,"Play MediaSubsession Failed\n");
			exit(0);
	}
	//fprintf(stderr,"Play Streams successful\n");
}

int playMediaSession(int socketNum,int start,int end, RTSP_T *pRtsp)//double start, double end)
{
	char* cmd = NULL;
	char cmdFmt[] =
			"PLAY %s/ RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			//"Range: \r\n"
			"Range: npt=%s-%s\r\n"
			"%s\r\n";
	char startStr[30], endStr[30];
	unsigned cmdSize;
	unsigned const readBufSize = 10000;
	char *readBuffer;
	char* readBuf;
	int bytesRead;
	char* firstLine;
	char* nextLineStart;
	unsigned responseCode;
	readBuffer = (char*)MALLOC(sizeof(char)*(readBufSize+1));
	if(readBuffer == NULL) return -1;
	memset(readBuffer,0,readBufSize+1);
	do 
	{
		// First, make sure that we have a RTSP session in progress
		if (pRtsp->fLastSessionId == NULL) 
		{
			fprintf(stderr,"No RTSP session is currently in progress\n");
			break;
		}

		// Send the PLAY command:

		// First, construct an authenticator string:
//		sprintf(startStr, "%.3f", start); 
//sprintf(endStr, "%.3f", end);
sprintf(startStr,"%d",start);
sprintf(endStr,"%d",end);
		
		if (start==-1) startStr[0]='\0';
		if (end == -1) endStr[0] = '\0';
			
		cmdSize = strlen(cmdFmt)
			+ strlen(pRtsp->fBaseURL)//+strlen(subsession->fControlPath)
			+ 20 /* max int len */
			+ strlen(pRtsp->fLastSessionId)
			+ strlen(startStr) + strlen(endStr)
			+ strlen(pRtsp->UserAgentHeaderStr);
		cmd = (char*)MALLOC(sizeof(char)*cmdSize);
		if(cmd == NULL) 
		{
			FREE(readBuffer);
			return -1;
		}
		memset(cmd,0,cmdSize);
		sprintf(cmd, cmdFmt,
			pRtsp->fBaseURL,
			//subsession->fControlPath,
			++pRtsp->fCSeq,
			pRtsp->fLastSessionId,
			startStr, endStr,
			pRtsp->UserAgentHeaderStr);

		//fprintf(stderr,"PLAY command-%d:\n%s\n",pRtsp->fCSeq,cmd);
		if (send(socketNum,cmd,strlen(cmd),0)<0) 
		{
			fprintf(stderr,"PLAY send() failed\n ");
			break;
		}
		readBuf = readBuffer;
		// Get the response from the server:
		bytesRead = getResponse(socketNum,readBuf, readBufSize);

		if (bytesRead <= 0) break;
		 
		//fprintf(stderr,"PLAY response-%d:\n%s\n",pRtsp->fCSeq,readBuf);
		
		// Inspect the first line to check whether it's a result code 200
		firstLine = readBuf;
		nextLineStart = getLine(firstLine);
		if (parseResponseCode(firstLine,&responseCode)) break;
		if (responseCode != 200) 
		{
			fprintf(stderr,"cannot handle PLAY response\n ");
			break;
		}

			FREE(cmd);
	FREE(readBuffer);
		return 0;
	} while (0);

	FREE(cmd);
 FREE(readBuffer);
	return -1;
}
/*int getPauseResponse(int socketNum,char* responseBuffer,unsigned responseBufferSize) 
{
	int fSocketNum;
	char *lastToCheck=NULL;
	char* p = NULL;//responseBuffer;
	int bytesRead = 0; // because we've already read the first byte
	unsigned bytesReadNow	= 0;
	fSocketNum = socketNum;

	if (responseBufferSize == 0) return 0; // just in case...
	//responseBuffer[0] = '\0'; // ditto
	*(responseBuffer) = '\0';

	// Keep reading data from the socket until we see "\r\n\r\n" (except
	// at the start), or until we fill up our buffer.
	// Don't read any more than this.
	while (bytesRead < (int)responseBufferSize) 
	{
		lastToCheck = NULL;
		if(blockUntilReadable(fSocketNum)<=0)
		{
			fprintf(stderr,"socket is unreadable\n");
			break;
		}
		bytesReadNow = recv(fSocketNum,(unsigned char*)(responseBuffer+bytesRead),1, 0);
		if (bytesReadNow != 1) 
		{
			fprintf(stderr,"RTSP response was truncated\n");
			break;
		}
		bytesRead++;
		
		// Check whether we have "\r\n\r\n":
	
		lastToCheck = responseBuffer+bytesRead-4;
		if (lastToCheck < responseBuffer) continue;
		p = lastToCheck;
		if (*p == '\r' && *(p+1) == '\n' &&
						*(p+2) == '\r' && *(p+3) == '\n') 
		{
			*(responseBuffer+bytesRead)= '\0';
			// Before returning, trim any \r or \n from the start:
			while (*responseBuffer == '\r' || *responseBuffer == '\n') 
			{
				++responseBuffer;
				--bytesRead;
			}
			if(strncmp(responseBuffer,"RTSP",4)==0)
				return bytesRead;
		}
	}
	return bytesRead;
}*/

/////////////


int pauseMediaSession(int socketNum, RTSP_T *pRtsp)
{
	char* cmd = NULL;
	char cmdFmt[] =
			"PAUSE %s/ RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"%s\r\n";
	unsigned cmdSize;
	unsigned readBufSize = 10000;
	char* readBuffer; 
	char* readBuf;
	int bytesRead;
	char* firstLine;
	unsigned responseCode;
	readBuffer = (char*)MALLOC(sizeof(char)*(readBufSize+1));
	if(readBuffer == NULL) return -1;
	memset(readBuffer,0,readBufSize+1);

	do 
	{
		// First, make sure that we have a RTSP session in progress
		if (pRtsp->fLastSessionId == NULL) 
		{
			fprintf(stderr,"No RTSP session is currently in progress\n");
			break;
		}
		
		// Send the PAUSE command:
		
		// First, construct an authenticator string:
		
		
		
		cmdSize = strlen(cmdFmt)
			+ strlen(pRtsp->fBaseURL)//+strlen(subsession->fControlPath)
			+ 20 /* max int len */
			+ strlen(pRtsp->fLastSessionId)
			+ strlen(pRtsp->UserAgentHeaderStr);
		cmd = (char*)MALLOC(sizeof(char)*cmdSize);
		if(cmd == NULL) 
		{
			FREE(readBuffer);
			return -1;
		}
		memset(cmd,0,cmdSize);
		sprintf(cmd, cmdFmt,
			pRtsp->fBaseURL,
			//subsession->fControlPath,
			++pRtsp->fCSeq,
			pRtsp->fLastSessionId,
			pRtsp->UserAgentHeaderStr);
		//fprintf(stderr,"PAUSE command-%d:\n%s\n",pRtsp->fCSeq,cmd);
		
		if (send(socketNum,cmd,strlen(cmd),0)<0) 
		{
			fprintf(stderr,"PAUSE send() failed!\n ");
			break;
		}
		
		// Get the response from the server:
		readBuf = readBuffer;
		bytesRead = getResponse(socketNum,readBuf, readBufSize);

		if (bytesRead <= 0) break;
		
		//fprintf(stderr,"bytesRead is %d\n",bytesRead);
		//fprintf(stderr,"PAUSE response-%d:\n%s\n",pRtsp->fCSeq,readBuf);
		
		 // Inspect the first line to check whether it's a result code 200
		firstLine = readBuf;
		/*char* nextLineStart =*/ getLine(firstLine);
		
		if (parseResponseCode(firstLine,&responseCode)) break;
		
		if (responseCode != 200) 
		{
			fprintf(stderr,"cannot handle PAUSE response\n ");
			break;
		}
		// (Later, check "CSeq" too #####)
		
		FREE(cmd);
		FREE(readBuffer);
		//fprintf(stderr,"Pause Streams successful\n");
		return 0;
	} while (0);
	
	FREE(cmd);
	FREE(readBuffer);
	fprintf(stderr,"Pause Streams failed\n");
	return -1;
}


///////////////////////////
int teardownMediaSession(int socketNum, RTSP_T *pRtsp) 
{
	char* cmd = NULL;
	char* const cmdFmt = "TEARDOWN %s RTSP/1.0\r\n"
				"CSeq: %d\r\n"
				"Session: %s\r\n"
				"%s\r\n";
	unsigned cmdSize;
	unsigned readBufSize = 10000;
	char* readBuffer; 
	char* readBuf;
	char* firstLine;
	unsigned responseCode;
	int bytesRead;
	readBuffer = (char *)MALLOC((readBufSize+1)*sizeof(char));
	if(readBuffer == NULL) return -1;
	memset(readBuffer,0,readBufSize+1);
	readBuf = readBuffer;
	do 
	{
		if (pRtsp->fLastSessionId == NULL) 
		{
			fprintf(stderr,"No RTSP session is currently in progress\n");
			break;
		}
		
		// Send the TEARDOWN command:

		// First, construct an authenticator string:
		
		cmdSize = strlen(cmdFmt)
			+ strlen(pRtsp->fBaseURL)
			+ 20 /* max int len */
			+ strlen(pRtsp->fLastSessionId)
			+ strlen(pRtsp->UserAgentHeaderStr);
		cmd = (char*)MALLOC(cmdSize*sizeof(char));
		if(cmd == NULL) return -1;
		memset(cmd,0,cmdSize);
		sprintf(cmd, cmdFmt,
			pRtsp->fBaseURL,
			++pRtsp->fCSeq,
			pRtsp->fLastSessionId,
			pRtsp->UserAgentHeaderStr);
		fprintf(stderr,"TEARDOWN command-%d:\n%s\n",pRtsp->fCSeq,cmd);
		
		if (send(socketNum,cmd,strlen(cmd),0)<0) 
		{
			fprintf(stderr,"TEARDOWN send() failed\n ");
			break;
		}

		// Get the response from the server:
		
		bytesRead = getResponse(socketNum,readBuf, readBufSize);
		if (bytesRead <= 0) break;
		//fprintf(stderr,"TEARDOWN response-%d:\n%s\n",pRtsp->fCSeq,readBuf);
		// Inspect the first line to check whether it's a result code 200
		firstLine = readBuf;
		/*char* nextLineStart =*/ getLine(firstLine);
		if (parseResponseCode(firstLine,&responseCode)) break;
		if (responseCode != 200) 
		{
			fprintf(stderr,"cannot handle TEARDOWN response\n ");
			break;
		}
		
		FREE(readBuffer);
		FREE(cmd);
		return 0;
	} while (0);

	FREE(readBuffer);
	FREE(cmd);
	return -1;
}


char* strDup(char* str) 
{
	char* copy;
	unsigned int len;
	if (str == NULL) return NULL;
	len = strlen(str) + 1;
	copy = (char*)MALLOC(len*sizeof(char));

	if (copy != NULL) {
		memcpy(copy, str, len);
	}
	return copy;
}

char* strDupSize(char* str) 
{
	char* copy;
	unsigned int len;
	if (str == NULL) return NULL;
	len = strlen(str) + 1;
	copy = (char *)MALLOC(len*sizeof(char));

	return copy;
}

int hex_a(unsigned char *hex, char *a,unsigned char length)
{
	unsigned char hLowbit,hHighbit;
	int i;

	for(i=0;i<length*2;i+=2)
	{
		hLowbit=hex[i/2]&0x0f;
		hHighbit=hex[i/2]/16;
		if (hHighbit>=10) a[i]=hHighbit+'7';
		else a[i]=hHighbit+'0';
		if (hLowbit>=10) a[i+1]=hLowbit+'7';
		else a[i+1]=hLowbit+'0';
	}
	a[length*2]='\0';
	return 0;
}


int a_hex( char *a,unsigned char *hex,unsigned char len)
{
	 short i;
	 unsigned char aLowbit,aHighbit;
	 unsigned char hconval,lconval;

	 for(i=0;i<len;i+=2)
	 {
		aHighbit=toupper(a[i]);
		if ((aHighbit>='A')&&(aHighbit<='F')) 
			hconval='7';
		else
			 if ((aHighbit>='0')&&(aHighbit<='9')) 
			 	hconval='0';
			 else	
			 	return -1;
		aLowbit=toupper(a[i+1]);
		if ((aLowbit>='A')&&(aLowbit<='F')) 
			lconval='7';
		else
			 if ((aLowbit>='0')&&(aLowbit<='9')) 
			 	lconval='0';
			 else 
			 	return -1;
		hex[(i/2)]=((aHighbit-hconval)*16+(aLowbit-lconval));
	 }
	 hex[len/2]=0x00;
	 return 0;
}





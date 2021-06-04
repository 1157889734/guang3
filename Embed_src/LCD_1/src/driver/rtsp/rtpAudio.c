/*******************************************************************
* author : ÁºÇÅ½­(oscar.liang@szbeihai.com)
* date : 2013-12-03
* fn: ÒôÆµ°ü½âÎö
*******************************************************************/

#include "rtp.h"

int rtp_audio_parse( struct ResultData *packData, rtp_frame * fr )
{
    fr->len 	= packData->len;
    fr->data 	= packData->buffer;
	packData->buffer = NULL;
	fr->timestamp	= packData->frtpTimestamp;

    return RTP_FILL_OK;
}


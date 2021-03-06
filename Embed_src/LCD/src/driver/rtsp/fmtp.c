/*******************************************************************
* author : 梁桥江(oscar.liang@szbeihai.com)
* date : 2013-12-03
* fn: 视频参数解析, 也就是h264 pps 解析
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtp.h"
#include "fmtp.h"

#include "bh_malloc.h"

#ifdef NO_CHECK_MALLOC
#define MALLOC(len)  	malloc(len)
#define FREE(ptr)	     	free(ptr);ptr=NULL
#endif

static uint8_t map2[] =
{
	0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36,
	0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01,
	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
	0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
	0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
	0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33
};


static int nms_base64_decode(uint8_t * out, const char *in, int out_length)
{
	int i, v;
	uint8_t *dst = out;

	v = 0;
	for (i = 0; in[i] && in[i] != '='; i++) {
		unsigned int index= in[i]-43;
		if (index>=(sizeof(map2)/sizeof(map2[0])) || map2[index] == 0xff)
			return -1;
		v = (v << 6) + map2[index];
		if (i & 3) {
			if (dst - out < out_length) {
				*dst++ = v >> (6 - 2 * (i & 3));
			}
		}
	}

	return (dst - out);
}


static int nms_get_attr_value(char *attr, const char *param, char *v, int v_len )
{
	char *value, *tmp;
	int len;
	if ((value = strstr(attr, param)) && (*(value += strlen(param)) == '=')) 
	{
		value++;
		strncpy(v, value, v_len - 1);
		v[v_len - 1] = '\0';
		if ((tmp = strstr(v,";"))) {
			len = tmp - v;
			v[len] = '\0';
		} else {
			len = strlen(v);
		}
		return len;
	}
	return 0;
}

int fmtp_parser( char *confBuf, unsigned char *pConf, int *pConfLen )
{
	char value[1024];
//    int i;
	int len;
	unsigned char *confHex;
	int  confLen;

	// for (i=0; i < attrs->size; i++)
	{
		if ((len = nms_get_attr_value(confBuf, "profile-level-id", value, sizeof(value)))) 
		{
			if (len==6) { /*hex string*/}
		}
		if ((len = nms_get_attr_value(confBuf, "packetization-mode", value, sizeof(value)))) 
		{
			// We do not support anything else.
			if (len != 1 || atoi(value) >= 2) 
			{
				printf( "Unsupported H.264 packetization mode %s\n", value);
				return RTP_PARSE_ERROR;
			}
		}
		if ((len = nms_get_attr_value(confBuf, "sprop-parameter-sets", value, sizeof(value)))) 
		{
			//shamelessly ripped from ffmpeg
			uint8_t start_seq[4] = {0, 0, 0, 1};
			char *v = value;
			confLen = 0;
			confHex = NULL;
			while (*v) 
			{
				char base64packet[1024];
				uint8_t decoded_packet[1024];
				unsigned packet_size;
				char *dst = base64packet;

 				while (*v && *v != ',' && (dst - base64packet) < sizeof(base64packet) - 1) 
				{
					*dst++ = *v++;
				}
				*dst++ = '\0';

				if (*v == ',')
				{
					v++;
				}

				packet_size = nms_base64_decode(decoded_packet, base64packet, sizeof(decoded_packet));
				if (packet_size) 
				{
					//uint8_t *dest = calloc(1, packet_size +
					//                       sizeof(start_seq) +
					//                       confLen);
					uint8_t *dest = (uint8_t *)MALLOC(packet_size + sizeof(start_seq) + confLen);
					if (dest) 
					{
						if (confLen) 
						{
							memcpy(dest, confHex, confLen);
							//free(confHex);
							if(confHex) FREE(confHex);
						}

						memcpy(dest+confLen, start_seq, sizeof(start_seq));
						memcpy(dest + confLen +  sizeof(start_seq), decoded_packet, packet_size);

						confHex = dest;
						confLen += sizeof(start_seq) + packet_size;
					} 
					else 
					{
						goto err_alloc;
					}
				}
			}
		}
	}

#if 0
	int k;
	for ( k = 0; k < confLen; ++k )
	{
		fprintf( stderr, "%02X ", confHex[k] );
	}
	fprintf( stderr, "\n" );
#endif
	if ( NULL != pConf && NULL != pConfLen )
	{
		memcpy( pConf, confHex, confLen );
		if(confHex) FREE(confHex);
		*pConfLen = confLen;
	}
	return 0;

err_alloc:
	return RTP_ERRALLOC;
}


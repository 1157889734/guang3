#ifndef __RTP_H__
#define __RTP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#include <time.h>
#include <pthread.h>
#include <stdint.h>

#include "srtspType.h"

/**
 * RTP packet structure, the header is a bitfield
 */
typedef struct {
#ifdef WORDS_BIGENDIAN
    uint32_t ver:2;
    uint32_t pad:1;
    uint32_t ext:1;
    uint32_t cc:4;
#else
    uint32_t cc:4;        //!< source count
    uint32_t ext:1;       //!< extension flag
    uint32_t pad:1;       //!< padding flag
    uint32_t ver:2;       //!< version
#endif
#ifdef WORDS_BIGENDIAN
    uint32_t mark:1;
    uint32_t pt:7;
#else
    uint32_t pt:7;        //!< payload type
    uint32_t mark:1;      //!< marker flag 
#endif
    uint32_t seq:16;      //!< sequence number
    uint32_t time;        //!< timestamp
    uint32_t ssrc;        //!< synchronization source identifier
    uint8_t data[1];      //!< beginning of RTP data
} rtp_pkt;

/**
 * rtp_fill error codes
 */
#define RTP_FILL_OK         0
#define RTP_BUFF_EMPTY      91
#define RTP_PARSE_ERROR     92
#define RTP_PKT_UNKNOWN     93
#define RTP_IN_PRM_ERR      94
#define RTP_SSRC_NOTVALID   97
#define RTP_BUFFERING       99

#define RTP_PKT_CC(pkt)     (pkt->cc)
#define RTP_PKT_MARK(pkt)   (pkt->mark)
#define RTP_PKT_PT(pkt)     (pkt->pt)
#define RTP_PKT_SEQ(pkt)    ntohs(pkt->seq)
#define RTP_PKT_TS(pkt)     ntohl(pkt->time)
#define RTP_PKT_SSRC(pkt)   ntohl(pkt->ssrc)
#define RTP_PKT_DATA(pkt)   (pkt->data  + pkt->cc)
// 输入包指针和包总长度, 返回H264 裸流长度
#define RTP_PAYLOAD_SIZE(pkt, pkt_len)    ((pkt) ? pkt_len - ((pkt->data)-(uint8_t *)pkt) - pkt->cc - ((*(((uint8_t *)pkt)+pkt_len-1)) * pkt->pad) : 0)

#define nms_consume_1(buff) *((uint8_t*)(*(buff))++)

#define RTP_OK           0
// general error codes. other, specific, error codes below
#define RTP_ERROR        -1
#define RTP_ERRALLOC     -2

int nms_alloc_data(uint8_t **buf, long *cur_len, long new_len);
void nms_append_data(uint8_t *dst, long offset, uint8_t *src, long len);
inline void nms_append_incr(uint8_t *dst, long *offset, uint8_t *src, long len);
uint16_t nms_consume_BE2(uint8_t ** buff);

extern int rtp_h264_parse( struct ResultData *packData, rtp_frame * fr );
extern int rtp_audio_parse( struct ResultData *packData, rtp_frame * fr );

#endif // __RTP_H__


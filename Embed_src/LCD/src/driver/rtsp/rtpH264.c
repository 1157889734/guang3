/*******************************************************************
* author : 梁桥江(oscar.liang@szbeihai.com)
* date : 2013-12-03
* fn: H264包解析
*******************************************************************/

#include "rtp.h"

int rtp_h264_parse( struct ResultData *packData, rtp_frame * fr )
{
    size_t len;
    uint8_t *buf;
    uint8_t type;
    uint8_t start_seq[4] = {0, 0, 0, 1};
    int err = RTP_FILL_OK;
    
    buf = packData->buffer; //RTP_PKT_DATA(pkt);
    len = packData->len; //RTP_PAYLOAD_SIZE(pkt, len);
    type = (buf[0] & 0x1f);
	//fprintf( stderr, "rtp_h264_parse---buf[0]=%02X, buf[1]=%02X\n", buf[0], buf[1] ); // buf[0] = 7C
    // In order to produce a compliant bitstream, a PPS NALU should prefix
    // the data stream.
    
#if 0 // ql del, 把pps 拷贝到h264 裸流, 真麻烦

    if (!priv->configured && priv->conf_len) {
    
	int k;
	for( k = 0; k < priv->conf_len; ++k )
		fprintf( stderr, "%02X ", priv->conf[k]);
	fprintf( stderr, "\n");
        if(nms_alloc_data(&fr->data, &priv->data_size, priv->conf_len)) {
            return RTP_ERRALLOC;
        }
        nms_append_incr(fr->data, &priv->len, priv->conf, priv->conf_len);
        priv->configured = 1;
    }

#endif
    if (type >= 1 && type <= 23) type = 1; // single packet
	//fprintf( stderr, "rtp_h264_parse, type(%d)\n", type );

    switch (type) {
    case 0: // undefined;
        err = RTP_PKT_UNKNOWN;
        break;
    case 1: // 一包为一帧
        if(nms_alloc_data(&fr->data, &fr->curLen,
                          len + sizeof(start_seq) + fr->curLen)) {
            return RTP_ERRALLOC;
        }
        nms_append_incr(fr->data, &fr->len, start_seq, sizeof(start_seq));
        nms_append_incr(fr->data, &fr->len, buf, len);
        fr->data = fr->data;
        fr->len = fr->curLen;
        fr->curLen = 0;
        
        break;
    case 24:    // STAP-A (aggregate, output as whole or split it?)
        #if 0
        {
            size_t frame_len;
            buf += priv->index + 1; // skip the nal
            frame_len = nms_consume_BE2(&buf);
            if (!frame_len) {
                printf( "Empty frame\n");
                err = RTP_PARSE_ERROR;
                break;
            }

            nms_printf(NMSML_WARN, "NAL: %d", *buf & 0x1f);

            if (frame_len + priv->index < len) {
                printf( "Packet size %d\n",frame_len);

                fr->data = fr->data = realloc(fr->data,
                                                sizeof(start_seq)
                                                + frame_len);
                memcpy(fr->data, &start_seq, sizeof(start_seq));
                memcpy(fr->data + sizeof(start_seq), buf, frame_len);
                fr->len = sizeof(start_seq) + frame_len;
                priv->index += 2 + frame_len;
                if ( priv->index + 1 < len) return err; // more to output
                else priv->index = 0; // get a new packet
            } else {
                nms_printf (NMSML_ERR,"STAP-A corrupted\n");
                err = RTP_PARSE_ERROR;
            }
        }
        break;
        #else
        // trim stap-a nalu header
        buf++;
        len--;
        // ffmpeg way
        {
            int pass = 0;
            int total_length = 0;

            for(pass= 0; pass<2; pass++) {
                uint8_t *src = buf;
                int src_len = len;
                do {
                    uint16_t nal_size = nms_consume_BE2(&src);
                    src_len -= 2;

                    if (nal_size <= src_len) {
                        if(pass==0) {
                            total_length += sizeof(start_seq) + nal_size;
                        } else {
                            nms_append_incr(fr->data, &fr->curLen, start_seq,
                                            sizeof(start_seq));
                            nms_append_incr(fr->data, &fr->curLen, src,
                                            nal_size);
                        }
                    } else {
                        printf( 
                                   "nal size exceeds length: %d %d\n",
                                   nal_size, src_len);
                    }

                    src += nal_size;
                    src_len -= nal_size;

                    if (src_len < 0)
                        printf( 
                                   "Consumed more bytes than we got! (%d)\n",
                                   src_len);
                } while (src_len > 2);  // because there could be rtp padding..

                if(pass==0) {
                    if(nms_alloc_data(&fr->data, &fr->curLen,
                                      total_length + fr->curLen)) {
                        return RTP_ERRALLOC;
                    }
                }
            }
        }
        fr->data = fr->data;
        fr->len = fr->curLen;
        fr->curLen = 0;
        break;
        #endif
        /* Unsupported for now */
    case 25:    // STAP-B
        err = RTP_PKT_UNKNOWN;
        printf( "STAP-B\n");
        break;
    case 26:    // MTAP-16
        err = RTP_PKT_UNKNOWN;
        printf( "MTAP-16\n");
        break;
    case 27:    // MTAP-24
        printf( "MTAP-24\n");
        err = RTP_PKT_UNKNOWN;
        break;

    case 28:    // FU-A (fragmented nal, output frags or aggregate it)
    
        {
            uint8_t fu_indicator = nms_consume_1(&buf);  // read the fu_indicator
            uint8_t fu_header    = nms_consume_1(&buf);  // read the fu_header.
            uint8_t start_bit    = (fu_header & 0x80) >> 7;
            uint8_t end_bit      = (fu_header & 0x40) >> 6;
            uint8_t nal_type     = (fu_header & 0x1f);
            uint8_t reconstructed_nal;

            len -= 2; // skip fu indicator and header

            // reconstruct this packet's true nal; only the data follows..
            // the original nal forbidden bit and NRI are stored in
            // this packet's nal;
            reconstructed_nal = fu_indicator & 0xe0;
            reconstructed_nal |= nal_type;

            if(start_bit && !fr->curLen) { // 第一包
                if(nms_alloc_data(&fr->data, &fr->curLen,
                                  len + 1 + sizeof(start_seq) + fr->curLen)) {
                    return RTP_ERRALLOC;
                }
                // copy in the start sequence, and the reconstructed nal....
                nms_append_incr(fr->data, &fr->len, start_seq,
                                sizeof(start_seq));
                nms_append_incr(fr->data, &fr->len, &reconstructed_nal, 1);
                nms_append_incr(fr->data, &fr->len, buf, len);
            } else { // 后面的包
                if (fr->timestamp != packData->frtpTimestamp/*RTP_PKT_TS(pkt)*/) {
                    fr->len = fr->len = 0;
                    return RTP_PKT_UNKNOWN;
                }
                if(nms_alloc_data(&fr->data, &fr->curLen,
                                  len + fr->curLen)) {
                    return RTP_ERRALLOC;
                }
                nms_append_incr(fr->data, &fr->len, buf, len);
            }

            fr->timestamp = packData->frtpTimestamp/*RTP_PKT_TS(pkt)*/;
            if (!end_bit) {
                err = 1;//EAGAIN;
            } else {
                fr->data = fr->data;
                fr->len = fr->curLen;
                fr->curLen = 0;
            }
        }
        break;
    case 30:                   // undefined
    case 31:                   // undefined
    default:
        err = RTP_PKT_UNKNOWN;
        break;
    }
    
    return err;
}



#include "rtp.h"
/**
 * Auxiliar temporary buffer allocation procedures
 */
int nms_alloc_data(uint8_t **buf, long *cur_len, long new_len) 
{
    if (buf && cur_len && *cur_len < new_len) {
        if (!(*buf = realloc(*buf, new_len))) {
            return -1;
        }
        *cur_len = new_len;
    }
    return 0;
}

void nms_append_data(uint8_t *dst, long offset, uint8_t *src, long len) {
    memcpy(dst + offset, src, len);
}

inline void nms_append_incr(uint8_t *dst, long *offset, uint8_t *src, long len) {
    nms_append_data(dst, *offset, src, len);
    *offset += len;
}

uint16_t nms_consume_BE2(uint8_t ** buff)
{
    uint16_t v = 0;
    uint8_t * buff_p = *buff;
   
    v = (buff_p[1])|(buff_p[0] << 8);
    *buff += 2;

    return v;
}

/*
* fn: select socket 是否可读
* socket: 检测的socket
* maxMsec: 超时时间,单位毫秒
*/
int SelectReadRtspc( int socket, int maxMsec )
{
	int nRet = -1;
	fd_set fd; 
	FD_ZERO( &fd );
  	FD_SET( socket, &fd );

	struct timeval timeout;
	timeout.tv_sec	= maxMsec / 1000;
  	timeout.tv_usec	= (maxMsec % 1000) * 1000;
	
  	nRet = select( socket + 1, &fd, NULL, NULL, &timeout );
	if ( nRet > 0 )
  	{
  		if ( FD_ISSET(socket, &fd) > 0 ) nRet = 1;
  		else nRet = -1;
  	}
  	return nRet;
}




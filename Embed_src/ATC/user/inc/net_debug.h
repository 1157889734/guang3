#ifndef _NET_DEBUG_H
#define	_NET_DEBUG_H


#define GET_DATA_FROM_ATC  1
#define GET_DATA_FROM_TMS  2
#define GET_DATA_FROM_PIS  4

extern uint8_t send_data_to_net_flag;
extern void net_debug_proc(void);
extern void fill_net_buf(uint8_t *buf, uint16_t length);
extern void set_data_flag(char *buf);
#endif

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 通用函数库 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:utils.h

//*文件作用:通用函数库

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef UTILS_H
#define UTILS_H



extern uint16 swap_int16(uint16 n);
extern uint8 int_to_dec(uint8 *dec, uint32 n);
extern void buf_to_hex(uint8 *hex, uint8 *buf, uint16 length);
extern void char_to_hex(uint8 *hex, uint8 n);
extern void int_to_hex(uint8 *hex, uint32 n);
extern uint8 hex_to_int8(uint8 *hex, uint8 *n);
extern uint8 hex_to_int16(uint8 *hex, uint16 *n);
extern uint8 hex_to_int32(uint8 *hex, uint32 *n);
extern uint8 dec_to_int8(uint8 *dec, uint8 *n);
extern uint8 dec_to_int16(uint8 *dec, uint16 *n);
extern uint8 dec_to_int32(uint8 *dec, uint32 *n);
extern uint16 hex_to_buf(uint8 *hex, uint8 *buf);
extern void print_line(char *str);
extern void ptu_print_debug(char * str);
extern void print_int(char *args, uint32 n);
extern void print_buf(uint8 *buf, uint16 length);
extern void ptu_print_buf(uint8 *buf, uint16 length);
extern void ptu_print_tbabuf(uint8 *buf, uint16 length);

extern void debug_print_line(char *str);
extern void debug_print_int(char *args, uint32 n);



#endif



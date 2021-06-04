//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& crc16  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:crc16.c
//*文件作用:
//*文件作者:mmaple
//*创建日期:2013年1月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef CRC16_H
#define CRC16_H

unsigned short CRC16(const unsigned char *nData, unsigned short wLength);
unsigned char CheckSum(const unsigned char *nData, unsigned short wLength);


#endif



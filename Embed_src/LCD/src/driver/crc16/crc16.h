//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& crc16  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:crc16.c
//*�ļ�����:
//*�ļ�����:mmaple
//*��������:2013��1��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef CRC16_H
#define CRC16_H

unsigned short CRC16(const unsigned char *nData, unsigned short wLength);
unsigned char CheckSum(const unsigned char *nData, unsigned short wLength);


#endif



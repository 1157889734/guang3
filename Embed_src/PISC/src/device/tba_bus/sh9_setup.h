//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& PC������� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:setup.h

//*�ļ�����:PC�������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef SETUP_H
#define SETUP_H



#define TBA_DEV_SETUP		0x0A		//PC�������




//�ӿں���
extern void setup_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);




#endif



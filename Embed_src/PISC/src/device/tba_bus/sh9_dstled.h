//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �յ�վ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:dstled.h

//*�ļ�����:�յ�վ��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DSTLED_H
#define DSTLED_H



#define TBA_DEV_DSTLED		0x07	//�յ�վ��
#define TBA_DSTLED_NUM		0x02	//�յ�վ������



//ͨ�Ŵ���
extern uint16 dstled_comm_error[];



//�ӿں���
extern void dstled_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length);



#endif



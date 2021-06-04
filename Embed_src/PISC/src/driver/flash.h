//�������������������������������������������ڲ�flash��̡���������������������������������������������������������
//���ļ�����:flash.h
//���ļ�˵��:�ڲ�flash���
//���ļ�����:��  ��
//����������:2007��5��
//����������������������������������������������������������������������������������������������������������������������



#ifndef FLASH_H
#define FLASH_H


//�洢�������ӳ��
#define FLASH_BASE		0x40000000
#define FLASH_BACKUP_BASE	0x40020000	//����FLASH�����ַ---���ڴ��������ݴ洢�Լ��������صı���
#define FLASH_BACKUP_LENGTH	0x20000		//����FLASH���򳤶�
#define FLASH_PARAM_BASE	0x400C0000	//����������ַ



//IAP������
#define FLASH_WMS_Mask   0x80000000
#define FLASH_SUSP_Mask  0x40000000
#define FLASH_WPG_Mask   0x20000000
#define FLASH_DWPG_Mask  0x10000000
#define FLASH_SER_Mask   0x08000000
#define FLASH_SPR_Mask   0x01000000
#define FLASH_DBGP_Mask  0x00000002
#define FLASH_ACCP_Mask  0x00000001


extern void  flash_init(void);
extern uint8 flash_erase(uint32 start, uint32 length);
extern uint8 flash_write(uint8 *buf, uint32 start, uint32 length);
extern void  flash_copy(uint32 length);


#endif



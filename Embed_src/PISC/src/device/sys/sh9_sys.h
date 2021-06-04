//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ϵͳģ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:sys.h

//*�ļ�����:ϵͳģ��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef SYS_H
#define SYS_H


//Ӳ������
#if 0
#define DEV_ID_DET_REG		GPIO0_PD
#define DEV_ID_DET_BIT		(12)
#define CAB_KEY_REG		GPIO2_PD
#define CAB_KEY_BIT		(1)
#endif
#define DEV_ID_DET_REG 		RD_EXT_IO_STAUTS
#define DEV_ID_DET_BIT0		(14)
#define DEV_ID_DET_BIT1		(10)
#define CAB_KEY_REG		RD_EXT_IO_STAUTS
#define CAB_KEY_BIT              (6)

#define B_S_SWITCH_REG		RD_EXT_IO_STAUTS
#define B_S_SWITCH_BIT        (7)  //�����л�

#define LINK_CTRL_REG		RD_EXT_IO_STAUTS
#define LINK_CTRL_BIT            (9)  //����

#define B_S_CLOSE_DELAY_REG		RD_EXT_IO_STAUTS
#define B_S_CLOSE_DELAY_BIT        (13)  //�ӳٹػ�
//USB����ָʾ�ƺ�
#define USB_LED_NO		6



//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
}sys_dev_t;
extern sys_dev_t sh9_sys_dev;



#endif



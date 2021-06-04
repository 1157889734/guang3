//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& �����豸���� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:device.c

//*�ļ�����:�����豸����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//�����豸ָ�붨��
sys_dev_t *sys_dev=NULL;
ptu_dev_t *ptu_dev=NULL;
tms_dev_t *tms_dev=NULL;
tba_bus_dev_t *tba_bus_dev=NULL;
apu_dev_t *apu_dev=NULL;
dva_dev_t *dva_dev=NULL;
media_dev_t *media_dev=NULL;
panel_dev_t *panel_dev=NULL;
cab_bus_dev_t *cab_bus_dev=NULL;
occ_dev_t *occ_dev = NULL;
rtc_dev_t *rtc_dev_p = NULL;

//***********************************************************************************************************************
//��������:�����豸�����ʼ��
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void device_init(void)
{
		
	//ָ��ʹ�õ��豸
	sys_dev=&sh9_sys_dev;//ָ��SYS�豸
	ptu_dev=&sh9_ptu_dev;//ָ��PTU�豸
	tms_dev=&sh9_tms_dev;//ָ��TMS�豸
	tba_bus_dev=&sh9_tba_bus_dev;//ָ���г������豸
	apu_dev=&sh9_apu_dev;//ָ��APU�豸
	dva_dev=&sh9_dva_dev;//ָ��DVA�豸
	media_dev=&sh9_media_dev;//ָ��MEDIA�豸
	//panel_dev=&shzh24_panel_dev;//ָ������豸
	occ_dev = &sh9_occ_dev;  //occ ��ʼ��
	cab_bus_dev = &xian2_cab_bus_dev;

	//rtc
	rtc_dev_p = &rtc_dev;
	rtc_dev_p->init();

	
	//��ʼ���豸
	sys_dev->init();//SYS�豸��ʼ��
	ptu_dev->init();//PTU�豸��ʼ��
	tms_dev->init();//TMS�豸��ʼ��
	//tba_bus_dev->init(MY_NET_ID,(((DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))?1:2)<<4)|MY_DEV_TYPE);//�г������豸��ʼ��
	tba_bus_dev->init(MY_NET_ID,(read_pisc_id()<<4)|MY_DEV_TYPE);//�г������豸��ʼ��
	apu_dev->init();//APU�豸��ʼ��
	//dva_dev->init();//DVA�豸��ʼ��
	media_dev->init();//MEDIA�豸��ʼ��
	//panel_dev->init();//����豸��ʼ��
	occ_dev->init();
	cab_bus_dev->init();

     	other_default_data.cab_status.dva_error =1;
	other_default_data.tms_status.tms_valid=1;
	memset((uint8*)&error_pack, 0x00, sizeof(error_packet_t));

       read_flash_id();
	
}



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��Ƶ���Ƶ�Ԫ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:apu.c

//*�ļ�����:��Ƶ���Ƶ�Ԫ

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//ͨ��Э��ؼ���
#define APU_PACKET_VOLUME	0x00	//��������
#define APU_PACKET_FFR		0x01	//ǰ������
#define APU_PACKET_FFL		0x02	//ǰ������
#define APU_PACKET_FRR		0x03	//��������
#define APU_PACKET_FRL		0x04	//��������
#define APU_PACKET_BASS		0x05	//�ص���
#define APU_PACKET_TREBLE	0x06	//����
#define APU_PACKET_SWITCH	0x07	//�л�ͨ��

//ͨ��ѡ�������
#define APU_CHANNEL_A		0x07
#define APU_CHANNEL_B		0x06
#define APU_CHANNEL_C		0x05
#define APU_CHANNEL_D		0x04
#define APU_CHANNEL_NULL	0x00



static uint8 apu_channel=APU_CHANNEL_NONE;//��ǰѡ���ͨ��
static uint8 apu_pa_onoff_flag=0;//��ǰPA��״̬
static uint32 apu_send_timer=0;//���Ͷ�ʱ��

uint8 apu_error = 0;
static uint8 mute_flag = 0;
//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ����ƽ��
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_set_balance(uint8 left, uint8 right)
{
	uint8 buf[2];
	
	//ǰ��
	buf[0]=APU_PACKET_FFL;
	buf[1]=left;
	APU_SEND_BUF(buf,2);
	//����
	buf[0]=APU_PACKET_FRL;
	buf[1]=left;
	APU_SEND_BUF(buf,2);
	//ǰ��
	buf[0]=APU_PACKET_FFR;
	buf[1]=right;
	APU_SEND_BUF(buf,2);
	//����
	buf[0]=APU_PACKET_FRR;
	buf[1]=right;
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƹ�������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_set_volume(uint8 volume_cmd, uint8 volume)
{
	uint8 buf[2];
	
	buf[0]=volume_cmd;
	buf[1]=volume;
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ�л�ͨ��
//����˵��:0---��ѡ���κ�ͨ��
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_switch(void)
{
	uint8 buf[2];
	
	if(apu_channel==APU_CHANNEL_NONE)
	{
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_NULL;	
	}
	else if(apu_channel==APU_CHANNEL_OCC)
	{
		apu_set_balance(0x00,saveparam.occ_volume);//�ر������� ������ͨ������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_A;
	}
	else if(apu_channel==APU_CHANNEL_DVA)
	{
		apu_set_balance(0x00,saveparam.dva_volume);//�ر������� ������ͨ������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_MEDIA)
	{
		apu_set_balance(saveparam.media_volume,0x00);//�ر������� ������ͨ������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_MEDIA_DOOR)
	{
		apu_set_balance(saveparam.door_volume,0x00);//�ر������� ������ͨ������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_DVA_DOOR)
	{
		apu_set_balance(0x00,saveparam.door_volume);//�ر������� ������ͨ������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	else if(apu_channel==APU_CHANNEL_OCC_MEDIA)
	{
		apu_set_balance(saveparam.media_volume-8,saveparam.occ_volume);//������ͨ������ͨ��������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_A;
	}
	else if(apu_channel==APU_CHANNEL_DVA_MEDIA)
	{
		apu_set_balance(saveparam.media_volume-8,saveparam.dva_volume);//������ͨ������ͨ��������
		buf[0]=APU_PACKET_SWITCH;
		buf[1]=APU_CHANNEL_B;
	}
	APU_SEND_BUF(buf,2);
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ���ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_proc(void)
{
	if(apu_error >= 40)
    	{
	  	led_onoff(BROADCAST_OCC_LED_NO, 0);
		led_onoff(BROADCAST_DVA_LED_NO, 0);
		led_onoff(BROADCAST_MEDIA_LED_NO, 0);
		my_default_data.cab_status.apu_error = 1;
    	}
	else
	{
	    	my_default_data.cab_status.apu_error = 0;
	}

	//��ʱ����
	if(checktimer(&apu_send_timer,1000))
	{		
		//����PA��
		if(apu_pa_onoff_flag==0)
		{
			APU_PAOUT_CTRL_REG &= ~(1<<APU_PAOUT_CTRL_BIT1);
			APU_PAOUT_CTRL_REG &= ~(1<<APU_PAOUT_CTRL_BIT2);
		}
		else
		{
			APU_PAOUT_CTRL_REG |= (1<<APU_PAOUT_CTRL_BIT1);
			APU_PAOUT_CTRL_REG |= (1<<APU_PAOUT_CTRL_BIT2);
		}
	
		//����������
		apu_set_volume(APU_PACKET_VOLUME,saveparam.global_volume);
		//�����ص���
		apu_set_volume(APU_PACKET_BASS,saveparam.apu_bass);
		//���ø���
		apu_set_volume(APU_PACKET_TREBLE,saveparam.apu_treble);
		//�л�ͨ��
		apu_switch();
	}
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ����PA_ON
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_set_pa_out(uint8 onoff)
{
	apu_pa_onoff_flag=onoff;
	if(apu_pa_onoff_flag==0)
	{
		APU_PAOUT_CTRL_REG&=~(1<<APU_PAOUT_CTRL_BIT1);
		APU_PAOUT_CTRL_REG&=~(1<<APU_PAOUT_CTRL_BIT2);
	}
	else
	{
		APU_PAOUT_CTRL_REG|=(1<<APU_PAOUT_CTRL_BIT1);
		APU_PAOUT_CTRL_REG|=(1<<APU_PAOUT_CTRL_BIT2);
	}
}


static void apu_set_door_out(uint8 onoff)
{
	if(onoff==0)
	{
		APU_DOOROUT_CTRL_REG&=~(1<<APU_DOOROUT_CTRL_BIT);		
	}
	else
	{
		APU_DOOROUT_CTRL_REG|=(1<<APU_DOOROUT_CTRL_BIT);		
	}
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ��ȡPA_ON
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 apu_get_pa_out(void)
{
	return apu_pa_onoff_flag;
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ��ȡPA_IN
//����˵��:
//ע������:
//����˵��:�ڲ��ŷ���1
//***********************************************************************************************************************
static uint8 apu_get_pa_in(void)
{
	if(APU_PA_IN_REG&(1<<APU_PA_IN_BIT))
		return 0;
	else 
		return 1;
}

static uint8 apu_get_door_in(void)
{
	if(APU_DOOR_IN_REG&(1<<APU_DOOR_IN_BIT))
		return 0; // �����ߵ�ƽ�ǲ�����
	else 
		return 1;
}

static uint8 apu_get_test_in(void)
{
	if(APU_DOOR_IN_REG&(1<<APU_TEST_IN_BIT))
		return 0; // �����ߵ�ƽû��
	else 
		return 1;
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ��ȡOCC_IN
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 apu_get_occ_in(void)
{
	if(APU_OCC_IN_REG&(1<<APU_OCC_IN_BIT))
		return 0;
	else 
		return 1;
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ����ͨ��
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_set_channel(uint8 channel)
{
       if(mute_flag) //��������
       {
           apu_channel = APU_CHANNEL_NONE;
	     apu_dev->set_pa_out(0);//�ͷ�PA��
       }
	if(apu_channel!=channel)
	{		
		apu_channel=channel;
		apu_switch();
		apu_switch();
		apu_switch();
	}
}

//***********************************************************************************************************************
//��������:��Ƶ���Ƶ�Ԫ��ʼ������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void apu_init(void)
{	
	//���ùܽ�����Ϊ���
	GPIO2_PC0|=(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC1&=~(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC2|=(1<<APU_PAOUT_CTRL_BIT1);
	GPIO2_PC0|=(1<<APU_PAOUT_CTRL_BIT2);
	GPIO2_PC1&=~(1<<APU_PAOUT_CTRL_BIT2);
	GPIO2_PC2|=(1<<APU_PAOUT_CTRL_BIT2);
/*
	//����PALINE���ܽ�����Ϊ����+������
	GPIO2_PC0|=(0x0001<<APU_PA_IN_BIT);
	GPIO2_PC1|=(0x0001<<APU_PA_IN_BIT);
	GPIO2_PC2&=~(0x0001<<APU_PA_IN_BIT);
	//����OCC���ܽ�����Ϊ����+������
	GPIO2_PC0|=(0x0001<<APU_OCC_IN_BIT);
	GPIO2_PC1|=(0x0001<<APU_OCC_IN_BIT);
	GPIO2_PC2&=~(0x0001<<APU_OCC_IN_BIT);
*/			
	//��ʼ��ͨ�Ŷ˿�
	APU_PORTINIT(APU_BAUDRATE);
}

static void apu_set_mute_flag(uint8 flag)
{
    mute_flag = flag;
}
static uint8 apu_get_mute_flag()
{
    return mute_flag;
}

//��������豸��
apu_dev_t sh9_apu_dev=
{
	apu_init,
	apu_proc,
	apu_set_pa_out,
	apu_get_pa_out,
	apu_get_pa_in,
	apu_get_occ_in,
	apu_set_channel,
	apu_set_mute_flag,
	apu_get_mute_flag,
	apu_set_door_out,
	apu_get_door_in,
	apu_get_test_in,
};



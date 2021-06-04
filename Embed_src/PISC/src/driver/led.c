//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ledָʾ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:led.c

//*�ļ�����:ledָʾ��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//LED����λ
static uint8 const led_ctrl_bit[]=
{
	LED1_CTRL_BIT,
	LED2_CTRL_BIT,
	LED3_CTRL_BIT,
	LED4_CTRL_BIT,
	LED5_CTRL_BIT,
	LED6_CTRL_BIT,
	LED7_CTRL_BIT,
	LED8_CTRL_BIT,
	LED9_CTRL_BIT,
	LED10_CTRL_BIT,
};
//LED����״̬
static uint16 led_state=0;
//��˸��ʱ��
static uint32 led_blink_timer[LED_NUM]={0};



//***********************************************************************************************************************
//��������:ledָʾ�Ƴ�ʼ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void led_init(void)
{
	uint8 led_index;
	unsigned short tmp;
	// Set GPIO1 pin 0, 1. 2, 3, 4, 5, 6, 7, 8, 9 is GPIO_OUT_PP
	tmp = MARST_LED_Pin | SLAVE_LED_Pin | TCMS_LED_Pin | TRAIN_LED_Pin | OCC_LED_Pin | MEDIA_LED_Pin | USB_LED_Pin | OCC_S_LED_Pin | DVA_S_LED_Pin | MEDIA_S_LED_Pin;
	GPIO1_PC0 |= tmp;
	GPIO1_PC1 &= ~tmp;
	GPIO1_PC2 |= tmp;
	GPIO1_PD &= ~tmp;
	
	for(led_index=0; led_index<LED_NUM; led_index++)
		led_onoff(led_index, 1);	
}

//***********************************************************************************************************************
//��������:ledָʾ�ƿ���
//����˵��:��
//ע������:
//����˵��:��
//***********************************************************************************************************************
void led_onoff(uint8 led_no, uint8 onoff)
{
#if 1 	
       led_state=LED_CTRL_REG;
	if(led_no>=LED_NUM)
		return;

//Ӳ���汾1
#if(HARDWARE_VERSION==1)
	if(onoff)
		led_state |= (1<<led_ctrl_bit[led_no]);
	else
		led_state &= ~(1<<led_ctrl_bit[led_no]);
//Ӳ���汾2
#elif(HARDWARE_VERSION==2)
	if(onoff)
		led_state &= ~(1<<led_ctrl_bit[led_no]);
	else
		led_state |= (1<<led_ctrl_bit[led_no]);
#endif
	LED_CTRL_REG=led_state;
#endif
}

//***********************************************************************************************************************
//��������:ledָʾ�ƿ���
//����˵��:��
//ע������:
//����˵��:��
//***********************************************************************************************************************
void led_toggle(uint8 led_no)
{
//Ӳ���汾1
#if(HARDWARE_VERSION==1)
	//�����ǰ����Ϩ��
	if( led_state & (1<<led_ctrl_bit[led_no]) )
	{
		led_onoff(led_no,0);
	}
	else
	{
		led_onoff(led_no,1);
	}
//Ӳ���汾2
#elif(HARDWARE_VERSION==2)
	//�����ǰ����Ϩ��
	if( ( led_state & (1<<led_ctrl_bit[led_no]) ) == 0 )
	{
		led_onoff(led_no,0);
	}
	else
	{
		led_onoff(led_no,1);
	}
#endif
}

//***********************************************************************************************************************
//��������:ledָʾ�ƿ���
//����˵��:��
//ע������:
//����˵��:��
//***********************************************************************************************************************
void led_blink(uint8 led_no, uint16 on_time, uint16 off_time)
{
	if( led_no>=LED_NUM )
		return;

//Ӳ���汾1
#if( HARDWARE_VERSION == 1 )
	//�����ǰ���� �ж�ʱ�䵽���Ժ�Ϩ��
	if(led_state & (1<<led_ctrl_bit[led_no]))
	{
		if( checktimer(&led_blink_timer[led_no],on_time) )
		{
			led_onoff(led_no, 0);
		}
	}
	//�����ǰϨ�� �ж�ʱ�䵽���Ժ����
	else
	{
		if( checktimer(&led_blink_timer[led_no],off_time) )
		{
			led_onoff(led_no, 1);
		}		
	}
//Ӳ���汾2
#elif(HARDWARE_VERSION == 2)
	//�����ǰ���� �ж�ʱ�䵽���Ժ�Ϩ��
	if((led_state & (1<<led_ctrl_bit[led_no]))==0)
	{
		if( checktimer(&led_blink_timer[led_no],on_time) )
		{
			led_onoff(led_no, 0);
		}
	}
	//�����ǰϨ�� �ж�ʱ�䵽���Ժ����
	else
	{
		if( checktimer(&led_blink_timer[led_no],off_time) )
		{
			led_onoff(led_no, 1);
		}		
	}
#endif
	

}



#ifndef __CONFIG_H
#define __CONFIG_H

#include "include.h"


#define IDU_COMPILE					1
#define FDU_COMPILE					0
#define SDU_COMPILE					0

#define AUTO  						 3
#define MAUAL						 4

/***********************************************************************
	ѡ�����:
	 LIGHT_SIDE : 1    �� CHOISE_PTU :0                
	 
**********************************************************************
*����ѡ��
*  1:  �Ų��
*  2: PTU�汾ѡ��
*  	2.1:  KIND_RUN ��̬��ͼģʽ����IDUģʽ
*  3:  WATCH DOG
*  4:  ���߹���
*  5:  �������е���
*  6: ���Զ����Զ������л�
*  7:��ʾ���ݷ�ʽ==>��2�����Էֳ�2�ζ�����ʾ�����Է���ɫ�;�̬��ʾ/ ��2�����Ժϳ�һ��ͼƬ��ʾ�������ھ�ֹͼƬ�ͷ���ɫ��ʾ
**********************************************************************
ע:
	��3����4 PTU (CHOISE_PTU ==1 )����(KIND_RUN = 1) ,���(CHOISE_PTU ==0 ) ==> (KIND_RUN = 0) 
	
***********************************************************************/
#define LIGHT_SIDE                			1           //�Ų��
#define CHOISE_PTU                               0           // 1: ��3 ��4         //0         ����PTU
#define WATCH_DOG 					1
#define LINE_CHANGE			 		0		//���߹���
#define DEBUG_ALL_LIGHT_ON			0		//�����ã�����ȫ��LED
#define MODE_CHANGE				0		
#define DISPLAY_KIND				0         // 0:���Էֿ�������ʾ����������ͼƬ��ֹ  
										    // 1: 2�����Ժϳ�һ��ͼƬ��ʾ�����������ھ�ֹͼƬ
/*2.1**********************************************************************/

		#define KIND_RUN					1            // 1 : ��̬��ͼ������ģʽ=> (Ԥ��վ������ֹͣ���������Ŵ��� +�Ų����ʾ���Ų���Ϣ)    0: IDU����ģʽ(Ԥ��վ��������վ����+������ʾ���Ų���Ϣ)

/*********************************************************************
*��������ѡ��
*
*1:Ԥ��վ����
*2:��վ����
*3:Խվ���� 
*4:�����㲥
*5:���Ŵ���
*6:���Ŵ���
*7:��վ����
*8:��վͣ��

ѡ��ĺ������Ӧ�����֣���ѡ������0


***************************************************************/
	#define PRE_BROADCAST    		1
	#define ARRIVE_BROADCAST		0
	#define JUMP 					0
	#define EME_FUCTION				1
	#define DOOR_CLOSE  				1
	#define DOOR_OPEN   				1
	#define LEAVING					0
	#define STOP						1

/*********************************************************************/
/**********************************************
*û���Ų�Ƶ�ʱ��,�Ҵ��˽����㲥������������ʾ���Ų�
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		2
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			0
*#define DOOR_OPEN   			0
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*û���Ų�Ƶ�ʱ��,���������㲥������������ʾ���Ų�
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		2
*#define JUMP					0
*#define EME_FUCTION				0
*#define DOOR_CLOSE  			0
*#define DOOR_OPEN   			0
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*���Ų�Ƶ�ʱ��,�Ҵ��˽����㲥�����Զ�����
*#define PRE_BROADCAST    		1
*#define ARRIVE_BROADCAST		0
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			1
*#define DOOR_OPEN   			1
*#define LEAVING					0
*#define STOP					0
*******************************************/
/**********************************************
*���Ų�Ƶ�ʱ��,�Ҵ��˽����㲥���Զ�����
*#define PRE_BROADCAST    		0
*#define ARRIVE_BROADCAST		1
*#define JUMP					0
*#define EME_FUCTION				4
*#define DOOR_CLOSE  			1
*#define DOOR_OPEN   			1
*#define LEAVING					0
*#define STOP					0
*******************************************/


#define CONFIG_MSG_LEN			5
#define CONFIG_AD_LEN			5
#define CONFIG_TOTAL_LEN		(2+CONFIG_MSG_LEN*2+CONFIG_AD_LEN*2+3+3)



#define GREETING "          Shenzhen BeiHai \r\n    "
#define NO_PROCESS_TIME                                1000         //������


#if IDU_COMPILE
#define LOCAL_DEVICE_TYPE					IDU
#define LED_PHYSICS_WIDTH					(24)		//�����ģ����Ÿ���

#define SHIFT_LEFT_NULL_WIDTH				32          //�ƶ���ɺ󣬵ȴ�XXʱ�������¿�ʼ��
#define VERSION "IDU LED   2013-06-07 V1.9\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						9

#define SHIFT_SPEED							0			//ɨ��n  ���ƶ�1��
#define STAY_TIME							650			//������ʾ���ݼ��ʱ��    //����ʱ��̫��������ʾ��ͼƬ����С�����ʱ�䣬���ͼƬ����ʾ2��
#define KEEP_TIME							0			//��Ļ��������ʱ����ʾ���ʱ��    16*SYSTICK ԼΪ 20ms
#endif

#if FDU_COMPILE
#define LOCAL_DEVICE_TYPE					FDU
#define LED_PHYSICS_WIDTH					(14)		//�����ģ����Ÿ���
#define SHIFT_LEFT_NULL_WIDTH				8          //�ƶ���ɺ󣬵ȴ�XXʱ�������¿�ʼ��
#define VERSION "FDU LED   2013-06-17 V1.7\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						7

#define SHIFT_SPEED							10			//ɨ��n  ���ƶ�1��
#define STAY_TIME							1200		//������ʾ����ʱ��
#endif

#if SDU_COMPILE
#define LOCAL_DEVICE_TYPE					SDU
#define LED_PHYSICS_WIDTH					(14)		//�����ģ����Ÿ���
#define SHIFT_LEFT_NULL_WIDTH				8			//�ƶ���ɺ󣬵ȴ�XXʱ�������¿�ʼ��
#define VERSION "SDU LED   2013-06-17 V1.7\r\n"
#define MSTER_VERSION						1
#define SLAVE_VERSION						7

#define SHIFT_SPEED							0			//ɨ��n  ���ƶ�1��
#define STAY_TIME							1200		//������ʾ����ʱ��
#endif



#define IMAGE_NO_PROCESS_WIDTH				 ((LED_PHYSICS_WIDTH)+2)
#define IMAGE_LED_TEST_WIDTH					((LED_PHYSICS_WIDTH)+2)





#endif

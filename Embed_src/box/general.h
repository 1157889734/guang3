//sfr16 DPTR = 0x82;

#ifndef GENERAL_H_
#define GENERAL_H_

//#include<reg51.h>
#include<absacc.h>
#include<stdio.h>
#include<intrins.h>
#include	"STC15Fxxxx.H"

#define byte unsigned char           //8bit
#define word unsigned int            //16bit
#define dword unsigned long          //32bit


#define T0_INTERVAL_L			0x00
#define T0_INTERVAL_H			0xdc

	



#define ENABLE				1
#define DISABLE				0


#define KEY_VALID			1
#define KEY_INVALID			0




//OUT PUT
/*
sbit MIC_CTRL_PIN			=P1^0;
sbit BUZZER_CRTL_PIN		=P1^1;
sbit SHIFT_CRTL_CLK			=P1^2;
sbit SHIFT_CRTL_DATA		=P1^3;
sbit SHIFT_CRTL_LE			=P1^4;
sbit Com_R_W			       =P1^5;
*/
sbit MIC_CTRL_PIN			=P0^0;
sbit BUZZER_CRTL_PIN		=P0^1;
sbit SHIFT_CRTL_CLK			=P0^2;
sbit SHIFT_CRTL_DATA		=P0^3;
sbit SHIFT_CRTL_LE			=P0^4;



//IN
sbit KEY_PA_STAT				=	P2^7;
sbit KEY_PC_STAT				=	P2^6;
sbit KEY_PTT_STAT				=	P2^5;
sbit KEY_CC_STAT				=	P2^4;
sbit KEY_PCRST_STAT		=	P2^1;
sbit KEY_LED_TEST_STAT		=	P2^0;


sbit DEBUG_PIN				=P2^0;



typedef struct {
	byte dest_net_id;
	byte dest_device_id;
	byte src_net_id;
	byte src_device_id;
	byte cmd;
	byte len;
	byte pisc_data[35];
	byte data_area[3];
}tFrameHeader;

typedef struct 
{
	byte dest_net_id;
	byte dest_device_id;
	byte src_net_id;
	byte src_device_id;
	byte cmd;
	byte len;
	byte hmi_area[12];
}tHmiHeader;


#endif


//#include<reg51.h>
#include<absacc.h>
#include<stdio.h>
#include<intrins.h>
#include<math.h>

#include<general.h>
#include<com.h>
#include<Timer.h>
#include<key.h>
#include "led_control.h"
//#include	"STC15Fxxxx.H"

//STC89C58RD+ sfr define
/*
sfr WDT_CONTR = 0xe1;
sfr ISP_COUNTER = 0xe7;
//RAM control register
//sfr AUXR = 0x8e;

sfr P4 = 0xe8;
sfr XICON = 0xc0;
sfr IPH = 0xb7;
*/
#define Dev_SUB 		0x02
#define Dev_MAIN		0x01
#define Dev_IDLE		0x00

#define UP		0x02	//内环
#define DOWN 	0x01	//外环

#define BELL_ON 1
#define BELL_OFF 0





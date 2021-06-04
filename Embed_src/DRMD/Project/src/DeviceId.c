#include "include.h"



u8 gDeviceId;
u8 gLedDispNum;
static u8 gSegIndex;

const u8 seg_led_bit[11][7] = {
	{0, 0, 0, 0, 0, 0, 1},  //0
	{1, 0, 0, 1, 1, 1, 1},   // 1
	{0, 0, 1, 0, 0, 1, 0},   // 2
	{0, 0, 0, 0, 1, 1, 0},   // 3
	{1, 0, 0, 1, 1, 0, 0},   // 4
	{0, 1, 0, 0, 1, 0, 0},   // 5
	{0, 1, 0, 0, 0, 0, 0},   // 6
	{0, 0, 0, 1, 1, 1, 1},   // 7
	{0, 0, 0, 0, 0, 0, 0},   // 8
	{0, 0, 0, 0, 1, 0, 0},   // 9
	{1, 1, 1, 1, 1, 1, 1},   // DARK
			
};


void LedDispId(void)
{
	u8 temp;
	static u8 flag;


	if (flag == 0) {
		GPIO_ResetBits(GPIOD, GPIO_Pin_10);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		flag = 1;
		return;
	}
	flag = 0;

	gSegIndex ++;
	gSegIndex %= 2;

	gLedDispNum %= 100;

	temp = (gSegIndex==0) ? (gLedDispNum%10) : (gLedDispNum/10);

	//十位数字如果为0   灭掉
	if ((gSegIndex == 1) && (gLedDispNum/10 == 0)) temp = 10;
		

	// A
	if (seg_led_bit[temp][0]) GPIO_SetBits(GPIOD, GPIO_Pin_12);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	
	//B
	if (seg_led_bit[temp][1]) GPIO_SetBits(GPIOD, GPIO_Pin_9);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_9);

	//C
	if (seg_led_bit[temp][2]) GPIO_SetBits(GPIOD, GPIO_Pin_8);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_8);

	//D
	if (seg_led_bit[temp][3]) GPIO_SetBits(GPIOB, GPIO_Pin_13);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_13);

	//E
	if (seg_led_bit[temp][4]) GPIO_SetBits(GPIOB, GPIO_Pin_15);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_15);

	//F
	if (seg_led_bit[temp][5]) GPIO_SetBits(GPIOD, GPIO_Pin_11);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_11);

	//G
	if (seg_led_bit[temp][6]) GPIO_SetBits(GPIOD, GPIO_Pin_13);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_13);

	(gSegIndex) ? (GPIO_SetBits(GPIOD, GPIO_Pin_10)) : (GPIO_SetBits(GPIOB, GPIO_Pin_12));
}


void GetDeviceId(void)
{
	static u8 temp, old;
	static u32 tick = 0;
	static u32 chg_tick = 0;

	temp = 0;
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15)) temp |= 1;
	temp <<= 1;
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14)) temp |= 1;
	temp <<= 1;	
	if(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)) temp |= 1;
	temp <<= 1;
	if(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)) temp |= 1;

	tick ++;
	if (temp != old) {
		tick = 0;
		old = temp;
	}
	else if (tick > 1000) {
		//显示id  号
		if (gSystemTick - chg_tick < 300) {
			gDeviceId = temp;
			gDeviceId += 1;
			gLedDispNum = gDeviceId;
			tick = 0;
		}
		else {  //显示版本号
			temp = MSTER_VERSION;
			temp *= 10;
			temp += SLAVE_VERSION;
			gLedDispNum = temp;
			if (gSystemTick - chg_tick > 400) {
				chg_tick = gSystemTick;
			}
		}
	}
	else {
		old = temp;
	}
}









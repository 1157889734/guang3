#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>

#include "gpio_i2c.h"
#include "my_rtc.h"

//#define DEBUG

#define DEV_ADDR (0xD0)
#define DS1307_SEC_REG (0x00)
#define  DS1307_CTRL_REG (0x07)

#define DS1307_ALL_DISABLED         0b00000000 // All disabled
#define DS1307_OUT_ON_DISABLED_HIHG 0b10000000 // Out to Hight on Disable Out
#define DS1307_OUT_ENABLED          0b00010000 // Out Enabled
#define DS1307_OUT_1_HZ             0b00000000 // Freq. Out to 1 Hz
#define DS1307_OUT_4_KHZ            0b00000001 // Freq. Out to 4.096 Khz
#define DS1307_OUT_8_KHZ            0b00000010 // Freq. Out to 8.192 Khz
#define DS1307_OUT_32_KHZ           0b00000011 // Freq. Out to 32.768 Khz

#define Start_user_address_nvram    0x08
#define End_user_address_nvram      0x3f

#define DEVICE_NAME "/dev/gpioi2c"

char days_of_week[7][11]={"Monday\0","Tuesday\0","Wednesday\0","Thursday\0","Friday\0","Saturday\0","Sunday\0"};

byte ds1307_bin2bcd(byte binary_value);
byte ds1307_bcd2bin(byte bcd_value);

#ifdef DEBUG
#define dbg(args...) printf(args)
#else
#define dbg(args...)
#endif


int i2c_write(int dev_addr, int reg_addr, int reg_val)
{
	int fd = open(DEVICE_NAME, 0);
	int  ret,value;
	if (fd < 0)
	{
		printf("Open i2c device error!\n");
		return -1;
	}
	value = ((dev_addr&0xff)<<24) | ((reg_addr&0xff)<<16) | (reg_val&0xffff);
	dbg("write: device_addr:0x%02x; reg_addr:0x%02x; reg_value:0x%02x.\n", dev_addr, reg_addr, value);
	ret = ioctl(fd, GPIO_I2C_WRITE, &value);
	if (ret)
	{
		printf("i2c write failed!\n");
		return -1 ;
	}
	close(fd);
	return 0;
}

int i2c_read(int dev_addr, int reg_addr, int *p_val)
{
	int fd = open("/dev/gpioi2c", 0);

	int ret,reg_value;
	if (fd<0)
	{
		printf("Open gpioi2c dev error!\n");
		return -1;
	}

	dbg("device_addr:0x%02x; reg_addr:0x%02x.\n", dev_addr, reg_addr);
	*p_val = ((dev_addr&0xff)<<24) | ((reg_addr&0xff)<<16);
	ret = ioctl(fd, GPIO_I2C_READ, p_val);
	reg_value = (*p_val)&0xff;
	dbg("0x%2x\n", reg_value);
	return 0;
}

int i2c_multi_read(int dev_addr, int reg_addr, int reg_addr_end)
{
	int ret;
	int reg_value, value;
	int fd = open("/dev/gpioi2c", 0);
	if (fd<0)
	{
		printf("Open gpioi2c dev error!\n");
		return -1;
	}

	int cur_addr;
	if (reg_addr_end < reg_addr)
	{
		printf("end addr(0x%02x) should bigger than start addr(0x%02x)\n",
		       reg_addr_end, reg_addr);
		return -1;
	}

	printf("device_addr:0x%02x; reg_addr_start:0x%02x; reg_addr_end:0x%02x.\n",
	       dev_addr, reg_addr, reg_addr_end);
	for (cur_addr=reg_addr; cur_addr<reg_addr_end+1; cur_addr++)
	{
		value = ((dev_addr&0xff)<<24) | ((cur_addr&0xff)<<16);
		ret = ioctl(fd, GPIO_I2C_READ, &value);
		reg_value = value&0xff;
		printf("0x%x 0x%x\n", cur_addr, reg_value);
	}
	return 0;
}

int ds1307_init(int val) {

	byte seconds = 0;
	int ret;

	/* 配置ds1307 控制寄存器 */
	i2c_write(DEV_ADDR, DS1307_CTRL_REG, (byte)val);
	ret = i2c_read(DEV_ADDR, DS1307_SEC_REG ,(int*)(&seconds));
	if (ret < 0) {
		return -1;
	}
	/* 使能晶振 */
	seconds &= 0x7f;
	i2c_write(DEV_ADDR, DS1307_SEC_REG, seconds);
	return 0;
}



void ds1307_set_time(struct ds1307_time_t *time)
{
	time->sec = (((byte)(time->sec)) & 0x7F);
	time->hour = ((byte)(time->hour)) & 0x3F;
	time->year %= 100;           
	i2c_write(DEV_ADDR, 0x0, ds1307_bin2bcd((byte)(time->sec)));
	i2c_write(DEV_ADDR, 0x1, ds1307_bin2bcd((byte)(time->min)));
	i2c_write(DEV_ADDR, 0x2, ds1307_bin2bcd((byte)(time->hour)));
	i2c_write(DEV_ADDR, 0x3, ds1307_bin2bcd((byte)(time->dow)));
	i2c_write(DEV_ADDR, 0x4, ds1307_bin2bcd((byte)(time->date)));
	i2c_write(DEV_ADDR, 0x5, ds1307_bin2bcd((byte)(time->month)));
	/*printf("-------------------->time->year = %d \n", time->year);
	printf("-------------------->time->month = %d \n", time->month);
	printf("-------------------->time->date = %d \n", time->date);
	printf("-------------------->time->dow = %d \n", time->dow);
	printf("-------------------->time->hour = %d \n", time->hour);
	printf("-------------------->time->min = %d \n", time->min);
	printf("-------------------->time->sec = %d \n", time->sec);*/
	i2c_write(DEV_ADDR, 0x6, ds1307_bin2bcd((byte)(time->year)));

}

void ds1307_get_time(struct ds1307_time_t *p_time)
{
	int tmp;
	printf("+++++++++++++++++++update-time+++++++++++++++++++++\n");
	i2c_read(DEV_ADDR, 0x00, &tmp);
	p_time->sec = ds1307_bcd2bin(tmp & 0x7f);
	i2c_read(DEV_ADDR, 0x01, &tmp);
	p_time->min = ds1307_bcd2bin(tmp & 0x7f);
	i2c_read(DEV_ADDR, 0x02, &tmp);
	p_time->hour = ds1307_bcd2bin(tmp & 0x3f);
	i2c_read(DEV_ADDR, 0x03, &tmp);
	p_time->dow = ds1307_bcd2bin(tmp & 0x7f);
	i2c_read(DEV_ADDR, 0x04, &tmp);
	p_time->date = ds1307_bcd2bin(tmp & 0x3f);
	i2c_read(DEV_ADDR, 0x05, &tmp);
	p_time->month = ds1307_bcd2bin(tmp & 0x1f);
	i2c_read(DEV_ADDR, 0x06, &tmp);
	p_time->year = ds1307_bcd2bin(tmp);

	printf("%02d:%02d:%02d  %04d-%02d-%02d  day of week:%d\n", p_time->hour, p_time->min, p_time->sec, \
	    p_time->year, p_time->month, p_time->date, p_time->dow);

}

#if 0
char ds1307_read_nvram_byte(char addr) {

	char retval;

	i2c_start();
	i2c_write(0xD0);
	i2c_write(addr);

	i2c_start();
	i2c_write(0xD1);
	retval = i2c_read(0);
	i2c_stop();

	return(retval);

}

void ds1307_write_nvram_byte(char addr, char value) {


	i2c_start();
	i2c_write(0xD0);
	i2c_write(addr);
	i2c_write(value);
	i2c_stop();

}


void ds1307_get_day_of_week(char* ptr) {

	byte lday;
	byte lmonth;
	byte lyr;
	byte ldow;
	ds1307_get_date(lday,lmonth,lyr,ldow);
	sprintf(ptr,"%s",days_of_week[ldow]);
}
#endif
///////////////////////////////////////////////////////////////////////////////

byte ds1307_bin2bcd(byte binary_value) {

	byte temp;
	byte retval;

	temp = binary_value;
	retval = 0;
	while (1) {
		if (temp >= 10) {
			temp -= 10;
			retval += 0x10;
		} else {
			retval += temp;
			break;
		}
	}
	return(retval);
}

byte ds1307_bcd2bin(byte bcd_value)
{

	byte temp,ret;

	temp = bcd_value;
	temp >>= 1;
	temp &= 0x78;
	ret = (temp + (temp >> 2) + (bcd_value & 0x0f));
	dbg("---------->%2x, %d\n", bcd_value, ret);
	return ret;
}


void SetSystemTime(char *dt)
{
	struct tm systime;
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	sscanf(dt, "%d-%d-%d %d:%d:%d", &systime.tm_year,&systime.tm_mon,&systime.tm_mday,&systime.tm_hour,&systime.tm_min,&systime.tm_sec);
	_tm.tm_sec = systime.tm_sec;
	_tm.tm_min = systime.tm_min;
	_tm.tm_hour = systime.tm_hour;
	_tm.tm_mday = systime.tm_mday;
	_tm.tm_mon = systime.tm_mon - 1;
	_tm.tm_year = systime.tm_year - 1900;

	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if (settimeofday (&tv, (struct timezone *) 0) < 0)
	{
		printf("Set system datatime error!\n");
	}
}

int SetDs1307RtcTimeHuman( char *dt )
{
	struct ds1307_time_t ds1307Time;

	printf("will SetDs1307RtcTimeHuman(%s)!", dt );

	memset(&ds1307Time,0x00, sizeof(ds1307Time));
	sscanf(dt, "%d-%d-%d %d:%d:%d", &ds1307Time.year,&ds1307Time.month,
									&ds1307Time.date,&ds1307Time.hour,
									&ds1307Time.min,&ds1307Time.sec);
	ds1307Time.year -= 2000;
	ds1307Time.year = ds1307Time.year > 0 ? ds1307Time.year : 0;
	ds1307_set_time( &ds1307Time );

	return 0;
}

//获取系统时间写进p_time
void setRtcTimeFromsys(struct ds1307_time_t *p_time)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
 
    tp = localtime(&t);
	p_time->sec = tp->tm_sec;
	p_time->min = tp->tm_min;
	p_time->hour = tp->tm_hour;
	p_time->date = tp->tm_mday;
	p_time->month = tp->tm_mon+1;
	p_time->year = tp->tm_year+1900;

}

int compare_time_and_now(char *dt)
{
	struct tm systime;
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	sscanf(dt, "%d-%d-%d %d:%d:%d", &systime.tm_year,&systime.tm_mon,&systime.tm_mday,&systime.tm_hour,&systime.tm_min,&systime.tm_sec);
	_tm.tm_sec = systime.tm_sec;
	_tm.tm_min = systime.tm_min;
	_tm.tm_hour = systime.tm_hour;
	_tm.tm_mday = systime.tm_mday;
	_tm.tm_mon = systime.tm_mon - 1;
	_tm.tm_year = systime.tm_year - 1900;
	timep = mktime(&_tm);
	time_t time_now = time(0);
	return abs(timep - time_now);
}
void rtc_init()
{
	ds1307_init(0x03);
	struct ds1307_time_t *p_time = (struct ds1307_time_t *)malloc(sizeof(struct ds1307_time_t));
	ds1307_get_time(p_time);//从rtc获取时间
	//setRtcTimeFromsys(p_time);//从系统获取时间
	char datetime[100];
	sprintf(datetime, "%d-%d-%d %d:%d:%d", 2000+p_time->year,p_time->month,p_time->date,p_time->hour,p_time->min,p_time->sec);
	printf("datetime=%s\n",datetime);
	SetSystemTime(datetime);
	//ds1307_set_time(p_time);//把时间写进rtc
	free(p_time);
}

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtc_1337.h"

#define LARPYEAR(y) (((0==(y)%400)||((0 ==(y)%4)&&(0 !=(y)%100)))?1:0)



#define I2C_PORT GPIOF
#define I2C_SCL GPIO_Pin_1
#define I2C_SDA GPIO_Pin_0

#define READ_ADDR 0xd1
#define WRITE_ADDR 0xd0
#define I2C1_WAIT_TIME 500

tRtcTimer cur_rtc_time;





void gpio_SDA_output()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Enable the GPIO_LED Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_PORT, &GPIO_InitStructure);
}

void gpio_SDA_input()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Enable the GPIO_LED Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;
    GPIO_Init(I2C_PORT, &GPIO_InitStructure);
}
void rtc_1337_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Enable the GPIO_LED Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = I2C_SDA | I2C_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_PORT, &GPIO_InitStructure);
}
void i2c_send_byte(uint8_t data)
{
    uint16_t wait;
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(I2C_PORT, I2C_SCL);
        
        for (wait = 0; wait < I2C1_WAIT_TIME / 3; wait++);
        
        if (data & 0x80)
        {
            GPIO_SetBits(I2C_PORT, I2C_SDA);
        }
        else
        {
            GPIO_ResetBits(I2C_PORT, I2C_SDA);
        }
        data <<= 1;
        
        for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
        
        GPIO_SetBits(I2C_PORT, I2C_SCL);
        
        for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    }
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME / 3; wait++);
}

uint8_t i2c_read_byte(uint8_t *data)
{
    uint8_t tmp = 0;
    uint16_t wait;
    uint8_t i;
    GPIO_SetBits(I2C_PORT, I2C_SDA);
    
    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(I2C_PORT, I2C_SCL);
        
        for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
        
        GPIO_SetBits(I2C_PORT, I2C_SCL);
        
        for (wait = 0; wait < I2C1_WAIT_TIME / 2; wait++);
        
        tmp = tmp << 1;
        
        if (GPIO_ReadInputDataBit(I2C_PORT, I2C_SDA))
        {
            tmp = tmp + 1;
        }
        for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    }
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    *data = tmp;
    return tmp;
}

void i2c_start()
{
    uint16_t wait;
    GPIO_SetBits(I2C_PORT, I2C_SCL);
    GPIO_SetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_ResetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME / 3; wait++);
}

void i2c_stop()
{
    uint16_t wait;
    GPIO_ResetBits(I2C_PORT, I2C_SDA);
    GPIO_SetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_SetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
}
void i2c_ack()
{
    uint16_t wait;
    gpio_SDA_output();
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_ResetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_SetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    gpio_SDA_input();
}

void i2c_noack()
{
    uint16_t wait;
    gpio_SDA_output();
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_SetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_SetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    gpio_SDA_input();
}

uint8_t i2c_waitack()
{
    uint16_t wait;
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    GPIO_SetBits(I2C_PORT, I2C_SDA);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    gpio_SDA_input();
    GPIO_SetBits(I2C_PORT, I2C_SCL);
    
    for (wait = 0; wait < I2C1_WAIT_TIME; wait++);
    
    if (GPIO_ReadInputDataBit(I2C_PORT, I2C_SDA))
    {
        GPIO_ResetBits(I2C_PORT, I2C_SCL);
        return 0;
    }
    
    GPIO_ResetBits(I2C_PORT, I2C_SCL);
    gpio_SDA_output();
    return 1;
}

void set_1337_time(tRtcTimer time)
{
    i2c_start();
    i2c_send_byte(WRITE_ADDR);
    i2c_waitack();
    i2c_send_byte(0x00);  //??????????????????
    i2c_waitack();
    i2c_send_byte(time.sec);  //seconds
    i2c_waitack();
    i2c_send_byte(time.min);  //min
    i2c_waitack();
    i2c_send_byte(time.hour);  //hour
    i2c_waitack();
    i2c_send_byte(1);  //day
    i2c_waitack();
    i2c_send_byte(time.date);  //date
    i2c_waitack();
    i2c_send_byte(time.month);  //mon
    i2c_waitack();
    i2c_send_byte(time.year);  //year
    i2c_waitack();
    i2c_stop();
}

void read_1337_time(tRtcTimer *time)
{
    i2c_start();
    i2c_send_byte(WRITE_ADDR);
    i2c_waitack();
    i2c_send_byte(0x00);  //??????????????????
    i2c_waitack();
    i2c_start();
    i2c_send_byte(READ_ADDR);
    i2c_waitack();
    gpio_SDA_input();
    i2c_read_byte(&time->sec);
    i2c_ack();
    i2c_read_byte(&time->min);
    i2c_ack();
    i2c_read_byte(&time->hour);
    i2c_ack();
    i2c_read_byte(&time->date);
    i2c_ack();
    i2c_read_byte(&time->date);
    i2c_ack();
    i2c_read_byte(&time->month);
    i2c_ack();
    i2c_read_byte(&time->year);
    i2c_noack();
    gpio_SDA_output();
    i2c_stop();
}

/***********************************************************************************************************************
*Function Nane  :  check_time
*Description      :  ????????????????
*Input              :  time ????
*Output            :  ??
*Return            :  1 ???? 0 ????
*Others            :  ??
***********************************************************************************************************************/
uint8 check_time(tRtcTimer time)
{
    u8 month[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if ((59 < time.sec) || (59 < time.min) || (23 < time.hour)
        || (12 < time.month) || (month[time.month - 1] < time.date) || (50 < time.year))
    {
        return 0;
    }
    
    //???????? 2??????????????28 ????
    if (2 == time.month)
    {
        if ((!(LARPYEAR(time.year + 2000))) && (28 < time.date))
        {
            return 0;
        }
    }
    
    return 1;
}


/***********************************************************************************************************************
*Function Nane         :  timer_convert
*Description            :  ????????????
*Input                  :  type   0:10????????16????
                                       1: 16????????10????
                               ttimer ????
*Output                :  ??
*Return                 :  ??
*Others                 :   ??
***********************************************************************************************************************/
void timer_convert(uint8 type, tRtcTimer *ttimer)
{
    uint8 i = 0;
    uint8 time_h = 0;
    uint8 time_l = 0;
    uint8 *buf = (uint8 *)ttimer;
    
    if (1 == type)
    {
        for (i = 0; i < 6; i++)
        {
            time_h = (buf[i] / 10) & 0x0f;
            time_l = (buf[i] % 10) & 0x0f;
            buf[i] = (time_h << 4) + time_l;
        }
    }
    else if (0 == type)
    {
        for (i = 0; i < 6; i++)
        {
            buf[i] = (buf[i] >> 4) * 10 + (buf[i] & 0x0f);
        }
    }
}

/***********************************************************************************************************************
*Function Nane      :  set_rtc_time
*Description        :  ????RTC????
*Input              :  time ????
*Output             :  ??
*Return             :  0 ????????  1 ????
*Others             :  ??
***********************************************************************************************************************/
uint8 set_rtc_time(tRtcTimer time)
{
    if (check_time(time))
    {
        timer_convert(1, &time);
        set_1337_time(time);
        return 1;
    }
    else
    {
        return 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  get_cur_time
*Description      :  ????????????
*Input              :  time :????
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void get_rtc_time(tRtcTimer *time)
{
    read_1337_time(time);
    timer_convert(0, time);
}
/***********************************************************************************************************************
*Function Nane  :  update_cur_time
*Description      :  ????????????
*Input              :  ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void update_cur_time(void)
{
    get_rtc_time(&cur_rtc_time);
}

/***********************************************************************************************************************
*Function Nane  :  get_cur_time
*Description      :  ????????????
*Input              :  ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void get_cur_time(tRtcTimer *time)
{
    memcpy(time, &cur_rtc_time, sizeof(tRtcTimer));
}

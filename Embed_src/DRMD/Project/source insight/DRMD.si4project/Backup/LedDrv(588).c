
#include "LedDrv.h"
#include "include.h"

#include "stm32f2x7_eth.h"
#include "stm32f2xx_iwdg.h"
#include "netconf.h"
#include "main.h"
//#include "tftpserver.h"
#include "httpserver.h"
#include "telnet.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "tba_3093.h"
#include "stm32_eval_sdio_sd.h"
#include "sd_data.h"
#include "flash_if.h"
//#include "udp_server.h"
#include "ff.h"

#include "common.h"
//#include "menu.h"
#include  "usbd_hid_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include  "stm32_eval.h"
#include  "usb.h"



extern uint8_t gDownFirmware;

u8 SpiDmaBuf[LED_PHYSICS_WIDTH];

u32 gStartCnt;

u8 gOldTerminal = 0xff;

tDoorStat gDoorStat;
tDispStat gDispStat;
tSysConfig gSysConfg;
tTrigerValue gTrigerValue;
//tLedColor gLedResetColor;


#define IMAGE_LOG_IMAGE_WIDTH               48
#define IMAGE_DOWN_IMAGE_WIDTH              11
#define IMAGE_ERROR_IMAGE_WIDTH             6



#define IMAGE_CHAR_DOT_WIDTH                    1
#define IMAGE_CHAR_V_WIDTH                  2
#define IMAGE_NUM_WIDTH                     2


//定义每条线路的站个数
u8 LineStationNumBuf[MAX_LINE_NUM] =
{
    LINE1_STATION_NUM, LINE2_STATION_NUM, LINE3_STATION_NUM,
};



u8 run_mode ; //全自动模式还是半自动模式
extern u8 com1_recv_length ;
extern u8 FIRST;


#if 0
const u8 char_dot_image[16][IMAGE_CHAR_DOT_WIDTH] =
{
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x38,
    0x38,
    0x38,
    0x00,
};

const u8 char_v_image[16][IMAGE_CHAR_V_WIDTH] =
{
    0x00, 0x00,
    0x38, 0x0E,
    0x1C, 0x1C,
    0x1C, 0x1C,
    0x1C, 0x1C,
    0x0E, 0x38,
    0x0E, 0x38,
    0x0E, 0x38,
    0x07, 0x70,
    0x07, 0x70,
    0x07, 0x70,
    0x03, 0xE0,
    0x03, 0xE0,
    0x03, 0xE0,
    0x01, 0xC0,
    0x00, 0x00,
};

const u8 num_image[10][16][IMAGE_NUM_WIDTH] =
{
    {
        {0x00, 0x00},
        {0x03, 0x80},
        {0x0F, 0xE0},
        {0x0E, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x0E, 0xE0},
        {0x0F, 0xE0},
        {0x03, 0x80},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x00, 0xC0},
        {0x01, 0xC0},
        {0x07, 0xC0},
        {0x0F, 0xC0},
        {0x0D, 0xC0},
        {0x09, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x07, 0xC0},
        {0x0F, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x00, 0x70},
        {0x00, 0x70},
        {0x00, 0xE0},
        {0x01, 0xE0},
        {0x03, 0xC0},
        {0x07, 0x80},
        {0x07, 0x00},
        {0x0E, 0x00},
        {0x1F, 0xF0},
        {0x1F, 0xF0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x07, 0xC0},
        {0x0F, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x00, 0x70},
        {0x01, 0xE0},
        {0x01, 0xE0},
        {0x00, 0x70},
        {0x00, 0x70},
        {0x00, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x0F, 0xE0},
        {0x07, 0xC0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x00, 0xE0},
        {0x01, 0xE0},
        {0x01, 0xE0},
        {0x03, 0xE0},
        {0x03, 0xE0},
        {0x06, 0xE0},
        {0x06, 0xE0},
        {0x0C, 0xE0},
        {0x18, 0xE0},
        {0x1F, 0xF8},
        {0x1F, 0xF8},
        {0x00, 0xE0},
        {0x00, 0xE0},
        {0x00, 0xE0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x07, 0xE0},
        {0x07, 0xE0},
        {0x0E, 0x00},
        {0x0E, 0x00},
        {0x0F, 0xC0},
        {0x1F, 0xE0},
        {0x1C, 0xF0},
        {0x00, 0x70},
        {0x00, 0x70},
        {0x00, 0x70},
        {0x1C, 0x70},
        {0x1E, 0xE0},
        {0x0F, 0xE0},
        {0x07, 0xC0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x03, 0xC0},
        {0x0F, 0xE0},
        {0x0E, 0x70},
        {0x1C, 0x00},
        {0x1D, 0xC0},
        {0x1F, 0xE0},
        {0x1E, 0xF0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x0E, 0xE0},
        {0x0F, 0xE0},
        {0x03, 0x80},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x0F, 0xF8},
        {0x0F, 0xF8},
        {0x00, 0x38},
        {0x00, 0x70},
        {0x00, 0xE0},
        {0x01, 0xC0},
        {0x01, 0xC0},
        {0x03, 0x80},
        {0x03, 0x80},
        {0x03, 0x80},
        {0x07, 0x00},
        {0x07, 0x00},
        {0x07, 0x00},
        {0x07, 0x00},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x07, 0xC0},
        {0x0F, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x0E, 0xE0},
        {0x07, 0xC0},
        {0x07, 0xC0},
        {0x0E, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x0F, 0xE0},
        {0x07, 0xC0},
        {0x00, 0x00},
    },
    {
        {0x00, 0x00},
        {0x03, 0x80},
        {0x0F, 0xE0},
        {0x0E, 0xE0},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1C, 0x70},
        {0x1E, 0xF0},
        {0x0F, 0xF0},
        {0x07, 0x70},
        {0x00, 0x70},
        {0x1C, 0xE0},
        {0x0F, 0xE0},
        {0x07, 0x80},
        {0x00, 0x00},
    }
};
#endif

const u8 down_image[16][IMAGE_DOWN_IMAGE_WIDTH] =
{

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0x78, 0xC7, 0x1B, 0x78, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0xFC, 0xC7, 0x1B, 0xFC, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x19, 0xCE, 0x67, 0x33, 0x8C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x19, 0x86, 0x6D, 0xB3, 0x0C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x19, 0x86, 0x6D, 0xB3, 0x0C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x19, 0x86, 0x6D, 0xB3, 0x0C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x31, 0xCE, 0x38, 0xE3, 0x0C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1F, 0xF0, 0xFC, 0x38, 0xE3, 0x0C, 0x0C, 0x0C, 0x0C},
    {0x00, 0x00, 0x1F, 0xC0, 0x78, 0x38, 0xE3, 0x0C, 0x0C, 0x0C, 0x0C},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    
};

const u8 error_image[16][IMAGE_ERROR_IMAGE_WIDTH] =
{
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C},
    {0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

#if IDU_COMPILE
const u8 no_process_image[16][IMAGE_NO_PROCESS_WIDTH] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0,
    0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80,
};

const u8 led_test_image[16][IMAGE_LED_TEST_WIDTH] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};


#endif

#if (SDU_COMPILE || FDU_COMPILE)
const u8 no_process_image[16][IMAGE_NO_PROCESS_WIDTH] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80,
};

const u8 led_test_image[16][IMAGE_NO_PROCESS_WIDTH] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};


#endif

#if 0
const u8 log_image[16][IMAGE_LOG_IMAGE_WIDTH] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x66, 0x01, 0x8C, 0x00, 0x61, 0x80, 0x06, 0x30, 0x03, 0x00, 0x30, 0x00, 0x06, 0x0C, 0x00, 0x30, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xFF, 0xC3, 0x60, 0xC0, 0x66, 0x00, 0xCC, 0x00, 0x61, 0x80, 0xC3, 0x60, 0x01, 0x80, 0x1B, 0xFE, 0x06, 0x0C, 0x00, 0x36, 0x00, 0x18, 0x07, 0xFF, 0xE0, 0x19, 0x80, 0x7F, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x81, 0x83, 0x6C, 0xC0, 0x66, 0x00, 0xCF, 0xFB, 0xFD, 0x80, 0x7F, 0xFC, 0x01, 0xC0, 0x18, 0x6C, 0x06, 0x0C, 0x00, 0x33, 0x07, 0xFF, 0xF6, 0xD8, 0x60, 0x1D, 0x80, 0x00, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0x03, 0x6C, 0xC0, 0x66, 0x60, 0x18, 0x00, 0xC1, 0x80, 0x61, 0x80, 0x7F, 0xFF, 0x00, 0x38, 0x3F, 0xFF, 0xC0, 0x30, 0x00, 0x30, 0x06, 0xDF, 0xE0, 0x19, 0x80, 0x00, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x67, 0x1F, 0xEC, 0xC0, 0x66, 0xF6, 0xFF, 0xE0, 0xF7, 0xF0, 0x03, 0x00, 0x0C, 0x30, 0x03, 0xFE, 0x06, 0x0C, 0x1F, 0xFF, 0xE0, 0x60, 0x07, 0x98, 0x60, 0x30, 0xC0, 0xFF, 0xEC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC3, 0x03, 0x6C, 0xC7, 0xE7, 0xC3, 0xDE, 0x61, 0xB1, 0xB0, 0x07, 0xF0, 0x0E, 0x1C, 0x03, 0x36, 0x07, 0x8C, 0x00, 0x30, 0x00, 0xFF, 0x87, 0x98, 0x60, 0x30, 0x60, 0x00, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x18, 0x03, 0x6C, 0xC0, 0x67, 0x03, 0xDB, 0x61, 0xB1, 0xB1, 0xE6, 0x30, 0x18, 0x0E, 0x7B, 0xFE, 0x07, 0x7F, 0x80, 0x78, 0x01, 0xC1, 0x86, 0xDF, 0xE0, 0x60, 0x70, 0x00, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x03, 0x6C, 0xC0, 0x66, 0x00, 0xD8, 0x63, 0xFD, 0xB0, 0x67, 0xF0, 0x30, 0x30, 0x1B, 0x36, 0x0E, 0x63, 0x00, 0xFC, 0x03, 0xC1, 0x86, 0x7E, 0x00, 0xCC, 0x3C, 0x7F, 0xCC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xFF, 0x83, 0x6C, 0xC0, 0x66, 0x01, 0xFF, 0xF8, 0x31, 0xB0, 0x66, 0x30, 0x0C, 0x38, 0x1B, 0x36, 0x3E, 0x33, 0x00, 0xF6, 0x06, 0xFF, 0x86, 0x7B, 0x61, 0x8E, 0x18, 0x60, 0xCC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x03, 0x6C, 0xC0, 0x66, 0x01, 0x9E, 0x60, 0x31, 0xB0, 0x67, 0xF0, 0x06, 0x70, 0x1B, 0xFE, 0x06, 0x36, 0x01, 0xB3, 0x0C, 0xC1, 0x87, 0xFB, 0xC0, 0x0C, 0x00, 0x60, 0xCC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x3C, 0x07, 0xEC, 0xC0, 0xE6, 0x07, 0x9B, 0x60, 0x7F, 0x30, 0x66, 0x30, 0x03, 0xE0, 0x1B, 0x36, 0x06, 0x1C, 0x03, 0x33, 0x80, 0xC1, 0x86, 0xD9, 0x80, 0x18, 0xC0, 0x7F, 0xCC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x7F, 0x1C, 0xCC, 0xC3, 0xE6, 0x31, 0x98, 0x63, 0xF3, 0x3C, 0x67, 0xF0, 0x01, 0xC0, 0x1B, 0x3E, 0x06, 0x1C, 0x06, 0x31, 0xE0, 0xFF, 0x86, 0x19, 0x80, 0x30, 0x60, 0x60, 0xCC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xDB, 0xC0, 0xCC, 0xCF, 0x66, 0x31, 0x9F, 0xF8, 0x33, 0x3C, 0x66, 0x30, 0x03, 0xE0, 0x1B, 0x3C, 0x06, 0x36, 0x1C, 0x30, 0xC0, 0xC1, 0x86, 0x18, 0xC0, 0x67, 0xF0, 0x60, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x19, 0x81, 0x80, 0xC6, 0x66, 0x31, 0x80, 0x60, 0x36, 0x3C, 0xF0, 0x00, 0x06, 0x38, 0x3C, 0x00, 0x06, 0x63, 0xC0, 0x30, 0x00, 0xCD, 0x86, 0x1E, 0x78, 0xFE, 0x70, 0x00, 0x3C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x03, 0x00, 0xC0, 0x63, 0xF1, 0x81, 0xE0, 0x36, 0x3D, 0x9F, 0xFE, 0x1C, 0x1E, 0x67, 0xFF, 0x9F, 0xC1, 0x80, 0x30, 0x00, 0xC7, 0x86, 0x1C, 0x30, 0x60, 0x60, 0x00, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xC0, 0x3C, 0x00, 0x00, 0x00, 0x70, 0x0C, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x06, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif


/*
const u8 *num_image_buf[16][IMAGE_NUM_WIDTH] = {
num0_image, num1_image, num2_image, num3_image, num4_image,
num5_image, num6_image, num7_image, num8_image, num9_image,
};
*/


const u16 row_mask_buf[] =
{
    0x0000, 0x0100, 0x0080, 0x0180,
    0x0040, 0x0140, 0x00c0, 0x01c0,
    0x0020, 0x0120, 0x00a0, 0x01a0,
    0x0060, 0x0160, 0x00e0, 0x01e0,
};
void LedSetRow(u8 index)
{
    u16 temp;
    temp = GPIO_ReadOutputData(GPIOG);
    temp &= 0xfe1f;
    temp |= row_mask_buf[index];
    GPIO_Write(GPIOG, temp);
#if 0
    
    if (index & 0x01) GPIO_SetBits(GPIOG, GPIO_Pin_8);
    else GPIO_ResetBits(GPIOG, GPIO_Pin_8);
    
    if (index & 0x02) GPIO_SetBits(GPIOG, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOG, GPIO_Pin_7);
    
    if (index & 0x04) GPIO_SetBits(GPIOG, GPIO_Pin_6);
    else GPIO_ResetBits(GPIOG, GPIO_Pin_6);
    
    if (index & 0x08) GPIO_SetBits(GPIOG, GPIO_Pin_5);
    else GPIO_ResetBits(GPIOG, GPIO_Pin_5);
    
#endif
}


void LedSetColour(tLedColor color)
{
#if IDU_COMPILE

    switch (color)
    {
        case LED_COLOR_RED:
            LED_DRV_R(1);
            LED_DRV_G(0);
            break;
            
        case LED_COLOR_GREEN:
            LED_DRV_R(0);
            LED_DRV_G(1);
            break;
            
        case LED_COLOR_RG:
            LED_DRV_R(1);
            LED_DRV_G(1);
            break;
            
        default:
            LED_DRV_R(1);
            LED_DRV_G(0);
            break;
    }
    
#else
    LED_DRV_R(1);
    LED_DRV_G(0);
#endif
    //gLedResetColor = color;
    gTrigerValue.disp_color = color;
}

void LoadData(const u8 *src_buf, u16 width, u16 column_index)
{
    u8 temp_l, temp_r;
    u16 byte_index;
    u8 bit_index;
    u16 temp;
    u16 i;
    byte_index = column_index / 8;
    bit_index = column_index % 8;
    
    for (i = 0; i < LED_PHYSICS_WIDTH; i++)
    {
        if ((byte_index + i) >= width) temp_l = 0;
        else temp_l = src_buf[byte_index + i];
        
        if ((byte_index + i + 1) >= width) temp_r = 0;
        else temp_r = src_buf[byte_index + i + 1];
        
        temp = temp_l;
        temp <<= 8;
        temp |= temp_r;
        temp <<= bit_index;
        temp >>= 8;
        SpiDmaBuf[i] = (u8)temp;
    }
}


const u8 bit_mask_buf[8] =
{
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
};
void LoadDataMid(const u8 *src_buf, u16 width, u16 column_index)
{
    u16 byte_index;
    u16 temp;
    u16 i;
    //  for (i=0; i<LED_PHYSICS_WIDTH; i++) SpiDmaBuf[i] = 0;
    byte_index = column_index / 8;
    
    /*
        for (i=0; i<byte_index; i++) {
            SpiDmaBuf[i] = 0;
        }
    */
    for (i = 0; i < width; i++)
    {
        SpiDmaBuf[byte_index++] = src_buf[i];
    }
    
#if 0
    
    for (i = 0; i < width; i++)
    {
        if (i == 0)
        {
            temp_l = src_buf[i];
            temp = temp_l;
        }
        else
        {
            temp_l = src_buf[i - 1];
            temp_r = src_buf[i];
            temp = temp_l;
            temp <<= 8;
            temp |= temp_r;
        }
        
        temp >>= bit_index;
        SpiDmaBuf[byte_index++] = (u8)temp;
    }
    
    if (width > 0)
    {
        SpiDmaBuf[byte_index - 1] &= bit_mask_buf[bit_index];
    }
    
    for (; byte_index < LED_PHYSICS_WIDTH; byte_index++)
    {
        SpiDmaBuf[byte_index] = 0;
    }
    
#endif
}



/*
void ShiftLeft(const u8 *src_buf, u16 width, u16 column_index)
{
    u8 temp_l, temp_r;
    u16 byte_index;
    u8 bit_index;
    u16 temp;
    u16 i;

    byte_index = column_index/8;
    bit_index = column_index%8;
    for (i=0; i<LED_PHYSICS_WIDTH; i++) {
        if ((byte_index+i) >= width) temp_l = 0;
        else temp_l = src_buf[byte_index+i];
        if ((byte_index+i+1) >= width) temp_r = 0;
        else temp_r = src_buf[byte_index+i+1];
        temp = temp_l;
        temp <<= 8;
        temp |= temp_r;
        temp <<= bit_index;
        temp >>= 8;
        SpiDmaBuf[i] = (u8)temp;
    }

}

void ShiftRight(const u8 *src_buf, u16 width, u16 column_index)
{
    u8 temp_l, temp_r;
    u16 byte_index;
    u8 bit_index;
    u16 temp;
    u16 i;

    byte_index = column_index/8;
    bit_index = column_index%8;
    for (i=0; i<LED_PHYSICS_WIDTH; i++) {
        if ((byte_index+i) >= width) temp_l = 0;
        else temp_l = src_buf[byte_index+i];
        if ((byte_index+i+1) >= width) temp_r = 0;
        else temp_r = src_buf[byte_index+i+1];
        temp = temp_l;
        temp <<= 8;
        temp |= temp_r;
        temp <<= bit_index;
        temp >>= 8;
        SpiDmaBuf[i] = (u8)temp;
    }

}

*/
/*
void DispTest(void)
{
    //LOG
    if (gLedTest.test_index == 0) {
        gLedTest.test_index = 1;
//      DispTypeInit(DISP_TYPE_TEST_LOG);
        DispTypeInit(DISP_TYPE_TEST_ALL_LIGHT);
    }
    // ALL LIGHT
    else if (gLedTest.test_index == 1) {
        gLedTest.test_index = 2;
        DispTypeInit(DISP_TYPE_TEST_ALL_LIGHT);
    }
    // ALL DARK
    else if (gLedTest.test_index >= 2) {
        gLedTest.test_index = 0;
        DispTypeInit(DISP_TYPE_DARK);
    }

    LED_DRV_R(1);
    LED_DRV_G(1);
}

*/
void ChangeDispLanguage(void)
{
    u32 i;
#ifdef DEBUG_ALL_LIGHT
    return;
#endif
    
    if (gDownFirmware == 1) return;
    /*if (gLedTest.test_flag == 1) {
        if (gSystemTick - gDispStat.switch_disp_tick > 300) {
            gDispStat.switch_disp_tick = gSystemTick;
            gLedTest.chg_flag = 1;
        }
    }*/
    else
    {
        if (gDispStat.disp_type != DISP_TYPE_NORMAL) return;
        
        if (gDispStat.disp_type == DISP_TYPE_DARK) return;
        
        if (gSystemTick - gDispStat.switch_disp_tick > STAY_TIME)
        {
            gDispStat.switch_disp_tick = gSystemTick;
#if     IDU_COMPILE
            gDispStat.change_lang_flag = 1;
            DispTypeInit(DISP_TYPE_DARK);
#else
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                gDispStat.change_lang_flag = 1;
                DispTypeInit(DISP_TYPE_DARK);
            
                //做延时处 100*1MS
                for (i = 0; i < 0xfffffff; i++);
            }
            
#endif
        }
    }
}

void LoadRowData(void)
{
    u8 *pt;
    u16 i, j;
    u16 width;
    u32 shift_speed;
    static u16 keep_time;
    
    if ((gDownFirmware == 1) && (gDispStat.disp_type != DISP_TYPE_ERROR_DOWN))gDispStat.disp_type = DISP_TYPE_DOWNING;
    
    if ((DISP_TYPE_LED_TEST == gDispStat.disp_type) || (DISP_TYPE_TEST_ALL_LIGHT == gDispStat.disp_type))
    {
        LED_DRV_R(1);
        LED_DRV_G(1);
        shift_speed = 10;
    }
    else
    {
        LedSetColour(gTrigerValue.disp_color);
        shift_speed = SHIFT_SPEED;
    }
    
    gDispStat.row_index ++;
    
    if (gDispStat.row_index >= 16)
    {
        gDispStat.row_index = 0;
        gDispStat.delay_cnt ++;
        
        if (gDispStat.delay_cnt >= shift_speed)
        {
            gDispStat.delay_cnt = 0;
            keep_time++;
            
            if (gDispStat.shift_type == SHIFT_LEFT)
            {
                gDispStat.column_index ++;
                
                if (gDispStat.column_index >= gDispStat.width)
                {
                    gDispStat.column_index = 0;
                    ChangeDispLanguage();
                }
            }
            else if (gDispStat.shift_type == SHIFT_RIGHT)
            {
                gDispStat.column_index --;
                
                if (gDispStat.column_index <= 1)
                {
                    gDispStat.column_index = gDispStat.width;
                    ChangeDispLanguage();
                }
            }
            else
            {
                if (KEEP_TIME < keep_time )
                {
                    keep_time = 0;
                    ChangeDispLanguage();
                }
            }
        }
    }
    
#if DEBUG_ALL_LIGHT_ON
    
    for (i = 0; i < LED_PHYSICS_WIDTH; i++) SpiDmaBuf[i] = 0xff;
    
    LED_DRV_R(1);
    LED_DRV_G(1);
    return;
#endif
#if 1
    
    for (i = 0; i < LED_PHYSICS_WIDTH; i++) SpiDmaBuf[i] = 0;
    
    if (gDispStat.disp_type == DISP_TYPE_DOWNING)
    {
        if (gSystemTick - gUpdateBmpSt.tick > 800)
        {
            //print_line("down time out");
            if (gDownFirmware == 1)
            {
                gDownFirmware = 0;
                DispTypeInit(DISP_TYPE_ERROR);
            }
            else
            {
                DispTypeInit(DISP_TYPE_NO_PROCESS);
            }
        }
        
        for (i = 0; i < IMAGE_DOWN_IMAGE_WIDTH; i++) SpiDmaBuf[i] = down_image[gDispStat.row_index][i];
    }
    else if (gDispStat.disp_type == DISP_TYPE_ERROR)
    {
        for (i = 0; i < IMAGE_ERROR_IMAGE_WIDTH; i++) SpiDmaBuf[i] = error_image[gDispStat.row_index][i];
    }
    else if (gDispStat.disp_type == DISP_TYPE_ERROR_DOWN)
    {
        for (i = 0; i < IMAGE_ERROR_IMAGE_WIDTH; i++) SpiDmaBuf[i] = error_image[gDispStat.row_index][i];
    }
    else if (gDispStat.disp_type == DISP_TYPE_NO_PROCESS)
    {
        gDispStat.shift_type = SHIFT_LEFT;
        LoadData(no_process_image[gDispStat.row_index], IMAGE_NO_PROCESS_WIDTH, gDispStat.column_index);
    }
    else if (gDispStat.disp_type == DISP_TYPE_LED_TEST)
    {
        gDispStat.shift_type = SHIFT_LEFT;
        LoadData(led_test_image[gDispStat.row_index], IMAGE_LED_TEST_WIDTH, gDispStat.column_index);
    }
    else if (gDispStat.disp_type == DISP_TYPE_NORMAL)
    {
        width = gDispStat.width;
        
        if (width % 8 == 0) width /= 8;
        else
        {
            width /= 8;
            width += 1;
        }
        
        if (gDispStat.shift_type == SHIFT_NONE)
        {
            j = (LED_PHYSICS_WIDTH * 8 - gDispStat.width) / 2;
            
            if (gDispStat.width % 8 != 0) width += 1;
            
            LoadDataMid(gLedImageBuf[gDispStat.row_index], width, j);
        }
        else
        {
            LoadData(gLedImageBuf[gDispStat.row_index], width, gDispStat.column_index);
        }
    }
    else if (gDispStat.disp_type == DISP_TYPE_VERSION)
    {
        //LoadData(gLedImageBuf[gDispStat.row_index], gDispStat.width, gDispStat.column_index);
    }
    else if (gDispStat.disp_type == DISP_TYPE_TEST_ALL_LIGHT)
    {
        for (i = 0; i < LED_PHYSICS_WIDTH; i++) SpiDmaBuf[i] = 0xff;
    }
    else if (gDispStat.disp_type == DISP_TYPE_TEST_LOG)
    {
        //      gDispStat.shift_type = SHIFT_LEFT;
        //      LoadData(log_image[gDispStat.row_index], IMAGE_LOG_IMAGE_WIDTH, gDispStat.column_index);
    }
    
#endif
}


void DispTypeInit(tDispType type)
{
    u8 i, j, index;
    
    switch (type)
    {
        case DISP_TYPE_DOWNING:
            gDispStat.shift_type = SHIFT_NONE;
            LedSetColour(LED_COLOR_GREEN);
            gDispStat.disp_type = DISP_TYPE_DOWNING;
            gDispStat.row_index = 16;
            break;
            
        case DISP_TYPE_ERROR:
            if (gDispStat.disp_type == DISP_TYPE_ERROR) break;
            
            if (gDispStat.disp_type == DISP_TYPE_VERSION) break;
            
            gDispStat.shift_type = SHIFT_NONE;
            LedSetColour(LED_COLOR_RED);
            gDispStat.disp_type = DISP_TYPE_ERROR;
            gDispStat.row_index = 16;
            break;
            
        case DISP_TYPE_ERROR_DOWN:
            if (gDispStat.disp_type == DISP_TYPE_ERROR) break;
            
            if (gDispStat.disp_type == DISP_TYPE_ERROR_DOWN) break;
            
            if (gDispStat.disp_type == DISP_TYPE_VERSION) break;
            
            gDispStat.shift_type = SHIFT_NONE;
            LedSetColour(LED_COLOR_RED);
            gDispStat.disp_type = DISP_TYPE_ERROR_DOWN;
            gDispStat.row_index = 16;
            break;
            
        case DISP_TYPE_NORMAL:
            if (gDispStat.disp_type == DISP_TYPE_ERROR) break;
            
            if (gDispStat.disp_type == DISP_TYPE_VERSION) break;
            
            gDispStat.switch_disp_tick = gSystemTick;
            LedSetColour(gLedColor);
            gDispStat.disp_type = DISP_TYPE_NORMAL;
            gDispStat.row_index = 16;
            
            if (gDispStat.shift_type == SHIFT_LEFT)
            {
                gDispStat.column_index = 0;
                gDispStat.width = gBmpWidth;
            }
            else if (gDispStat.shift_type == SHIFT_RIGHT)
            {
                gDispStat.column_index = gBmpWidth - SHIFT_LEFT_NULL_WIDTH;
                gDispStat.width = gBmpWidth - SHIFT_LEFT_NULL_WIDTH;
            }
            else
            {
                gDispStat.width = gBmpWidth;
            }
            
            break;
            
        case DISP_TYPE_NO_PROCESS:
            gOldTerminal = 0xff;
            
            if (gDispStat.disp_type == DISP_TYPE_NO_PROCESS) break;
            
            //if (gDispStat.disp_type == DISP_TYPE_ERROR) break;
            if (gDispStat.disp_type == DISP_TYPE_VERSION) break;
            
            //if (gDispStat.disp_type == DISP_TYPE_DARK) break;
            LedSetColour(LED_COLOR_RED);
            gDispStat.disp_type = DISP_TYPE_NO_PROCESS;
            gDispStat.column_index = 0;
            gDispStat.row_index = 16;
            gDispStat.width = IMAGE_NO_PROCESS_WIDTH * 8;
            break;
            
        case DISP_TYPE_DARK:
            if (gDispStat.disp_type == DISP_TYPE_DOWNING) break;
            
            gDispStat.shift_type = SHIFT_NONE;
            gDispStat.disp_type = DISP_TYPE_DARK;
            break;
            
        case DISP_TYPE_VERSION:
#if 0
            gDispStat.shift_type = SHIFT_NONE;
            LedSetColour(LED_COLOR_RED);
            gDispStat.disp_type = DISP_TYPE_VERSION;
            gDispStat.row_index = 16;
            index = 0;
            
            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < IMAGE_CHAR_V_WIDTH; j++)
                {
                    gLedImageBuf[i][index + j] = char_v_image[i][j];
                }
            }
            
            index += IMAGE_CHAR_V_WIDTH;
            
            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < IMAGE_NUM_WIDTH; j++)
                {
                    gLedImageBuf[i][index + j] = num_image[MSTER_VERSION][i][j];
                }
            }
            
            index += IMAGE_NUM_WIDTH;
            
            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < IMAGE_CHAR_DOT_WIDTH; j++)
                {
                    gLedImageBuf[i][index + j] = char_dot_image[i][j];
                }
            }
            
            index += IMAGE_CHAR_DOT_WIDTH;
            
            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < IMAGE_NUM_WIDTH; j++)
                {
                    gLedImageBuf[i][index + j] = num_image[SLAVE_VERSION][i][j];
                }
            }
            
            gDispStat.width = IMAGE_CHAR_DOT_WIDTH + IMAGE_CHAR_V_WIDTH + IMAGE_NUM_WIDTH * 2;
#endif
            break;
            
        case DISP_TYPE_TEST_ALL_LIGHT:
            gDispStat.shift_type = SHIFT_NONE;
            gDispStat.disp_type = DISP_TYPE_TEST_ALL_LIGHT;
            gDispStat.row_index = 16;
            break;
            
        case DISP_TYPE_TEST_LOG:
            gDispStat.shift_type = SHIFT_NONE;
            gDispStat.row_index = 16;
            gDispStat.column_index = 0;
            gDispStat.width = IMAGE_LOG_IMAGE_WIDTH * 8;
            gDispStat.disp_type = DISP_TYPE_TEST_LOG;
            break;
            
        case DISP_TYPE_LED_TEST:
            if (gDispStat.disp_type == DISP_TYPE_LED_TEST) break;
            
            LED_DRV_R(1);
            LED_DRV_G(1);
            gDispStat.column_index = 0;
            gDispStat.row_index = 16;
            gDispStat.width = IMAGE_LED_TEST_WIDTH * 8;
            gDispStat.disp_type = DISP_TYPE_LED_TEST;
            break;
    }
    
    gDispStat.delay_cnt = 0;
}


void PorcessDataOldSystem(u8 *buf);
void PorcessDataNewSystem(u8 *buf);
#if IDU_COMPILE


void ProcessDataOperat(u8 *buf)
{
    tFrameHeaderOld *frame_header;
    frame_header = (tFrameHeaderOld *)buf;
    
    if ((frame_header->receiverKeyword == 0xaa) && (com1_recv_length == 17))
    {
        PorcessDataNewSystem(buf);
    }
    else if (com1_recv_length > 17)
    {
        PorcessDataNormalSystem(buf);
    }
    else
    {
        PorcessDataOldSystem(buf);
    }
}

void PorcessDataOldSystem(u8 *buf)
{
    tFrameHeaderOld *frame_header;
    static u8 brd_flag = 0;
    static u8 arrive_flag = 0;
    static u8 close_door_flag = 0;
    static u8 eme_flag = 0;
    u16 temp;
    frame_header = (tFrameHeaderOld *)buf;
    
    if (frame_header->doorSignal == 0x01)
    {
        if (arrive_flag == 0)
        {
            arrive_flag = 1;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.triger_type = TRIGER_DOOR_OPEN;
            gDoorStat.mDoorSideStat = DOOR_SIDE_LIGHT;
        }
    }
    else
    {
        arrive_flag = 0;
    }
    
    if (frame_header->doorSignal == 0x02)
    {
        if (close_door_flag == 0)
        {
            close_door_flag = 1;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.triger_type = TRIGER_DOOR_CLOSE;
            gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
        }
    }
    else
    {
        close_door_flag = 0;
    }
    
    if (frame_header->announceActivation == 0x01 )
    {
        if (brd_flag == 0)
        {
            brd_flag = 1;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.triger_type = TRIGER_PRE_BROADCAST;
            gDoorStat.mDoorSideStat = DOOR_SIDE_FLASH;
        }
    }
    else
    {
        brd_flag = 0;
    }
    
    if ((frame_header->lineId & 0xf0) > 0)
    {
        gTrigerValue.station_start = 1 ;
        //gTrigerValue.station_end = frame_header->frameNo;  //老车实际数据是00
        gTrigerValue.station_end = 30 ;
    }
    else
    {
        gTrigerValue.station_start = 30;
        gTrigerValue.station_end = 1;
    }
    
    temp = frame_header->stationH;
    temp <<= 8;
    temp += frame_header->stationL;
    
    if (temp != gTrigerValue.station_next)
        gTrigerValue.triger_flag = 1;
        
    gTrigerValue.station_current = temp;
    gTrigerValue.station_next = temp;
    
    if (frame_header->tmsDoor == 0x01) gTrigerValue.door = DOOR_SIDE_LEFT;
    else if (frame_header->tmsDoor == 0x02) gTrigerValue.door = DOOR_SIDE_RIGHT;
    else if ((frame_header->tmsDoor == 0x03) || (frame_header->tmsDoor == 0x04)) gTrigerValue.door = DOOR_SIDE_BOTH;
    else gTrigerValue.door = DOOR_SIDE_NONE;
    
    /*******************************************
    紧急广播部分
    
    
    #if EME_FUCTION
        if (frame_header->data_.trigger_hbit.urgent_broadcast) {
    
            if ((eme_flag == 0) || (gTrigerValue.eme_index != frame_header->data_.urgent_broadcast_no)) {
                eme_flag = 1;
                gTrigerValue.eme_flag = 1;
                if (gSysConfg.disp_type == DISP_LANG_BOTH) {
                    gDispStat.curr_lang = LANG_ENGLISH ;
                }
                else if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY) {
                    gDispStat.curr_lang = LANG_ENGLISH;
                }
                else {
                    gDispStat.curr_lang = LANG_CHINESE;
                }
                gDispStat.change_lang_flag = 1;
                gTrigerValue.eme_index = frame_header->data_.urgent_broadcast_no;
            }
            return;
        }
        else {
            if (eme_flag == 1) gTrigerValue.triger_flag = 1;
            eme_flag = 0;
            gTrigerValue.eme_flag = 0;
        }
    #endif
    ********************************************/
    if (gTrigerValue.door == DOOR_SIDE_NONE)
    {
    }
    else if (gTrigerValue.door == DOOR_SIDE_LEFT)
    {
        if (gDeviceId % 2 == 0) gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    else if (gTrigerValue.door == DOOR_SIDE_RIGHT)
    {
        if (gDeviceId % 2 == 1)gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
}


void PorcessDataNewSystem(u8 *buf)
{
    tFrameHeaderNew *frame_header;
    tDoorSide door_side;
    u8 sendFlag = 0;
    static u8 eme_flag = 0;
    static u8 leaveFlag = 0;
    static u8 stopFlag = 0;
    static u8 closeDoorFlag = 0;
    static u8 preDoorFlag = 0;
    static u8 arrived_broadcast = 0;
    frame_header = (tFrameHeaderNew *)buf;
    
    if ( (frame_header->start_station != gTrigerValue.station_start) || \
         (frame_header->end_station != gTrigerValue.station_end)   || \
         frame_header->current_station != gTrigerValue.station_current)
    {
        gTrigerValue.station_start = frame_header->start_station;
        gTrigerValue.station_end = frame_header->end_station;
        gTrigerValue.station_current = frame_header->current_station;
        sendFlag = 1;
    }
    
    gTrigerValue.station_start = frame_header->start_station;
    gTrigerValue.station_end = frame_header->end_station;
    //gTrigerValue.station_next = frame_header->current_station;
    gTrigerValue.station_current = frame_header->current_station;
    
    if (frame_header->start_station < frame_header->end_station) //上行
    {
        gTrigerValue.station_next = frame_header->current_station + 1;
        
        if (gTrigerValue.station_next > frame_header->end_station)
            gTrigerValue.station_next = frame_header->end_station;
            
        if (gTrigerValue.station_next < frame_header->start_station)
            gTrigerValue.station_next = frame_header->start_station + 1;
    }
    
    if (frame_header->start_station > frame_header->end_station) //下行
    {
        gTrigerValue.station_next = frame_header->current_station - 1;
        
        if (gTrigerValue.station_next < frame_header->end_station)
            gTrigerValue.station_next = frame_header->end_station;
            
        if (gTrigerValue.station_next > frame_header->start_station)
            gTrigerValue.station_next = frame_header->start_station - 1;
    }
    
    /*******************************************
    紧急广播部分
    ********************************************/
#if EME_FUCTION
    
    if (frame_header->trigger_hbit.urgent_broadcast)
    {
        if ((eme_flag == 0) || (gTrigerValue.eme_index != frame_header->urgent_broadcast_no))
        {
            eme_flag = 1;
            gTrigerValue.eme_flag = 1;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                gDispStat.curr_lang = LANG_ENGLISH ;
            }
            else if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY)
            {
                gDispStat.curr_lang = LANG_ENGLISH;
            }
            else
            {
                gDispStat.curr_lang = LANG_CHINESE;
            }
            
            gDispStat.change_lang_flag = 1;
            gTrigerValue.eme_index = frame_header->urgent_broadcast_no;
        }
        
        return;
    }
    else
    {
        if (eme_flag == 1) gTrigerValue.triger_flag = 1;
        
        eme_flag = 0;
        gTrigerValue.eme_flag = 0;
    }
    
#endif
    
    if ( (1 == frame_header->status_hbit.door_right_sides) && (0 == frame_header->status_hbit.door_left_sides) )
    {
        door_side = DOOR_SIDE_RIGHT;
    }
    else if ( (0 == frame_header->status_hbit.door_right_sides) && (1 == frame_header->status_hbit.door_left_sides) )
    {
        door_side = DOOR_SIDE_LEFT;
    }
    else if ( (1 == frame_header->status_hbit.door_right_sides) && (1 == frame_header->status_hbit.door_left_sides) )
    {
        door_side = DOOR_SIDE_BOTH;
    }
    else if ( (0 == frame_header->status_hbit.door_right_sides) && (0 == frame_header->status_hbit.door_left_sides) )
    {
        door_side = DOOR_SIDE_NONE;
    }
    
    if ( DOOR_SIDE_NONE != door_side )
    {
        if ( 0 == preDoorFlag )
        {
            sendFlag = 1;
            preDoorFlag = 1;
            //if ((gDeviceId %2 == 0) && (door_side == DOOR_SIDE_LEFT))  //suven
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_FLASH;
            }
        }
    }
    else
    {
        preDoorFlag = 0;
    }
    
    if ( (frame_header->trigger_hbit.leave_broadcast))  //离站广播
    {
        if ( 0 == leaveFlag )
        {
            sendFlag = 1;
            leaveFlag = 1;
        }
    }
    else
    {
        leaveFlag = 0;
    }
    
    if ( frame_header ->trigger_hbit.arrived_broadcast
         && arrived_broadcast != frame_header ->trigger_hbit.arrived_broadcast) //到站广播
    {
        preDoorFlag = 0;  //清除门状态，防止开门侧一直给的情况下无法显示闪烁
    }
    
    arrived_broadcast = frame_header ->trigger_hbit.arrived_broadcast;
    
    if ( (frame_header->trigger_hbit.reach_stop)) //到站停稳
    {
        if ( 0 == stopFlag  )
        {
            sendFlag = 1;
            stopFlag  = 1;
            gDoorStat.mDoorSideStat = DOOR_SIDE_LIGHT;
        }
    }
    else
    {
        stopFlag  = 0;
    }
    
    if ( (frame_header->trigger_hbit.closedoor))  //离站信号
    {
        if ( 0 == closeDoorFlag )
        {
            sendFlag = 1;
            closeDoorFlag = 1;
            gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
        }
    }
    else
    {
        closeDoorFlag = 0;
    }
    
    /*
    if ( (frame_header->trigger_hbit.leave))  //离站信号
    {
        if ( 0==closeDoorFlag )
        {
            sendFlag = 1;
            closeDoorFlag = 1;
            gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
        }
    }
    else
    {
        closeDoorFlag = 0;
    }
    */
    if ((gDeviceId % 2 == 0) && (door_side == DOOR_SIDE_LEFT))
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    else if ((gDeviceId % 2 == 1) && (door_side == DOOR_SIDE_RIGHT))
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    else if (door_side == DOOR_SIDE_NONE)
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    
    if ( sendFlag == 1 )
    {
        sendFlag = 0;
        
        if (frame_header->trigger_hbit.leave_broadcast) gTrigerValue.triger_type = TRIGER_PRE_BROADCAST;
        else if (frame_header->trigger_hbit.reach_stop) gTrigerValue.triger_type = TRIGER_DOOR_OPEN;
        else if (frame_header->trigger_hbit.closedoor) gTrigerValue.triger_type = TRIGER_DOOR_CLOSE;
        
        gTrigerValue.triger_flag = 1;
    }
}


void PorcessDataNormalSystem(u8 *buf)
{
    tFrameHeader *frame_header;
    u8 line = 0;
    static u8 eme_flag = 0;
    static u8 welcome_flag = 0;
    static u8 leave_flag;
    static u8 arrive_flag;
    tDoorSide door_side;
    //  static u8 close_door_flag = 0;
    static u8 leaveFlag = 0;
    static u8 stopFlag = 0;
    //  static u8 closeDoorFlag = 0;
    static u8 preDoorFlag = 0;
    static u8 arrived_broadcast = 0;
    static u8 Old_Mode = 0;
    static u8 PRE_BROADCAST_FLAG = 0;
    static u8 DOOR_OPEN_FLAG = 0;
    static u8 DOOR_CLOSE_FLAG = 0;
    static u8 ARRIVE_BROADCAST_FLAG = 0;
    frame_header = (tFrameHeader *)buf;
    
    /*******************************************
    对数据进行容错
    *******************************************/
    if ((frame_header->data_.next_station ==  frame_header->data_.start_station) &&
        (frame_header->data_.next_station ==   frame_header->data_.end_station))
        return;
        
    if ((frame_header->data_.start_station  >   frame_header->data_.end_station) &&
        ((frame_header->data_.next_station > frame_header->data_.start_station) ||
         (frame_header->data_.next_station < frame_header->data_.end_station)))
        return;
        
    if ((frame_header->data_.start_station  <   frame_header->data_.end_station) &&
        ((frame_header->data_.next_station < frame_header->data_.start_station) ||
         (frame_header->data_.next_station > frame_header->data_.end_station)))
        return;
        
    /*******************************************
    开门侧判断
    ********************************************/
    if ((frame_header->data_.status_hbit.door_right_sides) && (frame_header->data_.status_hbit.door_left_sides)) door_side = DOOR_SIDE_BOTH;
    else if (frame_header->data_.status_hbit.door_right_sides) door_side = DOOR_SIDE_RIGHT;
    else if (frame_header->data_.status_hbit.door_left_sides) door_side = DOOR_SIDE_LEFT;
    else door_side = DOOR_SIDE_NONE;
    
    /*******************************************
    线号设置部分
    ********************************************/
#if LINE_CHANGE
    
    if (frame_header->data_.line_no == 0)
        frame_header->data_.line_no = 1;
        
    line = 1;
    else
        line = frame_header->data_.line_no;
        
#else
    frame_header->data_.line_no = 1;
    line = 1;
#endif
    /*******************************************
    紧急广播部分
    ********************************************/
#if EME_FUCTION
    
    if (frame_header->data_.trigger_hbit.urgent_broadcast)
    {
        if ((eme_flag == 0) || (gTrigerValue.eme_index != frame_header->data_.urgent_broadcast_no))
        {
            eme_flag = 1;
            gTrigerValue.eme_flag = 1;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                gDispStat.curr_lang = LANG_ENGLISH ;
            }
            else if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY)
            {
                gDispStat.curr_lang = LANG_ENGLISH;
            }
            else
            {
                gDispStat.curr_lang = LANG_CHINESE;
            }
            
            gDispStat.change_lang_flag = 1;
            gTrigerValue.eme_index = frame_header->data_.urgent_broadcast_no;
        }
        
        return;
    }
    else
    {
        if (eme_flag == 1) gTrigerValue.triger_flag = 1;
        
        eme_flag = 0;
        gTrigerValue.eme_flag = 0;
    }
    
#endif
    
    /***********************************************
    welcome 欢迎词的显示
    *****************************************************/
    if (frame_header->data_.setup_data_type.display_welcome)
    {
        if (welcome_flag == 0)
        {
            welcome_flag = 1;
            gTrigerValue.welcome_flag = 1;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                gDispStat.curr_lang = LANG_ENGLISH ;
            }
            else if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY)
            {
                gDispStat.curr_lang = LANG_ENGLISH;
            }
            else
            {
                gDispStat.curr_lang = LANG_CHINESE;
            }
            
            gDispStat.change_lang_flag = 1;
        }
        
        return;
    }
    else
    {
        if (welcome_flag == 1) gTrigerValue.triger_flag = 1;
        
        welcome_flag = 0;
        gTrigerValue.welcome_flag = 0;
    }
    
    /*******************************************
    门侧灯显示部分
    到站触发的时候门侧灯闪
    预到站的时候门侧灯灭(注释掉的部分是在预到站的时候没改开门侧信息，所有预到站触发门侧灯不亮)
    ********************************************/
#if LIGHT_SIDE
    //到站的时候开门侧亮，关门侧暗//////////////到站触发闪亮//////
    //这样做的好处是:中央控制器什么时候给开门侧信息，什么时候就显示，广4是到站触发的时候才给开门侧信息
    
    if ( DOOR_SIDE_NONE != door_side )
    {
        if (frame_header->data_.trigger_hbit.leave_broadcast)
            gDoorStat.mDoorSideStat = DOOR_SIDE_FLASH;
        else if (frame_header->data_.trigger_hbit.reach_stop)
            gDoorStat.mDoorSideStat = DOOR_SIDE_LIGHT;
    }
    else
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    
    if ((gDeviceId % 2 == 0) && (door_side == DOOR_SIDE_LEFT)) // 单左双右
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    else if ((gDeviceId % 2 == 1) && (door_side == DOOR_SIDE_RIGHT))
    {
        gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
    }
    
#endif
    /*
    #if LIGHT_SIDE
        if ( DOOR_SIDE_NONE != door_side )
        {
            //if ( (frame_header->data_.trigger_hbit.leave_broadcast) ||(frame_header->data_.trigger_hbit.arrived_broadcast ) )
    
            if ((frame_header->data_.trigger_hbit.arrived_broadcast ) )
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_FLASH;
            }
            else if(frame_header->data_.trigger_hbit.leave_broadcast)
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
            }
            else if((frame_header->data_.trigger_hbit.reach_stop)||(frame_header->data_.trigger_hbit.opendoor))
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_LIGHT;
            }
    
            if ((gDeviceId %2 == 0) && (door_side == DOOR_SIDE_LEFT))
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
            }
            else if ((gDeviceId %2 == 1) && (door_side == DOOR_SIDE_RIGHT))
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
            }
            else if(door_side == DOOR_SIDE_NONE)
            {
                gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
            }
        }
        else if((frame_header->data_.trigger_hbit.closedoor )||( DOOR_SIDE_NONE == door_side))
        {
    
            gDoorStat.mDoorSideStat = DOOR_SIDE_DARK;
        }
    #endif
    */
    /*******************************************
    模式设置部分
    ********************************************/
    run_mode = MAUAL;      //一般默认为半自动模式
#if MODE_CHANGE
    
    if (frame_header->data_.mode_run.mode == 1)
        run_mode = AUTO;
        
#endif
    gDispStat.scr_sav_tick = gSystemTick;
    
    if (MAUAL == run_mode) //半自动模式
    {
        if (( frame_header->data_.trigger_hbit.leave_broadcast) || (frame_header->data_.trigger_hbit.reach_stop) || (frame_header->data_.trigger_hbit.arrived_broadcast) || (frame_header->data_.trigger_hbit.closedoor))
        {}
        else
            return;
    }
    else if (AUTO == run_mode)
    {
        if (( frame_header->data_.trigger_hbit.leave_broadcast) || (frame_header->data_.trigger_hbit.reach_stop))
            //if(( frame_header->data_.trigger_hbit.arrived_broadcast)||(frame_header->data_.trigger_hbit.opendoor))
        {}
        else
            return;
    }
    
    /*******************************************
    帧数据类型判断部分
    ********************************************/
#if PRE_BROADCAST
    
    if ((frame_header->data_.trigger_hbit.leave_broadcast) )   //预到站显示
    {
        if ( (Old_Mode != run_mode) || (gTrigerValue.station_next != frame_header->data_.next_station) || (gTrigerValue.station_end != frame_header->data_.end_station) || (gTrigerValue.line_no != line) )
            PRE_BROADCAST_FLAG = 0;
            
        if (PRE_BROADCAST_FLAG == 0)
        {
            Old_Mode = run_mode;
            PRE_BROADCAST_FLAG = 1;
            gTrigerValue.line_no = line;
            gTrigerValue.station_next = frame_header->data_.next_station;
            gTrigerValue.station_start = frame_header->data_.start_station;
            gTrigerValue.station_end = frame_header->data_.end_station;
            
            if (MAUAL == run_mode)
                gTrigerValue.triger_type = TRIGER_PRE_BROADCAST;
            else
                gTrigerValue.triger_type = TRIGER_PRE_BROADCAST_1;
                
            gTrigerValue.triger_flag = 1;
            gTrigerValue.door = door_side;
            DispTypeInit(DISP_TYPE_DARK);
        }
    }
    else
        PRE_BROADCAST_FLAG = 0;
        
#endif
#if DOOR_OPEN
        
    if ((frame_header->data_.trigger_hbit.reach_stop))            //到站停止触发      显示     本站:XX
    {
        if ((gTrigerValue.station_next != frame_header->data_.next_station) || (gTrigerValue.station_end != frame_header->data_.end_station) || (gTrigerValue.line_no != line) )
            DOOR_OPEN_FLAG = 0;
            
        if (DOOR_OPEN_FLAG == 0)
        {
            DOOR_OPEN_FLAG = 1;
            gTrigerValue.line_no = line;
            gTrigerValue.station_next = frame_header->data_.next_station;
            gTrigerValue.station_start = frame_header->data_.start_station;
            gTrigerValue.station_end = frame_header->data_.end_station;
            gTrigerValue.triger_type = TRIGER_DOOR_OPEN;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.door = door_side;
            DispTypeInit(DISP_TYPE_DARK);
        }
    }
    else
        DOOR_OPEN_FLAG = 0;
        
#endif
#if DOOR_CLOSE
        
    //if ((frame_header->data_.trigger_hbit.closedoor))  {      //关门触发              终点站:XX
    if ( (frame_header->data_.status_hbit.door_right_sides == 0) && (frame_header->data_.status_hbit.door_left_sides == 0))         //关门触发              终点站:XX
    {
        if ((gTrigerValue.station_next != frame_header->data_.next_station) || (gTrigerValue.station_end != frame_header->data_.end_station) || (gTrigerValue.line_no != line) )
            DOOR_CLOSE_FLAG = 0;
            
        if (( frame_header->data_.next_station == frame_header->data_.end_station))
            return;
            
        if (DOOR_CLOSE_FLAG == 0)
        {
            DOOR_CLOSE_FLAG = 1;
            gTrigerValue.line_no = line;
            gTrigerValue.station_next = frame_header->data_.next_station;
            gTrigerValue.station_start = frame_header->data_.start_station;
            //gTrigerValue.station_current= frame_header->data_.current_station;
            gTrigerValue.station_end = frame_header->data_.end_station;
            gTrigerValue.triger_type = TRIGER_DOOR_CLOSE;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.door = DOOR_SIDE_NONE;
            DispTypeInit(DISP_TYPE_DARK);
        }
    }
    else
        DOOR_CLOSE_FLAG = 0;
        
#endif
#if ARRIVE_BROADCAST
        
    if ((frame_header->data_.trigger_hbit.arrived_broadcast))
    {
        if ((gTrigerValue.station_next != frame_header->data_.next_station) || (gTrigerValue.station_end != frame_header->data_.end_station) || (gTrigerValue.line_no != line) )
            ARRIVE_BROADCAST_FLAG = 0 ;
            
        if (ARRIVE_BROADCAST_FLAG == 0)
        {
            ARRIVE_BROADCAST_FLAG = 1;
            gTrigerValue.line_no = line;
            gTrigerValue.station_next = frame_header->data_.next_station;
            gTrigerValue.station_start = frame_header->data_.start_station;
            //gTrigerValue.station_current= frame_header->data_.current_station;
            gTrigerValue.station_end = frame_header->data_.end_station;
            gTrigerValue.triger_type = TRIGER_ARRIVE_BROADCAST;
            gTrigerValue.triger_flag = 1;
            gTrigerValue.door = door_side;
            DispTypeInit(DISP_TYPE_DARK);
        }
    }
    else
        ARRIVE_BROADCAST_FLAG = 0 ;
        
#endif
}


#else   //fdu   sdu
void PorcessDataNewSystem(u8 *buf)
{
    tFrameHeader *frame_header;
    tLanguage lang;
    static u8 old_line = 0xff;
    frame_header = (tFrameHeader *)buf;
    gDispStat.scr_sav_tick = gSystemTick;

    if ((frame_header->data_.line_no == 0) || (frame_header->data_.line_no > MAX_LINE_NUM)) frame_header->data_.line_no = 1;

    if ((gOldTerminal != frame_header->data_.end_station) || (old_line != frame_header->data_.line_no) || (gLedTestFlag == 1))
    {
        gLedTestFlag = 0;

        if (frame_header->data_.end_station > LineStationNumBuf[frame_header->data_.line_no - 1])
        {
            //gOldTerminal = LineStationNumBuf[frame_header->data_.line_no-1];
            //old_line = frame_header->data_.line_no;
            DispTypeInit(DISP_TYPE_NO_PROCESS);
            return;
        }
        else
        {
            gOldTerminal = frame_header->data_.end_station;
            old_line = frame_header->data_.line_no;
        }

        if (DISP_LANG_BOTH == gSysConfg.disp_type)
        {
            gDispStat.curr_lang = LANG_CHINESE;
        }
        else if (DISP_LANG_ENGLISH_ONLY == gSysConfg.disp_type)
        {
            gDispStat.curr_lang = LANG_CHINESE;
        }
        else
        {
            gDispStat.curr_lang = LANG_FARSI;
        }

        GetBmp(old_line, gOldTerminal - 1, gDispStat.curr_lang);
    }
}

#endif

/*********************
0xFE 后面接的是广告语

*********************/
void GetAdBuf(u8 *dest, u8 *src, u8 *num)
{
    u8 i, j;
    i = 0;
    
    while (src[i] != 0xfe)
    {
        i ++;
        
        if (i > (CONFIG_MSG_LEN + 4))
        {
            print_line("Get ad buf error");
            DispTypeInit(DISP_TYPE_ERROR);
            break;
        }
    }
    
    i += 1;
    j = 0;
    
    while (src[i] != 0xff)
    {
        dest[j] = src[i];
        j ++;
        i ++;
        
        if (j > CONFIG_AD_LEN)
        {
            print_line("Get ad buf error 12");
            DispTypeInit(DISP_TYPE_ERROR);
            break;
        }
    }
    
    *num = j;
}





void do_get_station_config_pre(void)
{
    GetStationConfig(gTrigerValue.english_config, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_ENGLISH, CONFIG_TYPE_PRE);
    GetStationConfig(gTrigerValue.chinese_config, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_CHINESE, CONFIG_TYPE_PRE);
    GetStationConfig(&gTrigerValue.prev_english_color, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_ENGLISH, CONFIG_TYPE_PRE_COLOR);
    GetStationConfig(&gTrigerValue.prev_chinese_color, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_CHINESE, CONFIG_TYPE_PRE_COLOR);
    gTrigerValue.ad_english_index = 0;
    gTrigerValue.ad_chinese_index = 0;
    GetAdBuf(gTrigerValue.ad_english_buf, gTrigerValue.english_config, &gTrigerValue.ad_english_num);
    GetAdBuf(gTrigerValue.ad_chinese_buf, gTrigerValue.chinese_config, &gTrigerValue.ad_chinese_num);
}


void do_get_station_config_arrive(void)
{
    GetStationConfig(gTrigerValue.english_config, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_ENGLISH, CONFIG_TYPE_ARRIVE);
    GetStationConfig(gTrigerValue.chinese_config, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_CHINESE, CONFIG_TYPE_ARRIVE);
    GetStationConfig(&gTrigerValue.arrive_english_color, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_ENGLISH, CONFIG_TYPE_ARRIVE_COLOR);
    GetStationConfig(&gTrigerValue.arrive_chinese_color, gTrigerValue.line_no - 1 + CONFIG_LINE1, gTrigerValue.station_next - 1, LANG_CHINESE, CONFIG_TYPE_ARRIVE_COLOR);
    gTrigerValue.ad_english_index = 0;
    gTrigerValue.ad_chinese_index = 0;
    GetAdBuf(gTrigerValue.ad_english_buf, gTrigerValue.english_config, &gTrigerValue.ad_english_num);
    GetAdBuf(gTrigerValue.ad_chinese_buf, gTrigerValue.chinese_config, &gTrigerValue.ad_chinese_num);
}


void DispFun_DS(void)  //选用广3、4 PTU
{
    u8 temp;
    tTrigerType type;
    /*
        if(gLedTest.test_flag ==1)  //test suven
            return;     //end test suven
    
        if (gLedTest.chg_flag == 1) {
            gLedTest.chg_flag = 0;
            DispTest();
            return;
        }
    */
    
    if (gDownFirmware == 1) return;
    
    if (gUpdateBmpSt.success_flag == BMP_FAIL)
    {
        DispTypeInit(DISP_TYPE_ERROR);
    }
    
#if IDU_COMPILE
#if   EME_FUCTION
    
    if (gTrigerValue.eme_flag == 1)
    {
        if (gDispStat.change_lang_flag == 1)
        {
            gDispStat.change_lang_flag = 0;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
                else gDispStat.curr_lang = LANG_ENGLISH;
            }
            
            GetBmp(BMP_TYPE_EME, gTrigerValue.eme_index - 1, gDispStat.curr_lang);
        }
        
        return;
    }
    
#endif
    
    if (gTrigerValue.triger_flag == 1)
    {
        gTrigerValue.triger_flag = 0;
        
        if (1 == GetStationConfig1(gTrigerValue.config_buf, gTrigerValue.station_next - 1, gTrigerValue.line_no))
        {
            DispTypeInit(DISP_TYPE_ERROR);
            return;
        }
        
        //同样，在配置信息的开头几个字节表示颜色
        if (gTrigerValue.triger_type == TRIGER_PRE_BROADCAST) temp = gTrigerValue.config_buf[2];
        else if (gTrigerValue.triger_type == TRIGER_DOOR_OPEN) temp = gTrigerValue.config_buf[0];
        else if (gTrigerValue.triger_type == TRIGER_DOOR_CLOSE) temp = gTrigerValue.config_buf[1];
        
        //显示颜色
        if ( temp == 1) gTrigerValue.disp_color = LED_COLOR_RED;
        else if ( temp == 2) gTrigerValue.disp_color = LED_COLOR_GREEN;
        else if ( temp == 3) gTrigerValue.disp_color = LED_COLOR_RG;
        else
        {
            DispTypeInit(DISP_TYPE_ERROR);
            return;
        }
        
        type = gTrigerValue.triger_type;
        
        if ((gTrigerValue.station_next == gTrigerValue.station_end) && (type == TRIGER_PRE_BROADCAST))
        {
            type = TRIGER_NEXT_TERMINAL;
        }
        
        if ((gTrigerValue.station_start == gTrigerValue.station_current) && (type == TRIGER_DOOR_CLOSE))
        {
            type = TRIGER_START_STATION;
        }
        
        if (1 == GetConfigOfType(gTrigerValue.config_buf, type))
        {
            DispTypeInit(DISP_TYPE_ERROR);
            return;
        }
        
        //可以再这里分别提取出中英文、广告配置，可以设置分开显示，只要在配置信息里面做个分隔标识符，不加的话是连续显示
        LoadBmpAndWriteToImageBuf(gTrigerValue.line_no, gTrigerValue.config_buf, gTrigerValue.station_current - 1, gTrigerValue.station_next - 1, gTrigerValue.station_end - 1);
    }
    
#else
    
    if (gDispStat.change_lang_flag == 1)
    {
        gDispStat.change_lang_flag = 0;
    
        if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
        else gDispStat.curr_lang = LANG_ENGLISH;
    
        GetBmp(gDispStat.curr_line, gDispStat.curr_station, gDispStat.curr_lang);
    }
    
#endif
}
/**************
为函数LoadConnect  特制的函数
***************/
void BufConsistToOne(u16 *page_buf, u16 *buf)
{
    u8 i = 0;
    u8 j = 0;
    
    while (page_buf[i] != 0xffff)
        i++;
        
    while (buf[j] != 0xffff)
    {
        page_buf[i]  = buf[j];
        i++, j++;
    }
    
    page_buf[i] = 0xffff;
}


void DispFun_NDS(void)  //伊朗PTU
{
    tLanguage lang;
    //u16 buf[10];
    tBmpParam param;
    u16 page_buf[30] = {0xffff};
    u16 buf[15] = {0xffff};
    
    if (gDownFirmware == 1) return;
    
    if (gUpdateBmpSt.success_flag == BMP_FAIL)
    {
        DispTypeInit(DISP_TYPE_ERROR);
    }
    
    if (FIRST == 1) return;
    
#if IDU_COMPILE
#if   EME_FUCTION
    
    if (gTrigerValue.eme_flag == 1)
    {
        if (gDispStat.change_lang_flag == 1)
        {
            gDispStat.change_lang_flag = 0;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
                else gDispStat.curr_lang = LANG_ENGLISH;
            }
            
            GetBmp(BMP_TYPE_EME, gTrigerValue.eme_index - 1, gDispStat.curr_lang);
        }
        
        return;
    }
    
#endif
    
    /********************************************
    welcome欢迎词
    ******************************************/
    if (gTrigerValue.welcome_flag == 1)
    {
        if (gDispStat.change_lang_flag == 1)
        {
            gDispStat.change_lang_flag = 0;
            
            if (gSysConfg.disp_type == DISP_LANG_BOTH)
            {
                if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
                else gDispStat.curr_lang = LANG_ENGLISH;
            }
            
            GetBmp(BMP_TYPE_MSG, 8, gDispStat.curr_lang);
        }
        
        return;
    }
    
    /**************************************************
    如果要2中语言分开来独立显示
    ==> DISPLAY_KIND = 1，(当图片没屏幕宽时静止显示+ 不同语言可以弄成不同颜色)
    
    如果要2中语言合成一张图片来显示
    ==> DISPLAY_KIND = 0，(当总图片没屏幕宽时静止显示+ 语言不可以弄成不同颜色)
    *************************************************/
#if DISPLAY_KIND
    
    if (gTrigerValue.triger_flag == 1)
    {
        gTrigerValue.triger_flag = 0;
        gTrigerValue.station_msg_disp_time = 2;
        gDispStat.curr_lang = LANG_CHINESE;
        
        if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY)
        {
            gDispStat.curr_lang = LANG_ENGLISH;
        }
        
        if ((gTrigerValue.triger_type == TRIGER_PRE_BROADCAST ) || (gTrigerValue.triger_type == TRIGER_PRE_BROADCAST_1))
            do_get_station_config_pre();
        else if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
            do_get_station_config_arrive();
            
        if (gSysConfg.disp_type == DISP_LANG_BOTH)
        {
            LoadConnect(LANG_CHINESE, page_buf);
            LoadConnect(LANG_ENGLISH, buf);
        }
        else
        {
            LoadConnect(gDispStat.curr_lang, page_buf);
        }
        
        BufConsistToOne(page_buf, buf);
        Do_ConnectBmp(page_buf);
    }
    
    if (1 == gDispStat.change_lang_flag)
    {
        gDispStat.change_lang_flag = 0;
        
        if (gSysConfg.disp_type == DISP_LANG_BOTH)
        {
            LoadConnect(LANG_CHINESE, page_buf);
            LoadConnect(LANG_ENGLISH, buf);
        }
        else
        {
            LoadConnect(gDispStat.curr_lang, page_buf);
        }
        
        //是否需要连接广告
        if (gTrigerValue.ad_english_num > 0)
        {
            if (gTrigerValue.station_msg_disp_time <= 0)
            {
                gTrigerValue.station_msg_disp_time = 0;
                
                if (gDispStat.curr_lang == LANG_ENGLISH)
                {
                    if (gTrigerValue.ad_english_index >= gTrigerValue.ad_english_num)
                    {
                        gTrigerValue.ad_english_index = 0;
                    }
                    
                    GetBmp(BMP_TYPE_AD, gTrigerValue.ad_english_buf[gTrigerValue.ad_english_index] - 1, gDispStat.curr_lang);
                    gTrigerValue.ad_english_index ++;
                }
                else if (gDispStat.curr_lang == LANG_CHINESE)
                {
                    if (gTrigerValue.ad_chinese_index >= gTrigerValue.ad_chinese_num)
                    {
                        gTrigerValue.ad_chinese_index = 0;
                    }
                    
                    GetBmp(BMP_TYPE_AD, gTrigerValue.ad_chinese_buf[gTrigerValue.ad_chinese_index] - 1, gDispStat.curr_lang);
                    gTrigerValue.ad_chinese_index ++;
                }
            }
            else
            {
                BufConsistToOne(page_buf, buf);
                Do_ConnectBmp(page_buf);
                gTrigerValue.station_msg_disp_time --;
            }
        }
        else
        {
            BufConsistToOne(page_buf, buf);
            Do_ConnectBmp(page_buf);
        }
    }
    
#else
    
    if (gTrigerValue.triger_flag == 1)
    {
        gTrigerValue.triger_flag = 0;
    
        if (gSysConfg.disp_type == DISP_LANG_BOTH)
        {
            gDispStat.curr_lang = LANG_CHINESE;
            gTrigerValue.station_msg_disp_e_time = 2;
            gTrigerValue.station_msg_disp_c_time = 2;
        }
        else if (gSysConfg.disp_type == DISP_LANG_ENGLISH_ONLY)
        {
            gDispStat.curr_lang = LANG_ENGLISH;
            gTrigerValue.station_msg_disp_e_time = 2;
        }
        else
        {
            gDispStat.curr_lang = LANG_CHINESE;
            gTrigerValue.station_msg_disp_c_time = 2;
        }
    
        if ((gTrigerValue.triger_type == TRIGER_PRE_BROADCAST ) || (gTrigerValue.triger_type == TRIGER_PRE_BROADCAST_1))
            do_get_station_config_pre();
        else if (gTrigerValue.triger_type == TRIGER_ARRIVE_BROADCAST)
            do_get_station_config_arrive();
    
        LoadConnect(gDispStat.curr_lang, page_buf);
        Do_ConnectBmp(page_buf);
    }
    
    if (1 == gDispStat.change_lang_flag)
    {
        gDispStat.change_lang_flag = 0;
    
        if (gSysConfg.disp_type == DISP_LANG_BOTH)
        {
            if (gDispStat.curr_lang == LANG_CHINESE)
                gDispStat.curr_lang = LANG_ENGLISH;
            else
                gDispStat.curr_lang = LANG_CHINESE;
        }
    
        if (gDispStat.curr_lang == LANG_ENGLISH)
        {
            //是否需要连接广告
            if (gTrigerValue.ad_english_num > 0)
            {
                if (gTrigerValue.station_msg_disp_e_time <= 0)
                {
                    gTrigerValue.station_msg_disp_e_time = 0;
    
                    if (gTrigerValue.ad_english_index >= gTrigerValue.ad_english_num)
                    {
                        gTrigerValue.ad_english_index = 0;
                    }
    
                    GetBmp(BMP_TYPE_AD, gTrigerValue.ad_english_buf[gTrigerValue.ad_english_index] - 1, gDispStat.curr_lang);
                    gTrigerValue.ad_english_index ++;
                }
                else
                {
                    gTrigerValue.station_msg_disp_e_time --;
                    LoadConnect(gDispStat.curr_lang, page_buf);
                    Do_ConnectBmp(page_buf);
                }
            }
            else
            {
                LoadConnect(gDispStat.curr_lang, page_buf);
                Do_ConnectBmp(page_buf);
            }
        }
        else if (gDispStat.curr_lang == LANG_CHINESE)
        {
            //是否需要连接广告
            if (gTrigerValue.ad_chinese_num > 0)
            {
                if (gTrigerValue.station_msg_disp_c_time <= 0)
                {
                    gTrigerValue.station_msg_disp_c_time = 0;
    
                    if (gTrigerValue.ad_chinese_index >= gTrigerValue.ad_chinese_num)
                    {
                        gTrigerValue.ad_chinese_index = 0;
                    }
    
                    GetBmp(BMP_TYPE_AD, gTrigerValue.ad_chinese_buf[gTrigerValue.ad_chinese_index] - 1, gDispStat.curr_lang);
                    gTrigerValue.ad_chinese_index ++;
                }
                else
                {
                    gTrigerValue.station_msg_disp_c_time --;
                    LoadConnect(gDispStat.curr_lang, page_buf);
                    Do_ConnectBmp(page_buf);
                }
            }
            else
            {
                LoadConnect(gDispStat.curr_lang, page_buf);
                Do_ConnectBmp(page_buf);
            }
        }
    }
    
#endif
#else
    
    if (gDispStat.change_lang_flag == 1)
    {
        gDispStat.change_lang_flag = 0;
    
        if (gDispStat.curr_lang == LANG_ENGLISH) gDispStat.curr_lang = LANG_CHINESE;
        else gDispStat.curr_lang = LANG_ENGLISH;
    
        GetBmp(gDispStat.curr_line, gDispStat.curr_station, gDispStat.curr_lang);
    }
    
#endif
}



void SpiDmaInit(void)
{
    //u16 i;
    DMA_InitTypeDef DMA_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    /*
        for (i=0; i<SPI_BUFFER_SIZE; i++) {
            SpiDmaBuf[i] = i;
        }
    */
    /* Configure DMA2 Stream0  channel0*/
    DMA_InitStructure.DMA_Channel = DMA_Channel_3;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&SpiDmaBuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; //DMA_DIR_MemoryToMemory;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)LED_PHYSICS_WIDTH;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);
    /* Enable DMA2 Stream0 Transfer Complete interrupt */
    DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
    /* Get Current Data Counter value before transfer begins */
    gStartCnt = DMA_GetCurrDataCounter(DMA2_Stream3);
    /* DMA2_Stream0 enable */
    DMA_Cmd(DMA2_Stream3, ENABLE);
    /* SPI1 Configuration (Master Tx, 14 MBaud) -----------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode =  SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    /* Enable SPI1 */
    /* Enable SPI1's DMA interface */
    SPI_I2S_DMACmd(SPI1,  SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_Cmd(SPI1, ENABLE);
}


tDoorStat GetDoorStat( void )
{
    return gDoorStat;
}



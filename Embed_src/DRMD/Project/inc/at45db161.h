//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//文件名称:AT45DB161.h

//文件作用:对外扩串行存储器at45db161操作的头文件

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifndef AT45DB161_H
#define AT45DB161_H

#include "config.h"
#include "TypeDef.h"
#include "LedDrv.h"

#include "include.h"




#define LED_HEIGHT                              16
#define LED_IMAGE_MAX_WIDTH                     (4096)


#define UPDATE_STATUS_ADDR                      0
#define CONFIG_ADDR                             1
#define LINE1_CONFIG_ADDR                       2
#define LINE2_CONFIG_ADDR                       82
#define LINE3_CONFIG_ADDR                       162
#define LINE1_BASE_ADDR                         242
#define LINE2_BASE_ADDR                         322
#define LINE3_BASE_ADDR                         402
#define MSG_BASE_ADDR                           482
#define AD_BASE_ADDR                            1282
#define EME_BASE_ADDR                           2482//2882  //改小应该可以  2482
#define VERSION_FLAG_ADDR                       3523

/*
const u16 SpiSpageAddrBuf[] = {
    LINE1_BASE_ADDR,LINE2_BASE_ADDR,LINE3_BASE_ADDR,MSG_BASE_ADDR,
    NEXT_BEFORE_ADDR,NEXT_AFTER_ADDR,ARRIVE_BEFORE_ADDR,ARRIVE_AFTER_ADDR,
};
*/

#define READ_STATE_REGISTER     0xD7        // 读取状态寄存器
#define BUFFER_1_WRITE          0x84        // 写入第一缓冲区
#define BUFFER_1_READ           0xD4        // 读取第一缓冲区
#define B1_TO_MM_PAGE_PROG_WITH_ERASE   0x83// 将第一缓冲区的数据写入主存储器（擦除模式）
#define MM_PAGE_TO_B1_XFER      0x53        // 将主存储器的指定页数据加载到第一缓冲区
#define CONTINUOUS_ARRAY_READ    0x0b    //连续读flash
#define PAGE_LEN    528             //Flash页长度
#define PAGE_ERASE              0x81

#define SET_SPI_CS()   GPIO_SetBits(GPIOA, GPIO_Pin_4);GPIO_SetBits(GPIOA, GPIO_Pin_4)   //置位片选引脚
#define CLR_SPI_CS()   GPIO_ResetBits(GPIOA, GPIO_Pin_4);GPIO_ResetBits(GPIOA, GPIO_Pin_4)   //清除片选引脚
#define SET_SPI_CLK()  GPIO_SetBits(GPIOC, GPIO_Pin_10);GPIO_SetBits(GPIOC, GPIO_Pin_10)    //置位时钟引脚
#define CLR_SPI_CLK()  GPIO_ResetBits(GPIOC, GPIO_Pin_10);GPIO_ResetBits(GPIOC, GPIO_Pin_10)    //清除时钟引脚


#define min(x,y) ((x) < (y) ? x : y)




extern u8 gLedImageBuf[16][(LED_IMAGE_MAX_WIDTH / 8) + 36];
//extern u8 gLedImageBuf[16][(LED_IMAGE_MAX_WIDTH/8)];

//extern u8 *gLedImageBuf;
extern u16 gBmpWidth;
extern tLedColor gLedColor;
extern tBmpUpdateFlag gBmpUpdateFlag;
extern tUpdateBmpSt gUpdateBmpSt;




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//              写入一个字节             //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
extern  void write_byte(uint8_t x);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//              读1个字节                //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
extern  uint8_t read_byte ( void );

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//            写一条(BMP)内容            //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//

extern  void write_bmp(u16 page0, u32 lenth0, u8 *buf);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//            读一条(BMP)内容            //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
extern  u8 read_bmp(u16 page0);


extern  u8 read_bmp(u16 page0);


extern u16 GetBmpStartPage(tBmpParam param);
extern void page_erase(u16 page);
extern void write_ok_flag(void);
extern void UpdateBmp(u8 *buf, u16 lenth);
extern void GetBmpUpdateFlag(void);
extern void UpdateConfig(u8 *buf, u16 lenth);
extern void GetConfig(void);
extern u8 GetBmp(tBmpType type, u16 index, tLanguage language);
extern void UsbPrintConfig(void);
extern void write_version_flag(u8 flag);
extern u8 get_version_flag(void);
extern void LoadConnect(tLanguage lang, u16 *page_buf);
extern u8 GetStationConfig(u8 *buf, tBmpType line_no, u8 index, tLanguage lang, tConfigType type);
extern u8 GetStationConfig1(u8 *buf, u8 index, u8 line_no);
extern  void UpdateStationConfig(u8 *buf, u16 lenth);
extern u8 GetConfigOfType( u8 *buf,  tTrigerType type );
extern void Do_ConnectBmp(u16 *page_buf);
extern void LoadBmpAndWriteToImageBuf(u8 line_no, u8 *buf, u8 station_current, u8 station_next, u8 station_end);

#endif


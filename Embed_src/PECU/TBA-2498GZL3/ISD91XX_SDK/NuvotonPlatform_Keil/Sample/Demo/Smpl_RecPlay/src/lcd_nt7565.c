
// qkdang

#include "ISD9xx.h"

//#include "lcd_nt7565.h"
#include "lcd_uc1601.h"

#include "Driver/DrvSYS.h"
#include "Driver/DrvSPI.h"
#include "Driver/DrvGPIO.h"

#ifndef SPI0_BASE
#define SPI0_BASE (0x40000000 + 0x30000)
#endif // SPI0_BASE

#define THE_PCLK    49152000
#define THE_SPI_CLK 4096000

#define CS_PIN 15
#define A0_PIN 14
#define CS_ON() DrvGPIO_ClrBit(GPA, 15)
#define CS_OFF() DrvGPIO_SetBit(GPA, 15)
#define A0_CMD() DrvGPIO_ClrBit(GPA, 14)
#define A0_DAT() DrvGPIO_SetBit(GPA, 14)

void lcd_enter(void)
{
	((volatile SPI_T *)SPI0_BASE)->DIVIDER.DIVIDER = ((((2*THE_PCLK) / (THE_SPI_CLK))) >> 2) - 1;
    ((volatile SPI_T *)SPI0_BASE)->SSR.SSR = 1; // channel
	((volatile SPI_T *)SPI0_BASE)->CNTRL.CLKP = 1;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.BYTE_SLEEP = 2;
}

#include "libSPIFlash.h"
extern const SFLASH_CTX g_SPIFLASH;
void lcd_leave(void)
{
	((volatile SPI_T *)SPI0_BASE)->DIVIDER.DIVIDER = 0;
    ((volatile SPI_T *)SPI0_BASE)->SSR.SSR = 0; // channel
	((volatile SPI_T *)SPI0_BASE)->CNTRL.CLKP = 0;
	sflash_set(&g_SPIFLASH);
}

static void nt7565_cmd(const unsigned char *dat, int cnt)
{
	A0_CMD();
	CS_ON();

	(*((volatile uint32_t *)(SPI0_BASE + 0x08))) = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TX_BIT_LEN = 8;

	while(cnt)
	{
		((volatile SPI_T*)(SPI0_BASE))->TX[0] = (*dat);
		((volatile SPI_T*)(SPI0_BASE))->CNTRL.GO_BUSY = 1;
		while(((volatile SPI_T*)(SPI0_BASE))->CNTRL.GO_BUSY);
		dat ++;
		cnt --;
	}

	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TX_BIT_LEN = 32;
	(*((volatile uint32_t *)(SPI0_BASE + 0x08))) = 0x08;

	CS_OFF();
	A0_DAT();
}

void lcd_dat(const unsigned char *dat, int cnt, int step)
{
	A0_DAT();
	CS_ON();

	(*((volatile uint32_t *)(SPI0_BASE + 0x08))) = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TX_BIT_LEN = 8;

	while(cnt)
	{
		((volatile SPI_T*)(SPI0_BASE))->TX[0] = (*dat);
		((volatile SPI_T*)(SPI0_BASE))->CNTRL.GO_BUSY = 1;
		while(((volatile SPI_T*)(SPI0_BASE))->CNTRL.GO_BUSY);
		dat += step;
		cnt --;
	}

	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TX_BIT_LEN = 32;
	(*((volatile uint32_t *)(SPI0_BASE + 0x08))) = 0x08;
	
	CS_OFF();
}

int lcd_init(void)
{
	DrvGPIO_Open(GPA, CS_PIN, IO_OUTPUT);
	DrvGPIO_Open(GPA, A0_PIN, IO_OUTPUT);
	
	CS_OFF();
	A0_DAT();

	SYS->GPA_ALT.GPA0              = 1; // MOSI0
	SYS->GPA_ALT.GPA1              = 1; // SCLK
	SYS->GPA_ALT.GPA3              = 1; // MISO0

	if(!SYSCLK->APBCLK.SPI0_EN)
	{
		SYSCLK->APBCLK.SPI0_EN        = 1;
		SYS->IPRSTC2.SPI0_RST         = 1;
		SYS->IPRSTC2.SPI0_RST         = 0;
	}

	((volatile SPI_T *)SPI0_BASE)->CNTRL.CLKP = 1;
	((volatile SPI_T *)SPI0_BASE)->CNTRL.TX_NEG = 1;
	((volatile SPI_T *)SPI0_BASE)->CNTRL.RX_NEG = 0;

    ((volatile SPI_T *)SPI0_BASE)->SSR.SSR = 1; // channel
    ((volatile SPI_T *)SPI0_BASE)->SSR.ASS = 1;

	((volatile SPI_T *)SPI0_BASE)->SSR.SS_LVL = 0;

	((volatile SPI_T *)SPI0_BASE)->DIVIDER.DIVIDER = ((((2*THE_PCLK) / (THE_SPI_CLK))) >> 2) - 1;
	//((volatile SPI_T *)SPI0_BASE)->DIVIDER.DIVIDER = 0;
	((volatile SPI_T *)SPI0_BASE)->DIVIDER.DIVIDER2 = 0;

	// need more init, omit here
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TX_BIT_LEN = 0; // 32
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.LSB = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.BYTE_ENDIAN = 0; // we need big endian
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.BYTE_SLEEP = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.SLEEP = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.SLAVE = 0;
	((volatile SPI_T*)(SPI0_BASE))->CNTRL.TWOB = 0;

	((volatile SPI_T*)(SPI0_BASE))->CNTRL.FIFO = 0;

	// DrvSPI_EnableInt(SPI0_BASE);

	lcd_enter();
	nt7565_cmd("\xE2", 1); // reset command
	nt7565_cmd("\xA2", 1); // bias 1-6
	nt7565_cmd("\xA0", 1); // adc: normal
	nt7565_cmd("\xC8", 1); // common output: reverse
	nt7565_cmd("\x24", 1); // V5 ratio: 0x04
	nt7565_cmd("\x81\x24", 2); // elec volume
	nt7565_cmd("\x2F", 1); // voltage, all on
	nt7565_cmd("\x40", 1); // display start address 0
	nt7565_cmd("\xAF", 1); // display on
	//nt7565_dat("\x55\x55\x55\x55\x55\x55\x55\x55\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA", 16, 1);
	lcd_leave();

	return 0;
}

void lcd_locate(int x, int y)
{
	unsigned char v;
	lcd_enter();
	v = 0xB0 + y;
	nt7565_cmd(&v, 1);
	y = x;
	y >>= 4;
	v = 0x10 | y;
	nt7565_cmd(&v, 1); // column address
	v = x & 0x0F;
	nt7565_cmd(&v, 1);
	lcd_leave();
}

void lcd_clr(int x, int y, int cx, int cy)
{
	const unsigned char v = 0x0;
	lcd_enter();
	while(cy)
	{
		lcd_locate(x, y);
		lcd_dat(&v, cx, 0);
		cy --;
		y ++;
	}
	lcd_leave();
}

#include "font_6x8.h"

static void lcd_putch(unsigned char val)
{
	if(val < ' ' || val > (' ' + 0x95))
	{
		val = '.' - ' ';
	}
#ifndef LOWER_CASE
	else if(val >= 'a' && val <= 'z')
	{
		val -= (32 + ' ');
	}
	else if(val > 'z')
	{
		val -= (26 + ' ');
	}
#endif
	else
	{
		val -= ' ';
	}
	
	lcd_dat(Font_6x8[val], 5, 1);
}

static const unsigned char tblHex[] = "0123456789ABCDEF";

void lcd_puthex(unsigned char val)
{
	unsigned char vx = 0;
	unsigned char vh = val;
	vh >>= 4;
	lcd_enter();
	lcd_putch(tblHex[vh]);
	lcd_dat(&vx, 1, 0);
	lcd_putch(tblHex[val & 0x0F]);
	lcd_dat(&vx, 1, 0);
	lcd_leave();
}

void lcd_puts(const char *val)
{
	unsigned char vx = 0;
	lcd_enter();
	while(*val)
	{
		lcd_putch(*val);
		lcd_dat(&vx, 1, 0);
		val ++;
	}
	lcd_leave();
}

void lcd_puts_at(int x, int y, const char *val)
{
	unsigned char vx = 0;
	unsigned char v;
	lcd_enter();
	v = 0xB0 + y;
	nt7565_cmd(&v, 1);
	y = x;
	y >>= 4;
	v = 0x10 | y;
	nt7565_cmd(&v, 1); // column address
	v = x & 0x0F;
	nt7565_cmd(&v, 1);
	while(*val)
	{
		lcd_putch(*val);
		lcd_dat(&vx, 1, 0);
		val ++;
	}
	lcd_leave();
}

/*  P0  */
sbit P0_7		= 	P0^7;
sbit P0_6		= 	P0^6;
sbit P0_5		= 	P0^5;
sbit P0_4		= 	P0^4;
sbit P0_3		= 	P0^3;
sbit P0_2		= 	P0^2;
sbit P0_1		= 	P0^1;
sbit P0_0		= 	P0^0;

/*  P1  */
sbit P1_7		= 	P1^7;
sbit P1_6		= 	P1^6;
sbit P1_5		= 	P1^5;
sbit P1_4		= 	P1^4;
sbit P1_3		= 	P1^3;
sbit P1_2		= 	P1^2;
sbit P1_1		= 	P1^1;
sbit P1_0		= 	P1^0;

/*	P2	*/
sbit P2_7		= 	P2^7;
sbit P2_6		= 	P2^6;
sbit P2_5		= 	P2^5;
sbit P2_4		= 	P2^4;
sbit P2_3		= 	P2^3;
sbit P2_2		= 	P2^2;
sbit P2_1		= 	P2^1;
sbit P2_0		= 	P2^0;

/*	P3	*/
sbit P3_7		= 	P3^7;
sbit P3_6		= 	P3^6;
sbit P3_5		= 	P3^5;
sbit P3_4		= 	P3^4;
sbit P3_3		= 	P3^3;
sbit P3_2		= 	P3^2;
sbit P3_1		= 	P3^1;
sbit P3_0		= 	P3^0;

sfr P4			=	0xe8;
sbit P4_3		=	P4^3;
sbit P4_2		=	P4^2;
sbit P4_1		=	P4^1;
sbit P4_0		=	P4^0;

// STC89C58RD+ additional SFR
sfr AUXR		=	0x8e;
sfr	ISP_DATA	=	0xe2;
sfr	ISP_ADDRH	=	0xe3;
sfr ISP_ADDRL	=	0xe4;
sfr ISP_CMD		= 	0xe5;
sfr	ISP_TRIG	=	0xe6;
sfr ISP_CONTR	=	0xe7;

sfr WDT_CONTR	=	0xe1;

sfr16 RCAP2LH	= 	0xca;
sfr16 T2LH		=	0xcc;

#define ISP_IAP_BYTE_READ		0x01
#define ISP_IAP_BYTE_PROGRAM	0x02
#define ISP_IAP_SECTOR_ERASE	0x03
#define ISP_IAP_WAIT_TIME		0x01

#define T0_INTERVAL_L			0x00
#define T0_INTERVAL_H			0xdc

#define TIMEOUT					0x01
#define TIME_OK					0x01

// 广播控制信号引脚定义
#define PA_AUDIO_OUT			P1_0
#define PA_AUDIO_IN				P1_1
#define PA_STATE_CHK			P0_1
#define PA_CTRL					P1_3
#define PA_LED					P2_3

// 对讲控制信号引脚定义
#define PC_AUDIO_OUT			P2_7
#define PC_AUDIO_IN				P4_1
#define PC_STATE_CHK			P0_0
#define PC_CTRL					P1_4
#define PC_LED					P2_2

// 重连广播控制信号引脚定义
#define LINK_AUDIO_OUT			P2_5
#define LINK_AUDIO_IN			P2_6
#define LINK_PA_STATE_CHK		P1_2
#define LINK_PC_STATE_CHK		P1_6
#define LINK_PA_CTRL			P1_5
#define LINK_PC_CTRL			P1_7
#define LINK_PA_LED				P2_1
#define LINK_PC_LED				P2_0		

// 工作指示灯引脚定义
#define WORK_LED				P2_4

#define ON						0
#define OFF						1

typedef struct
{
	unsigned char dest_netnum;
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;
	}dest_eqnum;
	unsigned char src_netnum;
	struct
	{
		unsigned char eq_type:4;
		unsigned char eq_num:4;
	}src_eqnum;
	union
	{
		struct
		{
			unsigned char packet_type:4;
			unsigned char token_return:1;
			unsigned char m_active:1;
			unsigned char m_error:1;
			unsigned char m_include:1;
		}bits;
		unsigned char bytes;
	}cmd;
	unsigned char len;
}FRAME_HEADER;

#define	COM_RX_HEAD				0x01
#define	COM_RX_DATA				0x02
#define	COM_RX_DLE				0x03

#define PROCESS_PACKET			0x00
#define P2P_PACKET				0x01
#define TOKEN_PACKET			0x02
#define DOWNLOAD_PACKET			0x04

#define	RX_BUF_LEN				128	//416	//408	// 360	//256		//64
#define TX_BUF_LEN				16	//64	//128		//64
unsigned char xdata rxbuf[RX_BUF_LEN];
unsigned char xdata txbuf[TX_BUF_LEN];
volatile unsigned int data rx_wr_index;
volatile unsigned int data rx_rd_index;
volatile unsigned int data rx_counter;

#define RX_BUFFER_LEN			32			
unsigned char xdata rx_buffer[RX_BUFFER_LEN];

unsigned char rx_status = COM_RX_HEAD;
unsigned char cmd_type;

unsigned char com_rx_status = COM_RX_HEAD;

volatile unsigned char pa_led_blink_interval = 0;
volatile unsigned char pc_led_blink_interval = 0;
volatile unsigned char link_pa_led_blink_interval = 0;
volatile unsigned char link_pc_led_blink_interval = 0;
volatile unsigned char work_led_blink_interval = 0;
void work_led_blink(unsigned char interval);				
void pa_led_blink(unsigned char interval);
void pc_led_blink(unsigned char interval);
void link_pa_led_blink(unsigned char interval);
void link_pc_led_blink(unsigned char interval);

void get_ver_info(void);

void prc_rx_data(void);
void prase_rx_data(unsigned char len);
unsigned char chksum(unsigned char *buf, unsigned char n);

void ISP_IAP_enable(void);
void ISP_IAP_disable(void);
void ISP_IAP_erase(unsigned int sector_addr);
unsigned char ISP_IAP_byte_read(unsigned int addr);
void ISP_IAP_n_byte_read(unsigned int addr, unsigned int n, unsigned char *buf);
unsigned char ISP_IAP_n_byte_program(unsigned int addr, unsigned int n, unsigned char *buf);

unsigned char get_char(unsigned char *pchar);
void put_n_char(unsigned char *buf, unsigned char n);
void put_char(unsigned char c);

void device_init(void);

void hex2ascii(unsigned char *buf, unsigned char hex);

void delay_1ms(unsigned int _1ms); 


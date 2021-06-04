
#define ISP_IAP_BYTE_READ		0x01
#define ISP_IAP_BYTE_PROGRAM	0x02
#define ISP_IAP_SECTOR_ERASE	0x03
#define ISP_IAP_WAIT_TIME		0x01

#define T0_INTERVAL_L			0x00
#define T0_INTERVAL_H			0xdc

#define TIMEOUT					0x01
#define TIME_OK					0x01

// 音量调节芯片控制引脚定义
#define M62429_CLOCK			P2_6
#define M62429_DATA				P2_7

// 广播触发信号检测、控制引脚定义
#define PA_STATE_CHK			P2_5
#define PA_STATE_CTRL			P2_4

// 噪检控制开关引脚定义
#define NOISE_KEY				P0_3

// 工作指示灯引脚定义
#define ACT_LED					P0_2

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

void PA_ctrl(void);

#define AGC_NORMAL				0x01
#define AGC_BYPASS				0x02
unsigned char data now_AGC_mode;				// 噪检工作模式，AGC_NORMAL为噪检模式，AGC_BYPASS为非噪检模式
unsigned char data old_AGC_mode;
void get_AGC_mode(void);
void alter_AGC_mode(void);						// 噪检工作模式发生变化，需要重新设置音量
void AGC_mode_init(void);
void AGC(void);

volatile unsigned char act_led_blink_interval = 0;
void act_led_blink(unsigned char interval);				

void get_ver_info(void);

void prc_rx_data(void);
void prase_rx_data(unsigned char len);
unsigned char chksum(unsigned char *buf, unsigned char n);

void init_volumn_all(void);
void m62429_op(unsigned char op_type, unsigned char volumn);

#define ADC_POWER				0x80			// ADC power control bit
#define ADC_FLAG				0x10			// ADC complete flag
#define ADC_START				0x08			// ADC start control bit
#define ADC_SPEEDLL				0x00			// 1080 clocks
#define ADC_SPEEDL				0x20			// 810 clocks
#define ADC_SPEEDH				0x40			// 540 clocks
#define ADC_SPEEDHH				0x60			// 270 clocks
unsigned char ad_sample(unsigned char an);
unsigned char ADC_8(unsigned char an);
void ADC_init(void);

void ISP_IAP_enable(void);
void ISP_IAP_disable(void);
void ISP_IAP_erase(unsigned int sector_addr);
unsigned char ISP_IAP_byte_read(unsigned int addr);
void ISP_IAP_n_byte_read(unsigned int addr, unsigned int n, unsigned char *buf);
unsigned char ISP_IAP_n_byte_program(unsigned int addr, unsigned int n, unsigned char *buf);

unsigned char get_char(unsigned char *pchar);
void put_n_char(unsigned char *buf, unsigned char n);
void put_char(unsigned char c);

void m62429_init(void);
void device_init(void);

void hex2ascii(unsigned char *buf, unsigned char hex);

void delay_50us(unsigned int _50us); 


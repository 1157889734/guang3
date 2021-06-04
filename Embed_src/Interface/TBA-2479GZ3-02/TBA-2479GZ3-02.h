
#define ISP_IAP_BYTE_READ		0x01
#define ISP_IAP_BYTE_PROGRAM	0x02
#define ISP_IAP_SECTOR_ERASE	0x03
#define ISP_IAP_WAIT_TIME		0x01

#define T0_INTERVAL_L			0x00
#define T0_INTERVAL_H			0xdc

#define TIMEOUT					0x01
#define TIME_OK					0x01

// 
#define PA_ON_CTRL				P0_0				// 广播状态线控制
#define PC_ON_CTRL				P0_1				// 对讲状态线控制
#define RADIO_ON_CTRL			P0_2				// 无线广播状态控制
#define PA_PC_SWITCH			P0_3				// 广播、对讲切换控制

// 
#define PA_STATE_CHK			P2_0
#define PC_STATE_CHK			P2_1
#define RADIO_STATE_CHK			P2_2
#define PA_ON_CHK				P2_3
#define PC_ON_CHK				P2_4
#define PADIO_CTRL_CHK			P2_5
#define PA_STATE_CTRL			P2_6
#define PC_STATE_CTRL			P2_7


// 工作指示灯引脚定义
#define ACT_LED					P3_7

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

#define BUS_PC_USE				0
#define BUS_PA_USE				1

#define STATUS_BUS_IDLE			0x00
#define STATUS_BUS_PA_MASTER 	0x01
#define STATUS_BUS_PA_SLAVE		0x02
#define STATUS_BUS_PC_MASTER	0x03
#define STATUS_BUS_PC_SLAVE		0x04
#define STATUS_BUS_RADIO_MASTER	0x05
#define STATUS_BUS_RADIO_SLAVE	0x06

unsigned char bus_status;

#define LEVEL_BUS_51V			0x00
#define LEVEL_BUS_39V			0x01
#define LEVEL_BUS_20V			0x02
#define LEVEL_BUS_0V			0x03

unsigned char bus_level;
void chk_bus_level(void);

void PA_PC_ctrl(void);

volatile unsigned char act_led_blink_interval = 0;
void act_led_blink(unsigned char interval);				

void get_ver_info(void);

void prc_rx_data(void);
void prase_rx_data(unsigned char len);
unsigned char chksum(unsigned char *buf, unsigned char n);


#define ADC_POWER				0x80			// ADC power control bit
#define ADC_FLAG				0x10			// ADC complete flag
#define ADC_START				0x08			// ADC start control bit
#define ADC_SPEEDLL				0x00			// 1080 clocks
#define ADC_SPEEDL				0x20			// 810 clocks
#define ADC_SPEEDH				0x40			// 540 clocks
#define ADC_SPEEDHH				0x60			// 270 clocks

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

void delay_50us(unsigned int _50us); 


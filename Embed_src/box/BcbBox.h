




#define KEY_PTT					1
#define KEY_PC					2
#define KEY_PA					3
#define KEY_CC					4



#define	RX_BUF_LEN				64	//408	// 360	//256		//64
#define TX_BUF_LEN				64	//64	//128		//64
//unsigned char xdata rx_int_buf[RX_BUF_LEN];
// char xdata rxbuf[RX_BUF_LEN];
//unsigned char xdata txbuf[TX_BUF_LEN];
//unsigned char txbuf_out[32];




typedef unsigned char UINT8;
typedef char INT8;
typedef unsigned int UINT16;
typedef int INT16;






void prc_rx_data(void);

void dispose_para(unsigned char len);

unsigned char chksum(unsigned char *buf, unsigned char n);


unsigned char ad_sample(unsigned char an);
void init_adc(void);
unsigned char adc8(unsigned char an);



void clr_rx_buf(unsigned char n);
//void clr_tx_buf(unsigned char n);
void init_device(void);

UINT8 get_device_addr(void);




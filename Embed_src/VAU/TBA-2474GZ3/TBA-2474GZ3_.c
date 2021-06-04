#include <stc12c5620ad.h>
#include <intrins.h>
#include "TBA-2474GZ3.h"

#define DEBUG

#define DEBUG_SWITCH			0x10
#define VERSION					0x20

unsigned char data base_volumn = 3;								// 2014-04-02修改，volume_AGC_val也需修改为volume_AGC_val[2] = {3, 0};							unsigned char data base_volumn = 6;					// 基础音量值为6

// 设定噪声采样值范围，采样值低于92，音量值不作调整
// 采样值大于90小于150，增益为6
// 采样值大于150，增益为3 + 6 = 9
unsigned char noise_ad_val[2] = {90, 150};
unsigned char volume_AGC_val[2] = {3, 0};						// unsigned char volume_AGC_val[2] = {3, 3};
unsigned char get_AGC_val(unsigned char val);		// 根据噪声A/D采样值输出音量增益值

volatile unsigned int data noise_ad_time = 0;		// 噪声采样时间间隔，设定1s间隔
volatile unsigned char data com_timeout = TIMEOUT;

unsigned char data debug_switch = 0;

void main(void)
{
	device_init();
	
	m62429_init();						// 音量调节芯片M62429数据、时钟I/O口初始化

	ADC_init();							// A/D转换器初始化

	AGC_mode_init();					// 噪检模式初始化
	
#ifdef	DEBUG
	get_ver_info();	
#endif
	
	while(1)
	{
		PA_ctrl();
		alter_AGC_mode();					// 检测AGC模式变化
		AGC();								// AGC工作
		prc_rx_data();
		
		if(com_timeout == TIMEOUT)
			act_led_blink(50);					// 工作指示灯间隔500ms亮灭1次，定时器间隔10ms中断一次
	}
}

void AGC(void)
{
	static unsigned int data sample_10_val[2] ={0, 0};		// 2 A/D通道10次采样累加值
	static unsigned char data sample_times = 0;						// 10次采样次数

	static unsigned int noise_sample_val[2] = {0, 0};		// 2 A/D通道每10次采样累加平均后再累加
	static unsigned int noise_sample_times;					// 10次平均值累加的次数

	unsigned char data i;
	unsigned char data tmp;
	unsigned char tmp_AGC_val;

	unsigned char xdata tmpbuf[32];

	if(now_AGC_mode != AGC_NORMAL)			// 如果当前工作模式不是AGC_NORMAL，则不进行噪检调节，直接退出
		return;

	// 工作于噪检模式，根据噪声变化自动调节音量输出
	if(noise_ad_time < 100)				// 1s几个时间内采样值进行累加，取平均值
	{
		for(i = 0; i < 2; i++)
 			sample_10_val[i] += ADC_8(i);	// A/D通道0、1采样值累加
		if(++sample_times >= 10)			// 各通道每10次采样结束后取一次平均值，将此平均值累加赋给noise_sample_val[2]数组
 	    {
			for(i = 0; i < 2; i++)
			{
				tmp = sample_10_val[i] / sample_times;
				noise_sample_val[i] += tmp;

				sample_10_val[i] = 0;
			}

			sample_times = 0;

			noise_sample_times++;				// 在1s时间内每10次采样累加值取平均值的次数
		}		
	}
	else		// 1s A/D采样时间到，进行音量调节处理			
	{
		for(i = 0; i < 2; i++)
		{
			tmp = noise_sample_val[i] / noise_sample_times;	// 1s时间内采样noise_sample_times * 10次的噪声平均值
			//tmp_AGC_val = get_incre_val(tmp);
			tmp_AGC_val = get_AGC_val(tmp);
			
			m62429_op(i, (base_volumn - tmp_AGC_val));		// 2通道音量单独调整
			
			if(debug_switch & 0x01)
			{
				// 打印通道号
				put_n_char("\r\n\r\n", 4);
				put_n_char("\tChannel ---- ", 14);
				hex2ascii(tmpbuf, (unsigned char)i);
				put_n_char(tmpbuf, 3);
				put_n_char("\r\n\r\n", 4);
				
				// 打印A/D采样值
				put_n_char("\r\n\r\n", 4);					
				put_n_char("\tA/D sample ", 12);
				hex2ascii(tmpbuf, (unsigned char)tmp);
				put_n_char(tmpbuf, 3);
				put_n_char("\r\n\r\n", 4);

				// 打印音量增加值
				put_n_char("\tVolume increment ", 18);
				hex2ascii(tmpbuf, (unsigned char)tmp_AGC_val);
				put_n_char(tmpbuf, 3);
				put_n_char("dB\r\n\r\n", 6);

				// 打印音量基础值
				put_n_char("\tBase volume ", 13);
				hex2ascii(tmpbuf, (unsigned char)base_volumn);
				put_n_char(tmpbuf, 3);
				put_n_char("dB\r\n\r\n", 6);

				// 打印音量值
				put_n_char("\tVolume ", 8);
				hex2ascii(tmpbuf, (unsigned char)(base_volumn - tmp_AGC_val));
				put_n_char(tmpbuf, 3);
				put_n_char("dB - M62429 writen\r\n\r\n", 22);
				put_n_char("\r\n\r\n", 4);				
			}
		}
		
		
		/*	2014-12-29修改，在深圳地铁1号线26列车改造项目上更改，原为只采样1路噪声信号，同时更改两路音量输出
		 /*	更改为采样2路噪声信号，分别调整两路音量输出
		
		// 根据1个A/D采样值同时调整两路音量
		tmp = noise_sample_val[1] / noise_sample_times;	// 1s时间内采样noise_sample_times * 10次的噪声平均值
		tmp_AGC_val = get_AGC_val(tmp);
		m62429_op(2, (base_volumn - tmp_AGC_val));		// 2通道音量同时调整

		if(debug_switch & 0x01)
		{
			// 打印A/D采样值
			put_n_char("\r\n\r\n", 4);					
			put_n_char("\tA/D sample ", 12);
			hex2ascii(tmpbuf, (unsigned char)tmp);
			put_n_char(tmpbuf, 3);
			put_n_char("\r\n\r\n", 4);

			// 打印音量增加值
			put_n_char("\tVolume increment ", 18);
			hex2ascii(tmpbuf, (unsigned char)tmp_AGC_val);
			put_n_char(tmpbuf, 3);
			put_n_char("dB\r\n\r\n", 6);

			// 打印音量基础值
			put_n_char("\tBase volume ", 13);
			hex2ascii(tmpbuf, (unsigned char)base_volumn);
			put_n_char(tmpbuf, 3);
			put_n_char("dB\r\n\r\n", 6);

			// 打印音量值
			put_n_char("\tVolume ", 8);
			hex2ascii(tmpbuf, (unsigned char)(base_volumn - tmp_AGC_val));
			put_n_char(tmpbuf, 3);
			put_n_char("dB - M62429 writen\r\n\r\n", 22);

		}
		
		/*************************************************************************************************************************************/
		
		for(i = 0; i < 2; i++)
			noise_sample_val[i] = 0;			// 噪声采样值初始化
		
		noise_sample_times = 0;					// 1s内每10次采样平均值的累加次数清0
		noise_ad_time = 0;						// 1s采样定时间隔清0

	}
}

// 根据噪声A/D采样值输出音量增益值
unsigned char get_AGC_val(unsigned char val)		
{
	if(val < noise_ad_val[0])	// 噪声A/D采样值低于noise_ad_val[1]，增益输出0
		return(0);
	else if((val >= noise_ad_val[0]) && (val < noise_ad_val[1]))
		return(volume_AGC_val[0]);
	else if(val >= noise_ad_val[1])
		return(volume_AGC_val[0] + volume_AGC_val[1]);
	else
		return(0);
}

void alter_AGC_mode(void)		// 噪检工作模式发生变化，需要重新设置音量
{
	get_AGC_mode();

	if(old_AGC_mode != now_AGC_mode)			// 如果噪检工作模式发生变化，则重新设置音量调节模块初始值
	{
		m62429_op(2, base_volumn);				// 音量初始值为-9dB，1和2两个通道输出同时调整为-9dB	
		
		old_AGC_mode = now_AGC_mode;			
	}
}

void get_AGC_mode(void)
{
	unsigned char noise_key_chk;
	unsigned char tmp_noise_key_chk;

	tmp_noise_key_chk = NOISE_KEY;
	delay_50us(200);
	noise_key_chk = NOISE_KEY;
	if(noise_key_chk != tmp_noise_key_chk)	// 间隔10ms读取2次NOISE_KEY引脚电平值，如果不相同退出
		return;

	if(noise_key_chk != 0)
		now_AGC_mode = AGC_NORMAL;
	else
		now_AGC_mode = AGC_BYPASS;
}

void AGC_mode_init(void)
{
	old_AGC_mode = AGC_BYPASS;				// 上电后噪检工作模式处于非噪检模式

	m62429_op(2, base_volumn);				// 音量初始值为-9dB，1和2两个通道输出同时调整为-9dB			
}

void PA_ctrl(void)
{
	unsigned char pa_state_chk;
	unsigned char tmp_pa_state_chk;

	tmp_pa_state_chk = PA_STATE_CHK;
	delay_50us(200);
	pa_state_chk = PA_STATE_CHK;
	if(pa_state_chk != tmp_pa_state_chk)	// 间隔10ms读取2次PA_STATE_CHK引脚电平值，如果不相同退出
		return;

	if(pa_state_chk != 1)
		PA_STATE_CTRL = 0;
	else
		PA_STATE_CTRL = 1;	
}

void act_led_blink(unsigned char interval)
{
	if(act_led_blink_interval >= interval)
	{
		ACT_LED = ~ACT_LED;
		act_led_blink_interval = 0;		
	}
}

void get_ver_info(void)
{
	put_n_char("\r\n\r\n\t设备名称:  音量调节模块  TBA-2474GZ3\r\n", 43);
	put_n_char("\t项目名称:  广州地铁3号线B1型车改造项目\r\n", 41);
	put_n_char("\t版本信息:  Ver 1.0  2014-12-30\r\n", 33);
	put_n_char("\t制造商:    深圳市北海轨道交通技术有限公司\r\n", 44);
}

unsigned char chksum(unsigned char *buf, unsigned char n)
{
	unsigned char idata i;
	unsigned char idata tmp = 0;

	for(i = 0; i < n; i++)
		tmp += buf[i];

	return(tmp);
}

// 音量调节范围-83dB ~ 0dB，volumn = 0， 输出音量0dB
//                                 = 83，输出音量-83dB
//                                 >=84，静音
// op_type为通道选择，volumn为音量衰减值
void m62429_op(unsigned char op_type, unsigned char volumn)
{
	unsigned char data i;
	unsigned char idata tmp_volumn = 87;
	unsigned int data tmp = 0;

	if(volumn > 87)			// 如果音量衰减值>87，则按衰减值87处理
		volumn = 87;

	tmp_volumn -= volumn;

	tmp |= ((tmp_volumn & 0x03) << 5);
	tmp |= ((tmp_volumn & 0x7c) >> 2);
	tmp <<= 2;

	// D1 = 0: Both channels at a time, 1: A channel at a time
    // D0 = 0: 1CH	1: 2CH
	switch(op_type)
	{
	case 0:						// 1通道音量单独调整  
		tmp |= 2;
		break;

	case 1:						// 2通道音量单独调整
		tmp |= 3;
		break;

	case 2:						// 1和2通道音量同时调整
		tmp |= 0;
		break;

	default:;
	}
	
	tmp |= 0x0600;

	EA = 0;

	for(i = 0; i < 10; i++)			// send D0 ~ D9 bit, first D0
	{
		if((tmp & 0x01) == 0x01)
			M62429_DATA = 1;
		else
			M62429_DATA = 0;
		_nop_();

		M62429_CLOCK = 1;
		_nop_();

		M62429_DATA = 0;
		_nop_();

		M62429_CLOCK = 0;
		_nop_();
		_nop_();

		tmp >>= 1;
	}

	M62429_DATA = 1;				// send D10 bit
	_nop_();
	M62429_CLOCK = 1;
	_nop_();

	M62429_DATA = 1;
	_nop_();
	M62429_CLOCK = 0;
	_nop_();
	_nop_();

	EA = 1;
}


void m62429_init(void)				// 音量调节芯片M62429数据、时钟I/O口初始化
{
	M62429_DATA = 1;				// M62429数据I/O口上电初始化为高电平
	M62429_CLOCK = 0;				// M62429时钟I/O口上电初始化为低电平
}

unsigned char ADC_8(unsigned char an)
{
	if(an > 7)
		return 0;

	an &= 0x07;				// 选择通道号

	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | an;	// 打开A/D转换器电源
												// A/D转换器转换速度设为1080个时钟周期一次
	ADC_CONTR |= ADC_START;						// 启动A/D转换

	// 设置ADC_CONTR控制寄存器的语句执行后，要经过4个CPU时钟延时，其值才能保证被设置进ADC_CONTR控制寄存器
	delay_50us(2);								// 延时100us，等待A/D转换器电源稳定

	while(!(ADC_CONTR & ADC_FLAG));				// 等待A/D转换结束
	ADC_CONTR &= 0xE7;							// A/D转换结束标志位清0，一定要软件清零

	return(ADC_DATA);							// 返回A/D转换值
}

void ADC_init(void)
{
	P1M0 = 0xff;				// P1口用作A/D输入口
	P1M1 = 0xff;				// P1口用作A/D输入口

	EADC_SPI = 0;				// 禁止A/D转化器中断和SPI中断
	AUXR &= 0xef;				// AUXR寄存器EADCI = 0，禁止A/D转换中断

	ADC_DATA = 0;				// A/D转换器结果寄存器清0

	ADC_CONTR &= 0x8f;			// A/D转换器转换速度设为1080个时钟周期一次
	ADC_CONTR &= 0xf7;			// A/D转换器转换结束标志位清0
	ADC_CONTR |= ADC_POWER;		// 打开A/D转换器电源 

	delay_50us(200);			// 延时10ms，等待A/D转换器电源稳定
}

void prc_rx_data(void)
{
	unsigned char data tmp;	
	static unsigned int data rx_data_len = 0;
	static unsigned char data tmp_rx_chksum;
	static unsigned char data rx_chksum; 

	while(get_char(&tmp))
	{
		switch(rx_status)
		{
		case COM_RX_HEAD:
			if(0x7e == tmp)
			{
				rx_status = COM_RX_DATA;
				rx_data_len = 0;
				tmp_rx_chksum = 0;
			}
			else
				rx_status = COM_RX_HEAD;
			break;

		case COM_RX_DATA:
			if(0x7e == tmp)
			{
				if(rx_data_len > 0)
				{
					rx_status = COM_RX_HEAD;
					tmp_rx_chksum -= rx_chksum;
					//if(rx_chksum != (0x55 - tmp_rx_chksum))
					//	break;
					prase_rx_data(rx_data_len);

					rx_data_len = 0;
				}
				else
					rx_status = COM_RX_DATA;
			}
			else if(0x7f == tmp)
				rx_status = COM_RX_DLE;
			else
			{
				rx_buffer[rx_data_len++] = tmp;
				tmp_rx_chksum += tmp;
				rx_chksum = tmp;

				rx_status = COM_RX_DATA;
			}
			break;

		case COM_RX_DLE:
			rx_status = COM_RX_DATA;
			if(0x80 == tmp)
			{
				rx_buffer[rx_data_len++] = 0x7e;
				tmp_rx_chksum += 0x7e;
				rx_chksum = 0x7e;
			}
			else if(0x81 == tmp)
			{
				rx_buffer[rx_data_len++] = 0x7f;
				tmp_rx_chksum += 0x7f;
				rx_chksum = 0x7f;
			}
			else
				rx_status = COM_RX_HEAD;
			break;
		}
	}
}

void prase_rx_data(unsigned char len)
{
	FRAME_HEADER *ptr_frame_header;

	ptr_frame_header = (FRAME_HEADER *)rx_buffer;

	if(len < 7)
		return;

	switch(ptr_frame_header->cmd.bits.packet_type)
	{
	case PROCESS_PACKET:
		break;

	case P2P_PACKET:
		break;

	case TOKEN_PACKET:
		if(ptr_frame_header->len != 1)
			return;

		cmd_type = rx_buffer[6];

		switch(cmd_type & 0xf0)
		{
		case DEBUG_SWITCH:
				debug_switch = cmd_type & 0x01;
			break;

		case VERSION:
			get_ver_info();
			break;
		}

		break;

	case DOWNLOAD_PACKET:
		break;
	}

	return;
}

unsigned char ISP_IAP_byte_read(unsigned int addr)
{
	ISP_ADDRH = (unsigned char)(addr >> 8);
	ISP_ADDRL = (unsigned char)(addr & 0x00ff);
	ISP_CMD &= 0xf8;
	ISP_CMD |= ISP_IAP_BYTE_READ;
	ISP_IAP_enable();
	ISP_TRIG = 0x46;
	ISP_TRIG = 0xb9;
	_nop_();
	ISP_IAP_disable();

	return(ISP_DATA);
}

void ISP_IAP_n_byte_read(unsigned int addr, unsigned int n, unsigned char *buf)
{
	unsigned int data i;

	for(i = 0; i < n; i++)
		buf[i] = ISP_IAP_byte_read(addr++);
}

unsigned char ISP_IAP_n_byte_program(unsigned int addr, unsigned int n, unsigned char *buf)
{
	unsigned int data i = 0;
	unsigned int data in_sector_begin_addr = 0;
	unsigned int data sector_addr = 0;

	if(n > 256)
		return 0;

	in_sector_begin_addr = addr & 0x1ff;
	if((in_sector_begin_addr + n) > 128)
		return 0;

	sector_addr = (addr & 0xfe00);
	ISP_ADDRH = (unsigned char)(sector_addr >> 8);
	ISP_ADDRL = 0;
	ISP_CMD &= 0xf8;
	ISP_CMD |= ISP_IAP_SECTOR_ERASE;
	ISP_IAP_enable();
	ISP_TRIG = 0x46;
	ISP_TRIG = 0xb9;
	_nop_();

	for(i = 0; i < n; i++)
	{
		ISP_ADDRH = (unsigned char)(addr >> 8);
		ISP_ADDRL = (unsigned char)(addr & 0x00ff);
		ISP_DATA = buf[i];
		ISP_CMD &= 0xf8;
		ISP_CMD |= ISP_IAP_BYTE_PROGRAM;
		ISP_TRIG = 0x46;
		ISP_TRIG = 0xb9;
		_nop_();

		ISP_DATA = 0;
		ISP_CMD &= 0xf8;
		ISP_CMD |= ISP_IAP_BYTE_READ;
		ISP_TRIG = 0x46;
		ISP_TRIG = 0xb9;
		_nop_();

		if(ISP_DATA != buf[i])
		{
			ISP_IAP_disable();
			return 0;
		}

		addr++;
	}

	ISP_IAP_disable();

	return 1;
}

void ISP_IAP_erase(unsigned int sector_addr)
{
	ISP_ADDRH = (unsigned char)(sector_addr >> 8);
	ISP_ADDRL = 0;

	ISP_CMD &= 0xf8;
	ISP_CMD |= ISP_IAP_SECTOR_ERASE;
	ISP_IAP_enable();
	ISP_TRIG = 0x46;
	ISP_TRIG = 0xb9;
	_nop_();
	ISP_IAP_disable();
}

void ISP_IAP_enable(void)
{
	EA = 0;
	ISP_CONTR &= 0x18;
	ISP_CONTR |= ISP_IAP_WAIT_TIME;
	ISP_CONTR |= 0x80;
}

void ISP_IAP_disable(void)
{
	ISP_CONTR &= 0x7f;
	ISP_TRIG = 0x00;
	EA = 1;
}


void timer0(void) interrupt 1 using 1		// 间隔10ms中断
{
	ET0 = 0;
	TR0 = 0;
	TL0 = T0_INTERVAL_L;
	TH0 = T0_INTERVAL_H;

	act_led_blink_interval++;

	if(com_timeout > TIMEOUT)
		com_timeout--;

	noise_ad_time++;

	TR0 = 1;
	ET0 = 1;
}

void serial(void) interrupt 4
{
	if(RI)
	{
		RI = 0;
		rxbuf[rx_wr_index++] = SBUF;
		if(rx_wr_index >= RX_BUF_LEN)
			rx_wr_index = 0;
		rx_counter++;
	}
}

unsigned char get_char(unsigned char *pchar)
{	
	if(0 == rx_counter)
		return(0);
	
	*pchar = rxbuf[rx_rd_index++];
	if(rx_rd_index >= RX_BUF_LEN)
		rx_rd_index = 0;
	ES = 0;
	rx_counter--;
	ES = 1;	

	return(1);
}

void put_n_char(unsigned char *buf, unsigned char n)
{
	unsigned char data i;

	com_timeout = 200;
	
	ACT_LED = 0;

	for(i = 0; i < n; i++)
		put_char(buf[i]);

	ACT_LED = 1;   	
}

void put_char(unsigned char c)
{
	SBUF = c;
	while(!TI);
	TI = 0;
}

void hex2ascii(unsigned char *buf, unsigned char hex)
{
	unsigned char tmp, tmp_B, tmp_S, tmp_G;

	tmp_B = hex /100;
	tmp = hex % 100;
	if(tmp_B > 0)
		*buf++ = tmp_B + 0x30;
	else
		*buf++ = 0x20;

	tmp_S = tmp / 10;
	tmp_G = tmp % 10;

	if(tmp_B == 0 && tmp_S == 0)
		*buf++ = 0x20;
	else
		*buf++ = tmp_S + 0x30;

	*buf++ = tmp_G + 0x30;

}

void delay_50us(unsigned int _50us)	// delay 50 us for STC12C5410AD
{
	unsigned int data i;
	unsigned char data j;

	for(i = 0; i < _50us; i++)
	{
		j = 50;
		while(j--);	
	}
}

void device_init(void)
{
	EA = 0;

	P0M0 = 0x01;
	P0M1 = 0x00;

	TMOD = 0x21;					// T1 mode 2, T0 mode 1
	TH0 = T0_INTERVAL_H;			// 10ms interrupt @11.0592MHz, time = (65536 - previous) * period *12
	TL0 = T0_INTERVAL_L;			//
	ET0 = 1;						// enable T0 interrupt
	TR0 = 1;						// start up T0
 
	SCON = 0x50;					// UART mode 1, 8 bit, REN = 1 
	PCON = 0x00;					// SMOD = 0;
	TH1 = 0xfd;						// Baud 9600 @11.0592MHz
	TL1 = 0xfd;
	TR1 = 1;						// start up T1
	ES = 1;							// enable UART interrupt

	// STC12C5620系列单片机上电复位后为准双向口/弱上拉(传统8051的I/O口)模式。
	// 2V以上是为高电平，0.8V以下时为低电平。每个I/O口驱动能力均可达到20mA，
	// 但整个芯片最大不得超过55mA
	// 基于以上描述，VAU模块使用的控制I/O口不需重新配置，按缺省配置即可
	ACT_LED = 0;					// 工作状态指示灯I/O口上电为低电平，工作状态指示灯处于点亮状态
	NOISE_KEY = 1;					// 噪检开关I/O口上电为高电平，I/O口为输入口

	PA_STATE_CTRL = 1;				// 广播状态控制I/O口上电为高电平，处于非广播状态
	PA_STATE_CHK = 1;				// 广播状态检测I/O口上电为高电平，I/O口为输入口

	com_rx_status = COM_RX_HEAD;
	
	EA = 1;	
}

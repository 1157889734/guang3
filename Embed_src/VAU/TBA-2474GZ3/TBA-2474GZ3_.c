#include <stc12c5620ad.h>
#include <intrins.h>
#include "TBA-2474GZ3.h"

#define DEBUG

#define DEBUG_SWITCH			0x10
#define VERSION					0x20

unsigned char data base_volumn = 3;								// 2014-04-02�޸ģ�volume_AGC_valҲ���޸�Ϊvolume_AGC_val[2] = {3, 0};							unsigned char data base_volumn = 6;					// ��������ֵΪ6

// �趨��������ֵ��Χ������ֵ����92������ֵ��������
// ����ֵ����90С��150������Ϊ6
// ����ֵ����150������Ϊ3 + 6 = 9
unsigned char noise_ad_val[2] = {90, 150};
unsigned char volume_AGC_val[2] = {3, 0};						// unsigned char volume_AGC_val[2] = {3, 3};
unsigned char get_AGC_val(unsigned char val);		// ��������A/D����ֵ�����������ֵ

volatile unsigned int data noise_ad_time = 0;		// ��������ʱ�������趨1s���
volatile unsigned char data com_timeout = TIMEOUT;

unsigned char data debug_switch = 0;

void main(void)
{
	device_init();
	
	m62429_init();						// ��������оƬM62429���ݡ�ʱ��I/O�ڳ�ʼ��

	ADC_init();							// A/Dת������ʼ��

	AGC_mode_init();					// ���ģʽ��ʼ��
	
#ifdef	DEBUG
	get_ver_info();	
#endif
	
	while(1)
	{
		PA_ctrl();
		alter_AGC_mode();					// ���AGCģʽ�仯
		AGC();								// AGC����
		prc_rx_data();
		
		if(com_timeout == TIMEOUT)
			act_led_blink(50);					// ����ָʾ�Ƽ��500ms����1�Σ���ʱ�����10ms�ж�һ��
	}
}

void AGC(void)
{
	static unsigned int data sample_10_val[2] ={0, 0};		// 2 A/Dͨ��10�β����ۼ�ֵ
	static unsigned char data sample_times = 0;						// 10�β�������

	static unsigned int noise_sample_val[2] = {0, 0};		// 2 A/Dͨ��ÿ10�β����ۼ�ƽ�������ۼ�
	static unsigned int noise_sample_times;					// 10��ƽ��ֵ�ۼӵĴ���

	unsigned char data i;
	unsigned char data tmp;
	unsigned char tmp_AGC_val;

	unsigned char xdata tmpbuf[32];

	if(now_AGC_mode != AGC_NORMAL)			// �����ǰ����ģʽ����AGC_NORMAL���򲻽��������ڣ�ֱ���˳�
		return;

	// ���������ģʽ�����������仯�Զ������������
	if(noise_ad_time < 100)				// 1s����ʱ���ڲ���ֵ�����ۼӣ�ȡƽ��ֵ
	{
		for(i = 0; i < 2; i++)
 			sample_10_val[i] += ADC_8(i);	// A/Dͨ��0��1����ֵ�ۼ�
		if(++sample_times >= 10)			// ��ͨ��ÿ10�β���������ȡһ��ƽ��ֵ������ƽ��ֵ�ۼӸ���noise_sample_val[2]����
 	    {
			for(i = 0; i < 2; i++)
			{
				tmp = sample_10_val[i] / sample_times;
				noise_sample_val[i] += tmp;

				sample_10_val[i] = 0;
			}

			sample_times = 0;

			noise_sample_times++;				// ��1sʱ����ÿ10�β����ۼ�ֵȡƽ��ֵ�Ĵ���
		}		
	}
	else		// 1s A/D����ʱ�䵽�������������ڴ���			
	{
		for(i = 0; i < 2; i++)
		{
			tmp = noise_sample_val[i] / noise_sample_times;	// 1sʱ���ڲ���noise_sample_times * 10�ε�����ƽ��ֵ
			//tmp_AGC_val = get_incre_val(tmp);
			tmp_AGC_val = get_AGC_val(tmp);
			
			m62429_op(i, (base_volumn - tmp_AGC_val));		// 2ͨ��������������
			
			if(debug_switch & 0x01)
			{
				// ��ӡͨ����
				put_n_char("\r\n\r\n", 4);
				put_n_char("\tChannel ---- ", 14);
				hex2ascii(tmpbuf, (unsigned char)i);
				put_n_char(tmpbuf, 3);
				put_n_char("\r\n\r\n", 4);
				
				// ��ӡA/D����ֵ
				put_n_char("\r\n\r\n", 4);					
				put_n_char("\tA/D sample ", 12);
				hex2ascii(tmpbuf, (unsigned char)tmp);
				put_n_char(tmpbuf, 3);
				put_n_char("\r\n\r\n", 4);

				// ��ӡ��������ֵ
				put_n_char("\tVolume increment ", 18);
				hex2ascii(tmpbuf, (unsigned char)tmp_AGC_val);
				put_n_char(tmpbuf, 3);
				put_n_char("dB\r\n\r\n", 6);

				// ��ӡ��������ֵ
				put_n_char("\tBase volume ", 13);
				hex2ascii(tmpbuf, (unsigned char)base_volumn);
				put_n_char(tmpbuf, 3);
				put_n_char("dB\r\n\r\n", 6);

				// ��ӡ����ֵ
				put_n_char("\tVolume ", 8);
				hex2ascii(tmpbuf, (unsigned char)(base_volumn - tmp_AGC_val));
				put_n_char(tmpbuf, 3);
				put_n_char("dB - M62429 writen\r\n\r\n", 22);
				put_n_char("\r\n\r\n", 4);				
			}
		}
		
		
		/*	2014-12-29�޸ģ������ڵ���1����26�г�������Ŀ�ϸ��ģ�ԭΪֻ����1·�����źţ�ͬʱ������·�������
		 /*	����Ϊ����2·�����źţ��ֱ������·�������
		
		// ����1��A/D����ֵͬʱ������·����
		tmp = noise_sample_val[1] / noise_sample_times;	// 1sʱ���ڲ���noise_sample_times * 10�ε�����ƽ��ֵ
		tmp_AGC_val = get_AGC_val(tmp);
		m62429_op(2, (base_volumn - tmp_AGC_val));		// 2ͨ������ͬʱ����

		if(debug_switch & 0x01)
		{
			// ��ӡA/D����ֵ
			put_n_char("\r\n\r\n", 4);					
			put_n_char("\tA/D sample ", 12);
			hex2ascii(tmpbuf, (unsigned char)tmp);
			put_n_char(tmpbuf, 3);
			put_n_char("\r\n\r\n", 4);

			// ��ӡ��������ֵ
			put_n_char("\tVolume increment ", 18);
			hex2ascii(tmpbuf, (unsigned char)tmp_AGC_val);
			put_n_char(tmpbuf, 3);
			put_n_char("dB\r\n\r\n", 6);

			// ��ӡ��������ֵ
			put_n_char("\tBase volume ", 13);
			hex2ascii(tmpbuf, (unsigned char)base_volumn);
			put_n_char(tmpbuf, 3);
			put_n_char("dB\r\n\r\n", 6);

			// ��ӡ����ֵ
			put_n_char("\tVolume ", 8);
			hex2ascii(tmpbuf, (unsigned char)(base_volumn - tmp_AGC_val));
			put_n_char(tmpbuf, 3);
			put_n_char("dB - M62429 writen\r\n\r\n", 22);

		}
		
		/*************************************************************************************************************************************/
		
		for(i = 0; i < 2; i++)
			noise_sample_val[i] = 0;			// ��������ֵ��ʼ��
		
		noise_sample_times = 0;					// 1s��ÿ10�β���ƽ��ֵ���ۼӴ�����0
		noise_ad_time = 0;						// 1s������ʱ�����0

	}
}

// ��������A/D����ֵ�����������ֵ
unsigned char get_AGC_val(unsigned char val)		
{
	if(val < noise_ad_val[0])	// ����A/D����ֵ����noise_ad_val[1]���������0
		return(0);
	else if((val >= noise_ad_val[0]) && (val < noise_ad_val[1]))
		return(volume_AGC_val[0]);
	else if(val >= noise_ad_val[1])
		return(volume_AGC_val[0] + volume_AGC_val[1]);
	else
		return(0);
}

void alter_AGC_mode(void)		// ��칤��ģʽ�����仯����Ҫ������������
{
	get_AGC_mode();

	if(old_AGC_mode != now_AGC_mode)			// �����칤��ģʽ�����仯��������������������ģ���ʼֵ
	{
		m62429_op(2, base_volumn);				// ������ʼֵΪ-9dB��1��2����ͨ�����ͬʱ����Ϊ-9dB	
		
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
	if(noise_key_chk != tmp_noise_key_chk)	// ���10ms��ȡ2��NOISE_KEY���ŵ�ƽֵ���������ͬ�˳�
		return;

	if(noise_key_chk != 0)
		now_AGC_mode = AGC_NORMAL;
	else
		now_AGC_mode = AGC_BYPASS;
}

void AGC_mode_init(void)
{
	old_AGC_mode = AGC_BYPASS;				// �ϵ����칤��ģʽ���ڷ����ģʽ

	m62429_op(2, base_volumn);				// ������ʼֵΪ-9dB��1��2����ͨ�����ͬʱ����Ϊ-9dB			
}

void PA_ctrl(void)
{
	unsigned char pa_state_chk;
	unsigned char tmp_pa_state_chk;

	tmp_pa_state_chk = PA_STATE_CHK;
	delay_50us(200);
	pa_state_chk = PA_STATE_CHK;
	if(pa_state_chk != tmp_pa_state_chk)	// ���10ms��ȡ2��PA_STATE_CHK���ŵ�ƽֵ���������ͬ�˳�
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
	put_n_char("\r\n\r\n\t�豸����:  ��������ģ��  TBA-2474GZ3\r\n", 43);
	put_n_char("\t��Ŀ����:  ���ݵ���3����B1�ͳ�������Ŀ\r\n", 41);
	put_n_char("\t�汾��Ϣ:  Ver 1.0  2014-12-30\r\n", 33);
	put_n_char("\t������:    �����б��������ͨ�������޹�˾\r\n", 44);
}

unsigned char chksum(unsigned char *buf, unsigned char n)
{
	unsigned char idata i;
	unsigned char idata tmp = 0;

	for(i = 0; i < n; i++)
		tmp += buf[i];

	return(tmp);
}

// �������ڷ�Χ-83dB ~ 0dB��volumn = 0�� �������0dB
//                                 = 83���������-83dB
//                                 >=84������
// op_typeΪͨ��ѡ��volumnΪ����˥��ֵ
void m62429_op(unsigned char op_type, unsigned char volumn)
{
	unsigned char data i;
	unsigned char idata tmp_volumn = 87;
	unsigned int data tmp = 0;

	if(volumn > 87)			// �������˥��ֵ>87����˥��ֵ87����
		volumn = 87;

	tmp_volumn -= volumn;

	tmp |= ((tmp_volumn & 0x03) << 5);
	tmp |= ((tmp_volumn & 0x7c) >> 2);
	tmp <<= 2;

	// D1 = 0: Both channels at a time, 1: A channel at a time
    // D0 = 0: 1CH	1: 2CH
	switch(op_type)
	{
	case 0:						// 1ͨ��������������  
		tmp |= 2;
		break;

	case 1:						// 2ͨ��������������
		tmp |= 3;
		break;

	case 2:						// 1��2ͨ������ͬʱ����
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


void m62429_init(void)				// ��������оƬM62429���ݡ�ʱ��I/O�ڳ�ʼ��
{
	M62429_DATA = 1;				// M62429����I/O���ϵ��ʼ��Ϊ�ߵ�ƽ
	M62429_CLOCK = 0;				// M62429ʱ��I/O���ϵ��ʼ��Ϊ�͵�ƽ
}

unsigned char ADC_8(unsigned char an)
{
	if(an > 7)
		return 0;

	an &= 0x07;				// ѡ��ͨ����

	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | an;	// ��A/Dת������Դ
												// A/Dת����ת���ٶ���Ϊ1080��ʱ������һ��
	ADC_CONTR |= ADC_START;						// ����A/Dת��

	// ����ADC_CONTR���ƼĴ��������ִ�к�Ҫ����4��CPUʱ����ʱ����ֵ���ܱ�֤�����ý�ADC_CONTR���ƼĴ���
	delay_50us(2);								// ��ʱ100us���ȴ�A/Dת������Դ�ȶ�

	while(!(ADC_CONTR & ADC_FLAG));				// �ȴ�A/Dת������
	ADC_CONTR &= 0xE7;							// A/Dת��������־λ��0��һ��Ҫ�������

	return(ADC_DATA);							// ����A/Dת��ֵ
}

void ADC_init(void)
{
	P1M0 = 0xff;				// P1������A/D�����
	P1M1 = 0xff;				// P1������A/D�����

	EADC_SPI = 0;				// ��ֹA/Dת�����жϺ�SPI�ж�
	AUXR &= 0xef;				// AUXR�Ĵ���EADCI = 0����ֹA/Dת���ж�

	ADC_DATA = 0;				// A/Dת��������Ĵ�����0

	ADC_CONTR &= 0x8f;			// A/Dת����ת���ٶ���Ϊ1080��ʱ������һ��
	ADC_CONTR &= 0xf7;			// A/Dת����ת��������־λ��0
	ADC_CONTR |= ADC_POWER;		// ��A/Dת������Դ 

	delay_50us(200);			// ��ʱ10ms���ȴ�A/Dת������Դ�ȶ�
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


void timer0(void) interrupt 1 using 1		// ���10ms�ж�
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

	// STC12C5620ϵ�е�Ƭ���ϵ縴λ��Ϊ׼˫���/������(��ͳ8051��I/O��)ģʽ��
	// 2V������Ϊ�ߵ�ƽ��0.8V����ʱΪ�͵�ƽ��ÿ��I/O�������������ɴﵽ20mA��
	// ������оƬ��󲻵ó���55mA
	// ��������������VAUģ��ʹ�õĿ���I/O�ڲ����������ã���ȱʡ���ü���
	ACT_LED = 0;					// ����״ָ̬ʾ��I/O���ϵ�Ϊ�͵�ƽ������״ָ̬ʾ�ƴ��ڵ���״̬
	NOISE_KEY = 1;					// ��쿪��I/O���ϵ�Ϊ�ߵ�ƽ��I/O��Ϊ�����

	PA_STATE_CTRL = 1;				// �㲥״̬����I/O���ϵ�Ϊ�ߵ�ƽ�����ڷǹ㲥״̬
	PA_STATE_CHK = 1;				// �㲥״̬���I/O���ϵ�Ϊ�ߵ�ƽ��I/O��Ϊ�����

	com_rx_status = COM_RX_HEAD;
	
	EA = 1;	
}

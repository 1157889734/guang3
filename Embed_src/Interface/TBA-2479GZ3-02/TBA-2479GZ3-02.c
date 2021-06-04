#include <stc12c5620ad.h>
#include <intrins.h>
#include "TBA-2479GZ3-02.h"

#define DEBUG

#define DEBUG_SWITCH			0x10
#define VERSION					0x20

unsigned char data base_volumn = 3;								// 2014-04-02修改，volume_AGC_val也需修改为volume_AGC_val[2] = {3, 0};							unsigned char data base_volumn = 6;					// 基础音量值为6

volatile unsigned char data com_timeout = TIMEOUT;

unsigned char data debug_switch = 0;

unsigned char data pa_on_status = 1;							// PA_ON状态标志，缺省为高电平；根据PA_ON引脚状态进行高、低电平切换
unsigned char data pc_on_status = 1;							// PC_ON状态标志，缺省为高电平；根据PC_ON引脚状态进行高、低电平切换
unsigned char data radio_on_status = 1;							// RADIO_ON状态标志，缺省为高电平；根据RADIO_ON引脚状态进行高、低电平切换

unsigned char data pa_on_ctrl_self = 1;							// PA_ON_CTRL引脚动作标志；当PA_ON_CTRL引脚拉低时，设置pa_on_ctrl_self为0；当PA_ON_CTRL引脚抬高时，设置pa_on_ctrl_self为1；缺省为1
unsigned char data pc_on_ctrl_self = 1;							// PC_ON_CTRL引脚动作标志；当PC_ON_CTRL引脚拉低时，设置pc_on_ctrl_self为0；当PA_ON_CTRL引脚抬高时，设置pc_on_ctrl_self为1；缺省为1

void main(void)
{
	device_init();
	
#ifdef	DEBUG
	get_ver_info();	
#endif
	
	while(1)
	{
		chk_bus_level();			// 检测音频总线工作电平
		 
		PA_PC_ctrl();

		prc_rx_data();
		
		if(com_timeout == TIMEOUT)
			act_led_blink(50);					// 工作指示灯间隔500ms亮灭1次，定时器间隔10ms中断一次
	}
}

void PA_PC_ctrl(void)
{
	switch(bus_level)
	{
	case LEVEL_BUS_0V:
		break;

	case LEVEL_BUS_20V:				// 对讲
		PA_PC_SWITCH = BUS_PC_USE;	// 音频总线切换为对讲总线

		PC_STATE_CTRL = 0;			// 对讲状态线拉低，输出对讲状态

		PA_STATE_CTRL = 1;			// 广播状态线抬高，输出广播状态

		break;

	case LEVEL_BUS_39V:				// 广播
		PA_PC_SWITCH = BUS_PA_USE;	// 音频总线切换为广播总线

		PC_STATE_CTRL = 1;			// 对讲状态线抬高，输出对讲状态

		PA_STATE_CTRL = 0;			// 广播状态线拉低，输出广播状态

		break;

	case LEVEL_BUS_51V:				// 空闲
		PA_PC_SWITCH = BUS_PC_USE;	// 音频总线切换为广播总线

		PC_STATE_CTRL = 1;			// 对讲状态线抬高，输出对讲状态

		PA_STATE_CTRL = 1;			// 广播状态线抬高，输出广播状态

		break;

	default:
		break;	 
	}	
}

void chk_bus_level(void)						// 检测音频总线工作电平
{
	unsigned char bus_level_chk;
	unsigned char tmp_bus_level_chk;

	tmp_bus_level_chk = P2 & 0x07;			// 读取P2口低3位值
	delay_50us(200);						// 延时10ms
	bus_level_chk = P2 & 0x07;				// 再次读取P2口低3位值
	if(bus_level_chk != tmp_bus_level_chk)
		return;								// 两次读取值不同，退出函数

	switch(bus_level_chk)					// RADIO_STATE_CHK - P2.2    PC_STATE_CHK - P2.1    PA_STATE_CHK - P2.0
	{
	case 7:			// 51V
		bus_level = LEVEL_BUS_51V;

#ifdef DEBUG
		put_n_char("音频总线电平为51V\r\n", 19);
#endif
		break;

	case 6:			// 39V
		bus_level = LEVEL_BUS_39V;

#ifdef DEBUG
		put_n_char("音频总线电平为39V\r\n", 19);
#endif
		break;

	case 4:			// 20V
		bus_level = LEVEL_BUS_20V;

#ifdef DEBUG
		put_n_char("音频总线电平为20V\r\n", 19);
#endif
		break;

	case 0:			// 0V
		bus_level = LEVEL_BUS_0V;

#ifdef DEBUG
		put_n_char("音频总线电平为0V\r\n", 18);
#endif
		break;

	default:
		bus_level = 0xff;

#ifdef DEBUG
		put_n_char("音频总线电平为未知\r\n", 20);
#endif
		break;		
	}
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
	put_n_char("\r\n\r\n\t设备名称:  音量调节模块  TBA-2474SZ1-26\r\n", 46);
	put_n_char("\t项目名称:  深圳地铁1号线26列车改造\r\n", 37);
	put_n_char("\t版本信息:  Ver 1.1  2014-04-04\r\n", 33);
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

	// P0端口设置
	P0M0 = 0x00;
	P0M1 = 0x08;					// P0.3设为强推挽

	PA_ON_CTRL = 1;					// 广播状态线控制I/O口上电为高电平，I/O口为输出口；广播音频变压器中心抽头电压为51V
	PC_ON_CTRL = 1;					// 对讲状态线控制I/O口上电为高电平，I/O口为输出口；广播音频变压器中心抽头电压为51V
	RADIO_ON_CTRL = 1;				// 无线广播状态控制I/O口上电为高电平，I/O口为输出口；广播音频变压器中心抽头电压为51V
	PA_PC_SWITCH = BUS_PC_USE;				// 广播、对讲切换控制I/O口上电为低电平，I/O口为输出口；缺省时音频总线作对讲总线使用

	// P2端口设置
	PA_STATE_CHK = 1;				// 总线广播状态检测I/O口上电为高电平，I/O口为输入口
	PC_STATE_CHK = 1;				// 总线对讲状态检测I/O口上电为高电平，I/O口为输入口
	RADIO_STATE_CHK = 1;			// 总线无线广播状态检测I/O口上电为高电平，I/O口为输入口
	PA_ON_CHK = 1;					// 内部广播控制检测I/O口上电为高电平，I/O口为输入口
	PC_ON_CHK = 1;					// 内部对讲控制检测I/O口上电为高电平，I/O口为输入口
	PADIO_CTRL_CHK = 1;				// 无线广播状态检测I/O口上电为高电平，I/O口为输入口
	PA_STATE_CTRL = 1;				// 内部广播状态控制I/O上电为高电平，I/O口为输出口；缺省为高电平 
	PC_STATE_CTRL = 1;				// 内部对讲状态控制I/O上电为高电平，I/O口为输出口；缺省为高电平

	com_rx_status = COM_RX_HEAD;

	bus_level = LEVEL_BUS_51V;		// 初始化总线电平为51V
	
	EA = 1;	
}

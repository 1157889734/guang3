#include <reg52.h>
#include <intrins.h>
#include "TBA-2476SZ1-26.h"

#define DEBUG

#define DEBUG_SWITCH				0x10
#define VERSION						0x20

#define IDLE						0x00
#define PA_2_LINK_PA_SUCCESS		0x01
#define PA_2_LINK_PA_FAIL			0x02
#define PC_2_LINK_PC_SUCCESS		0x03
#define PC_2_LINK_PC_FAIL			0x04
#define LINK_PA_2_PA_SUCCESS		0x05
#define LINK_PA_2_PA_FAIL			0x06
#define LINK_PC_2_PC_SUCCESS		0x07
#define LINK_PC_2_PC_FAIL			0x08

unsigned char work_status;

volatile unsigned char data com_timeout = TIMEOUT;

unsigned char data debug_switch = 0;

void debug_printf(unsigned char *buf, unsigned char n);

unsigned char pa_ctrl_op_flag;
unsigned char pc_ctrl_op_flag;
unsigned char link_pa_ctrl_op_flag;
unsigned char link_pc_ctrl_op_flag;

void pa_ctrl(unsigned char on_off);				 
void pc_ctrl(unsigned char on_off);				// 对讲控制
void link_pa_ctrl(unsigned char on_off);		// 重连广播控制
void link_pc_ctrl(unsigned char on_off);		// 重连对讲控制

void pa_2_link_pa(void);
void pc_2_link_pc(void);
void link_pa_2_pa(void);
void link_pc_2_pc(void);

unsigned char pa_state_chk(void);
unsigned char pc_state_chk(void);
unsigned char link_pa_state_chk(void);
unsigned char link_pc_state_chk(void);

void main(void)
{
	device_init();
	
#ifdef	DEBUG
	get_ver_info();	
#endif
	
	while(1)
	{
		work_led_blink(50);					// 工作指示灯间隔500ms亮灭1次，定时器间隔10ms中断一次

		pa_2_link_pa();
		pc_2_link_pc();
		link_pa_2_pa();
		link_pc_2_pc();

		prc_rx_data();		
	}
}

void pa_2_link_pa(void)
{
	if(pa_state_chk() == 1)							// 检测到PA总线电平拉低，PA_STATE_CHK = 0
	{
		debug_printf("PA_STATE_CHK = 0\r\n", 18);
		
		PA_LED = ON;									// 广播指示灯点亮

		if(work_status == PA_2_LINK_PA_SUCCESS)
			return;

		if((link_pa_state_chk() == 0) && (link_pc_state_chk() == 0))	// 重连广播、对讲状态都没有拉低
		{
			debug_printf("PA_AUDIO_IN = ON\r\n", 18);
			PA_AUDIO_IN = ON;						// 广播输入打开
						
			debug_printf("PA_AUDIO_OUT = OFF\r\n", 20);
			PA_AUDIO_OUT = OFF;						// 广播输出关闭
						
			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭
					
			debug_printf("LINK_AUDIO_OUT = ON\r\n", 21);
			LINK_AUDIO_OUT = ON;					// 重连音频输出打开
			
			debug_printf("LINK_PA_CTRL = ON\r\n", 19);
			link_pa_ctrl(ON);						// 拉低重连广播状态线
			delay_1ms(50);							// 延时50ms使重连广播状态稳定
			
			LINK_PA_LED = ON;						// 重连广播指示灯点亮

			debug_printf("work_status = PA_2_LINK_PA_SUCCESS\r\n\r\n", 38);
			work_status = PA_2_LINK_PA_SUCCESS;				// 工作状态处于广播到重连广播
		}
		else if(link_pa_state_chk() == 1)			// 重连广播状态为低，则重连广播指示灯间隔500ms闪亮
		{
			debug_printf("LINK_PA_STATE_CHK = 0\r\n", 23);
			link_pa_led_blink(200);
			debug_printf("work_status = PA_2_LINK_PA_FAIL\r\n\r\n", 35);
			work_status = PA_2_LINK_PA_FAIL;
		}
		else if(link_pc_state_chk() != 0)			// 重连对讲状态为低，则重连广播指示灯间隔1000ms闪亮
		{
			debug_printf("LINK_PC_STATE_CHK = 0/0xFF\r\n", 28);
			link_pa_led_blink(50);
			debug_printf("work_status = PA_2_LINK_PA_FAIL\r\n\r\n", 35);
			work_status = PA_2_LINK_PA_FAIL;
		}
	}
	else
	{
		if(work_status == PA_2_LINK_PA_SUCCESS)
		{
			debug_printf("PA_STATE_CHK = 1\r\n", 18);
			debug_printf("work_status = PA_2_LINK_PA_SUCCESS\r\n\r\n", 38);
			
			PA_LED = OFF;							// 广播指示灯熄灭

			debug_printf("PA_AUDIO_IN = OFF\r\n", 19);
			PA_AUDIO_IN = OFF;						// 广播输入关闭

			debug_printf("PA_AUDIO_OUT = OFF\r\n", 20);
			PA_AUDIO_OUT = OFF;						// 广播输出关闭

			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭
			
			debug_printf("LINK_PA_CTRL = OFF\r\n", 20);
			link_pa_ctrl(OFF);						// 抬高重连广播状态线
			delay_1ms(50);							// 延时50ms使重连广播状态稳定

			LINK_PA_LED = OFF;						// 重连广播指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == PA_2_LINK_PA_FAIL)
		{
			PA_LED = OFF;							// 广播指示灯熄灭
			LINK_PA_LED = OFF;						// 重连广播指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
	}		
}

void pc_2_link_pc(void)
{
	if(pc_state_chk() == 1)							// 检测到PC总线电平拉低，PC_STATE_CHK = 0
	{
		debug_printf("PC_STATE_CHK = 0\r\n", 18);

		PC_LED = ON;									// 对讲指示灯点亮

		if(work_status == PC_2_LINK_PC_SUCCESS)
			return;

		if((link_pc_state_chk() == 0) && (link_pa_state_chk() == 0))	// 重连对讲、广播状态都没有拉低
		{
			debug_printf("PC_AUDIO_IN = ON\r\n", 18);
			PC_AUDIO_IN = ON;						// 广播输入打开

			debug_printf("PC_AUDIO_OUT = OFF\r\n", 20);
			PC_AUDIO_OUT = OFF;						// 广播输出关闭

			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭

			debug_printf("LINK_AUDIO_OUT = ON\r\n", 21);
			LINK_AUDIO_OUT = ON;					// 重连音频输出打开

			debug_printf("LINK_PA_CTRL = ON\r\n", 19);
			link_pc_ctrl(ON);						// 拉低重连对讲状态线
			delay_1ms(50);							// 延时50ms使重连对讲状态稳定

			LINK_PC_LED = ON;						// 重连对讲指示灯点亮

			debug_printf("work_status = PC_2_LINK_PC_SUCCESS\r\n\r\n", 38);
			work_status = PC_2_LINK_PC_SUCCESS;				// 工作状态处于对讲到重连对讲
		}
		else if(link_pc_state_chk() == 1)			// 重连对讲状态为低，则重连对讲指示灯间隔500ms闪亮
		{
			debug_printf("LINK_PC_STATE_CHK = 0\r\n", 23);
			link_pc_led_blink(200);
			debug_printf("work_status = PC_2_LINK_PC_FAIL\r\n\r\n", 35);
			work_status = PC_2_LINK_PC_FAIL;
		}
		else if(link_pa_state_chk() != 0)			// 重连广播状态为低，则重连广播指示灯间隔1000ms闪亮
		{
			debug_printf("LINK_PA_STATE_CHK = 0/0xFF\r\n", 28);
			link_pc_led_blink(50);
			debug_printf("work_status = PC_2_LINK_PC_FAIL\r\n\r\n", 35);
			work_status = PC_2_LINK_PC_FAIL;
		}
	}
	else
	{
		if(work_status == PC_2_LINK_PC_SUCCESS)
		{
			debug_printf("PC_STATE_CHK = 1\r\n", 18);
			debug_printf("work_status = PC_2_LINK_PC_SUCCESS\r\n\r\n", 38);\

			PC_LED = OFF;							// 对讲指示灯熄灭

			debug_printf("PC_AUDIO_IN = OFF\r\n", 19);
			PC_AUDIO_IN = OFF;						// 对讲输入关闭

			debug_printf("PC_AUDIO_OUT = OFF\r\n", 20);
			PC_AUDIO_OUT = OFF;						// 对讲输出关闭

			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭

			debug_printf("LINK_PC_CTRL = OFF\r\n", 20);
			link_pc_ctrl(OFF);						// 抬高重连对讲状态线
			delay_1ms(50);							// 延时50ms使重连对讲状态稳定

			LINK_PC_LED = OFF;						// 重连对讲指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == PC_2_LINK_PC_FAIL)
		{
			PC_LED = OFF;							// 对讲指示灯熄灭

			LINK_PC_LED = OFF;						// 重连对讲指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
	}		
}

void link_pa_2_pa(void)
{
	if(link_pa_state_chk() == 1)							// 检测到重连PA总线电平拉低，LINK_PA_STATE_CHK = 0
	{
		debug_printf("LINK_PA_STATE_CHK = 0\r\n", 23);
		
		if(work_status == LINK_PA_2_PA_SUCCESS)
			return;
		else if(work_status == LINK_PC_2_PC_SUCCESS)
		{
			pa_led_blink(50);
			link_pa_led_blink(50);
			return;
		}

		LINK_PA_LED = ON;									// 广播指示灯点亮

		if(pa_state_chk() == 0)								// 广播状态为高
		{
			debug_printf("PA_STATE_CHK = 1\r\n", 18);
			
			debug_printf("PA_AUDIO_IN = OFF\r\n", 19);
			PA_AUDIO_IN = OFF;						// 广播输入关闭

			debug_printf("PA_AUDIO_OUT = ON\r\n", 19);
			PA_AUDIO_OUT = ON;						// 广播输出打开

			debug_printf("LINK_AUDIO_IN = ON\r\n", 20);
			LINK_AUDIO_IN = ON;						// 重连音频输出打开

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭

			debug_printf("PA_CTRL = ON\r\n", 14);
			pa_ctrl(ON);							// 拉低广播状态线
			delay_1ms(50);							// 延时50ms使广播状态稳定

			PA_LED = ON;						// 重连对讲指示灯点亮

			debug_printf("work_status = LINK_PA_2_PA_SUCCESS\r\n\r\n", 38);
			work_status = LINK_PA_2_PA_SUCCESS;				// 工作状态处于重连广播到广播
		}
		else 									// 广播状态为低，则广播指示灯间隔500ms闪亮
		{
			debug_printf("PA_STATE_CHK = 0/0xFF\r\n", 23);
			pa_led_blink(200);
			debug_printf("work_status = LINK_PA_2_PA_FAIL\r\n\r\n", 35);
			work_status = LINK_PA_2_PA_FAIL;
		}			
	}
	else
	{
		if(work_status == LINK_PA_2_PA_SUCCESS)
		{
			debug_printf("LINK_PA_STATE_CHK = 1\r\n", 23);
			debug_printf("work_status = LINK_PA_2_PA_SUCCESS\r\n", 36);
			
			LINK_PA_LED = OFF;						// 重连广播指示灯熄灭
			
			debug_printf("PA_AUDIO_IN = OFF\r\n", 19);
			PA_AUDIO_IN = OFF;						// 广播输入关闭

			debug_printf("PA_AUDIO_OUT = OFF\r\n", 20);
			PA_AUDIO_OUT = OFF;						// 广播输出关闭

			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭

			debug_printf("PA_CTRL = OFF\r\n", 15);
			pa_ctrl(OFF);							// 抬高广播状态线
			delay_1ms(50);							// 延时50ms使广播状态稳定

			PA_LED = OFF;							// 广播指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == LINK_PA_2_PA_FAIL)
		{
			LINK_PA_LED = OFF;						// 重连广播指示灯熄灭

			PA_LED = OFF;							// 广播指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == LINK_PC_2_PC_SUCCESS)
		{
			LINK_PA_LED = OFF;						// 重连广播指示灯熄灭

			PA_LED = OFF;							// 广播指示灯熄灭
		}
	}		
}

void link_pc_2_pc(void)
{
	if(link_pc_state_chk() == 1)							// 检测到重连PC总线电平拉低，LINK_PC_STATE_CHK = 0
	{
		debug_printf("LINK_PC_STATE_CHK = 0\r\n", 23);

		if(work_status == LINK_PC_2_PC_SUCCESS)
			return;
		else if(work_status == LINK_PA_2_PA_SUCCESS)
		{
			pc_led_blink(50);
			link_pc_led_blink(50);
			return;
		}

		LINK_PC_LED = ON;									// 重连对讲指示灯点亮

		if(pc_state_chk() == 0)								// 对讲状态为高
		{
			debug_printf("PC_STATE_CHK = 1\r\n", 18);
			
			debug_printf("PC_AUDIO_IN = OFF\r\n", 19);
			PC_AUDIO_IN = OFF;						// 对讲输入关闭

			debug_printf("PC_AUDIO_OUT = ON\r\n", 19);
			PC_AUDIO_OUT = ON;						// 对讲输出打开

			debug_printf("LINK_AUDIO_IN = ON\r\n", 20);
			LINK_AUDIO_IN = ON;						// 重连音频输出打开

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭

			debug_printf("PC_CTRL = ON\r\n", 14);
			pc_ctrl(ON);							// 拉低对讲状态线
			delay_1ms(50);							// 延时50ms使对讲状态稳定

			PC_LED = ON;							// 对讲指示灯点亮

			debug_printf("work_status = LINK_PC_2_PC_SUCCESS\r\n\r\n", 38);
			work_status = LINK_PC_2_PC_SUCCESS;				// 工作状态处于重连对讲到对讲
		}
		else 									// 对讲状态为低，则对讲指示灯间隔500ms闪亮
		{
			debug_printf("PC_STATE_CHK = 0/0xFF\r\n", 23);
			pc_led_blink(200);
			debug_printf("work_status = LINK_PC_2_PC_FAIL\r\n\r\n", 35);
			work_status = LINK_PC_2_PC_FAIL;
		}		
	}
	else
	{
		if(work_status == LINK_PC_2_PC_SUCCESS)
		{
			debug_printf("LINK_PC_STATE_CHK = 1\r\n", 23);
			debug_printf("work_status = LINK_PC_2_PC_SUCCESS\r\n", 36);

			LINK_PC_LED = OFF;						// 重连对讲指示灯熄灭
			
			debug_printf("PC_AUDIO_IN = OFF\r\n", 19);
			PC_AUDIO_IN = OFF;						// 对讲输入关闭

			debug_printf("PC_AUDIO_OUT = OFF\r\n", 20);
			PC_AUDIO_OUT = OFF;					// 对讲输出关闭

			debug_printf("LINK_AUDIO_IN = OFF\r\n", 21);
			LINK_AUDIO_IN = OFF;					// 重连音频输出关闭

			debug_printf("LINK_AUDIO_OUT = OFF\r\n", 22);
			LINK_AUDIO_OUT = OFF;					// 重连音频输出关闭

			debug_printf("PC_CTRL = OFF\r\n", 15);
			pc_ctrl(OFF);							// 抬高对讲状态线
			delay_1ms(50);							// 延时50ms使讲状状态稳定

			PC_LED = OFF;							// 对讲指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == LINK_PC_2_PC_FAIL)
		{
			LINK_PC_LED = OFF;						// 重连广播指示灯熄灭

			PC_LED = OFF;							// 广播指示灯熄灭

			debug_printf("work_status = IDLE\r\n\r\n\r\n", 24);
			work_status = IDLE;
		}
		else if(work_status == LINK_PA_2_PA_SUCCESS)
		{
			LINK_PC_LED = OFF;						// 重连广播指示灯熄灭

			PC_LED = OFF;							// 广播指示灯熄灭
		}
	}		
}

// 广播状态检测
unsigned char pa_state_chk(void)
{
	if(!PA_STATE_CHK)				// 广播状态I/O引脚PA_STATE_CHK电平为0
	{
		if(pa_ctrl_op_flag == 0)	// 广播控制操作标志为0，说明不是自己将PA_STATE_CHK状态拉低
			return(1);
		else
			return(0xff);
	}
	else
		return(0);
}

// 对讲状态检测
unsigned char pc_state_chk(void)
{
	if(!PC_STATE_CHK)		// 对讲状态I/O引脚PC_STATE_CHK电平为0
	{
		if(pc_ctrl_op_flag == 0) // 对讲控制操作标志为0，说明不是自己将PC_STATE_CHK状态拉低
			return(1);	
		else
			return(0xff);
	}
	else
		return(0);
}

// 重连广播状态检测
unsigned char link_pa_state_chk(void)
{
	if(!LINK_PA_STATE_CHK)			// 重连广播状态I/O引脚PC_STATE_CHK电平为0
	{
		if(link_pa_ctrl_op_flag == 0) 	// 重连广播控制操作标志为0，说明不是自己将LINK_PA_STATE_CHK状态拉低
			return(1);
		else
			return(0xff);
	}
	else
		return(0);
}

// 重连对讲状态检测
unsigned char link_pc_state_chk(void)
{
	if(!LINK_PC_STATE_CHK)			// 重连对讲状态I/O引脚PC_STATE_CHK电平为0
	{
		if(link_pc_ctrl_op_flag == 0) 	// 重连对讲控制操作标志为0，说明不是自己将LINK_PA_STATE_CHK状态拉低
			return(1);
		else
			return(0xff);
	}
	else
		return(0);
}

// 广播控制 on  - 拉低控制电平
//          off - 抬高控制电平
void pa_ctrl(unsigned char on_off)
{
	if(on_off == ON)
	{
		PA_CTRL = 0;
		pa_ctrl_op_flag = 1;			// 广播控制I/O拉低，广播控制操作标志置1
	}
	else
	{
		PA_CTRL = 1;
		pa_ctrl_op_flag = 0;
	}
}

// 对讲控制 on  - 拉低控制电平
//          off - 抬高控制电平
void pc_ctrl(unsigned char on_off)
{
	if(on_off == ON)
	{
		PC_CTRL = 0;
		pc_ctrl_op_flag = 1;			// 对讲控制操作标志置1
	}
	else
	{
		PC_CTRL = 1;
		pc_ctrl_op_flag = 0;
	}
}

// 重连广播控制 on  - 拉低控制电平
//              off - 抬高控制电平
void link_pa_ctrl(unsigned char on_off)
{
	if(on_off == ON)
	{
		LINK_PA_CTRL = 0;
		link_pa_ctrl_op_flag = 1;		// 重连广播控制操作标志置1
	}
	else
	{
		LINK_PA_CTRL = 1;
		link_pa_ctrl_op_flag = 0;
	}
}

// 重连对讲控制 on  - 拉低控制电平
//              off - 抬高控制电平
void link_pc_ctrl(unsigned char on_off)
{
	if(on_off == ON)
	{
		LINK_PC_CTRL = 0;
		link_pc_ctrl_op_flag = 1;		// 重连对讲控制操作标志置1
	}
	else
	{
		LINK_PC_CTRL = 1;
		link_pc_ctrl_op_flag = 0;
	}
}

void work_led_blink(unsigned char interval)
{
	if(work_led_blink_interval >= interval)
	{
		WORK_LED = ~WORK_LED;

		work_led_blink_interval = 0;		
	}
}

void pa_led_blink(unsigned char interval)
{
	if(pa_led_blink_interval >= interval)
	{
		PA_LED = ~PA_LED;
		pa_led_blink_interval = 0;		
	}	
}

void pc_led_blink(unsigned char interval)
{
	if(pc_led_blink_interval >= interval)
	{
		PC_LED = ~PC_LED;
		pc_led_blink_interval = 0;		
	}
}

void link_pa_led_blink(unsigned char interval)
{
	if(link_pa_led_blink_interval >= interval)
	{
		LINK_PA_LED = ~LINK_PA_LED;
		link_pa_led_blink_interval = 0;		
	}
}

void link_pc_led_blink(unsigned char interval)
{
	if(link_pc_led_blink_interval >= interval)
	{
		LINK_PC_LED = ~LINK_PC_LED;
		link_pc_led_blink_interval = 0;		
	}
}

void get_ver_info(void)
{
	put_n_char("\r\n\r\n\t设备名称:  重连模块  TBA-2476SZ1-26\r\n", 42);
	put_n_char("\t项目名称:  深圳地铁1号线26列车改造\r\n", 37);
	put_n_char("\t版本信息:  Ver 1.0  2014-01-10\r\n", 33);
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

void debug_printf(unsigned char *buf, unsigned char n)
{
	if(debug_switch)
		put_n_char(buf, n);
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

	work_led_blink_interval++;
	pa_led_blink_interval++;
	pc_led_blink_interval++;
	link_pa_led_blink_interval++;
	link_pc_led_blink_interval++;

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
	
//	WORK_LED = 0;

	for(i = 0; i < n; i++)
		put_char(buf[i]);

//	WORK_LED = 1;   	
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

void delay_1ms(unsigned int _1ms)
{
	RCAP2LH = 0xfc66;					// 1ms @11.0592MHz
	T2LH = 0xfc66;

	ET2 = 0;							// disable timer2 interrupt
	T2CON = 0x04;						// 16-bit auto-reload, clear TF2, start timer 2 TR2 = 1
	while(_1ms--)
	{
		while(!TF2);
		TF2 = 0;
	}

	TR2 = 0;
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

	WORK_LED = 0;					// 工作状态指示灯I/O口上电为低电平，工作状态指示灯处于点亮状态
									
	PA_STATE_CHK = 1;				// 广播状态检测I/O口上电为高电平，I/O口为输入口
	PC_STATE_CHK = 1;				// 对讲状态检测I/O口上电为高电平，I/O口为输入口
	LINK_PA_STATE_CHK = 1;			// 重连广播状态检测I/O口上电为高电平，I/O口为输入口
	LINK_PC_STATE_CHK = 1;			// 重连对讲状态检测I/O口上电为高电平，I/O口为输入口

	PA_LED = 1;						// 广播LED指示灯熄灭
	PC_LED = 1;						// 对讲LED指示灯熄灭
	LINK_PA_LED = 1;				// 重连广播LED指示灯熄灭
	LINK_PC_LED = 1;				// 重连对讲LED指示灯熄灭

	PA_AUDIO_OUT = OFF;				// 广播音频输出关闭
	PA_AUDIO_IN = OFF;				// 广播音频输入关闭	
	
	PC_AUDIO_OUT = OFF;				// 对讲音频输出关闭
	PC_AUDIO_IN = OFF;				// 对讲音频输入关闭

	LINK_AUDIO_OUT = OFF;			// 广播音频输出关闭
	LINK_AUDIO_IN = OFF;			// 广播音频输入关闭

	pa_ctrl(OFF);					// PA_CTRL = OFF;					// 广播控制为高电平，关闭
	pc_ctrl(OFF);					// PC_CTRL = OFF;					// 对讲控制为高电平，关闭

	link_pa_ctrl(OFF);				// LINK_PA_CTRL = OFF;				// 重连广播控制为高电平，关闭
	link_pc_ctrl(OFF);				// LINK_PC_CTRL = OFF;				// 重连对讲控制为高电平，关闭

	work_status = IDLE;				// 上电缺省工作状态处于空闲状态

	com_rx_status = COM_RX_HEAD;
	
	EA = 1;	
}

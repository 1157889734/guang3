#include "PECU.h"
#include "uart.h"
#include "DrvGPIO.h"

#include "playPCM.h"

vu16 alarm_led_flash_interval = 0;
vu16 busy_led_flash_interval = 0;
vu16 com_led_flash_interval = 0;

vu8 com_delay_time = 0;

#define RX_BUFFER_LEN			270
#define TX_BUFFER_LEN			64
uint8_t rx_buffer[RX_BUFFER_LEN];
uint8_t tx_buffer[TX_BUFFER_LEN];

#define PROCEE_DATA_LEN			16
uint8_t process_data_buffer[PROCEE_DATA_LEN];

static uint8_t rx_status = COM_RX_HEAD;

static uint8_t process_pecu_status = IDLE;				// 发送过程数据中紧急报警器状态，原先发送pecu_status，但紧急报警器忙时也发送pecu_status会出现错误，增加过程数据process_pecu_status
static uint8_t pecu_status = PECU_IDLE;			// 紧急报警器状态
static uint8_t lcu_cmd_type = LCU_CMD_IDLE;		// 本地控制单元命令

uint8_t pecu_eqnum;							// 紧急报警器设备号
static uint8_t lcu_eqnum;					// 本地控制单元设备号

vu16 pecu_com_timeout = 0;				// 紧急报警器通信超时即时，每收到一包发给本紧急报警器正确数据清0
vu16 alarm_timeout = 0;					// 报警超时，报警按键按下后，如果2分钟没有接通则自动复位
vu16 ptt_timeout = 0;					// 报警接通后，如果3分钟内PTT键没有按下，则紧急报警器自动复位

uint8_t playback_over_flag = 0;			// “忙”提示语音循环播放完毕标志
vu16 playback_time = 0;					// “忙”提示语音循环播放间隔

static void alarm_led(uint8_t on_off);
static void busy_led(uint8_t on_off);
static void listen_led(uint8_t on_off);
static void speak_led(uint8_t on_off);

static void mic_output(uint8_t on_off);
static void spk_input(uint8_t on_off);
static void LM4871_shutdown(uint8_t on_off);

static void alarm_led_flash(uint16_t flash_interval);
static void busy_led_flash(uint16_t flash_interval);
static void com_led_flash(uint16_t flash_interval);
static void prc_rx_data(void);
static uint8_t prase_rx_data(uint16_t len);
static void send_process_packet(void);
static void send_data(uint8_t *buf, uint8_t n);
static uint8_t chksum(uint8_t *buf, uint16_t n);
static void pecu_run_status(void);
static uint8_t chk_alarm_key(void);
static uint8_t chk_ptt_key(void);
//static uint8_t chk_reset_key(void);					// 2014-09-18修改，for南宁地铁1号线项目
static void noise_ctrl_sel(uint8_t on_off);				// 2014-09-18修改，for南宁地铁1号线项目，噪检功能选择及报警开关量输出，
														// 报警键按下，关闭噪检功能，报警断开开关量闭合，报警结束，打开噪检功能，报警开关量

void PECU_run(void)
{
	if(pecu_com_timeout >= PECU_COM_TIMEOUT)
		com_led_flash(100);

	prc_rx_data(); 	

	pecu_run_status();
}

static void pecu_run_status(void)
{
	switch(pecu_status)
	{
	case PECU_IDLE:							// 空闲状态
		mic_output(OFF);				    // 关闭MIC音频输出
		spk_input(OFF);						// 关闭SPK音频输入
		LM4871_shutdown(ON);				// 静音

		noise_ctrl_sel(ON);					// 空闲时即报警键没有被按下，选择噪检功能，报警开关量断开；2014-09-18修改，for南宁地铁1号线

		alarm_led(OFF);						// 关闭所有对讲状态指示灯
		busy_led(OFF);
		listen_led(OFF);
		speak_led(OFF);		

		if(chk_alarm_key())
		{
			noise_ctrl_sel(OFF);					// 报警键被按下，选择噪检关闭，报警开关量闭合；2014-09-18修改，for南宁地铁1号线

			pecu_status = PECU_ALARM;		// 检测报警按键按下，状态切换为报警状态
			alarm_timeout = 0;				// 初始化报警超时计时器	
		}
		
		process_pecu_status = IDLE;			// 紧急报警器工作于IDLE状态，过程数据process_pecu_status为IDLE = 0x00		
		
		break;

	case PECU_ALARM:
		alarm_led_flash(50);				// 报警状态，报警指示灯间隔1s闪亮
		busy_led_flash(50);					// 2014-09-18修改，for南宁地铁1号线，原占线指示灯变更为呼叫指示灯，与alarm_led_flash同步

		//noise_ctrl_sel(OFF);					// 报警键被按下，选择噪检关闭，报警开关量闭合；2014-09-18修改，for南宁地铁1号线

		if(LCU_CMD_BUSY == lcu_cmd_type)	// 收到占线状态，占线指示灯间隔1s闪亮
		{
			LM4871_shutdown(OFF);			// 功放打开
			playback_start();				// 播放占线提示音

			playback_over_flag = 0;			// “忙”提示语音循环播放完毕标志清0
			playback_time = 0;				// “忙”提示语音循环播放间隔清0			

			pecu_status = PECU_BUSY;
		}
		else if(LCU_CMD_ACTIVE == lcu_cmd_type)	// 接通命令，紧急报警器进入接通状态
		{
			alarm_led(ON);
			// busy_led(OFF);					// 2014-09-18修改，for南宁地铁1号线，原占线指示灯变更为呼叫指示灯，与alarm_led_flash同步
			busy_led(ON);

			speak_led(ON);

			ptt_timeout = 0;				// PTT按键按下，PTT超时计时器清零

			pecu_status = PECU_ACTIVED;	
		}
		else if(LCU_CMD_HANGUP == lcu_cmd_type)	// 挂断命令
			pecu_status = PECU_IDLE;
		
			
		if(alarm_timeout >= ALARM_TIMEOUT)		// 报警按键按下后3分钟没有接通，则自动进入IDLE状态
			pecu_status = PECU_IDLE;

	//	if(chk_reset_key() == 1) 				// 报警器复位按键按下进入空闲状态
	//		pecu_status = PECU_IDLE;

		process_pecu_status = ALARM;			// 紧急报警器工作于ALARM状态，过程数据process_pecu_status为ALARM = 0x01
		
		break;

	case PECU_BUSY:							// 其他报警器占线
		alarm_led_flash(50);				// 报警状态，报警指示灯间隔0.5s闪亮
		// busy_led_flash(100);				// 占线指示灯间隔1s闪亮				// 2014-09-18修改，for南宁地铁1号线，原占线指示灯变更为呼叫指示灯，与alarm_led_flash同步
		busy_led_flash(50);				// 占线指示灯间隔1s闪亮

		if((1 == playback_over_flag) &&	(playback_time >= 1000))		// 如果循环播放一次完毕，将下一次播放间隔清0
		{
			playback_over_flag = 0;			// 循环播放一次完毕标志清0
			playback_time = 0;

			playback_start();
		}
		
		if(LCU_CMD_ACTIVE == lcu_cmd_type)	// 接通命令，紧急报警器进入接通状态
		{
			alarm_led(ON);
			// busy_led(OFF);				// 2014-09-18修改，for南宁地铁1号线，原占线指示灯变更为呼叫指示灯，与alarm_led_flash同步
			busy_led(ON);

			speak_led(ON);

			playback_stop();				// 关闭占线提示音
			
			ptt_timeout = 0;				// PTT按键按下，PTT超时计时器清零
			
			pecu_status = PECU_ACTIVED;	
		}
		else if(LCU_CMD_HANGUP == lcu_cmd_type)	// 挂断命令
		{
			playback_stop();				// 关闭占线提示音

			pecu_status = PECU_IDLE;
		}
		else if(LCU_CMD_IDLE == lcu_cmd_type)	// 报警器处于忙状态，此时收到空闲命令，则进入报警状态
		{
			playback_stop();
			// busy_led(OFF);				// 2014-09-18修改，for南宁地铁1号线，原占线指示灯变更为呼叫指示灯，与alarm_led_flash同步
			LM4871_shutdown(ON);

			pecu_status = PECU_ALARM;
		}
			
		if(alarm_timeout >= ALARM_TIMEOUT)		// 报警按键按下后3分钟没有接通，则自动进入IDLE状态
		{
			playback_stop();				// 关闭占线提示音

			pecu_status = PECU_IDLE;
		}
		
		//if(chk_reset_key() == 1) 				// 报警器复位按键按下进入空闲状态
		//{
		//	playback_stop();				// 关闭占线提示音

		//	pecu_status = PECU_IDLE;
		//}	
		
		process_pecu_status = ALARM;			// 紧急报警器工作于BUSY状态，过程数据process_pecu_status应该仍为ALARM = 0x01			
			
		break; 

	case PECU_ACTIVED:
		mic_output(ON);				    // 打开MIC音频输出
		//spk_input(ON);					// 打开SPK音频输入
		//LM4871_shutdown(OFF);			// 2014-10-08修改			// 功放进入工作状态					

		if(chk_ptt_key() == 1)				// 检测PTT按键是否按下
		{
			listen_led(ON);
			speak_led(OFF);

		//	mic_output(OFF);				    // 关闭MIC音频输出
			LM4871_shutdown(OFF);			// 2014-10-08修改			// 功放进入工作状态
			spk_input(ON);

			ptt_timeout = 0;				// PTT按键按下，PTT超时计时器清零
		}
		else if(chk_ptt_key() == 0)			// 检测PTT按键是否抬起			
		{
			listen_led(OFF);
			speak_led(ON);

		//	mic_output(ON);				    // 关闭MIC音频输出
			LM4871_shutdown(ON);			// 2014-10-08修改			// 功放进入静音状态
			spk_input(OFF);
		}

		if(LCU_CMD_HANGUP == lcu_cmd_type)
			pecu_status = PECU_IDLE;
			
		if(ptt_timeout >= PTT_TIMEOUT)		// 报警接通后3分钟没有PTT按下，则自动进入IDLE状态
			pecu_status = PECU_IDLE;

		//if(chk_reset_key() == 1) 				// 报警器复位按键按下进入空闲状态
		//	pecu_status = PECU_IDLE;
		
		process_pecu_status = ACTIVE;			// 紧急报警器工作于ACTIVE状态，过程数据process_pecu_status应该仍为ACTIVE = 0x02 
			
		break;
	}
}

static uint8_t chk_alarm_key(void)
{
	static uint8_t old_alarm_key = 1;	// 紧急报警器报警按键原状态为1
	uint8_t now_alarm_key;
	uint8_t tmp_alarm_key;
	uint16_t i;

	tmp_alarm_key = DrvGPIO_GetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);
	for(i = 0; i < 2000; i++);			// 延时去抖动
	now_alarm_key = DrvGPIO_GetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);
	if(tmp_alarm_key != now_alarm_key)
		return(0);						// 读取报警按键引脚2次，如果不认为是干扰，无效退出

	if((1 == old_alarm_key) && (0 == now_alarm_key))	// 检测报警按键下跳沿产生报警触发
	{
		old_alarm_key = now_alarm_key;
		return(1);
	}
	
	old_alarm_key = now_alarm_key;
	return(0);		
}

static uint8_t chk_ptt_key(void)
{
	uint8_t now_ptt_key;
	uint8_t tmp_ptt_key;
	uint16_t i;

	tmp_ptt_key = DrvGPIO_GetBit(PECU_CTRL_PORT, PTT_CHK_PIN);
	for(i = 0; i < 2000; i++);			// 延时去抖动
	now_ptt_key = DrvGPIO_GetBit(PECU_CTRL_PORT, PTT_CHK_PIN);
	if(tmp_ptt_key != now_ptt_key)		// 读取PTT按键引脚2次，如果不认为是干扰，无效退出
		return(0xff);

	if(0 == now_ptt_key)				// PTT键按下
		return(1);
	else if(1 == now_ptt_key)			// PTT键抬起
		return(0);

	return(0xff);
}

/* 2014-09-18修改，for南宁地铁1号线项目	
static uint8_t chk_reset_key(void)
{
	static uint8_t old_reset_key = 1;	// 紧急报警器复位按键原状态为1
	uint8_t	now_reset_key;
	uint8_t tmp_reset_key;
	uint16_t i;

	tmp_reset_key = DrvGPIO_GetBit(PECU_CTRL_PORT, RESET_KEY_PIN);
	for(i = 0; i < 2000; i++);			// 延时去抖动
	now_reset_key = DrvGPIO_GetBit(PECU_CTRL_PORT, RESET_KEY_PIN);
	if(tmp_reset_key != now_reset_key)
		return(0);						// 读取报警器复位按键引脚2次，如果不相同则认为是干扰，无效退出

	if((1 == old_reset_key) && (0 == now_reset_key))	// 检测报警器复位按键下跳沿
	{
		old_reset_key = now_reset_key;
		return(1);
	}

	old_reset_key = now_reset_key;
	return(0);
}
*/

uint8_t get_addr(void)
{
	uint16_t i, j;
	uint8_t tmpaddr = 0x00;			// 0xf8;								// 2014-10-23修改
	uint8_t dev_addr = 7; 
	
	for(i = 0; i < 0xffff; i++)
		for(j = 0; j < 0xff; j++);
	

	tmpaddr |= (DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR0_PIN) & 0x07);					
	tmpaddr |= ((DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR1_PIN) << 1) & 0x07);
	tmpaddr |= ((DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR2_PIN) << 2) & 0x07);
	//tmpaddr |= (DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR3_PIN) << 3);

	// 2014-11-02修改，1、6短接为110，地址为1；2、7短接为101，地址为2；3、8短接为011，地址为3
	if(6 == tmpaddr)
		dev_addr = 1;
	else if(5 == tmpaddr)
		dev_addr = 2;
	else if(3 == tmpaddr)
		dev_addr = 3;
	
 	return(dev_addr);  
	
	// return(tmpaddr + 1);			// 2014-11-02修改		// return((~tmpaddr) + 1);					// 2014-10-23修改
	
	//return(1);	
}

static void send_process_packet(void)
{
	FRAME_HEADER *ptr_frame_header;
	
	ptr_frame_header = (FRAME_HEADER *)process_data_buffer;
	
	ptr_frame_header->dest_netnum = 0x00;
	ptr_frame_header->dest_eqnum.eq_type = LCU;					// 本地控制器类别
	ptr_frame_header->dest_eqnum.eq_num = lcu_eqnum;			// 本地控制器设备号
	ptr_frame_header->src_netnum = 0x00;
	ptr_frame_header->src_eqnum.eq_type = PECU;					// 紧急报警器类别
	ptr_frame_header->src_eqnum.eq_num = pecu_eqnum;			// 紧急报警器设备号
	ptr_frame_header->cmd.bits.m_include = 0;
	ptr_frame_header->cmd.bits.m_error = 0;
	ptr_frame_header->cmd.bits.m_active = 0;
	ptr_frame_header->cmd.bits.token_return = 1;
	ptr_frame_header->cmd.bits.packet_type = PROCESS_PACKET;
	ptr_frame_header->len = 1;
	process_data_buffer[6] = process_pecu_status;		// pecu_status;							// 将紧急报警器工作状态写入过程数据
	process_data_buffer[7] = chksum(process_data_buffer, (ptr_frame_header->len + 6));

	send_data(process_data_buffer, (ptr_frame_header->len + 7));	 	
}

static void send_data(uint8_t *buf, uint8_t n)
{
	uint8_t i, j = 0;
	
	tx_buffer[j++] = 0x7e;
	
	for(i = 0; i < n; i++)
	{
		if(0x7e == buf[i])
		{
			tx_buffer[j++] = 0x7f;
			tx_buffer[j++] = 0x80;
		}
		else if(0x7f == buf[i])
		{
			tx_buffer[j++] = 0x7f;
			tx_buffer[j++] = 0x81;
		}
		else
			tx_buffer[j++] = buf[i];	
	}
	tx_buffer[j++] = 0x7e;
	
	put_n_char(tx_buffer, j);	
}

static uint8_t prase_rx_data(uint16_t len)
{
	FRAME_HEADER *ptr_frame_header;

	ptr_frame_header = (FRAME_HEADER *)rx_buffer;

	if(len < 7)
		return(0);

	switch(ptr_frame_header->cmd.bits.packet_type)
	{
	case PROCESS_PACKET:
		break;

	case P2P_PACKET:
		break;

	case TOKEN_PACKET:
		if(ptr_frame_header->dest_netnum != 0x00)			// 是否本网
			return(0);
		if(ptr_frame_header->dest_eqnum.eq_type != PECU)	// 是否PECU
			return(0);
		if(ptr_frame_header->dest_eqnum.eq_num != pecu_eqnum)
			return(0);
		if(ptr_frame_header->src_netnum != 0x00)			// 是否本网
			return(0);
		if(ptr_frame_header->src_eqnum.eq_type != LCU)		// 是否LCU
			return(0);

		if(ptr_frame_header->len != 1)
			return(0);

		lcu_eqnum = ptr_frame_header->src_eqnum.eq_num;		// 获取本地控制单元设备号

		pecu_com_timeout = 0;

		lcu_cmd_type = rx_buffer[6];

		com_delay_time = 2;									// 收到一包数据，延时10ms应答
		while(com_delay_time !=0);

		send_process_packet();

		break;

	case DOWNLOAD_PACKET:
		break;
	}

	return(0xff);
}

static void prc_rx_data(void)
{
	uint8_t tmp;
	static uint16_t rx_data_len = 0;
	static uint8_t tmp_rx_chksum;
	static uint8_t rx_chksum;

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

static void alarm_led_flash(uint16_t flash_interval)
{
	if(alarm_led_flash_interval < flash_interval) 
		DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);
	else if((alarm_led_flash_interval >= flash_interval) && (alarm_led_flash_interval < (flash_interval * 2)))
		DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);
	else if(alarm_led_flash_interval >= (flash_interval * 2))
		alarm_led_flash_interval = 0;	
}

static void busy_led_flash(uint16_t flash_interval)
{
	if(busy_led_flash_interval < flash_interval) 
		DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);
	else if((busy_led_flash_interval >= flash_interval) && (busy_led_flash_interval < (flash_interval * 2)))
		DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);
	else if(busy_led_flash_interval >= (flash_interval * 2))
		busy_led_flash_interval = 0;
}

static void com_led_flash(uint16_t flash_interval)
{
	if(com_led_flash_interval < flash_interval)
		DrvGPIO_ClrBit(PECU_CTRL_PORT, COM_LED_PIN); 
		//DrvGPIO_SetBit(PECU_CTRL_PORT, COM_LED_PIN);
	else if((com_led_flash_interval >= flash_interval) && (com_led_flash_interval < (flash_interval * 2)))
		DrvGPIO_SetBit(PECU_CTRL_PORT, COM_LED_PIN);
		//DrvGPIO_ClrBit(PECU_CTRL_PORT, COM_LED_PIN);		
	else if(com_led_flash_interval >= (flash_interval * 2))
		com_led_flash_interval = 0;	
}

/* 2014-09-18修改，for南宁地铁1号线项目，噪检功能选择及报警开关量输出，
	报警键按下，关闭噪检功能，报警断开开关量闭合，报警结束，打开噪检功能，报警开关量
*/
static void noise_ctrl_sel(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_SetBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);			// NOISE_CTRL引脚为高电平，噪检功能被选择，报警开关量断开
	else
		DrvGPIO_ClrBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);			// NOISE_CTRL引脚为低电平，噪检功能被关闭，报警开关量闭合
}		 

static void LM4871_shutdown(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_SetBit(PECU_CTRL_PORT, SHUTDOWN_PIN);			// 高电平，shutdown		------	ON
	else
		DrvGPIO_ClrBit(PECU_CTRL_PORT, SHUTDOWN_PIN);			// 低电平，正常工作		------	OFF	
}

static void spk_input(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_ClrBit(PECU_CTRL_PORT, SPK_CTRL_PIN);			// 低电平，继电器吸合，扬声器音频输入
	else
		DrvGPIO_SetBit(PECU_CTRL_PORT, SPK_CTRL_PIN);			// 高电平，继电器断开
}

static void mic_output(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_ClrBit(PECU_CTRL_PORT, MIC_CTRL_PIN);			// 低电平，继电器吸合，MIC音频输出
	else
		DrvGPIO_SetBit(PECU_CTRL_PORT, MIC_CTRL_PIN);			// 高电平，继电器断开	
}

static void busy_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);			// 2014-04-09修改
		DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);			// 2014-04-09修改
		DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);
}

static void speak_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);			// 2014-04-09修改
		DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);			// 2014-04-09修改
		DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);
}

static void listen_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);		// 2014-04-09修改
		DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);		// 2014-04-09修改
		DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);
}

static void alarm_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);			// 2014-09-18修改，for南宁地铁1号线项目
		DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);			// 2014-09-18修改，for南宁地铁1号线项目
		DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);
}


void PECU_port_init(void)
{
	DrvGPIO_Open(PECU_CTRL_PORT, ALARM_KEY_PIN, IO_INPUT);			// 设置报警按键ALARM_KEY引脚为输入
	DrvGPIO_SetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);					// 设置报警按键ALARM_KEY引脚电平为高

	DrvGPIO_Open(PECU_CTRL_PORT, PTT_CHK_PIN, IO_INPUT);			// 设置报警按键PTT_CHK引脚为输入
	DrvGPIO_SetBit(PECU_CTRL_PORT, PTT_CHK_PIN);					// 设置报警按键PTT_CHK引脚电平为高

//	DrvGPIO_Open(PECU_CTRL_PORT, RESET_KEY_PIN, IO_INPUT);			// 设置报警按键PTT_CHK引脚为输入					// 2014-09-18修改，for南宁地铁1号线项目
//	DrvGPIO_SetBit(PECU_CTRL_PORT, RESET_KEY_PIN);					// 设置报警按键PTT_CHK引脚电平为高					// 2014-09-18修改，for南宁地铁1号线项目

	DrvGPIO_Open(PECU_CTRL_PORT, NOISE_CTRL_PIN, IO_OUTPUT);			// 设置噪检控制引脚NOISE_CTRL_PIN引脚为输输出		// 2014-09-18修改，for南宁地铁1号线项目
	DrvGPIO_SetBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);					// 设置噪检控制引脚NOISE_CTRL_PIN引脚电平为高		// 2014-09-18修改，for南宁地铁1号线项目，缺省状态为噪检输出


	DrvGPIO_Open(PECU_CTRL_PORT, MIC_CTRL_PIN, IO_OUTPUT);			// 设置MIC音频输出控制引脚MIC_CTRL_PIN为输出
	DrvGPIO_SetBit(PECU_CTRL_PORT, MIC_CTRL_PIN);				 	// 设置MIC音频输出控制引脚MIC_CTRL_PIN电平为高

	DrvGPIO_Open(PECU_CTRL_PORT, SPK_CTRL_PIN, IO_OUTPUT);			// 设置SPK音频输入控制引脚SPK_CTRL_PIN为输出
	DrvGPIO_SetBit(PECU_CTRL_PORT, SPK_CTRL_PIN);				 	// 设置SPK音频输入控制引脚SPK_CTRL_PIN电平为高
//	DrvGPIO_ClrBit(PECU_CTRL_PORT, SPK_CTRL_PIN);

	DrvGPIO_Open(PECU_CTRL_PORT, SHUTDOWN_PIN, IO_OUTPUT);			// 设置功放静音控制引脚SHUTDOWN_PIN为输出
	DrvGPIO_SetBit(PECU_CTRL_PORT, SHUTDOWN_PIN);				 	// 设置功放静音控制引脚SHUTDOWN_PIN电平为高

	DrvGPIO_Open(PECU_CTRL_PORT, COM_LED_PIN, IO_OUTPUT);			// 设置RS485 通信指示灯引脚COM_LED_PIN为输出
	DrvGPIO_ClrBit(PECU_CTRL_PORT, COM_LED_PIN);				 	// 设置RS485 通信指示灯引脚COM_LED_PIN电平为低

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR0_PIN, IO_INPUT);				// 设置地址ADDR0引脚为输入
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR0_PIN);						// 设置报地址ADDR0引脚电平为高

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR1_PIN, IO_INPUT);				// 设置地址ADDR1引脚为输入
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR1_PIN);						// 设置报地址ADDR1引脚电平为高

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR2_PIN, IO_INPUT);				// 设置地址ADDR2引脚为输入
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR2_PIN);						// 设置报地址ADDR2引脚电平为高

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR3_PIN, IO_INPUT);				// 设置地址ADDR3引脚为输入
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR3_PIN);						// 设置报地址ADDR3引脚电平为高

	DrvGPIO_Open(PECU_LED_PORT, ALARM_LED_PIN, IO_OUTPUT);			// 设置报警指示灯引脚ALARM_LED_PIN为输出
//	DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);					// 2014-09-18修改，for南宁地铁1号线项目	// 设置报警指示灯引脚ALARM_LED_PIN电平为低
	DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);					// 2014-09-18修改，for南宁地铁1号线项目

	DrvGPIO_Open(PECU_LED_PORT, LISTEN_LED_PIN, IO_OUTPUT);			// 设置报警指示灯引脚LISTEN_LED_PIN为输出
//	DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);					// 2014-04-09修改		// 设置报警指示灯引脚LISTEN_LED_PIN电平为低		
	DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);					// 2014-04-09修改
 	
	DrvGPIO_Open(PECU_LED_PORT, SPEAK_LED_PIN, IO_OUTPUT);			// 设置报警指示灯引脚SPEAK_LED_PIN为输出
//	DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);					// 2014-04-09修改		// 设置报警指示灯引脚SPEAK_LED_PIN电平为低
	DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);					// 2014-04-09修改

	DrvGPIO_Open(PECU_LED_PORT, BUSY_LED_PIN, IO_OUTPUT);			// 设置报警指示灯引脚BUSY_LED_PIN为输出
//	DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);					// 2014-04-09修改		// 设置报警指示灯引脚BUSY_LED_PIN电平为低
	DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);					// 2014-04-09修改

	mic_output(OFF);												// mic输出音频关掉
	spk_input(OFF);													// spk输入音频关掉
	LM4871_shutdown(ON);											// 功放静音，shutdown
}

uint8_t chksum(uint8_t *buf, uint16_t n)
{
	uint16_t i;
	uint8_t tmp = 0;

	for(i = 0; i < n; i++)
		tmp += buf[i];

	return((0x55 - tmp));	
}

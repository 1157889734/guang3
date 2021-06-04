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

static uint8_t process_pecu_status = IDLE;				// ���͹��������н���������״̬��ԭ�ȷ���pecu_status��������������æʱҲ����pecu_status����ִ������ӹ�������process_pecu_status
static uint8_t pecu_status = PECU_IDLE;			// ����������״̬
static uint8_t lcu_cmd_type = LCU_CMD_IDLE;		// ���ؿ��Ƶ�Ԫ����

uint8_t pecu_eqnum;							// �����������豸��
static uint8_t lcu_eqnum;					// ���ؿ��Ƶ�Ԫ�豸��

vu16 pecu_com_timeout = 0;				// ����������ͨ�ų�ʱ��ʱ��ÿ�յ�һ��������������������ȷ������0
vu16 alarm_timeout = 0;					// ������ʱ�������������º����2����û�н�ͨ���Զ���λ
vu16 ptt_timeout = 0;					// ������ͨ�����3������PTT��û�а��£�������������Զ���λ

uint8_t playback_over_flag = 0;			// ��æ����ʾ����ѭ��������ϱ�־
vu16 playback_time = 0;					// ��æ����ʾ����ѭ�����ż��

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
//static uint8_t chk_reset_key(void);					// 2014-09-18�޸ģ�for��������1������Ŀ
static void noise_ctrl_sel(uint8_t on_off);				// 2014-09-18�޸ģ�for��������1������Ŀ����칦��ѡ�񼰱��������������
														// ���������£��ر���칦�ܣ������Ͽ��������պϣ���������������칦�ܣ�����������

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
	case PECU_IDLE:							// ����״̬
		mic_output(OFF);				    // �ر�MIC��Ƶ���
		spk_input(OFF);						// �ر�SPK��Ƶ����
		LM4871_shutdown(ON);				// ����

		noise_ctrl_sel(ON);					// ����ʱ��������û�б����£�ѡ����칦�ܣ������������Ͽ���2014-09-18�޸ģ�for��������1����

		alarm_led(OFF);						// �ر����жԽ�״ָ̬ʾ��
		busy_led(OFF);
		listen_led(OFF);
		speak_led(OFF);		

		if(chk_alarm_key())
		{
			noise_ctrl_sel(OFF);					// �����������£�ѡ�����رգ������������պϣ�2014-09-18�޸ģ�for��������1����

			pecu_status = PECU_ALARM;		// ��ⱨ���������£�״̬�л�Ϊ����״̬
			alarm_timeout = 0;				// ��ʼ��������ʱ��ʱ��	
		}
		
		process_pecu_status = IDLE;			// ����������������IDLE״̬����������process_pecu_statusΪIDLE = 0x00		
		
		break;

	case PECU_ALARM:
		alarm_led_flash(50);				// ����״̬������ָʾ�Ƽ��1s����
		busy_led_flash(50);					// 2014-09-18�޸ģ�for��������1���ߣ�ԭռ��ָʾ�Ʊ��Ϊ����ָʾ�ƣ���alarm_led_flashͬ��

		//noise_ctrl_sel(OFF);					// �����������£�ѡ�����رգ������������պϣ�2014-09-18�޸ģ�for��������1����

		if(LCU_CMD_BUSY == lcu_cmd_type)	// �յ�ռ��״̬��ռ��ָʾ�Ƽ��1s����
		{
			LM4871_shutdown(OFF);			// ���Ŵ�
			playback_start();				// ����ռ����ʾ��

			playback_over_flag = 0;			// ��æ����ʾ����ѭ��������ϱ�־��0
			playback_time = 0;				// ��æ����ʾ����ѭ�����ż����0			

			pecu_status = PECU_BUSY;
		}
		else if(LCU_CMD_ACTIVE == lcu_cmd_type)	// ��ͨ������������������ͨ״̬
		{
			alarm_led(ON);
			// busy_led(OFF);					// 2014-09-18�޸ģ�for��������1���ߣ�ԭռ��ָʾ�Ʊ��Ϊ����ָʾ�ƣ���alarm_led_flashͬ��
			busy_led(ON);

			speak_led(ON);

			ptt_timeout = 0;				// PTT�������£�PTT��ʱ��ʱ������

			pecu_status = PECU_ACTIVED;	
		}
		else if(LCU_CMD_HANGUP == lcu_cmd_type)	// �Ҷ�����
			pecu_status = PECU_IDLE;
		
			
		if(alarm_timeout >= ALARM_TIMEOUT)		// �����������º�3����û�н�ͨ�����Զ�����IDLE״̬
			pecu_status = PECU_IDLE;

	//	if(chk_reset_key() == 1) 				// ��������λ�������½������״̬
	//		pecu_status = PECU_IDLE;

		process_pecu_status = ALARM;			// ����������������ALARM״̬����������process_pecu_statusΪALARM = 0x01
		
		break;

	case PECU_BUSY:							// ����������ռ��
		alarm_led_flash(50);				// ����״̬������ָʾ�Ƽ��0.5s����
		// busy_led_flash(100);				// ռ��ָʾ�Ƽ��1s����				// 2014-09-18�޸ģ�for��������1���ߣ�ԭռ��ָʾ�Ʊ��Ϊ����ָʾ�ƣ���alarm_led_flashͬ��
		busy_led_flash(50);				// ռ��ָʾ�Ƽ��1s����

		if((1 == playback_over_flag) &&	(playback_time >= 1000))		// ���ѭ������һ����ϣ�����һ�β��ż����0
		{
			playback_over_flag = 0;			// ѭ������һ����ϱ�־��0
			playback_time = 0;

			playback_start();
		}
		
		if(LCU_CMD_ACTIVE == lcu_cmd_type)	// ��ͨ������������������ͨ״̬
		{
			alarm_led(ON);
			// busy_led(OFF);				// 2014-09-18�޸ģ�for��������1���ߣ�ԭռ��ָʾ�Ʊ��Ϊ����ָʾ�ƣ���alarm_led_flashͬ��
			busy_led(ON);

			speak_led(ON);

			playback_stop();				// �ر�ռ����ʾ��
			
			ptt_timeout = 0;				// PTT�������£�PTT��ʱ��ʱ������
			
			pecu_status = PECU_ACTIVED;	
		}
		else if(LCU_CMD_HANGUP == lcu_cmd_type)	// �Ҷ�����
		{
			playback_stop();				// �ر�ռ����ʾ��

			pecu_status = PECU_IDLE;
		}
		else if(LCU_CMD_IDLE == lcu_cmd_type)	// ����������æ״̬����ʱ�յ������������뱨��״̬
		{
			playback_stop();
			// busy_led(OFF);				// 2014-09-18�޸ģ�for��������1���ߣ�ԭռ��ָʾ�Ʊ��Ϊ����ָʾ�ƣ���alarm_led_flashͬ��
			LM4871_shutdown(ON);

			pecu_status = PECU_ALARM;
		}
			
		if(alarm_timeout >= ALARM_TIMEOUT)		// �����������º�3����û�н�ͨ�����Զ�����IDLE״̬
		{
			playback_stop();				// �ر�ռ����ʾ��

			pecu_status = PECU_IDLE;
		}
		
		//if(chk_reset_key() == 1) 				// ��������λ�������½������״̬
		//{
		//	playback_stop();				// �ر�ռ����ʾ��

		//	pecu_status = PECU_IDLE;
		//}	
		
		process_pecu_status = ALARM;			// ����������������BUSY״̬����������process_pecu_statusӦ����ΪALARM = 0x01			
			
		break; 

	case PECU_ACTIVED:
		mic_output(ON);				    // ��MIC��Ƶ���
		//spk_input(ON);					// ��SPK��Ƶ����
		//LM4871_shutdown(OFF);			// 2014-10-08�޸�			// ���Ž��빤��״̬					

		if(chk_ptt_key() == 1)				// ���PTT�����Ƿ���
		{
			listen_led(ON);
			speak_led(OFF);

		//	mic_output(OFF);				    // �ر�MIC��Ƶ���
			LM4871_shutdown(OFF);			// 2014-10-08�޸�			// ���Ž��빤��״̬
			spk_input(ON);

			ptt_timeout = 0;				// PTT�������£�PTT��ʱ��ʱ������
		}
		else if(chk_ptt_key() == 0)			// ���PTT�����Ƿ�̧��			
		{
			listen_led(OFF);
			speak_led(ON);

		//	mic_output(ON);				    // �ر�MIC��Ƶ���
			LM4871_shutdown(ON);			// 2014-10-08�޸�			// ���Ž��뾲��״̬
			spk_input(OFF);
		}

		if(LCU_CMD_HANGUP == lcu_cmd_type)
			pecu_status = PECU_IDLE;
			
		if(ptt_timeout >= PTT_TIMEOUT)		// ������ͨ��3����û��PTT���£����Զ�����IDLE״̬
			pecu_status = PECU_IDLE;

		//if(chk_reset_key() == 1) 				// ��������λ�������½������״̬
		//	pecu_status = PECU_IDLE;
		
		process_pecu_status = ACTIVE;			// ����������������ACTIVE״̬����������process_pecu_statusӦ����ΪACTIVE = 0x02 
			
		break;
	}
}

static uint8_t chk_alarm_key(void)
{
	static uint8_t old_alarm_key = 1;	// ������������������ԭ״̬Ϊ1
	uint8_t now_alarm_key;
	uint8_t tmp_alarm_key;
	uint16_t i;

	tmp_alarm_key = DrvGPIO_GetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);
	for(i = 0; i < 2000; i++);			// ��ʱȥ����
	now_alarm_key = DrvGPIO_GetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);
	if(tmp_alarm_key != now_alarm_key)
		return(0);						// ��ȡ������������2�Σ��������Ϊ�Ǹ��ţ���Ч�˳�

	if((1 == old_alarm_key) && (0 == now_alarm_key))	// ��ⱨ�����������ز�����������
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
	for(i = 0; i < 2000; i++);			// ��ʱȥ����
	now_ptt_key = DrvGPIO_GetBit(PECU_CTRL_PORT, PTT_CHK_PIN);
	if(tmp_ptt_key != now_ptt_key)		// ��ȡPTT��������2�Σ��������Ϊ�Ǹ��ţ���Ч�˳�
		return(0xff);

	if(0 == now_ptt_key)				// PTT������
		return(1);
	else if(1 == now_ptt_key)			// PTT��̧��
		return(0);

	return(0xff);
}

/* 2014-09-18�޸ģ�for��������1������Ŀ	
static uint8_t chk_reset_key(void)
{
	static uint8_t old_reset_key = 1;	// ������������λ����ԭ״̬Ϊ1
	uint8_t	now_reset_key;
	uint8_t tmp_reset_key;
	uint16_t i;

	tmp_reset_key = DrvGPIO_GetBit(PECU_CTRL_PORT, RESET_KEY_PIN);
	for(i = 0; i < 2000; i++);			// ��ʱȥ����
	now_reset_key = DrvGPIO_GetBit(PECU_CTRL_PORT, RESET_KEY_PIN);
	if(tmp_reset_key != now_reset_key)
		return(0);						// ��ȡ��������λ��������2�Σ��������ͬ����Ϊ�Ǹ��ţ���Ч�˳�

	if((1 == old_reset_key) && (0 == now_reset_key))	// ��ⱨ������λ����������
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
	uint8_t tmpaddr = 0x00;			// 0xf8;								// 2014-10-23�޸�
	uint8_t dev_addr = 7; 
	
	for(i = 0; i < 0xffff; i++)
		for(j = 0; j < 0xff; j++);
	

	tmpaddr |= (DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR0_PIN) & 0x07);					
	tmpaddr |= ((DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR1_PIN) << 1) & 0x07);
	tmpaddr |= ((DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR2_PIN) << 2) & 0x07);
	//tmpaddr |= (DrvGPIO_GetBit(PECU_ADDR_PORT, ADDR3_PIN) << 3);

	// 2014-11-02�޸ģ�1��6�̽�Ϊ110����ַΪ1��2��7�̽�Ϊ101����ַΪ2��3��8�̽�Ϊ011����ַΪ3
	if(6 == tmpaddr)
		dev_addr = 1;
	else if(5 == tmpaddr)
		dev_addr = 2;
	else if(3 == tmpaddr)
		dev_addr = 3;
	
 	return(dev_addr);  
	
	// return(tmpaddr + 1);			// 2014-11-02�޸�		// return((~tmpaddr) + 1);					// 2014-10-23�޸�
	
	//return(1);	
}

static void send_process_packet(void)
{
	FRAME_HEADER *ptr_frame_header;
	
	ptr_frame_header = (FRAME_HEADER *)process_data_buffer;
	
	ptr_frame_header->dest_netnum = 0x00;
	ptr_frame_header->dest_eqnum.eq_type = LCU;					// ���ؿ��������
	ptr_frame_header->dest_eqnum.eq_num = lcu_eqnum;			// ���ؿ������豸��
	ptr_frame_header->src_netnum = 0x00;
	ptr_frame_header->src_eqnum.eq_type = PECU;					// �������������
	ptr_frame_header->src_eqnum.eq_num = pecu_eqnum;			// �����������豸��
	ptr_frame_header->cmd.bits.m_include = 0;
	ptr_frame_header->cmd.bits.m_error = 0;
	ptr_frame_header->cmd.bits.m_active = 0;
	ptr_frame_header->cmd.bits.token_return = 1;
	ptr_frame_header->cmd.bits.packet_type = PROCESS_PACKET;
	ptr_frame_header->len = 1;
	process_data_buffer[6] = process_pecu_status;		// pecu_status;							// ����������������״̬д���������
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
		if(ptr_frame_header->dest_netnum != 0x00)			// �Ƿ���
			return(0);
		if(ptr_frame_header->dest_eqnum.eq_type != PECU)	// �Ƿ�PECU
			return(0);
		if(ptr_frame_header->dest_eqnum.eq_num != pecu_eqnum)
			return(0);
		if(ptr_frame_header->src_netnum != 0x00)			// �Ƿ���
			return(0);
		if(ptr_frame_header->src_eqnum.eq_type != LCU)		// �Ƿ�LCU
			return(0);

		if(ptr_frame_header->len != 1)
			return(0);

		lcu_eqnum = ptr_frame_header->src_eqnum.eq_num;		// ��ȡ���ؿ��Ƶ�Ԫ�豸��

		pecu_com_timeout = 0;

		lcu_cmd_type = rx_buffer[6];

		com_delay_time = 2;									// �յ�һ�����ݣ���ʱ10msӦ��
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

/* 2014-09-18�޸ģ�for��������1������Ŀ����칦��ѡ�񼰱��������������
	���������£��ر���칦�ܣ������Ͽ��������պϣ���������������칦�ܣ�����������
*/
static void noise_ctrl_sel(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_SetBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);			// NOISE_CTRL����Ϊ�ߵ�ƽ����칦�ܱ�ѡ�񣬱����������Ͽ�
	else
		DrvGPIO_ClrBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);			// NOISE_CTRL����Ϊ�͵�ƽ����칦�ܱ��رգ������������պ�
}		 

static void LM4871_shutdown(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_SetBit(PECU_CTRL_PORT, SHUTDOWN_PIN);			// �ߵ�ƽ��shutdown		------	ON
	else
		DrvGPIO_ClrBit(PECU_CTRL_PORT, SHUTDOWN_PIN);			// �͵�ƽ����������		------	OFF	
}

static void spk_input(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_ClrBit(PECU_CTRL_PORT, SPK_CTRL_PIN);			// �͵�ƽ���̵������ϣ���������Ƶ����
	else
		DrvGPIO_SetBit(PECU_CTRL_PORT, SPK_CTRL_PIN);			// �ߵ�ƽ���̵����Ͽ�
}

static void mic_output(uint8_t on_off)
{
	if(on_off == ON)
		DrvGPIO_ClrBit(PECU_CTRL_PORT, MIC_CTRL_PIN);			// �͵�ƽ���̵������ϣ�MIC��Ƶ���
	else
		DrvGPIO_SetBit(PECU_CTRL_PORT, MIC_CTRL_PIN);			// �ߵ�ƽ���̵����Ͽ�	
}

static void busy_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);			// 2014-04-09�޸�
		DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);			// 2014-04-09�޸�
		DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);
}

static void speak_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);			// 2014-04-09�޸�
		DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);			// 2014-04-09�޸�
		DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);
}

static void listen_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);		// 2014-04-09�޸�
		DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);		// 2014-04-09�޸�
		DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);
}

static void alarm_led(uint8_t on_off)
{
	if(on_off == ON)
		//DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);			// 2014-09-18�޸ģ�for��������1������Ŀ
		DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);
	else
		//DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);			// 2014-09-18�޸ģ�for��������1������Ŀ
		DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);
}


void PECU_port_init(void)
{
	DrvGPIO_Open(PECU_CTRL_PORT, ALARM_KEY_PIN, IO_INPUT);			// ���ñ�������ALARM_KEY����Ϊ����
	DrvGPIO_SetBit(PECU_CTRL_PORT, ALARM_KEY_PIN);					// ���ñ�������ALARM_KEY���ŵ�ƽΪ��

	DrvGPIO_Open(PECU_CTRL_PORT, PTT_CHK_PIN, IO_INPUT);			// ���ñ�������PTT_CHK����Ϊ����
	DrvGPIO_SetBit(PECU_CTRL_PORT, PTT_CHK_PIN);					// ���ñ�������PTT_CHK���ŵ�ƽΪ��

//	DrvGPIO_Open(PECU_CTRL_PORT, RESET_KEY_PIN, IO_INPUT);			// ���ñ�������PTT_CHK����Ϊ����					// 2014-09-18�޸ģ�for��������1������Ŀ
//	DrvGPIO_SetBit(PECU_CTRL_PORT, RESET_KEY_PIN);					// ���ñ�������PTT_CHK���ŵ�ƽΪ��					// 2014-09-18�޸ģ�for��������1������Ŀ

	DrvGPIO_Open(PECU_CTRL_PORT, NOISE_CTRL_PIN, IO_OUTPUT);			// ��������������NOISE_CTRL_PIN����Ϊ�����		// 2014-09-18�޸ģ�for��������1������Ŀ
	DrvGPIO_SetBit(PECU_CTRL_PORT, NOISE_CTRL_PIN);					// ��������������NOISE_CTRL_PIN���ŵ�ƽΪ��		// 2014-09-18�޸ģ�for��������1������Ŀ��ȱʡ״̬Ϊ������


	DrvGPIO_Open(PECU_CTRL_PORT, MIC_CTRL_PIN, IO_OUTPUT);			// ����MIC��Ƶ�����������MIC_CTRL_PINΪ���
	DrvGPIO_SetBit(PECU_CTRL_PORT, MIC_CTRL_PIN);				 	// ����MIC��Ƶ�����������MIC_CTRL_PIN��ƽΪ��

	DrvGPIO_Open(PECU_CTRL_PORT, SPK_CTRL_PIN, IO_OUTPUT);			// ����SPK��Ƶ�����������SPK_CTRL_PINΪ���
	DrvGPIO_SetBit(PECU_CTRL_PORT, SPK_CTRL_PIN);				 	// ����SPK��Ƶ�����������SPK_CTRL_PIN��ƽΪ��
//	DrvGPIO_ClrBit(PECU_CTRL_PORT, SPK_CTRL_PIN);

	DrvGPIO_Open(PECU_CTRL_PORT, SHUTDOWN_PIN, IO_OUTPUT);			// ���ù��ž�����������SHUTDOWN_PINΪ���
	DrvGPIO_SetBit(PECU_CTRL_PORT, SHUTDOWN_PIN);				 	// ���ù��ž�����������SHUTDOWN_PIN��ƽΪ��

	DrvGPIO_Open(PECU_CTRL_PORT, COM_LED_PIN, IO_OUTPUT);			// ����RS485 ͨ��ָʾ������COM_LED_PINΪ���
	DrvGPIO_ClrBit(PECU_CTRL_PORT, COM_LED_PIN);				 	// ����RS485 ͨ��ָʾ������COM_LED_PIN��ƽΪ��

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR0_PIN, IO_INPUT);				// ���õ�ַADDR0����Ϊ����
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR0_PIN);						// ���ñ���ַADDR0���ŵ�ƽΪ��

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR1_PIN, IO_INPUT);				// ���õ�ַADDR1����Ϊ����
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR1_PIN);						// ���ñ���ַADDR1���ŵ�ƽΪ��

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR2_PIN, IO_INPUT);				// ���õ�ַADDR2����Ϊ����
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR2_PIN);						// ���ñ���ַADDR2���ŵ�ƽΪ��

	DrvGPIO_Open(PECU_ADDR_PORT, ADDR3_PIN, IO_INPUT);				// ���õ�ַADDR3����Ϊ����
	DrvGPIO_SetBit(PECU_ADDR_PORT, ADDR3_PIN);						// ���ñ���ַADDR3���ŵ�ƽΪ��

	DrvGPIO_Open(PECU_LED_PORT, ALARM_LED_PIN, IO_OUTPUT);			// ���ñ���ָʾ������ALARM_LED_PINΪ���
//	DrvGPIO_ClrBit(PECU_LED_PORT, ALARM_LED_PIN);					// 2014-09-18�޸ģ�for��������1������Ŀ	// ���ñ���ָʾ������ALARM_LED_PIN��ƽΪ��
	DrvGPIO_SetBit(PECU_LED_PORT, ALARM_LED_PIN);					// 2014-09-18�޸ģ�for��������1������Ŀ

	DrvGPIO_Open(PECU_LED_PORT, LISTEN_LED_PIN, IO_OUTPUT);			// ���ñ���ָʾ������LISTEN_LED_PINΪ���
//	DrvGPIO_ClrBit(PECU_LED_PORT, LISTEN_LED_PIN);					// 2014-04-09�޸�		// ���ñ���ָʾ������LISTEN_LED_PIN��ƽΪ��		
	DrvGPIO_SetBit(PECU_LED_PORT, LISTEN_LED_PIN);					// 2014-04-09�޸�
 	
	DrvGPIO_Open(PECU_LED_PORT, SPEAK_LED_PIN, IO_OUTPUT);			// ���ñ���ָʾ������SPEAK_LED_PINΪ���
//	DrvGPIO_ClrBit(PECU_LED_PORT, SPEAK_LED_PIN);					// 2014-04-09�޸�		// ���ñ���ָʾ������SPEAK_LED_PIN��ƽΪ��
	DrvGPIO_SetBit(PECU_LED_PORT, SPEAK_LED_PIN);					// 2014-04-09�޸�

	DrvGPIO_Open(PECU_LED_PORT, BUSY_LED_PIN, IO_OUTPUT);			// ���ñ���ָʾ������BUSY_LED_PINΪ���
//	DrvGPIO_ClrBit(PECU_LED_PORT, BUSY_LED_PIN);					// 2014-04-09�޸�		// ���ñ���ָʾ������BUSY_LED_PIN��ƽΪ��
	DrvGPIO_SetBit(PECU_LED_PORT, BUSY_LED_PIN);					// 2014-04-09�޸�

	mic_output(OFF);												// mic�����Ƶ�ص�
	spk_input(OFF);													// spk������Ƶ�ص�
	LM4871_shutdown(ON);											// ���ž�����shutdown
}

uint8_t chksum(uint8_t *buf, uint16_t n)
{
	uint16_t i;
	uint8_t tmp = 0;

	for(i = 0; i < n; i++)
		tmp += buf[i];

	return((0x55 - tmp));	
}

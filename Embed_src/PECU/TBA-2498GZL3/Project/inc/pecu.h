#ifndef __PECU_H
#define __PECU_H

#include "ISD9xx.h"

typedef struct
{
	uint8_t dest_netnum;
	struct
	{
		uint8_t eq_type:4;
		uint8_t eq_num:4;
	}dest_eqnum;
	uint8_t src_netnum;
	struct
	{
		uint8_t eq_type:4;
		uint8_t eq_num:4;
	}src_eqnum;
	union
	{
		struct
		{
			uint8_t packet_type:4;
			uint8_t token_return:1;
			uint8_t m_active:1;
			uint8_t m_error:1;
			uint8_t m_include:1;
		}bits;
		uint8_t bytes;
	}cmd;
	uint8_t len;
}FRAME_HEADER;

#define COM_RX_HEAD				0x01
#define COM_RX_DATA				0x02
#define COM_RX_DLE				0x03

#define ON						1
#define OFF						0

#define PROCESS_PACKET			0x00
#define P2P_PACKET				0x01
#define TOKEN_PACKET			0x02
#define DOWNLOAD_PACKET			0x04

#define LCU						0x03
#define PECU					0x05

#define IDLE					0x00
#define ALARM					0x01
#define ACTIVE					0x02

#define PECU_IDLE				0x00
#define PECU_ALARM				0x01
#define PECU_BUSY				0x02
#define PECU_ACTIVED			0x03

#define LCU_CMD_IDLE			0x00
#define LCU_CMD_BUSY			0x01
#define LCU_CMD_ACTIVE			0x02
#define LCU_CMD_HANGUP			0x03

#define PECU_CTRL_PORT			GPA
#define ALARM_KEY_PIN			0
#define PTT_CHK_PIN				1
#define MIC_CTRL_PIN			3
#define SPK_CTRL_PIN			4
#define SHUTDOWN_PIN			5
#define COM_LED_PIN				6
//#define RESET_KEY_PIN			7			// 2014-09-18�޸ģ���������1������Ŀ
#define NOISE_CTRL_PIN			7

#define PECU_LED_PORT			GPB
#define ALARM_LED_PIN			0
#define LISTEN_LED_PIN			1		// 3		// 1
#define SPEAK_LED_PIN			2
#define BUSY_LED_PIN			3		// 1		// 3

#define PECU_ADDR_PORT			GPB
#define ADDR0_PIN				4
#define ADDR1_PIN				5
#define ADDR2_PIN				6
#define ADDR3_PIN				7

#define EN1483_PIN				2
#define EN1483_PORT				GPA

#define	EN1483					DrvGPIO_SetBit(EN1483_PORT, EN1483_PIN)
#define DIS1483					DrvGPIO_ClrBit(EN1483_PORT, EN1483_PIN)

//#define PECU_LED_PORT			GPB
//#define ALARM_LED_PIN			0
//#define LISTEN_LED_PIN			1
//#define SPEAK_LED_PIN			2
//#define BUSY_LED_PIN			3

extern vu16 alarm_led_flash_interval;
extern vu16 busy_led_flash_interval;
extern vu16 com_led_flash_interval;

extern vu8 com_delay_time;

#define PECU_COM_TIMEOUT		500				// ����������û���յ�����ʱ����ֵ����ֵΪ5s
extern vu16 pecu_com_timeout;

#define ALARM_TIMEOUT			(3 * 60 * 1000 / 10)	// minute * 60s * 1000/10	3���ӳ�ʱ			
extern vu16 alarm_timeout;						// ������ʱ�������������º����3����û�н�ͨ���Զ���λ

#define PTT_TIMEOUT				(3 * 60 * 1000 / 10)				
extern vu16 ptt_timeout;					// ������ͨ�����3������PTT��û�а��£�������������Զ���λ

extern uint8_t playback_over_flag;			// ��æ����ʾ����ѭ��������ϱ�־
extern vu16 playback_time;					// ��æ����ʾ����ѭ�����ż��

extern uint8_t pecu_eqnum;

void PECU_port_init(void);
void PECU_run(void);
uint8_t get_addr(void);

#endif	/* __PECU_H	*/

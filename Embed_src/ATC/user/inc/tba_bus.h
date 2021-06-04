#ifndef __TBA_BUS_H__
#define __TBA_BUS_H__

#include "config.h"

//ͨ��Э�������ַ�
#define TBA_PACKET_TAB          0x7E    //���ݰ��ָ��
#define TBA_PACKET_DLE          0x7F    //���ݰ�ȫ��ת���
#define TBA_PACKET_DLE_TAB      0x80    //���ݰ��ָ����ת���
#define TBA_PACKET_DLE_DLE      0x81    //���ݰ�ת�����ת���

//������������
#define TBA_CMD_DEFAULTPACKET   0x00        //�������ݰ�
#define TBA_CMD_P2PPACKET       0x01        //��Ե�����
#define TBA_CMD_TOKENPACKET     0x02        //���ư�,����Ѳ��
#define TBA_CMD_TOKENCLEAR      0x03        //����������Ƶİ�
#define TBA_CMD_UPGRADE         0x04        //����������������

#define TBA_CMD_ATC_DATA        0x08    // Added by Lison on 2013-1-17 15:50:34 

#define  TBAOK    1
#define  TBAERR   2


#define TBA_FRAME_LEN   9



//���������ȶ���--���ݾ���Ӧ�øı�
#define TBA_BUF_SIZE        512
#define  UART_TBA           UART2

typedef struct  TBA_INFO
{
    uint16 len;
    uint8   buf[TBA_BUF_SIZE];
} ttba_info;



//�г��������ݰ��ṹ
__packed typedef struct
{
    uint8 head;
    uint8 dst_net_id;
    uint8 dst_dev_id;
    uint8 src_net_id;
    uint8 src_dev_id;
    //�г����������ֽṹ---��λ��ǰ
    __packed struct BUS_CMD
    {
        uint8 packet_type: 4; //������
        uint8 token_free: 1; //���ͷ��ͷ����Ƶı�־
        uint8 m_reserve: 3; //����
    } bus_cmd;
    uint8 app_length;
    uint8 app_data[TBA_BUF_SIZE];
} tba_packet_t;




#define USART_FIFO_SIZE (512)

extern uint8 tba_bus_get_packet(void);
extern uint8 decode_check_packet(void);
extern void proc_deal_packet(void);
/***********************************************************************************************************************
*Function Nane  :  get_train485_state
*Description      :  ȡ��train485��״̬
*Input              :  ��
*Output            :  ��
*Return            :  !0 : ���� 0 �쳣
*Others            :  ��
***********************************************************************************************************************/
uint8 get_train485_state(void);

/***********************************************************************************************************************
*Function Nane  :  get_train485_comm
*Description      :  ȡ��train485��ͨ��״̬
*Input              :  ��
*Output            :  ��
*Return            :  !0 : ��ͨ�� 0 ��ͨ��
*Others            :  ��
***********************************************************************************************************************/
uint8 get_train485_comm(void);

/***********************************************************************************************************************
*Function Nane  :  set_tba_debug
*Description      :  �����Ƿ��ӡtba����
*Input              :  type 0 ����ӡ 1 ��ӡ
*Output            :  ��
*Return            : ��
*Others            :  ��
***********************************************************************************************************************/
void set_tba_send(uint8 type);
/***********************************************************************************************************************
*Function Nane  :  set_tba_num_repeat
*Description      :  �����Ƿ��ӡtba����
*Input              :  num ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void set_tba_num_repeat(uint8 num);

/***********************************************************************************************************************
*Function Nane  :  save_tms_data
*Description      :  ����TMS������
*Input              :  buf ����
                          len ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void save_tms_data(uint8 *buf, uint16 len);


int read_tba_bus_data(void);
#endif


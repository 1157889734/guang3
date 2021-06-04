#ifndef __CMD_H
#define __CMD_H

#include"config.h"

#define PTU_BUF_LEN    512      //PTU���ݻ��峤��
#define FRAME_BUF_LEN  256      //����֡����

#define MAXPATH        50       //·����󳤶�

#define CMD_UPGRADE      0x01   //��������
#define CMD_VERSION      0x02   //��ѯ�汾����
#define CMD_SHOWNET      0x03   //��ʾ�������
#define CMD_SETNET       0x04   //��������ip ����
#define CMD_SETMAC       0x05   //��������MAC
#define CMD_SETTIME      0x06   //����ʱ��
#define CMD_GETTIME      0x07   //��ѯʱ��
#define CMD_GETSTATE     0x08   //��ѯ״̬
#define CMD_SDINFO       0x09   //��ѯSD��״̬
#define CMD_GETFILE      0x0a   //��ȡ�ļ�
#define CMD_CANCEL       0x0b   //ȡ����ȡ�ļ�����
#define CMD_FORMATSD     0x0c   //��ʽ��SD��
#define CMD_ERMOVEFILE   0x0d   //ɾ���ļ�
#define CMD_REBOOT       0x0e   //��λģ��
#define CMD_USB_DEBUG    0x0f   //usb��ӡ����
#define CMD_HEARTBEAT    0xff   //ptu����

#define SENDSTATE       0x01    //����ģ��״̬����
#define SENDTMS         0x02    //����TCMS��������
#define SENDPIS         0x03    //����PISC��������
#define SENDTBA         0x04    //����TBA��������

#define CANCELTMS       0x12    //ȡ������TCMS��������
#define CANCELPIS       0x13    //ȡ������PISC��������
#define CANCELTBA       0x14    //ȡ������ TBA��������


#define PTU_PACKET_TAB          0x7E    //���ݰ�֡ͷ
#define PTU_PACKET_DLE          0x7F    //���ݰ�ȫ��ת���
#define PTU_PACKET_DLE_TAB      0x80    //���ݰ��ָ����ת���
#define PTU_PACKET_DLE_DLE      0x81    //���ݰ�ת�����ת���


#define ERR_NO_FILE    0  //�ļ������ڴ���
#define ERR_USE_FILE   1  //�ļ���ʹ���д���


#define PTU_NORMAL    0  //����״̬
#define PTU_GETFILE   1  //��ȡ�ļ�״̬
#define PTU_COPYSD    2  //����SD״̬
#define PTU_NO_COMM   3

#define READY   0        //��ȡ�ļ�׼��
#define GOING   1        //�ļ���ȡ��
#define FINISH  2        //�ļ���ȡ����
#define CANCEL  3        //ȡ����ȡ�ļ�

#define TCMS_DEBUG 1
#define PISC_DEBUG 2
#define TBA_DEBUG  3

//PTU���ݽ������ݽṹ
typedef struct PTU_INFO
{
    uint16 len;               //���ݳ���
    uint8  buf[PTU_BUF_LEN];  //����
} tptu_info;

//PTUͨ��Э������֡�ṹ
typedef struct FREAM_INFO
{
    uint8 head;              //֡ͷ
    uint8 len;               //�ֳ�
    uint8 cmd;               //������
    uint8 buf[FRAME_BUF_LEN];//����
} tframe_info;

//��ȡ�ļ���Ϣ�ṹ��
typedef struct TPTU_GETFILE
{
    char   path[50];         //�ļ�·��
    uint8  flag;             //״̬��ʶ
    uint16 index;            //�����
    uint32 file_size;        //�ļ���С
    uint32 get_size;         //�Ѷ�ȡ�Ĵ�С
} tptu_getfile;
//PTU��Ϣ�ṹ��
typedef struct PTU_STATES
{
    uint8 states;           //PTU״̬
    uint8 comm_flag;
    uint32 time;            //���ͨ��ʱ��
    tptu_getfile getfile;
} tptu_states;

/***********************************************************************************************************************
*Function Nane  :  get_cs
*Description      :  �������ݵ�У���
*Input              :  pdata   ָ������
                          length  ���ݳ���
*Output            :  cs ָ��У���
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
uint8 get_cs(uint8 *pdata,  uint16  length,  uint8 *cs);

#endif


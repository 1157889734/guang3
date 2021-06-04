#include "main.h"
#include "config.h"
#include "usb.h"
#include "tba_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcms.h"
#include "uart.h"
#include <def.h>
#include "cmd.h"
#include "rtc_1337.h"
#include "sd_data.h"

uint8  tba_bus_default_send_buf[TBA_BUF_SIZE];
uint16 tba_bus_default_send_length = 0;

/*����train485��Ϣ*/
ttba_info tba_bus_recv;
ttba_info tba_bus_data;

uint8      old_tba_buf[TBA_FRAME_LEN] = {0};

/*�������ݰ���ָ��*/
tba_packet_t *tba_recv_packet  = (tba_packet_t *)tba_bus_data.buf;
tba_packet_t  tba_send_packet  ;


tTMS_INFO   tba_tms_to_pis;       /*  tms���ͣ�Ҫ����pis������*/

uint8  tba_num_repeat_send;

uint8  train485_get_pisc_token = 0;
uint32 train485_token_timer = 0;

tstate   atc_tba;                    //atc��tbaͨ��״̬

extern uint8 _gtmp_buf[256];

extern uint8 ATC_id;

/***********************************************************************************************************************
*Function Nane  :  send_tba_ptu
*Description      :  ����tba���ݵ�ptu
*Input              :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void send_tba_ptu(uint16 len)
{
    uint8 buf[4] = {0x7e, 0x37, 0x09, 0x04};
    buf[1] = len + 2;
    memcpy(_gtmp_buf, buf, 4);
    memcpy(&_gtmp_buf[4], tba_bus_recv.buf, len);
    get_cs(_gtmp_buf, len + 3, &_gtmp_buf[len + 4]);
    _gtmp_buf[len + 5] = 0x7e;
    usb_write_buf(_gtmp_buf, len + 6);;
}
/***********************************************************************************************************************
*Function Nane  :  tba_transcod
*Description    :  ת��
*Input          :  src  ת��ǰ����
                      slen ת��ǰ���ݳ���
*Output         :  dest ת��������
                       dest ת�������ݳ���
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void tba_transcod(uint8 *src, uint16 slen, uint8 *dest, uint16 *dlen)
{
    uint16 src_pos  = 1;
    uint16 dest_pos = 1;
    //֡ͷ��֡β��ת��
    dest[0] = src[0];
    
    for (src_pos = 1; src_pos < slen - 1; src_pos++)
    {
        if (TBA_PACKET_TAB == src[src_pos])
        {
            dest[dest_pos++] = TBA_PACKET_DLE;
            dest[dest_pos++] = TBA_PACKET_DLE_TAB;
        }
        else if (TBA_PACKET_DLE == src[src_pos])
        {
            dest[dest_pos++] = TBA_PACKET_DLE;
            dest[dest_pos++] = TBA_PACKET_DLE_DLE;
        }
        else
        {
            dest[dest_pos++] = src[src_pos];
        }
    }
    
    dest[dest_pos++] = TBA_PACKET_TAB;
    *dlen = dest_pos;
}

//*********************************************
//*��������:Ӧ�ò��װҪ���͵�����
//*����˵��:app_buf--Ӧ�ò����ݻ�����ָ��
//*ע������:
//*����˵��:��
//*********************************************
void tba_bus_send_data(uint8 dst_net_id, uint8 dst_dev_id, uint8 data_type, uint8 *data_buf, uint16 length)
{
    uint8  buf[512] = {0};
    uint8  app_len = 0;
    uint16 size = 0;
    //֡ͷ
    tba_send_packet.head  =  TBA_PACKET_TAB;
    //���Ŀ������š��豸��
    tba_send_packet.dst_net_id = dst_net_id;//Ŀ�������
    tba_send_packet.dst_dev_id = dst_dev_id;//Ŀ���豸ID
    //���Դ������š��豸��
    tba_send_packet.src_net_id = 0x00;
    tba_send_packet.src_dev_id = ATC_id;
    //�����������֣��������ͷ�������(bit 4)
    tba_send_packet.bus_cmd.packet_type = data_type;
    tba_send_packet.bus_cmd.token_free  = 0;//token_return bit == 1,���ͷ�������
    tba_send_packet.bus_cmd.m_reserve   = 0;
    
    //��Ӧ�ò�����
    if (250 < length)
    {
        tba_send_packet.app_length = 250;//���ȳ����ж�
    }
    else
    {
        tba_send_packet.app_length = length;
    }
    
    app_len = tba_send_packet.app_length;
    //��Ч����
    memcpy(&tba_send_packet.app_data, data_buf, app_len);
    //У����
    get_cs(&tba_send_packet.dst_net_id, 6 + app_len, &tba_send_packet.app_data[app_len]);
    //֡β
    tba_send_packet.app_data[app_len + 1] = TBA_PACKET_TAB;
    //ת��
    tba_transcod(&tba_send_packet.head, 7 + app_len + 2, buf, &size);
    //����
    uart_write(UART_TBA, buf, size);
}
/***********************************************************************************************************************
*Function Nane  :  save_tba_data_sd
*Description      :  �洢tba��������sd��
*Input              :  type TBAOK  ��ȷ��֡ TBAERR  �����֡
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void save_tba_data_sd(uint8 type)
{
    tRtcTimer time;
    get_cur_time(&time);
    
    if (TBAOK  == type)
    {
        //��¼����֡�Ľ���ʱ��
        if (1 == atc_tba.error)
        {
            atc_tba.error = 0;
            form_error_frame(TBA, 2, time, 0, 0);
        }
        
#if 1        //�ж�֡�����Ƿ�����һ֡����һ��
        
        if (memcmp(old_tba_buf, tba_bus_recv.buf, tba_bus_recv.len))
        {
            //��һ����¼��֡�Ľ���ʱ�� ����֡�Ŀ�ʼʱ��
            memcpy(old_tba_buf, tba_bus_recv.buf, TBA_FRAME_LEN);
            
            if (atc_tba.normal)
            {
                atc_tba.normal = 0;
                form_normal_frame(TBA , 2, time, NULL, atc_tba.num_repeat);
            }
            
            form_normal_frame(TBA , 1, time, tba_bus_recv.buf, tba_bus_recv.len);
            atc_tba.num_repeat = 0;
            atc_tba.normal = 1;
        }
        
#else
        
        if (atc_tba.normal)
        {
            atc_tba.normal = 0;
            form_normal_frame(TBA , 2, time, NULL, atc_tba.num_repeat);
        }
        
        form_normal_frame(TBA , 1, time, tba_bus_recv.buf, tba_bus_recv.len);
        atc_tba.num_repeat = 0;
        atc_tba.normal = 1;
#endif
        
        if (atc_tba.num_repeat != atc_tba.num_repeat)
        {
            atc_tba.num_repeat++; //֡���ظ�������1
        }
    }
    else if (TBAERR == type)
    {
        //��¼����֡�Ŀ�ʼʱ���������
        if (0 == atc_tba.error)
        {
            atc_tba.error = 1;
            form_error_frame(TBA, 0, time, tba_bus_recv.buf, tba_bus_recv.len);
        }
        else
        {
            form_error_frame(TBA, 1, time, tba_bus_recv.buf, tba_bus_recv.len);
        }
    }
}
/***********************************************************************************************************************
*Function Nane  :  reverse_code_packet
*Description      :  ת�벢���У�����Ƿ���ȷ
*Input              :  ��
*Output            :  ��
*Return            :  1:�ɹ� 0 :����
*Others            :  ��
***********************************************************************************************************************/
uint8 reverse_code_packet(void)
{
    uint16 i = 0;
    uint16 j = 0;
    uint8 cs = 0;
    //֡ͷ
    tba_bus_data.buf[0] = tba_bus_recv.buf[0];
    
    for (j = 1, i = 1; i < tba_bus_recv.len - 1; i++)
    {
        tba_bus_data.buf[j] = tba_bus_recv.buf[i];
        
        //ת��
        if (TBA_PACKET_DLE == tba_bus_recv.buf[i])
        {
            if (TBA_PACKET_DLE_TAB == tba_bus_recv.buf[i + 1])
            {
                tba_bus_data.buf[j] = TBA_PACKET_TAB;
                i++;
            }
            else if (TBA_PACKET_DLE_DLE == tba_bus_recv.buf[i + 1])
            {
                tba_bus_data.buf[j] = TBA_PACKET_DLE;
                i++;
            }
            else
            {
                tba_bus_recv.len = 0;
                tba_bus_data.len = 0;
                return 0;
            }
        }
        
        j++;
    }
    
    tba_bus_data.len = j;
    get_cs(&tba_bus_data.buf[1], tba_bus_data.len - 2, &cs);
    
    if (cs == tba_bus_data.buf[tba_bus_data.len - 1])
    {
        return 1;
    }
    else
    {
        tba_bus_data.len = 0;
        tba_bus_recv.len = 0;
        return 0;
    }
}

//*********************************************************
//*��������:�г����߽������ݰ�
//*����˵��:
//*ע������:�������ݰ���������tba_bus_recv_buf[]������
//*����˵��:
//*********************************************************
uint8 tba_bus_get_packet(void)
{
    uint16   size = 0;
    uint8    data = 0;
    static uint8  head_flag = 0; //��֡ͷ��־λ
    
    while ((1 == uart_read(UART_TBA, &data, 1)) && (size < 512))
    {
        atc_tba.comm_flag = 1;
        atc_tba.comm_time = get_time();
        size ++;
        
        //֡ͷ����֡β
        if (TBA_PACKET_TAB == data)
        {
            //�յ����ֽ�С��7�����϶�����
            if (7 <= tba_bus_recv.len)
            {
                tba_bus_recv.buf[tba_bus_recv.len] = data;
                tba_bus_recv.len++;
                head_flag = 0;
                
                if (1 == reverse_code_packet())
                {
                    //������ATCģ�������
                    if (tba_recv_packet->dst_dev_id == ATC_id)
                    {
                        if ((0x21 == tba_recv_packet->src_dev_id)
                            || (0x11 == tba_recv_packet->src_dev_id))
                        {
                            return 1;
                        }
                        else
                        {
                            tba_bus_recv.len = 0;
                            continue;
                        }
                    }
                    else
                    {
                        //���Ƿ���ATCģ�������
                        tba_bus_recv.len = 0;
                        continue;
                    }
                }
                else
                {
                    tba_bus_recv.len    = 1;
                    tba_bus_recv.buf[0] = TBA_PACKET_TAB;
                    head_flag = 1;
                    continue;
                }
            }
            else
            {
                tba_bus_recv.len    = 1;
                tba_bus_recv.buf[0] = TBA_PACKET_TAB;
                head_flag = 1;
            }
        }
        else
        {
            //֡ͷ֡β֮�������
            if (1 == head_flag)
            {
                tba_bus_recv.buf[tba_bus_recv.len] = data;
                tba_bus_recv.len++;
                
                if (TBA_BUF_SIZE <= tba_bus_recv.len)
                {
                    tba_bus_recv.len = 0;
                    head_flag = 0;
                }
            }
        }
    }
    
    return 0;
}
/***********************************************************************************************************************
*Function Nane  :  proc_token_packet
*Description    :  ����485�����յ����������ݰ�
*Input          :  ��
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void proc_token_packet(void)
{
    /*�ظ�Ѳ���������TMS���µ�վ����Ϣ
     Ŀ���豸id:  Ϊ�����������Ѳ��id
     �ظ������֣�00��ʾtms���ݸı䣬 08��ʾ���ϰ�tms����һ��*/
    if (tba_tms_to_pis.buf[11] & 0x18) //�п������ź�
    {
        tba_num_repeat_send = 5;
    }
    
    if (tba_num_repeat_send != 0) /*�ظ�������*/
    {
        tba_num_repeat_send--;
        tba_bus_send_data(tba_recv_packet->src_net_id, tba_recv_packet->src_dev_id, 0x00, tba_tms_to_pis.buf, tba_tms_to_pis.len);
    }
    else                        /*û��������,�ظ��ϰ�����*/
    {
        tba_bus_send_data(tba_recv_packet->src_net_id, tba_recv_packet->src_dev_id, 0x08, tba_tms_to_pis.buf, 0);
    }
}
/***********************************************************************************************************************
*Function Nane  :  proc_deal_packet
*Description    :  ����485�����յ������ݰ�
*Input          :  ��
*Output         :  ��
*Return         :  ��
*Others         :  ��
***********************************************************************************************************************/
void proc_deal_packet(void)
{
    tRtcTimer time;
    get_cur_time(&time);
    
    if (atc_tba.comm_rigth)
    {
        if (check_timer(&atc_tba.comm_rigth_time, 5000))
        {
            if (atc_tba.normal)
            {
                atc_tba.normal = 0;
                form_normal_frame(TBA , 2, time, NULL, 0);
                memset(old_tba_buf, 0x00, TBA_FRAME_LEN);
            }
            
            sd_fflush(TBA, FRAMEOK);
            atc_tba.comm_rigth = 0;
        }
    }
    
    if (atc_tba.comm_flag)
    {
        if (check_timer(&atc_tba.comm_time, 40000))
        {
            //ʧȥͨ��40s�󣬽��д������ݴ���
            if (atc_tba.error)
            {
                atc_tba.error = 0;
                form_error_frame(TBA, 2, time, NULL, 0);
            }
            
            sd_fflush(TBA, FRAMEERR);
            //�����ϱ�ATC��train485֮���Ƿ�ͨ��
            atc_tba.comm_flag = 0;
        }
    }
    
    if (1 == tba_bus_get_packet())
    {
        atc_tba.comm_rigth_time = get_time();
        atc_tba.comm_rigth = 1;
        save_tba_data_sd(TBAOK);
        
        //�����ATC û���յ�TMS���ݣ��ʲ�Ӧ��PISC��Ѳ��
        if (get_tms_state() == 0)
        {
            tba_bus_recv.len = 0;
            tba_bus_data.len = 0;
            return;
        }
        
        //�жϰ�����
        //�����֣�00��ʾPISC���豸״̬�ı䣬 08��ʾ���ϰ�����һ��
        switch (tba_recv_packet->bus_cmd.packet_type)
        {
            case TBA_CMD_ATC_DATA :  /*08: ��ʾ���ϰ�����һ������ʾ������豸״̬���� ATC�ظ�ǰһ�����ݸ�TMS*/
            {
            } break;
            
            case TBA_CMD_DEFAULTPACKET: /*00��ʾPISC���豸״̬�ı�(���ﲻ�Ǳ�ʾ��������)*/
            {
#if 0
                addr = get_device_addr();
                addr = (addr << 4) | 0x01;
                
                //�жϱ����豸��Կ���ź�
                if ((tba_recv_packet->app_data[7] & 0x02)
                    && (addr == tba_recv_packet->src_dev_id))
                {
                    activate = 1;
                    activate_time = get_time();
                }
                
#endif
            }
            break;
            
            case TBA_CMD_TOKENPACKET :  /*���ư�, Ӧ��PISC*/
            {
                proc_token_packet();
                train485_token_timer = get_time();
            }
            break;
            
            default://������
                break;
        }
        
        tba_bus_recv.len = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  get_train485_state
*Description      :  ȡ��train485��״̬
*Input              :  ��
*Output            :  ��
*Return            :  !0 : ���� 0 �쳣
*Others            :  ��
***********************************************************************************************************************/
uint8 get_train485_state(void)
{
    return atc_tba.comm_rigth;
}
/***********************************************************************************************************************
*Function Nane  :  get_train485_comm
*Description      :  ȡ��train485��ͨ��״̬
*Input              :  ��
*Output            :  ��
*Return            :  !0 : ��ͨ�� 0 ��ͨ��
*Others            :  ��
***********************************************************************************************************************/
uint8 get_train485_comm(void)
{
    return atc_tba.comm_flag;
}
/***********************************************************************************************************************
*Function Nane  :  set_tba_num_repeat
*Description      :  �����Ƿ��ӡtba����
*Input              :  num ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void set_tba_num_repeat(uint8 num)
{
    tba_num_repeat_send = num;
}

/***********************************************************************************************************************
*Function Nane  :  save_tms_data
*Description      :  ����TMS������
*Input              :  buf ����
                          len ����
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
void save_tms_data(uint8 *buf, uint16 len)
{
    if (TMS_BUF_SIZE <= len)
    {
        return;
    }
    
    memcpy(tba_tms_to_pis.buf, buf, len);
    tba_tms_to_pis.len = len;
}
/***********************************************************************************************************************
*Function Nane  :  read_tba_bus_data
*Description      :  ��ȡ485���ߵ�����
*Input              :  ��
*Output            :  ��
*Return            :  ��
*Others            :  ��
***********************************************************************************************************************/
//static u16 tba_data_pcount = 0;
//static u8 start_sd_data_flag =0;
//extern _tsd_mode  sd_form[3];
int  read_tba_bus_data(void)
{
    uint8 data;
    uint16 len = 0;
    tRtcTimer time;
    get_cur_time(&time);
    len =  uart_recv_len(UART_TBA);
    tba_bus_recv.len = 0;
    
    if (0 < len)
    {
        while ((1 == uart_read(UART_TBA, &data, 1)))    //ȡ��������
        {
            tba_bus_recv.buf[tba_bus_recv.len] = data;
            tba_bus_recv.len++;
        }
        
        //tcms_usb_debug(HEX,tba_bus_recv.buf,tba_bus_recv.len);
        save_tba_data_sd(TBAOK);
#if 0
        
        if (0 == tba_data_pcount)
            start_sd_data_flag = 1;
            
        //memcpy(tba_data_buf[tba_data_pcount],data,len);
        //memcpy(sd_form[TBA].buf[tba_data_pcount], data, len);
        //tba_data_pcount +=len;
        
        //write sd
        if (1 == start_sd_data_flag)
        {
            form_normal_frame(TBA , 1, time, data, len);
            start_sd_data_flag = 0;
            tba_data_pcount += len;
        }
        else
        {
            memcpy(&sd_form[TBA].buf[tba_data_pcount], data, len);
            form_normal_frame(TBA , 2, time, NULL, len);
            tba_data_pcount += len;
            
            if (FILEBUFLEN < tba_data_pcount)
            {
                tba_data_pcount  -=  FILEBUFLEN;
                start_sd_data_flag = 1;
            }
        }
        
#endif
    }
    
    return 0;
}
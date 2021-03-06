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

/*接收train485信息*/
ttba_info tba_bus_recv;
ttba_info tba_bus_data;

uint8      old_tba_buf[TBA_FRAME_LEN] = {0};

/*接收数据包的指针*/
tba_packet_t *tba_recv_packet  = (tba_packet_t *)tba_bus_data.buf;
tba_packet_t  tba_send_packet  ;


tTMS_INFO   tba_tms_to_pis;       /*  tms发送，要发往pis的数据*/

uint8  tba_num_repeat_send;

uint8  train485_get_pisc_token = 0;
uint32 train485_token_timer = 0;

tstate   atc_tba;                    //atc与tba通信状态

extern uint8 _gtmp_buf[256];

extern uint8 ATC_id;

/***********************************************************************************************************************
*Function Nane  :  send_tba_ptu
*Description      :  发送tba数据到ptu
*Input              :  无
*Output            :  无
*Return            :  无
*Others            :  无
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
*Description    :  转码
*Input          :  src  转码前数据
                      slen 转码前数据长度
*Output         :  dest 转码后的数据
                       dest 转码后的数据长度
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void tba_transcod(uint8 *src, uint16 slen, uint8 *dest, uint16 *dlen)
{
    uint16 src_pos  = 1;
    uint16 dest_pos = 1;
    //帧头和帧尾不转码
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
//*函数作用:应用层封装要发送的数据
//*参数说明:app_buf--应用层数据缓冲区指针
//*注意事项:
//*返回说明:无
//*********************************************
void tba_bus_send_data(uint8 dst_net_id, uint8 dst_dev_id, uint8 data_type, uint8 *data_buf, uint16 length)
{
    uint8  buf[512] = {0};
    uint8  app_len = 0;
    uint16 size = 0;
    //帧头
    tba_send_packet.head  =  TBA_PACKET_TAB;
    //填充目的网络号、设备号
    tba_send_packet.dst_net_id = dst_net_id;//目的网络号
    tba_send_packet.dst_dev_id = dst_dev_id;//目的设备ID
    //填充源端网络号、设备号
    tba_send_packet.src_net_id = 0x00;
    tba_send_packet.src_dev_id = ATC_id;
    //送总线命令字，包括发送方还令牌(bit 4)
    tba_send_packet.bus_cmd.packet_type = data_type;
    tba_send_packet.bus_cmd.token_free  = 0;//token_return bit == 1,发送方还令牌
    tba_send_packet.bus_cmd.m_reserve   = 0;
    
    //送应用层数据
    if (250 < length)
    {
        tba_send_packet.app_length = 250;//长度超过判断
    }
    else
    {
        tba_send_packet.app_length = length;
    }
    
    app_len = tba_send_packet.app_length;
    //有效数据
    memcpy(&tba_send_packet.app_data, data_buf, app_len);
    //校验码
    get_cs(&tba_send_packet.dst_net_id, 6 + app_len, &tba_send_packet.app_data[app_len]);
    //帧尾
    tba_send_packet.app_data[app_len + 1] = TBA_PACKET_TAB;
    //转码
    tba_transcod(&tba_send_packet.head, 7 + app_len + 2, buf, &size);
    //发送
    uart_write(UART_TBA, buf, size);
}
/***********************************************************************************************************************
*Function Nane  :  save_tba_data_sd
*Description      :  存储tba的数据至sd卡
*Input              :  type TBAOK  正确的帧 TBAERR  错误的帧
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void save_tba_data_sd(uint8 type)
{
    tRtcTimer time;
    get_cur_time(&time);
    
    if (TBAOK  == type)
    {
        //记录错误帧的结束时间
        if (1 == atc_tba.error)
        {
            atc_tba.error = 0;
            form_error_frame(TBA, 2, time, 0, 0);
        }
        
#if 1        //判断帧数据是否与上一帧数据一样
        
        if (memcmp(old_tba_buf, tba_bus_recv.buf, tba_bus_recv.len))
        {
            //不一样记录上帧的结束时间 ，本帧的开始时间
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
            atc_tba.num_repeat++; //帧的重复次数增1
        }
    }
    else if (TBAERR == type)
    {
        //记录错误帧的开始时间或者数据
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
*Description      :  转码并检查校验码是否正确
*Input              :  无
*Output            :  无
*Return            :  1:成功 0 :错误
*Others            :  无
***********************************************************************************************************************/
uint8 reverse_code_packet(void)
{
    uint16 i = 0;
    uint16 j = 0;
    uint8 cs = 0;
    //帧头
    tba_bus_data.buf[0] = tba_bus_recv.buf[0];
    
    for (j = 1, i = 1; i < tba_bus_recv.len - 1; i++)
    {
        tba_bus_data.buf[j] = tba_bus_recv.buf[i];
        
        //转码
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
//*函数作用:列车总线接收数据包
//*参数说明:
//*注意事项:接收数据包被保存在tba_bus_recv_buf[]数组中
//*返回说明:
//*********************************************************
uint8 tba_bus_get_packet(void)
{
    uint16   size = 0;
    uint8    data = 0;
    static uint8  head_flag = 0; //有帧头标志位
    
    while ((1 == uart_read(UART_TBA, &data, 1)) && (size < 512))
    {
        atc_tba.comm_flag = 1;
        atc_tba.comm_time = get_time();
        size ++;
        
        //帧头或者帧尾
        if (TBA_PACKET_TAB == data)
        {
            //收到的字节小于7个，肯定错误
            if (7 <= tba_bus_recv.len)
            {
                tba_bus_recv.buf[tba_bus_recv.len] = data;
                tba_bus_recv.len++;
                head_flag = 0;
                
                if (1 == reverse_code_packet())
                {
                    //发往本ATC模块的数据
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
                        //不是发往ATC模块的数据
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
            //帧头帧尾之间的数据
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
*Description    :  处理485总线收到的令牌数据包
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
***********************************************************************************************************************/
void proc_token_packet(void)
{
    /*回复巡检的内容是TMS最新的站点信息
     目的设备id:  为中央控制器的巡检id
     回复命令字：00表示tms数据改变， 08表示与上包tms数据一样*/
    if (tba_tms_to_pis.buf[11] & 0x18) //有开关门信号
    {
        tba_num_repeat_send = 5;
    }
    
    if (tba_num_repeat_send != 0) /*回复新数据*/
    {
        tba_num_repeat_send--;
        tba_bus_send_data(tba_recv_packet->src_net_id, tba_recv_packet->src_dev_id, 0x00, tba_tms_to_pis.buf, tba_tms_to_pis.len);
    }
    else                        /*没有新数据,回复上包数据*/
    {
        tba_bus_send_data(tba_recv_packet->src_net_id, tba_recv_packet->src_dev_id, 0x08, tba_tms_to_pis.buf, 0);
    }
}
/***********************************************************************************************************************
*Function Nane  :  proc_deal_packet
*Description    :  处理485总线收到的数据包
*Input          :  无
*Output         :  无
*Return         :  无
*Others         :  无
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
            //失去通信40s后，进行错误数据处理
            if (atc_tba.error)
            {
                atc_tba.error = 0;
                form_error_frame(TBA, 2, time, NULL, 0);
            }
            
            sd_fflush(TBA, FRAMEERR);
            //用于上报ATC与train485之间是否通信
            atc_tba.comm_flag = 0;
        }
    }
    
    if (1 == tba_bus_get_packet())
    {
        atc_tba.comm_rigth_time = get_time();
        atc_tba.comm_rigth = 1;
        save_tba_data_sd(TBAOK);
        
        //本侧的ATC 没有收到TMS数据，故不应答PISC的巡检
        if (get_tms_state() == 0)
        {
            tba_bus_recv.len = 0;
            tba_bus_data.len = 0;
            return;
        }
        
        //判断包类型
        //命令字：00表示PISC下设备状态改变， 08表示与上包数据一样
        switch (tba_recv_packet->bus_cmd.packet_type)
        {
            case TBA_CMD_ATC_DATA :  /*08: 表示与上包数据一样，表示下面的设备状态不变 ATC回复前一包数据给TMS*/
            {
            } break;
            
            case TBA_CMD_DEFAULTPACKET: /*00表示PISC下设备状态改变(这里不是表示过程数据)*/
            {
#if 0
                addr = get_device_addr();
                addr = (addr << 4) | 0x01;
                
                //判断本侧设备有钥匙信号
                if ((tba_recv_packet->app_data[7] & 0x02)
                    && (addr == tba_recv_packet->src_dev_id))
                {
                    activate = 1;
                    activate_time = get_time();
                }
                
#endif
            }
            break;
            
            case TBA_CMD_TOKENPACKET :  /*令牌包, 应答PISC*/
            {
                proc_token_packet();
                train485_token_timer = get_time();
            }
            break;
            
            default://其他包
                break;
        }
        
        tba_bus_recv.len = 0;
    }
}
/***********************************************************************************************************************
*Function Nane  :  get_train485_state
*Description      :  取得train485的状态
*Input              :  无
*Output            :  无
*Return            :  !0 : 正常 0 异常
*Others            :  无
***********************************************************************************************************************/
uint8 get_train485_state(void)
{
    return atc_tba.comm_rigth;
}
/***********************************************************************************************************************
*Function Nane  :  get_train485_comm
*Description      :  取得train485的通信状态
*Input              :  无
*Output            :  无
*Return            :  !0 : 有通信 0 无通信
*Others            :  无
***********************************************************************************************************************/
uint8 get_train485_comm(void)
{
    return atc_tba.comm_flag;
}
/***********************************************************************************************************************
*Function Nane  :  set_tba_num_repeat
*Description      :  设置是否打印tba数据
*Input              :  num 次数
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_tba_num_repeat(uint8 num)
{
    tba_num_repeat_send = num;
}

/***********************************************************************************************************************
*Function Nane  :  save_tms_data
*Description      :  保存TMS的数据
*Input              :  buf 数据
                          len 长度
*Output            :  无
*Return            :  无
*Others            :  无
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
*Description      :  获取485总线的数据
*Input              :  无
*Output            :  无
*Return            :  无
*Others            :  无
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
        while ((1 == uart_read(UART_TBA, &data, 1)))    //取串口数据
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

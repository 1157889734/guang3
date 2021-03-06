#ifndef __TBA_BUS_H__
#define __TBA_BUS_H__

#include "config.h"

//通信协议特殊字符
#define TBA_PACKET_TAB          0x7E    //数据包分割符
#define TBA_PACKET_DLE          0x7F    //数据包全局转码符
#define TBA_PACKET_DLE_TAB      0x80    //数据包分割符的转码符
#define TBA_PACKET_DLE_DLE      0x81    //数据包转码符的转码符

//总线命令类型
#define TBA_CMD_DEFAULTPACKET   0x00        //过程数据包
#define TBA_CMD_P2PPACKET       0x01        //点对点命令
#define TBA_CMD_TOKENPACKET     0x02        //令牌包,用于巡检
#define TBA_CMD_TOKENCLEAR      0x03        //清除所有令牌的包
#define TBA_CMD_UPGRADE         0x04        //过程数据升级程序

#define TBA_CMD_ATC_DATA        0x08    // Added by Lison on 2013-1-17 15:50:34 

#define  TBAOK    1
#define  TBAERR   2


#define TBA_FRAME_LEN   9



//缓冲区长度定义--根据具体应用改变
#define TBA_BUF_SIZE        512
#define  UART_TBA           UART2

typedef struct  TBA_INFO
{
    uint16 len;
    uint8   buf[TBA_BUF_SIZE];
} ttba_info;



//列车总线数据包结构
__packed typedef struct
{
    uint8 head;
    uint8 dst_net_id;
    uint8 dst_dev_id;
    uint8 src_net_id;
    uint8 src_dev_id;
    //列车总线命令字结构---低位在前
    __packed struct BUS_CMD
    {
        uint8 packet_type: 4; //包类型
        uint8 token_free: 1; //发送方释放令牌的标志
        uint8 m_reserve: 3; //保留
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
*Description      :  取得train485的状态
*Input              :  无
*Output            :  无
*Return            :  !0 : 正常 0 异常
*Others            :  无
***********************************************************************************************************************/
uint8 get_train485_state(void);

/***********************************************************************************************************************
*Function Nane  :  get_train485_comm
*Description      :  取得train485的通信状态
*Input              :  无
*Output            :  无
*Return            :  !0 : 有通信 0 无通信
*Others            :  无
***********************************************************************************************************************/
uint8 get_train485_comm(void);

/***********************************************************************************************************************
*Function Nane  :  set_tba_debug
*Description      :  设置是否打印tba数据
*Input              :  type 0 不打印 1 打印
*Output            :  无
*Return            : 无
*Others            :  无
***********************************************************************************************************************/
void set_tba_send(uint8 type);
/***********************************************************************************************************************
*Function Nane  :  set_tba_num_repeat
*Description      :  设置是否打印tba数据
*Input              :  num 次数
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void set_tba_num_repeat(uint8 num);

/***********************************************************************************************************************
*Function Nane  :  save_tms_data
*Description      :  保存TMS的数据
*Input              :  buf 数据
                          len 长度
*Output            :  无
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
void save_tms_data(uint8 *buf, uint16 len);


int read_tba_bus_data(void);
#endif



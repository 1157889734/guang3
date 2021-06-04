#ifndef __CMD_H
#define __CMD_H

#include"config.h"

#define PTU_BUF_LEN    512      //PTU数据缓冲长度
#define FRAME_BUF_LEN  256      //数据帧长度

#define MAXPATH        50       //路径最大长度

#define CMD_UPGRADE      0x01   //升级命令
#define CMD_VERSION      0x02   //查询版本命令
#define CMD_SHOWNET      0x03   //显示网络参数
#define CMD_SETNET       0x04   //设置网络ip 网关
#define CMD_SETMAC       0x05   //设置网络MAC
#define CMD_SETTIME      0x06   //设置时间
#define CMD_GETTIME      0x07   //查询时间
#define CMD_GETSTATE     0x08   //查询状态
#define CMD_SDINFO       0x09   //查询SD卡状态
#define CMD_GETFILE      0x0a   //读取文件
#define CMD_CANCEL       0x0b   //取消读取文件命令
#define CMD_FORMATSD     0x0c   //格式化SD卡
#define CMD_ERMOVEFILE   0x0d   //删除文件
#define CMD_REBOOT       0x0e   //复位模块
#define CMD_USB_DEBUG    0x0f   //usb打印命令
#define CMD_HEARTBEAT    0xff   //ptu心跳

#define SENDSTATE       0x01    //发送模块状态命令
#define SENDTMS         0x02    //发送TCMS数据命令
#define SENDPIS         0x03    //发送PISC数据命令
#define SENDTBA         0x04    //发送TBA数据命令

#define CANCELTMS       0x12    //取消发送TCMS数据命令
#define CANCELPIS       0x13    //取消发送PISC数据命令
#define CANCELTBA       0x14    //取消发送 TBA数据命令


#define PTU_PACKET_TAB          0x7E    //数据包帧头
#define PTU_PACKET_DLE          0x7F    //数据包全局转码符
#define PTU_PACKET_DLE_TAB      0x80    //数据包分割符的转码符
#define PTU_PACKET_DLE_DLE      0x81    //数据包转码符的转码符


#define ERR_NO_FILE    0  //文件不存在错误
#define ERR_USE_FILE   1  //文件在使用中错误


#define PTU_NORMAL    0  //正常状态
#define PTU_GETFILE   1  //读取文件状态
#define PTU_COPYSD    2  //拷贝SD状态
#define PTU_NO_COMM   3

#define READY   0        //读取文件准备
#define GOING   1        //文件读取中
#define FINISH  2        //文件读取结束
#define CANCEL  3        //取消读取文件

#define TCMS_DEBUG 1
#define PISC_DEBUG 2
#define TBA_DEBUG  3

//PTU数据接收数据结构
typedef struct PTU_INFO
{
    uint16 len;               //数据长度
    uint8  buf[PTU_BUF_LEN];  //数据
} tptu_info;

//PTU通信协议数据帧结构
typedef struct FREAM_INFO
{
    uint8 head;              //帧头
    uint8 len;               //字长
    uint8 cmd;               //命令字
    uint8 buf[FRAME_BUF_LEN];//数据
} tframe_info;

//读取文件信息结构体
typedef struct TPTU_GETFILE
{
    char   path[50];         //文件路径
    uint8  flag;             //状态标识
    uint16 index;            //包序号
    uint32 file_size;        //文件大小
    uint32 get_size;         //已读取的大小
} tptu_getfile;
//PTU信息结构体
typedef struct PTU_STATES
{
    uint8 states;           //PTU状态
    uint8 comm_flag;
    uint32 time;            //最近通信时间
    tptu_getfile getfile;
} tptu_states;

/***********************************************************************************************************************
*Function Nane  :  get_cs
*Description      :  计算数据的校验和
*Input              :  pdata   指向数据
                          length  数据长度
*Output            :  cs 指向校验和
*Return            :  无
*Others            :  无
***********************************************************************************************************************/
uint8 get_cs(uint8 *pdata,  uint16  length,  uint8 *cs);

#endif


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tba_m.h

//*文件作用:列车通信总线管理器

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TBA_M_H
#define TBA_M_H

#define pisc_token_num  		2      // 巡检pisc 数量
#define dcp_token_num  		2		 // 巡检dcp 数量
#define lcu_token_num   		6		 // 巡检lcu 数量
#define io_token_num    			6		 // 巡检io 数量


#define TBA_M_ACK_TIMEOUT	350	//巡检应答超时
//modify by hk   100-->200
#define TBA_M_WARNING_TIMEOUT	200	//巡检应答超时警告
#define TBA_M_TABLE_NUM		32	//巡检表总数


//接口函数
extern void tba_m_init(uint8 net_id, uint8 dev_id);
extern void tba_m_proc(tba_packet_t *packet);
extern void tba_m_set_table(uint8 *dev_id, uint16 *interval);
extern uint8 tba_m_get_quality(uint8 *dev_id, uint16 *total, uint16 *error);
extern void tba_m_send_token(uint8 dev_id);

extern uint8 send_token_cnt;
//令牌巡检表结构
__packed typedef struct
{
	uint8 dev_id;//设备号
	uint16 interval;//巡检间隔 0xFFFF--代表立即巡检  0--代表不参与巡检
	uint32 timer;//巡检间隔定时器
	uint16 poll_total;//巡检总数---通信质量分母
	uint16 poll_error;//错误的巡检数量---通信质量分子
	uint8 poll_continue_timeout;//连续超时的巡检数量 如果连续巡检超时 降低这个设备的优先级
      uint32 time_out;

	uint8 poll_flag; //  1:已发送巡检令牌
	uint8 poll_interval_time;   //巡检间隔次数   在此次数内没收到回复则重发令牌
}token_polling_table_t; 

extern token_polling_table_t token_polling_pisc_table[pisc_token_num];
extern token_polling_table_t token_polling_dcp_table[dcp_token_num];
extern token_polling_table_t token_polling_lcu_table[lcu_token_num];
extern token_polling_table_t token_polling_io_table[io_token_num];

#endif



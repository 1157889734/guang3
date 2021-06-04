//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 客室控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:lcu.c

//*文件作用:客室控制器

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//8个客室控制器过程数据结构
lcu_default_data_t lcu_default_data[TBA_LCU_NUM]=
{
//      1    2    3     4    5    6     7    8
	{{0},{0},{0},{0},{0},{0},{0}},
	{{0},{0},{0},{0},{0},{0},{0}},
	{{0},{0},{0},{0},{0},{0},{0}},
	{{0},{0},{0},{0},{0},{0},{0}},
	{{0},{0},{0},{0},{0},{0},{0}},
	{{0},{0},{0},{0},{0},{0},{0}},
};

//8个客室控制器通信错误
uint16 lcu_comm_error[TBA_LCU_NUM]={0};



//***********************************************************************************************************************
//函数作用:客室控制器数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void lcu_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
	//过程数据
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		//判断长度
		if(length==sizeof(lcu_default_data_t))
		{		
			uint8 dev_no=src_dev_id>>4;
			
			//判断源设备编号
			if( (dev_no) && (dev_no<=TBA_LCU_NUM) )
			{
				//清除通信错误
				lcu_comm_error[dev_no-1]=0;
				
				//拷贝数据
				memcpy( (uint8 *)&lcu_default_data[dev_no-1], packet, sizeof(lcu_default_data_t) );
				//usb_write_buf((uint8 *)&lcu_default_data[dev_no-1], sizeof(lcu_default_data_t));
			}
		}
	}
	//点对点
	else if(data_type==TBA_CMD_P2PPACKET)
	{
		
	}
}

//***********************************************************************************************************************
//函数作用:LCU获取对讲状态
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void lcu_get_icc_state(icc_status_t *status)
{
	uint8 index;
	
	for(index=0; index<TBA_LCU_NUM; index++)
	{		
		//只有在挂断状态下才能更新为LCU的呼叫
		status->pecu1=lcu_default_data[index].icc_status.pecu1;
		status->pecu2=lcu_default_data[index].icc_status.pecu2;
		status->pecu3=lcu_default_data[index].icc_status.pecu3;
		status->pecu4=lcu_default_data[index].icc_status.pecu4;
					
		status++;
	}
}

//***********************************************************************************************************************
//函数作用:LCU获取对讲是否连接
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
uint8 lcu_get_icc_connect(void)
{
	uint8 index;
	
	for(index=0;index<TBA_LCU_NUM;index++)
	{		
		//送紧急通信连接标志到TMS
		if(lcu_default_data[index].icc_status.pecu1==TBA_LCU_PECU_CONNECT)
			return (index*3)+1;
		if(lcu_default_data[index].icc_status.pecu2==TBA_LCU_PECU_CONNECT)
			return (index*3)+2;
		if(lcu_default_data[index].icc_status.pecu3==TBA_LCU_PECU_CONNECT)
			return (index*3)+3;
	}
	
	return 0;
}


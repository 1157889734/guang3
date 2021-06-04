//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 司机室通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:CCTV.c

//*文件作用:列车通信总线接口

//*文件作者:王会斌

//*创建日期:200年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include  "include.h"

//接收缓冲区
static uint8 cctv_recv_buf[CCTV_BUF_SIZE]={0};
static uint8 cctv_recv_length=0;

//

__packed typedef struct
{
	//uint8 length;
	//uint8 device_type;
	//uint8 device_no;
	uint8 cmd;
	uint16 cctv_status;
	
}cab_bus_cctv_t;


cab_bus_cctv_t *cab_cctv_data=(cab_bus_cctv_t *)&cctv_recv_buf;

//通讯错误处理
uint8 cctv_comm_err[CCTV_NUM]={0};



//***********************************************************************************************************************
//函数作用:获得CCTV数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint8 cctv_send_data(uint8 cmd, uint8 *param, uint8 length)
{ 
	uint8 send_buf[128];
	send_buf[0]=cmd;
	if(length>127)return 0;   //超出缓冲区长度，返回0
	
	memcpy((uint8 *)&send_buf[1],param,length);
	
	cab_bus_dev->send_data(0x06,0x01,1,length+1,send_buf);
	
	return 1;

}

//***********************************************************************************************************************
//函数作用:CCTV数据处理
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void cctv_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{
	
	cctv_comm_err[device_no-1]=0;
	memcpy(cctv_recv_buf,buf,data_length);
	
	//MEDIA_SEND_BUF(buf,data_length);

}

//***********************************************************************************************************************
//函数作用:获得CCTV数据
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
static uint16 get_cctv_data(void)
{
	uint16 temp=cab_cctv_data->cctv_status;
	cab_cctv_data->cctv_status=0;
	return temp;

}

//输出设备类
cctv_dev_t sh11_cctv_dev=
{
	get_cctv_data,
	cctv_send_data,
	{
	0,
	{0},
	}

};
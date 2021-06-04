//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ˾����ͨ�����߽ӿ� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:CCTV.c

//*�ļ�����:�г�ͨ�����߽ӿ�

//*�ļ�����:�����

//*��������:200��3��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include  "include.h"

//���ջ�����
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

//ͨѶ������
uint8 cctv_comm_err[CCTV_NUM]={0};



//***********************************************************************************************************************
//��������:���CCTV����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 cctv_send_data(uint8 cmd, uint8 *param, uint8 length)
{ 
	uint8 send_buf[128];
	send_buf[0]=cmd;
	if(length>127)return 0;   //�������������ȣ�����0
	
	memcpy((uint8 *)&send_buf[1],param,length);
	
	cab_bus_dev->send_data(0x06,0x01,1,length+1,send_buf);
	
	return 1;

}

//***********************************************************************************************************************
//��������:CCTV���ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void cctv_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{
	
	cctv_comm_err[device_no-1]=0;
	memcpy(cctv_recv_buf,buf,data_length);
	
	//MEDIA_SEND_BUF(buf,data_length);

}

//***********************************************************************************************************************
//��������:���CCTV����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint16 get_cctv_data(void)
{
	uint16 temp=cab_cctv_data->cctv_status;
	cab_cctv_data->cctv_status=0;
	return temp;

}

//����豸��
cctv_dev_t sh11_cctv_dev=
{
	get_cctv_data,
	cctv_send_data,
	{
	0,
	{0},
	}

};
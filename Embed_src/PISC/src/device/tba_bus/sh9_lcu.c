//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ���ҿ����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:lcu.c

//*�ļ�����:���ҿ�����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//8�����ҿ������������ݽṹ
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

//8�����ҿ�����ͨ�Ŵ���
uint16 lcu_comm_error[TBA_LCU_NUM]={0};



//***********************************************************************************************************************
//��������:���ҿ��������ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void lcu_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
	//��������
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		//�жϳ���
		if(length==sizeof(lcu_default_data_t))
		{		
			uint8 dev_no=src_dev_id>>4;
			
			//�ж�Դ�豸���
			if( (dev_no) && (dev_no<=TBA_LCU_NUM) )
			{
				//���ͨ�Ŵ���
				lcu_comm_error[dev_no-1]=0;
				
				//��������
				memcpy( (uint8 *)&lcu_default_data[dev_no-1], packet, sizeof(lcu_default_data_t) );
				//usb_write_buf((uint8 *)&lcu_default_data[dev_no-1], sizeof(lcu_default_data_t));
			}
		}
	}
	//��Ե�
	else if(data_type==TBA_CMD_P2PPACKET)
	{
		
	}
}

//***********************************************************************************************************************
//��������:LCU��ȡ�Խ�״̬
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void lcu_get_icc_state(icc_status_t *status)
{
	uint8 index;
	
	for(index=0; index<TBA_LCU_NUM; index++)
	{		
		//ֻ���ڹҶ�״̬�²��ܸ���ΪLCU�ĺ���
		status->pecu1=lcu_default_data[index].icc_status.pecu1;
		status->pecu2=lcu_default_data[index].icc_status.pecu2;
		status->pecu3=lcu_default_data[index].icc_status.pecu3;
		status->pecu4=lcu_default_data[index].icc_status.pecu4;
					
		status++;
	}
}

//***********************************************************************************************************************
//��������:LCU��ȡ�Խ��Ƿ�����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
uint8 lcu_get_icc_connect(void)
{
	uint8 index;
	
	for(index=0;index<TBA_LCU_NUM;index++)
	{		
		//�ͽ���ͨ�����ӱ�־��TMS
		if(lcu_default_data[index].icc_status.pecu1==TBA_LCU_PECU_CONNECT)
			return (index*3)+1;
		if(lcu_default_data[index].icc_status.pecu2==TBA_LCU_PECU_CONNECT)
			return (index*3)+2;
		if(lcu_default_data[index].icc_status.pecu3==TBA_LCU_PECU_CONNECT)
			return (index*3)+3;
	}
	
	return 0;
}


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ����豸�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:panel.c

//*�ļ�����:����豸��

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//ͨ��Э��ؼ���
#define PANEL_PACKET_HEAD		0x7E	//����ͷ
#define PANEL_PACKET_END		0x0D	//����β

//���յ�����
#define PANEL_PACKET_KEY		0x04	//�յ����̰�




//���ջ�����
static uint8 panel_recv_buf[PANEL_BUF_SIZE];//���ջ�����
//static uint8 panel_recv_length=0;//�����ܳ���
//static uint8 panel_data_length=0;//���ݲ��ֵĳ���


//˽�б���
static uint32 panel_send_timer=0;//��ʱ���Ͷ�ʱ��
static uint8 panel_volume=0;//����
static uint8 panel_mode=0;//ģʽ
static uint8 panel_mode_backup=0;//����ģʽ


//***********************************************************************************************************************
//��������:��巢������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void panel_send_cmd(uint8 cmd, uint8 *param, uint8 length)
{
     #if 0  //delete by hk
	//���ͻ�����
	uint8 panel_send_buf[PANEL_BUF_SIZE];
	uint8 panel_send_length=0;
	//У���
	uint8 checksum=cmd;
	
	panel_send_length=0;
	panel_send_buf[panel_send_length++]=PANEL_PACKET_HEAD;//ͷ
	panel_send_buf[panel_send_length++]=length+1;//����
	panel_send_buf[panel_send_length++]=cmd;//������
	//����
	if(length)
	{
		memcpy(&panel_send_buf[panel_send_length],param,length);
		panel_send_length+=length;
	}
	while(length--)
		checksum+=*param++;
	panel_send_buf[panel_send_length++]=checksum;//У���
	panel_send_buf[panel_send_length++]=PANEL_PACKET_END;//β
	
	//����
	//PANEL_SEND_BUF(panel_send_buf,panel_send_length);
#endif

      //add by hk
	uint8 buf[PANEL_BUF_SIZE]={0};
	buf[0]=cmd;
	if(length>PANEL_BUF_SIZE)
	{

		return;
	}		
	else 
	{
	    memcpy((uint8 *)&buf[1],param,length);
	}
	
	cab_bus_dev->send_data(CAB_PANEL,0x0f,1,length+1,buf);
}

//***********************************************************************************************************************
//��������:����������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
#if 0
static uint8 panel_get_packet(void)
{
	static uint8  start_flag=0;
	static uint16 timeout=0;
	uint8 temp;
	uint8 checksum;
	uint8 i;
	
	//����ֱ��û������
	while(PANEL_RECEIVE_CHAR(&temp))
	{		
		//û���յ���ʼ����ʱ�� �ж��ǲ�����ʼ��
		if(!start_flag)
		{
			//�������ʼ�� �ͱ�־������ײ�fifo��ָ��---�Ա㷢�ּ���ʼ����ʱ��Ļָ�
			if(temp==PANEL_PACKET_HEAD)
			{
				start_flag=1;
				PANEL_FIFO_BACKUP();
			}
			//�����ַ� �����Զ�������
			else
			{
				
			}
		}
		//����Ѿ��յ���ʼ�� �ж��ǲ�����������ʼ��
		else
		{			
			//�ոս��յ���ʼ�� �Ƚ��յڶ����ֽ�(����) �ѵ�ǰ�ֽڼ���Ϊ����
			if(start_flag==1)
			{
				start_flag++;
				panel_data_length=temp;
				timeout=2000;//���ý��ճ�ʱ
			}
			//�Ѿ����յ����� �Ͱ����ݷ��뻺���� ������ϼ���У���
			else
			{
				//û�н������ �ͻ�����
				if(panel_recv_length < panel_data_length)
				{
					panel_recv_buf[panel_recv_length++] = temp;//�򻺴�������
					if(panel_recv_length >= PANEL_BUF_SIZE-1)
						panel_recv_length = 0;//�ж��Ƿ񳬳�	
				}		
				//������� ��ǰ�ֽھ���У��� ���У���
				else
				{
					//���־
					start_flag=0;
					timeout=0;
					panel_recv_length=0;
					//����У���
					checksum=0;
					for(i=0; i<panel_data_length; i++)
						checksum += panel_recv_buf[i];
					//У��ʹ��� ��ǰ��ʼ���Ǽٵ� �ָ��ײ�fifo��ָ��
					if(checksum != temp)
					{
						PANEL_FIFO_RESUME();
						print_line("dva checksum error");
					}
					//У�����ȷ ��������
					else
					{
						return 1;
					}
				}
			}
		}
	}
	
	//�жϽ��ճ�ʱ
	if(timeout)
	{
		//��ʱ˵�� ��ǰ��ʼ���Ǽٵ�
		if(--timeout==0)
		{
			//���־
			start_flag=0;
			timeout=0;
			panel_recv_length=0;
			//��ǰ��ʼ���Ǽٵ� �ָ��ײ�fifo��ָ��
			PANEL_FIFO_RESUME();
			print_line("dva recv timeout");
		}
	}
	
	return 0;
}
#endif
/****************************************************
*��������panel_proc			       		         
*���ߣ�hk             							 
*������������ݴ���				
*���룺device_no       --- �豸��
                 data_length    ---���ݳ���
                 buf                ---���������
                  
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
 void panel_proc(uint8 device_no,uint8 data_length,uint8 *buf)
{
	//Ӧ�𻺳���
	uint8 ack_buf[]={0,0};
	
	memcpy((uint8 *)panel_recv_buf, buf, data_length);
	//��������
	//if(panel_get_packet())
	{
		//����
		if(panel_recv_buf[0]==PANEL_PACKET_KEY)
		{
			shzh24_panel_dev.refresh_timer=65500;
			//���ദ�����
			/*switch(panel_recv_buf[1])
			{
				//ģʽ��
				case KEY_PRESET:
					panel_mode=MODE_PRESET;
					break;
					
				case KEY_MUTE:
					panel_mode=MODE_MUTE;
					break;
					
				case KEY_MANUAL:
					panel_mode=MODE_MANUAL;
					break;
				
				case KEY_AUTO:
					panel_mode=MODE_AUTO;
					break;
				
				case KEY_TEST:
					//�жϽ�����˳�����ģʽ
					if(panel_mode!=MODE_TEST)
					{
						panel_mode_backup=panel_mode;
						panel_mode=MODE_TEST;
					}
					else
					{
						panel_mode=panel_mode_backup;
					}
					break;
				
				//������	
				case KEY_60DB:
					panel_volume=VOLUME_60DB;
					break;
					
				case KEY_70DB:
					panel_volume=VOLUME_70DB;
					break;
					
				case KEY_80DB:
					panel_volume=VOLUME_80DB;
					break;

				case KEY_90DB:
					panel_volume=VOLUME_90DB;
					break;
					
				case KEY_100DB:
					panel_volume=VOLUME_100DB;
					break;
							
				default:
					break;
			}
			*/
			panel_mode = panel_recv_buf[1];
			panel_volume = panel_recv_buf[2];
								
			//�жϲ����ı� �������
			if(((panel_mode<<4)|panel_volume)!=saveparam.panel_volume_param)
			{
				saveparam.panel_volume_param=(panel_mode<<4)|panel_volume;
				save_set_task();
				print_int("%d set panel volume:", saveparam.panel_volume_param);
			}
		}
		
		//Ӧ��---����ģʽ�����������ָʾ��
		ack_buf[0]=saveparam.panel_volume_param>>4;
		ack_buf[1]=saveparam.panel_volume_param&0x0F;
		panel_send_cmd(PANEL_ACK_LED,ack_buf,2);
	}
	
	//��ʱ����Ӧ��--��ֹ����
	if(checktimer(&panel_send_timer,800))
	{
		ack_buf[0]=saveparam.panel_volume_param>>4;
		ack_buf[1]=saveparam.panel_volume_param&0x0F;
		panel_send_cmd(PANEL_ACK_LED,ack_buf,2);			
	}
}

//***********************************************************************************************************************
//��������:����ʼ������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void panel_init(void)
{	
	//��ʼ��ͨ�Ŷ˿�
	PANEL_PORTINIT(PANEL_BAUDRATE);
	
	//װ�ز���
	panel_mode=saveparam.panel_volume_param>>4;
	panel_mode_backup=panel_mode;
	panel_volume=saveparam.panel_volume_param&0x0F;
}



//��������豸��
panel_dev_t shzh24_panel_dev=
{
	panel_init,
	panel_proc,
	
	0,//ˢ��ʱ�� ��������ͬ��
};


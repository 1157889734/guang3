
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& OCC &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:sh9_occ.c

//*�ļ�����:OCC 

//*�ļ�����:

//*��������:2010��3��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"




//ͨ��Э��ؼ���
#define OCC_PACKET_HEAD			0x7E	//����ͷ
#define OCC_PACKET_END				0x0D	//����β
#define OCC_PACKET_DEFAULT		0x01	//��������

#define OCC_ACK_DEFAULT			0x81	//��������Ӧ��



//���ջ�����
static uint8 occ_recv_buf[OCC_BUF_SIZE];//���ջ�����
static uint8 occ_recv_length=0;//�����ܳ���
static uint8 occ_data_length=0;//���ݲ��ֵĳ���
//���ͻ�����
static uint8 occ_send_buf[OCC_BUF_SIZE];//���ͻ�����
static uint8 occ_send_length=0;//�����ܳ���

static uint8 occ_cmd = 0;    //0:stop occ  1:start occ

//�������ݽṹ
__packed typedef struct
{
	uint8 cmd;
	uint8 DeviceState[8];	
}occ_recv_t;
static occ_recv_t *occ_recv=(occ_recv_t *)&occ_recv_buf;



//***********************************************************************************************************************
//��������:OCC ��������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 occ_get_packet(void)
{
	static uint8  start_flag=0;
	static uint16 timeout=0;
	uint8 temp;
	uint8 checksum;
	uint8 i;
	
	//����ֱ��û������
	while(OCC_RECEIVE_CHAR(&temp))
	{
		//û���յ���ʼ����ʱ�� �ж��ǲ�����ʼ��
		if( !start_flag )
		{
			//�������ʼ�� �ͱ�־������ײ�fifo��ָ��---�Ա㷢�ּ���ʼ����ʱ��Ļָ�
			if(temp == OCC_PACKET_HEAD)
			{
				start_flag = 1;
				//OCC_FIFO_BACKUP();
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
				occ_data_length=temp;
				timeout=2000;//���ý��ճ�ʱ
			}
			//�Ѿ����յ����� �Ͱ����ݷ��뻺���� ������ϼ���У���
			else
			{
				//û�н������ �ͻ�����
				if(occ_recv_length<occ_data_length)
				{
					occ_recv_buf[occ_recv_length++]=temp;//�򻺴�������
					if(occ_recv_length>=OCC_BUF_SIZE-1)
						occ_recv_length=0;//�ж��Ƿ񳬳�	
				}		
				
				//������� ��ǰ�ֽھ���У��� ���У���
				else if(temp == 0x0d)
				{
				
				start_flag=0;
				//uart0_write_char(0xaa);
				//uart0_write_char(occ_recv_buf[0]);
				//uart0_write_buf(occ_recv_buf, occ_recv_length);
				occ_recv_length=0;
				return 1;
				}
				else
				{
					     //OCC_FIFO_RESUME();
					     print_line("occ data error");
				}
					//���־
					start_flag=0;
					timeout=0;
					occ_recv_length=0;
					//����У���
					checksum=0;
					for(i=0;i<occ_data_length;i++)
						checksum^=occ_recv_buf[i];
					//У��ʹ��� ��ǰ��ʼ���Ǽٵ� �ָ��ײ�fifo��ָ��
					if(checksum!=temp)
					{
						OCC_FIFO_RESUME();
						print_line("occ checksum error");
					}
					//У�����ȷ ��������
					else
					{
						return 1;
					}
				
			}
		}
	}
	
	//�жϽ��ճ�ʱ
	/*if(timeout)
	{
		//��ʱ˵�� ��ǰ��ʼ���Ǽٵ�
		if(--timeout==0)
		{
			//���־
			start_flag=0;
			timeout=0;
			occ_recv_length=0;
			//��ǰ��ʼ���Ǽٵ� �ָ��ײ�fifo��ָ��
			OCC_FIFO_RESUME();
			print_line("occ recv timeout");
		}
	}
	*/
	return 0;
}

//***********************************************************************************************************************
//��������:OCC ���ݴ���
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void occ_proc(void)
{
       static uint16 num = 0;  //ָʾ����ʾ����
	//��������
	if(occ_get_packet())
	{
               num = 5000;
		//���ͨ�Ŵ���
		sh9_occ_dev.comm_error=0;

		if(occ_recv->cmd != 0x55 && occ_recv->cmd != 0xbb)
		{
		      //OCC�����쳣
		}
		else
		{
		    occ_send_buf[0] = 0x7e;
		    occ_send_buf[1] = 0x01;
		    if(occ_recv->cmd == 0x55)  //occ start
		    {
		    	   occ_cmd = 1; 
			   occ_send_buf[2] = 0xaa;  // �㲥Ӧ��
		    }
		    else //occ stop
		    {
		        occ_cmd = 0;
			 occ_send_buf[2] = 0xcc;
		    }
		    occ_send_buf[3] = 0x0d;
		    occ_send_length = 4;
		    //����Ӧ��
		   OCC_SEND_BUF(occ_send_buf, occ_send_length);
		}

	}
	else
		{
		//uart0_write_char(0x55);
		occ_recv->cmd = 0;
		//ͨ�ŵ�
		if(num == 0)
			led_onoff(OCC_LED_NO, 1);
		}
	if(num)
	{     num--;
	    	//ͨ�ŵ�
		//led_toggle(OCC_LED_NO);
		 led_blink(OCC_LED_NO,300,300);
	}
	//����ͨ�Ŵ������
	if( ++sh9_occ_dev.comm_error > COMM_DATA_TIMEOUT )
	{
		//sh9_occ_dev.comm_error = COMM_DATA_TIMEOUT;
		//memset((uint8 *)&sh9_occ_dev.device_error, 0xFF, 8);
	}
}

//***********************************************************************************************************************
//��������:OCC ��������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void occ_send_data(uint8 *buf, uint8 length)
{
	uint8 checksum=0;
	uint8 *p=&occ_send_buf[2];
	
	occ_send_length=0;
	//�����ʼ��
	occ_send_buf[occ_send_length++]=OCC_PACKET_HEAD;
	//�ͳ���
	occ_send_buf[occ_send_length++]=length+1;//��������ĳ���
	//�͹�������������
	occ_send_buf[occ_send_length++]=OCC_PACKET_DEFAULT;
	//��������
	memcpy(&occ_send_buf[occ_send_length], buf, length);
	occ_send_length += length;
	//����У���
	checksum=0;
	while(p < &occ_send_buf[occ_send_length])
		checksum ^= *p++;
	occ_send_buf[occ_send_length++] = checksum;
	//��ӽ�����
	occ_send_buf[occ_send_length++] =OCC_PACKET_END;
}

//***********************************************************************************************************************
//��������:OCC��ȡ����
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static uint8 occ_get_playing(void)
{
       occ_proc();
	return occ_cmd;
}
//***********************************************************************************************************************
//��������OCC ��ʼ������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
static void occ_init(void)
{	
	//��ʼ��ͨ�Ŷ˿�
	OCC_PORTINIT(OCC_BAUDRATE);
	
}

//��������豸��
occ_dev_t sh9_occ_dev=
{
	occ_init,
	occ_proc,
	occ_send_data,
	occ_get_playing,
	
	0,//����ͨ�Ŵ���
	
};

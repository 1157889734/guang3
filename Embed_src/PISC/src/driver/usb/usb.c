//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& usb���� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:usb.c

//*�ļ�����:usb����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//�ж�������
uint16  wInterrupt_Mask=IMR_MSK;
//��������״̬λ�ĳ���ʱ��---������
volatile uint8 resume_esof_cnt=0;
//�Ƿ����ڷ���
static uint8 is_sending=0;

//fifo����
static uint8 usb_recv_fifo[USB_FIFO_SIZE];
static uint16 usb_recv_fifo_in=0;
static uint16 usb_recv_fifo_out=0;
static uint8 usb_send_fifo[USB_FIFO_SIZE];
static uint16 usb_send_fifo_in=0;
static uint16 usb_send_fifo_out=0;

	

//***********************************************************************************************************************
//��������:usb���²��� �жϼ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void XTI_IRQHandler(void)
{	
	Device_Info.Current_Configuration=0;
	is_sending=0;
	usb_send_fifo_in=usb_send_fifo_out=0;
		
	resume_esof_cnt=50;//���û���״̬λ�ĳ���ʱ��
	
	XTI_PRL &= ~(XTI_PRL|((uint16)XTI_PRH<<8));
	XTI_PRH &= ~((XTI_PRL|((uint16)XTI_PRH<<8)>>8)&0x00FF);
	
	//RCCU_PLL2=ON//����48MHzʱ��
	USB_CNTR&=(~CNTR_LPMODE);//�˳��͹���ģʽ
	USB_CNTR&=~(CNTR_FSUSP);//�Ƴ�����ģʽ
	USB_CNTR=(CNTR_CTRM|CNTR_WKUPM|CNTR_SUSPM|CNTR_ERRM|CNTR_SOFM|CNTR_ESOFM|CNTR_RESETM);//���ж�
	USB_CNTR|=CNTR_RESUME;//���������ͻ�������
}

//***********************************************************************************************************************
//��������:usb�ͼ��жϷ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void USBLP_IRQHandler(void)
{
	//���ڴ洢�ж�״̬�Ĵ�����ֵ
	volatile uint16 wIstr;
	
	//���жϼĴ���
	wIstr=USB_ISTR;

	//�豸��λ�ж�
	if(wIstr & ISTR_RESET & wInterrupt_Mask)
	{
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_RESET));
		
		USB_CNTR&=(~CNTR_RESUME);//������������
		//Set DEVICE as not configured
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//���÷��黺������USB_BUFADDR�е�ƫ��
		USB_BTABLE = (uint16)(BTABLE_ADDRESS & 0xFFF8);
		//Initialize Endpoint 0
		USB_EPxR(ENDP0)=(USB_EPxR(ENDP0)&EP_T_MASK)|EP_CONTROL;//���ƴ���
		USB_ADDR_RX(ENDP0)=((ENDP0_RXADDR>>1)<<1);//���ջ�������ַ
		SetEPRxCount(ENDP0, STD_MAXPACKETSIZE);//���ջ���������
		SetEPTxStatus(ENDP0, EP_TX_NAK);//�رշ���
		USB_ADDR_TX(ENDP0) =((ENDP0_TXADDR>>1)<<1);//���ͻ�������ַ
		USB_COUNT_TX(ENDP0)=STD_MAXPACKETSIZE;//Ҫ���͵ĳ���
		USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&EPKIND_MASK;//��״̬
		SetEPRxStatus(ENDP0, EP_RX_VALID);//��������
		//Initialize Endpoint 1
		USB_EPxR(ENDP1)=(USB_EPxR(ENDP1)&EP_T_MASK)|EP_INTERRUPT;//�жϴ���
		USB_ADDR_RX(ENDP1)=((ENDP1_RXADDR>>1)<<1);//���ջ�������ַ
		SetEPRxCount(ENDP1, EP1_MAXPACKETSIZE);//���ջ���������
		SetEPTxStatus(ENDP1, EP_TX_NAK);//�رշ���
		USB_ADDR_TX(ENDP1) =((ENDP1_TXADDR>>1)<<1);//���ͻ�������ַ
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//Ҫ���͵ĳ���
		USB_EPxR(ENDP1)=USB_EPxR(ENDP1)&EPKIND_MASK;//��״̬
		SetEPRxStatus(ENDP1, EP_RX_VALID);//��������
		//�����豸��ַΪ0 �Ա���ӦPC��ȱʡ����
		SetDeviceAddress(0);
	}
	//DMA OVER�ж�
	if(wIstr & ISTR_DOVR & wInterrupt_Mask)
	{
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_DOVR));
	}
	//�����ж�
	if(wIstr & ISTR_ERR & wInterrupt_Mask)
	{
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_ERR));
	}
	//�����ж�
	if(wIstr & ISTR_WKUP & wInterrupt_Mask)
	{
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_WKUP));

		//RCCU_PLL2=ON//����48MHzʱ��
		//USB_CNTR&=(~CNTR_LPMODE);//�˳��͹���ģʽ
		USB_CNTR=(CNTR_CTRM|CNTR_WKUPM|CNTR_SUSPM|CNTR_ERRM|CNTR_SOFM|CNTR_ESOFM||CNTR_RESETM);//���ж�
		//�ж��Ƿ��Ǹ����ٴν������ģʽ
		if(USB_FNR&FNR_RXDP)
		{
			//USB_CNTR|=CNTR_FSUSP;//�������ģʽ
			//USB_CNTR|=CNTR_LPMODE;//���͹���ģʽ
			//RCCU_PLL2=OFF//�ر�48MHzʱ��
		}
	}
	//�����ж�
	if(wIstr & ISTR_SUSP & wInterrupt_Mask)
	{
		//Set DEVICE as not configured
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//USB_CNTR|=CNTR_FSUSP;//�������ģʽ
		//USB_CNTR|=CNTR_LPMODE;//���͹���ģʽ
		//RCCU_PLL2=OFF//�ر�48MHzʱ��
		
		//�����ȹ��������жϱ�־
		USB_ISTR=((uint16)(~ISTR_SUSP));
	}
	//�忪ʼ�ж�
	if(wIstr & ISTR_SOF & wInterrupt_Mask)
	{
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_SOF));
	}
	//�ڴ��忪ʼ�ж�
	if(wIstr & ISTR_ESOF & wInterrupt_Mask)
	{
		//���жϱ�־
		USB_ISTR=((uint16)(~ISTR_ESOF));
		
		//�жϻ���״̬λ����ʱ�䵽 ��������״̬λ
		if(resume_esof_cnt)
		{
			if(--resume_esof_cnt==0)
			{
				USB_CNTR&=(~CNTR_RESUME);//������������
			}
		}
		else
		{
			USB_CNTR&=(~CNTR_RESUME);//������������
		}
	}
	//�˵���ȷ�����ж�
	if(wIstr & ISTR_CTR & wInterrupt_Mask)
	{
		uint16 wEPVal;//�˵�״ֵ̬
		uint8 EPindex;//�˵��
		
	 	//һֱ��ȡ�жϼĴ��� ֱ���������ݴ��䴦�����
		while((wIstr=USB_ISTR)&ISTR_CTR)
		{
			//���жϱ�־
			USB_ISTR=((uint16)(~ISTR_CTR)); /* clear CTR flag */
			//��ȡ�˵��
			EPindex=(uint8)(wIstr & ISTR_EP_ID);
			//����˵�0
			if(EPindex==0)
			{
				//����˵�0״̬ ����ʱ�ر������շ�
				SaveRState=((uint16)USB_EPxR(ENDP0)&EPRX_STAT);
				SaveTState=((uint16)USB_EPxR(ENDP0)&EPTX_STAT);
				SetEPRxStatus(ENDP0, EP_RX_NAK);
				SetEPTxStatus(ENDP0, EP_TX_NAK);

				//�жϴ��䷽��		�豸->����
				if((wIstr&ISTR_DIR)==0)
				{
					//��ȡ�˵�״̬
					wEPVal=USB_EPxR(ENDP0);
					//�˵㷢����ϴ���
					//if((wEPVal & EP_CTR_TX) != 0)
					{
						//����˵�ķ�����ɱ�־
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0xFF7F&EPREG_MASK;
						In0_Process();
					}
					//�����͹����е���������
					if(wEPVal&(EP_CTR_RX|EP_SETUP))
					{
						//����˵�Ľ�����ɱ�־
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Setup0_Process();
					}

				}
				//�жϴ��䷽��		����->�豸
				else
				{
					//��ȡ�˵�״̬
					wEPVal=USB_EPxR(ENDP0);
					//�˵㷢����ϴ���
					if(wEPVal&EP_CTR_TX)
					{
						//����˵�ķ�����ɱ�־
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0xFF7F&EPREG_MASK;
						In0_Process();
					}
					
					//������������
					if(wEPVal&(EP_CTR_RX|EP_SETUP))
					{
						//����˵�Ľ�����ɱ�־
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Setup0_Process();
					}
					//�˵������ϴ���
					else if(wEPVal&EP_CTR_RX)
					{
						//����˵�Ľ�����ɱ�־
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Out0_Process();
					}
				}
				
				//�ָ��˵�0��״̬
				SetEPRxStatus(ENDP0, SaveRState);
				SetEPTxStatus(ENDP0, SaveTState);
			}
			//���������˵�
			else
			{
				//��ȡ�˵�״̬
				wEPVal=USB_EPxR(EPindex);
				//�˵㷢����ϴ��� ������Ӳ��FIFOװ������
				if(wEPVal&EP_CTR_TX)
				{
					//����˵�ķ�����ɱ�־
					USB_EPxR(EPindex)=USB_EPxR(EPindex)&0xFF7F&EPREG_MASK;
					
					//����˵�1�ķ���
					if(EPindex==1)
					{
						//û������Ҫ���� �ض˵�ķ���(�Զ��ر���)
						if(usb_send_fifo_out==usb_send_fifo_in)
						{
							is_sending=0;
						}
						//������Ҫ���� ��Ӳ��FIFOװ��
						else
						{
							uint32 *packet_length;//�����������ݵĳ���
							uint32 *dst;
							uint16 Nbytes=0;
							
							is_sending=1;
							dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
							packet_length=dst;//ָ�򳤶ȵ�λ��
							(*packet_length)=0;//������0
							dst++;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ���
							Nbytes+=2;
							while(Nbytes<EP1_MAXPACKETSIZE)
							{
								//��һ���ֽ���Ӳ��fifo
								if(usb_send_fifo_out!=usb_send_fifo_in)
								{
									*dst=usb_send_fifo[usb_send_fifo_out];
									if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
									(*packet_length)++;//�����������ݵĳ���
								}
								else
								{
									*dst=0;
								}
								//�ڶ����ֽ���Ӳ��fifo
								if(usb_send_fifo_out!=usb_send_fifo_in)
								{
									*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
									if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
									(*packet_length)++;//�����������ݵĳ���
								}
								else
								{
									*dst|=0;
								}
								dst++;
								Nbytes+=2;
							}
							USB_COUNT_TX(EPindex)=EP1_MAXPACKETSIZE;//����Ҫ���͵ĳ���
							SetEPTxStatus(EPindex,EP_TX_VALID);//��������
						}
					}
					//����˵�2�ķ���
					else if(EPindex==2)
					{
						
					}
				}
				//�˵������ϴ���
				if(wEPVal&EP_CTR_RX)
				{
					//����˵�1�Ľ���
					if(EPindex==1)
					{
						uint16 packet_length;//�����������ݵĳ���
						uint8 *src;
						
						src=USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_RX(EPindex)*2);
						packet_length=*((uint16 *)src);//USB_COUNT_RX(EPindex)&0x03FF;//��ȡ�������ݵĳ���
						if(packet_length>EP1_MAXPACKETSIZE-2)
							packet_length=EP1_MAXPACKETSIZE-2;
						src+=4;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ��ȡ
						while(packet_length)
						{
							//��һ���ֽ������fifo
							usb_recv_fifo[usb_recv_fifo_in++]=*src++;
							if(usb_recv_fifo_in>=USB_FIFO_SIZE)
								usb_recv_fifo_in=0;
							packet_length--;
							if(packet_length)
							{
								//�ڶ����ֽ������fifo
								usb_recv_fifo[usb_recv_fifo_in++]=*src++;
								if(usb_recv_fifo_in>=USB_FIFO_SIZE)
									usb_recv_fifo_in=0;
								packet_length--;
							}
							//��Ϊ����Ӳ��fifo��32λ������ ����32λ��ǰ�����ֽ���Ч ���������������ֽ�
							src+=2;
						}
					}
					//����˵�2�Ľ���
					else if(EPindex==2)
					{
						
					}
					
					//����˵�Ľ�����ɱ�־
					USB_EPxR(EPindex)=USB_EPxR(EPindex)&0x7FFF&EPREG_MASK;			
					SetEPRxStatus(EPindex,EP_RX_VALID);//�����´ν���
				}
			}
		}
	}
}

//***********************************************************************************************************************
//��������:usb��ʼ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void usb_init(uint32 bps)
{	
	bps=bps;
	
	//ע��usb�ж�����
	USB_IRQ_SLOT=(USB_IRQ_SLOT&0xFFFF0000)|USB_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<USB_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�

	//�豸�ϵ�
	USB_CNTR=CNTR_FRES|CNTR_PDWN;//�ص�Դ�����ָ�λ״̬
	USB_CNTR=CNTR_FRES;//����Դ�����ָ�λ״̬
	USB_ISTR=0;//����жϱ�־
	//Set DEVICE as not configured
	Device_Info.Current_Configuration=0;
	is_sending=0;
	usb_send_fifo_in=usb_send_fifo_out=0;
	//USB_CNTR|=CNTR_FSUSP;//�������ģʽ
	//USB_CNTR|=CNTR_LPMODE;//���͹���ģʽ
	
	USB_CNTR=CNTR_RESETM|CNTR_SUSPM|CNTR_WKUPM;//��һ�����ж� �ȴ���������

	//ע���ⲿ�ж� usb���²��� �жϼ��
	XTI_TRL &= ~(1<<1);//XTI Line1 FallingEdge
	XTI_TRH &= ~((1<<1)>>8)&0x00FF;//XTI Line1 FallingEdge
	XTI_MRL |= (1<<1);//XTI Line1 enable
	XTI_MRH |= ((1<<1)>>8)&0x00FF;//XTI Line1 enable
	USB_DET_IRQ_SLOT=(USB_DET_IRQ_SLOT&0xFFFF0000)|USB_DET_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<USB_DET_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�
}

//***********************************************************************************************************************
//��������:��ѯusb�Ľ���fifo ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8 usb_read_char(uint8 *ch)
{
	if(usb_recv_fifo_out == usb_recv_fifo_in)
		return 0;
	
	*ch = usb_recv_fifo[usb_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	usb_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(usb_recv_fifo_out == USB_FIFO_SIZE)
		usb_recv_fifo_out = 0;//�������fifoĩβ ���ش�ͷ��ʼ
	
	return 1;
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢��һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void usb_write_char(uint8 ch)
{	
	uint16 fifo_in_test;

	//����豸��û�����ú� �˳�
	if(Device_Info.Current_Configuration==0)
	{
		return;
	}

	//���Ի������Ƿ���
	fifo_in_test=usb_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=USB_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==usb_send_fifo_out)
		return;
	//��FIFO�������
	usb_send_fifo[usb_send_fifo_in++]=ch;
	if(usb_send_fifo_in>=USB_FIFO_SIZE)
		usb_send_fifo_in=0;
		
	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
		uint32 *packet_length;//�����������ݵĳ���
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//ָ�򳤶ȵ�λ��
		(*packet_length)=0;//������0
		dst++;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ���
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//��һ���ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst=0;
			}
			//�ڶ����ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//����Ҫ���͵ĳ���
		SetEPTxStatus(ENDP1,EP_TX_VALID);//��������
	}	
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢�Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 usb_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=usb_send_fifo_in;
	
	//����豸��û�����ú� �˳�
	if(Device_Info.Current_Configuration==0)
	{
		return 0;
	}
	
	while(length--)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			usb_send_fifo_in=send_fifo_in_backup;
			return 0;
		}
		//��FIFO�������
		usb_send_fifo[usb_send_fifo_in++]=*buf++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)
			usb_send_fifo_in=0;
	}
	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
		uint32 *packet_length;//�����������ݵĳ���
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//ָ�򳤶ȵ�λ��
		(*packet_length)=0;//������0
		dst++;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ���
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//��һ���ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst=0;
			}
			//�ڶ����ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//����Ҫ���͵ĳ���
		SetEPTxStatus(ENDP1,EP_TX_VALID);//��������
	}
	
	return 1;
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢���ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void usb_write_str(char *str)
{	
	//����豸��û�����ú� �˳�
	if(Device_Info.Current_Configuration==0)
	{
		return;
	}
	
	while(*str)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)
			return;
		//��FIFO�������
		usb_send_fifo[usb_send_fifo_in++]=*str++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)
			usb_send_fifo_in=0;
	}
	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
		uint32 *packet_length;//�����������ݵĳ���
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//ָ�򳤶ȵ�λ��
		(*packet_length)=0;//������0
		dst++;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ���
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//��һ���ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst=0;
			}
			//�ڶ����ֽ���Ӳ��fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//�����������ݵĳ���
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//����Ҫ���͵ĳ���
		SetEPTxStatus(ENDP1,EP_TX_VALID);//��������
	}
}


//***********************************************************************************************************************
//��������:usb��ȡ�Ƿ�����״̬
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 usb_get_connect_state(void)
{
	if( Device_Info.Current_Configuration == 0 )
		return 0;
	else
		return 1;
}





//���ö˵���ջ���������
void SetEPRxCount(uint8 bEpNum, uint16 wCount)
{
	uint16 wNBlocks;
	
	if(wCount>62)
	{
		wNBlocks=wCount>>5;
		if((wCount&0x1f)==0)
			wNBlocks--;
		USB_COUNT_RX(bEpNum)=(uint32)((wNBlocks << 10) | 0x8000);
	}
	else
	{
		wNBlocks=wCount>>1;
		if((wCount&0x1)!=0)
			wNBlocks++;
		USB_COUNT_RX(bEpNum)=(uint32)(wNBlocks << 10);
	}
}

//���ý��ն˵�״̬
void SetEPRxStatus(uint8 bEpNum, uint16 wState)
{
	register uint16 _wRegVal;
	
	_wRegVal=USB_EPxR(bEpNum)&EPRX_DTOGMASK;
	//toggle first bit
	if((EPRX_DTOG1 & wState)!= 0)
		_wRegVal ^= EPRX_DTOG1;
	//toggle second bit
	if((EPRX_DTOG2 & wState)!= 0)
		_wRegVal ^= EPRX_DTOG2;
	USB_EPxR(bEpNum)=_wRegVal;
}

//���÷��Ͷ˵�״̬
void SetEPTxStatus(uint8 bEpNum, uint16 wState)
{
	register uint16 _wRegVal;
	
	_wRegVal = USB_EPxR(bEpNum)&EPTX_DTOGMASK;
	//toggle first bit
	if((EPTX_DTOG1 & wState)!= 0)
		_wRegVal ^= EPTX_DTOG1;
	//toggle second bit
	if((EPTX_DTOG2 & wState)!= 0)
		_wRegVal ^= EPTX_DTOG2;
	USB_EPxR(bEpNum)=_wRegVal;
}

//�����豸��ַ
void SetDeviceAddress(uint8 Val)
{
	uint8 i;

	//����ÿ���˵�ĵ�ַ
	for(i=0;i<TOTAL_ENDP;i++)
		USB_EPxR(i)=USB_EPxR(i)&EPREG_MASK|i;
	//�����豸��ַ
	USB_DADDR=(Val|DADDR_EF);
}



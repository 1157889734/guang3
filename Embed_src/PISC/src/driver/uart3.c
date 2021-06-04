//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&����3��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:uart3.c

//*�ļ�����:����3��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo����
static uint8 uart3_recv_fifo[UART3_FIFO_SIZE];
static uint16 uart3_recv_fifo_in=0;
static uint16 uart3_recv_fifo_out=0;
static uint8 uart3_send_fifo[UART3_FIFO_SIZE];
static uint16 uart3_send_fifo_in=0;
static uint16 uart3_send_fifo_out=0;

static uint16 uart3_recv_fifo_out_backup=0;

//Ӳ����������
static uint8 uart3_hardware_test_flag=0;


//***********************************************************************************************************************
//��������:����3�жϷ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void UART3_IRQHandler(void)
{
	uint16 status=UART3_SR;
	
	//����FIFO����(8���ֽ�)---��8���ֽڴ����ж���߽��յ�Ч��
	if(status& (0x0100|0x40))
	{
		//30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
		UART3_TOR=30;
		//��Ӳ��FIFO���� һ�߶�һ�������FIFO
		while(UART3_SR&0x01)
		{
			uart3_recv_fifo[uart3_recv_fifo_in++]=UART3_RxBUFR;
			if(uart3_recv_fifo_in>=UART3_FIFO_SIZE)
				uart3_recv_fifo_in=0;
		}
	}
	//���ݷ������ ������Ӳ��FIFOװ������	
	if(status&(0x02))
	{
		//û������Ҫ���� �ط����ж�
		if(uart3_send_fifo_out==uart3_send_fifo_in)
		{
			UART3_IER&=~(0x02);
			//MAX485��״̬
			#ifdef UART3_MAX485_CTRL_REG
			UART3_MAX485_CTRL_REG&=~(0x0001<<UART3_MAX485_CTRL_BIT);
			#endif
		}
		//������Ҫ���� ��Ӳ��FIFOװ��
		else
		{
			while(UART3_SR&0x02)
			{
				UART3_TxBUFR=uart3_send_fifo[uart3_send_fifo_out++];
				if(uart3_send_fifo_out>=UART3_FIFO_SIZE)
					uart3_send_fifo_out=0;
				if(uart3_send_fifo_out==uart3_send_fifo_in)
					break;//�ж��Ƿ�������
			}
		}
	}
}

//***********************************************************************************************************************
//��������:���ڳ�ʼ��
//����˵��:������
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart3_init(uint32 bps)
{
	//ע�ᴮ���ж�
	//UART3_IRQ_SLOT=(uint32)UART3_HANDLER;//�����жϴ�������ַ---��IRQ.S���Ѿ����
	UART3_IRQ_SLOT=(UART3_IRQ_SLOT&0xFFFF0000)|UART3_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<UART3_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�
	
	//���ùܽ�����ΪUART3
	GPIO0_PC0|=(0x0001<<0);
	GPIO0_PC1|=(0x0001<<0);
	GPIO0_PC2|=(0x0001<<0);
	GPIO0_PC0&=~(0x0001<<1);
	GPIO0_PC1|=(0x0001<<1);
	GPIO0_PC2&=~(0x0001<<1);
	//����MAX485���ƹܽ�����Ϊ���+����
	#ifdef UART3_MAX485_CTRL_REG
	GPIO2_PC0|=(0x0001<<UART3_MAX485_CTRL_BIT);
	GPIO2_PC1&=~(0x0001<<UART3_MAX485_CTRL_BIT);
	GPIO2_PC2|=(0x0001<<UART3_MAX485_CTRL_BIT);
	#endif
	
	//������мĴ���
	UART3_CR=0x0000;
	UART3_IER=0x0000;
	(void)UART3_RxBUFR;
	UART3_RxRSTR=0xFFFF;
	UART3_TxRSTR=0xFFFF;
	UART3_TOR=0x0000;
	UART3_GTR=0x0000;
	//���üĴ���
	UART3_CR|=0x0080;//������
	UART3_CR|=0x0400;//ʹ��Ӳ��FIFO
	UART3_RxRSTR=0xFFFF;//��Ӳ������FIFO
	UART3_TxRSTR=0xFFFF;//��Ӳ������FIFO
	UART3_CR&=~0x0040;//�رձ��ػػ�
	UART3_TOR=30;//30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
	UART3_BR=(uint16)(Fpcl/(16*bps));//������
	UART3_CR=(UART3_CR&0xFFDF)|0;//У�� N
	UART3_CR=(UART3_CR&0xFFF8)|0x01;//����λ 8
	UART3_CR=(UART3_CR&0xFFE7)|0x08;//ֹͣλ 1
	UART3_CR|=0x0100;//��������
	//�������ж�
	UART3_IER=0x0100|0x40;//��8���ֽڴ����ж���߽��յ�Ч�� 30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж�
	UART3_IER|=(0x02);//Ӳ��FIFO����һ���ʱ�������ϵ�ʱ��---�����жϼ�����Ӳ��FIFOװ������
	
	//MAX485��״̬
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG&=~(0x0001<<UART3_MAX485_CTRL_BIT);
	#endif
	uart3_recv_fifo_out=uart3_recv_fifo_in=0;
}

//***********************************************************************************************************************
//��������:��ѯ���� ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8 uart3_read_char(uint8 *ch)
{
	if(uart3_recv_fifo_out==uart3_recv_fifo_in)
		return 0;
	*ch=uart3_recv_fifo[uart3_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	uart3_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(uart3_recv_fifo_out==UART3_FIFO_SIZE)
		uart3_recv_fifo_out=0;//�������fifoĩβ ���ش�ͷ��ʼ
	
	//�ж�Ӳ����������
	if(*ch==HARDWARE_TEST_CMD)
	{
		uart3_hardware_test_flag++;
		if(uart3_hardware_test_flag>=200)
			uart3_hardware_test_flag=200;
	}
	else
	{
		uart3_hardware_test_flag=0;
	}	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ���һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart3_write_char(uint8 ch)
{	
	//���Ի������Ƿ���
	uint16 fifo_in_test=uart3_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=UART3_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==uart3_send_fifo_out)
		return;
	//��FIFO�������
	uart3_send_fifo[uart3_send_fifo_in++]=ch;
	if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
		uart3_send_fifo_in=0;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART3_SR&0x02) �ͻ�����ж��Զ�����
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485����״̬
	#endif
	UART3_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:���ڷ��Ͷ�������
//����˵��:
//ע������:
//����˵��:������������0
//***********************************************************************************************************************
uint8 uart3_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=uart3_send_fifo_in;
	
	while(length--)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart3_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART3_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart3_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			uart3_send_fifo_in=send_fifo_in_backup;
			UART3_IER|=(0x02);
			return 0;
		}
		//��FIFO�������
		uart3_send_fifo[uart3_send_fifo_in++]=*buf++;
		if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
			uart3_send_fifo_in=0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART3_SR&0x02) �ͻ�����ж��Զ�����
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485����״̬	
	#endif
	UART3_IER|=(0x02);
	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ����ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart3_write_str(char *str)
{
	while(*str)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart3_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART3_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart3_send_fifo_out)
			break;
		//��FIFO�������
		uart3_send_fifo[uart3_send_fifo_in++]=*str++;
		if(uart3_send_fifo_in>=UART3_FIFO_SIZE)
			uart3_send_fifo_in=0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART3_SR&0x02) �ͻ�����ж��Զ�����
	#ifdef UART3_MAX485_CTRL_REG
	UART3_MAX485_CTRL_REG|=(0x0001<<UART3_MAX485_CTRL_BIT);//MAX485����״̬	
	#endif
	UART3_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:����fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart3_fifo_out_backup(void)
{
	uart3_recv_fifo_out_backup=uart3_recv_fifo_out;
}

//***********************************************************************************************************************
//��������:�ָ�fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart3_fifo_out_resume(void)
{
	uart3_recv_fifo_out=uart3_recv_fifo_out_backup;
}

//***********************************************************************************************************************
//��������:��ȡӲ����������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart3_get_hardware_test(void)
{
	if(uart3_hardware_test_flag==60)
	{
		uart3_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}



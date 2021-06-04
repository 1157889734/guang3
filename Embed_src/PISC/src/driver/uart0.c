//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&����0��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:uart0.c

//*�ļ�����:����0��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo����
static uint8 uart0_recv_fifo[UART0_FIFO_SIZE];
static uint16 uart0_recv_fifo_in=0;
static uint16 uart0_recv_fifo_out=0;
static uint8 uart0_send_fifo[UART0_FIFO_SIZE];
static uint16 uart0_send_fifo_in=0;
static uint16 uart0_send_fifo_out=0;

static uint16 uart0_recv_fifo_out_backup=0;

//Ӳ����������
static uint8 uart0_hardware_test_flag=0;



//***********************************************************************************************************************
//��������:����0�жϷ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void UART0_IRQHandler(void)
{
	uint16 status=UART0_SR;
	
	//����FIFO����(8���ֽ�)---��8���ֽڴ����ж���߽��յ�Ч��
	if(status& (0x0100|0x40))
	{
		//30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
		UART0_TOR=30;
		//��Ӳ��FIFO���� һ�߶�һ�������FIFO
		while(UART0_SR&0x01)
		{
			uart0_recv_fifo[uart0_recv_fifo_in++]=UART0_RxBUFR;
			if(uart0_recv_fifo_in>=UART0_FIFO_SIZE)
				uart0_recv_fifo_in=0;
		}
	}
	//���ݷ������ ������Ӳ��FIFOװ������	
	if(status&(0x02))
	{
		//û������Ҫ���� �ط����ж�
		if(uart0_send_fifo_out==uart0_send_fifo_in)
		{
			UART0_IER&=~(0x02);
		}
		//������Ҫ���� ��Ӳ��FIFOװ��
		else
		{
			while(UART0_SR&0x02)
			{
				UART0_TxBUFR=uart0_send_fifo[uart0_send_fifo_out++];
				if(uart0_send_fifo_out>=UART0_FIFO_SIZE)
					uart0_send_fifo_out=0;
				if(uart0_send_fifo_out==uart0_send_fifo_in)
					break;//�ж��Ƿ�������
			}
		}
	}
}

//***********************************************************************************************************************
//��������:����0��ʼ��
//����˵��:������
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart0_init(uint32 bps)
{
	//ע�ᴮ��0�ж�
	//UART0_IRQ_SLOT=(uint32)UART0_HANDLER;//�����жϴ�������ַ---��IRQ.S���Ѿ����
	UART0_IRQ_SLOT=(UART0_IRQ_SLOT&0xFFFF0000)|UART0_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<UART0_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�
	
	//���ùܽ�����ΪUART0
	GPIO0_PC0|=(0x0001<<9);
	GPIO0_PC1|=(0x0001<<9);
	GPIO0_PC2|=(0x0001<<9);
	GPIO0_PC0&=~(0x0001<<8);
	GPIO0_PC1|=(0x0001<<8);
	GPIO0_PC2&=~(0x0001<<8);
	//������мĴ���
	UART0_CR=0x0000;
	UART0_IER=0x0000;
	(void)UART0_RxBUFR;
	UART0_RxRSTR=0xFFFF;
	UART0_TxRSTR=0xFFFF;
	UART0_TOR=0x0000;
	UART0_GTR=0x0000;
	//���üĴ���
	UART0_CR|=0x0080;//������0
	UART0_CR|=0x0400;//ʹ��Ӳ��FIFO
	UART0_RxRSTR=0xFFFF;//��Ӳ������FIFO
	UART0_TxRSTR=0xFFFF;//��Ӳ������FIFO
	UART0_CR&=~0x0040;//�رձ��ػػ�
	UART0_TOR=30;//30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
	UART0_BR=(uint16)(Fpcl/(16*bps));//������
	UART0_CR=(UART0_CR&0xFFDF)|0;//У�� N
	UART0_CR=(UART0_CR&0xFFF8)|0x01;//����λ 8
	UART0_CR=(UART0_CR&0xFFE7)|0x08;//ֹͣλ 1
	UART0_CR|=0x0100;//��������
	//�������ж�
	UART0_IER=0x0100|0x40;//��8���ֽڴ����ж���߽��յ�Ч�� 30��������û������ �͸���FIFO��ʣ�µ����ݴ����ж�
	UART0_IER|=(0x02);//Ӳ��FIFO����һ���ʱ�������ϵ�ʱ��---�����жϼ�����Ӳ��FIFOװ������
}

//***********************************************************************************************************************
//��������:��ѯ����0 ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8 uart0_read_char(uint8 *ch)
{
	if(uart0_recv_fifo_out==uart0_recv_fifo_in)
		return 0;
	*ch=uart0_recv_fifo[uart0_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	uart0_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(uart0_recv_fifo_out==UART0_FIFO_SIZE)
		uart0_recv_fifo_out=0;//�������fifoĩβ ���ش�ͷ��ʼ
	
	//�ж�Ӳ����������
	if(*ch==HARDWARE_TEST_CMD)
	{
		uart0_hardware_test_flag++;
		if(uart0_hardware_test_flag>=200)
			uart0_hardware_test_flag=200;
	}
	else
	{
		uart0_hardware_test_flag=0;
	}
	return 1;
}

//***********************************************************************************************************************
//��������:����0����һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart0_write_char(uint8 ch)
{	
	//���Ի������Ƿ���
	uint16 fifo_in_test=uart0_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=UART0_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==uart0_send_fifo_out)
		return;
	//��FIFO�������
	uart0_send_fifo[uart0_send_fifo_in++]=ch;
	if(uart0_send_fifo_in>=UART0_FIFO_SIZE)
		uart0_send_fifo_in=0;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART0_SR&0x02) �ͻ�����ж��Զ�����
	UART0_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:����0���Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart0_write_buf(uint8 *buf, uint16 length)
{
	uint16 send_fifo_in_backup=uart0_send_fifo_in;
	
	while(length--)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart0_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART0_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart0_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			uart0_send_fifo_in=send_fifo_in_backup;
			UART0_IER|=(0x02);
			return 0;
		}
		//��FIFO�������
		uart0_send_fifo[uart0_send_fifo_in++]=*buf++;
		if(uart0_send_fifo_in>=UART0_FIFO_SIZE)
			uart0_send_fifo_in=0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART0_SR&0x02) �ͻ�����ж��Զ�����
	UART0_IER|=(0x02);
	
	return 1;
}

//***********************************************************************************************************************
//��������:����0�����ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart0_write_str(char *str)
{
	while(*str)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart0_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART0_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart0_send_fifo_out)
			break;
		//��FIFO�������
		uart0_send_fifo[uart0_send_fifo_in++]=*str++;
		if(uart0_send_fifo_in>=UART0_FIFO_SIZE)
			uart0_send_fifo_in=0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART0_SR&0x02) �ͻ�����ж��Զ�����
	UART0_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:����fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart0_fifo_out_backup(void)
{
	uart0_recv_fifo_out_backup=uart0_recv_fifo_out;
}

//***********************************************************************************************************************
//��������:�ָ�fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart0_fifo_out_resume(void)
{
	uart0_recv_fifo_out=uart0_recv_fifo_out_backup;
}

//***********************************************************************************************************************
//��������:��ȡӲ����������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart0_get_hardware_test(void)
{
	if(uart0_hardware_test_flag==60)
	{
		uart0_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}



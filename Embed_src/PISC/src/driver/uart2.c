//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&����2��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:uart2.c

//*�ļ�����:����2��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo����
static uint8 uart2_recv_fifo[UART2_FIFO_SIZE];
static uint16 uart2_recv_fifo_in=0;
static uint16 uart2_recv_fifo_out=0;
static uint8 uart2_send_fifo[UART2_FIFO_SIZE];
static uint16 uart2_send_fifo_in=0;
static uint16 uart2_send_fifo_out=0;

static uint16 uart2_recv_fifo_out_backup=0;
//Ӳ����������
static uint8 uart2_hardware_test_flag=0;



//***********************************************************************************************************************
//��������:����2�жϷ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void UART2_IRQHandler(void)
{
	uint16 status=UART2_SR;
	
	//����FIFO����(8���ֽ�)---��8���ֽڴ����ж���߽��յ�Ч��
	if(status&(0x0100|0x40))
	{
		//15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
		UART2_TOR=15;
		//��Ӳ��FIFO���� һ�߶�һ�������FIFO
		while(UART2_SR&0x01)
		{
			uart2_recv_fifo[uart2_recv_fifo_in++]=UART2_RxBUFR;
			if(uart2_recv_fifo_in>=UART2_FIFO_SIZE)
				uart2_recv_fifo_in=0;
		}
	}
	//���ݷ������ ������Ӳ��FIFOװ������	
	if(status&(0x02))
	{
		//û������Ҫ���� �ط����ж�
		if(uart2_send_fifo_out==uart2_send_fifo_in)
		{
			UART2_IER&=~(0x02);
			//MAX485��״̬
		#ifdef UART2_MAX485_CTRL_REG	
			UART2_MAX485_CTRL_REG&=~(0x0001<<UART2_MAX485_CTRL_BIT);
		#endif
		}
		//������Ҫ���� ��Ӳ��FIFOװ��
		else
		{
			while(UART2_SR&0x02)
			{
				UART2_TxBUFR=uart2_send_fifo[uart2_send_fifo_out++];
				if(uart2_send_fifo_out>=UART2_FIFO_SIZE)
					uart2_send_fifo_out=0;
				if(uart2_send_fifo_out==uart2_send_fifo_in)
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
void uart2_init(uint32 bps)
{
	//ע�ᴮ���ж�
	//UART2_IRQ_SLOT=(uint32)UART2_HANDLER;//�����жϴ�������ַ---��IRQ.S���Ѿ����
	UART2_IRQ_SLOT=(UART2_IRQ_SLOT&0xFFFF0000)|UART2_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<UART2_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�
	
	//���ùܽ�����ΪUART2
	GPIO0_PC0|=(0x0001<<14);
	GPIO0_PC1|=(0x0001<<14);
	GPIO0_PC2|=(0x0001<<14);
	GPIO0_PC0&=~(0x0001<<13);
	GPIO0_PC1|=(0x0001<<13);
	GPIO0_PC2&=~(0x0001<<13);
#ifdef UART2_MAX485_CTRL_REG	
	//����MAX485���ƹܽ�����Ϊ���+����
	GPIO2_PC0|=(0x0001<<UART2_MAX485_CTRL_BIT);
	GPIO2_PC1&=~(0x0001<<UART2_MAX485_CTRL_BIT);
	GPIO2_PC2|=(0x0001<<UART2_MAX485_CTRL_BIT);
#endif	
	//������мĴ���
	UART2_CR=0x0000;
	UART2_IER=0x0000;
	(void)UART2_RxBUFR;
	UART2_RxRSTR=0xFFFF;
	UART2_TxRSTR=0xFFFF;
	UART2_TOR=0x0000;
	UART2_GTR=0x0000;
	//���üĴ���
	UART2_CR|=0x0080;//������
	UART2_CR|=0x0400;//ʹ��Ӳ��FIFO
	UART2_RxRSTR=0xFFFF;//��Ӳ������FIFO
	UART2_TxRSTR=0xFFFF;//��Ӳ������FIFO
	UART2_CR&=~0x0040;//�رձ��ػػ�
	UART2_TOR=15;//15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
	UART2_BR=(uint16)(Fpcl/(16*bps));//������
	UART2_CR=(UART2_CR&0xFFDF)|0;//У�� N
	UART2_CR=(UART2_CR&0xFFF8)|0x01;//����λ 8
	UART2_CR=(UART2_CR&0xFFE7)|0x08;//ֹͣλ 1
	UART2_CR|=0x0100;//��������
	//�������ж�
	UART2_IER=0x0100|0x40;//��8���ֽڴ����ж���߽��յ�Ч�� 15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж�
	UART2_IER|=(0x02);//Ӳ��FIFO����һ���ʱ�������ϵ�ʱ��---�����жϼ�����Ӳ��FIFOװ������

	//MAX485��״̬
#ifdef UART2_MAX485_CTRL_REG	
	UART2_MAX485_CTRL_REG&=~(0x0001<<UART2_MAX485_CTRL_BIT);
#endif
}

//***********************************************************************************************************************
//��������:��ѯ���� ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8 uart2_read_char(uint8 *ch)
{
	if(uart2_recv_fifo_out == uart2_recv_fifo_in)
		return 0;
	
	*ch = uart2_recv_fifo[uart2_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	uart2_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(uart2_recv_fifo_out == UART2_FIFO_SIZE)
		uart2_recv_fifo_out = 0;//�������fifoĩβ ���ش�ͷ��ʼ

	//�ж�Ӳ����������
	if( *ch == HARDWARE_TEST_CMD )
	{
		uart2_hardware_test_flag++;
		if(uart2_hardware_test_flag >= 200)
			uart2_hardware_test_flag = 200;
	}
	else
	{
		uart2_hardware_test_flag = 0;
	}	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ���һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart2_write_char(uint8 ch)
{	
	//���Ի������Ƿ���
	uint16 fifo_in_test = uart2_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test >= UART2_FIFO_SIZE)
		fifo_in_test = 0;
	if(fifo_in_test == uart2_send_fifo_out)
		return;
	//��FIFO�������
	uart2_send_fifo[uart2_send_fifo_in++] = ch;
	if(uart2_send_fifo_in >= UART2_FIFO_SIZE)
		uart2_send_fifo_in = 0;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART2_SR&0x02) �ͻ�����ж��Զ�����
#ifdef UART2_MAX485_CTRL_REG
	UART2_MAX485_CTRL_REG |= (0x0001<<UART2_MAX485_CTRL_BIT);//MAX485����״̬	
#endif
	UART2_IER |= (0x02);
}

//***********************************************************************************************************************
//��������:���ڷ��Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart2_write_buf(uint8 *buf, uint16 length)
{	
	uint16 wait;
	uint16 send_fifo_in_backup = uart2_send_fifo_in;
	
	while( length-- )
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test = uart2_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test >= UART2_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test == uart2_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			uart2_send_fifo_in = send_fifo_in_backup;
			UART2_IER |= (0x02);
			return 0;
		}
		//��FIFO�������
		uart2_send_fifo[uart2_send_fifo_in++] = *buf++;
		if(uart2_send_fifo_in >= UART2_FIFO_SIZE)
			uart2_send_fifo_in = 0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART2_SR&0x02) �ͻ�����ж��Զ�����
#ifdef UART2_MAX485_CTRL_REG
	UART2_MAX485_CTRL_REG |= (0x0001<<UART2_MAX485_CTRL_BIT);//MAX485����״̬	
#endif
	for(wait=0; wait<1000; wait++)
		;
	UART2_IER |= (0x02);
	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ����ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart2_write_str(char *str)
{
	while(*str)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart2_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART2_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart2_send_fifo_out)
			break;
		//��FIFO�������
		uart2_send_fifo[uart2_send_fifo_in++]=*str++;
		if(uart2_send_fifo_in>=UART2_FIFO_SIZE)
			uart2_send_fifo_in=0;
	}
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART2_SR&0x02) �ͻ�����ж��Զ�����
#ifdef UART2_MAX485_CTRL_REG
	UART2_MAX485_CTRL_REG|=(0x0001<<UART2_MAX485_CTRL_BIT);//MAX485����״̬	
#endif
	UART2_IER|=(0x02);
}
//***********************************************************************************************************************
//��������:����fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart2_fifo_out_backup(void)
{
	uart2_recv_fifo_out_backup=uart2_recv_fifo_out;
}

//***********************************************************************************************************************
//��������:�ָ�fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart2_fifo_out_resume(void)
{
	uart2_recv_fifo_out=uart2_recv_fifo_out_backup;
}
//***********************************************************************************************************************
//��������:��ȡӲ����������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart2_get_hardware_test(void)
{
	if(uart2_hardware_test_flag==60)
	{
		uart2_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}



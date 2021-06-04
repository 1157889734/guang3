//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&����1��������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:uart1.c

//*�ļ�����:����1��������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//fifo����
static uint8 uart1_recv_fifo[UART1_FIFO_SIZE];
static uint16 uart1_recv_fifo_in=0;
static uint16 uart1_recv_fifo_out=0;
static uint8 uart1_send_fifo[UART1_FIFO_SIZE];
static uint16 uart1_send_fifo_in=0;
static uint16 uart1_send_fifo_out=0;
static uint16 uart1_recv_fifo_out_backup=0;
//Ӳ����������
static uint8 uart1_hardware_test_flag=0;
//extern uint32 bus_timer;
extern uint32 bus_timer_key ;
extern uint32 bus_timer_recon ;

extern uint32 timer0_clock;
uint8 test_uart1_irq_flag = 0;
uint16 MAX485_CTRL_REG_D = 0; 
//uint8 recv_data_flag_hd = 0;

//***********************************************************************************************************************
//��������:����1�жϷ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void UART1_IRQHandler(void)
{
	uint16 status=UART1_SR;
	uint16 send_delay = 480;
	//����FIFO����(8���ֽ�)---��8���ֽڴ����ж���߽��յ�Ч��
	if(status&(0x0100|0x40))
	{
		test_uart1_irq_flag = 1;
		//15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
		UART1_TOR=15;
		//��Ӳ��FIFO���� һ�߶�һ�������FIFO
		while(UART1_SR&0x01)
		{
			uart1_recv_fifo[uart1_recv_fifo_in++]=UART1_RxBUFR;
			if(uart1_recv_fifo_in>=UART1_FIFO_SIZE)
				uart1_recv_fifo_in=0;
		}
	}
	//���ݷ������ ������Ӳ��FIFOװ������
	if(status&(0x02)) //���ݷ���Ϊ��
	{
	test_uart1_irq_flag = 2;
		//û������Ҫ���� �ط����ж�
		if(uart1_send_fifo_out==uart1_send_fifo_in)
		{
			test_uart1_irq_flag = 8;
			//sh9_tba_bus_dev.tba_send = 0;   //û������Ҫ����
			
			UART1_IER&=~(0x02);
			while(send_delay--);
			//MAX485��״̬
			//UART1_MAX485_CTRL_REG&=~(0x0001<<UART1_MAX485_CTRL_BIT);
			UART1_MAX485_CTRL_REG |= (0x0001<<UART1_MAX485_CTRL_BIT);
			//MAX485_CTRL_REG_D = UART1_MAX485_CTRL_REG;
			
			//MAX485_CTRL_REG_D |= (0x0001<<UART1_MAX485_CTRL_BIT);
			//UART1_MAX485_CTRL_REG = MAX485_CTRL_REG_D ;
			//UART1_MAX485_CTRL_REG = MAX485_CTRL_REG_D ;
			
			MAX485_CTRL_REG_D = UART1_MAX485_CTRL_REG;
				test_uart1_irq_flag = 9;
		}
		//������Ҫ���� ��Ӳ��FIFOװ��
		else
		{
		test_uart1_irq_flag = 6;
			while(UART1_SR&0x02)
			{
			test_uart1_irq_flag = 7;
				UART1_TxBUFR=uart1_send_fifo[uart1_send_fifo_out++];
				if(uart1_send_fifo_out>=UART1_FIFO_SIZE)
					uart1_send_fifo_out=0;
				if(uart1_send_fifo_out==uart1_send_fifo_in)
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
void uart1_init(uint32 bps)
{
	//ע�ᴮ���ж�
	//UART1_IRQ_SLOT=(uint32)UART1_HANDLER;//�����жϴ�������ַ---��IRQ.S���Ѿ����
	UART1_IRQ_SLOT=(UART1_IRQ_SLOT&0xFFFF0000)|UART1_IRQ_PRIORITY;//�������ȼ�--���15
	EIC_IER|=1<<UART1_IRQ_VECTOR;//ʹ���жϿ�������Ӧ�Ĳ�
	
	//���ùܽ�����ΪUART1
	GPIO0_PC0|=(0x0001<<11);
	GPIO0_PC1|=(0x0001<<11);
	GPIO0_PC2|=(0x0001<<11);
	GPIO0_PC0&=~(0x0001<<10);
	GPIO0_PC1|=(0x0001<<10);
	GPIO0_PC2&=~(0x0001<<10);
	//����MAX485���ƹܽ�����Ϊ���+����
/*	GPIO1_PC0|=(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO1_PC1&=~(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO1_PC2|=(0x0001<<UART1_MAX485_CTRL_BIT);
*/
			
	GPIO0_PC0 |= (0x0001<<UART1_MAX485_CTRL_BIT);	// ��ʼ��EN1483B��ʹ���ϵ��Ϊ�͵�ƽ
	GPIO0_PC1 &=~(0x0001<<UART1_MAX485_CTRL_BIT);
	GPIO0_PC2 |= (0x0001<<UART1_MAX485_CTRL_BIT);
	
	//MAX485��״̬
	UART1_MAX485_CTRL_REG |= (0x0001<<UART1_MAX485_CTRL_BIT);		
	//������мĴ���
	UART1_CR=0x0000;
	UART1_IER=0x0000;
	(void)UART1_RxBUFR;
	UART1_RxRSTR=0xFFFF;
	UART1_TxRSTR=0xFFFF;
	UART1_TOR=0x0000;
	UART1_GTR=0x0000;
	//���üĴ���
	UART1_CR|=0x0080;//������
	UART1_CR|=0x0400;//ʹ��Ӳ��FIFO
	UART1_RxRSTR=0xFFFF;//��Ӳ������FIFO
	UART1_TxRSTR=0xFFFF;//��Ӳ������FIFO
	UART1_CR&=~0x0040;//�رձ��ػػ�
	UART1_TOR=15;//15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж� ���ڷ�8�������������ݽ���
	UART1_BR=(uint16)(Fpcl/(16*bps));//������
	UART1_CR=(UART1_CR&0xFFDF)|0;//У�� N
	UART1_CR=(UART1_CR&0xFFF8)|0x01;//����λ 8
	UART1_CR=(UART1_CR&0xFFE7)|0x08;//ֹͣλ 1
	UART1_CR|=0x0100;//��������
	//�������ж�
	UART1_IER=0x0100|0x40;//��8���ֽڴ����ж���߽��յ�Ч�� 15��������û������ �͸���FIFO��ʣ�µ����ݴ����ж�
	UART1_IER|=(0x02);//Ӳ��FIFO����һ���ʱ�������ϵ�ʱ��---�����жϼ�����Ӳ��FIFOװ������
}

//***********************************************************************************************************************
//��������:��ѯ���� ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8 uart1_read_char(uint8 *ch)
{
	if(uart1_recv_fifo_out==uart1_recv_fifo_in)
		return 0;
	*ch=uart1_recv_fifo[uart1_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	uart1_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(uart1_recv_fifo_out==UART1_FIFO_SIZE)
		uart1_recv_fifo_out=0;//�������fifoĩβ ���ش�ͷ��ʼ
	
	//�ж�Ӳ����������
	if(*ch==HARDWARE_TEST_CMD)
	{
		uart1_hardware_test_flag++;
		if(uart1_hardware_test_flag>=200)
			uart1_hardware_test_flag=200;
	}
	else
	{
		uart1_hardware_test_flag=0;
	}	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ���һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart1_write_char(uint8 ch)
{	
	//���Ի������Ƿ���
	uint16 fifo_in_test=uart1_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=UART1_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==uart1_send_fifo_out)
		return;
	//��FIFO�������
	uart1_send_fifo[uart1_send_fifo_in++]=ch;
	if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
		uart1_send_fifo_in=0;
	test_uart1_irq_flag = 5;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART1_SR&0x02) �ͻ�����ж��Զ�����
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485����״̬
	UART1_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:���ڷ��Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart1_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=uart1_send_fifo_in;
	while(length--)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart1_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART1_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart1_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			uart1_send_fifo_in=send_fifo_in_backup;
			UART1_IER|=(0x02);
			return 0;
		}
		//��FIFO�������
		uart1_send_fifo[uart1_send_fifo_in++]=*buf++;
		if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
			uart1_send_fifo_in=0;
	}
	test_uart1_irq_flag = 4;

	if(sh9_tba_bus_dev.tba_send)
		return 0;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART1_SR&0x02) �ͻ�����ж��Զ�����
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485����״̬//MAX485����״̬	
	UART1_IER|=(0x02);
	
	return 1;
}

//***********************************************************************************************************************
//��������:���ڷ����ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart1_write_str(char *str)
{
	while(*str)
	{
		//���Ի������Ƿ���
		uint16 fifo_in_test=uart1_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=UART1_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==uart1_send_fifo_out)
			break;
		//��FIFO�������
		uart1_send_fifo[uart1_send_fifo_in++]=*str++;
		if(uart1_send_fifo_in>=UART1_FIFO_SIZE)
			uart1_send_fifo_in=0;
	}
	test_uart1_irq_flag = 3;
	//�������ж� �����жϷ��� �����ʱ��Ӳ������FIFO�п�λ��(UART1_SR&0x02) �ͻ�����ж��Զ�����
	UART1_MAX485_CTRL_REG &=~ (0x0001<<UART1_MAX485_CTRL_BIT);//MAX485����״̬//MAX485����״̬
	UART1_IER|=(0x02);
}

//***********************************************************************************************************************
//��������:��ȡӲ����������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint8 uart1_get_hardware_test(void)
{
	if(uart1_hardware_test_flag==60)
	{
		uart1_hardware_test_flag++;
		return 1;
	}
	else
	{
		return 0;
	}
}


//***********************************************************************************************************************
//��������:����fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart1_fifo_out_backup(void)
{
	uart1_recv_fifo_out_backup=uart1_recv_fifo_out;
}

//***********************************************************************************************************************
//��������:�ָ�fifo��ָ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void uart1_fifo_out_resume(void)
{
	uart1_recv_fifo_out=uart1_recv_fifo_out_backup;
}

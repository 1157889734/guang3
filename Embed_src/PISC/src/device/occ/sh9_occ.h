
//��������˿ڶ���--����Ӳ���ӿڸı�
#define OCC_BAUDRATE		9600			//������
#define OCC_PORTINIT		uart0_init		//ͨ�Ŷ˿ڳ�ʼ��
#define OCC_RECEIVE_CHAR	uart0_read_char		//ͨ�Ŷ˿ڽ���
#define OCC_SEND_BUF		uart0_write_buf		//ͨ�Ŷ˿ڷ���
#define OCC_SEND_CHAR		uart0_write_char	//ͨ�Ŷ˿ڷ���
#define OCC_FIFO_BACKUP	uart0_fifo_out_backup	//�ײ�ָ�뱣��
#define OCC_FIFO_RESUME	uart0_fifo_out_resume	//�ײ�ָ��ض�

#define OCC_LED_NO 			4		//occ singal indicate led
#define OCC_BUF_SIZE		255

//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	void (*send_data)(uint8 *,uint8);
	uint8 (*get_playing)(void);
	
	uint16 comm_error;//����ͨ�Ŵ���
	
}occ_dev_t;
extern occ_dev_t sh9_occ_dev;

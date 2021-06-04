
/******************************************************************************
*�ļ����ƣ�cab_bus.c		       		         
*��          �ߣ�hk           							 
*��          ����˾����ͨ�����߽ӿڴ���		
*�������ڣ�2010-3-10
*�޸ļ�¼����								
******************************************************************************/
#ifndef CAB_BUS_H
#define CAB_BUS_H

//��������˿ڶ���--����Ӳ���ӿڸı�
#define CAB_BUS_BAUDRATE	9600			//������
#define CAB_BUS_PORTINIT	uart2_init		//ͨ�Ŷ˿ڳ�ʼ��
#define CAB_BUS_RECV_CHAR	uart2_read_char		//ͨ�Ŷ˿ڽ���
#define CAB_BUS_SEND_BUF	uart2_write_buf		//ͨ�Ŷ˿ڷ���
#define CAB_BUS_SEND_CHAR	uart2_write_char	//ͨ�Ŷ˿ڷ���
#define CAB_BUS_FIFO_BACKUP     uart2_fifo_out_backup
#define CAB_BUS_FIFO_RESUME		uart2_fifo_out_resume

//ͨ��Э�������ַ�
#define CAB_PACKET_TAB			0x7E	//���ݰ��ָ��

//�����豸����
#define CAB_DVA               0x07    //˾��������DVA�豸���ͱ��
#define CAB_CCTV              0x06    //˾��������radio�豸���ͱ��
#define CAB_PANEL            0x0A    //˾������������豸���ͱ��


#define  DVA_DEVICE_NO			0x01	//DVA ģ���豸��
#define  RECORD_DEVICE_NO         0x02      //¼��ģ���豸��

//���������ȶ���--���ݾ���Ӧ�øı�
#define CAB_BUF_SIZE		255

#define DVA_NEW   1     
#define DVA_OLD    0  

#if DVA_NEW  //new dva
__packed typedef struct 
{
	uint8 data_length;			//���ݳ���
	uint8 device_type;			//�豸����
	uint8 device_no;			//�豸���
	uint8 cab_data[CAB_BUF_SIZE];				//�豸�����ݳ���
} cab_bus_packet_t;
#endif

#if DVA_OLD //old dva
__packed typedef struct 
{
	uint8 data_length;			//���ݳ���
	uint8 cab_data[CAB_BUF_SIZE];				//�豸�����ݳ���
} cab_bus_packet_t;
#endif

typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	uint8 (*send_data)(uint8,uint8,uint8,uint8,uint8 *);
	void (*set_debug)(uint8);
	//void (*set_debug)(uint8);
}cab_bus_dev_t;

extern cab_bus_dev_t xian2_cab_bus_dev;
#endif


//----------------------------------------------------------------------------------
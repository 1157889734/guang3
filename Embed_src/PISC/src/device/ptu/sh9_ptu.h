//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��λ������ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:ptu.h

//*�ļ�����:��λ������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PTU_H
#define PTU_H



//PTUͨ��ָʾ�ƺ�
#define PTU_LED_NO		6


//��������˿ڶ���--����Ӳ���ӿڸı�
#define PTU_BAUDRATE		9600			//������
#define PTU_PORTINIT		usb_init		//ͨ�Ŷ˿ڳ�ʼ��
#define PTU_RECEIVE_CHAR	usb_read_char		//ͨ�Ŷ˿ڽ���
#define PTU_SEND_CHAR		usb_write_char		//ͨ�Ŷ˿ڷ���
#define PTU_SEND_BUF		usb_write_buf		//ͨ�Ŷ˿ڷ���

//��������
#define SETUP_CMD_STATIONNUM			0x01	//��վ����
#define SETUP_CMD_STATIONCODE			0x02	//վ����
#define SETUP_CMD_STATIONDISTANCE		0x03	//վ���
#define SETUP_CMD_PULSEDISTANCE		0x04	//�������о���
#define SETUP_CMD_ARRIVEDISTANCE		0x05	//��վ�㲥����
#define SETUP_CMD_LEAVEDISTANCE		0x06	//��վ�㲥����
#define SETUP_CMD_VOLUME			       0x07	//����
#define SETUP_CMD_LEAVETABLE			0x08	//��վ�㲥�б�
#define SETUP_CMD_ARRIVETABLE			0x09	//��վ�㲥�б�
#define SETUP_CMD_PRE_DEST                    0x10      //�յ�վԤ��վ�㲥
#define SETUP_CMD_VERSION			       0x11	//��ѯ�汾��
#define SETUP_CMD_RESET				 0x12	//��λ
#define SETUP_CMD_ARRIVE_DEST                0x13      //���յ�վ�㲥�б�
#define SETUP_CMD_DIS_STATIONCODE        0x14     //��ʾվ����
#define SETUP_CMD_DIS_VOLUME                 0x15     //��ѯ����
#define SETUP_CMD_DIS_ERROR                    0x16     //��ѯ�쳣�洢��Ϣ
#define SETUP_CMD_DIS_FLASH                      0x17  //��ѯflash
#define SETUP_CMD_UPDATE_SOFT                0x19     //����
#define SETUP_CMD_LOAD_DEFAULT_PARM     0x20 //�ָ�Ĭ������
#define SETUP_CMD_OK                                 0x21   //�յ�ptu����󣬻ظ�������
#define SETUP_CMD_DIS_BROADCASTPRIORITY  0x22  //��ѯ�㲥���ȼ�
#define SETUP_CMD_SETSTATION_NAME          0x23    //����վ��
#define SETUP_CMD_BROC_DELAY_TIME          0x24   //�ֶ�ģʽ������ÿ��վ���ӳٱ�վʱ��
#define SETUP_CMD_DIS_BROC_DELAY_TIME   0x25   //��ѯ�ֶ�ģʽ������ÿ��վ���ӳٱ�վʱ��

#define SETUP_CMD_DMP_WDS_VERSION         0x30  //��ѯ��̬��ͼ����������汾
#define SETUP_CMD_SECTION_PLAYLIST          0x31 //�������������յ�վ��ʾ�ﲥ���б�
#define SETUP_CMD_DIS_SECTION_PLAYLIST           0x32 //��ѯ���������յ�վ��ʾ�������б�

#define SETUP_CMD_DCP_VOLUME           0x33 //���ù㲥�п�������
#define SETUP_CMD_DIS_DCP_VOLUME           0x34 //��ѯ�㲥�п�������

#define SETUP_CMD_SPECIALTIMES			  0x0B	//����㲥����
#define SETUP_CMD_BROADCASTPRIORITY	  0x0C	//�㲥���ȼ�
#define SETUP_CMD_MEDIASAMETIME		   0x0D	//�Ƿ�ͬʱý�岥��
#define SETUP_CMD_MTABLE			         0x0E	//����Ѳ���



//���������ȶ���--���ܸı� ��ϵ�����س���
#define PTU_BUF_SIZE		1024
extern uint8 ptu_recv_buf[];



//�����豸��
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	uint8 *(*get_debug_cmd)(void);
	void (*ptu_send_data)(uint8 *, uint8);
	uint8 print_flag;
}ptu_dev_t;
extern ptu_dev_t sh9_ptu_dev;



#endif



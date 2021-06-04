//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& PC������� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:setup.c

//*�ļ�����:PC�������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//��������
#define SETUP_CMD_STATIONNUM			0x01	//��վ����
#define SETUP_CMD_STATIONCODE			0x02	//վ����
#define SETUP_CMD_STATIONDISTANCE		0x03	//վ���
#define SETUP_CMD_PULSEDISTANCE			0x04	//�������о���
#define SETUP_CMD_ARRIVEDISTANCE		0x05	//��վ�㲥����
#define SETUP_CMD_LEAVEDISTANCE			0x06	//��վ�㲥����
#define SETUP_CMD_VOLUME			0x07	//����
#define SETUP_CMD_LEAVETABLE			0x08	//��վ�㲥�б�
#define SETUP_CMD_ARRIVETABLE			0x09	//��վ�㲥�б�
#define SETUP_CMD_SPECIALTIMES			0x0B	//����㲥����
#define SETUP_CMD_BROADCASTPRIORITY		0x0C	//�㲥���ȼ�
#define SETUP_CMD_MEDIASAMETIME			0x0D	//�Ƿ�ͬʱý�岥��
#define SETUP_CMD_MTABLE			0x0E	//����Ѳ���

#define SETUP_CMD_VERSION			0x11	//��ѯ�汾��
#define SETUP_CMD_RESET				0x12	//��λ


//Ӧ����
#define SETUP_ACK_DOWNLOAD			0x18	//���س���Ӧ��


//Ӧ�𻺳���
static uint8 setup_ack_buf[TBA_BUF_SIZE];
//���ݻ�����
static uint8 setup_data_buf[TBA_BUF_SIZE];


//�������ݰ��ṹ
#define DOWNLOAD_DATA_SIZE	128
typedef struct
{
	uint32 total_length;//�ܳ���
	uint32 current_length;//��ǰ����
	uint8 data_buf[DOWNLOAD_DATA_SIZE];//����
}download_packet_t;
static download_packet_t *download_packet=(download_packet_t *)&setup_data_buf;



static uint32 total_length=0;//�ܳ���
static uint32 recv_num=0;//�Ѿ����յĳ���
static uint8 download_step=0;//���ز���
static uint16 checksum_add1=0;//�Լ�������ۼ�У���
static uint16 checksum_xor1=0;//�Լ���������У���



//***********************************************************************************************************************
//��������:PC�����������
//����˵��:
//ע������:
//����˵��:
//***********************************************************************************************************************
void setup_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
	//��������
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		
	}
	//���س���
	else if(data_type==TBA_CMD_DOWNLOAD)
	{
		uint8 dev_no=src_dev_id>>4;
		uint8 encrypt;
		uint16 i;
		
		//�ж����ݱ��ܳ���
		if(length!=sizeof(download_packet_t))
			return;
		//�ж�Դ�豸���
		if((dev_no!=1))
			return;
		//��������
		memcpy(setup_data_buf, packet, sizeof(setup_data_buf));
		
		//������ڰ��Ƴ��� �˳�
		if(download_step==3)
			return;
		
		//����ǲ�������FLASH������ ����У��� ��flash
		if(download_packet->current_length==0)
		{
			//�ж��ܳ���
			if(download_packet->total_length>FLASH_BACKUP_LENGTH)
			{
				download_step=0;
				print_line("LEN ERROR");
				//����Ӧ��
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				return;					
			}
			//���ò�������FLASH
			save_set_erase();
			//��¼�ܳ��� �����ǰ���յĳ��� ���У��� �Ͳ����־
			total_length=download_packet->total_length;
			recv_num=0;
			checksum_add1=0;
			checksum_xor1=0;
			download_step=1;
			//����Ϊֹ ��������дFLASH ��Ϊ����������
			return;
		}
		
		//����д��FLASH�׶�
		if(download_step==1)
		{			
			//���ݽ���
			encrypt=download_packet->data_buf[0]^0x55;
			for(i=1;i<DOWNLOAD_DATA_SIZE;i++)
				download_packet->data_buf[i] ^= encrypt;
			//�ѽ��ջ��������д��flash
			if(!flash_write(download_packet->data_buf,FLASH_BACKUP_BASE+recv_num,DOWNLOAD_DATA_SIZE))
			{
				download_step=0;
				print_line("WRITE ERROR");//д��ʧ�� ����Ӧ��
				return;
			}
			print_line("WRITE OK");//һ����д����� ����Ӧ��

			//����У���
			for(i=0;i<DOWNLOAD_DATA_SIZE;i++)
				checksum_add1 += *((uint8 *)(FLASH_BACKUP_BASE+recv_num+i));//�����ۼ�У���
				
			for(i=0;i<DOWNLOAD_DATA_SIZE;i+=2)
				checksum_xor1 ^= *((uint16 *)(FLASH_BACKUP_BASE+recv_num+i));//�������У���
				
			//ָ����һ������
			recv_num += DOWNLOAD_DATA_SIZE;
                	//�ж϶���
			if(download_packet->current_length != recv_num)
			{
				download_step=0;
				print_line("LOSS DATA PACKET");//��ʧ���ݰ�
				//����Ӧ��
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				return;
			}
			//�жϽ�����һ��
			if(recv_num>=total_length)
			{
				download_step=2;
				//print_line("WAIT VERIFY");
			}
		}
		//����У��Ͱ��ƽ׶�
		else if(download_step==2)
		{
			//���־
			download_step=0;
					
			//���������У��Ͱ� �ж�У��� �����г������---�ӱ���flash����������flash ����������ÿ��Ź���λ
			if(download_packet->total_length==0)
			{				
				//�ж�У���
				if((*((uint16 *)&download_packet->data_buf[0])!=checksum_add1) || (*((uint16 *)&download_packet->data_buf[2])!=checksum_xor1))
				{
					print_line("VERIFY ERROR1");
					//����Ӧ��
					setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
					setup_ack_buf[1]=0;
					tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
					return;
				}
				print_line("VERIFY OK");
				
				//�ɹ�Ӧ��
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=1;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);
				
				//���ø��³���ı�־---������������� ��ΪҪӦ��PC��
				save_set_update();
				
				//������һ��
				download_step=3;
			}
			//����������� ˵�����һ����ʧ
			else
			{
				//����Ӧ��
				setup_ack_buf[0]=SETUP_ACK_DOWNLOAD;
				setup_ack_buf[1]=0;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,2);

				print_line("LOSS LAST PACKET");//��ʧ���ݰ� ����Ӧ��			
			}		
		}
	}
	//��Ե�
	else if(data_type==TBA_CMD_P2PPACKET)
	{		
		switch(packet[0])
		{
		#if 0
			//վ����
			case SETUP_CMD_STATIONCODE:
				if(packet[1] >= STATION_NUM_MAX-1)
					break;
				saveparam.stations_code_table[packet[1]] = ((uint16)packet[2]<<8)|packet[3];
				saveparam.stations_code_table[saveparam.station_total_num]=0;//������
				//�������
				save_set_task();
				break;
              #endif
			//��������
			case SETUP_CMD_VOLUME:
				saveparam.global_volume=packet[1];
				saveparam.occ_volume=packet[2];
				saveparam.dva_volume=packet[3];
				saveparam.media_volume=packet[4];
				//�������
				save_set_task();
				break;
			
			//��վ�㲥�б�
			case SETUP_CMD_LEAVETABLE:
			#if 0
				if(packet[2] >= STATION_NUM_MAX-1)
					break;
				{
					uint8 num=0;
					uint16 *src = (uint16 *)&packet[3];
					uint16 *dst;
					
					if(packet[1] == 0)
						dst = &saveparam.dva_leave_list_up[packet[2]][0];
					else
						dst = &saveparam.dva_leave_list_down[packet[2]][0];
					while(src[num])
					{
						dst[num] = src[num];
						if( ++num >= DVA_LIST_MAX-1 )
							break;//�жϳ���
					}
					dst[num]=0;//������
				}
				//�������
				save_set_task();
			#endif
				break;
			
			//����㲥����
			case SETUP_CMD_SPECIALTIMES:
				saveparam.special_broadcast_times[0]=packet[1];
				saveparam.special_broadcast_times[1]=packet[2];
				saveparam.special_broadcast_times[2]=packet[3];
				//�������
				save_set_task();
				break;
								
			//�㲥���ȼ�
			case SETUP_CMD_BROADCASTPRIORITY:
				saveparam.broadcast_priority[0]=packet[1];
				saveparam.broadcast_priority[1]=packet[2];
				saveparam.broadcast_priority[2]=packet[3];
				saveparam.broadcast_priority[3]=packet[4];
				saveparam.broadcast_priority[4]=packet[5];
				saveparam.broadcast_priority[5]=packet[6];
				saveparam.broadcast_priority[6]=packet[7];
				saveparam.broadcast_priority[7]=packet[8];
				//�������
				save_set_task();
				break;
			
			//�Ƿ�ͬʱý�岥��			
			case SETUP_CMD_MEDIASAMETIME:
				saveparam.media_sametime=packet[1];
				//�������
				save_set_task();
				break;
			
			//����Ѳ���
			case SETUP_CMD_MTABLE:
			{
				uint8 index=0;
				//������Ӧ���豸
				while(saveparam.token_table_interval[index])
				{
					if(saveparam.token_table_dev_id[index]==packet[1])
					{
						saveparam.token_table_dev_id[index]=packet[1];
						saveparam.token_table_interval[index]=((uint16)packet[2]<<8)|packet[3];
				
						//�ж��Ƿ�Ҫɾ��
						if(saveparam.token_table_interval[index]==0)
						{
							while(saveparam.token_table_interval[index+1])
							{
								saveparam.token_table_dev_id[index]=saveparam.token_table_dev_id[index+1];
								saveparam.token_table_interval[index]=saveparam.token_table_interval[index+1];
								index++;
								if(index>=TBA_M_TABLE_NUM-1)break;
							}
							saveparam.token_table_dev_id[index]=0;
							saveparam.token_table_interval[index]=0;
						}
						break;
					}
					index++;
					if(index>=TBA_M_TABLE_NUM-1)break;
				}
				//���û���ҵ��豸˵������ӵ����豸 ��ӵ�ĩβ ����ӽ�����
				if((saveparam.token_table_interval[index]==0) && (index<TBA_M_TABLE_NUM-1))
				{
					saveparam.token_table_dev_id[index]=packet[1];
					saveparam.token_table_interval[index]=((uint16)packet[2]<<8)|packet[3];
					if(saveparam.token_table_interval[index])index++;
					//��ӽ�����
					saveparam.token_table_dev_id[index]=0;
					saveparam.token_table_interval[index]=0;
				}
				//���õ�M
				tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
				//�������
				save_set_task();
				break;
			}
			
			//�汾��
			case SETUP_CMD_VERSION:
				//Ӧ��
				setup_ack_buf[0]=SETUP_CMD_VERSION;
				setup_ack_buf[1]=HARDWARE_VERSION;
				setup_ack_buf[2]=ADD_FUNCTION_VERSION;
				setup_ack_buf[3]=MAJOR_VERSION;
				setup_ack_buf[4]=MINOR_VERSION;
				tba_bus_dev->send_data(src_net_id,src_dev_id,TBA_CMD_P2PPACKET,1,setup_ack_buf,5);
				break;

			//��λ
			case SETUP_CMD_RESET:
				while(1);
				break;
										
			default:
				break;
		}
	}
}



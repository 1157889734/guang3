
/******************************************************************************
*�ļs���� ��pisc.c		       		         
*��          �� ��thomson   							 
*��          �� �����������	
*�������ڣ�2010-3-10
*GS�ļ�¼����							
******************************************************************************/


#include "include.h"



//��������
pisc_default_data_t other_default_data=
{
	{0,0,0,0,0,0,0,0,},//������־
	1,//��ʼվ
	STATION_TOTAL_NUM,//�յ�վ
	1,//��ǰվ
	2,//��һվ
	0,//Խվ�㲥
	0,//�����㲥
	{1,0,0,1,0,0,1},//״̬��־
	0,0,0,0,//Խվ����
	{0},//�㲥���ȼ�
	{0},//�㲥��Դ
	{{0},{0},{0},{0},{0},{0},{0},{0},},//32��������״̬��
	
	0,//ý����ʾģʽ
	{0},//˾����״̬
	{0},//TMS״̬
	
	0,//������õ���������
	
	0,//���еľ���---�ٷֱ�
	1,//������վ--��һ��ͣ��
	
	0,//����
};


//ͨ�Ŵ���
uint16 pisc_comm_error[TBA_PISC_NUM]={0};

/****************************************************
*���i����	pisc_proc 		         
*���ߣ�thomson             							 
*�������G��������i�I����
*���룺�S                 
*�����	
*����ֵ���S
*GS�ļ�¼���S								
****************************************************/
void pisc_proc(uint8 src_net_id, uint8 src_dev_id, uint8 data_type, uint8 *packet, uint8 length)
{
       static uint8 other_pisc_master = 0;
	//��������
	if(data_type==TBA_CMD_DEFAULTPACKET)
	{
		//�жϳ���
		if(length==sizeof(pisc_default_data_t))
		{	
			uint8 dev_no=src_dev_id>>4;
			
			//�ж�Դ�豸���
			if((dev_no) && (dev_no<=TBA_PISC_NUM))
			{
				//���ͨ�Ŵ���
				pisc_comm_error[dev_no-1]=0;
				
				//�����Է��i�I
				//if(dev_no!=((DEV_ID_DET_REG&(0x0001<<DEV_ID_DET_BIT))?1:2))
				if(dev_no!= read_pisc_id())		//xi'an Line NO.2
				{	
					memcpy( (uint8 *)&other_default_data, packet, sizeof(pisc_default_data_t) );

					//ͬ���㲥�������Խ�����˷�����					
					if(other_default_data.status_hbit.master && other_default_data.dcp_volume&0xf0 !=0)
					{
						switch(other_default_data.dcp_volume&0xf0)  //�Բ�����������
						{							
							case 1:
								saveparam.dcp_pa_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;
								
							case 2:
								saveparam.dcp_cc_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 3:
								saveparam.dcp_pc_listen_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 4:
								saveparam.dcp_pa_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 5:
								saveparam.dcp_cc_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							case 6:
								saveparam.dcp_pc_mic_volume = other_default_data.dcp_volume & 0x0f;
								break;

							default:
								break;
						}
					}
					else if(my_default_data.status_hbit.master) //�����������Զ˼�������
					{
						if((other_default_data.dcp_volume & 0x0f) != saveparam.dcp_pa_listen_volume)
							sh9_tba_bus_dev.dcp_volume_set = 1;
					}
					
					

                               	if(other_default_data.test_status.master_req || other_default_data.test_status.slave_req)
				          tba_bus_dev->data_flag = 0;
							   
					if(other_default_data.status_hbit.master && my_default_data.test_status.master_req == 0
					   && my_default_data.tms_status.tms_valid)//������������,���߶���tms��������tms���ϲ�ͬ��
					{
					    other_pisc_master = (++other_pisc_master)%200;
					}
					else
						other_pisc_master = 0;

					//�ֶ�ģʽ��,�Բ�Ϊ����ͬ����ǰվ����һվ
					if((other_pisc_master > 10) && (WORK_AT_AUTO != sh9_tms_dev.pis_work_mode))
					{
						 other_pisc_master = 0;
						 if(sh9_tms_dev.next_station != other_default_data.next_station)
						 {
						     ptu_print_debug("sync with master---next_station");
						     sh9_tms_dev.next_station = other_default_data.next_station;
						 }
						if(sh9_tms_dev.current_station != other_default_data.current_station)
						{
						   sh9_tms_dev.current_station = other_default_data.current_station;
						   ptu_print_debug("sync with master---current_station");
						}
						if(sh9_tms_dev.start_station != other_default_data.start_station)
						 {
						     ptu_print_debug("sync with master---start_station");
						     sh9_tms_dev.start_station = other_default_data.start_station;
						 }
						if(sh9_tms_dev.end_station != other_default_data.end_station)
						{
						   sh9_tms_dev.end_station = other_default_data.end_station;
						   ptu_print_debug("sync with master---end_station");
						}
					}
					//uart0_write_char(0x99);
				       //uart0_write_buf((uint8 *)&other_default_data, sizeof(pisc_default_data_t) );
				}
				//if(other_default_data.status_hbit.master)print_line("get other master");
				//else print_line("get other slave");
			}
		}
	}
	//��Ե�
	else if(data_type==TBA_CMD_P2PPACKET)
	{
		
	}
}
/****************************************************
*���i����read_pisc_id       		         
*���ߣ�thomson             							 
*��������PISC ��ID  
bit IN9 IN6  ID
	0    1    1
	1    0    2
*���룺�S                 
*������S	
*����ֵ��PISC ID
*GS�ļ�¼���S								
****************************************************/
uint8 read_pisc_id(void)
{
	uint8 dev_pisc_id = 0x0;
	if(!(DEV_ID_DET_REG &(0x0001<<DEV_ID_DET_BIT0)))
		dev_pisc_id =1;
	else if((DEV_ID_DET_REG &(0x0001<<DEV_ID_DET_BIT0)))
		dev_pisc_id =2;
	return 	dev_pisc_id;
	//return 1;
}



//�����������������������������������������������������������������������������������������������������
//���ļ�����:save.h
//���ļ�˵��:
//���ļ�����:��  ��
//����������:2007��5��
//����������������������������������������������������������������������������������������������������������������������

#include "error.h"

#ifndef SAVE_H
#define SAVE_H



//���������ַ
#define PARAM_ADDR	FLASH_PARAM_BASE

#define POLL_NUM_MAX   12     //����Ϊsaveparam_t�ṹ����Ѳ����и���

#define STATION_NUM_MAX		66	//���֧�ֵ�վ��
#define BROADCAST_NUM_MAX	9	//���֧�ֵĹ㲥ͨ��
#define LINE_NUM_MAX         3// ֧��3����


//�����������ݽṹ����
typedef struct
{
	uint32 version;//�汾��

	//�յ�վ��ʾ�ﲥ���б�
	uint8 endstation_mind_list[30][5];
	
	//����Ѳ���
	uint8 token_table_dev_id[TBA_M_TABLE_NUM];
	uint16 token_table_interval[TBA_M_TABLE_NUM];
	
	uint16 stations_code_table_up[STATION_NUM_MAX];//վ�Ĵ����
	uint16 stations_code_table_down[STATION_NUM_MAX];//վ�Ĵ����
	uint8 station_total_num;//վ����		
	
	uint16 door_sides_distance;
	
	//�������Ų�ָʾ
	uint8 door_sides_up[STATION_NUM_MAX];
	uint8 door_sides_down[STATION_NUM_MAX];

	//������վ�㲥�ľ���
	uint16  leave_broc_distance[STATION_NUM_MAX]; 
	//������վ�㲥�ľ���
	uint16 arrive_broc_distance[STATION_NUM_MAX]; 

       //��վ֮�����
	uint16 distance[LINE_NUM_MAX][STATION_NUM_MAX];

	//�յ�վԤ��վ�㲥
	//uint16 dva_pre_dest_list[STATION_NUM_MAX][DVA_LIST_MAX];
	uint32   broadcast_delay_time[STATION_NUM_MAX];  //�ֶ�ģʽ���ӳٴ�����վʱ��

	//վ��
	uint8 station_name[STATION_NUM_MAX][DVA_LIST_MAX*2];
	
	//������ʾ��
	uint16 dva_door_list[DVA_LIST_MAX];
	//������ʾ�����Ŵ���
	uint8 close_door_playtimes;
	//����㲥����
	uint8 special_broadcast_times[30];
	//�㲥���ȼ� �㲥���ȼ�:OCC��  �˿ͶԽ��� ˾���Խ��� �˹��㲥��
	// ���š�DVA�� MEDIA �������㲥����������
	uint8 broadcast_priority[BROADCAST_NUM_MAX];
	
	//����
	uint8 global_volume;//������
	uint8 occ_volume;//ͨ��1����
	uint8 dva_volume;//ͨ��2����
	uint8 media_volume;//ͨ��3����
	uint8 door_volume;//������ʾ�� ����
	uint8 apu_bass;//�ص���
	uint8 apu_treble;//����
	uint8 global_volume_pcent;//�������ٷֱ�
	uint8 occ_volume_pcent;//ͨ��1�����ٷֱ�
	uint8 dva_volume_pcent;//ͨ��2�����ٷֱ�
	uint8 media_volume_pcent;//ͨ��3�����ٷֱ�
	uint8 door_volume_pcent;//������ʾ�� �����ٷֱ�
	uint8 apu_bass_pcent;//�ص����ٷֱ�
	uint8 apu_treble_pcent;//�����ٷֱ�
	uint8 media_sametime;//�Ƿ�ͬʱý�岥��
	uint8 door_broadcast_flag;//�Ƿ񲥷Ź�����ʾ��

	//�㲥�п�������
	uint8 dcp_pa_listen_volume;  // �㲥��������
	uint8 dcp_cc_listen_volume;  // ˾���Խ���������
	uint8 dcp_pc_listen_volume; // �˿ͶԽ���������
	uint8 dcp_pa_mic_volume;  // �㲥�������
	uint8 dcp_cc_mic_volume;  // ˾���Խ��������
	uint8 dcp_pc_mic_volume;  // �˿ͶԽ��������
	
		
	uint8 panel_volume_param;//���ҹ��Ų��� MODE_PRESET<<4|VOLUME

	//begin add by hk 2010-5-24 for �쳣�洢
	uint32 error_flash_addr;   //���һ�α����ַ
	uint8  re_write_flash;      //flashд��һ�飬���׵�ַ��ͷ��ʼ��־
	error_packet_t pre_error_pack;//��һ�δ洢������ 2010-9-9
	//end add by hk 2010-5-24
	
	//У���			
	uint8 crc;
}saveparam_t;
extern saveparam_t saveparam;




//�ӿں���
extern void save_init(void);
extern void save_proc(void);
extern void save_set_task(void);
extern void save_set_erase(void);
extern void save_set_update(void);
extern void save_load_default(void);


#endif



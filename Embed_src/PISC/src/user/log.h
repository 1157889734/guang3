//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ��־ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:log.h

//*�ļ�����:��־

//*�ļ�����:Ҷ����

//*��������:2015��10��10
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

/*===========================================================================================
�ⲿ����:   
mdelay �ӿ�
read_flash �ӿ�
write_flash �ӿ�
PAGE_SIZE ��
dog �ӿ�
softdog �ӿ�

�ⲿ���:
debug.c
main.c
include.h
Broadcast.c
sh9_dcp.c
sh9_dva.c
sh9_lcu.c
============================================================================================*/
  
#ifndef LOG_H
#define LOG_H
  
#define FLASH_PAGE_SIZE PAGE_SIZE
//At45db161d  
#define FLASH_LOG_HEAD_BASE START_LOG //��־ͷ����ַ    
#define FLASH_LOG_HEAD_BASE_HEAD_DEFAULT (FLASH_LOG_HEAD_BASE-(FLASH_LOG_HEAD_BASE%FLASH_PAGE_SIZE)+FLASH_PAGE_SIZE) //������־ͷ��ַ
#define FLASH_LOG_HEAD_BASE_HEAD (FLASH_LOG_HEAD_BASE_HEAD_DEFAULT+FLASH_PAGE_SIZE)//��־ͷ����ַ 
#define FLASH_LOG_HEAD_BASE_DATA (FLASH_LOG_HEAD_BASE_HEAD+FLASH_PAGE_SIZE) //��־����ַ          
#define FLASH_LOG_HEAD_BASE_DATA_END END_LOG//��־������ַ                    
   

#define LOG_DATE_LENGTH 30   //�ļ�����  
#define LOG_LENGTH 40000       //��־����
#define LOG_SIZE 15             //��־��С
#define LOG_NAME_SIZE 3          //��־�ļ�����С

#define LOG_HEAD_FLAG 0x01234567  //��־ͷħ������ħ������ʾ��־  

#define LOG_TYPE_NULL  0x00  //����־����   
#define LOG_TYPE_DVA  0x01   //�Զ��㲥  
#define LOG_TYPE_OCC 0x02  //OCC�㲥  
#define LOG_TYPE_DCP_MANUAL 0x03 //�˹��㲥 
#define LOG_TYPE_LCU_SPECIAL 0x04 //��������  
#define LOG_TYPE_LOAD 0x05 //����
#define LOG_TYPE_DCP_TALKING  0x06//˾���Խ�   


__packed typedef struct
{//�ļ��� 151011
	uint8 year;//�� 15
	uint8 mon;//�� 10
	uint8 date;//�� 11
	uint8 flag;//����
}LOG_DATE_NAME_T; 
__packed typedef struct
{   //��־ͷ
	uint32 start_log_date_index;//��ʼ����
	uint32 end_log_date_index;//��������
	uint32 current_log_addr;//��ǰ��־��ַ  
	LOG_DATE_NAME_T log_date_name[LOG_DATE_LENGTH];//�ļ��������������б� 
	uint32 log_date_start_addr[LOG_DATE_LENGTH];//�ļ�����ַ�����ڻ���ַ�б�
	uint32 log_date_end_addr[LOG_DATE_LENGTH];  //�ļ���־�����б�
	//uint32 current_log_size;//��ǰ��������
	uint32 log_base_addr;//��־����ַ
	uint32 log_date_size;//�������� 
	uint32 log_head_flag;  //��ʾͷ  
}LOG_HEAD_T;      
__packed typedef struct
{//��־���� 15��10��11��09ʱ32��
	uint8 log_time[5];//1510110932   
	uint8 log_data[LOG_SIZE];
}LOG_T;
extern LOG_HEAD_T g_LogHead;
extern uint8 g_LogFlag;  
extern uint8 g_log_debug;     

extern uint8 g_log_data[];     
extern LOG_T g_temp_log;   
extern uint8 g_sys_start_flag;          

extern void my_memset(uint8 *buf,uint8 data,uint32 length);  
extern void my_memcpy(uint8 *dst,uint8 *src,uint32 length);  //����
extern int8 my_memcmp(uint8 * s1,uint8 *s2,uint32 length);     //�Ƚ�
extern void log_write_flash(uint32 addr, uint8 *buf, uint16 length);//дflash
extern uint8 log_read_flash(uint32 addr, uint8 *buf, uint16 length);  //��flash
extern void write_log(void);
extern void write_log_head(void);
extern void read_log_head(void);
extern uint32 read_log_file_list(uint8 *file_list_buff);  //����־�ļ��б�
extern int8 write_log_data(void);  
extern void read_log_date(LOG_DATE_NAME_T *log_file_name);//����־�ļ�    
extern void erase_log(void);         
extern void log_init_head(void);   //��־��ʼ��
extern void log_head_init_set(void);  
extern uint8 log_head_check(void);       
extern void set_log(uint8 type,uint8 *buf,uint16 length);//������־
extern void read_log_all(void);  //��������־
extern void read_log_head_default(void);    
extern void write_log_head_default(void);   
extern void change_log_head(void);
#endif  



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 日志 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:log.h

//*文件作用:日志

//*文件作者:叶江南

//*创建日期:2015年10月10
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

/*===========================================================================================
外部输入:   
mdelay 接口
read_flash 接口
write_flash 接口
PAGE_SIZE 宏
dog 接口
softdog 接口

外部输出:
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
#define FLASH_LOG_HEAD_BASE START_LOG //日志头基地址    
#define FLASH_LOG_HEAD_BASE_HEAD_DEFAULT (FLASH_LOG_HEAD_BASE-(FLASH_LOG_HEAD_BASE%FLASH_PAGE_SIZE)+FLASH_PAGE_SIZE) //备份日志头地址
#define FLASH_LOG_HEAD_BASE_HEAD (FLASH_LOG_HEAD_BASE_HEAD_DEFAULT+FLASH_PAGE_SIZE)//日志头基地址 
#define FLASH_LOG_HEAD_BASE_DATA (FLASH_LOG_HEAD_BASE_HEAD+FLASH_PAGE_SIZE) //日志基地址          
#define FLASH_LOG_HEAD_BASE_DATA_END END_LOG//日志结束地址                    
   

#define LOG_DATE_LENGTH 30   //文件总数  
#define LOG_LENGTH 40000       //日志总数
#define LOG_SIZE 15             //日志大小
#define LOG_NAME_SIZE 3          //日志文件名大小

#define LOG_HEAD_FLAG 0x01234567  //日志头魔法数，魔法数表示标志  

#define LOG_TYPE_NULL  0x00  //空日志类型   
#define LOG_TYPE_DVA  0x01   //自动广播  
#define LOG_TYPE_OCC 0x02  //OCC广播  
#define LOG_TYPE_DCP_MANUAL 0x03 //人工广播 
#define LOG_TYPE_LCU_SPECIAL 0x04 //紧急报警  
#define LOG_TYPE_LOAD 0x05 //开机
#define LOG_TYPE_DCP_TALKING  0x06//司机对讲   


__packed typedef struct
{//文件名 151011
	uint8 year;//年 15
	uint8 mon;//月 10
	uint8 date;//日 11
	uint8 flag;//保留
}LOG_DATE_NAME_T; 
__packed typedef struct
{   //日志头
	uint32 start_log_date_index;//起始索引
	uint32 end_log_date_index;//结束索引
	uint32 current_log_addr;//当前日志地址  
	LOG_DATE_NAME_T log_date_name[LOG_DATE_LENGTH];//文件名，日期名字列表 
	uint32 log_date_start_addr[LOG_DATE_LENGTH];//文件基地址，日期基地址列表
	uint32 log_date_end_addr[LOG_DATE_LENGTH];  //文件日志条数列表
	//uint32 current_log_size;//当前日期条数
	uint32 log_base_addr;//日志基地址
	uint32 log_date_size;//日期天数 
	uint32 log_head_flag;  //标示头  
}LOG_HEAD_T;      
__packed typedef struct
{//日志内容 15年10月11日09时32分
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
extern void my_memcpy(uint8 *dst,uint8 *src,uint32 length);  //复制
extern int8 my_memcmp(uint8 * s1,uint8 *s2,uint32 length);     //比较
extern void log_write_flash(uint32 addr, uint8 *buf, uint16 length);//写flash
extern uint8 log_read_flash(uint32 addr, uint8 *buf, uint16 length);  //读flash
extern void write_log(void);
extern void write_log_head(void);
extern void read_log_head(void);
extern uint32 read_log_file_list(uint8 *file_list_buff);  //读日志文件列表
extern int8 write_log_data(void);  
extern void read_log_date(LOG_DATE_NAME_T *log_file_name);//读日志文件    
extern void erase_log(void);         
extern void log_init_head(void);   //日志初始化
extern void log_head_init_set(void);  
extern uint8 log_head_check(void);       
extern void set_log(uint8 type,uint8 *buf,uint16 length);//设置日志
extern void read_log_all(void);  //读索引日志
extern void read_log_head_default(void);    
extern void write_log_head_default(void);   
extern void change_log_head(void);
#endif  



/******************************************************************************
*文件名称 ：log.c		       		         
*作          者 ：叶江南   							 
*描          述 ：日志
*创建日期：2015-10-10
*修改记录：无								
******************************************************************************/

#include "include.h"
//广播信源类型
#define PISC_BROADCAST_ICC	       0x20
#define PISC_BROADCAST_OCC	0x10
#define PISC_BROADCAST_DVA	0x08
#define PISC_BROADCAST_MEDIA	0x04
#define PISC_BROADCAST_DOOR	0x02
#define PISC_BROADCAST_SPECIAL	0x01
#define PISC_BROADCAST_NONE	0x00

LOG_HEAD_T g_LogHead;  //日志头
uint8 g_LogFlag;  //日志调试变量  
uint8 g_log_debug=0;//调试日志信息  
uint32 g_LogSize=sizeof(LOG_HEAD_T);
LOG_T g_temp_log;  //添加日志临时变量  
uint8 g_log_data[LOG_SIZE]; //临时日志文件名      
uint8 g_sys_start_flag = 0;  //系统启动，日志标志

/***************************************************************************
function    :   my_memset
description :   设置
inptu       :   buf-->目的地址
                data-->数据
                length-->长度  
****************************************************************************/
#if 1
void my_memset(uint8 *buf,uint8 data1,uint32 length)
{
	uint32 i;
	if(!(buf&&(length>0)))
	{
		return;  
	}
	for(i=0;i<length;i++)
	{
		*buf++ = data1;
	}
}
#endif
/***************************************************************************
function    :   my_memcpy
description :   复制
inptu       :   dst-->目的地址
                str-->源地址
                length-->长度
****************************************************************************/
void my_memcpy(uint8 *dst,uint8 *src,uint32 length)
{
	uint32 i;  
	if(!(dst&&src&&length>0))
	{
		return;  
	}
	for(i=0;i<length;i++)
	{  
		*dst++=*src++;
	}
}

/***************************************************************************
function    :   my_memcmp
description :   比较
inptu       :   s1-->第一个buff地址
                s2-->第二个buff地址
                length-->比较长度
return      :   0-->相等
                -2-->参数不合法
                1-->s1>s2
                -1-->s1<s2
****************************************************************************/
int8 my_memcmp(uint8 * s1,uint8 *s2,uint32 length)
{  
	uint32 i;
	if(!(s1&&s2&&length>0))
	{
		return -2;
	}
	for(i=0;i<length;i++)
	{
		if(*s1 == *s2)
		{
			s1++;
			s2++;
			continue;
		}
		else if( *s1>*s2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return 0;  
}

/***************************************************************************
function    :   log_write_flash
description :   写flash，按页写
inptu       :   addr-->目的地址
                buf-->数据地址
                length-->长度
return      :   无
****************************************************************************/
void log_write_flash(uint32 addr, uint8 *buf, uint16 length)
{
	//length 不能大于512  
	static uint8 buf_temp[PAGE_SIZE];//临时缓冲
	uint32 i= 0;//临时偏移  
	uint32 addr_temp=0;  //临时地址
	if(!((length>0)&&(length<=FLASH_PAGE_SIZE)))
	{
	    return;
	}
	if(0==buf)
	{
	    return;
	}
	addr_temp = addr-(addr%FLASH_PAGE_SIZE);//当前页起始地址
	i = addr%FLASH_PAGE_SIZE;   //当前页偏移量
	if(i+length<=FLASH_PAGE_SIZE)//写一页
	{
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);  
		my_memcpy((uint8 *)&buf_temp[i],(uint8 *)&buf[0],(uint32)length);      
		write_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);
		mdelay(50); 
	}
	else//跨页写
	{
		#if 1
		//写第一页
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);   
		my_memcpy((uint8 *)&buf_temp[i],(uint8 *)&buf[0],(uint32)(FLASH_PAGE_SIZE-i));      
		write_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);    
		mdelay(50);      
		
        //写第二页
		addr_temp += FLASH_PAGE_SIZE;     
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);      
		//ptu_print_buf((uint8 *)&buf_temp[0],(uint32)((uint32)length+i-PAGE_SIZE));  
		my_memcpy((uint8 *)&buf_temp[0],(uint8 *)&buf[FLASH_PAGE_SIZE-i],(uint32)((uint32)length+i-FLASH_PAGE_SIZE));               
		write_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);   
		mdelay(50); 
		#endif
	}

	 
}

/***************************************************************************
function    :   log_read_flash
description :   读flash，按页写
inptu       :   addr-->目的地址
                buf-->数据地址
                length-->长度
return      :   无
****************************************************************************/
uint8 log_read_flash(uint32 addr, uint8 *buf, uint16 length)
{
	//length 不能大于512  
	static uint8 buf_temp[FLASH_PAGE_SIZE];//临时缓冲
	uint32 i= 0;//偏移量
	uint32 addr_temp=0;  //临时地址
	
	if(!(length>0&&length<=FLASH_PAGE_SIZE))
	{
	    return 1;  
	}
	if(0==buf)
	{
	    return 1;
	}
	if(!(addr<=END_LOG&&addr>=START_LOG))
	{
	    //return 1;
	}
	if((addr + (uint32)length - 1) > END_LOG)
	{
		//return 1;
	}   
	addr_temp = addr-(addr%FLASH_PAGE_SIZE);//当前页起始地址
	i = addr%FLASH_PAGE_SIZE;   //当前页偏移量
	if(i+length<=FLASH_PAGE_SIZE)//读一页
	{
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);  
		my_memcpy((uint8 *)&buf[0],(uint8 *)&buf_temp[i],(uint32)length);      
		//write_flash(addr_temp,(uint8 *)&buf_temp[0],PAGE_SIZE);    
		//mdelay(100);  
	}
	else//跨页读
	{
		#if 1   
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);  
		my_memcpy((uint8 *)&buf[0],(uint8 *)&buf_temp[i],(uint32)(FLASH_PAGE_SIZE-i));    
		//mdelay(50);
		addr_temp += FLASH_PAGE_SIZE;     
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);      
		my_memcpy((uint8 *)&buf[FLASH_PAGE_SIZE-i],(uint8 *)&buf_temp[0],(uint32)((uint32)length+i-FLASH_PAGE_SIZE));               
		#endif
	}
	return 0;
}

/***************************************************************************
function    :   set_log
description :   写日志3级
inptu       :   type-->日志类型
                buf-->数据地址
                length-->长度
return      :   无
****************************************************************************/
void set_log(uint8 type,uint8 *buf,uint16 length)
{           
	if(length<=sizeof(g_temp_log)-6)    
	{
	}
	else
	{
		print_line("set_log error");
		return;   
	}
	if(LOG_TYPE_OCC==type)
	{
	}
	else
	{
		if(PISC_BROADCAST_OCC==other_default_data.broadcast_signal.broadcast_type)
		{
			return ;
		}
	}
	//时间设置
    my_memset((uint8 *)&g_temp_log,0,sizeof(g_temp_log));
	rtc_dev_p->get_rtc((uint8 *)&g_temp_log.log_time);    
	g_temp_log.log_data[0]=type; //设置日志类型   
	if(length&&buf)//非空日志  
	{
	    //复制日志内容
		my_memcpy((uint8 *)&g_temp_log.log_data[1],(uint8 *)&buf[0],length);  
	}
	else//空日志
	{
		
	}
	write_log();  //写日志  
}
  
/***************************************************************************
function    :   read_log_file_list
description :   读日志文件列表
inptu       :   buff-->数据地址
return      :   buff的数据长度
****************************************************************************/
uint32 read_log_file_list(uint8 *buff)
{
	uint32 date_length;//日志文件个数
	date_length =0;

	log_head_check();//检查日志头合法性  
	if(0==g_LogHead.log_date_size)
	{
		print_line("log file is null");
		return 0;  
	}
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)//起始索引小于结束索引
	{
		date_length =  g_LogHead.end_log_date_index-g_LogHead.start_log_date_index+1;
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[g_LogHead.start_log_date_index],date_length*sizeof(LOG_DATE_NAME_T));
	}
	else   //起始索引大于结束索引
	{
		date_length = LOG_DATE_LENGTH-g_LogHead.start_log_date_index;   
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[g_LogHead.start_log_date_index],date_length*sizeof(LOG_DATE_NAME_T));
		buff = buff+date_length*sizeof(LOG_DATE_NAME_T);
		date_length = date_length+g_LogHead.end_log_date_index+1;  
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[0],(g_LogHead.end_log_date_index+1)*sizeof(LOG_DATE_NAME_T));  
	}   
	return date_length*sizeof(LOG_DATE_NAME_T);//返回日志文件列表大小
}

/***************************************************************************
function    :   read_log_all
description :   读所有日志
inptu       :   无
return      :   无
****************************************************************************/
void read_log_all(void)
{
	
	uint32 i;    
	log_head_check(); //检查日志头合法性   
	print_line("read_log_all start");
	if(g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]<=g_LogHead.current_log_addr)
	{
	    //起始地址小于当前地址
		for(i=g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index];i<g_LogHead.current_log_addr;i+=sizeof(g_temp_log))
		{
			log_read_flash(i,(uint8 *)&g_temp_log,sizeof(LOG_T));
			ptu_print_buf((uint8 *)&g_temp_log,sizeof(g_temp_log));
			mdelay(1); 
			dog();
			softdog();  
		}
	}
	else
	{
	    //起始地址大于当前地址
	    
	    //读起始地址到末尾日志
		for(i=g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index];i+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;i+=sizeof(g_temp_log))  
		{   
			log_read_flash(i,(uint8 *)&g_temp_log,sizeof(LOG_T));
			ptu_print_buf((uint8 *)&g_temp_log,sizeof(g_temp_log));
			mdelay(1); 
			dog();
			softdog();  
		}

		//读基地址到当前地址日志
		for(i=g_LogHead.log_base_addr;i<g_LogHead.current_log_addr;i+=sizeof(g_temp_log))
		{
			log_read_flash(i,(uint8 *)&g_temp_log,sizeof(LOG_T));
			ptu_print_buf((uint8 *)&g_temp_log,sizeof(g_temp_log));  
			mdelay(1);    
			dog();
			softdog();     
		}
	}
	print_line("read_log_all end");
}

/***************************************************************************
function    :   read_log_date
description :   读日志文件
inptu       :   log_file_name-->文件名
return      :   无
****************************************************************************/
#if 0
void read_log_date(LOG_DATE_NAME_T *log_file_name)
{     
	uint32 i,j;  
	uint8 temp[LOG_NAME_SIZE];    
    my_memset((uint8 *)temp[0],0,LOG_NAME_SIZE);
    //判断文件名是否是空
	if(0==my_memcmp((uint8 * )log_file_name, (uint8 *)&temp[0],LOG_NAME_SIZE ))
	{
		print_line("read log file is error");  
		return ;
	}
	log_init_head();
	if(0!=log_head_check())      //检查日志头合法?
	{
		log_init_head();
		return;
	}
	//起始文件索引小于结束文件索引
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)
	{
		for(i=g_LogHead.start_log_date_index;i<=g_LogHead.end_log_date_index;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//读日志时，判断地址合法性
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//起始地址小于结束地址
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//读日志内容，起始地址到结束地址
					for(j=g_LogHead.log_date_start_addr[i];j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						//mdelay(30);      
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log));   
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));    
						mdelay(1); 
						dog();
						softdog();    
						#endif 
					}  
				}
				else//起始地址大于结束地址 
				{
				    //读日志内容，起始地址到末尾
					for(j=g_LogHead.log_date_start_addr[i];j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log));          
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
					//读日志内容，基地址到结束地址
					for(j=g_LogHead.log_base_addr;j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log));   		  
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
				}  
			}
			
		}
	}
	else//起始文件索引大于结束文件索引
	{
	    //  起始索引到末尾查找文件
		for(i=g_LogHead.start_log_date_index;i<LOG_DATE_LENGTH;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{
			    //读日志时，判断地址合法性
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
			    //起始地址小于结束地址  
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
				    //读日志内容，起始地址到结束地址
					for(j=g_LogHead.log_date_start_addr[i];j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log));
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
				}
				else//起始地址大于结束地址
				{
				    //读日志内容，起始地址到末尾
					for(j=g_LogHead.log_date_start_addr[i];j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log)); 
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
					//读日志内容，基地址到结束地址
					for(j=g_LogHead.log_base_addr;j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log)); 
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif  
					}
				}
			} 
		}  
		//0到结束索引查找文件
		for(i=0;i<=g_LogHead.end_log_date_index;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{
			    //起始地址小于结束地址
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
				    //读日志内容，起始地址到结束地址
					for(j=g_LogHead.log_date_start_addr[i];j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log));
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
				}
				else//起始地址大于结束地址
				{
				    //读日志内容，起始地址到末尾
					for(j=g_LogHead.log_date_start_addr[i];j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log)); 
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1); 
						dog();
						softdog();  
						#endif 
					}
					//读日志内容，基地址到结束地址
					for(j=g_LogHead.log_base_addr;j<g_LogHead.log_date_end_addr[i];j+=sizeof(LOG_T))
					{
						#if 1         
						log_read_flash(j,(uint8 *)&g_temp_log,sizeof(LOG_T));
						my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&g_temp_log,sizeof(g_temp_log)); 
						ptu_send_data(ptu_data_send_p,3+sizeof(g_temp_log));  
						mdelay(1);   
						dog();
						softdog();    
						#endif  
					}
				}
				return ;
			}
		}
	}
}
#endif
#if 1
#define __log_num 25
#define __log_ptu_send_delay 30    //出错延迟再发送
#define __log_send_delay 30       //25条日志延迟一次
#define __log_send_pack_delay 30
void read_log_date(LOG_DATE_NAME_T *log_file_name)
{     
	uint32 i,j;  
	uint8 temp[LOG_NAME_SIZE];  
	
	uint8 log_data_buf[FLASH_PAGE_SIZE];
	uint32 log_num;
	uint32 log_size;
	uint32 log_i;
    my_memset((uint8 *)temp[0],0,LOG_NAME_SIZE);
    //判断文件名是否是空
	if(0==my_memcmp((uint8 * )log_file_name, (uint8 *)&temp[0],LOG_NAME_SIZE ))
	{
		print_line("read log file is error");  
		return ;
	}
	if(0!=log_head_check())      //检查日志头合法?
	{
		log_init_head();
		return;
	}
	//起始文件索引小于结束文件索引
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)
	{
		for(i=g_LogHead.start_log_date_index;i<=g_LogHead.end_log_date_index;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//读日志时，判断地址合法性
				if(g_LogHead.log_date_start_addr[i]+sizeof(LOG_T)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(LOG_T)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//起始地址小于结束地址
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//读日志内容，起始地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						//mdelay(30);      
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
									
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}  
				}
				else//起始地址大于结束地址 
				{
				    //读日志内容，起始地址到末尾
				       log_size = (FLASH_LOG_HEAD_BASE_DATA_END-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
					//读日志内容，基地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_base_addr)/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_base_addr;
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						} 
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
				}  
			}
			
		}
	}
	else//起始文件索引大于结束文件索引
	{
	    //  起始索引到末尾查找文件
		for(i=g_LogHead.start_log_date_index;i<LOG_DATE_LENGTH;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//读日志时，判断地址合法性
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//起始地址小于结束地址
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//读日志内容，起始地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						//mdelay(30);      
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						} 
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}  
				}
				else//起始地址大于结束地址 
				{
				    //读日志内容，起始地址到末尾
				       log_size = (FLASH_LOG_HEAD_BASE_DATA_END-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
					//读日志内容，基地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_base_addr)/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_base_addr;
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
				}  
			} 
		}  
		//0到结束索引查找文件
		for(i=0;i<=g_LogHead.end_log_date_index;i++)
		{
		    //查找文件名
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//读日志时，判断地址合法性
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//起始地址小于结束地址
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//读日志内容，起始地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						//mdelay(30);      
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}  
				}
				else//起始地址大于结束地址 
				{
				    //读日志内容，起始地址到末尾
				       log_size = (FLASH_LOG_HEAD_BASE_DATA_END-g_LogHead.log_date_start_addr[i])/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_date_start_addr[i];
					for(;j+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
					//读日志内容，基地址到结束地址
					log_size = (g_LogHead.log_date_end_addr[i]-g_LogHead.log_base_addr)/sizeof(LOG_T);
					log_num = 0;
					j=g_LogHead.log_base_addr;
					for(;j<g_LogHead.log_date_end_addr[i];)
					{
						#if 1       
						if((log_size-log_num)>=__log_num)
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*__log_num));
							j+=(sizeof(LOG_T)*__log_num);
							log_num+=__log_num;
							for(log_i=0;log_i<__log_num;)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							  
						}
						else
						{
							my_memset(log_data_buf,0,FLASH_PAGE_SIZE);
							log_read_flash(j,(uint8 *)log_data_buf,(sizeof(LOG_T)*(log_size-log_num)));
							j+=(sizeof(LOG_T)*(log_size-log_num));
							for(log_i=0;log_i<(log_size-log_num);)
							{
								my_memcpy((uint8 *)(ptu_data_send_p+3),(uint8 *)&log_data_buf[log_i*sizeof(LOG_T)],sizeof(g_temp_log));   
								if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
								{
									mdelay(__log_ptu_send_delay);
									dog();
									softdog();
									if(0==ptu_send_data(ptu_data_send_p,3+sizeof(LOG_T)))
									{
										return ;
									}
								}
								log_i++;
							}
							log_num=log_size;
							
						}
						mdelay(__log_send_delay);
						dog();
						softdog();   
						#endif 
					}
				}  
			}
		}
	}
}

#endif

/***************************************************************************
function    :   write_log_data
description :   写日志1级
inptu       :   无
return      :   无  
****************************************************************************/
int8 write_log_data(void)
{  
       //read_log_head();
       LOG_T temp_log;
	uint8 log_buf[FLASH_PAGE_SIZE];
	read_log_head();
	if(0!=log_head_check())
	{
		ptu_print_buf((uint8 *)&g_LogHead,sizeof(g_LogHead));
		log_init_head();
		return 1;
	}
	if(0 != my_memcmp((uint8 *)&g_temp_log.log_time, (uint8 *)&g_LogHead.log_date_name[g_LogHead.end_log_date_index],LOG_NAME_SIZE))
	{    
		//更新结束索引日志
		if(0==g_LogHead.log_date_size)     
		{
			//第一次写   
		}  
		else//不是第一次写
		{
		    //更新结束索引
			g_LogHead.end_log_date_index = (g_LogHead.end_log_date_index+1)%LOG_DATE_LENGTH;
			//起始索引和结束索引是否重叠
			if(g_LogHead.start_log_date_index==g_LogHead.end_log_date_index)
			{   
			    //更新起始索引
				g_LogHead.start_log_date_index = (g_LogHead.start_log_date_index +1)%LOG_DATE_LENGTH; 
				//更新文件数
				g_LogHead.log_date_size--;        
			}
		}   
		//更新结束文件名
		my_memcpy((uint8 *)&g_LogHead.log_date_name[g_LogHead.end_log_date_index],(uint8 *)&g_temp_log.log_time,LOG_NAME_SIZE);
		//更新结束文件起始地址
		g_LogHead.log_date_start_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
		//更新结束文件结束地址
		g_LogHead.log_date_end_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
		g_LogHead.log_date_size++;  
		ptu_print_buf((uint8 *)&g_temp_log.log_time,sizeof(g_temp_log));
	} 
	//写日志  
	if(1==g_log_debug)
	{
	    //打印写入记录
	    print_int("%x write g_LogHead.log_date_start_addr",g_LogHead.log_date_start_addr[g_LogHead.end_log_date_index]); 
	    print_int("%x write g_LogHead.current_log_addr",g_LogHead.current_log_addr);  
	    ptu_print_buf((uint8 *)&g_LogHead,5);
	    //ptu_print_buf((uint8 * )&g_temp_log,sizeof(g_temp_log));
	}
	log_write_flash(g_LogHead.current_log_addr,(uint8 *)&g_temp_log,sizeof(g_temp_log));

	//写完日志，更新日志头
		  
	g_LogHead.current_log_addr += sizeof(g_temp_log);
	if(g_LogHead.current_log_addr+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END)
	{
		//检查当前地址合法性
		g_LogHead.current_log_addr = g_LogHead.log_base_addr;   
	}        
	if((g_LogHead.current_log_addr<=g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index])&&(g_LogHead.current_log_addr+sizeof(g_temp_log)> g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]))
	{
		//检查当前地址是否超过数据内存，更新起始日期索引  
		if(g_LogHead.log_date_size>1)  
		{   
			g_LogHead.start_log_date_index = (g_LogHead.start_log_date_index+1)%LOG_DATE_LENGTH;
			g_LogHead.log_date_size--;
			if(1==g_LogHead.log_date_size)
			{
				g_LogHead.end_log_date_index = g_LogHead.start_log_date_index;
			}
		}
		else
		{   
			g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index] += sizeof(g_temp_log);  
			if((g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END)
			||g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]>FLASH_LOG_HEAD_BASE_DATA_END
			||g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]<FLASH_LOG_HEAD_BASE_DATA)
			{
				//检查当前地址合法性    
				g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index] = g_LogHead.log_base_addr;   
			}
			g_LogHead.log_date_end_addr[g_LogHead.start_log_date_index] = g_LogHead.current_log_addr;  
		}
	}
		//更新结束文件结束地址
	g_LogHead.log_date_end_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
    return 0;
	
}

/***************************************************************************
function    :   log_head_check
description :   检查日志头合法性
inptu       :   无
return      :   0-->合法
                非0-->不合法  
****************************************************************************/
uint8 log_head_check(void)  
{
	#if 1   
	if((g_LogHead.log_date_size>LOG_DATE_LENGTH))     
	{
		return 1;
	}
	//检查日志头标志
	if(g_LogHead.log_head_flag != LOG_HEAD_FLAG)
	{
		return 1;
	}
	//检查日志基地址
	if(g_LogHead.log_base_addr != (FLASH_LOG_HEAD_BASE_DATA) )   
	{
        return 1;
	}
	//检查起始索引
	if(g_LogHead.start_log_date_index >= LOG_DATE_LENGTH)
	{
		return 1;
	}
	//检查结束索引   
	if(g_LogHead.end_log_date_index >= LOG_DATE_LENGTH)
	{
		return 1;  
	}
	//检查当前地址
	if(g_LogHead.current_log_addr+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
		||g_LogHead.current_log_addr<FLASH_LOG_HEAD_BASE_DATA)  
	{ 
		return 1;  
	}
       return 0;
	#endif
}    

/***************************************************************************
function    :   read_log_head
description :   读日志头
inptu       :   无
return      :   无 
****************************************************************************/
void read_log_head(void)
{   
	my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
	log_read_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
}

/***************************************************************************
function    :   read_log_head_default
description :   读备份日志头
inptu       :   无
return      :   无 
****************************************************************************/
void read_log_head_default(void)     
{
	log_read_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
}

/***************************************************************************
function    :   write_log_head_default
description :   写备份日志头
inptu       :   无
return      :   无 
****************************************************************************/
void write_log_head_default(void)
{
	#if 1       
	if(0==log_head_check())
	{
		log_write_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead));  
	}
	else
	{
		log_init_head();   
	}
	#endif
}

/***************************************************************************
function    :   write_log_head
description :   写日志头
inptu       :   无
return      :   无 
****************************************************************************/
void write_log_head(void)
{
	#if 1         
	LOG_HEAD_T LogHead;
	LOG_HEAD_T LogHeadtemp;
	if(0==log_head_check())
	{
		log_write_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));
		log_write_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead));
	}
	else
	{
		log_init_head();  
	}
	#endif
}

/***************************************************************************
function    :   write_log
description :   写日志2级
inptu       :   无
return      :   无 
****************************************************************************/
void write_log(void)
{    
	uint8 temp[LOG_NAME_SIZE];   
	//写日志         
	//判断g_temp_log是否为空     
    my_memset((uint8 *)&temp[0],0,LOG_NAME_SIZE);
	if(0==my_memcmp((uint8 * )&g_temp_log.log_time, (uint8 *)&temp[0],LOG_NAME_SIZE ))  
	{
		print_line("write log is error");     
		return ;  
	}
    if(0!=write_log_data())
    {
        print_line("write_log_data error");
        log_init_head();
        return ;
        
    }
	     
	//写日志头    
	write_log_head();
	//写日志头备份   
	//write_log_head_default();
	mdelay(30);
	print_line("write_log_data OK");  
}
void read_log(void)
{//没写完
print_line("已经取消");   
} 

/***************************************************************************
function    :   log_head_init_set
description :   初始化日志头1级
inptu       :   无
return      :   无 
****************************************************************************/
void log_head_init_set(void)
{  
		my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
		g_LogHead.log_base_addr = FLASH_LOG_HEAD_BASE_DATA;      
		g_LogHead.current_log_addr = g_LogHead.log_base_addr;
		g_LogHead.start_log_date_index = 0;  
		g_LogHead.end_log_date_index = 0;
		g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index] = g_LogHead.log_base_addr;
		g_LogHead.log_date_end_addr[g_LogHead.start_log_date_index] = g_LogHead.log_base_addr;
		g_LogHead.log_date_start_addr[g_LogHead.end_log_date_index] = g_LogHead.log_base_addr;
		g_LogHead.log_date_end_addr[g_LogHead.end_log_date_index] = g_LogHead.log_base_addr;  
		g_LogHead.log_head_flag = LOG_HEAD_FLAG;    
		g_LogHead.log_date_size = 0;       
	print_line("log_head_init_set");      
}

/***************************************************************************
function    :   log_init_head
description :   初始化日志头2级
inptu       :   无
return      :   无 
****************************************************************************/
void log_init_head(void)
{     
	mdelay(50);  
	my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
	log_read_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));
	if(0!=log_head_check())
	{
		my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
		read_log_head_default();       
		if(0!=log_head_check())
		{
			log_head_init_set();   
			log_write_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
			log_write_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead)); 
		}
		else
		{
			log_write_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));    
		}
	}
	else
	{   
		log_write_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead)); 
	}
	print_line("log_init_head");   
	  

}  

/***************************************************************************
function    :   erase_log
description :   擦除日志头
inptu       :   无
return      :   无    
****************************************************************************/
void erase_log(void)     
{ 
    my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
	//log_init_head();     
	log_write_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
	log_write_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead)); 
	log_init_head(); 
	return ;    
}

void change_log_head(void)
{
	log_init_head();
	g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]=g_LogHead.log_base_addr;
}
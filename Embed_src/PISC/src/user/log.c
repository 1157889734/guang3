/******************************************************************************
*�ļ����� ��log.c		       		         
*��          �� ��Ҷ����   							 
*��          �� ����־
*�������ڣ�2015-10-10
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"
//�㲥��Դ����
#define PISC_BROADCAST_ICC	       0x20
#define PISC_BROADCAST_OCC	0x10
#define PISC_BROADCAST_DVA	0x08
#define PISC_BROADCAST_MEDIA	0x04
#define PISC_BROADCAST_DOOR	0x02
#define PISC_BROADCAST_SPECIAL	0x01
#define PISC_BROADCAST_NONE	0x00

LOG_HEAD_T g_LogHead;  //��־ͷ
uint8 g_LogFlag;  //��־���Ա���  
uint8 g_log_debug=0;//������־��Ϣ  
uint32 g_LogSize=sizeof(LOG_HEAD_T);
LOG_T g_temp_log;  //�����־��ʱ����  
uint8 g_log_data[LOG_SIZE]; //��ʱ��־�ļ���      
uint8 g_sys_start_flag = 0;  //ϵͳ��������־��־

/***************************************************************************
function    :   my_memset
description :   ����
inptu       :   buf-->Ŀ�ĵ�ַ
                data-->����
                length-->����  
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
description :   ����
inptu       :   dst-->Ŀ�ĵ�ַ
                str-->Դ��ַ
                length-->����
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
description :   �Ƚ�
inptu       :   s1-->��һ��buff��ַ
                s2-->�ڶ���buff��ַ
                length-->�Ƚϳ���
return      :   0-->���
                -2-->�������Ϸ�
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
description :   дflash����ҳд
inptu       :   addr-->Ŀ�ĵ�ַ
                buf-->���ݵ�ַ
                length-->����
return      :   ��
****************************************************************************/
void log_write_flash(uint32 addr, uint8 *buf, uint16 length)
{
	//length ���ܴ���512  
	static uint8 buf_temp[PAGE_SIZE];//��ʱ����
	uint32 i= 0;//��ʱƫ��  
	uint32 addr_temp=0;  //��ʱ��ַ
	if(!((length>0)&&(length<=FLASH_PAGE_SIZE)))
	{
	    return;
	}
	if(0==buf)
	{
	    return;
	}
	addr_temp = addr-(addr%FLASH_PAGE_SIZE);//��ǰҳ��ʼ��ַ
	i = addr%FLASH_PAGE_SIZE;   //��ǰҳƫ����
	if(i+length<=FLASH_PAGE_SIZE)//дһҳ
	{
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);  
		my_memcpy((uint8 *)&buf_temp[i],(uint8 *)&buf[0],(uint32)length);      
		write_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);
		mdelay(50); 
	}
	else//��ҳд
	{
		#if 1
		//д��һҳ
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);   
		my_memcpy((uint8 *)&buf_temp[i],(uint8 *)&buf[0],(uint32)(FLASH_PAGE_SIZE-i));      
		write_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);    
		mdelay(50);      
		
        //д�ڶ�ҳ
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
description :   ��flash����ҳд
inptu       :   addr-->Ŀ�ĵ�ַ
                buf-->���ݵ�ַ
                length-->����
return      :   ��
****************************************************************************/
uint8 log_read_flash(uint32 addr, uint8 *buf, uint16 length)
{
	//length ���ܴ���512  
	static uint8 buf_temp[FLASH_PAGE_SIZE];//��ʱ����
	uint32 i= 0;//ƫ����
	uint32 addr_temp=0;  //��ʱ��ַ
	
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
	addr_temp = addr-(addr%FLASH_PAGE_SIZE);//��ǰҳ��ʼ��ַ
	i = addr%FLASH_PAGE_SIZE;   //��ǰҳƫ����
	if(i+length<=FLASH_PAGE_SIZE)//��һҳ
	{
		read_flash(addr_temp,(uint8 *)&buf_temp[0],FLASH_PAGE_SIZE);  
		my_memcpy((uint8 *)&buf[0],(uint8 *)&buf_temp[i],(uint32)length);      
		//write_flash(addr_temp,(uint8 *)&buf_temp[0],PAGE_SIZE);    
		//mdelay(100);  
	}
	else//��ҳ��
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
description :   д��־3��
inptu       :   type-->��־����
                buf-->���ݵ�ַ
                length-->����
return      :   ��
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
	//ʱ������
    my_memset((uint8 *)&g_temp_log,0,sizeof(g_temp_log));
	rtc_dev_p->get_rtc((uint8 *)&g_temp_log.log_time);    
	g_temp_log.log_data[0]=type; //������־����   
	if(length&&buf)//�ǿ���־  
	{
	    //������־����
		my_memcpy((uint8 *)&g_temp_log.log_data[1],(uint8 *)&buf[0],length);  
	}
	else//����־
	{
		
	}
	write_log();  //д��־  
}
  
/***************************************************************************
function    :   read_log_file_list
description :   ����־�ļ��б�
inptu       :   buff-->���ݵ�ַ
return      :   buff�����ݳ���
****************************************************************************/
uint32 read_log_file_list(uint8 *buff)
{
	uint32 date_length;//��־�ļ�����
	date_length =0;

	log_head_check();//�����־ͷ�Ϸ���  
	if(0==g_LogHead.log_date_size)
	{
		print_line("log file is null");
		return 0;  
	}
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)//��ʼ����С�ڽ�������
	{
		date_length =  g_LogHead.end_log_date_index-g_LogHead.start_log_date_index+1;
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[g_LogHead.start_log_date_index],date_length*sizeof(LOG_DATE_NAME_T));
	}
	else   //��ʼ�������ڽ�������
	{
		date_length = LOG_DATE_LENGTH-g_LogHead.start_log_date_index;   
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[g_LogHead.start_log_date_index],date_length*sizeof(LOG_DATE_NAME_T));
		buff = buff+date_length*sizeof(LOG_DATE_NAME_T);
		date_length = date_length+g_LogHead.end_log_date_index+1;  
		my_memcpy(buff,(uint8 *)&g_LogHead.log_date_name[0],(g_LogHead.end_log_date_index+1)*sizeof(LOG_DATE_NAME_T));  
	}   
	return date_length*sizeof(LOG_DATE_NAME_T);//������־�ļ��б��С
}

/***************************************************************************
function    :   read_log_all
description :   ��������־
inptu       :   ��
return      :   ��
****************************************************************************/
void read_log_all(void)
{
	
	uint32 i;    
	log_head_check(); //�����־ͷ�Ϸ���   
	print_line("read_log_all start");
	if(g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]<=g_LogHead.current_log_addr)
	{
	    //��ʼ��ַС�ڵ�ǰ��ַ
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
	    //��ʼ��ַ���ڵ�ǰ��ַ
	    
	    //����ʼ��ַ��ĩβ��־
		for(i=g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index];i+sizeof(LOG_T)<FLASH_LOG_HEAD_BASE_DATA_END;i+=sizeof(g_temp_log))  
		{   
			log_read_flash(i,(uint8 *)&g_temp_log,sizeof(LOG_T));
			ptu_print_buf((uint8 *)&g_temp_log,sizeof(g_temp_log));
			mdelay(1); 
			dog();
			softdog();  
		}

		//������ַ����ǰ��ַ��־
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
description :   ����־�ļ�
inptu       :   log_file_name-->�ļ���
return      :   ��
****************************************************************************/
#if 0
void read_log_date(LOG_DATE_NAME_T *log_file_name)
{     
	uint32 i,j;  
	uint8 temp[LOG_NAME_SIZE];    
    my_memset((uint8 *)temp[0],0,LOG_NAME_SIZE);
    //�ж��ļ����Ƿ��ǿ�
	if(0==my_memcmp((uint8 * )log_file_name, (uint8 *)&temp[0],LOG_NAME_SIZE ))
	{
		print_line("read log file is error");  
		return ;
	}
	log_init_head();
	if(0!=log_head_check())      //�����־ͷ�Ϸ��
	{
		log_init_head();
		return;
	}
	//��ʼ�ļ�����С�ڽ����ļ�����
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)
	{
		for(i=g_LogHead.start_log_date_index;i<=g_LogHead.end_log_date_index;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//����־ʱ���жϵ�ַ�Ϸ���
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//��ʼ��ַС�ڽ�����ַ
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ 
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
	else//��ʼ�ļ��������ڽ����ļ�����
	{
	    //  ��ʼ������ĩβ�����ļ�
		for(i=g_LogHead.start_log_date_index;i<LOG_DATE_LENGTH;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{
			    //����־ʱ���жϵ�ַ�Ϸ���
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
			    //��ʼ��ַС�ڽ�����ַ  
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
				    //����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
		//0���������������ļ�
		for(i=0;i<=g_LogHead.end_log_date_index;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{
			    //��ʼ��ַС�ڽ�����ַ
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
				    //����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
#define __log_ptu_send_delay 30    //�����ӳ��ٷ���
#define __log_send_delay 30       //25����־�ӳ�һ��
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
    //�ж��ļ����Ƿ��ǿ�
	if(0==my_memcmp((uint8 * )log_file_name, (uint8 *)&temp[0],LOG_NAME_SIZE ))
	{
		print_line("read log file is error");  
		return ;
	}
	if(0!=log_head_check())      //�����־ͷ�Ϸ��
	{
		log_init_head();
		return;
	}
	//��ʼ�ļ�����С�ڽ����ļ�����
	if(g_LogHead.start_log_date_index<=g_LogHead.end_log_date_index)
	{
		for(i=g_LogHead.start_log_date_index;i<=g_LogHead.end_log_date_index;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//����־ʱ���жϵ�ַ�Ϸ���
				if(g_LogHead.log_date_start_addr[i]+sizeof(LOG_T)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(LOG_T)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//��ʼ��ַС�ڽ�����ַ
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ 
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
	else//��ʼ�ļ��������ڽ����ļ�����
	{
	    //  ��ʼ������ĩβ�����ļ�
		for(i=g_LogHead.start_log_date_index;i<LOG_DATE_LENGTH;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//����־ʱ���жϵ�ַ�Ϸ���
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//��ʼ��ַС�ڽ�����ַ
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ 
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
		//0���������������ļ�
		for(i=0;i<=g_LogHead.end_log_date_index;i++)
		{
		    //�����ļ���
			if(0==my_memcmp((uint8 *)&g_LogHead.log_date_name[i],(uint8 *)log_file_name,LOG_NAME_SIZE))
			{    
				//����־ʱ���жϵ�ַ�Ϸ���
				if(g_LogHead.log_date_start_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_start_addr[i]<FLASH_LOG_HEAD_BASE_DATA
                ||g_LogHead.log_date_end_addr[i]+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END
                ||g_LogHead.log_date_end_addr[i]<FLASH_LOG_HEAD_BASE_DATA)
                {
                    return ;
                }
				//��ʼ��ַС�ڽ�����ַ
				if(g_LogHead.log_date_start_addr[i]<g_LogHead.log_date_end_addr[i])
				{
					//����־���ݣ���ʼ��ַ��������ַ
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
				else//��ʼ��ַ���ڽ�����ַ 
				{
				    //����־���ݣ���ʼ��ַ��ĩβ
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
					//����־���ݣ�����ַ��������ַ
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
description :   д��־1��
inptu       :   ��
return      :   ��  
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
		//���½���������־
		if(0==g_LogHead.log_date_size)     
		{
			//��һ��д   
		}  
		else//���ǵ�һ��д
		{
		    //���½�������
			g_LogHead.end_log_date_index = (g_LogHead.end_log_date_index+1)%LOG_DATE_LENGTH;
			//��ʼ�����ͽ��������Ƿ��ص�
			if(g_LogHead.start_log_date_index==g_LogHead.end_log_date_index)
			{   
			    //������ʼ����
				g_LogHead.start_log_date_index = (g_LogHead.start_log_date_index +1)%LOG_DATE_LENGTH; 
				//�����ļ���
				g_LogHead.log_date_size--;        
			}
		}   
		//���½����ļ���
		my_memcpy((uint8 *)&g_LogHead.log_date_name[g_LogHead.end_log_date_index],(uint8 *)&g_temp_log.log_time,LOG_NAME_SIZE);
		//���½����ļ���ʼ��ַ
		g_LogHead.log_date_start_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
		//���½����ļ�������ַ
		g_LogHead.log_date_end_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
		g_LogHead.log_date_size++;  
		ptu_print_buf((uint8 *)&g_temp_log.log_time,sizeof(g_temp_log));
	} 
	//д��־  
	if(1==g_log_debug)
	{
	    //��ӡд���¼
	    print_int("%x write g_LogHead.log_date_start_addr",g_LogHead.log_date_start_addr[g_LogHead.end_log_date_index]); 
	    print_int("%x write g_LogHead.current_log_addr",g_LogHead.current_log_addr);  
	    ptu_print_buf((uint8 *)&g_LogHead,5);
	    //ptu_print_buf((uint8 * )&g_temp_log,sizeof(g_temp_log));
	}
	log_write_flash(g_LogHead.current_log_addr,(uint8 *)&g_temp_log,sizeof(g_temp_log));

	//д����־��������־ͷ
		  
	g_LogHead.current_log_addr += sizeof(g_temp_log);
	if(g_LogHead.current_log_addr+sizeof(g_temp_log)>FLASH_LOG_HEAD_BASE_DATA_END)
	{
		//��鵱ǰ��ַ�Ϸ���
		g_LogHead.current_log_addr = g_LogHead.log_base_addr;   
	}        
	if((g_LogHead.current_log_addr<=g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index])&&(g_LogHead.current_log_addr+sizeof(g_temp_log)> g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index]))
	{
		//��鵱ǰ��ַ�Ƿ񳬹������ڴ棬������ʼ��������  
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
				//��鵱ǰ��ַ�Ϸ���    
				g_LogHead.log_date_start_addr[g_LogHead.start_log_date_index] = g_LogHead.log_base_addr;   
			}
			g_LogHead.log_date_end_addr[g_LogHead.start_log_date_index] = g_LogHead.current_log_addr;  
		}
	}
		//���½����ļ�������ַ
	g_LogHead.log_date_end_addr[g_LogHead.end_log_date_index] = g_LogHead.current_log_addr;
    return 0;
	
}

/***************************************************************************
function    :   log_head_check
description :   �����־ͷ�Ϸ���
inptu       :   ��
return      :   0-->�Ϸ�
                ��0-->���Ϸ�  
****************************************************************************/
uint8 log_head_check(void)  
{
	#if 1   
	if((g_LogHead.log_date_size>LOG_DATE_LENGTH))     
	{
		return 1;
	}
	//�����־ͷ��־
	if(g_LogHead.log_head_flag != LOG_HEAD_FLAG)
	{
		return 1;
	}
	//�����־����ַ
	if(g_LogHead.log_base_addr != (FLASH_LOG_HEAD_BASE_DATA) )   
	{
        return 1;
	}
	//�����ʼ����
	if(g_LogHead.start_log_date_index >= LOG_DATE_LENGTH)
	{
		return 1;
	}
	//����������   
	if(g_LogHead.end_log_date_index >= LOG_DATE_LENGTH)
	{
		return 1;  
	}
	//��鵱ǰ��ַ
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
description :   ����־ͷ
inptu       :   ��
return      :   �� 
****************************************************************************/
void read_log_head(void)
{   
	my_memset((uint8 *)&g_LogHead,0,sizeof(g_LogHead));
	log_read_flash(FLASH_LOG_HEAD_BASE_HEAD,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
}

/***************************************************************************
function    :   read_log_head_default
description :   ��������־ͷ
inptu       :   ��
return      :   �� 
****************************************************************************/
void read_log_head_default(void)     
{
	log_read_flash(FLASH_LOG_HEAD_BASE_HEAD_DEFAULT,(uint8 *)&g_LogHead, sizeof(g_LogHead));   
}

/***************************************************************************
function    :   write_log_head_default
description :   д������־ͷ
inptu       :   ��
return      :   �� 
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
description :   д��־ͷ
inptu       :   ��
return      :   �� 
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
description :   д��־2��
inptu       :   ��
return      :   �� 
****************************************************************************/
void write_log(void)
{    
	uint8 temp[LOG_NAME_SIZE];   
	//д��־         
	//�ж�g_temp_log�Ƿ�Ϊ��     
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
	     
	//д��־ͷ    
	write_log_head();
	//д��־ͷ����   
	//write_log_head_default();
	mdelay(30);
	print_line("write_log_data OK");  
}
void read_log(void)
{//ûд��
print_line("�Ѿ�ȡ��");   
} 

/***************************************************************************
function    :   log_head_init_set
description :   ��ʼ����־ͷ1��
inptu       :   ��
return      :   �� 
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
description :   ��ʼ����־ͷ2��
inptu       :   ��
return      :   �� 
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
description :   ������־ͷ
inptu       :   ��
return      :   ��    
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
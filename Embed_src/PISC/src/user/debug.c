//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&����ģ��&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:debug.c

//*�ļ�����:����ģ��

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//�����б�
static uint8 debug_argc;//��������
static uint8 * debug_argv[DEBUG_ARGV_MAXNUM];//�����б�

//�����б����ݽṹ
__packed typedef struct
{
	char *cmdstr;
	void  (*callback)(void);
}debug_commandlist_t;



//***************************************************************************************
//��������:�����б�
//����˵��:��
//ע������:
//����˵��:���������0
//***************************************************************************************
//�汾�Ų�ѯ
static void version(void)
{
	//��ѯ
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{
		print_line(VENDER_NAME);
		print_line(SYSTEM_NAME);
		print_line(PRODECT_NAME);
		print_line(VERSION_DATE);
		print_line(PRODECT_CODE);
		print_line("VERSOIN:");
		print_int("%d",HARDWARE_VERSION);
		print_int("%d",ADD_FUNCTION_VERSION);
		print_int("%d",MAJOR_VERSION);
		print_int("%d",MINOR_VERSION);
	}
}
//��λ---���ÿ��Ź�
static void reset(void)
{
	print_line("OK");//����ɹ�
	while(1);
}
//�ָ�ȱʡ����
static void loaddefault(void)
{
	if(debug_argc!=1)
	{
		print_line("ERROR");
		return;
	}//��������
	save_load_default();//װ��
	save_set_task();//����
	print_line("OK");//����ɹ�
}
//���س���
static uint8 wiat_char(uint8 ch)
{
	uint8 temp;
	uint32 timeout=0;
	
	while(!DEBUG_RECEIVE_CHAR(&temp))
	{
		dog();
		if(++timeout>=3000000)
		{
			print_line("TIMEOUT");
			return 0;	
		}
	}
	if(temp!=ch)
		return 0;
	return 1;
}
static uint8 receive(uint8 *ch)
{
	uint32 timeout=0;
	
	while(!DEBUG_RECEIVE_CHAR(ch))
	{
		dog();
		if(++timeout>=3000000)
		{
			print_line("TIMEOUT");
			return 0;	
		}
	}
	return 1;
}
static void download(void)
{
	uint32 total_length;
	uint32 recv_num=0;
	uint8 temp;
	uint16 i;
	uint16 checksum_add0;//����PC���ۼ�У���
	uint16 checksum_xor0;//����PC�����У���
	uint16 checksum_add1=0;//�Լ�������ۼ�У���
	uint16 checksum_xor1=0;//�Լ���������У���
	      	
	if(debug_argc!=1)
	{
		print_line("ERROR");
		return;
	}//��������
	DEBUG_RECEIVE_CHAR(&temp);//ɨ������
	DEBUG_RECEIVE_CHAR(&temp);//ɨ������
	DEBUG_RECEIVE_CHAR(&temp);//ɨ������
	print_line("OK");//����ɹ�

	// 1.������ʼ��
	if(!wiat_char('A'))
		return;//�ȴ���ʼ��1
	if(!wiat_char('R'))
		return;//�ȴ���ʼ��1
	if(!wiat_char('M'))
		return;//�ȴ���ʼ��1
	print_line("VER "PRODECT_CODE);//���Ͱ汾��---��ΪӦ��
	if(!wiat_char(0x07))
		return;//�ȴ�CPU�ͺŴ���
	if(!wiat_char(0x10))
		return;//�ȴ�CPU�ͺŴ���
	
	// 2.����ͬ����
	if(!wiat_char(0x55))
		return;
	if(!wiat_char(0xAA))
		return;
	
	// 3.���������ܳ���
	if(!receive(&temp))
		return;
	total_length=(uint32)temp;
	if(!receive(&temp))
		return;
	total_length|=(uint32)temp<<8;
	if(!receive(&temp))
		return;
	total_length|=(uint32)temp<<16;
	if(!receive(&temp))
		return;
	total_length|=(uint32)temp<<24;
	if(total_length>FLASH_BACKUP_LENGTH)
	{
		print_line("LENGTH ERROR");
		return;
	}
	//��������flash����
	if(!flash_erase(FLASH_BACKUP_BASE,total_length))
	{
		print_line("ERASE ERROR");
		return;
	}
	print_line("ERASE OK");

	
	// 4.ѭ���������ݰ� һ�߽���һ�߼���У���
	while(recv_num<total_length)
	{
		uint16 count;
		uint8 encrypt;
                		
		//�ж��Ƿ�ʣ�²���һ��
		if(total_length-recv_num>=PTU_BUF_SIZE)
			count=PTU_BUF_SIZE;
		else   
			count=total_length-recv_num;
                //�������ݵ�����
		for(i=0;i<count;i++)
		{
			if(!receive(&temp))
				return;
			ptu_recv_buf[i]=temp;
		}
		//���ݽ���
		encrypt=ptu_recv_buf[0]^0x55;
		for(i=1;i<count;i++)ptu_recv_buf[i]^=encrypt;
		//�ѽ��ջ��������д��flash
		if(!flash_write(ptu_recv_buf,FLASH_BACKUP_BASE+recv_num,count))
		{
			print_line("WRITE ERROR");//д��ʧ�� ����Ӧ��
			return;
		}
		print_line("WRITE OK");//һ����д����� ����Ӧ��

		//����У���
		for(i=0;i<count;i++)
			checksum_add1+=*((uint8 *)(FLASH_BACKUP_BASE+recv_num+i));//�����ۼ�У���
		for(i=0;i<count;i+=2)
			checksum_xor1^=*((uint16 *)(FLASH_BACKUP_BASE+recv_num+i));//�������У���
		//ָ����һ������
		recv_num+=count;
	}
	
	//5.��������PC��У���
	if(!receive(&temp))
		return;
	checksum_add0=(uint32)temp;
	if(!receive(&temp))
		return;
	checksum_add0|=(uint32)temp<<8;
	if(!receive(&temp))
		return;
	checksum_xor0=(uint32)temp;
	if(!receive(&temp))
		return;
	checksum_xor0|=(uint32)temp<<8;
			
	//6.�ж�У���
	if((checksum_add0!=checksum_add1) || (checksum_xor0!=checksum_xor1))
	{
		print_line("VERIFY ERROR2");
		return;
	}
	print_line("VERIFY OK");
	for(recv_num=0;recv_num<300000;recv_num++);//�ȴ��������
	
	//7.������������
	flash_erase(PARAM_ADDR,sizeof(saveparam_t));
	
	//8.���г������---�ӱ���flash����������flash ����������ÿ��Ź���λ
	flash_copy(total_length);
	
}
//�Զ����������
extern debug_commandlist_t const debug_commandlist[];
static void help(void)
{
	uint16 i=0;
	
	while(debug_commandlist[i].cmdstr)
	{
		print_line(debug_commandlist[i].cmdstr);
		i++;
	}
}
//���õ��Ա�־
static void debug(void)
{
	if(debug_argc!=3)
	{
		print_line("ERROR");
		return;
	}
	
	if(strcmp((char *)debug_argv[1],"PRINT")==0)
	{
		sh9_ptu_dev.print_flag=debug_argv[2][0]-'0';
	}
	else if(strcmp((char *)debug_argv[1],"TMS")==0)
	{
		tms_dev->debug_flag=debug_argv[2][0]-'0';		
	}		
	else if(strcmp((char *)debug_argv[1],"TBA")==0)
	{
		tba_bus_dev->set_debug(debug_argv[2][0]-'0');
	}
	else if(strcmp((char *)debug_argv[1],"DVA")==0)
	{
		cab_bus_dev->set_debug(debug_argv[2][0]-'0');
	}
	else if(strcmp((char *)debug_argv[1],"RECORD")==0)
	{
		set_error_record(debug_argv[2][0]-'0');
	}
	else
	{
		print_line("ERROR");
		return;
	}
	print_line("OK");//��ȷӦ��
}
//͸������
static void trans(void)
{
	uint8 uart_no;
	uint8 temp;
	uint8 num=0;
	
	if(debug_argc!=2)
	{
		print_line("ERROR");
		return;
	}//��������
	
	//�жϴ���ͨ��
	if(strcmp((char *)debug_argv[1],"MEDIA")==0)
	{
		uart_no=0;
	}
	else if(strcmp((char *)debug_argv[1],"TMS")==0)
	{
		uart_no=1;
	}
	else if(strcmp((char *)debug_argv[1],"TBA")==0)
	{
		uart_no=2;
	}
	else if(strcmp((char *)debug_argv[1],"DVA")==0)
	{
		uart_no=3;
	}
	else
	{
		print_line("ERROR");
		return;
	}
	print_line("OK");//��ȷӦ��
	
	//͸����ѭ��
	while(1)
	{
		dog();
		softdog();
		
		if(uart_no==0)
		{
			if(DEBUG_RECEIVE_CHAR(&temp))
			{
				MEDIA_SEND_CHAR(temp);
				if(temp=='&')//�յ�'&' num����
				{
					num++;
					if(num==3)
						break;
				}
				else	//�յ���'&' num��0
				{
					num=0;
				}
			}
			if(MEDIA_RECEIVE_CHAR(&temp))
			{
				DEBUG_SEND_CHAR(temp);
			}
		}
		else if(uart_no==3)
		{
			if(DEBUG_RECEIVE_CHAR(&temp))
			{
				TMS_SEND_CHAR(temp);
				if(temp=='&')//�յ�'&' num����
				{
					num++;
					if(num==3)
						break;
				}
				else	//�յ���'&' num��0
				{
					num=0;
				}
			}
			if(TMS_RECEIVE_CHAR(&temp))
			{
				DEBUG_SEND_CHAR(temp);
			}
		}
		else if(uart_no==1)
		{
			if(DEBUG_RECEIVE_CHAR(&temp))
			{
				TBA_BUS_SEND_CHAR(temp);
				if(temp=='&')//�յ�'&' num����
				{
					num++;
					if(num==3)
						break;
				}
				else	//�յ���'&' num��0
				{
					num=0;
				}
			}
			if(TBA_BUS_RECEIVE_CHAR(&temp))
			{
				DEBUG_SEND_CHAR(temp);
			}
		}
		else if(uart_no==2)
		{
			if(DEBUG_RECEIVE_CHAR(&temp))
			{
				CAB_BUS_SEND_CHAR(temp);
				if(temp=='&')//�յ�'&' num����
				{
					num++;
					if(num==3)
						break;
				}
				else	//�յ���'&' num��0
				{
					num=0;
				}
			}
			if(CAB_BUS_RECV_CHAR(&temp))
			{
				DEBUG_SEND_CHAR(temp);
			}
		}
	}
		
	//��ȷӦ��
	print_line("OK");	
}

//���ò�ѯѲ���
static void mtable(void)
{	
	uint8 index=0;
	uint8 dev_id=0;
	
	
	//��ѯ
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{
		uint8 hex[6];
		
		while(saveparam.token_table_dev_id[index])
		{
			int_to_hex(hex,saveparam.token_table_dev_id[index]);
			DEBUG_SEND_STR((char *)hex);
			DEBUG_SEND_STR(": ");
			int_to_dec(hex,saveparam.token_table_interval[index]);
			DEBUG_SEND_STR((char *)hex);
			DEBUG_SEND_STR("\r\n");
			index++;
		}
		print_line("OK");
	}
	//����
	else if(debug_argc==3)
	{
		hex_to_int8(debug_argv[1],&dev_id);
		
		//������Ӧ���豸
		while(saveparam.token_table_interval[index])
		{
			if(saveparam.token_table_dev_id[index]==dev_id)
			{
				hex_to_int8(debug_argv[1],&saveparam.token_table_dev_id[index]);
				dec_to_int16(debug_argv[2],&saveparam.token_table_interval[index]);
				
				//�ж��Ƿ�Ҫɾ��
				if(saveparam.token_table_interval[index]==0)
				{
					while(saveparam.token_table_interval[index+1])
					{
						saveparam.token_table_dev_id[index]=saveparam.token_table_dev_id[index+1];
						saveparam.token_table_interval[index]=saveparam.token_table_interval[index+1];
						index++;
						if(index>=TBA_M_TABLE_NUM-1)
							break;
					}
					saveparam.token_table_dev_id[index]=0;
					saveparam.token_table_interval[index]=0;
				}
				break;
			}
			index++;
			if(index>=TBA_M_TABLE_NUM-1)
				break;
		}
		//���û���ҵ��豸˵������ӵ����豸 ��ӵ�ĩβ ����ӽ�����
		if((saveparam.token_table_interval[index]==0) && (index<TBA_M_TABLE_NUM-1))
		{
			hex_to_int8(debug_argv[1],&saveparam.token_table_dev_id[index]);
			dec_to_int16(debug_argv[2],&saveparam.token_table_interval[index]);
			if(saveparam.token_table_interval[index])index++;
			//��ӽ�����
			saveparam.token_table_dev_id[index]=0;
			saveparam.token_table_interval[index]=0;
		}
		//���õ�M
		tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
	
		//�������
		save_set_task();
		//��ȷ����
		print_line("OK");
	}
}
//��ѯͨ������
static void mquality(void)
{
	uint8 dev_num=0;
	uint8 dev_list[TBA_M_TABLE_NUM];
	uint16 total_list[TBA_M_TABLE_NUM];
	uint16 error_list[TBA_M_TABLE_NUM];
	
	//��ѯ
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{
		uint8 index;
		uint8 hex[6];

		dev_num=tba_m_get_quality(dev_list,total_list,error_list);
		for(index=0;index<dev_num;index++)
		{
			int_to_hex(hex,dev_list[index]);
			DEBUG_SEND_STR((char *)hex);
			DEBUG_SEND_STR(": ");
			int_to_dec(hex,error_list[index]);
			DEBUG_SEND_STR((char *)hex);
			DEBUG_SEND_STR("/");
			int_to_dec(hex,total_list[index]);
			DEBUG_SEND_STR((char *)hex);
			DEBUG_SEND_STR("\r\n");
		}
		print_line("OK");
	}
}
//apu����
static void apu(void)
{
	uint8 volume;

	//��ѯ
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{		
		print_int("%xOCC:",saveparam.occ_volume);
		print_int("%xDVA:",saveparam.dva_volume);
		print_int("%xMEDIA:",saveparam.media_volume);
		print_int("%xBASS:",saveparam.apu_bass);
		print_int("%xTREBLE:",saveparam.apu_treble);
		return;
	}
		
	//��������
	if(debug_argc!=3)
	{
		print_line("ERROR");
		return;
	}
	
	//����1
	if(strcmp((char *)debug_argv[1],"OCC")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.occ_volume=volume;
	}
	//����2
	else if(strcmp((char *)debug_argv[1],"DVA")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.dva_volume=volume;
	}
	//����3
	else if(strcmp((char *)debug_argv[1],"MEDIA")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.media_volume=volume;
	}

	//door volume
	else if(strcmp((char *)debug_argv[1],"DOOR")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.door_volume=volume;
	}
	
	//�ص���
	else if(strcmp((char *)debug_argv[1],"BASS")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.apu_bass=volume;
	}
	//����
	else if(strcmp((char *)debug_argv[1],"TREBLE")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.apu_treble=volume;
	}			
	//ͨ��
	else if(strcmp((char *)debug_argv[1],"CHANNEL")==0)
	{
		apu_dev->set_channel(debug_argv[2][0]-'0');
		print_line("OK");
		return;
	}
	//PA
	else if(strcmp((char *)debug_argv[1],"PA")==0)
	{
		apu_dev->set_pa_out(debug_argv[2][0]-'0');
		print_line("OK");
		return;
	}
	else
	{
		print_line("ERROR");
		return;
	}
	//�������
	save_set_task();
	//��ȷ����
	print_line("OK");
}

//���ò�ѯվ����
static void stationcode(void)
{	
	//uint8 index=0;
	
	
	//��ѯ
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{
		//uint8 dec[6];
	#if 0
		while(saveparam.stations_code_table[index])
		{
			int_to_dec(dec,saveparam.stations_code_table[index]);
			DEBUG_SEND_STR((char *)dec);
			DEBUG_SEND_STR(" ");
			index++;
		}
		print_line("OK");
	#endif
	}
	
}



//�����б�ṹ
debug_commandlist_t const debug_commandlist[]=
{
	{"VER",version},
	{"RESET",reset},	
	{"LOADDFT",loaddefault},
	{"DOWNLOAD",download},
	{"HELP",help},
	{"DEBUG",debug},
	{"TRANS",trans},
	
	{"MTABLE",mtable},
	{"MQUALITY",mquality},
	{"APU",apu},
	{"STATIONCODE",stationcode},
		
	{NULL,NULL},
};



//***********************************************************************************************************************
//��������:���������в���
//����˵��:��
//ע������:
//����˵��:���������0
//***********************************************************************************************************************
static void debug_args_param(uint8 *cmd_line)
{	
	debug_argc=0;//���������
		
	//��������ͷ �õ���������ָ��
	if(DEBUG_COMMAND_HEAD)
		debug_argv[0] = cmd_line+1;
	else
		debug_argv[0] = cmd_line;
	if( *debug_argv[0] == 0 )
		return;//������������Ϊ��
	debug_argc++;
	
	//��������
	while( *cmd_line )
	{
		//�ҵ��˲����ָ���
		if(*cmd_line == DEBUG_COMMAND_TAB)
		{
			*cmd_line = 0;
			debug_argv[debug_argc] = cmd_line+1;
			if(*debug_argv[debug_argc]==0)
			{
				debug_argc = 0;
				return;
			}//���ǲ���Ϊ��		
			debug_argc++;
			if(debug_argc >= DEBUG_ARGV_MAXNUM)
			{
				debug_argc = 0;
				return;
			}//����̫��
		}
		cmd_line++;
	}
}

//***********************************************************************************************************************
//��������:�����������
//����˵��:��
//ע������:
//����˵��:
//***********************************************************************************************************************
void debug_proc(uint8 *cmd_line)
{	
	debug_commandlist_t const *list;
	
	
	//�жϿ�ָ��ͳ���
	if(!cmd_line || !cmd_line[0])
		return;
	
	//�ж�����ͷ
	if(DEBUG_COMMAND_HEAD)
		if(cmd_line[0] != DEBUG_COMMAND_HEAD)
			return;
	
	//���������в���
	debug_args_param(cmd_line);
	if( !debug_argc )
		return;
	
	//���ݰ��б��в��Ҳ��ҵ�����Ӧ������
	for(list=debug_commandlist; list->cmdstr != NULL; list++)
	{
		//�ҵ����Ժ� ������Ӧ����
		if(strcmp((char *)debug_argv[0],list->cmdstr)==0)
		{
			list->callback();
			return;
		}
	}		
}



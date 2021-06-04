//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&调试模块&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:debug.c

//*文件作用:调试模块

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//参数列表
static uint8 debug_argc;//参数个数
static uint8 * debug_argv[DEBUG_ARGV_MAXNUM];//参数列表

//命令列表数据结构
__packed typedef struct
{
	char *cmdstr;
	void  (*callback)(void);
}debug_commandlist_t;



//***************************************************************************************
//函数作用:命令列表
//参数说明:无
//注意事项:
//返回说明:如果出错返回0
//***************************************************************************************
//版本号查询
static void version(void)
{
	//查询
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
//复位---利用看门狗
static void reset(void)
{
	print_line("OK");//命令成功
	while(1);
}
//恢复缺省参数
static void loaddefault(void)
{
	if(debug_argc!=1)
	{
		print_line("ERROR");
		return;
	}//参数错误
	save_load_default();//装载
	save_set_task();//保存
	print_line("OK");//命令成功
}
//下载程序
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
	uint16 checksum_add0;//来自PC的累加校验和
	uint16 checksum_xor0;//来自PC的异或校验和
	uint16 checksum_add1=0;//自己计算的累加校验和
	uint16 checksum_xor1=0;//自己计算的异或校验和
	      	
	if(debug_argc!=1)
	{
		print_line("ERROR");
		return;
	}//参数错误
	DEBUG_RECEIVE_CHAR(&temp);//扫除垃圾
	DEBUG_RECEIVE_CHAR(&temp);//扫除垃圾
	DEBUG_RECEIVE_CHAR(&temp);//扫除垃圾
	print_line("OK");//命令成功

	// 1.接收起始符
	if(!wiat_char('A'))
		return;//等待起始符1
	if(!wiat_char('R'))
		return;//等待起始符1
	if(!wiat_char('M'))
		return;//等待起始符1
	print_line("VER "PRODECT_CODE);//发送版本号---作为应答
	if(!wiat_char(0x07))
		return;//等待CPU型号代码
	if(!wiat_char(0x10))
		return;//等待CPU型号代码
	
	// 2.接收同步码
	if(!wiat_char(0x55))
		return;
	if(!wiat_char(0xAA))
		return;
	
	// 3.接收数据总长度
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
	//擦除备份flash扇区
	if(!flash_erase(FLASH_BACKUP_BASE,total_length))
	{
		print_line("ERASE ERROR");
		return;
	}
	print_line("ERASE OK");

	
	// 4.循环接收数据包 一边接收一边计算校验和
	while(recv_num<total_length)
	{
		uint16 count;
		uint8 encrypt;
                		
		//判断是否剩下不到一包
		if(total_length-recv_num>=PTU_BUF_SIZE)
			count=PTU_BUF_SIZE;
		else   
			count=total_length-recv_num;
                //接收数据到缓存
		for(i=0;i<count;i++)
		{
			if(!receive(&temp))
				return;
			ptu_recv_buf[i]=temp;
		}
		//数据解密
		encrypt=ptu_recv_buf[0]^0x55;
		for(i=1;i<count;i++)ptu_recv_buf[i]^=encrypt;
		//把接收缓存的数据写入flash
		if(!flash_write(ptu_recv_buf,FLASH_BACKUP_BASE+recv_num,count))
		{
			print_line("WRITE ERROR");//写入失败 错误应答
			return;
		}
		print_line("WRITE OK");//一个包写入完毕 发送应答

		//计算校验和
		for(i=0;i<count;i++)
			checksum_add1+=*((uint8 *)(FLASH_BACKUP_BASE+recv_num+i));//计算累加校验和
		for(i=0;i<count;i+=2)
			checksum_xor1^=*((uint16 *)(FLASH_BACKUP_BASE+recv_num+i));//计算异或校验和
		//指向下一包数据
		recv_num+=count;
	}
	
	//5.接收来自PC的校验和
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
			
	//6.判断校验和
	if((checksum_add0!=checksum_add1) || (checksum_xor0!=checksum_xor1))
	{
		print_line("VERIFY ERROR2");
		return;
	}
	print_line("VERIFY OK");
	for(recv_num=0;recv_num<300000;recv_num++);//等待发送完毕
	
	//7.擦除参数区域
	flash_erase(PARAM_ADDR,sizeof(saveparam_t));
	
	//8.进行程序搬移---从备份flash拷贝到运行flash 拷贝完毕利用看门狗复位
	flash_copy(total_length);
	
}
//自定义命令帮助
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
//设置调试标志
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
	print_line("OK");//正确应答
}
//透明传输
static void trans(void)
{
	uint8 uart_no;
	uint8 temp;
	uint8 num=0;
	
	if(debug_argc!=2)
	{
		print_line("ERROR");
		return;
	}//参数错误
	
	//判断传输通道
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
	print_line("OK");//正确应答
	
	//透传死循环
	while(1)
	{
		dog();
		softdog();
		
		if(uart_no==0)
		{
			if(DEBUG_RECEIVE_CHAR(&temp))
			{
				MEDIA_SEND_CHAR(temp);
				if(temp=='&')//收到'&' num增加
				{
					num++;
					if(num==3)
						break;
				}
				else	//收到非'&' num清0
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
				if(temp=='&')//收到'&' num增加
				{
					num++;
					if(num==3)
						break;
				}
				else	//收到非'&' num清0
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
				if(temp=='&')//收到'&' num增加
				{
					num++;
					if(num==3)
						break;
				}
				else	//收到非'&' num清0
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
				if(temp=='&')//收到'&' num增加
				{
					num++;
					if(num==3)
						break;
				}
				else	//收到非'&' num清0
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
		
	//正确应答
	print_line("OK");	
}

//设置查询巡检表
static void mtable(void)
{	
	uint8 index=0;
	uint8 dev_id=0;
	
	
	//查询
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
	//设置
	else if(debug_argc==3)
	{
		hex_to_int8(debug_argv[1],&dev_id);
		
		//查找相应的设备
		while(saveparam.token_table_interval[index])
		{
			if(saveparam.token_table_dev_id[index]==dev_id)
			{
				hex_to_int8(debug_argv[1],&saveparam.token_table_dev_id[index]);
				dec_to_int16(debug_argv[2],&saveparam.token_table_interval[index]);
				
				//判断是否要删除
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
		//如果没有找到设备说明是添加的新设备 添加到末尾 并添加结束符
		if((saveparam.token_table_interval[index]==0) && (index<TBA_M_TABLE_NUM-1))
		{
			hex_to_int8(debug_argv[1],&saveparam.token_table_dev_id[index]);
			dec_to_int16(debug_argv[2],&saveparam.token_table_interval[index]);
			if(saveparam.token_table_interval[index])index++;
			//添加结束符
			saveparam.token_table_dev_id[index]=0;
			saveparam.token_table_interval[index]=0;
		}
		//设置到M
		tba_m_set_table(saveparam.token_table_dev_id,saveparam.token_table_interval);
	
		//保存参数
		save_set_task();
		//正确返回
		print_line("OK");
	}
}
//查询通信质量
static void mquality(void)
{
	uint8 dev_num=0;
	uint8 dev_list[TBA_M_TABLE_NUM];
	uint16 total_list[TBA_M_TABLE_NUM];
	uint16 error_list[TBA_M_TABLE_NUM];
	
	//查询
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
//apu设置
static void apu(void)
{
	uint8 volume;

	//查询
	if(debug_argc==2 && debug_argv[1][0]=='?')
	{		
		print_int("%xOCC:",saveparam.occ_volume);
		print_int("%xDVA:",saveparam.dva_volume);
		print_int("%xMEDIA:",saveparam.media_volume);
		print_int("%xBASS:",saveparam.apu_bass);
		print_int("%xTREBLE:",saveparam.apu_treble);
		return;
	}
		
	//参数错误
	if(debug_argc!=3)
	{
		print_line("ERROR");
		return;
	}
	
	//音量1
	if(strcmp((char *)debug_argv[1],"OCC")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.occ_volume=volume;
	}
	//音量2
	else if(strcmp((char *)debug_argv[1],"DVA")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.dva_volume=volume;
	}
	//音量3
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
	
	//重低音
	else if(strcmp((char *)debug_argv[1],"BASS")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.apu_bass=volume;
	}
	//高音
	else if(strcmp((char *)debug_argv[1],"TREBLE")==0)
	{
		hex_to_int8(debug_argv[2],&volume);
		saveparam.apu_treble=volume;
	}			
	//通道
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
	//保存参数
	save_set_task();
	//正确返回
	print_line("OK");
}

//设置查询站代码
static void stationcode(void)
{	
	//uint8 index=0;
	
	
	//查询
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



//命令列表结构
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
//函数作用:分析命令行参数
//参数说明:无
//注意事项:
//返回说明:如果出错返回0
//***********************************************************************************************************************
static void debug_args_param(uint8 *cmd_line)
{	
	debug_argc=0;//清参数个数
		
	//跳过命名头 得到命令类型指针
	if(DEBUG_COMMAND_HEAD)
		debug_argv[0] = cmd_line+1;
	else
		debug_argv[0] = cmd_line;
	if( *debug_argv[0] == 0 )
		return;//但是命令类型为空
	debug_argc++;
	
	//分析参数
	while( *cmd_line )
	{
		//找到了参数分隔符
		if(*cmd_line == DEBUG_COMMAND_TAB)
		{
			*cmd_line = 0;
			debug_argv[debug_argc] = cmd_line+1;
			if(*debug_argv[debug_argc]==0)
			{
				debug_argc = 0;
				return;
			}//但是参数为空		
			debug_argc++;
			if(debug_argc >= DEBUG_ARGV_MAXNUM)
			{
				debug_argc = 0;
				return;
			}//参数太多
		}
		cmd_line++;
	}
}

//***********************************************************************************************************************
//函数作用:处理调试命令
//参数说明:无
//注意事项:
//返回说明:
//***********************************************************************************************************************
void debug_proc(uint8 *cmd_line)
{	
	debug_commandlist_t const *list;
	
	
	//判断空指针和长度
	if(!cmd_line || !cmd_line[0])
		return;
	
	//判断命令头
	if(DEBUG_COMMAND_HEAD)
		if(cmd_line[0] != DEBUG_COMMAND_HEAD)
			return;
	
	//分析命令行参数
	debug_args_param(cmd_line);
	if( !debug_argc )
		return;
	
	//数据包列表中查找并且调用相应处理函数
	for(list=debug_commandlist; list->cmdstr != NULL; list++)
	{
		//找到了以后 调用相应函数
		if(strcmp((char *)debug_argv[0],list->cmdstr)==0)
		{
			list->callback();
			return;
		}
	}		
}



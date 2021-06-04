//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&内部flash编程&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:flash.c

//*文件作用:内部flash编程

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//扇区表
static uint32 sector_table[]=
{
0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,
0x00010000,0x00020000,
};

//为了在ram中运行flash擦写程序 开辟一块空间
static uint8 run_in_ram_buf[2048];
static uint32 run_addr=0;



//***********************************************************************************************************************
//函数作用:擦除flash
//参数说明:start--开始地址  length--长度
//注意事项:这个函数中不能调用任何其他函数和常量 否则在ram中运行就会跑飞
//返回说明:无
//***********************************************************************************************************************
static uint8 flash_erase_(uint32 start, uint32 length)
{
	uint32 timeout;
	uint32 start_sector,end_sector;
	uint32 Xsectors=0;

	//判断范围
	if(start<FLASH_BASE)return 0;
	//计算扇区地址
	start_sector=start-FLASH_BASE;
	if(start_sector<0x8000)start_sector=start_sector/0x2000;
	else if(start_sector>=0x8000 && start_sector<0x10000)start_sector=4+(start_sector-0x8000)/0x8000;
	else if(start_sector>=0x10000 && start_sector<0x40000)start_sector=5+(start_sector-0x10000)/0x10000;
	else if(start_sector>=0xC0000 && start_sector<0xC4000)start_sector=8+(start_sector-0xC0000)/0x2000;
	else return 0;
	end_sector=start-FLASH_BASE+length-1;
	if(end_sector<0x8000)end_sector=end_sector/0x2000;
	else if(end_sector>=0x8000 && end_sector<0x10000)end_sector=4+(end_sector-0x8000)/0x8000;
	else if(end_sector>=0x10000 && end_sector<0x40000)end_sector=5+(end_sector-0x10000)/0x10000;
	else if(end_sector>=0xC0000 && end_sector<0xC4000)end_sector=8+(end_sector-0xC0000)/0x2000;
	else return 0;
	//选择要操作的所有扇区
	if(end_sector<start_sector)return 0;
	while(start_sector<=end_sector)
	{
		Xsectors|=sector_table[start_sector];
		start_sector++;
	}
	//关中断
	EIC_ICR&=(~0x0001);//关总中断
	//关闭所有扇区的写保护
	FLASHR_CR0|=FLASH_SPR_Mask;//Set the Set protection Bit
	FLASHR_AR=0x4010DFB0;//Set the Register Address
	FLASHR_DR0=FLASHPR_NVWPAR|Xsectors;//Data to be programmed to the Protection Register
	FLASHR_CR0|=FLASH_WMS_Mask;//Set the WMS bit to Start the Sequence
	timeout=0;
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;WDG_KR=0x5AA5;
		if(++timeout>=9000000)
		{
			EIC_ICR|=0x0001;//开总中断
			return 0;
		}
	}
	//擦除所有扇区
	FLASHR_CR0|=FLASH_SER_Mask;//Set the Sector Erase flag 'SER' in the CRO reg
	FLASHR_CR1|=Xsectors;//Select in the CR1 register the Sectors to be erased
	FLASHR_CR0|=FLASH_WMS_Mask;//Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Erase Operation
	timeout=0;
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;WDG_KR=0x5AA5;
		if(++timeout>=9000000)
		{
			EIC_ICR|=0x0001;//开总中断
			return 0;
		}
	}
	//开中断
	EIC_ICR|=0x0001;//开总中断
	
	return 1;
}
uint8 flash_erase(uint32 start, uint32 length)
{	
	if(!run_addr)return 0;
	
	//把擦写函数拷贝到ram中的运行地址 因为自己不能擦写自己
	memcpy((uint8 *)run_in_ram_buf,(uint8 *)&flash_erase_,sizeof(run_in_ram_buf));	
	//运行拷贝到ram中的函数
	return ((uint8(*)(uint32 start0,uint32 length0))run_addr)(start,length);
}

//***********************************************************************************************************************
//函数作用:写入flash
//参数说明:start--开始地址  length--长度
//注意事项:这个函数中不能调用任何其他函数和常量 否则在ram中运行就会跑飞
//返回说明:无
//***********************************************************************************************************************
static uint8 flash_write_(uint8 *buf, uint32 start, uint32 length)
{
	uint32 timeout;
	uint32 start_sector,end_sector;
	uint32 Xsectors=0;

	//判断范围
	if( start<FLASH_BASE )
		return 0;
	//计算扇区地址
	start_sector = start-FLASH_BASE;
	if(start_sector<0x8000)
		start_sector = start_sector/0x2000;
	else if(start_sector>=0x8000 && start_sector<0x10000)
		start_sector = 4+(start_sector-0x8000)/0x8000;
	else if(start_sector>=0x10000 && start_sector<0x40000)
		start_sector = 5+(start_sector-0x10000)/0x10000;
	else if(start_sector>=0xC0000 && start_sector<0xC4000)
		start_sector = 8+(start_sector-0xC0000)/0x2000;
	else
		return 0;
	
	end_sector = start-FLASH_BASE+length-1;
	if(end_sector<0x8000)
		end_sector = end_sector/0x2000;
	else if(end_sector>=0x8000 && end_sector<0x10000)
		end_sector = 4+(end_sector-0x8000)/0x8000;
	else if(end_sector>=0x10000 && end_sector<0x40000)
		end_sector = 5+(end_sector-0x10000)/0x10000;
	else if(end_sector>=0xC0000 && end_sector<0xC4000)
		end_sector = 8+(end_sector-0xC0000)/0x2000;
	else
		return 0;
	
	//选择要操作的所有扇区
	if(end_sector<start_sector)
		return 0;
	while(start_sector<=end_sector)
	{
		Xsectors |= sector_table[start_sector];
		start_sector++;
	}
	//关中断
	EIC_ICR &= (~0x0001);//关总中断
	//关闭所有扇区的写保护
	FLASHR_CR0 |= FLASH_SPR_Mask;//Set the Set protection Bit
	FLASHR_AR = 0x4010DFB0;//Set the Register Address
	FLASHR_DR0 = FLASHPR_NVWPAR|Xsectors;//Data to be programmed to the Protection Register
	FLASHR_CR0 |= FLASH_WMS_Mask;//Set the WMS bit to Start the Sequence
	//等待操作成功
	timeout = 0;
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;
		WDG_KR=0x5AA5;
		if(++timeout>=9000000)
		{
			EIC_ICR|=0x0001;//开总中断
			return 0;
		}
	}
	//数据写入扇区
	if(length%4)
		length += 4;//修正不是4的倍数
	length>>=2;//以32位方式写入
	while(length--)
	{
		FLASHR_CR0 |= FLASH_WPG_Mask;//Set the Word Programming bit 'WPG' in the CR0 Reg  
		FLASHR_AR = start;//Load the destination address in AR
		FLASHR_DR0 = *(uint32 *)buf;//Load DATA to be programmed in DR0
		FLASHR_CR0 |= FLASH_WMS_Mask;//Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Write Operation
		buf += 4;
		start += 4;
		//等待操作成功
		timeout = 0;
		while(FLASHR_CR0&0x16)
		{
			WDG_KR=0xA55A;
			WDG_KR=0x5AA5;
			if(++timeout>=9000000)
			{
				EIC_ICR |= 0x0001;//开总中断
				return 0;
			}
		}
	}
	//打开所有扇区的写保护
	FLASHR_CR0 |= FLASH_SPR_Mask;//Set the Set protection Bit
	FLASHR_AR = 0x4010DFB0;//Set the Register Address
	FLASHR_DR0 = ~Xsectors;//Data to be programmed to the Protection Register
	FLASHR_CR0 |= FLASH_WMS_Mask;//Set the WMS bit to Start the Sequence
	//等待操作成功
	timeout = 0;
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;
		WDG_KR=0x5AA5;
		if(++timeout>=9000000)
		{
			EIC_ICR|=0x0001;//开总中断
			return 0;
		}
	}
	
	//开中断
	EIC_ICR |= 0x0001;//开总中断
	
	return 1;
}

uint8 flash_write(uint8 *buf, uint32 start, uint32 length)
{
	if(!run_addr)return 0;
	
	//把擦写函数拷贝到ram中的运行地址 因为自己不能擦写自己
	memcpy((uint8 *)run_in_ram_buf,(uint8 *)&flash_write_,sizeof(run_in_ram_buf));	
	//运行拷贝到ram中的函数
	return ((uint8(*)(uint8 *buf0, uint32 start0, uint32 length0))run_addr)(buf,start,length);
}

//***********************************************************************************************************************
//函数作用:flash拷贝---用于程序下载完毕 从备份flash更新到flash基地址
//参数说明:
//注意事项:这个函数中不能调用任何其他函数和常量 否则在ram中运行就会跑飞
//返回说明:无
//***********************************************************************************************************************
void flash_copy_(uint32 length)
{
	uint32 dst = FLASH_BASE;
	uint32 src = FLASH_BACKUP_BASE;
	uint32 start_sector,end_sector;
	uint32 Xsectors = 0;

	//计算扇区地址
	start_sector = dst-FLASH_BASE;
	if(start_sector<0x8000)
		start_sector = start_sector/0x2000;
	else if(start_sector>=0x8000 && start_sector<0x10000)
		start_sector = 4+(start_sector-0x8000)/0x8000;
	else if(start_sector>=0x10000 && start_sector<0x40000)
		start_sector = 5+(start_sector-0x10000)/0x10000;
	else if(start_sector>=0xC0000 && start_sector<0xC4000)
		start_sector = 8+(start_sector-0xC0000)/0x2000;
	else 
		return;
	
	end_sector = length-1;
	if(end_sector<0x8000)
		end_sector = end_sector/0x2000;
	else if(end_sector>=0x8000 && end_sector<0x10000)
		end_sector = 4+(end_sector-0x8000)/0x8000;
	else if(end_sector>=0x10000 && end_sector<0x40000)
		end_sector = 5+(end_sector-0x10000)/0x10000;
	else if(end_sector>=0xC0000 && end_sector<0xC4000)
		end_sector = 8+(end_sector-0xC0000)/0x2000;
	else
		return;
	
	//选择要操作的所有扇区
	if( end_sector<start_sector )
		return;
	while(start_sector<=end_sector)
	{
		Xsectors |= sector_table[start_sector];
		start_sector++;
	}
	//关中断
	EIC_ICR &= (~0x0001);//关总中断
	//关闭所有扇区的写保护
	FLASHR_CR0 |= FLASH_SPR_Mask;//Set the Set protection Bit
	FLASHR_AR  = 0x4010DFB0;//Set the Register Address
	FLASHR_DR0 = FLASHPR_NVWPAR|Xsectors;//Data to be programmed to the Protection Register
	FLASHR_CR0 |= FLASH_WMS_Mask;//Set the WMS bit to Start the Sequence
	while(FLASHR_CR0&0x16)
	{
		WDG_KR = 0xA55A;
		WDG_KR = 0x5AA5;
	}
	//擦除所有扇区
	FLASHR_CR0 |= FLASH_SER_Mask;//Set the Sector Erase flag 'SER' in the CRO reg
	FLASHR_CR1 |= Xsectors;//Select in the CR1 register the Sectors to be erased
	FLASHR_CR0 |= FLASH_WMS_Mask;//Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Erase Operation
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;
		WDG_KR=0x5AA5;
	}
	//数据写入扇区
	length>>=2;//以32位方式写入
	while(length--)
	{
		FLASHR_CR0 |= FLASH_WPG_Mask;//Set the Word Programming bit 'WPG' in the CR0 Reg  
		FLASHR_AR = dst;//Load the destination address in AR
		FLASHR_DR0 = *(uint32 *)src;//Load DATA to be programmed in DR0
		FLASHR_CR0 |= FLASH_WMS_Mask;//Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Write Operation
		dst += 4;
		src += 4;
		while(FLASHR_CR0&0x16)
		{
			WDG_KR=0xA55A;
			WDG_KR=0x5AA5;
		}
	}	
	//打开所有扇区的写保护
	FLASHR_CR0 |= FLASH_SPR_Mask;//Set the Set protection Bit
	FLASHR_AR = 0x4010DFB0;//Set the Register Address
	FLASHR_DR0 = ~Xsectors;//Data to be programmed to the Protection Register
	FLASHR_CR0 |= FLASH_WMS_Mask;//Set the WMS bit to Start the Sequence
	while(FLASHR_CR0&0x16)
	{
		WDG_KR=0xA55A;
		WDG_KR=0x5AA5;
	}
	//利用看门狗复位
	while(1);
}

void flash_copy(uint32 length)
{
	if(!run_addr)
		return;
	
	//把擦写函数拷贝到ram中的运行地址 因为自己不能擦写自己
	memcpy((uint8 *)run_in_ram_buf,(uint8 *)&flash_copy_,sizeof(run_in_ram_buf));	
	//运行拷贝到ram中的函数
	((void(*)(uint32 length0))run_addr)(length);
}

//***********************************************************************************************************************
//函数作用:flash初始化
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void flash_init(void)
{
	//Reset Flash Control Registers
	FLASHR_CR0=0x00000000;
	FLASHR_CR1=0x00000000;
	//Reset Flash Data Registers
	FLASHR_DR0=0xFFFFFFFF;
	FLASHR_DR1=0xFFFFFFFF;
	//Reset Flash Error Register
	FLASHR_ER=0x00000000;
	
	//指向要运行函数的ram地址
	run_addr=(uint32)&run_in_ram_buf;
}



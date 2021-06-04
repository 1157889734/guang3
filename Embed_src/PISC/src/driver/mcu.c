//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&处理器基本设置&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:mcu.c

//*文件作用:处理器基本设置

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



#define EMI_ENABLE       0x8000
#define EMI_DISABLE      0x0000
#define EMI_WAITSTATE(n) ((n) << 2)
#define EMI_SIZE_8       0x0000
#define EMI_SIZE_16      0x0001



//***********************************************************************************************************************
//函数作用:专门针对某种arm的初始化
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void mcu_init(void)
{
	//设置系统主频
	RCCU_CFR|=0x8000;//把晶振频率2分频
	RCCU_PLL1CR&=(~(1<<7));//关闭自由运行模式
	RCCU_PLL1CR|=(1<<6);//设置晶振频率范围---高于3MHz
	if(Fosc==16000000)//根据晶振不同设置不同的PLL分频和倍频
	{
		RCCU_PLL1CR&=(~(0x3<<4));RCCU_PLL1CR|=(0x1<<4);//把PLL输出12倍频
		RCCU_PLL1CR&=(~0x07);RCCU_PLL1CR|=0x1;//把PLL输出2分频	
	}
	else if(Fosc==8000000)//根据晶振不同设置不同的PLL分频和倍频
	{
		RCCU_PLL1CR&=(~(0x3<<4));RCCU_PLL1CR|=(0x1<<4);//把PLL输出12倍频
		RCCU_PLL1CR&=(~0x07);//把PLL输出没有分频
	}
	PCU_MDIVR&=(~0x3);//设置MCLK分频系数---不分频
	//设置片上外设的时钟
	PCU_PDIVR&=(~0x3);//设置APB1分频系数---不分频
	PCU_PDIVR&=(~(0x3<<8));//设置APB2分频系数---不分频
	RCCU_PER=0;//关闭所有片上外设的时钟
	RCCU_PER|=(1<<4);//使能片上外设的USB时钟
	RCCU_PER|=(1<<2);//使能片上外设的EMI时钟
	while(!(RCCU_CFR&(0x1<<1)));//等待锁相环锁相

	//选择PLL输出为系统时钟源
	RCCU_CFR|=0x00000008;//Set the CK2_16 Bit in the CFR
	if((RCCU_PLL1CR&0x0007)!=7)while(!(RCCU_CFR&(0x1<<1)));//等待锁相环锁相
	RCCU_CCR&=~0x00000004;// Deselect The CKAF
	RCCU_CFR|=0x00000001;//Select The CSU_CKSEL
	
	
	//配置外部总线片选
	//GPIO2_PC0|=Port_Pins;
	//GPIO2_PC1|=Port_Pins;
	//GPIO2_PC2|=Port_Pins;
	//配置外部总线BANK2
	//BCON2=EMI_ENABLE | EMI_WAITSTATE(15) | EMI_SIZE_16;
	//配置外部总线BANK1 to xian 2 line
	//BCON1=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON2=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON3=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON0=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
}



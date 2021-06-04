//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&��������������&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:mcu.c

//*�ļ�����:��������������

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



#define EMI_ENABLE       0x8000
#define EMI_DISABLE      0x0000
#define EMI_WAITSTATE(n) ((n) << 2)
#define EMI_SIZE_8       0x0000
#define EMI_SIZE_16      0x0001



//***********************************************************************************************************************
//��������:ר�����ĳ��arm�ĳ�ʼ��
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void mcu_init(void)
{
	//����ϵͳ��Ƶ
	RCCU_CFR|=0x8000;//�Ѿ���Ƶ��2��Ƶ
	RCCU_PLL1CR&=(~(1<<7));//�ر���������ģʽ
	RCCU_PLL1CR|=(1<<6);//���þ���Ƶ�ʷ�Χ---����3MHz
	if(Fosc==16000000)//���ݾ���ͬ���ò�ͬ��PLL��Ƶ�ͱ�Ƶ
	{
		RCCU_PLL1CR&=(~(0x3<<4));RCCU_PLL1CR|=(0x1<<4);//��PLL���12��Ƶ
		RCCU_PLL1CR&=(~0x07);RCCU_PLL1CR|=0x1;//��PLL���2��Ƶ	
	}
	else if(Fosc==8000000)//���ݾ���ͬ���ò�ͬ��PLL��Ƶ�ͱ�Ƶ
	{
		RCCU_PLL1CR&=(~(0x3<<4));RCCU_PLL1CR|=(0x1<<4);//��PLL���12��Ƶ
		RCCU_PLL1CR&=(~0x07);//��PLL���û�з�Ƶ
	}
	PCU_MDIVR&=(~0x3);//����MCLK��Ƶϵ��---����Ƶ
	//����Ƭ�������ʱ��
	PCU_PDIVR&=(~0x3);//����APB1��Ƶϵ��---����Ƶ
	PCU_PDIVR&=(~(0x3<<8));//����APB2��Ƶϵ��---����Ƶ
	RCCU_PER=0;//�ر�����Ƭ�������ʱ��
	RCCU_PER|=(1<<4);//ʹ��Ƭ�������USBʱ��
	RCCU_PER|=(1<<2);//ʹ��Ƭ�������EMIʱ��
	while(!(RCCU_CFR&(0x1<<1)));//�ȴ����໷����

	//ѡ��PLL���Ϊϵͳʱ��Դ
	RCCU_CFR|=0x00000008;//Set the CK2_16 Bit in the CFR
	if((RCCU_PLL1CR&0x0007)!=7)while(!(RCCU_CFR&(0x1<<1)));//�ȴ����໷����
	RCCU_CCR&=~0x00000004;// Deselect The CKAF
	RCCU_CFR|=0x00000001;//Select The CSU_CKSEL
	
	
	//�����ⲿ����Ƭѡ
	//GPIO2_PC0|=Port_Pins;
	//GPIO2_PC1|=Port_Pins;
	//GPIO2_PC2|=Port_Pins;
	//�����ⲿ����BANK2
	//BCON2=EMI_ENABLE | EMI_WAITSTATE(15) | EMI_SIZE_16;
	//�����ⲿ����BANK1 to xian 2 line
	//BCON1=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON2=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON3=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
	//BCON0=EMI_ENABLE | EMI_WAITSTATE(12) | EMI_SIZE_16;
}



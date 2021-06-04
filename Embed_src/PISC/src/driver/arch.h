//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆硬件平台定义◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//◆文件名称:arch.h
//◆文件说明:硬件平台定义
//◆文件作者:翟  鹏
//◆创建日期:2007年5月
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆



#ifndef ARCH_H
#define ARCH_H



//Flash存储器
#define FLASHR_CR0	(*((volatile unsigned int *) 0x40100000))
#define FLASHR_CR1	(*((volatile unsigned int *) 0x40100004))
#define FLASHR_DR0	(*((volatile unsigned int *) 0x40100008))
#define FLASHR_DR1	(*((volatile unsigned int *) 0x4010000C))
#define FLASHR_AR	(*((volatile unsigned int *) 0x40100010))
#define FLASHR_ER	(*((volatile unsigned int *) 0x40100014))
#define FLASHPR_NVWPAR	(*((volatile unsigned int *) 0x4010DFB0))
#define FLASHPR_EMPTY	(*((volatile unsigned int *) 0x4010DFB4))
#define FLASHPR_NVAPR0	(*((volatile unsigned int *) 0x4010DFB8))
#define FLASHPR_NVAPR1	(*((volatile unsigned int *) 0x4010DFBC))



//外部总线控制器
#define BCON0		(*((volatile unsigned short *) 0x6C000000))
#define BCON1		(*((volatile unsigned short *) 0x6C000004))
#define BCON2		(*((volatile unsigned short *) 0x6C000008))
#define BCON3		(*((volatile unsigned short *) 0x6C00000C))



//时钟 电源 启动
#define RCCU_CCR	(*((volatile unsigned int *) 0xA0000000))
#define RCCU_CFR	(*((volatile unsigned int *) 0xA0000008))
#define RCCU_PLL1CR	(*((volatile unsigned int *) 0xA0000018))
#define RCCU_PER	(*((volatile unsigned int *) 0xA000001C))
#define RCCU_SMR	(*((volatile unsigned int *) 0xA0000020))
#define PCU_MDIVR	(*((volatile unsigned int *) 0xA0000040))
#define PCU_PDIVR	(*((volatile unsigned int *) 0xA0000044))
#define PCU_PRSTR	(*((volatile unsigned int *) 0xA0000048))
#define PCU_PLL2CR	(*((volatile unsigned int *) 0xA000004C))
#define PCU_BOOTCR	(*((volatile unsigned int *) 0xA0000050))
#define PCU_PWRCR	(*((volatile unsigned int *) 0xA0000054))



//I2C
#define I2C0_CR		(*((volatile unsigned int *) 0xC0001000))
#define I2C0_SR1	(*((volatile unsigned int *) 0xC0001004))
#define I2C0_SR2	(*((volatile unsigned int *) 0xC0001008))
#define I2C0_CCR	(*((volatile unsigned int *) 0xC000100C))
#define I2C0_OAR1	(*((volatile unsigned int *) 0xC0001010))
#define I2C0_OAR2	(*((volatile unsigned int *) 0xC0001014))
#define I2C0_DR		(*((volatile unsigned int *) 0xC0001018))
#define I2C0_ECCR	(*((volatile unsigned int *) 0xC000101C))
#define I2C1_CR		(*((volatile unsigned int *) 0xC0002000))
#define I2C1_SR1	(*((volatile unsigned int *) 0xC0002004))
#define I2C1_SR2	(*((volatile unsigned int *) 0xC0002008))
#define I2C1_CCR	(*((volatile unsigned int *) 0xC000200C))
#define I2C1_OAR1	(*((volatile unsigned int *) 0xC0002010))
#define I2C1_OAR2	(*((volatile unsigned int *) 0xC0002014))
#define I2C1_DR		(*((volatile unsigned int *) 0xC0002018))
#define I2C1_ECCR	(*((volatile unsigned int *) 0xC000201C))



//UART
#define UART0_BR	(*((volatile unsigned int *) 0xC0004000))
#define UART0_TxBUFR	(*((volatile unsigned int *) 0xC0004004))
#define UART0_RxBUFR	(*((volatile unsigned int *) 0xC0004008))
#define UART0_CR	(*((volatile unsigned int *) 0xC000400C))
#define UART0_IER	(*((volatile unsigned int *) 0xC0004010))
#define UART0_SR	(*((volatile unsigned int *) 0xC0004014))
#define UART0_GTR	(*((volatile unsigned int *) 0xC0004018))
#define UART0_TOR	(*((volatile unsigned int *) 0xC000401C))
#define UART0_TxRSTR	(*((volatile unsigned int *) 0xC0004020))
#define UART0_RxRSTR	(*((volatile unsigned int *) 0xC0004024))
#define UART1_BR	(*((volatile unsigned int *) 0xC0005000))
#define UART1_TxBUFR	(*((volatile unsigned int *) 0xC0005004))
#define UART1_RxBUFR	(*((volatile unsigned int *) 0xC0005008))
#define UART1_CR	(*((volatile unsigned int *) 0xC000500C))
#define UART1_IER	(*((volatile unsigned int *) 0xC0005010))
#define UART1_SR	(*((volatile unsigned int *) 0xC0005014))
#define UART1_GTR	(*((volatile unsigned int *) 0xC0005018))
#define UART1_TOR	(*((volatile unsigned int *) 0xC000501C))
#define UART1_TxRSTR	(*((volatile unsigned int *) 0xC0005020))
#define UART1_RxRSTR	(*((volatile unsigned int *) 0xC0005024))
#define UART2_BR	(*((volatile unsigned int *) 0xC0006000))
#define UART2_TxBUFR	(*((volatile unsigned int *) 0xC0006004))
#define UART2_RxBUFR	(*((volatile unsigned int *) 0xC0006008))
#define UART2_CR	(*((volatile unsigned int *) 0xC000600C))
#define UART2_IER	(*((volatile unsigned int *) 0xC0006010))
#define UART2_SR	(*((volatile unsigned int *) 0xC0006014))
#define UART2_GTR	(*((volatile unsigned int *) 0xC0006018))
#define UART2_TOR	(*((volatile unsigned int *) 0xC000601C))
#define UART2_TxRSTR	(*((volatile unsigned int *) 0xC0006020))
#define UART2_RxRSTR	(*((volatile unsigned int *) 0xC0006024))
#define UART3_BR	(*((volatile unsigned int *) 0xC0007000))
#define UART3_TxBUFR	(*((volatile unsigned int *) 0xC0007004))
#define UART3_RxBUFR	(*((volatile unsigned int *) 0xC0007008))
#define UART3_CR	(*((volatile unsigned int *) 0xC000700C))
#define UART3_IER	(*((volatile unsigned int *) 0xC0007010))
#define UART3_SR	(*((volatile unsigned int *) 0xC0007014))
#define UART3_GTR	(*((volatile unsigned int *) 0xC0007018))
#define UART3_TOR	(*((volatile unsigned int *) 0xC000701C))
#define UART3_TxRSTR	(*((volatile unsigned int *) 0xC0007020))
#define UART3_RxRSTR	(*((volatile unsigned int *) 0xC0007024))



//USB
#define USB_BUFADDR		0xC0008000
#define USB_ADDR_TX(EpNum)	(*((volatile unsigned int *)(USB_BUFADDR+USB_BTABLE*2+(EpNum)*16+0)))
#define USB_COUNT_TX(EpNum)	(*((volatile unsigned int *)(USB_BUFADDR+USB_BTABLE*2+(EpNum)*16+4)))
#define USB_ADDR_RX(EpNum)	(*((volatile unsigned int *)(USB_BUFADDR+USB_BTABLE*2+(EpNum)*16+8)))
#define USB_COUNT_RX(EpNum)	(*((volatile unsigned int *)(USB_BUFADDR+USB_BTABLE*2+(EpNum)*16+12)))
#define USB_EPxR(EpNum)		(*((volatile unsigned int *)(0xC0008800+4*EpNum)))
#define USB_CNTR		(*((volatile unsigned int *) 0xC0008840))
#define USB_ISTR		(*((volatile unsigned int *) 0xC0008844))
#define USB_FNR			(*((volatile unsigned int *) 0xC0008848))
#define USB_DADDR		(*((volatile unsigned int *) 0xC000884C))
#define USB_BTABLE		(*((volatile unsigned int *) 0xC0008850))



//外部中断
#define XTI_SR		(*((volatile unsigned int *) 0xE000101C))
#define XTI_CTRL	(*((volatile unsigned int *) 0xE0001024))
#define XTI_MRH		(*((volatile unsigned int *) 0xE0001028))
#define XTI_MRL		(*((volatile unsigned int *) 0xE000102C))
#define XTI_TRH		(*((volatile unsigned int *) 0xE0001030))
#define XTI_TRL		(*((volatile unsigned int *) 0xE0001034))
#define XTI_PRH		(*((volatile unsigned int *) 0xE0001038))
#define XTI_PRL		(*((volatile unsigned int *) 0xE000103C))



//I/O口
#define GPIO0_PC0	(*((volatile unsigned int *) 0xE0003000))
#define GPIO0_PC1	(*((volatile unsigned int *) 0xE0003004))
#define GPIO0_PC2	(*((volatile unsigned int *) 0xE0003008))
#define GPIO0_PD	(*((volatile unsigned short *) 0xE000300C))
#define GPIO1_PC0	(*((volatile unsigned int *) 0xE0004000))
#define GPIO1_PC1	(*((volatile unsigned int *) 0xE0004004))
#define GPIO1_PC2	(*((volatile unsigned int *) 0xE0004008))
#define GPIO1_PD	(*((volatile unsigned short *) 0xE000400C))
#define GPIO2_PC0	(*((volatile unsigned int *) 0xE0005000))
#define GPIO2_PC1	(*((volatile unsigned int *) 0xE0005004))
#define GPIO2_PC2	(*((volatile unsigned int *) 0xE0005008))
#define GPIO2_PD	(*((volatile unsigned short *) 0xE000500C))



//定时器
#define TIM0_ICAR	(*((volatile unsigned short *) 0xE0009000))
#define TIM0_ICBR	(*((volatile unsigned short *) 0xE0009004))
#define TIM0_OCAR	(*((volatile unsigned short *) 0xE0009008))
#define TIM0_OCBR	(*((volatile unsigned short *) 0xE000900C))
#define TIM0_CNTR	(*((volatile unsigned short *) 0xE0009010))
#define TIM0_CR1	(*((volatile unsigned short *) 0xE0009014))
#define TIM0_CR2	(*((volatile unsigned short *) 0xE0009018))
#define TIM0_SR		(*((volatile unsigned short *) 0xE000901C))
#define TIM1_ICAR	(*((volatile unsigned short *) 0xE000A000))
#define TIM1_ICBR	(*((volatile unsigned short *) 0xE000A004))
#define TIM1_OCAR	(*((volatile unsigned short *) 0xE000A008))
#define TIM1_OCBR	(*((volatile unsigned short *) 0xE000A00C))
#define TIM1_CNTR	(*((volatile unsigned short *) 0xE000A010))
#define TIM1_CR1	(*((volatile unsigned short *) 0xE000A014))
#define TIM1_CR2	(*((volatile unsigned short *) 0xE000A018))
#define TIM1_SR		(*((volatile unsigned short *) 0xE000A01C))
#define TIM2_ICAR	(*((volatile unsigned short *) 0xE000B000))
#define TIM2_ICBR	(*((volatile unsigned short *) 0xE000B004))
#define TIM2_OCAR	(*((volatile unsigned short *) 0xE000B008))
#define TIM2_OCBR	(*((volatile unsigned short *) 0xE000B00C))
#define TIM2_CNTR	(*((volatile unsigned short *) 0xE000B010))
#define TIM2_CR1	(*((volatile unsigned short *) 0xE000B014))
#define TIM2_CR2	(*((volatile unsigned short *) 0xE000B018))
#define TIM2_SR		(*((volatile unsigned short *) 0xE000B01C))
#define TIM3_ICAR	(*((volatile unsigned short *) 0xE000C000))
#define TIM3_ICBR	(*((volatile unsigned short *) 0xE000C004))
#define TIM3_OCAR	(*((volatile unsigned short *) 0xE000C008))
#define TIM3_OCBR	(*((volatile unsigned short *) 0xE000C00C))
#define TIM3_CNTR	(*((volatile unsigned short *) 0xE000C010))
#define TIM3_CR1	(*((volatile unsigned short *) 0xE000C014))
#define TIM3_CR2	(*((volatile unsigned short *) 0xE000C018))
#define TIM3_SR		(*((volatile unsigned short *) 0xE000C01C))



//实时时钟-RTC
#define RTC_CRH		(*((volatile unsigned int *) 0xE000D000))
#define RTC_CRL		(*((volatile unsigned int *) 0xE000D004))
#define RTC_PRLH	(*((volatile unsigned int *) 0xE000D008))
#define RTC_PRLL	(*((volatile unsigned int *) 0xE000D00C))
#define RTC_DIVH	(*((volatile unsigned int *) 0xE000D010))
#define RTC_DIVL	(*((volatile unsigned int *) 0xE000D014))
#define RTC_CNTH	(*((volatile unsigned int *) 0xE000D018))
#define RTC_CNTL	(*((volatile unsigned int *) 0xE000D01C))
#define RTC_ALRH	(*((volatile unsigned int *) 0xE000D020))
#define RTC_ALRL	(*((volatile unsigned int *) 0xE000D024))



//看门狗--WDG
#define WDG_CR		(*((volatile unsigned int *) 0xE000E000))
#define WDG_PR		(*((volatile unsigned int *) 0xE000E004))
#define WDG_VR		(*((volatile unsigned int *) 0xE000E008))
#define WDG_CNT		(*((volatile unsigned int *) 0xE000E00C))
#define WDG_SR		(*((volatile unsigned int *) 0xE000E010))
#define WDG_MR		(*((volatile unsigned int *) 0xE000E014))
#define WDG_KR		(*((volatile unsigned int *) 0xE000E018))



//中断控制器
#define EIC_ICR		(*((volatile unsigned int *) 0xFFFFF800))
#define EIC_CICR	(*((volatile unsigned int *) 0xFFFFF804))
#define EIC_CIPR	(*((volatile unsigned int *) 0xFFFFF808))
#define EIC_IVR		(*((volatile unsigned int *) 0xFFFFF818))
#define EIC_FIR		(*((volatile unsigned int *) 0xFFFFF81C))
#define EIC_IER		(*((volatile unsigned int *) 0xFFFFF820))
#define EIC_IPR		(*((volatile unsigned int *) 0xFFFFF840))
#define EIC_SIR0	(*((volatile unsigned int *) 0xFFFFF860))
#define EIC_SIR1	(*((volatile unsigned int *) 0xFFFFF864))
#define EIC_SIR2	(*((volatile unsigned int *) 0xFFFFF868))
#define EIC_SIR3	(*((volatile unsigned int *) 0xFFFFF86C))
#define EIC_SIR4	(*((volatile unsigned int *) 0xFFFFF870))
#define EIC_SIR5	(*((volatile unsigned int *) 0xFFFFF874))
#define EIC_SIR6	(*((volatile unsigned int *) 0xFFFFF878))
#define EIC_SIR7	(*((volatile unsigned int *) 0xFFFFF87C))
#define EIC_SIR8	(*((volatile unsigned int *) 0xFFFFF880))
#define EIC_SIR9	(*((volatile unsigned int *) 0xFFFFF884))
#define EIC_SIR10	(*((volatile unsigned int *) 0xFFFFF888))
#define EIC_SIR11	(*((volatile unsigned int *) 0xFFFFF88C))
#define EIC_SIR12	(*((volatile unsigned int *) 0xFFFFF890))
#define EIC_SIR13	(*((volatile unsigned int *) 0xFFFFF894))
#define EIC_SIR14	(*((volatile unsigned int *) 0xFFFFF898))
#define EIC_SIR15	(*((volatile unsigned int *) 0xFFFFF89C))
#define EIC_SIR16	(*((volatile unsigned int *) 0xFFFFF8A0))
#define EIC_SIR17	(*((volatile unsigned int *) 0xFFFFF8A4))
#define EIC_SIR18	(*((volatile unsigned int *) 0xFFFFF8A8))
#define EIC_SIR19	(*((volatile unsigned int *) 0xFFFFF8AC))
#define EIC_SIR20	(*((volatile unsigned int *) 0xFFFFF8B0))
#define EIC_SIR21	(*((volatile unsigned int *) 0xFFFFF8B4))
#define EIC_SIR22	(*((volatile unsigned int *) 0xFFFFF8B8))
#define EIC_SIR23	(*((volatile unsigned int *) 0xFFFFF8BC))
#define EIC_SIR24	(*((volatile unsigned int *) 0xFFFFF8C0))
#define EIC_SIR25	(*((volatile unsigned int *) 0xFFFFF8C4))
#define EIC_SIR26	(*((volatile unsigned int *) 0xFFFFF8C8))
#define EIC_SIR27	(*((volatile unsigned int *) 0xFFFFF8CC))
#define EIC_SIR28	(*((volatile unsigned int *) 0xFFFFF8D0))
#define EIC_SIR29	(*((volatile unsigned int *) 0xFFFFF8D4))
#define EIC_SIR30	(*((volatile unsigned int *) 0xFFFFF8D8))
#define EIC_SIR31	(*((volatile unsigned int *) 0xFFFFF8DC))



#endif



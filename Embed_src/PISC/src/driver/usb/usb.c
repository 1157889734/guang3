//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& usb驱动 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:usb.c

//*文件作用:usb驱动

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//中断屏蔽字
uint16  wInterrupt_Mask=IMR_MSK;
//唤醒请求状态位的持续时间---计数器
volatile uint8 resume_esof_cnt=0;
//是否正在发送
static uint8 is_sending=0;

//fifo定义
static uint8 usb_recv_fifo[USB_FIFO_SIZE];
static uint16 usb_recv_fifo_in=0;
static uint16 usb_recv_fifo_out=0;
static uint8 usb_send_fifo[USB_FIFO_SIZE];
static uint16 usb_send_fifo_in=0;
static uint16 usb_send_fifo_out=0;

	

//***********************************************************************************************************************
//函数作用:usb线缆插上 中断检测
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void XTI_IRQHandler(void)
{	
	Device_Info.Current_Configuration=0;
	is_sending=0;
	usb_send_fifo_in=usb_send_fifo_out=0;
		
	resume_esof_cnt=50;//设置唤醒状态位的持续时间
	
	XTI_PRL &= ~(XTI_PRL|((uint16)XTI_PRH<<8));
	XTI_PRH &= ~((XTI_PRL|((uint16)XTI_PRH<<8)>>8)&0x00FF);
	
	//RCCU_PLL2=ON//开启48MHz时钟
	USB_CNTR&=(~CNTR_LPMODE);//退出低功耗模式
	USB_CNTR&=~(CNTR_FSUSP);//推出挂起模式
	USB_CNTR=(CNTR_CTRM|CNTR_WKUPM|CNTR_SUSPM|CNTR_ERRM|CNTR_SOFM|CNTR_ESOFM|CNTR_RESETM);//开中断
	USB_CNTR|=CNTR_RESUME;//向主机发送唤醒请求
}

//***********************************************************************************************************************
//函数作用:usb低级中断服务程序
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void USBLP_IRQHandler(void)
{
	//用于存储中断状态寄存器的值
	volatile uint16 wIstr;
	
	//读中断寄存器
	wIstr=USB_ISTR;

	//设备复位中断
	if(wIstr & ISTR_RESET & wInterrupt_Mask)
	{
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_RESET));
		
		USB_CNTR&=(~CNTR_RESUME);//撤销唤醒请求
		//Set DEVICE as not configured
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//设置分组缓冲区在USB_BUFADDR中的偏移
		USB_BTABLE = (uint16)(BTABLE_ADDRESS & 0xFFF8);
		//Initialize Endpoint 0
		USB_EPxR(ENDP0)=(USB_EPxR(ENDP0)&EP_T_MASK)|EP_CONTROL;//控制传输
		USB_ADDR_RX(ENDP0)=((ENDP0_RXADDR>>1)<<1);//接收缓冲区地址
		SetEPRxCount(ENDP0, STD_MAXPACKETSIZE);//接收缓冲区长度
		SetEPTxStatus(ENDP0, EP_TX_NAK);//关闭发送
		USB_ADDR_TX(ENDP0) =((ENDP0_TXADDR>>1)<<1);//发送缓冲区地址
		USB_COUNT_TX(ENDP0)=STD_MAXPACKETSIZE;//要发送的长度
		USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&EPKIND_MASK;//清状态
		SetEPRxStatus(ENDP0, EP_RX_VALID);//启动接收
		//Initialize Endpoint 1
		USB_EPxR(ENDP1)=(USB_EPxR(ENDP1)&EP_T_MASK)|EP_INTERRUPT;//中断传输
		USB_ADDR_RX(ENDP1)=((ENDP1_RXADDR>>1)<<1);//接收缓冲区地址
		SetEPRxCount(ENDP1, EP1_MAXPACKETSIZE);//接收缓冲区长度
		SetEPTxStatus(ENDP1, EP_TX_NAK);//关闭发送
		USB_ADDR_TX(ENDP1) =((ENDP1_TXADDR>>1)<<1);//发送缓冲区地址
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//要发送的长度
		USB_EPxR(ENDP1)=USB_EPxR(ENDP1)&EPKIND_MASK;//清状态
		SetEPRxStatus(ENDP1, EP_RX_VALID);//启动接收
		//设置设备地址为0 以便响应PC的缺省请求
		SetDeviceAddress(0);
	}
	//DMA OVER中断
	if(wIstr & ISTR_DOVR & wInterrupt_Mask)
	{
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_DOVR));
	}
	//错误中断
	if(wIstr & ISTR_ERR & wInterrupt_Mask)
	{
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_ERR));
	}
	//唤醒中断
	if(wIstr & ISTR_WKUP & wInterrupt_Mask)
	{
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_WKUP));

		//RCCU_PLL2=ON//开启48MHz时钟
		//USB_CNTR&=(~CNTR_LPMODE);//退出低功耗模式
		USB_CNTR=(CNTR_CTRM|CNTR_WKUPM|CNTR_SUSPM|CNTR_ERRM|CNTR_SOFM|CNTR_ESOFM||CNTR_RESETM);//开中断
		//判断是否是干扰再次进入挂起模式
		if(USB_FNR&FNR_RXDP)
		{
			//USB_CNTR|=CNTR_FSUSP;//进入挂起模式
			//USB_CNTR|=CNTR_LPMODE;//进低功耗模式
			//RCCU_PLL2=OFF//关闭48MHz时钟
		}
	}
	//挂起中断
	if(wIstr & ISTR_SUSP & wInterrupt_Mask)
	{
		//Set DEVICE as not configured
		Device_Info.Current_Configuration=0;
		is_sending=0;
		usb_send_fifo_in=usb_send_fifo_out=0;
		//USB_CNTR|=CNTR_FSUSP;//进入挂起模式
		//USB_CNTR|=CNTR_LPMODE;//进低功耗模式
		//RCCU_PLL2=OFF//关闭48MHz时钟
		
		//必须先挂起在清中断标志
		USB_ISTR=((uint16)(~ISTR_SUSP));
	}
	//桢开始中断
	if(wIstr & ISTR_SOF & wInterrupt_Mask)
	{
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_SOF));
	}
	//期待桢开始中断
	if(wIstr & ISTR_ESOF & wInterrupt_Mask)
	{
		//清中断标志
		USB_ISTR=((uint16)(~ISTR_ESOF));
		
		//判断唤醒状态位持续时间到 撤销唤醒状态位
		if(resume_esof_cnt)
		{
			if(--resume_esof_cnt==0)
			{
				USB_CNTR&=(~CNTR_RESUME);//撤销唤醒请求
			}
		}
		else
		{
			USB_CNTR&=(~CNTR_RESUME);//撤销唤醒请求
		}
	}
	//端点正确传输中断
	if(wIstr & ISTR_CTR & wInterrupt_Mask)
	{
		uint16 wEPVal;//端点状态值
		uint8 EPindex;//端点号
		
	 	//一直读取中断寄存器 直到所有数据传输处理完成
		while((wIstr=USB_ISTR)&ISTR_CTR)
		{
			//清中断标志
			USB_ISTR=((uint16)(~ISTR_CTR)); /* clear CTR flag */
			//读取端点号
			EPindex=(uint8)(wIstr & ISTR_EP_ID);
			//处理端点0
			if(EPindex==0)
			{
				//保存端点0状态 并暂时关闭数据收发
				SaveRState=((uint16)USB_EPxR(ENDP0)&EPRX_STAT);
				SaveTState=((uint16)USB_EPxR(ENDP0)&EPTX_STAT);
				SetEPRxStatus(ENDP0, EP_RX_NAK);
				SetEPTxStatus(ENDP0, EP_TX_NAK);

				//判断传输方向		设备->主机
				if((wIstr&ISTR_DIR)==0)
				{
					//读取端点状态
					wEPVal=USB_EPxR(ENDP0);
					//端点发送完毕处理
					//if((wEPVal & EP_CTR_TX) != 0)
					{
						//清除端点的发送完成标志
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0xFF7F&EPREG_MASK;
						In0_Process();
					}
					//处理发送过程中的主机设置
					if(wEPVal&(EP_CTR_RX|EP_SETUP))
					{
						//清除端点的接收完成标志
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Setup0_Process();
					}

				}
				//判断传输方向		主机->设备
				else
				{
					//读取端点状态
					wEPVal=USB_EPxR(ENDP0);
					//端点发送完毕处理
					if(wEPVal&EP_CTR_TX)
					{
						//清除端点的发送完成标志
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0xFF7F&EPREG_MASK;
						In0_Process();
					}
					
					//处理主机设置
					if(wEPVal&(EP_CTR_RX|EP_SETUP))
					{
						//清除端点的接收完成标志
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Setup0_Process();
					}
					//端点接收完毕处理
					else if(wEPVal&EP_CTR_RX)
					{
						//清除端点的接收完成标志
						USB_EPxR(ENDP0)=USB_EPxR(ENDP0)&0x7FFF&EPREG_MASK;
						Out0_Process();
					}
				}
				
				//恢复端点0的状态
				SetEPRxStatus(ENDP0, SaveRState);
				SetEPTxStatus(ENDP0, SaveTState);
			}
			//处理其他端点
			else
			{
				//读取端点状态
				wEPVal=USB_EPxR(EPindex);
				//端点发送完毕处理 继续向硬件FIFO装载数据
				if(wEPVal&EP_CTR_TX)
				{
					//清除端点的发送完成标志
					USB_EPxR(EPindex)=USB_EPxR(EPindex)&0xFF7F&EPREG_MASK;
					
					//处理端点1的发送
					if(EPindex==1)
					{
						//没有数据要发送 关端点的发送(自动关闭了)
						if(usb_send_fifo_out==usb_send_fifo_in)
						{
							is_sending=0;
						}
						//有数据要发送 把硬件FIFO装满
						else
						{
							uint32 *packet_length;//包中有用数据的长度
							uint32 *dst;
							uint16 Nbytes=0;
							
							is_sending=1;
							dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
							packet_length=dst;//指向长度的位置
							(*packet_length)=0;//长度清0
							dst++;//前两个字节是包中有用数据的长度 从第三个字节开始填充
							Nbytes+=2;
							while(Nbytes<EP1_MAXPACKETSIZE)
							{
								//第一个字节送硬件fifo
								if(usb_send_fifo_out!=usb_send_fifo_in)
								{
									*dst=usb_send_fifo[usb_send_fifo_out];
									if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
									(*packet_length)++;//增加有用数据的长度
								}
								else
								{
									*dst=0;
								}
								//第二个字节送硬件fifo
								if(usb_send_fifo_out!=usb_send_fifo_in)
								{
									*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
									if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
									(*packet_length)++;//增加有用数据的长度
								}
								else
								{
									*dst|=0;
								}
								dst++;
								Nbytes+=2;
							}
							USB_COUNT_TX(EPindex)=EP1_MAXPACKETSIZE;//设置要发送的长度
							SetEPTxStatus(EPindex,EP_TX_VALID);//启动发送
						}
					}
					//处理端点2的发送
					else if(EPindex==2)
					{
						
					}
				}
				//端点接收完毕处理
				if(wEPVal&EP_CTR_RX)
				{
					//处理端点1的接收
					if(EPindex==1)
					{
						uint16 packet_length;//包中有用数据的长度
						uint8 *src;
						
						src=USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_RX(EPindex)*2);
						packet_length=*((uint16 *)src);//USB_COUNT_RX(EPindex)&0x03FF;//提取有用数据的长度
						if(packet_length>EP1_MAXPACKETSIZE-2)
							packet_length=EP1_MAXPACKETSIZE-2;
						src+=4;//前两个字节是包中有用数据的长度 从第三个字节开始读取
						while(packet_length)
						{
							//第一个字节送软件fifo
							usb_recv_fifo[usb_recv_fifo_in++]=*src++;
							if(usb_recv_fifo_in>=USB_FIFO_SIZE)
								usb_recv_fifo_in=0;
							packet_length--;
							if(packet_length)
							{
								//第二个字节送软件fifo
								usb_recv_fifo[usb_recv_fifo_in++]=*src++;
								if(usb_recv_fifo_in>=USB_FIFO_SIZE)
									usb_recv_fifo_in=0;
								packet_length--;
							}
							//因为接收硬件fifo是32位操作的 而且32位的前两个字节有效 所以跳过后两个字节
							src+=2;
						}
					}
					//处理端点2的接收
					else if(EPindex==2)
					{
						
					}
					
					//清除端点的接收完成标志
					USB_EPxR(EPindex)=USB_EPxR(EPindex)&0x7FFF&EPREG_MASK;			
					SetEPRxStatus(EPindex,EP_RX_VALID);//启动下次接收
				}
			}
		}
	}
}

//***********************************************************************************************************************
//函数作用:usb初始化
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void usb_init(uint32 bps)
{	
	bps=bps;
	
	//注册usb中断向量
	USB_IRQ_SLOT=(USB_IRQ_SLOT&0xFFFF0000)|USB_IRQ_PRIORITY;//设置优先级--最高15
	EIC_IER|=1<<USB_IRQ_VECTOR;//使能中断控制器相应的槽

	//设备上电
	USB_CNTR=CNTR_FRES|CNTR_PDWN;//关电源并保持复位状态
	USB_CNTR=CNTR_FRES;//开电源并保持复位状态
	USB_ISTR=0;//清除中断标志
	//Set DEVICE as not configured
	Device_Info.Current_Configuration=0;
	is_sending=0;
	usb_send_fifo_in=usb_send_fifo_out=0;
	//USB_CNTR|=CNTR_FSUSP;//进入挂起模式
	//USB_CNTR|=CNTR_LPMODE;//进低功耗模式
	
	USB_CNTR=CNTR_RESETM|CNTR_SUSPM|CNTR_WKUPM;//开一部分中断 等待主机唤醒

	//注册外部中断 usb线缆插上 中断检测
	XTI_TRL &= ~(1<<1);//XTI Line1 FallingEdge
	XTI_TRH &= ~((1<<1)>>8)&0x00FF;//XTI Line1 FallingEdge
	XTI_MRL |= (1<<1);//XTI Line1 enable
	XTI_MRH |= ((1<<1)>>8)&0x00FF;//XTI Line1 enable
	USB_DET_IRQ_SLOT=(USB_DET_IRQ_SLOT&0xFFFF0000)|USB_DET_IRQ_PRIORITY;//设置优先级--最高15
	EIC_IER|=1<<USB_DET_IRQ_VECTOR;//使能中断控制器相应的槽
}

//***********************************************************************************************************************
//函数作用:查询usb的接收fifo 如果有数据读取一个字节
//参数说明:ch---读取字符存放地址
//注意事项:
//返回说明:如果有数据返回1
//***********************************************************************************************************************
uint8 usb_read_char(uint8 *ch)
{
	if(usb_recv_fifo_out == usb_recv_fifo_in)
		return 0;
	
	*ch = usb_recv_fifo[usb_recv_fifo_out];//得到当前要读取的字符
	usb_recv_fifo_out++;//指向下一个要读取的字符
	if(usb_recv_fifo_out == USB_FIFO_SIZE)
		usb_recv_fifo_out = 0;//如果到了fifo末尾 则重从头开始
	
	return 1;
}

//***********************************************************************************************************************
//函数作用:向usb数据端点发送一个字节
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void usb_write_char(uint8 ch)
{	
	uint16 fifo_in_test;

	//如果设备还没有配置好 退出
	if(Device_Info.Current_Configuration==0)
	{
		return;
	}

	//测试缓冲区是否满
	fifo_in_test=usb_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=USB_FIFO_SIZE)
		fifo_in_test=0;
	if(fifo_in_test==usb_send_fifo_out)
		return;
	//向FIFO填充数据
	usb_send_fifo[usb_send_fifo_in++]=ch;
	if(usb_send_fifo_in>=USB_FIFO_SIZE)
		usb_send_fifo_in=0;
		
	//产生一个发送中断 利用中断发送剩余数据
	if(is_sending==0)
	{
		uint32 *packet_length;//包中有用数据的长度
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//指向长度的位置
		(*packet_length)=0;//长度清0
		dst++;//前两个字节是包中有用数据的长度 从第三个字节开始填充
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//第一个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst=0;
			}
			//第二个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//设置要发送的长度
		SetEPTxStatus(ENDP1,EP_TX_VALID);//启动发送
	}	
}

//***********************************************************************************************************************
//函数作用:向usb数据端点发送定长数据
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 usb_write_buf(uint8 *buf, uint16 length)
{	
	uint16 send_fifo_in_backup=usb_send_fifo_in;
	
	//如果设备还没有配置好 退出
	if(Device_Info.Current_Configuration==0)
	{
		return 0;
	}
	
	while(length--)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)
		{
			//缓冲区满恢复发送之前的指针
			usb_send_fifo_in=send_fifo_in_backup;
			return 0;
		}
		//向FIFO填充数据
		usb_send_fifo[usb_send_fifo_in++]=*buf++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)
			usb_send_fifo_in=0;
	}
	//产生一个发送中断 利用中断发送剩余数据
	if(is_sending==0)
	{
		uint32 *packet_length;//包中有用数据的长度
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//指向长度的位置
		(*packet_length)=0;//长度清0
		dst++;//前两个字节是包中有用数据的长度 从第三个字节开始填充
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//第一个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst=0;
			}
			//第二个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//设置要发送的长度
		SetEPTxStatus(ENDP1,EP_TX_VALID);//启动发送
	}
	
	return 1;
}

//***********************************************************************************************************************
//函数作用:向usb数据端点发送字符串
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void usb_write_str(char *str)
{	
	//如果设备还没有配置好 退出
	if(Device_Info.Current_Configuration==0)
	{
		return;
	}
	
	while(*str)
	{
		//测试缓冲区是否满
		uint16 fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)
			fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)
			return;
		//向FIFO填充数据
		usb_send_fifo[usb_send_fifo_in++]=*str++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)
			usb_send_fifo_in=0;
	}
	//产生一个发送中断 利用中断发送剩余数据
	if(is_sending==0)
	{
		uint32 *packet_length;//包中有用数据的长度
		uint32 *dst;
		uint16 Nbytes=0;

		dst=(uint32 *)(USB_BUFADDR+(uint8 *)((uint16)USB_ADDR_TX(ENDP1)*2));
		packet_length=dst;//指向长度的位置
		(*packet_length)=0;//长度清0
		dst++;//前两个字节是包中有用数据的长度 从第三个字节开始填充
		Nbytes+=2;
		while(Nbytes<EP1_MAXPACKETSIZE)
		{
			//第一个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst=usb_send_fifo[usb_send_fifo_out];
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst=0;
			}
			//第二个字节送硬件fifo
			if(usb_send_fifo_out!=usb_send_fifo_in)
			{
				*dst|=(usb_send_fifo[usb_send_fifo_out]<<8);
				if(++usb_send_fifo_out>=USB_FIFO_SIZE)
					usb_send_fifo_out=0;
				(*packet_length)++;//增加有用数据的长度
			}
			else
			{
				*dst|=0;
			}
			dst++;
			Nbytes+=2;
		}
		is_sending=1;
		USB_COUNT_TX(ENDP1)=EP1_MAXPACKETSIZE;//设置要发送的长度
		SetEPTxStatus(ENDP1,EP_TX_VALID);//启动发送
	}
}


//***********************************************************************************************************************
//函数作用:usb获取是否连接状态
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
uint8 usb_get_connect_state(void)
{
	if( Device_Info.Current_Configuration == 0 )
		return 0;
	else
		return 1;
}





//设置端点接收缓冲区长度
void SetEPRxCount(uint8 bEpNum, uint16 wCount)
{
	uint16 wNBlocks;
	
	if(wCount>62)
	{
		wNBlocks=wCount>>5;
		if((wCount&0x1f)==0)
			wNBlocks--;
		USB_COUNT_RX(bEpNum)=(uint32)((wNBlocks << 10) | 0x8000);
	}
	else
	{
		wNBlocks=wCount>>1;
		if((wCount&0x1)!=0)
			wNBlocks++;
		USB_COUNT_RX(bEpNum)=(uint32)(wNBlocks << 10);
	}
}

//设置接收端点状态
void SetEPRxStatus(uint8 bEpNum, uint16 wState)
{
	register uint16 _wRegVal;
	
	_wRegVal=USB_EPxR(bEpNum)&EPRX_DTOGMASK;
	//toggle first bit
	if((EPRX_DTOG1 & wState)!= 0)
		_wRegVal ^= EPRX_DTOG1;
	//toggle second bit
	if((EPRX_DTOG2 & wState)!= 0)
		_wRegVal ^= EPRX_DTOG2;
	USB_EPxR(bEpNum)=_wRegVal;
}

//设置发送端点状态
void SetEPTxStatus(uint8 bEpNum, uint16 wState)
{
	register uint16 _wRegVal;
	
	_wRegVal = USB_EPxR(bEpNum)&EPTX_DTOGMASK;
	//toggle first bit
	if((EPTX_DTOG1 & wState)!= 0)
		_wRegVal ^= EPTX_DTOG1;
	//toggle second bit
	if((EPTX_DTOG2 & wState)!= 0)
		_wRegVal ^= EPTX_DTOG2;
	USB_EPxR(bEpNum)=_wRegVal;
}

//设置设备地址
void SetDeviceAddress(uint8 Val)
{
	uint8 i;

	//设置每个端点的地址
	for(i=0;i<TOTAL_ENDP;i++)
		USB_EPxR(i)=USB_EPxR(i)&EPREG_MASK|i;
	//设置设备地址
	USB_DADDR=(Val|DADDR_EF);
}



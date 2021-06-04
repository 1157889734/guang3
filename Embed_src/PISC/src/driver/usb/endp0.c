//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 端点0处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:endp0.c

//*文件作用:端点0处理

//*文件作者:翟  鹏

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/



DEVICE_INFO Device_Info=
{
	0,//bmRequestType
	0,//bRequest
	0,//wValue
	0,//wIndex
	0,//wLength
	IN_DATA,//端点状态
	0,//Current_Feature
	0,//Current_Configuration
	0,//Current_Interface
	
	//ENDPOINT_INFO
	{
	0,//Usb_wLength
	0,//Usb_wOffset
	0,//PacketSize
	NULL,//uint8 * (*CopyData_callback)(uint16 Length);
	}
};

DEVICE_INFO *pInformation=&Device_Info;


	
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
DEVICE_PROP *pProperty=&myProperty;






//设备描述符长度定义
#define	MOUSE_SIZ_DEVICE_DESC	18
#define	MOUSE_SIZ_CONFIG_DESC	41
#define	MOUSE_SIZ_STRING_DESC	106
#define	MOUSE_SIZ_REPORT_DESC	27
#define MOUSE_OFF_HID_DESC	18
#define MOUSE_SIZ_HID_DESC	9


//设备描述符
const uint8 MOUSE_DeviceDescriptor[MOUSE_SIZ_DEVICE_DESC]=
{
	0x12,				// 设备描述符长度,固定=0x12
	0x01,				// 设备描述符类型,固定=0x01
	0x00,0x01,			// 协议版本,=1.10
	0x00,				// 设备类型
	0x00,				// 设备子类
	0x00,				// 设备协议
	0x08,				// 端点0最大数据包大小
	0xB4,0x04,			// 公司的设备ID
	0x00,0x00,			// 设备制造商定的产品ID
	0x88,0x02,			// 设备系列号
	4,				// 生产商在字符串描述符里的偏移量
	32,				// 产品名称在字符串描述符里的偏移量
	56, 				// 设备系列号在字符串描述符里的偏移量
	0x01				// 配置描述符的数量
};

//配置描述符
const uint8 MOUSE_ConfigDescriptor[MOUSE_SIZ_CONFIG_DESC]=
{
	0x09,				// 配置描述符的长度,固定=0x09
	0x02,				// 配置描述符的类型,固定=0x02
	MOUSE_SIZ_CONFIG_DESC,0x00,	// 配置描述符总长度
	0x01,				// 接口数量,1个接口
	0x01,				// 配置值
	84,				// 配置描述在字符串描述符里的偏移量
	0x40,				// 自供电
	0x32,				// 最大功耗(100mA)

//接口描述符
	0x09,				// 接口描述符长度,固定=0x09
	0x04,				// 接口描述符类型,固定=0x04
	0x00,				// 接口数,只有1个
	0x00,				// 可选配置,只有1个
	0x02,				// 除端点0之外的端点数目--In和out的总数
	0x03,				// 设备类型HID
	0x00,				// 子类代码1=BOOT, 0=no boot
	0x00,				// 协议代码: 0=none, 1=keyboard, 2=mouse
	84,				// 接口描述符在字符串描述符里的偏移量

//HID描述符
	0x09,				// HID描述符长度,固定=0x09
	0x21,				// HID描述符类型,固定=0x21
	0x00,0x01,			// bcdHID: HID Class Spec release number
	0x00,				// 国家代码
	0x01,				// 描述符数量
	0x22,				// 描述符类型
	MOUSE_SIZ_REPORT_DESC,0x00,	// 报告描述符的长度

//端点1发送描述符
	0x07,				// 端点描述符长度,固定=0x07
	0x05,				// 端点描述符类型,固定=0x05
	0x81,				// 端点地址---端点1的发送
	0x03,				// 中断传输方式,=0x03
	0x10,0x00,			// 端点1发送最大包的大小
	1,				// 传输间隔时间---1ms
	
//端点1接收描述符
	0x07,				// 端点描述符长度,固定=0x07	
	0x05,				// 端点描述符类型,固定=0x05
	0x01,				// 端点地址---端点1的接收
	0x03,				// 中断传输方式,=0x03
	0x10,0x00,			// 端点1接收最大包的大小
	1,				// 传输间隔时间---1ms
};

//字符串描述符
const uint8 MOUSE_StringDescriptor[MOUSE_SIZ_STRING_DESC]=
{
	0x04,
	0x03,
	0x09,
	0x04,		/* LangID = 0x0409: U.S. English */

	// 生产商: "天津市北海通信技术有限公司"
	28,		//Size of string
	0x03,		//bDescriptorType = String descriptor
	0x29,0x59, 0x25,0x6D, 0x02,0x5E, 0x17,0x53, 0x77,0x6D, 0x1A,0x90, 0xE1,0x4F, 
	0x80,0x62, 0x2F,0x67, 0x09,0x67, 0x50,0x96, 0x6C,0x51, 0xF8,0x53,
	
	// 产品名称: "PIDS系统中央控制器"
	24,		//Size of string
	0x03,		//bDescriptorType = String descriptor
	0x50,0x00, 0x49,0x00, 0x44,0x00, 0x53,0x00, 0xFB,0x7C, 0xDF,0x7E, 
	0x2D,0x4E, 0x2E,0x59, 0xA7,0x63, 0x36,0x52, 0x68,0x56,
	
	// 序列号: "20072000-0001"
	28,		//Size of string
	0x03,		//bDescriptorType = String descriptor
	0x32,0x00, 0x30,0x00, 0x30,0x00, 0x37,0x00, 0x30,0x00, 0x37,0x00, 
	0x30,0x00, 0x37,0x00, 0x2D,0x00, 0x30,0x00, 0x30,0x00, 0x30,0x00, 0x31,0x00,

	// 接口名称 0: "USB管理与配置端口"
	22,		//Size of string
	0x03,		//bDescriptorType = String descriptor
	0x55,0x00, 0x53,0x00, 0x42,0x00, 0xA1,0x7B, 0x06,0x74, 
	0x0E,0x4E, 0x4D,0x91, 0x6E,0x7F, 0xEF,0x7A, 0xE3,0x53,
};

//报告描述符---数据发送的数据结构
const uint8 MOUSE_ReportDescriptor[MOUSE_SIZ_REPORT_DESC]=
{
	0x05, 0x0c,			// 应用设备类型
	0x09, 0x01,			// 应用类型
	0x15, 0x81,			// 逻辑最小值
	0x25, 0x7f,			// 逻辑最大值
	
	0xa1, 0x01,			// 应用集合开始标志
	0x09, 0x30,			// 发送包应用类型
	0x75, 0x08,			// 发送单位大小 (8bit)
	0x95, EP1_MAXPACKETSIZE,	// 发送单位数量 (8*8bit)
	0x81, 0x02,			// 发送类型标志
	0x09, 0x30,			// 接收包应用类型
	0x75, 0x08,			// 接收单位大小 (8bit)
	0x95, EP1_MAXPACKETSIZE,	// 接收单位数量 (8*8bit)
	0x91, 0x02,			// 接收类型标志
	0xc0,		    		// 应用集合结束标志
};



//设备描述符处理
ONE_DESCRIPTOR Device_Descriptor =
{
	(uint8*)MOUSE_DeviceDescriptor,
	MOUSE_SIZ_DEVICE_DESC
};
uint8 *MOUSE_GetDeviceDescriptor(uint16 Length)
{
	return Standard_GetDescriptorData(Length,&Device_Descriptor);
}

//配置描述符处理
ONE_DESCRIPTOR Config_Descriptor=
{
	(uint8*)MOUSE_ConfigDescriptor,
	MOUSE_SIZ_CONFIG_DESC
};
uint8 *MOUSE_GetConfigDescriptor(uint16 Length)
{
	return Standard_GetDescriptorData( Length, &Config_Descriptor );
}

//字符串描述符处理
ONE_DESCRIPTOR String_Descriptor=
{
	(uint8*)MOUSE_StringDescriptor,
	MOUSE_SIZ_STRING_DESC
};
uint8 *MOUSE_GetStringDescriptor(uint16 Length)
{
	return Standard_GetStringDescriptor( Length, &String_Descriptor );
}

//HID描述符处理
ONE_DESCRIPTOR Mouse_Hid_Descriptor={
	(uint8 *)MOUSE_ConfigDescriptor+MOUSE_OFF_HID_DESC,
	MOUSE_SIZ_HID_DESC
};
uint8 *Mouse_GetHidDescriptor(uint16 Length)
{
	return Standard_GetDescriptorData( Length, &Mouse_Hid_Descriptor );
}

//报告描述符处理
ONE_DESCRIPTOR Mouse_Report_Descriptor=
{
	(uint8 *)MOUSE_ReportDescriptor,
	MOUSE_SIZ_REPORT_DESC
};
uint8 *Mouse_GetReportDescriptor(uint16 Length)
{
	return Standard_GetDescriptorData( Length, &Mouse_Report_Descriptor );
}

//主机设置处理
uint8 MOUSE_Data_Setup(uint8 RequestNo)
{
	DEVICE_INFO *pInfo = &Device_Info;
	uint8	*(*CopyRoutine)(uint16);
	uint8 *pbLen;
	uint16 wLen;

	CopyRoutine = NULL;
	if(RequestNo == GET_CONFIGURATION)
	{
		if(Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))CopyRoutine = Standard_GetConfiguration;
	}
	else if(RequestNo == GET_INTERFACE)
	{
		if(Type_Recipient==(STANDARD_REQUEST|INTERFACE_RECIPIENT))CopyRoutine = Standard_GetInterface;
	}
	else if(RequestNo==GET_DESCRIPTOR && Type_Recipient==(STANDARD_REQUEST|INTERFACE_RECIPIENT))
	{
		if(pInfo->USBwIndex0!=0)return UNSUPPORT;

		switch(pInfo->USBwValue1)
		{
			case HID_DESCRIPTOR:
				CopyRoutine = Mouse_GetHidDescriptor;
				break;
				
			case REPORT_DESCRIPTOR:
				CopyRoutine = Mouse_GetReportDescriptor;
				break;
			
			default:
				return UNSUPPORT;
		}
	}
	else
	{
		return UNSUPPORT;
	}
	
	if(CopyRoutine==NULL)return UNSUPPORT;

	pInfo->Ctrl_Info.CopyData = CopyRoutine;
	pInfo->Ctrl_Info.Usb_wOffset = 0;
	pbLen = (*CopyRoutine)(0);
	wLen = (uint16)((uint32)pbLen);
	pInfo->Ctrl_Info.Usb_wLength = wLen;
	return SUCCESS;

}

//自定义设备处理接口
DEVICE_PROP myProperty=
{
	MOUSE_Data_Setup,
	MOUSE_GetDeviceDescriptor,
	MOUSE_GetConfigDescriptor,
	MOUSE_GetStringDescriptor,
	0,/*MOUSE_EP0Buffer*/
	STD_MAXPACKETSIZE
};






#define ValBit(VAR,Place)    (VAR & (1<<Place))
#define SetBit(VAR,Place)    ( VAR |= (1<<Place) )
#define ClrBit(VAR,Place)    ( VAR &= ((1<<Place)^255) )

uint8 *Standard_GetStatus(uint16 Length);
uint8 Standard_ClearFeature(void);
uint8 Standard_SetFeature(void);


/* cells saving status during interrupt servicing */
uint16 SaveRState;
uint16 SaveTState;



//高低字节交换
uint16 ByteSwap(uint16 wSwW)
{
	uint8 bTemp;
	uint16 wRet;
	bTemp = (uint8)(wSwW & 0xff);
	wRet =  (wSwW >> 8) | ((uint16)bTemp << 8);
	return(wRet);
} 

uint8 *Standard_GetConfiguration(uint16 Length)
{
	if (Length == 0)
		return (uint8 *)sizeof(pInformation->Current_Configuration);
	return (uint8 *)&pInformation->Current_Configuration;
}

uint8 Standard_SetConfiguration(void)
{
	uint8 wValue0 = pInformation->USBwValue0;
	if (wValue0<=TOTAL_CONFIG_ENDP)
	{
		/* If the number of configuration is within the range */
		pInformation->Current_Configuration = wValue0;
		return SUCCESS;
	}
	else
		return UNSUPPORT;
}

uint8 *Standard_GetInterface(uint16 Length)
{
	if (Length == 0)
		return (uint8 *)sizeof(pInformation->Current_Interface);

	return (uint8 *)&pInformation->Current_Interface;
}

uint8 Standard_SetInterface(void)
{
	DEVICE_INFO *pInfo = pInformation;
	if (pInfo->USBwValue0 != 0 || pInfo->USBwIndex0 != 0)
		return UNSUPPORT;

	/* I have only one interface & one alternative setting */
	pInfo->Current_Interface = 0;
	return SUCCESS;
}

uint16_uint8 StatusInfo;
#define StatusInfo0 StatusInfo.bw.bb1	/* Reverse bb0 & bb1 */
#define StatusInfo1 StatusInfo.bw.bb0
uint8 *Standard_GetStatus(uint16 Length)
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	Type_Rec;

	if(Length == 0)
		return (uint8 *)2;

	StatusInfo.w = 0;/* Reset Status Information */

	Type_Rec = Type_Recipient;
	if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
	{
		uint8 Feature = pInfo->Current_Feature;

		if (ValBit(Feature, 5))SetBit(StatusInfo0, 1); /* Remote Wakeup enabled */

		if (ValBit(Feature, 7));	/*	ClrBit(StatusInfo0, 0); */ /* Bus-powered */
		else if (ValBit(Feature, 6))SetBit(StatusInfo0, 0);  /* Self-powered */
	}
	else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
	{
		uint8	Related_Endpoint;
		uint8	wIndex0 = pInfo->USBwIndex0;

		Related_Endpoint = ENDP0 + (wIndex0 & 0x0f);
		if (ValBit(wIndex0, 7))
		{
			/* IN endpoint */
			if (((uint16)USB_EPxR(Related_Endpoint)&EPTX_STAT)==EP_TX_STALL)SetBit(StatusInfo0, 0);	/* IN Endpoint stalled */
		}
		else
		{
			/* OUT endpoint */
			if (((uint16)USB_EPxR(Related_Endpoint)&EPRX_STAT)==EP_RX_STALL)SetBit(StatusInfo0, 0);	/* OUT Endpoint stalled */
		}

	}
	else
	{
		return NULL;
	}
	
	return (uint8 *)&StatusInfo;
}

uint8 Standard_ClearFeature(void)
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	Type_Rec = Type_Recipient;
	uint8	wValue0 = pInfo->USBwValue0;

	if ( Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT) ) {
		if (wValue0 != DEVICE_REMOTE_WAKEUP)
			return UNSUPPORT;

		ClrBit(pInfo->Current_Feature, 5);
		return SUCCESS;
	}
	else if ( Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT) )
	{
		uint8	Related_Endpoint;
		uint8	wIndex0;
		uint8	rEP;

		if (wValue0 != ENDPOINT_STALL)return UNSUPPORT;

		wIndex0 = pInfo->USBwIndex0;
		rEP = wIndex0 & ~0x80;
		if(rEP>=TOTAL_ENDP)return UNSUPPORT;

		//从基地址ENDP0开始的偏移量
		Related_Endpoint = ENDP0 + rEP;
		if (wIndex0 & 0x80)
		{		/* IN endpoint */
			if (((uint16)USB_EPxR(Related_Endpoint)&Related_Endpoint)==EP_TX_STALL)SetEPTxStatus(Related_Endpoint, EP_TX_NAK);
		}
		else
		{							/* OUT endpoint */
			if (((uint16)USB_EPxR(Related_Endpoint)&EPRX_STAT)==EP_RX_STALL)
			{
				if (Related_Endpoint == ENDP0)
				{
					/* After clear the STALL, enable the default endpoint receiver */
					SetEPRxCount(Related_Endpoint, STD_MAXPACKETSIZE);
					SetEPRxStatus(Related_Endpoint, EP_RX_VALID);//启动接收
				}
				else
				{
					SetEPRxStatus(Related_Endpoint, EP_RX_NAK);
				}
			}
		}
		return SUCCESS;
	}

	return UNSUPPORT;
}

uint8 Standard_SetFeature(void)
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	Type_Rec = Type_Recipient;
	uint8	wValue0 = pInfo->USBwValue0;

	if( Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
	{
		if (wValue0 != DEVICE_REMOTE_WAKEUP)
			return UNSUPPORT;

		SetBit(pInfo->Current_Feature, 5);
		return SUCCESS;
	}
	else if(Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
	{
		uint8	Related_Endpoint;
		uint8	wIndex0;
		uint8	rEP;

		if (wValue0 != ENDPOINT_STALL)
			return UNSUPPORT;

		wIndex0 = pInfo->USBwIndex0;
		rEP=wIndex0&~0x80;
		if (rEP>=TOTAL_ENDP)
			return UNSUPPORT;

		Related_Endpoint = ENDP0 + rEP;
		if (wIndex0 & 0x80)/* IN endpoint */
		{		
			SetEPTxStatus(Related_Endpoint, EP_TX_STALL);
		}
		else		/* OUT endpoint */
		{	
			SetEPRxStatus(Related_Endpoint, EP_RX_STALL);
		}

		return SUCCESS;
	}

	return UNSUPPORT;
}

uint8 *Standard_GetStringDescriptor(uint16 Length, ONE_DESCRIPTOR *pDesc)
{
	int len, offset, wOffset;

	wOffset = pInformation->Ctrl_Info.Usb_wOffset;

	if(Length == 0)
	{
		offset = 0;
		do
		{
			len = (int)*(pDesc->Descriptor + offset);
			if (wOffset >= 0 && wOffset < len)
			{
				len -= wOffset;
				if(len > 0)
					return (uint8*)len;
				break;
			}
			wOffset -= len;
			offset += len;
		}while(offset<pDesc->Descriptor_Size);
		return 0;
	}
	
	return pDesc->Descriptor + wOffset;
}

uint8 *Standard_GetDescriptorData(uint16 Length, ONE_DESCRIPTOR *pDesc)
{
	int len, wOffset;

	wOffset = pInformation->Ctrl_Info.Usb_wOffset;
	if (Length == 0)
	{
		len = pDesc->Descriptor_Size - wOffset;
		if (len <= 0)
			return 0;
		return (uint8 *)len;
	}
	return pDesc->Descriptor + wOffset;
}

void DataStageOut(void)
{
	ENDPOINT_INFO	*pEPinfo = &pInformation->Ctrl_Info;
	uint16	save_rLength;

	save_rLength = pEPinfo->Usb_rLength;

	if (pEPinfo->CopyData && save_rLength)
	{
		uint8 *Buffer;
		uint16 Length;
		uint8 *Source;

		Length = pEPinfo->PacketSize;
		if (Length > save_rLength)
			Length = save_rLength;

		Buffer = (*pEPinfo->CopyData)(Length);

		pEPinfo->Usb_rLength -= Length;
		pEPinfo->Usb_rOffset += Length;

		Source = USB_BUFADDR + (uint8 *)((uint16)USB_ADDR_RX(ENDP0)*2); /* *2 for 32 bits addr */
		while (Length)
		{
			*Buffer++ = *Source++;
			Length--;
			if(Length == 0) 
				break; /* odd counter */
			*Buffer++ = *Source++;
			Length--;
			Source++;Source++; /* skip 2 bytes for 32 bit addressing */
		}
	}

	if (pEPinfo->Usb_rLength == 0) {
		/* this OUT Transaction is the last one */
		pInformation->ControlState =
			(save_rLength == pEPinfo->PacketSize) ?
					/*	If the previous read length is same as MaxPacketSize
						This is the multiple MaxPacketSize package
						Wait for another OUT token and send 0 length data
					*/
				WAIT_OUT_ZERO :
					/*	If the previous read length is NOT same as MaxPacketSize
						Wait for the IN token to finish the status stage
					*/
				LAST_OUT_DATA;
	}
}

void DataStageIn(void)
{
	ENDPOINT_INFO	*pEPinfo = &pInformation->Ctrl_Info;
	uint16	save_wLength = pEPinfo->Usb_wLength;
	uint8	ControlState;

	uint8	*DataBuffer;
	uint16	Length;
	int i;
	uint32 *pTxBuff;
	uint16 wTra;
 	union
 	{
		uint8 *bTra;
    		uint16 *wTra;
	}pBuf;

	if (save_wLength == 0)
	{
		/* if the number of byte to be sent is				*/
		/* multiple of MaxPacketSize: send a 0 length data packet	*/
		ControlState = WAIT_IN_ZERO;
		USB_COUNT_TX(ENDP0)=0;
		SaveTState=(EP_TX_VALID);
		goto Expect_Status_Out;
	}

	Length = pEPinfo->PacketSize;
	ControlState = (save_wLength < Length) ? LAST_IN_DATA : IN_DATA;

	/* Same as UsbWrite */
	if (Length > save_wLength)Length = save_wLength;

	DataBuffer = (*pEPinfo->CopyData)(Length);
	/* transfer data from buffer to PMA */
	pTxBuff = (uint32 *)(USB_BUFADDR + (uint8 *)((uint16)USB_ADDR_TX(ENDP0)*2));
	pBuf.wTra = &wTra;
	for(i=0;i < Length;)
	{
		*(pBuf.bTra  ) = *DataBuffer++;
		i++;
		*(pBuf.bTra+1) = *DataBuffer++;
		i++;
		*pTxBuff = wTra;
		pTxBuff++;
	}

	USB_COUNT_TX(ENDP0)=Length;
	pEPinfo->Usb_wLength -= Length;
	pEPinfo->Usb_wOffset += Length;
	SaveTState=(EP_TX_VALID);

Expect_Status_Out:
	SaveRState=EP_RX_VALID;	/* Expect the host to abort the data IN stage */
	pInformation->ControlState = ControlState;
}

void NoData_Setup0()
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	Result;
	uint8	RequestNo = pInfo->USBbRequest;
	uint8	ControlState;

	if (RequestNo == CLEAR_FEATURE)Result = Standard_ClearFeature();
	else if (RequestNo == SET_FEATURE)Result = Standard_SetFeature();
	else if (RequestNo == SET_ADDRESS && Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))Result = SUCCESS;
	else Result = UNSUPPORT;

	//标准请求
	if(Result != SUCCESS)
	{
		if((pInfo->USBbmRequestType & (REQUEST_TYPE | RECIPIENT))==(STANDARD_REQUEST|DEVICE_RECIPIENT))
		{
			if(RequestNo==SET_CONFIGURATION)Result= Standard_SetConfiguration();
		}
		else if((pInfo->USBbmRequestType & (REQUEST_TYPE | RECIPIENT)) == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
		{
			if (RequestNo == SET_INTERFACE)Result= Standard_SetInterface();
		}
		else
		{
			Result = UNSUPPORT;
		}
		
		if (Result == NOT_READY)
		{
			ControlState = PAUSE;
			goto exit_NoData_Setup0;
		}
	}
	if (Result != SUCCESS)
	{
		ControlState = STALLED;
		goto exit_NoData_Setup0;
	}

	ControlState = WAIT_STATUS_IN;	/* After no data stage SETUP */
	USB_COUNT_TX(ENDP0)=0;
	SaveTState=(EP_TX_VALID);

exit_NoData_Setup0:
	pInformation->ControlState = ControlState;
	return;
}

void Data_Setup0()
{
	DEVICE_INFO *pInfo = pInformation;
	DEVICE_PROP *pProp = pProperty;
	uint8	*(*CopyRoutine)(uint16);
	uint16	wOffset;
	uint8	Result;
	uint8 Request_No = pInfo->USBbRequest;
	uint8 *pbLen;
	uint16 wLen;

	CopyRoutine = NULL;
	wOffset = 0;

	if (Request_No == GET_DESCRIPTOR)
	{
		if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
		{
			uint8 wValue1 = pInfo->USBwValue1;
			if (wValue1 == DEVICE_DESCRIPTOR)
			{	
				CopyRoutine = pProp->GetDeviceDescriptor;
			}
			else if (wValue1 == CONFIG_DESCRIPTOR)
			{
				CopyRoutine = pProp->GetConfigDescriptor;
			}
			else if (wValue1 == STRING_DESCRIPTOR)
			{
				wOffset = pInfo->USBwValue0;
				CopyRoutine = pProp->GetStringDescriptor;
			}
		}
	}
	else if (Request_No == GET_STATUS)
	{
		if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT) || Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
		{
			CopyRoutine = Standard_GetStatus;
		}
	}
	
	if (CopyRoutine)
	{
		pInfo->Ctrl_Info.Usb_wOffset = wOffset;
		pInfo->Ctrl_Info.CopyData = CopyRoutine;

		pbLen = (*CopyRoutine)(0);
		wLen = (uint16)((uint32)pbLen);
		pInfo->Ctrl_Info.Usb_wLength = wLen;
		Result = SUCCESS;
	}
	else
	{
		Result = (*pProp->Class_Data_Setup)(pInfo->USBbRequest);
		if (Result == NOT_READY)
		{
			pInfo->ControlState = PAUSE;
			return;
		}
	}

	if (pInfo->Ctrl_Info.Usb_wLength == 0xffff)
	{	/* Data is not ready, wait it */
		pInfo->ControlState = PAUSE;
		return;
	}
	if (Result == UNSUPPORT || pInfo->Ctrl_Info.Usb_wLength == 0)
	{
		/* Unsupported request */
		pInfo->ControlState = STALLED;
		return;
	}

	if (ValBit(pInfo->USBbmRequestType, 7))
	{
		/* Device ==> Host */
		uint16	wLength = pInfo->USBwLength;

		/* Restrict the data length to be the one host asks */
		if (pInfo->Ctrl_Info.Usb_wLength > wLength)pInfo->Ctrl_Info.Usb_wLength = wLength;
		pInfo->Ctrl_Info.PacketSize = pProp->MaxPacketSize;
		DataStageIn();
	}
	else
	{
		pInfo->ControlState = OUT_DATA;
		SaveRState = EP_RX_VALID;
	}

	return;
}

uint8 Setup0_Process()
{
	DEVICE_INFO *pInfo = pInformation;
	union {
		uint8*	b;
		uint16*	w;
	} pBuf;

	pBuf.b = USB_BUFADDR + (uint8 *)((uint16)USB_ADDR_RX(ENDP0)*2);//2 for 32 bits addr
	if (pInfo->ControlState != PAUSE)
	{
		pInfo->USBbmRequestType=*pBuf.b++;	/* bmRequestType */
		pInfo->USBbRequest= *pBuf.b++;	/* bRequest */
		pBuf.w++;  /* sb word not accessed because of 32 bits addressing */
		pInfo->USBwValue=ByteSwap(*pBuf.w++);	/* wValue */
		pBuf.w++;  /* word not accessed because of 32 bits addressing */
		pInfo->USBwIndex= ByteSwap(*pBuf.w++);	/* wIndex */
		pBuf.w++;  /* word not accessed because of 32 bits addressing */
/* sb		pInfo->USBwLength		= ByteSwap(*pBuf.w);*/	/* wLength */
		pInfo->USBwLength		= *pBuf.w;	/* wLength */
	}

	pInfo->ControlState=SETTING_UP;
	if (pInfo->USBwLength == 0)
	{	/* No data statge processing */
		NoData_Setup0();
	}
	else
	{	/* Setup with data stage */
		Data_Setup0();
	}
	return Post0_Process();
}

uint8 In0_Process()
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	ControlState = pInfo->ControlState;

	if(ControlState == IN_DATA)
	{
		DataStageIn();
		ControlState = pInfo->ControlState; /* may be changed outside the function */
	}
	else if (ControlState == LAST_IN_DATA || ControlState == WAIT_IN_ZERO)
	{
		ControlState = WAIT_STATUS_OUT;
		SaveRState=EP_RX_VALID;
	}
	else if (ControlState == WAIT_OUT_ZERO || ControlState == WAIT_STATUS_IN)
	{
		if (pInfo->USBbRequest == SET_ADDRESS && Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
		{
			/* Device address must be written */
			/* after completion of Status Stage (ACK from Host) */
			SetDeviceAddress(pInfo->USBwValue0);
		}
		ControlState = WAIT_SETUP;
	}
	else
	{
		ControlState = STALLED;
	}
	
	pInfo->ControlState = ControlState;

	return Post0_Process();
}

uint8 Out0_Process()
{
	DEVICE_INFO	*pInfo = pInformation;
	uint8	ControlState = pInfo->ControlState;

	if (ControlState == OUT_DATA)
	{
		DataStageOut();

		if (pInfo->ControlState == LAST_OUT_DATA)
		{
			ControlState = WAIT_STATUS_IN;
			USB_COUNT_TX(ENDP0)=0;
			SaveTState=(EP_TX_VALID);
		}
		else
		{
			/* Expecting another OUT token with 0 length data */
			SetEPRxCount(ENDP0, 0);
			SaveRState=EP_RX_VALID;

			/* Also expecting an IN token to finish the transaction */
			USB_COUNT_TX(ENDP0)=0;
			SaveTState=(EP_TX_VALID);
		}
	}
	else if (ControlState == WAIT_STATUS_OUT || ControlState == IN_DATA)
	{
		SaveTState=(EP_TX_NAK);
		ControlState = WAIT_SETUP;
	}
	else if (ControlState == WAIT_OUT_ZERO)
	{
		ControlState = WAIT_STATUS_IN;
		USB_COUNT_TX(ENDP0)=0;
		SaveTState=(EP_TX_VALID);
	}
	else
	{
		//未知state, STALL the endpoint
		ControlState=STALLED;
	}

	pInfo->ControlState = ControlState;

	return Post0_Process();
} 

uint8 Post0_Process(void)
{
	SetEPRxCount(ENDP0, STD_MAXPACKETSIZE);

	if (pInformation->ControlState == STALLED)
	{
		SaveRState=EP_RX_STALL;
		SaveTState=EP_TX_STALL;
	}

	return (pInformation->ControlState == PAUSE);
}





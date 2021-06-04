//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 端点0处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:endp0.h

//*文件作用:端点0处理

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef ENDP0_H
#define ENDP0_H



//USB全局请求类型
#define REQUEST_TYPE		0x60	/* Mask to get request type */
#define STANDARD_REQUEST	0x00
#define	CLASS_REQUEST		0x20
#define	VENDOR_REQUEST		0x40

//recipient类型
#define RECIPIENT		0x1F	/* Mask to get recipient */
#define DEVICE_RECIPIENT	0
#define INTERFACE_RECIPIENT	1
#define ENDPOINT_RECIPIENT	2
#define OTHER_RECIPIENT		3

//11种标准请求类型
#define GET_STATUS		0
#define CLEAR_FEATURE		1
#define RESERVED1		2
#define SET_FEATURE		3
#define RESERVED2		4
#define SET_ADDRESS		5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11

//标准描述符类型
#define DEVICE_DESCRIPTOR		1
#define CONFIG_DESCRIPTOR		2
#define STRING_DESCRIPTOR		3
#define INTERFACE_DESCRIPTOR		4
#define ENDPOINT_DESCRIPTOR		5

//Feature selector of a SET_FEATURE or CLEAR_FEATURE
#define ENDPOINT_STALL			0
#define DEVICE_REMOTE_WAKEUP		1

//HID设备描述符定义
#define HID_DESCRIPTOR		0x21
#define REPORT_DESCRIPTOR	0x22
#define PHYSICAL_DESCRIPTOR	0x23

//HID设备请求定义
#define GET_REPORT		1
#define GET_IDLE		2
#define GET_PROTOCOL		3
#define SET_REPORT		9
#define SET_IDLE		10
#define SET_PROTOCOL		11

//HID设备报告定义
#define INPUT_REPORT		1
#define OUTPUT_REPORT		2
#define FEATURE_REPORT		3



//端点号
#define ENDP0	 ((uint8)0)
#define ENDP1	 ((uint8)1)
#define ENDP2	 ((uint8)2)
#define ENDP3	 ((uint8)3)
#define ENDP4	 ((uint8)4)
#define ENDP5	 ((uint8)5)
#define ENDP6	 ((uint8)6)
#define ENDP7	 ((uint8)7)
#define ENDP8	 ((uint8)8)
#define ENDP9	 ((uint8)9)
#define ENDP10	 ((uint8)10)
#define ENDP11	 ((uint8)11)
#define ENDP12	 ((uint8)12)
#define ENDP13	 ((uint8)13)
#define ENDP14	 ((uint8)14)
#define ENDP15	 ((uint8)15)




#define Type_Recipient	(pInfo->USBbmRequestType & (REQUEST_TYPE | RECIPIENT))

//端点状态定义
#define WAIT_SETUP		0
#define SETTING_UP		1
#define IN_DATA			2
#define OUT_DATA		3
#define LAST_IN_DATA		4
#define LAST_OUT_DATA		5
#define WAIT_IN_ZERO		6
#define WAIT_OUT_ZERO		7
#define WAIT_STATUS_IN		8
#define WAIT_STATUS_OUT		9
#define STALLED			10
#define PAUSE			11


//描述符结构定义
typedef struct OneDescriptor
{
	uint8 *Descriptor;
	uint16 Descriptor_Size;
}ONE_DESCRIPTOR, *PONE_DESCRIPTOR;


//处理结果定义
#define SUCCESS		0
#define ERROR		1
#define UNSUPPORT	2
#define NOT_READY	3



/*-*-*-*-*-*-*-*-*-*-* Definitions for endpoint level -*-*-*-*-*-*-*-*-*-*-*-*/

typedef struct _ENDPOINT_INFO
{
	uint16		Usb_wLength;
	uint16		Usb_wOffset;
	uint16		PacketSize;
	uint8 		*(*CopyData)(uint16 Length);
}ENDPOINT_INFO;

#define Usb_rLength Usb_wLength
#define Usb_rOffset Usb_wOffset


/*-*-*-*-*-*-*-*-*-*-*-* Definitions for device level -*-*-*-*-*-*-*-*-*-*-*-*/

typedef struct _DEVICE
{
/*sb	uint8 EP0;	*/			 /*	Endpoint number */
	uint8 Total_Endpoint;	 /* Number of endpoints that are used */
	uint8 Total_Configuration;/* Number of configuration available */
}DEVICE;

typedef union
{
	uint16	w;
	struct BW
	{
		uint8	bb1;
		uint8	bb0;
	}bw;
}uint16_uint8;

typedef struct _DEVICE_INFO
{
	uint8		USBbmRequestType;		/* bmRequestType */
	uint8		USBbRequest;			/* bRequest */
	uint16_uint8	USBwValues;			/* wValue */
	uint16_uint8	USBwIndexs;			/* wIndex */
	uint16_uint8	USBwLengths;			/* wLength */

	uint8		ControlState;			/* of type CONTROL_STATE */
	uint8		Current_Feature;

	uint8		Current_Configuration;
	uint8		Current_Interface;
	/* Selected interface of current configuration */
	ENDPOINT_INFO	Ctrl_Info;
} DEVICE_INFO;
extern DEVICE_INFO Device_Info;


//自定义设备专用处理函数接口
typedef struct _DEVICE_PROP
{
	uint8 (*Class_Data_Setup)(uint8 RequestNo);
	uint8* (*GetDeviceDescriptor)(uint16 Length);
	uint8* (*GetConfigDescriptor)(uint16 Length);
	uint8* (*GetStringDescriptor)(uint16 Length);
	uint8*	RxEP_buffer;
	uint16	MaxPacketSize;

}DEVICE_PROP;
extern DEVICE_PROP myProperty;



/* cells saving status during interrupt servicing */
extern uint16 SaveRState;
extern uint16 SaveTState;

#define	USBwValue	USBwValues.w
#define	USBwValue0	USBwValues.bw.bb0
#define	USBwValue1	USBwValues.bw.bb1
#define	USBwIndex	USBwIndexs.w
#define	USBwIndex0	USBwIndexs.bw.bb0
#define	USBwIndex1	USBwIndexs.bw.bb1
#define	USBwLength	USBwLengths.w
#define	USBwLength0	USBwLengths.bw.bb0
#define	USBwLength1	USBwLengths.bw.bb1

extern uint8 Setup0_Process(void);
extern uint8 Post0_Process(void);
extern uint8 Out0_Process(void);
extern uint8 In0_Process(void);

extern uint8 *Standard_GetConfiguration(uint16 Length);
extern uint8 Standard_SetConfiguration(void);
extern uint8 *Standard_GetInterface(uint16 Length);
extern uint8 Standard_SetInterface(void);
extern uint8 *Standard_GetDescriptorData(uint16 Length, PONE_DESCRIPTOR pDesc);
extern uint8 *Standard_GetStringDescriptor(uint16 Length, PONE_DESCRIPTOR pDesc);



#endif



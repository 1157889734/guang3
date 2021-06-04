 #include "StdAfx.h"
#include "HidUSB.h"
#include <Setupapi.h>
#include "usbhead.h"

extern "C"
{
#include "hidsdi.h"
};

#define MAX_DEVICE 20 

CHidUSB::CHidUSB(void)
{
	m_hRead = NULL;          //读管道句柄
	m_hWrite = NULL;         //写管道句柄
    m_hRecvThread = NULL;    //接收数据线程句柄
}

CHidUSB::CHidUSB(CWnd* pWnd, const unsigned short sVID, const unsigned short sPID)
{
	m_pOwnerWnd  = pWnd;
	m_sDeviceVID = sVID;
	m_sDevicePID = sPID;
}

CHidUSB::~CHidUSB(void)
{
	m_hRead = NULL;          //读管道句柄
	m_hWrite = NULL;         //写管道句柄
	m_hRecvThread = NULL;    //接收数据线程句柄
}

/*********************************************************************************************************
** 函数名称: OpenDevice
** 功能描述: 打开所需要的USB设备,成功则创建读写管道
** 输　入: 无
** 输　出: BOOL
** 全局变量: m_hRead：读管道句柄，m_hWrite：写管道句柄
m_sDeviceVID；所需打开设备VID，m_sDevicePID：所需打开设备PID
m_nBytesToRead：可读USB报告长度，m_nBytesToWrite：可写USB报告长度
********************************************************************************************************/
BOOL CHidUSB::OpenDevice()
{
	GUID            hidGuid;
	HDEVINFO        hDevInfo;
	HANDLE          hidHandle;

	//获取HID类GUID
	HidD_GetHidGuid(&hidGuid);

	//获取HID设备类信息
	hDevInfo = SetupDiGetClassDevs(&hidGuid,
		NULL,
		NULL,	
		(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		//MessageBox(0,"SetupDiGetClassDevs Failed!","error",0);
		return FALSE;
	}

	SP_DEVICE_INTERFACE_DATA devInfoData;
	devInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

	for (int i = 0; i < MAX_DEVICE; i++)
	{
		//if(!SetupDiEnumDeviceInfo(hDevInfo, i, NULL))
		//{
		//	//MessageBox(0,"SetupDiEnumDeviceInfo Failed!","error",0);
		//	return FALSE;
		//}

		//枚举设备接口
		if (!SetupDiEnumInterfaceDevice (hDevInfo, 0, &hidGuid, i, &devInfoData))
		{
			//MessageBox(0,"SetupDiEnumInterfaceDevice Failed!","error",0);
			continue;
		} 
		else
		{
			ULONG  reqLen = 0;

			//获取设备接口详细信息数据长度
			SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData, 
				NULL,
				0,
				&reqLen, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (reqLen);       

			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			//获取设备接口详细信息
			if(!SetupDiGetInterfaceDeviceDetail(hDevInfo, &devInfoData, devDetail, reqLen, NULL, NULL))
			{
				//free(devDetail);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			hidHandle = CreateFile(devDetail->DevicePath, 
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);

			if (hidHandle==INVALID_HANDLE_VALUE)
			{
				//free(devDetail);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			_HIDD_ATTRIBUTES hidAttributes;

			//获取属性
			if(!HidD_GetAttributes(hidHandle, &hidAttributes))
			{
				//free(devDetail);
				//CloseHandle(hidHandle);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			//根据VID，PID判断是否是所要的设备
			if (m_sDeviceVID == hidAttributes.VendorID  
				&& m_sDevicePID  == hidAttributes.ProductID)
			{
				//HidD_SetNumInputBuffers()

				char szRead[MAX_PATH], szWrite[MAX_PATH];
				//WCHAR szRead[MAX_PATH], szWrite[MAX_PATH];


				//宽字节转单字节
				char *pcstr = (char *)malloc(sizeof(char)*(2 * wcslen(devDetail->DevicePath)+1));
				memset(pcstr , 0 , 2 * wcslen(devDetail->DevicePath)+1 );

				int nlength=wcslen(devDetail->DevicePath);

				//获取转换后的长度
				int nbytes = WideCharToMultiByte( 0, 0, devDetail->DevicePath,  nlength,  NULL, 0, NULL,  NULL );   

				int len = 2 * wcslen(devDetail->DevicePath)+1;
				if(nbytes>len)   nbytes=len;

				// 通过以上得到的结果，转换unicode 字符为ascii 字符
				WideCharToMultiByte( 0, 0, devDetail->DevicePath, nlength, pcstr, nbytes,  NULL,  NULL );

				strncpy( szRead, pcstr, sizeof(szRead) );
				strncpy( szWrite, pcstr, sizeof(szWrite) );

				strcat (szRead, "\\" );
				strcat (szWrite, "\\" );

				strcat(szRead, "PIPE_00");
				strcat(szWrite, "PIPE_01");

				m_hRead = CreateFile(CString(szRead),
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/,
					NULL);

				m_hWrite = CreateFile(CString(szWrite),
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL /*| FILE_FLAG_OVERLAPPED*/,
					NULL);


				if(m_hRead == INVALID_HANDLE_VALUE || m_hWrite == INVALID_HANDLE_VALUE)
				{
					//MessageBox(0,"Create read/write failed!","error",0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				PHIDP_PREPARSED_DATA preparsed_data;
				HIDP_CAPS hidPCaps;

				// Prepare preparsed data for read
				if(!HidD_GetPreparsedData(m_hRead, &preparsed_data))
				{
					//MessageBox(0, "Failed to get pre-parsed data for read!","error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				if(HidP_GetCaps(preparsed_data, &hidPCaps) != HIDP_STATUS_SUCCESS)
				{
					//MessageBox(0, "Failed to get capabilities for read!", "error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				m_nBytesToRead = hidPCaps.InputReportByteLength;

				if(!HidD_FreePreparsedData(preparsed_data))
				{
					//MessageBox(0, "Failed to release pre-parsed data resource!", "error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				// Prepare preparsed data for write
				if(!HidD_GetPreparsedData(m_hWrite, &preparsed_data))
				{
					//MessageBox(0, "Failed to get pre-parsed data for write!", "error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				if(HidP_GetCaps(preparsed_data, &hidPCaps) != HIDP_STATUS_SUCCESS)
				{
					//MessageBox(0, "Failed to get capabilities for write!", "error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				m_nBytesToWrite = hidPCaps.OutputReportByteLength;

				if(!HidD_FreePreparsedData(preparsed_data))
				{
					//MessageBox(0, "Failed to release pre-parsed data resource!", "error", 0);
					if(pcstr != NULL)
					{
						free(pcstr);
						pcstr = NULL;
					}
					if(devDetail != NULL)
					{
						free(devDetail);
						devDetail = NULL;
					}
					return FALSE;
				}

				//// Create output report
				//HIDP_VALUE_CAPS valueCaps;			// special value capabilities including ReportID

				//if(!HidP_GetValueCaps(HidP_Output, &valueCaps, &hidPCaps.NumberOutputValueCaps, preparsed_data))
				//{
				//	//MessageBox(0, "Failed to get special value capabilities!", "error", 0);
				//	return FALSE;
				//}

				if(devDetail != NULL)
				{
					free(devDetail);
					devDetail = NULL;
				}
				CloseHandle(hidHandle);
				SetupDiDestroyDeviceInfoList(hDevInfo);

				DWORD id;
				m_hRecvThread = CreateThread(NULL, 0, RecvDataThread, this, CREATE_SUSPENDED, &id);	//创建USB数据读线程
				ResumeThread(m_hRecvThread);

				if(pcstr != NULL)
				{
					free(pcstr);
					pcstr = NULL;
				}
				return TRUE;
			}
			if(devDetail != NULL)
			{
				free(devDetail);
				devDetail = NULL;
			}
			CloseHandle(hidHandle);
			SetupDiDestroyDeviceInfoList(hDevInfo);
		}
	}
	//CloseHandle(hidHandle);
	//SetupDiDestroyDeviceInfoList(hDevInfo);
	return FALSE;
}



/*********************************************************************************************************
** 函数名称: SendData
** 功能描述: 发送数据
** 输　入: LPUSB_DATAPACK pPack，USB数据包
** 输　出: BOOL
** 全局变量: m_hWrite：写管道句柄
m_nBytesToWrite：可写USB报告长度
********************************************************************************************************/
BOOL CHidUSB::SendData(LPUSB_DATAPACK pPack)
{
	if (m_nBytesToWrite == 0)
	{
		//MessageBox(0, "m_nBytesToWrite == 0!", "error", 0, 0);
		return FALSE;
	}

	//目前根据USBTrace抓到的数据包分析得到的USB报告结果（有待查证）：
	//1.用WriteFile写数据只能发送固定长度的数据，其长度可根据HidD_GetPreparsedData函数来确定；
	//2.第一个字节需为0x00才能发送成功（报告ID？）；
	//3.第二个字节为所发送的数据长度，第三个字节一直为0x00，不知代表什么；
	//4.第四个字节开始为要发送的数据；
	//5.综上，每次发送的数据长度为用HidD_GetPreparsedData获取的解析长度减3。
	BYTE *outReport = (BYTE *)malloc(m_nBytesToWrite);

	int   nDataLen = 0;   //每次WriteFile实际可发送数据长度
	DWORD nWritedLen = 0;
	OVERLAPPED m_osWrite;
	ZeroMemory(&m_osWrite,sizeof(m_osWrite));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (NULL == m_osWrite.hEvent)
	{
		m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	for (int i = 0; i < pPack->nLen; i += (m_nBytesToWrite - 3))//发送数据，每次用WriteFile发送m_nBytesToWrite - 3，
	{	                                                        //最后一次为pPack->nLen - i
		memset(outReport, 0, m_nBytesToWrite);

		//计算每次发送数据
		if (pPack->nLen - i < m_nBytesToWrite - 3)
		{
			nDataLen = pPack->nLen - i;         //最后一次
		} 
		else
		{
			nDataLen = m_nBytesToWrite - 3;
		}
		outReport[0] = 0x00;             //第一字节需为0x00 ?
		outReport[1] = nDataLen;         //第二字节为可发送数据长度
		outReport[2] = 0x00;             //第一字节为0x00 ?
		for (int j = 0; j < nDataLen; j++)
		{
			outReport[j+3] = *(pPack->pPtr + i + j);
		}
		//CString cs;
		//CString cs1;
		//for (int i = 0; i < m_nBytesToWrite; i++)
		//{
		//	cs1.Format("%02X ", outReport[i]);
		//	cs += cs1;
		//}
		//MessageBox(0,cs,0,0);

		WriteFile(m_hWrite, outReport, m_nBytesToWrite, &nWritedLen, &m_osWrite);

		Sleep(10);

	}
	if (m_osWrite.hEvent != NULL)
	{
		CloseHandle(m_osWrite.hEvent);
		m_osWrite.hEvent = NULL;
	}	

	if (outReport != NULL)
	{
		free(outReport);
		outReport = NULL;
	}
	return TRUE;
}
BOOL CHidUSB::SendData2(LPUSB_DATAPACK pPack)
{
	if (m_nBytesToWrite == 0)
	{
		//MessageBox(0, "m_nBytesToWrite == 0!", "error", 0, 0);
		return FALSE;
	}
	//目前根据USBTrace抓到的数据包分析得到的USB报告结果（有待查证）：
	//1.用WriteFile写数据只能发送固定长度的数据，其长度可根据HidD_GetPreparsedData函数来确定；
	//2.第一个字节需为0x00才能发送成功（报告ID？）；
	//3.第二个字节为所发送的数据长度，第三个字节一直为0x00，不知代表什么；
	//4.第四个字节开始为要发送的数据；
	//5.综上，每次发送的数据长度为用HidD_GetPreparsedData获取的解析长度减3。
	BYTE *outReport = (BYTE *)malloc(m_nBytesToWrite);

	int   nDataLen = 0;   //每次WriteFile实际可发送数据长度
	DWORD nWritedLen = 0;
	OVERLAPPED m_osWrite;
	ZeroMemory(&m_osWrite,sizeof(m_osWrite));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (NULL == m_osWrite.hEvent)
	{
		m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	BOOL isfirstflag=TRUE;
	for (int i = 0; i < pPack->nLen; i += (m_nBytesToWrite - 1))//发送数据，每次用WriteFile发送m_nBytesToWrite - 3，
	{	                                                        //最后一次为pPack->nLen - i
		memset(outReport, 0, m_nBytesToWrite);

		//计算每次发送数据
		if (pPack->nLen - i < m_nBytesToWrite - 1)
		{
			nDataLen = pPack->nLen - i;         //最后一次

		} 
		else
		{
			nDataLen = m_nBytesToWrite - 1;
		}
		/*if(isfirstflag)
		{
		outReport[0] = 0x00;             //第一字节需为0x00 ?
		outReport[1] = nDataLen;         //第二字节为可发送数据长度
		outReport[2] = 0x00;             //第一字节为0x00 ?
		for (int j = 0; j < nDataLen; j++)
		{
		outReport[j+3] = *(pPack->pPtr + i + j);
		}
		isfirstflag=false;
		}
		else
		{*/
		outReport[0] = 0x00;             //第一字节需为0x00 ?
		//outReport[1] = nDataLen;         //第二字节为可发送数据长度
		//outReport[2] = 0x00;             //第一字节为0x00 ?
		for (int j = 0; j < nDataLen; j++)
		{
			outReport[j+1] = *(pPack->pPtr + i + j);
		}

		//}

		//CString cs;
		//CString cs1;
		//for (int i = 0; i < m_nBytesToWrite; i++)
		//{
		//	cs1.Format("%02X ", outReport[i]);
		//	cs += cs1;
		//}
		//MessageBox(0,cs,0,0);

		WriteFile(m_hWrite, outReport, m_nBytesToWrite, &nWritedLen, &m_osWrite);

		//Sleep(30);

	}
	if (m_osWrite.hEvent != NULL)
	{
		CloseHandle(m_osWrite.hEvent);
		m_osWrite.hEvent = NULL;
	}	

	if (outReport != NULL)
	{
		free(outReport);
		outReport = NULL;
	}
	return TRUE;

}
/*********************************************************************************************************
** 函数名称: RecvDataThread
** 功能描述: 读数据线程，如收到数据，则以消息的格式给父窗口发送数据
** 输　入: LPVOID lparam，CHidUSB类指针
** 输　出: DWORD
** 全局变量: 
********************************************************************************************************/
DWORD CHidUSB::RecvDataThread(LPVOID lparam)
{

	CHidUSB *pHidUSB = (CHidUSB *)lparam;

	DWORD nReaddeLen = 0;

	while (1)
	{
		BYTE *inReport = (BYTE *)malloc(pHidUSB->m_nBytesToRead);
		USB_DATAPACK *pDataPack = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		memset(pDataPack, 0, sizeof(USB_DATAPACK));
		memset(inReport, 0, pHidUSB->m_nBytesToRead);
		//OVERLAPPED m_osRead;
		//memset(&m_osRead,0,sizeof(OVERLAPPED));
		//m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
		if (ReadFile(pHidUSB->m_hRead, inReport, pHidUSB->m_nBytesToRead, &nReaddeLen, NULL))
		{
			int nRecCount = inReport[1];   //inReport[1]表示实际接收字节数
			BYTE *pRec = (BYTE *)malloc(nRecCount);
			memset(pRec, 0, nRecCount);

			for (int i = 0; i < nRecCount; i++)
			{
				pRec[i] = inReport[i+3];
			}

			pDataPack->nLen = nRecCount;
			pDataPack->pPtr = pRec;
            
			pHidUSB->m_pOwnerWnd->SendMessage(WM_RECV_USB_DATA, (WPARAM)pHidUSB, (LPARAM)pDataPack);
			if (pRec != NULL)
			{
				free(pRec);
				pRec = NULL;
			}
		}
		else
		{
			
			SendMessage(pHidUSB->m_pOwnerWnd->m_hWnd, WM_CLOSE_USB_DEV, NULL, NULL);		//modified by Amy 2012-9-17
		}

		if (inReport != NULL)
		{
			free(inReport);
			inReport = NULL;
		}

		if (pDataPack != NULL)
		{
			free(pDataPack);
			pDataPack = NULL;
		}
		
	}

	return 0;
}

/*********************************************************************************************************
** 函数名称: CloseDevice
** 功能描述: 关闭设备，结束读线程，关闭句柄
** 输　入: 
** 输　出: 
** 全局变量: m_hRecvThread，m_hRead，m_hWrite，m_hRecvThread
********************************************************************************************************/
void CHidUSB::CloseDevice()
{
	TerminateThread(m_hRecvThread, 10);

	if (m_hRead != NULL)
	{
		CloseHandle(m_hRead);
		m_hRead = NULL;
	}
	if (m_hWrite != NULL)
	{
		CloseHandle(m_hWrite);
		m_hWrite = NULL;
	}
	if (m_hRecvThread != NULL)
	{
		CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;
	}
	
}

BOOL CHidUSB::CheckDevice()
{
	GUID            hidGuid;
	HDEVINFO        hDevInfo;
	HANDLE          hidHandle;

	//获取HID类GUID
	HidD_GetHidGuid(&hidGuid);

	//获取HID设备类信息
	hDevInfo = SetupDiGetClassDevs(&hidGuid,
		NULL,
		NULL,	
		(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		//MessageBox(0,"SetupDiGetClassDevs Failed!","error",0);
		return FALSE;
	}

	SP_DEVICE_INTERFACE_DATA devInfoData;
	devInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

	for (int i = 0; i < MAX_DEVICE; i++)
	{
		//if(!SetupDiEnumDeviceInfo(hDevInfo, i, NULL))
		//{
		//	//MessageBox(0,"SetupDiEnumDeviceInfo Failed!","error",0);
		//	return FALSE;
		//}

		//枚举设备接口
		if (!SetupDiEnumInterfaceDevice (hDevInfo, 0, &hidGuid, i, &devInfoData))
		{
			//MessageBox(0,"SetupDiEnumInterfaceDevice Failed!","error",0);
			continue;
		} 
		else
		{
			ULONG  reqLen = 0;

			//获取设备接口详细信息数据长度
			SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData, 
				NULL,
				0,
				&reqLen, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (reqLen);       

			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			//获取设备接口详细信息
			if(!SetupDiGetInterfaceDeviceDetail(hDevInfo, &devInfoData, devDetail, reqLen, NULL, NULL))
			{
				free(devDetail);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			hidHandle = CreateFile(devDetail->DevicePath, 
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);

			if (hidHandle==INVALID_HANDLE_VALUE)
			{
				free(devDetail);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			_HIDD_ATTRIBUTES hidAttributes;

			//获取属性
			if(!HidD_GetAttributes(hidHandle, &hidAttributes))
			{
				free(devDetail);
				//CloseHandle(hidHandle);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			//根据VID，PID判断是否是所要的设备
			if (m_sDeviceVID == hidAttributes.VendorID  
				&& m_sDevicePID  == hidAttributes.ProductID)
			{
				free(devDetail);
				CloseHandle(hidHandle);
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return TRUE;
			}
		}
	}
	CloseHandle(hidHandle);
	SetupDiDestroyDeviceInfoList(hDevInfo);

	CloseDevice();

	return FALSE;
}

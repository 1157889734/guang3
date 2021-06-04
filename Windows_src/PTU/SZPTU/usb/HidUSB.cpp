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
	m_hRead = NULL;          //���ܵ����
	m_hWrite = NULL;         //д�ܵ����
    m_hRecvThread = NULL;    //���������߳̾��
}

CHidUSB::CHidUSB(CWnd* pWnd, const unsigned short sVID, const unsigned short sPID)
{
	m_pOwnerWnd  = pWnd;
	m_sDeviceVID = sVID;
	m_sDevicePID = sPID;
}

CHidUSB::~CHidUSB(void)
{
	m_hRead = NULL;          //���ܵ����
	m_hWrite = NULL;         //д�ܵ����
	m_hRecvThread = NULL;    //���������߳̾��
}

/*********************************************************************************************************
** ��������: OpenDevice
** ��������: ������Ҫ��USB�豸,�ɹ��򴴽���д�ܵ�
** �䡡��: ��
** �䡡��: BOOL
** ȫ�ֱ���: m_hRead�����ܵ������m_hWrite��д�ܵ����
m_sDeviceVID��������豸VID��m_sDevicePID��������豸PID
m_nBytesToRead���ɶ�USB���泤�ȣ�m_nBytesToWrite����дUSB���泤��
********************************************************************************************************/
BOOL CHidUSB::OpenDevice()
{
	GUID            hidGuid;
	HDEVINFO        hDevInfo;
	HANDLE          hidHandle;

	//��ȡHID��GUID
	HidD_GetHidGuid(&hidGuid);

	//��ȡHID�豸����Ϣ
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

		//ö���豸�ӿ�
		if (!SetupDiEnumInterfaceDevice (hDevInfo, 0, &hidGuid, i, &devInfoData))
		{
			//MessageBox(0,"SetupDiEnumInterfaceDevice Failed!","error",0);
			continue;
		} 
		else
		{
			ULONG  reqLen = 0;

			//��ȡ�豸�ӿ���ϸ��Ϣ���ݳ���
			SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData, 
				NULL,
				0,
				&reqLen, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (reqLen);       

			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			//��ȡ�豸�ӿ���ϸ��Ϣ
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

			//��ȡ����
			if(!HidD_GetAttributes(hidHandle, &hidAttributes))
			{
				//free(devDetail);
				//CloseHandle(hidHandle);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			//����VID��PID�ж��Ƿ�����Ҫ���豸
			if (m_sDeviceVID == hidAttributes.VendorID  
				&& m_sDevicePID  == hidAttributes.ProductID)
			{
				//HidD_SetNumInputBuffers()

				char szRead[MAX_PATH], szWrite[MAX_PATH];
				//WCHAR szRead[MAX_PATH], szWrite[MAX_PATH];


				//���ֽ�ת���ֽ�
				char *pcstr = (char *)malloc(sizeof(char)*(2 * wcslen(devDetail->DevicePath)+1));
				memset(pcstr , 0 , 2 * wcslen(devDetail->DevicePath)+1 );

				int nlength=wcslen(devDetail->DevicePath);

				//��ȡת����ĳ���
				int nbytes = WideCharToMultiByte( 0, 0, devDetail->DevicePath,  nlength,  NULL, 0, NULL,  NULL );   

				int len = 2 * wcslen(devDetail->DevicePath)+1;
				if(nbytes>len)   nbytes=len;

				// ͨ�����ϵõ��Ľ����ת��unicode �ַ�Ϊascii �ַ�
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
				m_hRecvThread = CreateThread(NULL, 0, RecvDataThread, this, CREATE_SUSPENDED, &id);	//����USB���ݶ��߳�
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
** ��������: SendData
** ��������: ��������
** �䡡��: LPUSB_DATAPACK pPack��USB���ݰ�
** �䡡��: BOOL
** ȫ�ֱ���: m_hWrite��д�ܵ����
m_nBytesToWrite����дUSB���泤��
********************************************************************************************************/
BOOL CHidUSB::SendData(LPUSB_DATAPACK pPack)
{
	if (m_nBytesToWrite == 0)
	{
		//MessageBox(0, "m_nBytesToWrite == 0!", "error", 0, 0);
		return FALSE;
	}

	//Ŀǰ����USBTraceץ�������ݰ������õ���USB���������д���֤����
	//1.��WriteFileд����ֻ�ܷ��͹̶����ȵ����ݣ��䳤�ȿɸ���HidD_GetPreparsedData������ȷ����
	//2.��һ���ֽ���Ϊ0x00���ܷ��ͳɹ�������ID������
	//3.�ڶ����ֽ�Ϊ�����͵����ݳ��ȣ��������ֽ�һֱΪ0x00����֪����ʲô��
	//4.���ĸ��ֽڿ�ʼΪҪ���͵����ݣ�
	//5.���ϣ�ÿ�η��͵����ݳ���Ϊ��HidD_GetPreparsedData��ȡ�Ľ������ȼ�3��
	BYTE *outReport = (BYTE *)malloc(m_nBytesToWrite);

	int   nDataLen = 0;   //ÿ��WriteFileʵ�ʿɷ������ݳ���
	DWORD nWritedLen = 0;
	OVERLAPPED m_osWrite;
	ZeroMemory(&m_osWrite,sizeof(m_osWrite));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (NULL == m_osWrite.hEvent)
	{
		m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	for (int i = 0; i < pPack->nLen; i += (m_nBytesToWrite - 3))//�������ݣ�ÿ����WriteFile����m_nBytesToWrite - 3��
	{	                                                        //���һ��ΪpPack->nLen - i
		memset(outReport, 0, m_nBytesToWrite);

		//����ÿ�η�������
		if (pPack->nLen - i < m_nBytesToWrite - 3)
		{
			nDataLen = pPack->nLen - i;         //���һ��
		} 
		else
		{
			nDataLen = m_nBytesToWrite - 3;
		}
		outReport[0] = 0x00;             //��һ�ֽ���Ϊ0x00 ?
		outReport[1] = nDataLen;         //�ڶ��ֽ�Ϊ�ɷ������ݳ���
		outReport[2] = 0x00;             //��һ�ֽ�Ϊ0x00 ?
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
	//Ŀǰ����USBTraceץ�������ݰ������õ���USB���������д���֤����
	//1.��WriteFileд����ֻ�ܷ��͹̶����ȵ����ݣ��䳤�ȿɸ���HidD_GetPreparsedData������ȷ����
	//2.��һ���ֽ���Ϊ0x00���ܷ��ͳɹ�������ID������
	//3.�ڶ����ֽ�Ϊ�����͵����ݳ��ȣ��������ֽ�һֱΪ0x00����֪����ʲô��
	//4.���ĸ��ֽڿ�ʼΪҪ���͵����ݣ�
	//5.���ϣ�ÿ�η��͵����ݳ���Ϊ��HidD_GetPreparsedData��ȡ�Ľ������ȼ�3��
	BYTE *outReport = (BYTE *)malloc(m_nBytesToWrite);

	int   nDataLen = 0;   //ÿ��WriteFileʵ�ʿɷ������ݳ���
	DWORD nWritedLen = 0;
	OVERLAPPED m_osWrite;
	ZeroMemory(&m_osWrite,sizeof(m_osWrite));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (NULL == m_osWrite.hEvent)
	{
		m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	BOOL isfirstflag=TRUE;
	for (int i = 0; i < pPack->nLen; i += (m_nBytesToWrite - 1))//�������ݣ�ÿ����WriteFile����m_nBytesToWrite - 3��
	{	                                                        //���һ��ΪpPack->nLen - i
		memset(outReport, 0, m_nBytesToWrite);

		//����ÿ�η�������
		if (pPack->nLen - i < m_nBytesToWrite - 1)
		{
			nDataLen = pPack->nLen - i;         //���һ��

		} 
		else
		{
			nDataLen = m_nBytesToWrite - 1;
		}
		/*if(isfirstflag)
		{
		outReport[0] = 0x00;             //��һ�ֽ���Ϊ0x00 ?
		outReport[1] = nDataLen;         //�ڶ��ֽ�Ϊ�ɷ������ݳ���
		outReport[2] = 0x00;             //��һ�ֽ�Ϊ0x00 ?
		for (int j = 0; j < nDataLen; j++)
		{
		outReport[j+3] = *(pPack->pPtr + i + j);
		}
		isfirstflag=false;
		}
		else
		{*/
		outReport[0] = 0x00;             //��һ�ֽ���Ϊ0x00 ?
		//outReport[1] = nDataLen;         //�ڶ��ֽ�Ϊ�ɷ������ݳ���
		//outReport[2] = 0x00;             //��һ�ֽ�Ϊ0x00 ?
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
** ��������: RecvDataThread
** ��������: �������̣߳����յ����ݣ�������Ϣ�ĸ�ʽ�������ڷ�������
** �䡡��: LPVOID lparam��CHidUSB��ָ��
** �䡡��: DWORD
** ȫ�ֱ���: 
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
			int nRecCount = inReport[1];   //inReport[1]��ʾʵ�ʽ����ֽ���
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
** ��������: CloseDevice
** ��������: �ر��豸���������̣߳��رվ��
** �䡡��: 
** �䡡��: 
** ȫ�ֱ���: m_hRecvThread��m_hRead��m_hWrite��m_hRecvThread
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

	//��ȡHID��GUID
	HidD_GetHidGuid(&hidGuid);

	//��ȡHID�豸����Ϣ
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

		//ö���豸�ӿ�
		if (!SetupDiEnumInterfaceDevice (hDevInfo, 0, &hidGuid, i, &devInfoData))
		{
			//MessageBox(0,"SetupDiEnumInterfaceDevice Failed!","error",0);
			continue;
		} 
		else
		{
			ULONG  reqLen = 0;

			//��ȡ�豸�ӿ���ϸ��Ϣ���ݳ���
			SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData, 
				NULL,
				0,
				&reqLen, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (reqLen);       

			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			//��ȡ�豸�ӿ���ϸ��Ϣ
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

			//��ȡ����
			if(!HidD_GetAttributes(hidHandle, &hidAttributes))
			{
				free(devDetail);
				//CloseHandle(hidHandle);
				//SetupDiDestroyDeviceInfoList(hDevInfo);

				continue;
			}

			//����VID��PID�ж��Ƿ�����Ҫ���豸
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

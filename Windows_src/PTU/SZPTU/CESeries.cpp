// CESeries.cpp: implementation of the CCESeries class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CESeries.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//������д�̷߳��͵���Ϣ����
const int CM_THREADCOMMWRITE = WM_USER+110;
const int CM_THREADCOMMGETNEWCHAR = WM_USER+111;

#define DATAHEAD 0x7E
#define DATATAIL 0x0D

//��CCESeries���캯��
CCESeries::CCESeries()
{
	idstr.Empty();
	m_LastPackTime=CTime(1970,1,1);
	m_hComm = INVALID_HANDLE_VALUE;
	m_hReadCloseEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hWriteCloseEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hAnsysCloseEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	//m_RecvBuff[FRAME_BUFFER_SIZE] = {0};
	::InitializeCriticalSection(&m_Csection);	
}

//��CCESeries��������
CCESeries::~CCESeries()
{
	ClosePort();
	if (NULL != m_hAnsysCloseEvent)
	{
		CloseHandle(m_hAnsysCloseEvent);
		m_hAnsysCloseEvent = NULL;
	}
	if (NULL != m_hReadCloseEvent)
	{
		CloseHandle(m_hReadCloseEvent);
		m_hReadCloseEvent = NULL;
	}
	if (NULL != m_hWriteCloseEvent)
	{
		CloseHandle(m_hWriteCloseEvent);
		m_hWriteCloseEvent = NULL;
	}
	
	::DeleteCriticalSection(&m_Csection);
}

/*
*�������ܣ��򿪴���
*��ڲ�����pPortOwner	:ʹ�ô˴�����Ĵ�����
		   portNo		:���ں�
		   baud			:������
		   parity		:��żУ��
		   databits		:����λ
		   stopbits		:ֹͣλ
*���ڲ�����(��)
*����ֵ��TRUE:�ɹ��򿪴���;FALSE:�򿪴���ʧ��
*/

BOOL CCESeries::OpenPort(UINT portNo	,			/*���ں�*/
						 UINT baud		,			/*������*/
						 UINT parity	,			/*��żУ��*/
						 UINT databits	,			/*����λ*/
						 UINT stopbits			   /*ֹͣλ*/
						 )
{
	DCB commParam;
	TCHAR szPort[15];	
	
	// �Ѿ��򿪵Ļ���ֱ�ӷ���
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	
	//���ô�����
	wsprintf(szPort, _T("COM%d:"), portNo);
	//�򿪴���
	m_hComm = CreateFile(
		szPort,
		GENERIC_READ | GENERIC_WRITE,	//�������д
		0,								//��ռ��ʽ������ģʽ��
		NULL,
		OPEN_EXISTING,					//�򿪶����Ǵ�����������ʽ��
		FILE_FLAG_OVERLAPPED,
		NULL 
		);
	idstr.Format(_T("ID:Com%d"),portNo);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// ��Ч���,���ء�		
		return FALSE;
	}
	
	// �õ��򿪴��ڵĵ�ǰ���Բ������޸ĺ����������ô��ڡ�
	// ���ô��ڵĳ�ʱ����Ϊ�������ء�
	
	if (!GetCommState(m_hComm,&commParam))
	{		
		//�رմ���
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	GetCommState(m_hComm,&commParam);
	commParam.BaudRate = baud;					// ���ò����� 
	commParam.fBinary = TRUE;					// ���ö�����ģʽ���˴���������TRUE
	commParam.fParity = FALSE;//TRUE;					// ֧����żУ�� 
	commParam.ByteSize = (BYTE)databits;				// ����λ,��Χ:4-8 
	commParam.Parity = (BYTE)parity;					// У��ģʽ
	commParam.StopBits = (BYTE)stopbits;				// ֹͣλ 
	
	commParam.fOutxCtsFlow = FALSE;				// No CTS output flow control 
	commParam.fOutxDsrFlow = FALSE;				// No DSR output flow control 
	commParam.fDtrControl = DTR_CONTROL_ENABLE; 
	// DTR flow control type 
	commParam.fDsrSensitivity = FALSE;			// DSR sensitivity 
	commParam.fTXContinueOnXoff = FALSE;		// XOFF continues Tx 
	commParam.fOutX = FALSE;					// No XON/XOFF out flow control 
	commParam.fInX = FALSE;						// No XON/XOFF in flow control 
	commParam.fErrorChar = FALSE;				// Disable error replacement 
	commParam.fNull = FALSE;					// Disable null stripping 

	commParam.fRtsControl = RTS_CONTROL_ENABLE; //RTS_CONTROL_ENABLE; 
	// 485�Ķ�
	// RTS flow control 
	commParam.fAbortOnError = FALSE;			// �����ڷ������󣬲�����ֹ���ڶ�д
	
	if (!SetCommState(m_hComm, &commParam))
	{
		//�رմ���
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	//m_pPortOwner = pPortOwner;
	
    //���ô��ڶ�д��ʱʱ��
	COMMTIMEOUTS CommTimeOuts;
	//�õ���ʱ����
	GetCommTimeouts (m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;  
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;  
	CommTimeOuts.ReadTotalTimeoutConstant = 0;    
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeOuts.WriteTotalTimeoutConstant = 0;  
	
	if(!SetCommTimeouts( m_hComm, &CommTimeOuts ))
	{
		//�رմ���
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}	
	//ָ���˿ڼ����¼���, �����߳���ָ��
	SetCommMask (m_hComm, EV_RXCHAR);
	
	//�����豸������
	SetupComm(m_hComm,512,512);
	
	//��ʼ���������е���Ϣ
	PurgeComm(m_hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);

	EscapeCommFunction(m_hComm,CLRDTR);
	EscapeCommFunction(m_hComm,CLRRTS);
	
	ResetEvent(m_hAnsysCloseEvent);
	ResetEvent(m_hReadCloseEvent);
	ResetEvent(m_hWriteCloseEvent);
	//�����������߳�
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,CREATE_SUSPENDED,&m_dwReadThreadID);
	//����д�����߳�
	m_hWriteThread = CreateThread(NULL,0,WriteThreadFunc,this,CREATE_SUSPENDED,&m_dwWriteThreadID);

	ResumeThread(m_hWriteThread);
	ResumeThread(m_hReadThread);

	return TRUE;
}

/*
*�������ܣ��رմ���
*��ڲ�����(��)
*���ڲ�����(��)
*����ֵ��  (��)
*/
void CCESeries::ClosePort()
{	
	//��ʾ���ڻ�û�д�
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	
	//�رն��߳�
	CloseReadThread();
	//�ر�д�߳�
	CloseWriteThread();
	
	//�رմ���
	if (CloseHandle (m_hComm))
	{
		m_hComm = INVALID_HANDLE_VALUE;
		return ;
	}
}


/*
*�������ܣ��򴮿ڷ�������
*��ڲ�����buf		: ��Ҫ������д������ݵĻ�����
		   bufLen	: ��Ҫ������д������ݵĻ���������
*���ڲ�����(��)
*����ֵ��TRUE:��ʾ�ɹ��ؽ�Ҫ���͵����ݴ��ݵ�д�߳���Ϣ���С�
		 FALSE:��ʾ��Ҫ���͵����ݴ��ݵ�д�߳���Ϣ����ʧ�ܡ�
		 ע�ӣ��˴���TRUE,��ֱ�Ӵ�������һ���ɹ�д�뵽�����ˡ�
*/
BOOL CCESeries::WritePort(const BYTE *buf,DWORD bufLen)
{
	//��Ҫ���͵����ݴ��ݵ�д�߳���Ϣ����
	// ȡ���첽���� [5/16/2012 beihai]
	/*if (PostThreadMessage(m_dwWriteThreadID,CM_THREADCOMMWRITE,
		WPARAM(bufLen), LPARAM(buf)))
	{
		return TRUE;
	}*/
	//�򴮿�д
	::EnterCriticalSection(&m_Csection);
	BOOL bRet = WritePort(m_hComm,buf,bufLen);
	//ɾ����̬������ڴ�
	::LeaveCriticalSection(&m_Csection);
	return bRet;
}

/*
*�������ܣ����ô��ڶ�ȡ��д�볬ʱ
*��ڲ�����CommTimeOuts : ָ��COMMTIMEOUTS�ṹ
*���ڲ�����(��)
*����ֵ��TRUE:���óɹ�;FALSE:����ʧ��
*/
BOOL CCESeries::SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);
	return SetCommTimeouts(m_hComm,&CommTimeOuts);
}


//���ڶ��̺߳���
DWORD CCESeries::ReadThreadFunc(LPVOID lparam)
{
	CCESeries *ceSeries = (CCESeries*)lparam;
	
	DWORD	evtMask;
	BYTE * readBuf = NULL;//��ȡ���ֽ�
	DWORD actualReadLen=0;//ʵ�ʶ�ȡ���ֽ���
	DWORD willReadLen;
	DWORD dwCommModemStatus;
	
	DWORD dwReadErrors;
	COMSTAT	cmState;

	OVERLAPPED ovn;
	HANDLE hevent;
	hevent=CreateEvent(NULL,TRUE,FALSE,NULL);
	ZeroMemory(&ovn,sizeof(ovn));
	ovn.hEvent=hevent;
	
	// ��ջ��壬����鴮���Ƿ�򿪡�
	ASSERT(ceSeries->m_hComm !=INVALID_HANDLE_VALUE); 
	
	
	//��մ���
	PurgeComm(ceSeries->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR );
	
	SetCommMask(ceSeries->m_hComm, EV_RXCHAR);
	
	//����յ����߳��˳��źţ����˳��߳�
	while (WaitForSingleObject(ceSeries->m_hReadCloseEvent,1) == WAIT_TIMEOUT)
	{   	
		SetCommMask (ceSeries->m_hComm, EV_RXCHAR);

		if (WaitCommEvent(ceSeries->m_hComm,&evtMask,0))
		{		
			ClearCommError(ceSeries->m_hComm,&dwReadErrors,&cmState);
			//��ʾ�����յ��ַ�		
			if (evtMask & EV_RXCHAR) 
			{
				willReadLen = cmState.cbInQue ;
				if (willReadLen <= 0)
				{
					continue;
				}
				readBuf=(BYTE*)malloc(willReadLen);
				::EnterCriticalSection(&ceSeries->m_Csection);
				BOOL bret=ReadFile(ceSeries->m_hComm, readBuf, willReadLen, &actualReadLen,&ovn);
				if(!bret)
				{
					if(GetLastError()==ERROR_IO_PENDING)
					{
						//////////////////////////////////////////////////////////////////
						DWORD des;
						des=WaitForSingleObject(ovn.hEvent,1000);
						if(des==WAIT_OBJECT_0)//
						{
							BOOL bResult = GetOverlappedResult(ceSeries->m_hComm, &ovn, 
																&actualReadLen, FALSE) ; 
							// if there was a problem ... 
							if (!bResult) 
								actualReadLen=0;
						}
					}
				}
				::LeaveCriticalSection(&ceSeries->m_Csection);
				
				//�����ȡ�����ݴ���0��
				if (actualReadLen>0)
				{
					COMM_DATAPACK *pptr=(COMM_DATAPACK*)malloc(sizeof(COMM_DATAPACK));
					pptr->nLen=actualReadLen;
					pptr->pPtr=readBuf;
					ceSeries->OnRecv(pptr); //������� deleted by cl 2012-8-7

					if(readBuf != NULL)
					{
						free(readBuf);
						readBuf = NULL;
					}
					if(pptr != NULL)
					{
						free(pptr);
						pptr = NULL;
					}
				}
				else
				{
					if(readBuf != NULL)
					{
						free(readBuf);
						readBuf = NULL;
					}
				}
			}
		}
		GetCommModemStatus(ceSeries->m_hComm, &dwCommModemStatus);
	}
	return 0;
}

//����д�̺߳���
DWORD CCESeries::WriteThreadFunc(LPVOID lparam)
{
	CCESeries *ceSeries = (CCESeries*)lparam;
	MSG msg;
	DWORD dwWriteLen = 0;
	BYTE * buf = NULL;
				
	//����յ�д�߳��˳��źţ����˳��߳�
	while (WaitForSingleObject(ceSeries->m_hWriteCloseEvent,1) == WAIT_TIMEOUT)
	{
		//�����׽���߳���Ϣ
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.hwnd != 0 )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			
			if (msg.message == CM_THREADCOMMWRITE)
			{
				//�򴮿�д
				buf = (BYTE*)msg.lParam;
				dwWriteLen = (DWORD)msg.wParam;
				//�򴮿�д
				::EnterCriticalSection(&ceSeries->m_Csection);
				WritePort(ceSeries->m_hComm,buf,dwWriteLen);
				//ɾ����̬������ڴ�
				::LeaveCriticalSection(&ceSeries->m_Csection);
				//free(buf); // const�����˴�����Ҫ�ͷ� [5/14/2012 beihai]
				Sleep(1);
			}
		}
	}

	if(buf != NULL)
	{
		buf = NULL;
	}
	if(ceSeries != NULL)
	{
		ceSeries = NULL;
	}
	return 0;
}


//˽�÷����������򴮿�д���ݣ���д�̵߳���
BOOL CCESeries::WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen)
{
	DWORD dwNumBytesWritten;
	DWORD dwHaveNumWritten =0 ; //�Ѿ�д�����
	
	ASSERT(hComm != INVALID_HANDLE_VALUE);
	BOOL bResult=FALSE;
	BOOL bExitWhile=FALSE; 
	OVERLAPPED ovn;
	ZeroMemory(&ovn,sizeof(ovn));
	ovn.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	
	do
	{
		BOOL bret = WriteFile ( hComm,								//���ھ�� 
								buf+dwHaveNumWritten,				//��д���ݻ����� 
								bufLen - dwHaveNumWritten,          //��д���ݻ�������С
								&dwNumBytesWritten,					//����ִ�гɹ��󣬷���ʵ���򴮿�д�ĸ���	
								&ovn);
		if(!bret)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				if(!GetOverlappedResult(hComm,&ovn,&dwNumBytesWritten,TRUE))
					break;
			}else  //�������������������Ƿ��ͳ����˴���ĺ���
				break;
		}
		dwHaveNumWritten = dwHaveNumWritten + dwNumBytesWritten;
		//д�����
		if (dwHaveNumWritten == bufLen)
		{
			bResult=TRUE;
			bExitWhile=TRUE;
		}
	}while (!bExitWhile);

	if(ovn.hEvent != NULL)
	{
		CloseHandle(ovn.hEvent);
		ovn.hEvent = NULL;
	}

	return bResult;	
}



//�رն��߳�
void CCESeries::CloseReadThread()
{
	SetEvent(m_hReadCloseEvent);
	//���������¼���Ч��Ч
	SetCommMask(m_hComm, 0);
	//������н�Ҫ��������
    PurgeComm( m_hComm,  PURGE_RXCLEAR );
    //�ȴ�10�룬������߳�û���˳�����ǿ���˳�
    if (WaitForSingleObject(m_hReadThread,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hReadThread,10);
		DWORD dwExit;
		
		while(1)
		{
			GetExitCodeThread(m_hReadThread,&dwExit);
			if(dwExit!=STILL_ACTIVE) 
				break;
			Sleep(10);
		}
	}
	m_hReadThread = NULL;
}

//�ر�д�߳�
void CCESeries::CloseWriteThread()
{
	SetEvent(m_hWriteCloseEvent);
	
	//������н�Ҫд������
    PurgeComm( m_hComm,  PURGE_TXCLEAR );
	
    //�ȴ�10�룬������߳�û���˳�����ǿ���˳�
    if (WaitForSingleObject(m_hWriteThread,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hWriteThread,10);
		DWORD dwExit;
		
		while(1)
		{
			GetExitCodeThread(m_hWriteThread,&dwExit);
			if(dwExit!=STILL_ACTIVE) 
				break;
			Sleep(10);
		}
	}
	m_hWriteThread = NULL;
}



BOOL CCESeries::GetCommObjState()
{
	if(	m_hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	else 
		return TRUE;
}

void CCESeries::OnRecv( LPCOMM_DATAPACK packData )
{
	//���û�м̳���ɾ������
	//if (packData->pPtr != NULL)
	//{
	//	free(packData->pPtr);
	//	packData->pPtr = NULL;
	//}
	//if (packData != NULL)
	//{
	//	free(packData);
	//	packData = NULL;
	//}
		
}

CHDLCSeries::CHDLCSeries()
	:m_nSendCount(0),m_nRecvCount(0),m_bNeed7E(TRUE)

{
	m_pWndOwner = NULL;
}

void CHDLCSeries::SetOwner( CWnd* pOwner )
{
	m_pWndOwner = pOwner;
}

void CHDLCSeries::OnRecv( LPCOMM_DATAPACK packData )
{
	//ԭʼ����ֱ�ӷ���
	if (!m_bNeed7E)
	{
		if (m_pWndOwner != NULL)
		{
			//m_pWndOwner->PostMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)packData);
			m_pWndOwner->SendMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)packData);		//modified by Amy 2012-9-19
		}
		return;
	}
	//������ѵĴ�����
	if (m_nRecvCount+packData->nLen < FRAME_BUFFER_SIZE)
	{
		//ƴ����һ��
		//strncpy((char*)m_RecvBuff,(char*)packData->pPtr,packData->nLen);  //deleted by amy 
		memcpy((char*)(m_RecvBuff+m_nRecvCount),(char*)packData->pPtr,packData->nLen);
		m_nRecvCount += packData->nLen;
		
	}
	else
	{
		memset(m_RecvBuff,0,FRAME_BUFFER_SIZE);
		memcpy((char*)m_RecvBuff,(char*)packData->pPtr,packData->nLen);
		m_nRecvCount = packData->nLen;
	}
	//�����ַ�ת�岢ȥ��֡ͷ��֡β
	static BYTE temBuff[FRAME_BUFFER_SIZE] = {0};

	int nPackLen = 0;//���ڼ�¼���ݰ��ĳ���

	//����Ƿ�ʼƥ��
	bool bStartMatch = false;
	//����������
    for (int i = 0; i < m_nRecvCount; i++) 
    {
		if (m_RecvBuff[i] == 0x7E)
		{
			if (!bStartMatch)
			{	
				//���û��ƥ����ʼ����ƥ��
				bStartMatch = true;
				nPackLen = 0;//�ָ�

				//added by cl
				temBuff[nPackLen] = m_RecvBuff[i];
				nPackLen++;
			}
			else
			{
				//�Ѿ�ƥ��Ļ��ͽ�ƥ��õ����ݷ��͵����ڣ�
				bStartMatch = false;
				if (nPackLen > 0)
				{
					temBuff[nPackLen] = m_RecvBuff[i];
					nPackLen++;
					//int nActualLen;
					//����ת���ַ�
					//ConvertData(temBuff,nPackLen,&nActualLen); //deleted by cl 2012-8-8
					if (m_pWndOwner != NULL)
					{
						COMM_DATAPACK *pptr = (LPCOMM_DATAPACK)malloc(sizeof(COMM_DATAPACK));
						//pptr->nLen = nActualLen;
						//pptr->pPtr = (BYTE*)malloc(nActualLen);		//deleted by cl 2012-8-8
						//strncpy((char*)pptr->pPtr,(char*)temBuff,nActualLen);
						pptr->nLen =nPackLen;
						pptr->pPtr = (BYTE*)malloc(nPackLen);
						memcpy((char*)pptr->pPtr,(char*)temBuff,nPackLen);
						m_pWndOwner->SendMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)pptr);
						if (pptr->pPtr != NULL)
						{
							free(pptr->pPtr);
							pptr->pPtr = NULL;
						}
						if (pptr != NULL)
						{
							free(pptr);
							pptr = NULL;
						}
					}
				}	
				//��������������ǰ�ƶ�������һ��ƥ��
				for(int j = i+1;j < m_nRecvCount;j++)
				{
					m_RecvBuff[j-i+1] = m_RecvBuff[j];
					m_RecvBuff[j] = 0;
				}
				m_nRecvCount = m_nRecvCount-i-1;
				//����ƥ��ʣ����ַ� forѭ������ǰ��һ
				i = -1; 
			}			
		}
		else
		{
			 if (bStartMatch)
			 {
				 temBuff[nPackLen] = m_RecvBuff[i];
				 nPackLen++;
			 }
		}
    }
	//CCESeries::OnRecv(packData);//���ø����������  //deleted by amy 2012-9-19
}

BOOL CHDLCSeries::WriteHDLC( const BYTE *buf,DWORD bufLen, BOOL bNeed7E)
{
	//Щ����ֻ�Ǽ򵥵�Ϊ������֡ͷ֡βת���ַ�
	if(bufLen > FRAME_BUFFER_SIZE) return FALSE;

	memset(m_SendBuff,0,FRAME_BUFFER_SIZE);

	m_bNeed7E = bNeed7E;
	//�����ʹ��HDLCЭ��ֱ�ӷ���
	if (!m_bNeed7E)	
	{
		for (int i = 0; i< (int)bufLen; i++)
		{
			m_SendBuff[i] = *buf;
			buf++;
		}
		return WritePort(m_SendBuff,bufLen);
	}

	m_SendBuff[0]  = 0x7E;//����֡ͷ
	int nInsertPos = 1;    //�ӻ�����1����ʼд��
	m_nSendCount   = 1;   //���ݳ���

	for (int i = 0; i< (int)bufLen; i++)
	{
		if((*buf) == 0x7E)
		{
			m_SendBuff[nInsertPos] = 0x7F;
			nInsertPos++;
			m_nSendCount++;
			m_SendBuff[nInsertPos] = 0x80;
			nInsertPos++;
			m_nSendCount++;
			buf++;
		}
		else if((*buf) == 0x7F)
		{
			m_SendBuff[nInsertPos] = 0x7F;
			nInsertPos++;
			m_nSendCount++;
			m_SendBuff[nInsertPos] = 0x81;
			nInsertPos++;
			m_nSendCount++;
			buf++;
		}
		else
		{
			m_SendBuff[nInsertPos] = *buf;
			nInsertPos++;
			m_nSendCount++;
			buf++;
		}
	}
	m_SendBuff[nInsertPos] = 0x7E;//֡β
	m_nSendCount++;
	return WritePort(m_SendBuff,m_nSendCount);
}

void CHDLCSeries::ConvertData( BYTE *pData,int nLen,int *actLen )
{
	//����λ�����ڼ���ת������ݵĲ���λ
	int nWatchPos = 0;
	*actLen = 0;
	//����
	BYTE *pTemp = pData;
	for (int i=0; i<nLen; i++)
	{
		if(*(pTemp+i) == 0x7F)
		{
			if (*(pTemp+i+1) == 0x80)
			{
				//��ʾ��7Eת��
				*(pData+nWatchPos) = 0x7E;
				nWatchPos++;
				(*actLen)++;
				i++;//ֱ������һ�¸�λ��
			}
			else if (*(pTemp+i+1) == 0x81)
			{
				//��ʾ��7Fת��
				*(pData+nWatchPos) = 0x7F;
				nWatchPos++;
				(*actLen)++;
				i++;//ֱ������һ�¸�λ��
			}
			else
			{
				//�˴�Ϊ7F����ûת��
				*(pData+nWatchPos) = *(pTemp+i);
				nWatchPos++;
				(*actLen)++;
			}
		}
		else
		{
			*(pData+nWatchPos) = *(pTemp+i);
			nWatchPos++;
			(*actLen)++;
		}
	}
	//�����ʵ�ֽ�������������
	memset((pData+(*actLen)),0,(*actLen));

	if(pTemp != NULL)
	{
		pTemp = NULL;
	}
}
#include "StdAfx.h"
#include "ComPro.h"

CComPro::CComPro(void)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hReadCloseEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hWriteCloseEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	::InitializeCriticalSection(&m_Csection);
}

CComPro::~CComPro(void)
{
	
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
void CComPro::SetOwner(CWnd* pOwner)
{
	m_pPortOwner = pOwner;
}
BOOL CComPro::OpenPort(UINT portNo	,			/*���ں�*/
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


	ResetEvent(m_hReadCloseEvent);
	ResetEvent(m_hWriteCloseEvent);
	//�����������߳�
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,CREATE_SUSPENDED,&m_dwReadThreadID);

	ResumeThread(m_hReadThread);

	return TRUE;
}
void CComPro::ClosePort()
{	
	//��ʾ���ڻ�û�д�
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}

	//�رն��߳�
	CloseReadThread();
	//�رմ���
	if (CloseHandle (m_hComm))
	{
		m_hComm = INVALID_HANDLE_VALUE;
		return ;
	}
}
BOOL CComPro::WritePort(const BYTE *buf,DWORD bufLen)
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
//˽�÷����������򴮿�д���ݣ���д�̵߳���
BOOL CComPro::WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen)
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
			bufLen- dwHaveNumWritten,          //��д���ݻ�������С
			&dwNumBytesWritten,					//����ִ�гɹ��󣬷���ʵ���򴮿�д�ĸ���	
			&ovn);
		if(!bret)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				if(!GetOverlappedResult(hComm,&ovn,&dwNumBytesWritten,TRUE))
					break;
			}
			else  //�������������������Ƿ��ͳ����˴���ĺ���
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
void CComPro::CloseReadThread()
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

//���ڶ��̺߳���
DWORD CComPro::ReadThreadFunc(LPVOID lparam)
{
	CComPro *ceSeries = (CComPro *) lparam;	
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
		Sleep(5);
	}
	return 0;
}
void CComPro::OnRecv( LPCOMM_DATAPACK packData )
{
	m_pPortOwner->SendMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)packData);

}
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
BOOL CComPro::OpenPort(UINT portNo	,			/*串口号*/
						 UINT baud		,			/*波特率*/
						 UINT parity	,			/*奇偶校验*/
						 UINT databits	,			/*数据位*/
						 UINT stopbits			   /*停止位*/
						 )
{
	DCB commParam;
	TCHAR szPort[15];	

	// 已经打开的话，直接返回
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	//设置串口名
	wsprintf(szPort, _T("COM%d:"), portNo);
	//打开串口
	m_hComm = CreateFile(
		szPort,
		GENERIC_READ | GENERIC_WRITE,	//允许读和写
		0,								//独占方式（共享模式）
		NULL,
		OPEN_EXISTING,					//打开而不是创建（创建方式）
		FILE_FLAG_OVERLAPPED,
		NULL 
		);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// 无效句柄,返回。		
		return FALSE;
	}
	// 得到打开串口的当前属性参数，修改后再重新设置串口。
	// 设置串口的超时特性为立即返回。

	if (!GetCommState(m_hComm,&commParam))
	{		
		//关闭串口
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	GetCommState(m_hComm,&commParam);
	commParam.BaudRate = baud;					// 设置波特率 
	commParam.fBinary = TRUE;					// 设置二进制模式，此处必须设置TRUE
	commParam.fParity = FALSE;//TRUE;					// 支持奇偶校验 
	commParam.ByteSize = (BYTE)databits;				// 数据位,范围:4-8 
	commParam.Parity = (BYTE)parity;					// 校验模式
	commParam.StopBits = (BYTE)stopbits;				// 停止位 

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
	// 485改动
	// RTS flow control 
	commParam.fAbortOnError = FALSE;			// 当串口发生错误，并不终止串口读写

	if (!SetCommState(m_hComm, &commParam))
	{
		//关闭串口
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	//设置串口读写超时时间
	COMMTIMEOUTS CommTimeOuts;
	//得到超时参数
	GetCommTimeouts (m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;  
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;  
	CommTimeOuts.ReadTotalTimeoutConstant = 0;    
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeOuts.WriteTotalTimeoutConstant = 0;  

	if(!SetCommTimeouts( m_hComm, &CommTimeOuts ))
	{
		//关闭串口
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}	
	//指定端口监测的事件集, 留到线程中指定
	SetCommMask (m_hComm, EV_RXCHAR);

	//分配设备缓冲区
	SetupComm(m_hComm,512,512);

	//初始化缓冲区中的信息
	PurgeComm(m_hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);

	EscapeCommFunction(m_hComm,CLRDTR);
	EscapeCommFunction(m_hComm,CLRRTS);


	ResetEvent(m_hReadCloseEvent);
	ResetEvent(m_hWriteCloseEvent);
	//创建读串口线程
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,CREATE_SUSPENDED,&m_dwReadThreadID);

	ResumeThread(m_hReadThread);

	return TRUE;
}
void CComPro::ClosePort()
{	
	//表示串口还没有打开
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}

	//关闭读线程
	CloseReadThread();
	//关闭串口
	if (CloseHandle (m_hComm))
	{
		m_hComm = INVALID_HANDLE_VALUE;
		return ;
	}
}
BOOL CComPro::WritePort(const BYTE *buf,DWORD bufLen)
{
	//将要发送的数据传递到写线程消息队列
	// 取消异步发送 [5/16/2012 beihai]
	/*if (PostThreadMessage(m_dwWriteThreadID,CM_THREADCOMMWRITE,
	WPARAM(bufLen), LPARAM(buf)))
	{
	return TRUE;
	}*/
	//向串口写
	::EnterCriticalSection(&m_Csection);
	BOOL bRet = WritePort(m_hComm,buf,bufLen);
	//删除动态分配的内存
	::LeaveCriticalSection(&m_Csection);
	return bRet;
}
//私用方法，用于向串口写数据，被写线程调用
BOOL CComPro::WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen)
{
	DWORD dwNumBytesWritten;
    DWORD dwHaveNumWritten =0 ; //已经写入多少

	ASSERT(hComm != INVALID_HANDLE_VALUE);

	BOOL bResult=FALSE;
	BOOL bExitWhile=FALSE; 
	OVERLAPPED ovn;
	ZeroMemory(&ovn,sizeof(ovn));
	ovn.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	do
	{
		BOOL bret = WriteFile ( hComm,								//串口句柄 
			buf+dwHaveNumWritten,				//被写数据缓冲区 
			bufLen- dwHaveNumWritten,          //被写数据缓冲区大小
			&dwNumBytesWritten,					//函数执行成功后，返回实际向串口写的个数	
			&ovn);
		if(!bret)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				if(!GetOverlappedResult(hComm,&ovn,&dwNumBytesWritten,TRUE))
					break;
			}
			else  //这里上下两处跳出，是发送出现了错误的含义
				break;
		}
		dwHaveNumWritten = dwHaveNumWritten + dwNumBytesWritten;
		//写入完成
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
	//设置所有事件无效无效
	SetCommMask(m_hComm, 0);
	//清空所有将要读的数据
	PurgeComm( m_hComm,  PURGE_RXCLEAR );
	//等待10秒，如果读线程没有退出，则强制退出
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

//串口读线程函数
DWORD CComPro::ReadThreadFunc(LPVOID lparam)
{
	CComPro *ceSeries = (CComPro *) lparam;	
	DWORD	evtMask;
	BYTE * readBuf = NULL;//读取的字节
	DWORD actualReadLen=0;//实际读取的字节数
	DWORD willReadLen;
	DWORD dwCommModemStatus;

	DWORD dwReadErrors;
	COMSTAT	cmState;

	OVERLAPPED ovn;
	HANDLE hevent;
	hevent=CreateEvent(NULL,TRUE,FALSE,NULL);
	ZeroMemory(&ovn,sizeof(ovn));
	ovn.hEvent=hevent;

	// 清空缓冲，并检查串口是否打开。
	ASSERT(ceSeries->m_hComm !=INVALID_HANDLE_VALUE); 
    

	//清空串口
	PurgeComm(ceSeries->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR );

	SetCommMask(ceSeries->m_hComm, EV_RXCHAR);

	//如果收到读线程退出信号，则退出线程
	while (WaitForSingleObject(ceSeries->m_hReadCloseEvent,1) == WAIT_TIMEOUT)
	{   	
		SetCommMask (ceSeries->m_hComm, EV_RXCHAR);

		if (WaitCommEvent(ceSeries->m_hComm,&evtMask,0))
		{		
			ClearCommError(ceSeries->m_hComm,&dwReadErrors,&cmState);
			//表示串口收到字符		
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

				//如果读取的数据大于0，
				if (actualReadLen>0)
				{
					COMM_DATAPACK *pptr=(COMM_DATAPACK*)malloc(sizeof(COMM_DATAPACK));
					pptr->nLen=actualReadLen;
					pptr->pPtr=readBuf;
					ceSeries->OnRecv(pptr); //输出数据 deleted by cl 2012-8-7

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
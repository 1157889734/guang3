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

//定义向写线程发送的消息常量
const int CM_THREADCOMMWRITE = WM_USER+110;
const int CM_THREADCOMMGETNEWCHAR = WM_USER+111;

#define DATAHEAD 0x7E
#define DATATAIL 0x0D

//类CCESeries构造函数
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

//类CCESeries析构函数
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
*函数介绍：打开串口
*入口参数：pPortOwner	:使用此串口类的窗体句柄
		   portNo		:串口号
		   baud			:波特率
		   parity		:奇偶校验
		   databits		:数据位
		   stopbits		:停止位
*出口参数：(无)
*返回值：TRUE:成功打开串口;FALSE:打开串口失败
*/

BOOL CCESeries::OpenPort(UINT portNo	,			/*串口号*/
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
	idstr.Format(_T("ID:Com%d"),portNo);
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

	//m_pPortOwner = pPortOwner;
	
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
	
	ResetEvent(m_hAnsysCloseEvent);
	ResetEvent(m_hReadCloseEvent);
	ResetEvent(m_hWriteCloseEvent);
	//创建读串口线程
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,CREATE_SUSPENDED,&m_dwReadThreadID);
	//创建写串口线程
	m_hWriteThread = CreateThread(NULL,0,WriteThreadFunc,this,CREATE_SUSPENDED,&m_dwWriteThreadID);

	ResumeThread(m_hWriteThread);
	ResumeThread(m_hReadThread);

	return TRUE;
}

/*
*函数介绍：关闭串口
*入口参数：(无)
*出口参数：(无)
*返回值：  (无)
*/
void CCESeries::ClosePort()
{	
	//表示串口还没有打开
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	
	//关闭读线程
	CloseReadThread();
	//关闭写线程
	CloseWriteThread();
	
	//关闭串口
	if (CloseHandle (m_hComm))
	{
		m_hComm = INVALID_HANDLE_VALUE;
		return ;
	}
}


/*
*函数介绍：向串口发送数据
*入口参数：buf		: 将要往串口写入的数据的缓冲区
		   bufLen	: 将要往串口写入的数据的缓冲区长度
*出口参数：(无)
*返回值：TRUE:表示成功地将要发送的数据传递到写线程消息队列。
		 FALSE:表示将要发送的数据传递到写线程消息队列失败。
		 注视：此处的TRUE,不直接代表数据一定成功写入到串口了。
*/
BOOL CCESeries::WritePort(const BYTE *buf,DWORD bufLen)
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

/*
*函数介绍：设置串口读取、写入超时
*入口参数：CommTimeOuts : 指向COMMTIMEOUTS结构
*出口参数：(无)
*返回值：TRUE:设置成功;FALSE:设置失败
*/
BOOL CCESeries::SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);
	return SetCommTimeouts(m_hComm,&CommTimeOuts);
}


//串口读线程函数
DWORD CCESeries::ReadThreadFunc(LPVOID lparam)
{
	CCESeries *ceSeries = (CCESeries*)lparam;
	
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
	}
	return 0;
}

//串口写线程函数
DWORD CCESeries::WriteThreadFunc(LPVOID lparam)
{
	CCESeries *ceSeries = (CCESeries*)lparam;
	MSG msg;
	DWORD dwWriteLen = 0;
	BYTE * buf = NULL;
				
	//如果收到写线程退出信号，则退出线程
	while (WaitForSingleObject(ceSeries->m_hWriteCloseEvent,1) == WAIT_TIMEOUT)
	{
		//如果捕捉到线程消息
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
				//向串口写
				buf = (BYTE*)msg.lParam;
				dwWriteLen = (DWORD)msg.wParam;
				//向串口写
				::EnterCriticalSection(&ceSeries->m_Csection);
				WritePort(ceSeries->m_hComm,buf,dwWriteLen);
				//删除动态分配的内存
				::LeaveCriticalSection(&ceSeries->m_Csection);
				//free(buf); // const量，此处不需要释放 [5/14/2012 beihai]
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


//私用方法，用于向串口写数据，被写线程调用
BOOL CCESeries::WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen)
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
								bufLen - dwHaveNumWritten,          //被写数据缓冲区大小
								&dwNumBytesWritten,					//函数执行成功后，返回实际向串口写的个数	
								&ovn);
		if(!bret)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				if(!GetOverlappedResult(hComm,&ovn,&dwNumBytesWritten,TRUE))
					break;
			}else  //这里上下两处跳出，是发送出现了错误的含义
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



//关闭读线程
void CCESeries::CloseReadThread()
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

//关闭写线程
void CCESeries::CloseWriteThread()
{
	SetEvent(m_hWriteCloseEvent);
	
	//清空所有将要写的数据
    PurgeComm( m_hComm,  PURGE_TXCLEAR );
	
    //等待10秒，如果读线程没有退出，则强制退出
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
	//如果没有继承则删除数据
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
	//原始数据直接发送
	if (!m_bNeed7E)
	{
		if (m_pWndOwner != NULL)
		{
			//m_pWndOwner->PostMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)packData);
			m_pWndOwner->SendMessage(WM_RECV_COM_DATA,(WPARAM)this,(LPARAM)packData);		//modified by Amy 2012-9-19
		}
		return;
	}
	//添加自已的处理函数
	if (m_nRecvCount+packData->nLen < FRAME_BUFFER_SIZE)
	{
		//拼接在一起
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
	//处理字符转义并去除帧头与帧尾
	static BYTE temBuff[FRAME_BUFFER_SIZE] = {0};

	int nPackLen = 0;//用于记录数据包的长度

	//标记是否开始匹配
	bool bStartMatch = false;
	//遍历缓冲区
    for (int i = 0; i < m_nRecvCount; i++) 
    {
		if (m_RecvBuff[i] == 0x7E)
		{
			if (!bStartMatch)
			{	
				//如果没有匹配则开始设置匹配
				bStartMatch = true;
				nPackLen = 0;//恢复

				//added by cl
				temBuff[nPackLen] = m_RecvBuff[i];
				nPackLen++;
			}
			else
			{
				//已经匹配的话就将匹配好的数据发送到窗口，
				bStartMatch = false;
				if (nPackLen > 0)
				{
					temBuff[nPackLen] = m_RecvBuff[i];
					nPackLen++;
					//int nActualLen;
					//处理转义字符
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
				//并将后面数据往前移动进行新一轮匹配
				for(int j = i+1;j < m_nRecvCount;j++)
				{
					m_RecvBuff[j-i+1] = m_RecvBuff[j];
					m_RecvBuff[j] = 0;
				}
				m_nRecvCount = m_nRecvCount-i-1;
				//重新匹配剩余的字符 for循环会提前加一
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
	//CCESeries::OnRecv(packData);//调用父类的清理函数  //deleted by amy 2012-9-19
}

BOOL CHDLCSeries::WriteHDLC( const BYTE *buf,DWORD bufLen, BOOL bNeed7E)
{
	//些过程只是简单的为包增加帧头帧尾转换字符
	if(bufLen > FRAME_BUFFER_SIZE) return FALSE;

	memset(m_SendBuff,0,FRAME_BUFFER_SIZE);

	m_bNeed7E = bNeed7E;
	//如果不使用HDLC协议直接发送
	if (!m_bNeed7E)	
	{
		for (int i = 0; i< (int)bufLen; i++)
		{
			m_SendBuff[i] = *buf;
			buf++;
		}
		return WritePort(m_SendBuff,bufLen);
	}

	m_SendBuff[0]  = 0x7E;//增加帧头
	int nInsertPos = 1;    //从缓冲区1处开始写入
	m_nSendCount   = 1;   //数据长度

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
	m_SendBuff[nInsertPos] = 0x7E;//帧尾
	m_nSendCount++;
	return WritePort(m_SendBuff,m_nSendCount);
}

void CHDLCSeries::ConvertData( BYTE *pData,int nLen,int *actLen )
{
	//监视位，用于监视转义后数据的插入位
	int nWatchPos = 0;
	*actLen = 0;
	//遍历
	BYTE *pTemp = pData;
	for (int i=0; i<nLen; i++)
	{
		if(*(pTemp+i) == 0x7F)
		{
			if (*(pTemp+i+1) == 0x80)
			{
				//表示有7E转义
				*(pData+nWatchPos) = 0x7E;
				nWatchPos++;
				(*actLen)++;
				i++;//直接跳过一下个位置
			}
			else if (*(pTemp+i+1) == 0x81)
			{
				//表示有7F转义
				*(pData+nWatchPos) = 0x7F;
				nWatchPos++;
				(*actLen)++;
				i++;//直接跳过一下个位置
			}
			else
			{
				//此处为7F本身，没转义
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
	//最后将真实字节数后的数据清空
	memset((pData+(*actLen)),0,(*actLen));

	if(pTemp != NULL)
	{
		pTemp = NULL;
	}
}
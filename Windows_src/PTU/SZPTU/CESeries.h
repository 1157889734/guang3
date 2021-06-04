// CESeries.h: interface for the CCESeries class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_)
#define AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//CE串口通讯类
// 修改成为一个通用的串口封装，可对串口进行读写
// 可继承OnRecive函数进行协议封装
class CCESeries
{

public:
	CCESeries();
	virtual ~CCESeries();
	CTime m_LastPackTime;
public:
	BOOL GetCommObjState();
	//打开串口
	BOOL OpenPort(UINT portNo	= 2,		/*串口号*/
				  UINT baud		= 9600,		/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0         /*停止位*/
				  );

	//关闭串口
	void ClosePort();
	//设置串口读取、写入超时
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);

	//向串口写入数据,供外部调用
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	CRITICAL_SECTION m_Csection; 

	// 接收到原始数据时触发函数
	virtual void OnRecv(LPCOMM_DATAPACK packData);

private:
    //串口读线程函数
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);

    //协议分析线程函数

	//串口写线程函数
    static DWORD WINAPI WriteThreadFunc(LPVOID lparam);


	//实际向串口写入数据
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//关闭读线程
	void CloseReadThread();
	//关闭写线程
	void CloseWriteThread();
private:
    //已打开的串口句柄
	HANDLE	m_hComm;

	CWnd* m_pPortOwner;

	CString idstr;
	//读写线程句柄
	HANDLE m_hReadThread;
	HANDLE m_hWriteThread;
	HANDLE m_hAnsysThread;
	
	//读写线程ID标识
	DWORD m_dwReadThreadID;
	DWORD m_dwWriteThreadID;
	DWORD m_dwAnsysThreadID;
	
	//读线程退出事件
	HANDLE m_hReadCloseEvent;
	//写线程退出事件
	HANDLE m_hWriteCloseEvent;
	HANDLE m_hAnsysCloseEvent;
};

//封装HDLC串口类，提前处理数据帧

//定义一个最大帧缓冲区
#define FRAME_BUFFER_SIZE 2048  
class CHDLCSeries : public CCESeries
{
public:
	CHDLCSeries();
	//构造默认接收消息窗口
	void SetOwner(CWnd* pOwner);
	//发送HDLC数据，处理过程有增加帧头，字符转义
	BOOL WriteHDLC(const BYTE *buf,DWORD bufLen,BOOL bNeed7E = FALSE); 
private:
	//处理转义字符如遇到
	//如果某个字节为7EH，转化为双字节7F80H，如果某个字节为7FH，转化为双字节7F81H
	//反转化
	//actLen为最后字符长度
	void ConvertData(BYTE *pData,int nLen,int *actLen);

	//重载父类接收数据函数
	void OnRecv(LPCOMM_DATAPACK packData);

	BOOL m_bNeed7E;
	CWnd *m_pWndOwner; //接收数据窗口
	int  m_nSendCount; //缓冲区发送的字节长
	int  m_nRecvCount; //接收区的字节长
	BYTE m_SendBuff[FRAME_BUFFER_SIZE];//发送缓冲区
	BYTE m_RecvBuff[FRAME_BUFFER_SIZE];//接收缓冲区
};

#endif // !defined(AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_)

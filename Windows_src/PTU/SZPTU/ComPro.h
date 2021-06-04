#pragma once

class CComPro
{
public:
	CComPro(void);
	~CComPro(void);
	//打开串口
	BOOL OpenPort(UINT portNo	= 2,		/*串口号*/
		UINT baud		= 9600,		/*波特率*/
		UINT parity	= NOPARITY, /*奇偶校验*/
		UINT databits	= 8,		/*数据位*/
		UINT stopbits	= 0         /*停止位*/
		);

	//关闭串口
	void ClosePort();

	//向串口写入数据,供外部调用
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	

	// 接收到原始数据时触发函数
	void OnRecv(LPCOMM_DATAPACK packData);
	//串口读线程函数
	static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
	//实际向串口写入数据
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//关闭读线程
	void CloseReadThread();
    //设置接收窗口
	void SetOwner(CWnd* pOwner);
public:
	CRITICAL_SECTION m_Csection; 
	HANDLE	m_hComm;

	CWnd* m_pPortOwner;


	//读写线程句柄
	HANDLE m_hReadThread;


	//读写线程ID标识
	DWORD m_dwReadThreadID;


	//读线程退出事件
	HANDLE m_hReadCloseEvent;
	//写线程退出事件
	HANDLE m_hWriteCloseEvent;
	
};




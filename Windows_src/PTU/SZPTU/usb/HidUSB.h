#pragma once


class CHidUSB
{
public:
	CHidUSB(void);
	CHidUSB(CWnd* pWnd, const unsigned short sVID, const unsigned short sPID);
public:
	~CHidUSB(void);

private:
	HANDLE             m_hRead;          //读管道句柄
	HANDLE             m_hWrite;         //写管道句柄
	HANDLE             m_hRecvThread;    //接收数据线程句柄
	unsigned short     m_sDeviceVID;     //所需打开设备VID
	unsigned short     m_sDevicePID;     //所需打开设备PID
	int                m_nBytesToRead;   //可读USB报告长度
	int                m_nBytesToWrite;  //可写USB报告长度
	CWnd*              m_pOwnerWnd;      //父窗口，用于有USB数据时发送消息

public:
	BOOL OpenDevice(void);
	BOOL SendData(LPUSB_DATAPACK pPack);
	void CloseDevice(void);
	BOOL CheckDevice();
    BOOL SendData2(LPUSB_DATAPACK pPack);

private:
	static DWORD WINAPI RecvDataThread(LPVOID lparam);

};

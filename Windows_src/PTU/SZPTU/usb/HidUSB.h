#pragma once


class CHidUSB
{
public:
	CHidUSB(void);
	CHidUSB(CWnd* pWnd, const unsigned short sVID, const unsigned short sPID);
public:
	~CHidUSB(void);

private:
	HANDLE             m_hRead;          //���ܵ����
	HANDLE             m_hWrite;         //д�ܵ����
	HANDLE             m_hRecvThread;    //���������߳̾��
	unsigned short     m_sDeviceVID;     //������豸VID
	unsigned short     m_sDevicePID;     //������豸PID
	int                m_nBytesToRead;   //�ɶ�USB���泤��
	int                m_nBytesToWrite;  //��дUSB���泤��
	CWnd*              m_pOwnerWnd;      //�����ڣ�������USB����ʱ������Ϣ

public:
	BOOL OpenDevice(void);
	BOOL SendData(LPUSB_DATAPACK pPack);
	void CloseDevice(void);
	BOOL CheckDevice();
    BOOL SendData2(LPUSB_DATAPACK pPack);

private:
	static DWORD WINAPI RecvDataThread(LPVOID lparam);

};

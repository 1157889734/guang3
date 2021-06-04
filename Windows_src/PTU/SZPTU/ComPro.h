#pragma once

class CComPro
{
public:
	CComPro(void);
	~CComPro(void);
	//�򿪴���
	BOOL OpenPort(UINT portNo	= 2,		/*���ں�*/
		UINT baud		= 9600,		/*������*/
		UINT parity	= NOPARITY, /*��żУ��*/
		UINT databits	= 8,		/*����λ*/
		UINT stopbits	= 0         /*ֹͣλ*/
		);

	//�رմ���
	void ClosePort();

	//�򴮿�д������,���ⲿ����
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	

	// ���յ�ԭʼ����ʱ��������
	void OnRecv(LPCOMM_DATAPACK packData);
	//���ڶ��̺߳���
	static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
	//ʵ���򴮿�д������
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//�رն��߳�
	void CloseReadThread();
    //���ý��մ���
	void SetOwner(CWnd* pOwner);
public:
	CRITICAL_SECTION m_Csection; 
	HANDLE	m_hComm;

	CWnd* m_pPortOwner;


	//��д�߳̾��
	HANDLE m_hReadThread;


	//��д�߳�ID��ʶ
	DWORD m_dwReadThreadID;


	//���߳��˳��¼�
	HANDLE m_hReadCloseEvent;
	//д�߳��˳��¼�
	HANDLE m_hWriteCloseEvent;
	
};




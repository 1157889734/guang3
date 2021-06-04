// CESeries.h: interface for the CCESeries class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_)
#define AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//CE����ͨѶ��
// �޸ĳ�Ϊһ��ͨ�õĴ��ڷ�װ���ɶԴ��ڽ��ж�д
// �ɼ̳�OnRecive��������Э���װ
class CCESeries
{

public:
	CCESeries();
	virtual ~CCESeries();
	CTime m_LastPackTime;
public:
	BOOL GetCommObjState();
	//�򿪴���
	BOOL OpenPort(UINT portNo	= 2,		/*���ں�*/
				  UINT baud		= 9600,		/*������*/
				  UINT parity	= NOPARITY, /*��żУ��*/
				  UINT databits	= 8,		/*����λ*/
				  UINT stopbits	= 0         /*ֹͣλ*/
				  );

	//�رմ���
	void ClosePort();
	//���ô��ڶ�ȡ��д�볬ʱ
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);

	//�򴮿�д������,���ⲿ����
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	CRITICAL_SECTION m_Csection; 

	// ���յ�ԭʼ����ʱ��������
	virtual void OnRecv(LPCOMM_DATAPACK packData);

private:
    //���ڶ��̺߳���
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);

    //Э������̺߳���

	//����д�̺߳���
    static DWORD WINAPI WriteThreadFunc(LPVOID lparam);


	//ʵ���򴮿�д������
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//�رն��߳�
	void CloseReadThread();
	//�ر�д�߳�
	void CloseWriteThread();
private:
    //�Ѵ򿪵Ĵ��ھ��
	HANDLE	m_hComm;

	CWnd* m_pPortOwner;

	CString idstr;
	//��д�߳̾��
	HANDLE m_hReadThread;
	HANDLE m_hWriteThread;
	HANDLE m_hAnsysThread;
	
	//��д�߳�ID��ʶ
	DWORD m_dwReadThreadID;
	DWORD m_dwWriteThreadID;
	DWORD m_dwAnsysThreadID;
	
	//���߳��˳��¼�
	HANDLE m_hReadCloseEvent;
	//д�߳��˳��¼�
	HANDLE m_hWriteCloseEvent;
	HANDLE m_hAnsysCloseEvent;
};

//��װHDLC�����࣬��ǰ��������֡

//����һ�����֡������
#define FRAME_BUFFER_SIZE 2048  
class CHDLCSeries : public CCESeries
{
public:
	CHDLCSeries();
	//����Ĭ�Ͻ�����Ϣ����
	void SetOwner(CWnd* pOwner);
	//����HDLC���ݣ��������������֡ͷ���ַ�ת��
	BOOL WriteHDLC(const BYTE *buf,DWORD bufLen,BOOL bNeed7E = FALSE); 
private:
	//����ת���ַ�������
	//���ĳ���ֽ�Ϊ7EH��ת��Ϊ˫�ֽ�7F80H�����ĳ���ֽ�Ϊ7FH��ת��Ϊ˫�ֽ�7F81H
	//��ת��
	//actLenΪ����ַ�����
	void ConvertData(BYTE *pData,int nLen,int *actLen);

	//���ظ���������ݺ���
	void OnRecv(LPCOMM_DATAPACK packData);

	BOOL m_bNeed7E;
	CWnd *m_pWndOwner; //�������ݴ���
	int  m_nSendCount; //���������͵��ֽڳ�
	int  m_nRecvCount; //���������ֽڳ�
	BYTE m_SendBuff[FRAME_BUFFER_SIZE];//���ͻ�����
	BYTE m_RecvBuff[FRAME_BUFFER_SIZE];//���ջ�����
};

#endif // !defined(AFX_CESERIES_H__4040241A_FA58_4655_88BA_8D8DF018446D__INCLUDED_)

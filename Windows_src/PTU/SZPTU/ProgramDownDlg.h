#pragma once
#include "afxcmn.h"
#include "usb\HidUSB.h"
#include "CESeries.h"
// CProgramDownDlg �Ի���

class CProgramDownDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgramDownDlg)

public:
	CHidUSB *m_ptctcusb;
	CHidUSB *m_ptatcusb;
	CHDLCSeries *m_ptcom;
	HANDLE	m_hThread;				//�����߳�
	BYTE m_bType;
	int m_nDownloadSize;            //bin�ļ�����
	BYTE*	m_pFileBuffer;			//�����ļ�
	CProgramDownDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgramDownDlg();

// �Ի�������
	enum { IDD = IDD_PRGDOWNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVerquery();
	afx_msg void OnBnClickedSelectfile();
	afx_msg void OnBnClickedBegindown();
	virtual BOOL OnInitDialog();
	static DWORD WINAPI WorkThread(LPVOID lParam);
	void DownLoadProgram();
	afx_msg LRESULT  DownLoadATCProgram(WPARAM wParam,LPARAM lParam);
	void DownLoadLEDMAPProgram(void);
	void DownLoadStartEnd(int flag);
	CProgressCtrl m_procDownload;
	void RefleshVersion(BYTE * tempBuf, int bufcount);
	void RefleshVersionUSB(BYTE * tempBuf, int bufcount);
	void DownLoadAtcStartEnd(int flag);
	afx_msg LRESULT OnSendBeginEndData(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSendMidData(WPARAM wParam,LPARAM lParam);

	
	CRichEditCtrl m_rich;
	void ShowRichMsg(CString str);
	int SetUsbComPt(CHidUSB * m_ptctcusb, CHidUSB * m_ptatcusb, CHDLCSeries * m_ptcom);
	afx_msg void OnBnClickedAtcquery();
	afx_msg void OnCbnSelchangeStreetcomb();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ClearVersion(void);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

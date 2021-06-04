#pragma once
#include "afxcmn.h"


// CSelFileDlg �Ի���

class CSelFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelFileDlg)

public:
	int m_filesize;
	int m_addsize;
	CSelFileDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelFileDlg();

// �Ի�������
	enum { IDD = IDD_SELFILEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSendselfilecomd();
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDownload();
	afx_msg LRESULT OnRecvFileData( WPARAM wParam,LPARAM lParam);//�ļ���Ϣ���պ���
	afx_msg LRESULT OnRecvFileFinish( WPARAM wParam,LPARAM lParam);//�ļ���������ص�������
	afx_msg LRESULT OnRecvFileCount( WPARAM wParam,LPARAM lParam);//�ļ���������ص�������
	afx_msg void OnBnClickedDeletefile();
	afx_msg void OnBnClickedFormat();
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedDownloadcancle();
	void SendHeartData(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};

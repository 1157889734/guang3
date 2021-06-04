#pragma once
#include "afxcmn.h"


// CSelFileDlg 对话框

class CSelFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelFileDlg)

public:
	int m_filesize;
	int m_addsize;
	CSelFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelFileDlg();

// 对话框数据
	enum { IDD = IDD_SELFILEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSendselfilecomd();
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDownload();
	afx_msg LRESULT OnRecvFileData( WPARAM wParam,LPARAM lParam);//文件信息接收函数
	afx_msg LRESULT OnRecvFileFinish( WPARAM wParam,LPARAM lParam);//文件传输结束回调函数。
	afx_msg LRESULT OnRecvFileCount( WPARAM wParam,LPARAM lParam);//文件传输结束回调函数。
	afx_msg void OnBnClickedDeletefile();
	afx_msg void OnBnClickedFormat();
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedDownloadcancle();
	void SendHeartData(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};

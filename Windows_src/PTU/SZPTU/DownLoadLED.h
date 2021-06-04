#pragma once
#include "afxcmn.h"
#include "SZPTU.h"
#include "MyButton.h"
#include "LittaceSet.h"

// CDownLoadLED dialog

class CDownLoadLED : public CDialog
{
	DECLARE_DYNAMIC(CDownLoadLED)

public:
	CBrush m_brush2;
	HANDLE	m_hThread;				//œ¬‘ÿœﬂ≥Ã
	BOOL m_isdownall;
	CDownLoadLED(CWnd* pParent = NULL, LPWND_INFO pInfo = NULL);   // standard constructor
	virtual ~CDownLoadLED();

	void InitPanel();
	void DoLoadList();
	void LayoutList();
	afx_msg void OnBnClickedButtonMultiDown();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDType();
	afx_msg void OnBnClickedCheckAll();

// Dialog Data
	enum { IDD = IDD_DIALOG_DOWN_ALL_LED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list1;
	CProgressCtrl m_procCtrl;
	CLatticeSet m_cLatticeSet;
	WND_INFO m_WndInfo;
	CMyButton	m_btnClose;
	CMyButton	m_btnDownload;
	CMyButton	m_btnDownloadAll;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonClose();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void DownOneType(int isel_type);
	void SetDownAll(bool isdownall);
	afx_msg void OnBnClickedButtonMultiDownall();
	static DWORD WINAPI WorkThread(LPVOID lParam);
	void DownAllPic(void);
	afx_msg void OnCbnSelchangeComboDCitytype();
};

#pragma once

#include "MyButton.h"
// CPortLinkDlg 对话框

class CPortLinkDlg : public CDialog
{
	DECLARE_DYNAMIC(CPortLinkDlg)

public:
	CMyButton m_atcrestbtn;
	CMyButton m_ctcbtn;
	CMyButton m_atcbtn;
	CMyButton m_combtn;
	CPortLinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPortLinkDlg();
   
// 对话框数据
	enum { IDD = IDD_PORTLINK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void InitComPort();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCtcusb();
	afx_msg void OnBnClickedAtcusb();
	afx_msg void OnBnClickedPctcom();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAtcreset();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
extern void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown, CString strEnter, CString strDisable);
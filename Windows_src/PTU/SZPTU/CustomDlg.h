#pragma once
#include "afxcmn.h"
#include "OperateDlg.h"
//#include "ConfigurationDlg.h"
#include "DeleteDlg.h"


// CCustomDlg dialog

class CCustomDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomDlg)

public:
	CCustomDlg(CWnd* pParent/* = NULL*/);   // standard constructor
	virtual ~CCustomDlg();

// Dialog Data
	enum { IDD = IDD_CUSTOMDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//CTabCtrl m_tabCustom;
	void InitTab();
	void InitDlg();
	void DoTab(UINT nTab);
	void SetDlgState(CWnd *pWnd, BOOL bShow);
	COperateDlg m_cOperateDlg;
	CDeleteDlg m_cDeleteDlg;
	virtual BOOL OnInitDialog();
	//afx_msg void OnTcnSelchangeTabCustom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnPaint();
	CMyButton	m_btnAdd;
	CMyButton	m_btnDelete;
	CMyButton	m_btnClose;
};

extern void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown, CString strEnter, CString strDisable);
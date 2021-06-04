#pragma once

#include "stdafx.h"
#include "SZPTU.h"
#include "MyButton.h"

// COperateDlg dialog

class COperateDlg : public CDialog
{
	DECLARE_DYNAMIC(COperateDlg)

public:
	CFont m_font2;
	CBrush m_brush2;
	COperateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COperateDlg();

// Dialog Data
	enum { IDD = IDD_OPERATEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCustomAdd();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void InitDlg();

	CButton m_radioTip;
	CButton m_radioAd;
	CButton m_radioBroadcast;
	CMyButton	m_btnAdd;
	CMyButton	m_btnAd;
	CMyButton	m_btnTip;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadioTip();
	afx_msg void OnBnClickedRadioAd();
};

extern void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown, CString strEnter, CString strDisable);
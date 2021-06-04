#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "SZPTU.h"
#include "MyButton.h"

// CDeleteDlg dialog

class CDeleteDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeleteDlg)

public:
	CBrush m_brush2;
	CDeleteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeleteDlg();

	// Dialog Data
	enum { IDD = IDD_DELETEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void LayoutList();
	void DoLoadList();
	void DeleteConfigItem(int iType, int iNo);		//�������ļ���ɾ��������ʾ����Ϣ
	void DeleteAdItem(int iNo);			//�������ļ���ɾ��ѡ�еĹ����Ϣ
	void DeleteTipItem(int iNo);		//�������ļ���ɾ��ѡ�е���ʾ��Ϣ
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedCheckAll();	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CListCtrl m_list;
	CButton m_radioTip;
	CButton m_radioAd;
	CButton m_radioBroadcast;
	CMyButton	m_btnDelete;
	afx_msg void OnBnClickedRadioAd();
	afx_msg void OnBnClickedRadioTip();
	afx_msg	LRESULT OnAddMsg(WPARAM wParam,LPARAM lParam);	
	afx_msg	LRESULT OnDeleteMsg(WPARAM wParam,LPARAM lParam);	
public:
	afx_msg void OnPaint();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadioBroadcast();
	void DeleteBroadcastItem(int iNo);
};

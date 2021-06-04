#pragma once
#include "afxcmn.h"
#include "SelFileDlg.h"
#include "RTCTimeSetdlg.h"

#include "DlgHistoricalView.h"
// CDevStatusQueryDlg �Ի���


class CProtocolAnalysis;


typedef enum enumHisDataListViewType{
	SHOW_LIST_STYLE_NONE,
	SHOW_LIST_BASIC_INFO,
	SHOW_LIST_TCMS_DATA,
	SHOW_LIST_PISC_DATA,
}HisDataListViewType;




class CDevStatusQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CDevStatusQueryDlg)

public:
	CRTCTimeSetdlg m_rtctimesetdlg;
	int filecount;
	BYTE * ptbuf;
	CFile file;
	CString binFilePath;
	CDevStatusQueryDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDevStatusQueryDlg();

	BOOL  m_bStopQuerData;
// �Ի�������
	enum { IDD = IDD_BREAKQUERYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSelFileDlg m_selfiledlg;
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSelectfileButton();
	afx_msg void OnBnClickedSelectlocalfile();
	afx_msg void OnBnClickedButton1();
	void ShowListItem(CString strbegintime, CString strendtime, CString strcount);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	
	int InsertTMSDataToPanle();
	int InsertPISCDataToPanle();

	CRichEditCtrl m_rich;
	afx_msg void OnBnClickedCleanscreen();
	afx_msg void OnBnClickedRtcset();
	afx_msg void OnBnClickedCleanscreenfile();

	int GetCompatibleViewStyle(const CString &strFileName);
	CProtocolAnalysis * GetCompatibleProtocolAnalysizer(const CString &strFileName);

	void ShowTimeTxtData(void );
	void ShowProtocolTxtData(int nViewStyle);

	afx_msg void OnClose();

private:
	CProtocolAnalysis *m_pTcmsProtocol;
	CProtocolAnalysis *m_pPiscProtocol;
	CDlgHistoricalView m_dlgHistoricalView;
	int m_nCurrViewStyle;
	afx_msg void OnPaint();
};

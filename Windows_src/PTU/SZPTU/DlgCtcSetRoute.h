#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgCtcSetRoute �Ի���
class CMainFrame;


class CDlgCtcSetRoute : public CDialog
{
	DECLARE_DYNAMIC(CDlgCtcSetRoute)

public:
	CDlgCtcSetRoute(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCtcSetRoute();

// �Ի�������
	enum { IDD = IDD_DLG_CTC_SET_ROUTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��ť��Ӧ
	afx_msg void OnBnClickedBtnCtcSettingRead();
	afx_msg void OnBnClickedBtnRouteSettingSync();
	afx_msg void OnBnClickedBtnNewRouteSetting();
	afx_msg void OnBnClickedBtnRouteSettingClear();
	afx_msg void OnBnClickedBtnDeleteRouteSetting();
	afx_msg void OnStnClickedBtnSelectAll();
	afx_msg void OnStnClickedBtnSelectReverse();
	afx_msg void OnCbnSelchangeComboTerminus();
	afx_msg void OnNMClickListRoutes(NMHDR *pNMHDR, LRESULT *pResult);

	void ShowRouteNotifySet(BYTE byteStation, DWORD dwNotificationSet);
	BOOL CollectOneWayRouteSetting(OUT BYTE &byteStation, OUT DWORD &dwNotificationSet);

	void InitializeListRoutes(void );
	void InitializeListNotifySet(void );

	BOOL SendRouteSetting(void );
	void SetMainFrameCTCCommandType(int cmd);

	// ��ʾ����
	void UpdateRouteList(BYTE *tempBuf, int bufcount);

	BYTE SumUp(const BYTE *pData, const int nLen);



private:
	DWORD *m_arrayStationNotify;  // ����
	
	DWORD *m_arrayStationNotifyTemp; // ��ͼ
	int m_nUpdatedStationNum;

	CImageList m_imglstRouteTable;
	CListCtrl m_listRoutes;
	CComboBox m_combTerminus;
	CListCtrl m_listNotifySet;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	int m_nCurrSyncStationCode;
	CStatic m_stcRouteQueryProcess;
	CStatic m_stcRouteSyncProcess;
	afx_msg void OnPaint();
};

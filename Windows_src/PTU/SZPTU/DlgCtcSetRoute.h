#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgCtcSetRoute 对话框
class CMainFrame;


class CDlgCtcSetRoute : public CDialog
{
	DECLARE_DYNAMIC(CDlgCtcSetRoute)

public:
	CDlgCtcSetRoute(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCtcSetRoute();

// 对话框数据
	enum { IDD = IDD_DLG_CTC_SET_ROUTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 按钮相应
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

	// 显示函数
	void UpdateRouteList(BYTE *tempBuf, int bufcount);

	BYTE SumUp(const BYTE *pData, const int nLen);



private:
	DWORD *m_arrayStationNotify;  // 设置
	
	DWORD *m_arrayStationNotifyTemp; // 视图
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

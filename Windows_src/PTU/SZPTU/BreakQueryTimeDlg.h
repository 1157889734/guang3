#pragma once


// CBreakQueryTimeDlg 对话框

class CBreakQueryTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(CBreakQueryTimeDlg)

public:
	BYTE m_tmscount;
	BYTE m_piscount;
	int blockstatus[3];
	int m_ksstatus[6],m_ksstatic[6];
	int m_bjqstatus[18],m_bjqstatic[18];
	int m_WBSstatus[24],m_WBSstatic[24];
	int m_DRMCstatus[6],m_DRMCstatic[6];
	
	int newpisflag;
	int oldpisflag;
	CBreakQueryTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBreakQueryTimeDlg();

// 对话框数据
	enum { IDD = IDD_BREAKQUERYTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void SendHeartData(void);
	void ChangColor(int nID/*, int iState*/);
	int GetIDIndex(int staticid, int & groupint);
	afx_msg void OnBnClickedQuery();
	afx_msg LRESULT OnRecvBlockStatus( WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRecvPISStatus( WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRecv485Status( WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRecvTMSStatus( WPARAM wParam,LPARAM lParam);
	void QueryDeviceStatus(int flag);
	
	afx_msg void OnBnClickedQuerytms();
	afx_msg void OnBnClickedQuerypis();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

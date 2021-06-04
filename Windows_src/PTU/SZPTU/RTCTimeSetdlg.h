#pragma once


// CRTCTimeSetdlg 对话框

class CRTCTimeSetdlg : public CDialog
{
	DECLARE_DYNAMIC(CRTCTimeSetdlg)

public:
	CRTCTimeSetdlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRTCTimeSetdlg();

// 对话框数据
	enum { IDD = IDD_RTCSETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNativetimeset();
	afx_msg void OnBnClickedIntimeset();
	CTime m_longtime;
	CTime m_shorttime;
	void ShowMsg(CString str);
	afx_msg void OnBnClickedQuerytime();
	void ShowRecTimeMsg(CString str);
	virtual BOOL OnInitDialog();
};

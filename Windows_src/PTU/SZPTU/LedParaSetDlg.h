#pragma once


// CLedParaSetDlg 对话框

class CLedParaSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CLedParaSetDlg)

public:
	CLedParaSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLedParaSetDlg();

// 对话框数据
	enum { IDD = IDD_LEDPARASETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_lanselint;
	int m_mescount;
};

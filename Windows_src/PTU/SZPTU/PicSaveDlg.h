#pragma once


// CPicSaveDlg 对话框

class CPicSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CPicSaveDlg)

public:
	int m_piccount;
	CPicSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicSaveDlg();
    int pictype; 
	int languageint; 
	int picNO;
// 对话框数据
	enum { IDD = IDD_PICSAVEFILEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int GetFileCount(CString csDirPath);
	int GetPicCount();
	void SetParates(int pictype, int languageint, int picNO);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

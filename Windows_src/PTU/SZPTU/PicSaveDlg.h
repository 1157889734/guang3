#pragma once


// CPicSaveDlg �Ի���

class CPicSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CPicSaveDlg)

public:
	int m_piccount;
	CPicSaveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPicSaveDlg();
    int pictype; 
	int languageint; 
	int picNO;
// �Ի�������
	enum { IDD = IDD_PICSAVEFILEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int GetFileCount(CString csDirPath);
	int GetPicCount();
	void SetParates(int pictype, int languageint, int picNO);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

#pragma once


// CLedParaSetDlg �Ի���

class CLedParaSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CLedParaSetDlg)

public:
	CLedParaSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLedParaSetDlg();

// �Ի�������
	enum { IDD = IDD_LEDPARASETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_lanselint;
	int m_mescount;
};

#pragma once


// CRTCTimeSetdlg �Ի���

class CRTCTimeSetdlg : public CDialog
{
	DECLARE_DYNAMIC(CRTCTimeSetdlg)

public:
	CRTCTimeSetdlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRTCTimeSetdlg();

// �Ի�������
	enum { IDD = IDD_RTCSETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

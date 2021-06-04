#pragma once


// CDeviceQueryCTCDlg 对话框

class CDeviceQueryCTCDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeviceQueryCTCDlg)

public:
	BYTE m_ctccount;
	int m_ctcbuf[16+6],m_staticidctc[16+6];
	int m_ksbuf1[15],m_staticidks1[15];
	int m_ksbuf2[15],m_staticidks2[15];
	int m_ksbuf3[15],m_staticidks3[15];
	int m_ksbuf4[15],m_staticidks4[15];
	int m_ksbuf5[15],m_staticidks5[15];
	int m_ksbuf6[15],m_staticidks6[15];
	CDeviceQueryCTCDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceQueryCTCDlg();

// 对话框数据
	enum { IDD = IDD_DEVICE_QUERY_CTC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void ChangColor(int nID/*, int iState*/);
	afx_msg void OnBnClickedQuery();
	void RecStateInfo(BYTE * tempBuf, int bufcount);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	int GetIDIndex(int staticid, int & groupint);
public:
	afx_msg void OnPaint();
};

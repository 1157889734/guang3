#pragma once
#include "afxcmn.h"
#include <vector>
using namespace std;

// CDownByTypeDlg 对话框

class CDownByTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CDownByTypeDlg)

public:
	int streetint;//0表示com，1表示usb
	int iPicColumns;
	vector<BYTE> m_vMapData;
	int nBmpCount;
	CStringArray m_Filenamearray;
	CStringArray m_NOarray;
	int colint;//红绿蓝表示123
	CDownByTypeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownByTypeDlg();

// 对话框数据
	enum { IDD = IDD_DOWNBYTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void SetNOArray();
	BOOL GetFileFromDir(CString csDirPath);
	virtual BOOL OnInitDialog();
	CListCtrl m_listAllDown;
	void SetListValues(int itype,int ilan);
	afx_msg void OnBnClickedCheckall();
	afx_msg void OnCbnSelchangeTypecomb();
	afx_msg void OnCbnSelchangeLancomb();
	afx_msg void OnBnClickedDownbutton();
	CProgressCtrl m_procCtrl;
	void GetBmpPix(CString strFileName);
	byte Littlebag(byte b7,byte b6,byte b5,byte b4,byte b3,byte b2,byte b1,byte b0);
	void SetColorint(int colorint,int streetint);
	void ShowListItem(CString strbegintime, CString strendtime, CString strcount);
	void DownStartPack(void);
	void DownEndPack(void);
};

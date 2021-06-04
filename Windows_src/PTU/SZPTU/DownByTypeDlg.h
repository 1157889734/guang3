#pragma once
#include "afxcmn.h"
#include <vector>
using namespace std;

// CDownByTypeDlg �Ի���

class CDownByTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CDownByTypeDlg)

public:
	int streetint;//0��ʾcom��1��ʾusb
	int iPicColumns;
	vector<BYTE> m_vMapData;
	int nBmpCount;
	CStringArray m_Filenamearray;
	CStringArray m_NOarray;
	int colint;//��������ʾ123
	CDownByTypeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDownByTypeDlg();

// �Ի�������
	enum { IDD = IDD_DOWNBYTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CustomDlg.h"
#include "IHListBox.h"
#include "list"
#include "vector"
#include <afxmt.h>

using namespace std;

// CStationCfg dialog

class CStationCfg : public CDialog
{
	DECLARE_DYNAMIC(CStationCfg)

public:
	CStationCfg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationCfg();

	void DoLoadList();
	void LayoutList();
	void DoLoadAdsAndTipsList();
	void DeleteTipsAndAdsItem(CListBox* pWnd);
	void InitList();
	void InitDlg();
	void InitCombo();
	void DownLoadStaticPic(UINT nMethod , UINT nStation , UINT nLine = 0, UINT nLan = 0, UINT nPreClr = 0, UINT nArrivalClr = 0);
	//void ReadSystemConfig();		//��Config.ini�ж�ȡϵͳ������Ϣ
	//void ReadStationConfig(UINT uLine, UINT uStation);		//��Config.ini�ж�ȡվ������Ϣ
	//void AnalysisConfigData();			//�����������ļ��ж�ȡ������

// Dialog Data
	enum { IDD = IDD_DIALOG_STATION_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCtrl;
public:
	afx_msg void OnBnClickedCheckCfgAll();
	//afx_msg void OnBnClickedButtonCfgSet();	
	afx_msg void OnCbnSelchangeComboCfgLine();
	afx_msg void OnBnClickedButtonSetSyslan();
	afx_msg void OnCbnSelchangeComboCfgLan();
	afx_msg void OnBnClickedButtonPreTipsAdd();
	afx_msg void OnBnClickedButtonPreAdsAdd();
	afx_msg void OnBnClickedButtonArrivalTipsAdd();
	afx_msg void OnBnClickedButtonArrivalAdsAdd();
	afx_msg void OnBnClickedButtonStationCfg();
	afx_msg void OnBnClickedButtonPreTipsDel();
	afx_msg void OnBnClickedButtonPreAdsDel();
	afx_msg void OnBnClickedButtonArrivalTipsDel();
	afx_msg void OnBnClickedButtonArrivalAdsDel();
	virtual BOOL OnInitDialog();

public:
	CFont m_font2;
	CIHListBox m_listPreTipsSelected;
	CIHListBox m_listArrivalAdsSelected;
	CIHListBox m_listArrivalTipsSelected;
	CIHListBox m_listPreAdsSelected;
	CIHListBox m_listTip;
	CIHListBox m_listAd;
	int		   m_nselRow;
	BOOL	   m_bOption;
	CMutex	   m_Mutex;
	list<BYTE> m_listArr[2][255];//��������
	BOOL m_bEnglish;			//TRUE-Enlish,FALSE-Iran 
	list<BYTE> m_list;			//���ڴ洢ѡ�еĹ���Ｐ��ʾ��
	UINT m_uDispLan;			//���ڴ洢LED����ʾ�����֡�1��Ӣ�� 2����˹�� 3���߶���ʾ
	UINT m_uPreColor[2];			//Ԥ��վ��ʾ��ɫ��1�� 2�� 3��
	UINT m_uArrivalColor[2];		//��վ��ʾ��ɫ��1�� 2�� 3��
	vector<CString> m_vecEn;		//���ڴ洢�������ļ��ж�ȡ��Ӣ��վ������Ϣ
	vector<CString> m_vecIran;		//���ڴ洢�������ļ��ж�ȡ�Ĳ�˹��վ������Ϣ
	UINT m_nPercet;			//�������ý���
	CMyButton	m_btnSysCfg;
	CMyButton	m_btnStationCfg;
	CMyButton	m_btnPreTipDel;
	CMyButton	m_btnPreTipAdd;
	CMyButton	m_btnPreAdDel;
	CMyButton	m_btnPreAdAdd;
	CMyButton	m_btnArrTipAdd;
	CMyButton	m_btnArrTipDel;
	CMyButton	m_btnArrAdAdd;
	CMyButton	m_btnArrAdDel;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	afx_msg void OnLbnDblclkListTip();
	afx_msg void OnLbnDblclkListAd();
	afx_msg void OnTipAdd();
	afx_msg void OnAdAdd();
	afx_msg void OnAdModify();
	afx_msg void OnAdDelete();
	//afx_msg void OnBnClickedButtonSysDown();
	//afx_msg void OnBnClickedButtonStationDown();
	afx_msg LRESULT OnAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteMsg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult);

	void OnDeleteListBoxString(CIHListBox *plistBox);

	void OnSetPreTipsSelectedInfo(list<BYTE> *plist);
	void OnSetArrivalTipsSelectedInfo(list<BYTE> *plist);
	void OnSetPreAdsSelectedInfo(list<BYTE> *plist);
	void OnSetArrivalAdsSelectedInfo(list<BYTE> *plist);

	void OnlistCopyData(list<BYTE> *plistDes, list<BYTE> *plistSrc, BOOL bClear=TRUE);

	void ReadFileStationInfo();
	void WriteFileStationInfo();
};

extern void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown, CString strEnter, CString strDisable);
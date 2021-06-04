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
	//void ReadSystemConfig();		//从Config.ini中读取系统配置信息
	//void ReadStationConfig(UINT uLine, UINT uStation);		//从Config.ini中读取站配置信息
	//void AnalysisConfigData();			//分析从配置文件中读取的数据

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
	list<BYTE> m_listArr[2][255];//两种语言
	BOOL m_bEnglish;			//TRUE-Enlish,FALSE-Iran 
	list<BYTE> m_list;			//用于存储选中的广告语及提示语
	UINT m_uDispLan;			//用于存储LED屏显示的语种—1仅英文 2仅波斯文 3两者都显示
	UINT m_uPreColor[2];			//预到站显示颜色—1红 2绿 3橙
	UINT m_uArrivalColor[2];		//到站显示颜色—1红 2绿 3橙
	vector<CString> m_vecEn;		//用于存储从配置文件中读取的英文站配置信息
	vector<CString> m_vecIran;		//用于存储从配置文件中读取的波斯文站配置信息
	UINT m_nPercet;			//下载配置进度
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
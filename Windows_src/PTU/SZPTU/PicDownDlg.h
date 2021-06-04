#pragma once
#include "PicSaveDlg.h"
#include "GridCtrl_src/GridCtrl.h"
#include "LedParaSetDlg.h"
#include "DownByTypeDlg.h"
// CPicDownDlg 对话框
#include <vector>
using namespace std;
#include "MyButton.h"
typedef struct  
{
	BYTE bStart;    //升级开始
	BYTE bStop;     //升级结束
	BYTE bFirstPack;//是否第一包数据
	BYTE bLastPack; //是否最后一包数据
	BYTE bEn;       //是否英文
	BYTE bReserve;  //无用
}bFlag; 
class CPicDownDlg : public CDialog
{
	DECLARE_DYNAMIC(CPicDownDlg)

public:
	CMyButton m_desbtn;
	CMyButton m_selectfont;
	CMyButton m_impottn;
	CMyButton m_setsizebtn;
	CMyButton m_clearbtn;
	CMyButton m_savebtn;
	CMyButton m_creensetbtn;
	CMyButton m_singledownbtn;
	CMyButton m_typedownbtn;

	int m_msgcount;
	CDownByTypeDlg m_downbytypedlg;
	 vector<BYTE> m_vBmpByte;	
	CStringArray m_Filenamearray;
	CStringArray m_NOarray;
	CPicDownDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicDownDlg();

// 对话框数据
	enum { IDD = IDD_PICDOWNDLG };
    /////////////////////////////////////////////
	CPicSaveDlg m_picsavedlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPicsave();
	
	CGridCtrl m_pGrid;
	CLedParaSetDlg m_ledparasetdlg;
	afx_msg void OnBnClickedLedparaset();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangePictypecomb();
	BOOL GetFileFromDir(CString csDirPath);
	BOOL GetPicFile();
	void SetNOArray();
	void SetNOComb();
	afx_msg void OnCbnSelchangePiclancomb();
	void SetLatticeSize(int iWidth, int iHeight);
	UINT m_gridhigh;
	UINT m_gridwidth;
	afx_msg void OnBnClickedSetgridwidth();
	afx_msg void OnBnClickedCleargrid();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDowm();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedRight();
	void MoveBmp(int direction);
	void DrawPic( int iRow,int iCol,int iFlag );
	afx_msg void OnBnClickedDescrible();
	afx_msg void OnBnClickedChecktext();
	BOOL m_radiotxt;
	afx_msg void OnBnClickedLoadpic();
	void ImportStationPic( CString strFileName );
	void SaveClttice( int iPicWidth,int iPicHeight,CString sSavepath );
	int GetEncoderClsid( const WCHAR *format, CLSID *pClsid );
	afx_msg void OnBnClickedSigledown();
	void GetBmpByte();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeStationnocomb();
	afx_msg void OnCbnSelchangeOthernocomb();
	afx_msg void OnCbnSelchangeStreetcomb();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

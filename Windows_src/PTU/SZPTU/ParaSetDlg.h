#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CParaSetDlg 对话框

class CParaSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CParaSetDlg)

public:

	CParaSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParaSetDlg();

	CSliderCtrl m_Gobal_Volume;
	CSliderCtrl m_Occ_Volume;
	CSliderCtrl m_Apu_Treble;
	CSliderCtrl m_Dva_Volume;	
	CSliderCtrl m_Media_Volume;
	CSliderCtrl m_Door_Volume;	
	CSliderCtrl m_Apu_Bass;
// 对话框数据
	enum { IDD = IDD_PARASETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSliderApuGlobal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderOcc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderDvaVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderMediaValume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderDoorVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderApuBass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderApuTreble(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedWbccount();
	afx_msg void OnBnClickedPriset();
	virtual BOOL OnInitDialog();
	void IniComb(int * nid, int count);
	afx_msg void OnBnClickedPrirece();
	void Refleshpri(BYTE * buf, int bufcount);
	afx_msg void OnBnClickedSetvoice();
	afx_msg void OnBnClickedGetvoice();
	void InitVolumeInfo();
	void GetVolumeValue(VolumeInfo &m_VolumeInfo);

	void RecVolumeInfo(BYTE * buf, int count);
	void RecBroadCastBoxVolumeInfo(BYTE *buf, int iCount);
	afx_msg void OnBnClickedIni();
	UINT m_emgcountint;
	int HaveSamePro(int * buf);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	CComboBox m_comboxBroadCastBoxVolType;
public:
	CStatic m_staticBCBoxVol;
public:
	CSliderCtrl m_sliderCtlBCBoxVol;
public:
	afx_msg void OnBnClickedButtonSetbcboxvol();
public:
	afx_msg void OnBnClickedButtonGetbcboxvol();
public:
	afx_msg void OnCbnSelchangeComboBroadcastboxType();
public:

private:
	BYTE m_arrBroadCastBoxVol[6];
	afx_msg void OnNMCustomdrawSliderBcboxvolume(NMHDR *pNMHDR, LRESULT *pResult);
};

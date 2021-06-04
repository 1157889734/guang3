#pragma once
#include "GridCtrl_src/GridCtrl.h"
#include "MyEdit.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include "SZPTU.h"
#include "MyButton.h"
//#include "CfgBuffer.h"
//#include "CustomDlg.h"
using namespace std;

// CLatticeSet dialog
class CLatticeSet : public CDialog
{
	DECLARE_DYNAMIC(CLatticeSet)

public:
	int m_inputeditidshow;
	CLatticeSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLatticeSet();

// Dialog Data
	enum { IDD = IDD_DIALOG_LATTICE };

	//�����ѱ����ͼƬ����ͼƬ·�����������ͣ�������ţ���ʾ��ɫ,��������Э���������ͼƬ����Э��
	void DownLoadStaticPic(int nNo
		, int nCount
		, CString strFileName
		,UINT nMethod = 0
		,UINT nLedType = IDU_VALUE
		,UINT nType = 1
		,UINT nNum = 1
		,UINT nClr = 1
		,UINT nLan = 0);

	void SendDefaultPack();//���غ���һ��Ĭ�ϵ�վ������

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CGridCtrl m_pGrid;
	CMyEdit m_LatticeWidth;
	CMyEdit m_LatticeHeight;
	CSpinButtonCtrl m_spinWidth;
	CSpinButtonCtrl m_spinHeight;
	CProgressCtrl m_proCtrl;
	CStatic m_Pic;
	Bitmap  *m_bmImage;
	HBITMAP m_hBitmap;
	vector<BYTE> m_vBmpByte;		//���ͼƬ����

	//�Զ��尴ť�ؼ�
	CMyButton	m_btnDownSingle;
	CMyButton	m_btnDownByType;
	CMyButton	m_btnDownAll;
	CMyButton	m_btnFont;
	CMyButton	m_btnImportImage;
	CMyButton	m_btnDescribe;
	CMyButton	m_btnSetGrid;
	CMyButton	m_btnCustomize;
	CMyButton	m_btnSaveBmp;
	CMyButton	m_btnClearBmp;
	CMyButton	m_btnUp;
	CMyButton	m_btnDown;
	CMyButton	m_btnLeft;
	CMyButton	m_btnRight;

	virtual BOOL OnInitDialog();
	void loadImageFromPath(CString sPath);			//����ͼƬ
	int  GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
	void SaveClttice(int iPicWidth,int iPicHeight,CString sSavepath);//����ͼƬ
	void GetBmpByte();								//��ͼƬ����תΪ�������ص��ֽ�����
	void DrawPic(int iRow,int iCol,int iFlag );
	void DlgInit();									//�ؼ���ʼ��
	void DoLoadCfg();								//��ʼ��վ���ơ���ʾ���������Ϣ
	void SetLatticeSize(int iWidth,int iHeight);	//��������ؼ��Ŀ���
	void ImportStationPic(CString strFileName);		//�Ӽ���ͼƬ
	void DeleteConfigItem(int iType, int iNo);		//�������ļ���ɾ��������ʾ����Ϣ
	void LoadBmpImage();		//������������λͼ��Դ

public:
	afx_msg void OnBnClickedButtonSetGrid();
	afx_msg void OnCbnSelchangeComboMode();
	afx_msg void OnBnClickedButtonDes();
	afx_msg void OnBnClickedButtonClearLattice();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonImportImage();
	afx_msg void OnBnClickedButtonSaveLattice();
	afx_msg void OnBnClickedButtonDownSingle();
	afx_msg void OnCbnSelchangeComboDownloadType();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnCbnSelchangeComboLineNo();
	afx_msg void OnCbnSelchangeComboStationNo();
	afx_msg void OnCbnSelchangeComboLan();
	afx_msg void OnCbnSelchangeComboDataList();
	afx_msg void OnBnClickedButtonFont();
	afx_msg void OnBnClickedButtonDownByType();
	afx_msg void OnBnClickedButtonAllDown();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg LRESULT OnAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnChangeEditInputText();
};

// MainFrm.h : CMainFrame ��Ľӿ�
//
#include "PortLinkDlg.h"
#include "ParaSetDlg.h"
#include "ProgramDownDlg.h"
#include "DevStatusQueryDlg.h"
#include "PicDownDlg.h"
#include "BreakQueryTimeDlg.h"
#include "DeviceQueryCTCDlg.h"
#include "DlgCtcSetRoute.h"
#include "usb\HidUSB.h"
#include "ComPro.h"
#include "LittaceSet.h"
#include "StationCfg.h"
#include "CESeries.h"

#pragma once

#define         USB_BUFFER_SIZE			512

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	CString m_filepath;
	CFile file3;
	BOOL isfileopen;
	CStringArray m_filesize;
	CDeviceQueryCTCDlg m_DeviceQueryCTCDlg;
	CFile file;
	CStringArray m_filebufarray;
	BOOL isatcbufempty;
	CBreakQueryTimeDlg  m_BreakQueryTimeDlg;
	BOOL            m_ctcusbisopen;
	BOOL            m_atcusbisopen;
	BOOL            m_comisopen;
    CReBar		    m_wndReBar; 
	CPortLinkDlg    m_portlinkdlg;
	CParaSetDlg     m_parasetdlg;
	CProgramDownDlg m_programdlwndlg;
	CDevStatusQueryDlg  m_devstatusquerydlg;
	//CPicDownDlg     m_picdowndlg;
	CLatticeSet		 m_picdowndlg;
	CStationCfg		 m_cStationCfg;
	CDlgCtcSetRoute m_dlgCtcSetRout;
	CHidUSB *m_ptctcusb;
	CHidUSB *m_ptatcusb;
	//CComPro m_com;
	CHDLCSeries m_com;
	BYTE m_bRecBufferComm[COMM_BUFFER_SIZE];	//���ڽ��ջ�����
	int m_nRecCntComm;
	BYTE m_bRecBufferUsb[COMM_BUFFER_SIZE];	//usb���ջ����� ctc
	int m_nRecCntUsb;
	BYTE m_bRecBufferUsbATC[720];	//usb���ջ����� atc
	int m_nRecCntUsbATC;

	int m_nCurrCTCCmdType; // ��ǰCTC���ÿ������

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
  
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLinkDev();
	afx_msg void OnParaSet();
	afx_msg void OnPicUpdata();
	afx_msg void OnProgramUpdata();
	afx_msg void OnBreakQuery();
	afx_msg void OnTMS();
	afx_msg void OnTMSQuery();
	afx_msg void OnBreakQueryTime();
	afx_msg void OnSetRoute();
	afx_msg void OnStationInfo();
	
	afx_msg LRESULT OnRecvComData( WPARAM wParam,LPARAM lParam);//���ڽ��մ�����
	afx_msg LRESULT OnRecvUsbData( WPARAM wParam,LPARAM lParam);//usb���մ�����
	afx_msg LRESULT OnUsbCheckError( WPARAM wParam,LPARAM lParam);//usb���ݳ���У��ʧ��
	DECLARE_MESSAGE_MAP()
	int OnProcUsbData(BYTE *tempBuf,int bufcount);//usb���̴�����
	int OnProcComData(BYTE *tempBuf,int bufcount);//���ڹ��̴�����
	void RecVolumeInfo(BYTE *tempBuf,int bufcount);
	void RecEquipmentState(BYTE *tempBuf,int bufcount);
	void RecPriorInfo(BYTE *tempBuf,int bufcount);
	void RecVerCtcState(BYTE *tempBuf,int bufcount);
	void RecVerLcuState(BYTE *tempBuf,int bufcount);
	void RecCtcSetRoute(BYTE *tempBuf,int bufcount);
	void RecBroadCastBoxVolumeInfo(BYTE *tempBuf, int bufcount);
public:
	void DelAllMenu(HMENU hMenu);
	bool OpenCTCUsb(void);
	bool OpenATCUsb(void);
	
	BYTE VerifyFun(BYTE * buf, int beginint , int endint);//�±���������±�buf[i]
	int SwitchSendData(BYTE * buf, int count);//���������������buf[count]
	bool OpenComm(int com);
	afx_msg void OnClose();
	CString GetAppPath();
	void OnProcUsbDataATC(BYTE * tempBuf, int count);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//////////////////////////////////////////////////////////////////////////
	vector<XString> m_vLineStationName[3];	//�����ߵ�վ����
	vector<XString> m_vLineTipName;			//��ʾ��	
	vector<XString> m_vLineAdName;			//���
	vector<XString> m_vLineBcName;			//�����㲥
	vector<XString> m_vAddTipsName;			//��Ҫ������ӵ�Tips
	void ReadCfgFromIni();

	afx_msg LRESULT OnAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteMsg(WPARAM wParam, LPARAM lParam);

	BYTE m_bUsbSendBuffer[USB_BUFFER_SIZE];		//USB���ͻ�����
	int m_nSendCnt;						//���͵�USB�ֽ���

	void AddHeadAndTail(BYTE *pData, int nLen, BOOL bNeed7E = TRUE);
	BOOL SendUSBData(LPUSB_DATAPACK pPack);
	BOOL SendHdlcComPack( LPCOMM_DATAPACK lpCOM_DATAPACK );
};



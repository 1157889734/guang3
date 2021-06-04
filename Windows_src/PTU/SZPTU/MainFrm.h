// MainFrm.h : CMainFrame 类的接口
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
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
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
	BYTE m_bRecBufferComm[COMM_BUFFER_SIZE];	//串口接收缓存区
	int m_nRecCntComm;
	BYTE m_bRecBufferUsb[COMM_BUFFER_SIZE];	//usb接收缓存区 ctc
	int m_nRecCntUsb;
	BYTE m_bRecBufferUsbATC[720];	//usb接收缓存区 atc
	int m_nRecCntUsbATC;

	int m_nCurrCTCCmdType; // 当前CTC设置口令代码

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
  
// 生成的消息映射函数
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
	
	afx_msg LRESULT OnRecvComData( WPARAM wParam,LPARAM lParam);//串口接收处理函数
	afx_msg LRESULT OnRecvUsbData( WPARAM wParam,LPARAM lParam);//usb接收处理函数
	afx_msg LRESULT OnUsbCheckError( WPARAM wParam,LPARAM lParam);//usb数据出现校验失败
	DECLARE_MESSAGE_MAP()
	int OnProcUsbData(BYTE *tempBuf,int bufcount);//usb过程处理函数
	int OnProcComData(BYTE *tempBuf,int bufcount);//串口过程处理函数
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
	
	BYTE VerifyFun(BYTE * buf, int beginint , int endint);//下标是数组的下标buf[i]
	int SwitchSendData(BYTE * buf, int count);//数量是数组的数量buf[count]
	bool OpenComm(int com);
	afx_msg void OnClose();
	CString GetAppPath();
	void OnProcUsbDataATC(BYTE * tempBuf, int count);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//////////////////////////////////////////////////////////////////////////
	vector<XString> m_vLineStationName[3];	//三条线的站名称
	vector<XString> m_vLineTipName;			//提示语	
	vector<XString> m_vLineAdName;			//广告
	vector<XString> m_vLineBcName;			//紧急广播
	vector<XString> m_vAddTipsName;			//需要额外添加的Tips
	void ReadCfgFromIni();

	afx_msg LRESULT OnAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteMsg(WPARAM wParam, LPARAM lParam);

	BYTE m_bUsbSendBuffer[USB_BUFFER_SIZE];		//USB发送缓存区
	int m_nSendCnt;						//发送的USB字节数

	void AddHeadAndTail(BYTE *pData, int nLen, BOOL bNeed7E = TRUE);
	BOOL SendUSBData(LPUSB_DATAPACK pPack);
	BOOL SendHdlcComPack( LPCOMM_DATAPACK lpCOM_DATAPACK );
};



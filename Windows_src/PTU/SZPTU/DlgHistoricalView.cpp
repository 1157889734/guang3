// DlgHistoricalView.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "DlgHistoricalView.h"


// CDlgHistoricalView 对话框

IMPLEMENT_DYNAMIC(CDlgHistoricalView, CDialog)

CDlgHistoricalView::CDlgHistoricalView(CWnd* pParent /*=NULL*/)
: CDialog(CDlgHistoricalView::IDD, pParent)
{
	int Saloonstatic[6]={IDC_STATIC_KS_1,IDC_STATIC_KS_2,IDC_STATIC_KS_3,IDC_STATIC_KS_4,IDC_STATIC_KS_5,IDC_STATIC_KS_6};

	int PECUStatic[18]={IDC_STATIC_PECU11,IDC_STATIC_PECU12,IDC_STATIC_PECU13,IDC_STATIC_PECU21,
		IDC_STATIC_PECU22,IDC_STATIC_PECU23,IDC_STATIC_PECU31,IDC_STATIC_PECU32,
		IDC_STATIC_PECU33,IDC_STATIC_PECU41,IDC_STATIC_PECU42,IDC_STATIC_PECU43
		,IDC_STATIC_PECU51,IDC_STATIC_PECU52,IDC_STATIC_PECU53,IDC_STATIC_PECU61,
		IDC_STATIC_PECU62,IDC_STATIC_PECU63};

	int WDSStatic[24]={IDC_STATIC_WBS11,IDC_STATIC_WBS12,IDC_STATIC_WBS13,IDC_STATIC_WBS14,IDC_STATIC_WBS21,
		IDC_STATIC_WBS22,IDC_STATIC_WBS23,IDC_STATIC_WBS24,IDC_STATIC_WBS31,IDC_STATIC_WBS32,IDC_STATIC_WBS33,
		IDC_STATIC_WBS34,IDC_STATIC_WBS41,IDC_STATIC_WBS42,IDC_STATIC_WBS43,IDC_STATIC_WBS44,IDC_STATIC_WBS51,
		IDC_STATIC_WBS52,IDC_STATIC_WBS53,IDC_STATIC_WBS54,IDC_STATIC_WBS61,IDC_STATIC_WBS62,IDC_STATIC_WBS63,
		IDC_STATIC_WBS64};

	int DRMCStatic[6]={IDC_STATIC_DRMC1,IDC_STATIC_DRMC2,IDC_STATIC_DRMC3,IDC_STATIC_DRMC4,IDC_STATIC_DRMC5,IDC_STATIC_DRMC6};

	memcpy(m_Saloonstatic, Saloonstatic, sizeof(Saloonstatic));
	memcpy(m_PECUStatic, PECUStatic, sizeof(PECUStatic));
	memcpy(m_WDSStatic, WDSStatic, sizeof(WDSStatic));
	memcpy(m_DRMCStatic, DRMCStatic, sizeof(DRMCStatic));
	memset(m_SaloonStatus, 0, sizeof(m_SaloonStatus));
	memset(m_PECUStatus, 0, sizeof(m_PECUStatus));
	memset(m_WDSStatus, 0, sizeof(m_WDSStatus));
	memset(m_DRMCStatus, 0, sizeof(m_DRMCStatus));
}

CDlgHistoricalView::~CDlgHistoricalView()
{
}

void CDlgHistoricalView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHistoricalView, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgHistoricalView 消息处理程序


void CDlgHistoricalView::OnTcmsStatusDisplay(BYTE *pData, int nLen)
{
	InitDisplay();

	if (NULL == pData || nLen != 0x99/*0x35*/)
	{
		//ASSERT(FALSE);
		return;
	}

	int nInfoOn = (pData[50]>>3) & 1;
	int nLeftDoorOpen = (pData[45] >> 2) & 1;
	int nRightDoorOpen = (pData[45] >> 1) & 1;
	int nPowerLevel = pData[45] & 1;
	int nStartStation = pData[36];
	int nDestStation = pData[53];
	int nNextStation = pData[53];

	SetDlgItemInt(IDC_STATIC_TCMS_INFO,nInfoOn);
	SetDlgItemInt(IDC_STATIC_TCMS_POWE,nRightDoorOpen);//修改为右门
	SetDlgItemInt(IDC_STATIC_TCMS_DOOR,nLeftDoorOpen);//修改为左门
// 	SetDlgItemInt(IDC_STATIC_TCMS_CURRENTSTATION,nNextStation);
// 	SetDlgItemInt(IDC_STATIC_TCMS_DESTINATION,nDestStation);
// 	SetDlgItemInt(IDC_STATIC_TCMS_STARTSTATION,nStartStation);
	CString szTMSStartStation = _T("");
	CString szTMSDestStation = _T("");
	CString szTMSNextStation = _T("");

	if ((nStartStation >= 101) && (nStartStation <= 130))
	{
		szTMSStartStation = ARRAY_STR_STATION_NAME[nStartStation - 101];
	}

	if ((nNextStation >= 101) && (nNextStation <= 130))
	{
		szTMSNextStation = ARRAY_STR_STATION_NAME[nNextStation - 101];
	}

	int iDestStationIndex = -1;
	

	if (iDestStationIndex != -1)
	{
		szTMSDestStation = ARRAY_STR_STATION_NAME[iDestStationIndex];
	}

	SetDlgItemText(IDC_STATIC_TCMS_STARTSTATION, szTMSStartStation);
	SetDlgItemText(IDC_STATIC_TCMS_DESTINATION, szTMSDestStation);
	SetDlgItemText(IDC_STATIC_TCMS_CURRENTSTATION, szTMSNextStation);
}

void CDlgHistoricalView::OnTcmsStatusDisplay(tagTmsInfo stTmsInfo,CString szDesStation, CString szNextStation)
{
	SetDlgItemInt(IDC_STATIC_TCMS_INFO,stTmsInfo.bAto);
	SetDlgItemInt(IDC_STATIC_TCMS_POWE,stTmsInfo.bright);//修改为右门
	SetDlgItemInt(IDC_STATIC_TCMS_DOOR,stTmsInfo.bleft);//修改为左门
	
	SetDlgItemText(IDC_STATIC_TCMS_DESTINATION, szDesStation);
	SetDlgItemText(IDC_STATIC_TCMS_CURRENTSTATION, szNextStation);
}

void CDlgHistoricalView::OnPiscStatusDisplay(BYTE *pData, int nLen)
{
	//客室
	int nState1[6]={ (pData[11]>>1)&1, (pData[11]>>2)&1, (pData[11]>>3)&1, (pData[11]>>4)&1, (pData[11]>>5)&1, (pData[11]>>6)&1 };

	//机室
	int nState2[8]={ (pData[12]>>1)&1, (pData[12]>>2)&1, (pData[12]>>3)&1, (pData[12]>>4)&1,
		(pData[12]>>5)&1, (pData[12]>>6)&1, (pData[12]>>7)&1 , (pData[12]>>8)&1 };
	
	//IO （atc 2 io6）
	int nState3[8]={ (pData[13]>>1)&1, (pData[13]>>2)&1, (pData[13]>>3)&1, (pData[13]>>4)&1, 
		(pData[13]>>5)&1, (pData[13]>>6)&1, (pData[13]>>7)&1 , (pData[13]>>8)&1 };

	//客室
	int Saloonstatic1[6]={IDC_STATIC_KS_1,IDC_STATIC_DRMC1,IDC_STATIC_KS_2,IDC_STATIC_DRMC2,IDC_STATIC_KS_3,IDC_STATIC_DRMC3};

	int Saloonstatic2[6]={IDC_STATIC_WBS11,IDC_STATIC_WBS12,IDC_STATIC_WBS21,IDC_STATIC_WBS22,IDC_STATIC_WBS31,IDC_STATIC_WBS32};

	//IO
	int Saloonstatic3[8]={IDC_STATIC_WBS23, IDC_STATIC_WBS24, IDC_STATIC_WBS33, IDC_STATIC_WBS34, IDC_STATIC_PECU11, IDC_STATIC_PECU12, IDC_STATIC_PECU21,IDC_STATIC_PECU22};

	for(int i=0;i<8;i++)
	{
		if(i<6)
		{
			SetDlgItemInt(Saloonstatic1[i], nState1[i] + 1);
			((CStatic *)GetDlgItem(Saloonstatic1[i]))->Invalidate();

			SetDlgItemInt(Saloonstatic2[i], nState2[i] + 1);
			((CStatic *)GetDlgItem(Saloonstatic2[i]))->Invalidate();
		}

		SetDlgItemInt(Saloonstatic3[i], nState3[i] + 1);
		((CStatic *)GetDlgItem(Saloonstatic3[i]))->Invalidate();
	}

/*	InitDisplay();

	int SaloonStatus[6]={(pData[21]>>1)&1,(pData[21]>>2)&1,(pData[21]>>3)&1,(pData[21]>>4)&1,(pData[21]>>5)&1,(pData[21]>>6)&1};
	int Saloonstatic[6]={IDC_STATIC_KS_1,IDC_STATIC_KS_2,IDC_STATIC_KS_3,IDC_STATIC_KS_4,IDC_STATIC_KS_5,IDC_STATIC_KS_6};

	int PECUStatus[18]={(pData[23]>>3)&1,(pData[23]>>4)&1,(pData[23]>>5)&1,
		(pData[23]>>7)&1,(pData[24]>>0)&1,(pData[24]>>1)&1,
		(pData[24]>>3)&1,(pData[24]>>4)&1,(pData[24]>>5)&1,
		(pData[25]>>1)&1,(pData[25]>>2)&1,(pData[25]>>3)&1,
		(pData[25]>>5)&1,(pData[25]>>6)&1,(pData[25]>>7)&1,
		(pData[26]>>3)&1,(pData[26]>>4)&1,(pData[26]>>7)&1};
	int PECUStatic[18]={IDC_STATIC_PECU11,IDC_STATIC_PECU12,IDC_STATIC_PECU13,
		IDC_STATIC_PECU21,IDC_STATIC_PECU22,IDC_STATIC_PECU23,
		IDC_STATIC_PECU31,IDC_STATIC_PECU32,IDC_STATIC_PECU33,
		IDC_STATIC_PECU41,IDC_STATIC_PECU42,IDC_STATIC_PECU43,
		IDC_STATIC_PECU51,IDC_STATIC_PECU52,IDC_STATIC_PECU53,
		IDC_STATIC_PECU61,IDC_STATIC_PECU62,IDC_STATIC_PECU63};

// 	int WDSStatus[24]={pData[90]&1,(pData[90]>>1)&1,(pData[90]>>2)&1,(pData[90]>>3)&1,
// 		pData[91]&1,(pData[91]>>1)&1,(pData[91]>>2)&1,(pData[91]>>3)&1,
// 		pData[92]&1,(pData[92]>>1)&1,(pData[92]>>2)&1,(pData[92]>>3)&1,
// 		pData[93]&1,(pData[93]>>1)&1,(pData[93]>>2)&1,(pData[93]>>3)&1,
// 		pData[94]&1,(pData[94]>>1)&1,(pData[94]>>2)&1,(pData[94]>>3)&1,
// 		pData[95]&1,(pData[95]>>1)&1,(pData[95]>>2)&1,(pData[95]>>3)&1};
// 	int WDSStatic[24]={IDC_STATIC_WBS11,IDC_STATIC_WBS12,IDC_STATIC_WBS13,IDC_STATIC_WBS14,
// 		IDC_STATIC_WBS21,IDC_STATIC_WBS22,IDC_STATIC_WBS23,IDC_STATIC_WBS24,
// 		IDC_STATIC_WBS31,IDC_STATIC_WBS32,IDC_STATIC_WBS33,IDC_STATIC_WBS34,
// 		IDC_STATIC_WBS41,IDC_STATIC_WBS42,IDC_STATIC_WBS43,IDC_STATIC_WBS44,
// 		IDC_STATIC_WBS51,IDC_STATIC_WBS52,IDC_STATIC_WBS53,IDC_STATIC_WBS54,
// 		IDC_STATIC_WBS61,IDC_STATIC_WBS62,IDC_STATIC_WBS63,IDC_STATIC_WBS64};
// 
// 	int DRMCstatus[6]={(pData[90]>>7)&1,(pData[91]>>7)&1,(pData[92]>>7)&1,(pData[93]>>7)&1,(pData[94]>>7)&1,(pData[95]>>7)&1};
// 	int DRMCstatic[6]={IDC_STATIC_DRMC1,IDC_STATIC_DRMC2,IDC_STATIC_DRMC3,IDC_STATIC_DRMC4,IDC_STATIC_DRMC5,IDC_STATIC_DRMC6};

	for(int i=0;i<6;i++)
	{
		if(SaloonStatus[i] == 1)
		{
			PECUStatus[3 * i] = PECUStatus[3 * i + 1] = PECUStatus[3 * i + 2] = 1;
// 			WDSStatus[4 * i] = WDSStatus[4 * i + 1] = WDSStatus[4 * i + 2] = WDSStatus[4 * i + 3] = 1;
// 			DRMCstatus[i] = 1;
		}

	}

	for(int i=0;i<6;i++)
	{
		SetDlgItemInt(Saloonstatic[i], SaloonStatus[i] + 1);
		((CStatic *)GetDlgItem(Saloonstatic[i]))->Invalidate();
	}
	for(int i=0;i<18;i++)
	{
		SetDlgItemInt(PECUStatic[i], PECUStatus[i] + 1);
		((CStatic *)GetDlgItem(PECUStatic[i]))->Invalidate();
	}
// 	for(int i=0;i<24;i++)
// 	{
// 		SetDlgItemInt(WDSStatic[i], WDSStatus[i] + 1);
// 		((CStatic *)GetDlgItem(WDSStatic[i]))->Invalidate();
// 	}
// 	for(int i=0;i<6;i++)
// 	{
// 		SetDlgItemInt(DRMCstatic[i],DRMCstatus[i] + 1);
// 		((CStatic *)GetDlgItem(DRMCstatic[i]))->Invalidate();
// 	}*/
}

void CDlgHistoricalView::InitDisplay(void )
{
	SetDlgItemInt(IDC_STATIC_TCMS_INFO, 0);
	SetDlgItemInt(IDC_STATIC_TCMS_POWE, 0);//修改为右门
	SetDlgItemInt(IDC_STATIC_TCMS_DOOR, 0);//修改为左门
	SetDlgItemInt(IDC_STATIC_TCMS_CURRENTSTATION, 0);
	SetDlgItemInt(IDC_STATIC_TCMS_DESTINATION, 0);
	SetDlgItemInt(IDC_STATIC_TCMS_STARTSTATION, 0);

	for(int i=0;i<6;i++)
	{
		SetDlgItemInt(m_Saloonstatic[i], 0);
		((CStatic *)GetDlgItem(m_Saloonstatic[i]))->Invalidate();
	}
	for(int i=0;i<18;i++)
	{
		SetDlgItemInt(m_PECUStatic[i], 0);
		((CStatic *)GetDlgItem(m_PECUStatic[i]))->Invalidate();
	}
	for(int i=0;i<24;i++)
	{
		SetDlgItemInt(m_WDSStatic[i], 0);
		((CStatic *)GetDlgItem(m_WDSStatic[i]))->Invalidate();
	}
	for(int i=0;i<6;i++)
	{
		SetDlgItemInt(m_DRMCStatic[i], 0);
		((CStatic *)GetDlgItem(m_DRMCStatic[i]))->Invalidate();
	}
}

HBRUSH CDlgHistoricalView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();
	int nGroupVal = 0;
	int nInGroupIndex = GetIDIndex(nID, nGroupVal);

	if (-1 != nInGroupIndex)
	{
		int nVal = GetDlgItemInt(nID);

		if(nVal == 2)
		{
			pDC->SetBkColor(RGB(255,0,0));
		}
		else if(nVal == 1)
		{
			pDC->SetBkColor(RGB(0,255,0));
		}
		else
		{
			pDC->SetBkColor(RGB(150,150,150));
		}
	}

	return hbr;
}

int CDlgHistoricalView::GetIDIndex(int staticid, int & groupint)
{
	for(int i=0;i<6;i++)
	{
		if(staticid == m_Saloonstatic[i])
		{
			groupint=0;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<18;i++)
	{
		if(staticid == m_PECUStatic[i])
		{
			groupint=1;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<24;i++)
	{
		if(staticid == m_WDSStatic[i])
		{
			groupint=2;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<6;i++)
	{
		if(staticid == m_DRMCStatic[i])
		{
			groupint=3;
			return i;
		}
	}
	return -1;
}

BOOL CDlgHistoricalView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitDisplay();

	SetDlgItemText(IDC_STATIC_INFO, _T("dva 	数字报站器\r\n apu 	音频处理器 \r\n dcp 	广播控制盒\r\n\
record	录音模块	\r\n\
atc	TMS模块 \r\n \
mvb	MVB模块 \r\n\
crck	司机室广播机柜\r\n\
srck	客室广播机柜") );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgHistoricalView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

// BreakQueryTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "BreakQueryTimeDlg.h"
#include "MainFrm.h"

// CBreakQueryTimeDlg 对话框

IMPLEMENT_DYNAMIC(CBreakQueryTimeDlg, CDialog)

CBreakQueryTimeDlg::CBreakQueryTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBreakQueryTimeDlg::IDD, pParent)
{
	m_tmscount=m_piscount=0;
	memset(blockstatus,0,3*4);
	newpisflag=oldpisflag=0;
	int ksstatic[6]={IDC_STATIC_KS_1,IDC_STATIC_KS_2,IDC_STATIC_KS_3,IDC_STATIC_KS_4,IDC_STATIC_KS_5,IDC_STATIC_KS_6};
	int bjqstatic[18]={IDC_STATIC_PECU11,IDC_STATIC_PECU12,IDC_STATIC_PECU13,IDC_STATIC_PECU21,IDC_STATIC_PECU22,IDC_STATIC_PECU23,IDC_STATIC_PECU31,IDC_STATIC_PECU32,IDC_STATIC_PECU33,IDC_STATIC_PECU41,IDC_STATIC_PECU42,IDC_STATIC_PECU43,IDC_STATIC_PECU51,IDC_STATIC_PECU52,IDC_STATIC_PECU53,IDC_STATIC_PECU61,IDC_STATIC_PECU62,IDC_STATIC_PECU63};
	int WBSstatic[24]={IDC_STATIC_WBS11,IDC_STATIC_WBS12,IDC_STATIC_WBS13,IDC_STATIC_WBS14,IDC_STATIC_WBS21,IDC_STATIC_WBS22,IDC_STATIC_WBS23,IDC_STATIC_WBS24,IDC_STATIC_WBS31,IDC_STATIC_WBS32,IDC_STATIC_WBS33,IDC_STATIC_WBS34,IDC_STATIC_WBS41,IDC_STATIC_WBS42,IDC_STATIC_WBS43,IDC_STATIC_WBS44,IDC_STATIC_WBS51,IDC_STATIC_WBS52,IDC_STATIC_WBS53,IDC_STATIC_WBS54,IDC_STATIC_WBS61,IDC_STATIC_WBS62,IDC_STATIC_WBS63,IDC_STATIC_WBS64};
	int DRMCstatic[6]={IDC_STATIC_DRMC1,IDC_STATIC_DRMC2,IDC_STATIC_DRMC3,IDC_STATIC_DRMC4,IDC_STATIC_DRMC5,IDC_STATIC_DRMC6};
    memcpy(m_ksstatic,ksstatic,6*4);
	memcpy(m_bjqstatic,bjqstatic,18*4);
	memcpy(m_WBSstatic,WBSstatic,24*4);
	memcpy(m_DRMCstatic,DRMCstatic,6*4);
	memset(m_ksstatus,0,6*4);
	memset(m_bjqstatus,0,18*4);
	memset(m_WBSstatus,0,24*4);
	memset(m_DRMCstatus,0,24);

}

CBreakQueryTimeDlg::~CBreakQueryTimeDlg()
{
}

void CBreakQueryTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBreakQueryTimeDlg, CDialog)
	ON_BN_CLICKED(IDC_QUERY, &CBreakQueryTimeDlg::OnBnClickedQuery)
	ON_MESSAGE(WM_DEVICE_STATUS,&CBreakQueryTimeDlg::OnRecvBlockStatus)
	ON_MESSAGE(WM_PIS_INFO,&CBreakQueryTimeDlg::OnRecvPISStatus)
	ON_MESSAGE(WM_485_INFO,&CBreakQueryTimeDlg::OnRecv485Status)
	ON_MESSAGE(WM_TMS_INFO,&CBreakQueryTimeDlg::OnRecvTMSStatus)
	ON_BN_CLICKED(IDC_QUERYTMS, &CBreakQueryTimeDlg::OnBnClickedQuerytms)
	ON_BN_CLICKED(IDC_QUERYPIS, &CBreakQueryTimeDlg::OnBnClickedQuerypis)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CBreakQueryTimeDlg 消息处理程序
void CBreakQueryTimeDlg::ChangColor(int nID/*, int iState*/)
{
	//static int nCount = 0;
	//m_uFaultState[nID] = iState;
	((CStatic *)GetDlgItem(/*IDC_STATIC0 + */nID))->Invalidate();

}
void CBreakQueryTimeDlg::OnBnClickedQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}

    //发送查询设备的实时通信状态
	CString str;
	GetDlgItemText(IDC_QUERY,str);
	if(str.Compare(_T("查询"))==0)
	{
		QueryDeviceStatus(0x01);
		
	}
   
	
	
}
void CBreakQueryTimeDlg::QueryDeviceStatus(int flag)
{
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x02;
	buf[2]=0x08;
	buf[3]=flag;
	buf[4]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,3);
	buf[5]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,6);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);

}

LRESULT CBreakQueryTimeDlg::OnRecvBlockStatus( WPARAM wParam,LPARAM lParam)
{
	BYTE *buf=(BYTE *)wParam;
	/////////////////////////////////////////////进行显示
	blockstatus[0]=buf[0];
    blockstatus[1]=buf[1];
	blockstatus[2]=buf[2];
	
	SetDlgItemInt(IDC_STATIC_ATC_TCMS,buf[0]);
	SetDlgItemInt(IDC_STATIC_ATC_PISC,buf[1]);
	SetDlgItemInt(IDC_STATIC_ATC_485,buf[2]);
	///////////////////////////////////////////////////////
	ChangColor(IDC_STATIC_ATC_TCMS);
	ChangColor(IDC_STATIC_ATC_PISC);
	ChangColor(IDC_STATIC_ATC_485);
    //QueryDeviceStatus(0x02);
	//QueryDeviceStatus(0x03);
    //Invalidate(TRUE);
	
	return 1;

}
LRESULT CBreakQueryTimeDlg::OnRecvPISStatus( WPARAM wParam,LPARAM lParam)
{

	////////////////////////////////////////
	if(m_piscount==0xff)
	{
		m_piscount=0;
	}
	m_piscount++;
	SetDlgItemInt(IDC_STATIC_PISCOUNT,m_piscount);
	//////////////////////////////////////////
	BYTE *buf=(BYTE *)wParam;
	////////////////////////////////////
	
	CFile file;
	file.Open(_T("atc.txt"),CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
	file.SeekToEnd();

	file.Write(buf,78);
	///////////////////////////////////////
	int ksstatus[6]={(buf[25]>>1)&1,(buf[25]>>2)&1,(buf[25]>>3)&1,(buf[25]>>4)&1,(buf[25]>>5)&1,(buf[25]>>7)&1};
	int ksstatic[6]={IDC_STATIC_KS_1,IDC_STATIC_KS_2,IDC_STATIC_KS_3,IDC_STATIC_KS_4,IDC_STATIC_KS_5,IDC_STATIC_KS_6};
	memcpy(m_ksstatus,ksstatus,6*4);
	//客室6故障
	////////////////////////////////////////
	//前面3为表示报警器故障1到6。
	int bjqstatus[18]={(buf[27]>>3)&1,(buf[27]>>4)&1,(buf[27]>>5)&1,
		(buf[27]>>7)&1,(buf[28])&1,(buf[28]>>1)&1,
		(buf[28]>>3)&1,(buf[28]>>4)&1,(buf[28]>>5)&1,
		(buf[29]>>1)&1,(buf[29]>>2)&1,(buf[29]>>3)&1,
		(buf[29]>>5)&1,(buf[29]>>6)&1,(buf[29]>>7)&1,
		(buf[30]>>3)&1,(buf[30]>>4)&1,(buf[30]>>5)&1};
	int bjqstatic[18]={IDC_STATIC_PECU11,IDC_STATIC_PECU12,IDC_STATIC_PECU13,IDC_STATIC_PECU21,IDC_STATIC_PECU22,IDC_STATIC_PECU23,IDC_STATIC_PECU31,IDC_STATIC_PECU32,IDC_STATIC_PECU33,IDC_STATIC_PECU41,IDC_STATIC_PECU42,IDC_STATIC_PECU43,IDC_STATIC_PECU51,IDC_STATIC_PECU52,IDC_STATIC_PECU53,IDC_STATIC_PECU61,IDC_STATIC_PECU62,IDC_STATIC_PECU63};
	
// 	int WBSstatus[24]={buf[90]&1,(buf[90]>>1)&1,(buf[90]>>2)&1,(buf[90]>>3)&1,buf[91]&1,(buf[91]>>1)&1,(buf[91]>>2)&1,(buf[91]>>3)&1,buf[92]&1,(buf[92]>>1)&1,(buf[92]>>2)&1,(buf[92]>>3)&1,buf[93]&1,(buf[93]>>1)&1,(buf[93]>>2)&1,(buf[93]>>3)&1,buf[94]&1,(buf[94]>>1)&1,(buf[94]>>2)&1,(buf[94]>>3)&1,buf[95]&1,(buf[95]>>1)&1,(buf[95]>>2)&1,(buf[95]>>3)&1};
// 	int WBSstatic[24]={IDC_STATIC_WBS11,IDC_STATIC_WBS12,IDC_STATIC_WBS13,IDC_STATIC_WBS14,IDC_STATIC_WBS21,IDC_STATIC_WBS22,IDC_STATIC_WBS23,IDC_STATIC_WBS24,IDC_STATIC_WBS31,IDC_STATIC_WBS32,IDC_STATIC_WBS33,IDC_STATIC_WBS34,IDC_STATIC_WBS41,IDC_STATIC_WBS42,IDC_STATIC_WBS43,IDC_STATIC_WBS44,IDC_STATIC_WBS51,IDC_STATIC_WBS52,IDC_STATIC_WBS53,IDC_STATIC_WBS54,IDC_STATIC_WBS61,IDC_STATIC_WBS62,IDC_STATIC_WBS63,IDC_STATIC_WBS64};
// 	
// 	//////////////////////////////
// 	int DRMCstatus[6]={(buf[90]>>7)&1,(buf[91]>>7)&1,(buf[92]>>7)&1,(buf[93]>>7)&1,(buf[94]>>7)&1,(buf[95]>>7)&1};
// 	int DRMCstatic[6]={IDC_STATIC_DRMC1,IDC_STATIC_DRMC2,IDC_STATIC_DRMC3,IDC_STATIC_DRMC4,IDC_STATIC_DRMC5,IDC_STATIC_DRMC6};
	
	//前面4位表示文字屏，第8位表示动态地图
	///////////////////////////////////////////////////////////////
	for(int i=0;i<6;i++)
	{
		if(ksstatus[i]==1)
		{
			bjqstatus[3*i]=bjqstatus[3*i+1]=bjqstatus[3*i+2]=1;
// 			WBSstatus[4*i]=WBSstatus[4*i+1]=WBSstatus[4*i+2]=WBSstatus[4*i+3]=1;
// 			DRMCstatus[i]=1;

		}

	}
    memcpy(m_bjqstatus,bjqstatus,18*4);
// 	memcpy(m_WBSstatus,WBSstatus,24*4);
// 	memcpy(m_DRMCstatus,DRMCstatus,6*4);
	///////////////////////////////////////////////////////////////////////
	
	for(int i=0;i<6;i++)
	{
		SetDlgItemInt(ksstatic[i],ksstatus[i]);
		ChangColor(ksstatic[i]);
	}
	for(int i=0;i<18;i++)
	{
		SetDlgItemInt(bjqstatic[i],bjqstatus[i]);
		ChangColor(bjqstatic[i]);

	}
// 	for(int i=0;i<24;i++)
// 	{
// 		SetDlgItemInt(WBSstatic[i],WBSstatus[i]);
// 		ChangColor(WBSstatic[i]);
// 
// 	}
// 	for(int i=0;i<6;i++)
// 	{
// 		SetDlgItemInt(DRMCstatic[i],DRMCstatus[i]);
// 		ChangColor(DRMCstatic[i]);
// 
// 	}

	//Invalidate(TRUE);
	return 1;


}
LRESULT CBreakQueryTimeDlg::OnRecv485Status( WPARAM wParam,LPARAM lParam)
{
	return 1;

}
LRESULT CBreakQueryTimeDlg::OnRecvTMSStatus( WPARAM wParam,LPARAM lParam)
{
	if(m_tmscount==0xff)
	{
		m_tmscount=0;
	}
	m_tmscount++;
	SetDlgItemInt(IDC_STATIC_TCMSCOUNT,m_tmscount);
	///////////////////////////////////////////////////
	BYTE *buf=(BYTE *)wParam;
	int dinfoonint = (buf[49]>>3)&1;
	int doorleftint=(buf[49]>>1)&1;
	int doorrightint=(buf[49]>>2)&1;
	int powerint=(buf[11]>>5)&1;
	int startstationint=buf[40];
	int destinationsstationint=buf[42];
	int currentstationint=buf[44];
	int hmistartstation = buf[58];
	int hmideststation = buf[60];
	
	SetDlgItemInt(IDC_STATIC_TCMS_INFO,dinfoonint);
	SetDlgItemInt(IDC_STATIC_TCMS_POWE,doorrightint);//修改为右门
	SetDlgItemInt(IDC_STATIC_TCMS_DOOR,doorleftint);//修改为左门
// 	SetDlgItemInt(IDC_STATIC_TCMS_CURRENTSTATION,currentstationint);
// 	SetDlgItemInt(IDC_STATIC_TCMS_DESTINATION,destinationsstationint);
// 	SetDlgItemInt(IDC_STATIC_TCMS_STARTSTATION,startstationint);
// 	SetDlgItemInt(IDC_STATIC_HMI_STARTSTATION, hmistartstation);
// 	SetDlgItemInt(IDC_STATIC_HMI_DESTSTATION, hmideststation);

	CString szTMSStartStation = _T("");
	CString szTMSDestStation = _T("");
	CString szTMSNextStation = _T("");

	CString szHMIStartStation = _T("");
	CString szHMIDestStation = _T("");


	if ((startstationint >= 101) && (startstationint <= 130))
	{
		szTMSStartStation = ARRAY_STR_STATION_NAME[startstationint - 101];
	}

	if ((currentstationint >= 101) && (currentstationint <= 130))
	{
		szTMSNextStation = ARRAY_STR_STATION_NAME[currentstationint - 101];
	}

	if ((hmistartstation >= 1) && (hmistartstation <= 30))
	{
		szHMIStartStation = ARRAY_STR_STATION_NAME[hmistartstation - 1];
	}

	if ((hmideststation >= 1) && (hmideststation <= 30))
	{
		szHMIDestStation = ARRAY_STR_STATION_NAME[hmideststation - 1];
	}

	int iDestStationIndex = -1;
	switch (destinationsstationint)
	{
	case 11://罗湖
	case 12:
	case 13:
	case 16:
	case 17:
	case 18:
		{
			iDestStationIndex = 0;
			break;
		}
	case 21://大剧院
	case 26:
		{
			iDestStationIndex = 3;
			break;
		}
	case 22://岗厦
	case 27:
		{
			iDestStationIndex = 6;
			break;
		}
	case 24://会展中心
	case 29:
		{
			iDestStationIndex = 7;
			break;
		}
	case 10://竹子林
	case 15:
	case 20:
	case 23:
	case 25:
		{
			iDestStationIndex = 11;
			break;
		}
	case 31://世界之窗
	case 36:
		{
			iDestStationIndex = 14;
			break;
		}
	case 44://深大
	case 49:
		{
			iDestStationIndex = 17;
			break;
		}
	case 91://桃园
	case 96:
		{
			iDestStationIndex = 18;
			break;
		}
	case 42://前海湾
	case 43:
	case 47:
	case 48:
		{
			iDestStationIndex = 21;
			break;
		}
	case 81://新安
	case 82:
	case 86:
	case 87:
		{
			iDestStationIndex = 22;
			break;
		}
	case 45://宝体
	case 50:
		{
			iDestStationIndex = 24;
			break;
		}
	case 51:
	case 52://西乡
	case 56:
	case 57:
	case 58:
		{
			iDestStationIndex = 26;
			break;
		}
	case 54://固戍
	case 59:
		{
			iDestStationIndex = 27;
			break;
		}
	case 71://后瑞
	case 76:
		{
			iDestStationIndex = 28;
			break;
		}
	case 61://机场东
	case 62:
	case 66:
	case 67:
		{
			iDestStationIndex = 29;
			break;
		}
	default:
		{
			iDestStationIndex = -1;
			break;
		}
	}

	if (iDestStationIndex != -1)
	{
		szTMSDestStation = ARRAY_STR_STATION_NAME[iDestStationIndex];
	}

	SetDlgItemText(IDC_STATIC_HMI_STARTSTATION, szHMIStartStation);
	SetDlgItemText(IDC_STATIC_HMI_DESTSTATION, szHMIDestStation);
	SetDlgItemText(IDC_STATIC_TCMS_STARTSTATION, szTMSStartStation);
	SetDlgItemText(IDC_STATIC_TCMS_DESTINATION, szTMSDestStation);
	SetDlgItemText(IDC_STATIC_TCMS_CURRENTSTATION, szTMSNextStation);

	return 1;
}
void CBreakQueryTimeDlg::OnBnClickedQuerytms()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	CString str;
	GetDlgItemText(IDC_QUERYTMS,str);
	if(str.Compare(_T("查询"))==0)
	{
		QueryDeviceStatus(0x02);
		SetDlgItemText(IDC_QUERYTMS,_T("取消查询"));
		
	}
	else
	{
		//QueryDeviceStatus(0x13);
		QueryDeviceStatus(0x12);
		SetDlgItemText(IDC_QUERYTMS,_T("查询"));
		
	}
}

void CBreakQueryTimeDlg::OnBnClickedQuerypis()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	CString str;
	GetDlgItemText(IDC_QUERYPIS,str);
	if(str.Compare(_T("查询"))==0)
	{
		QueryDeviceStatus(0x03);
		SetDlgItemText(IDC_QUERYPIS,_T("取消查询"));
	}
	else
	{
		//QueryDeviceStatus(0x13);
		QueryDeviceStatus(0x13);
		SetDlgItemText(IDC_QUERYPIS,_T("查询"));
	}
}

HBRUSH CBreakQueryTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		int nID = pWnd->GetDlgCtrlID();
		if(nID==IDC_STATIC_ATC_TCMS)
		{
			if(blockstatus[0]==0)
				pDC->SetBkColor(RGB(255,0,0));
			else
				pDC->SetBkColor(RGB(0,255,0));
			return hbr;
		}
		if(nID==IDC_STATIC_ATC_PISC)
		{
			if(blockstatus[1]==0)
				pDC->SetBkColor(RGB(255,0,0));
			else
				pDC->SetBkColor(RGB(0,255,0));
			return hbr;
		}
		if(nID==IDC_STATIC_ATC_485)
		{
			if(blockstatus[2]==0)
				pDC->SetBkColor(RGB(255,0,0));
			else
				pDC->SetBkColor(RGB(0,255,0));
			return hbr;
		}
		int groupint;
		int index=GetIDIndex(nID,groupint);
		int values=0;
		if(index==-1)
		{
			if(nID==IDC_STATIC_EXAMNORMAL)
				pDC->SetBkColor(RGB(0,255,0));
			if(nID==IDC_STATIC_EXAMERROR)
                pDC->SetBkColor(RGB(255,0,0));
			return hbr;

		}
		if(groupint==0)
		{
			values=m_ksstatus[index];
		}
		if(groupint==1)
		{
			values=m_bjqstatus[index];
		}
		if(groupint==2)
		{
			values=m_WBSstatus[index];
		}
		if(groupint==3)
		{
			values=m_DRMCstatus[index];
		}
		if(values==1)
		{
			pDC->SetBkColor(RGB(255,0,0));
		}
		else if(values==0)
		{
			pDC->SetBkColor(RGB(0,255,0));
		}
		else
		{
			pDC->SetBkColor(RGB(0,0,255));
		}
	}
	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
int CBreakQueryTimeDlg::GetIDIndex(int staticid, int & groupint)
{
	for(int i=0;i<6;i++)
	{
		if(staticid==m_ksstatic[i])
		{
			groupint=0;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<18;i++)
	{
		if(staticid==m_bjqstatic[i])
		{
			groupint=1;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<24;i++)
	{
		if(staticid==m_WBSstatic[i])
		{
			groupint=2;
			return i;
		}
	}
	///////////////////////////////
	for(int i=0;i<6;i++)
	{
		if(staticid==m_DRMCstatic[i])
		{
			groupint=3;
			return i;
		}
	}
	return -1;
}
BOOL CBreakQueryTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreakQueryTimeDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    KillTimer(WM_HEART_DATA);
	CString str;
	GetDlgItemText(IDC_QUERYPIS,str);
	if(str.Compare(_T("取消查询"))==0)
	{
		if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
		{
			QueryDeviceStatus(0x13);

		}
		
		SetDlgItemText(IDC_QUERYPIS,_T("查询"));
	}
	//////////////////////////////////
	GetDlgItemText(IDC_QUERYTMS,str);
	if(str.Compare(_T("取消查询"))==0)
	{
		if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
		{
			QueryDeviceStatus(0x12);

		}
		
		SetDlgItemText(IDC_QUERYTMS,_T("查询"));

	}

	CDialog::OnClose();
}

void CBreakQueryTimeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
    SetTimer(WM_HEART_DATA,1000,NULL);
	// TODO: 在此处添加消息处理程序代码
}

void CBreakQueryTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if(nIDEvent==WM_HEART_DATA)
	{
		if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
		{
			SendHeartData();

		}
	}
	CDialog::OnTimer(nIDEvent);
}
void CBreakQueryTimeDlg::SendHeartData(void)
{
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0xff;
	buf[3]=0x55;
	buf[4]=0x7e;

	pSendSet->pPtr=buf;
	pSendSet->nLen=5;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

BOOL CBreakQueryTimeDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

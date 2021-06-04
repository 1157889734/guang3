// ParaSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "ParaSetDlg.h"
#include "MainFrm.h"
#define VOICE_MAX_DATA 100
// CParaSetDlg 对话框

IMPLEMENT_DYNAMIC(CParaSetDlg, CDialog)

CParaSetDlg::CParaSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParaSetDlg::IDD, pParent)
	, m_emgcountint(1)
{
	for (int i = 0; i < 6; i++)
	{
		m_arrBroadCastBoxVol[i] = i;
	}
}

CParaSetDlg::~CParaSetDlg()
{
}

void CParaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_GLOBAL, m_Gobal_Volume);
	DDX_Control(pDX, IDC_SLIDER_OCC, m_Occ_Volume);
	DDX_Control(pDX, IDC_SLIDER_DVA, m_Dva_Volume);
	DDX_Control(pDX, IDC_SLIDER_MEDIA, m_Media_Volume);
	DDX_Control(pDX, IDC_SLIDER_DOOR, m_Door_Volume);
	DDX_Control(pDX, IDC_SLIDER_BASS, m_Apu_Bass);
	DDX_Control(pDX, IDC_SLIDER_TREBLE, m_Apu_Treble);
	DDX_Text(pDX, IDC_EDIT_EMCCOUNT, m_emgcountint);
	//DDV_MinMaxUInt(pDX, m_emgcountint, 1, 255);
	DDX_Control(pDX, IDC_COMBO_BROADCASTBOX_TYPE, m_comboxBroadCastBoxVolType);
	DDX_Control(pDX, IDC_STATIC_BCBOXVOL, m_staticBCBoxVol);
	DDX_Control(pDX, IDC_SLIDER_BCBOXVOLUME, m_sliderCtlBCBoxVol);
}


BEGIN_MESSAGE_MAP(CParaSetDlg, CDialog)
	ON_BN_CLICKED(IDC_WBCCOUNT, &CParaSetDlg::OnBnClickedWbccount)
	ON_BN_CLICKED(IDC_PRISET, &CParaSetDlg::OnBnClickedPriset)
	ON_BN_CLICKED(IDC_PRIRECE, &CParaSetDlg::OnBnClickedPrirece)
	ON_BN_CLICKED(IDC_SETVOICE, &CParaSetDlg::OnBnClickedSetvoice)
	ON_BN_CLICKED(IDC_GETVOICE, &CParaSetDlg::OnBnClickedGetvoice)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DVA, &CParaSetDlg::OnNMCustomdrawSliderDvaVolume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MEDIA, &CParaSetDlg::OnNMCustomdrawSliderMediaValume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DOOR, &CParaSetDlg::OnNMCustomdrawSliderDoorVolume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BASS, &CParaSetDlg::OnNMCustomdrawSliderApuBass)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TREBLE, &CParaSetDlg::OnNMCustomdrawSliderApuTreble)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GLOBAL, &CParaSetDlg::OnNMCustomdrawSliderApuGlobal)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_OCC, &CParaSetDlg::OnNMCustomdrawSliderOcc)
	ON_BN_CLICKED(IDC_INI, &CParaSetDlg::OnBnClickedIni)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SETBCBOXVOL, &CParaSetDlg::OnBnClickedButtonSetbcboxvol)
	ON_BN_CLICKED(IDC_BUTTON_GETBCBOXVOL, &CParaSetDlg::OnBnClickedButtonGetbcboxvol)
	ON_CBN_SELCHANGE(IDC_COMBO_BROADCASTBOX_TYPE, &CParaSetDlg::OnCbnSelchangeComboBroadcastboxType)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BCBOXVOLUME, &CParaSetDlg::OnNMCustomdrawSliderBcboxvolume)
END_MESSAGE_MAP()


// CParaSetDlg 消息处理程序

void CParaSetDlg::OnBnClickedWbccount()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strValue;
	GetDlgItemText(IDC_EDIT_EMCCOUNT,strValue);
	/////////////////////////////////////////////////

	//检测非法字符
	int nCount = strValue.GetLength(); // 获得字符个数
	for ( int i = 0; i < nCount; i ++ )
	{
		if ( 0 == isdigit( strValue.GetAt(i) ) ) // 不是数字就置标志位
		{
			MessageBox(_T("必须输入1到255之间的数字!"),_T("Prompt"),MB_ICONWARNING);
			return;// 退出
		}
	}
	if ( _wtoi(strValue)> 255 || _wtoi(strValue) < 1 )
	{
		MessageBox(_T("必须输入1到255之间的数字!"),_T("Prompt"),MB_ICONWARNING);
		return;// 退出
	}
	///////////////////////////////////////////////////////////////////
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	BYTE sendbuf[100]={0x7e,0x0b,0x00,0x01};
	int count=GetDlgItemInt(IDC_EDIT_EMCCOUNT);
	sendbuf[4]=count;
	sendbuf[5]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,4);
	sendbuf[6]=0x7e;
    count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,7);
	pSendSet->pPtr=sendbuf;
	pSendSet->nLen=count;
    ((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);

}

void CParaSetDlg::OnBnClickedPriset()
{
	// TODO: 在此添加控件通知处理程序代码
	int buf[8];
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
    USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count=0;
	BYTE sendbuf[100]={0x7e,0x0c,0x00,0x08};
	sendbuf[4]=((CComboBox *)GetDlgItem(IDC_WLBROCOMB))->GetCurSel()+1;
	sendbuf[5]=((CComboBox *)GetDlgItem(IDC_TALKBOTH))->GetCurSel()+1;
	sendbuf[6]=sendbuf[5];
	sendbuf[7]=((CComboBox *)GetDlgItem(IDC_HANDBROCOMB))->GetCurSel()+1;
	sendbuf[8]=((CComboBox *)GetDlgItem(IDC_CLOSEDOORTIPCOMB))->GetCurSel()+1;
	sendbuf[9]=((CComboBox *)GetDlgItem(IDC_DATABROCOMB))->GetCurSel()+1;
	sendbuf[10]=((CComboBox *)GetDlgItem(IDC_MEDIOPLAYCOMB))->GetCurSel()+1;
	sendbuf[11]=((CComboBox *)GetDlgItem(IDC_EMGBROCOMB))->GetCurSel()+1;
	sendbuf[12] = ((CComboBox *)GetDlgItem(IDC_TESTAUDIOCOMB))->GetCurSel() + 1;
    sendbuf[13]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,12);
	sendbuf[14]=0x7e;
	///////////////////////////////////////////
	buf[0]=sendbuf[4];
	buf[1]=sendbuf[5];
	buf[2]=sendbuf[7];
	buf[3]=sendbuf[8];
	buf[4]=sendbuf[9];
	buf[5]=sendbuf[10];
	buf[6]=sendbuf[11];
	buf[7] = sendbuf[12];
//    int sameint= HaveSamePro(buf);
//    if(sameint==1)
//    {
// 	   MessageBox(_T("不能有相同的优先级"),_T("Prompt"),MB_ICONWARNING);
// 	   return;
// 
//    }
	//////////////////////////////////////////
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,15);
	pSendSet->pPtr=sendbuf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

BOOL CParaSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int nidbuf[8]={IDC_WLBROCOMB,IDC_HANDBROCOMB,IDC_CLOSEDOORTIPCOMB,IDC_TALKBOTH,IDC_DATABROCOMB,IDC_MEDIOPLAYCOMB,IDC_EMGBROCOMB, IDC_TESTAUDIOCOMB};
	IniComb(nidbuf,8);
    InitVolumeInfo();
	///////////////////////////////////////
	
	//////////////////////////////////////////////
	// TODO:  在此添加额外的初始化
    
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CParaSetDlg::InitVolumeInfo()
{
	m_Gobal_Volume.SetRange(0,VOICE_MAX_DATA);
	m_Gobal_Volume.SetTicFreq(1);
	m_Gobal_Volume.SetPos(0);
	m_Occ_Volume.SetRange(0,VOICE_MAX_DATA);
	m_Occ_Volume.SetTicFreq(1);
	m_Occ_Volume.SetPos(0);
	m_Dva_Volume.SetRange(0,VOICE_MAX_DATA);
	m_Dva_Volume.SetTicFreq(1);
	m_Dva_Volume.SetPos(0);
	m_Media_Volume.SetRange(0,VOICE_MAX_DATA);
	m_Media_Volume.SetTicFreq(1);
	m_Media_Volume.SetPos(0);
	m_Door_Volume.SetRange(0,VOICE_MAX_DATA);
	m_Door_Volume.SetTicFreq(1);
	m_Door_Volume.SetPos(0);
	//m_Apu_Bass.SetRange(0,10);
	m_Apu_Bass.SetRange(0,VOICE_MAX_DATA);
	m_Apu_Bass.SetTicFreq(1);
	m_Apu_Bass.SetPos(0);
	//m_Apu_Treble.SetRange(0,8);
	m_Apu_Treble.SetRange(0,VOICE_MAX_DATA);
	m_Apu_Treble.SetTicFreq(1);
	m_Apu_Treble.SetPos(0);

	int idx = 0;
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("广播监听音量"));
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("司机对讲监听音量"));
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("乘客对讲监听音量"));
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("广播麦克音量"));
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("司机对讲麦克音量"));
	m_comboxBroadCastBoxVolType.InsertString(idx++, _T("乘客对讲麦克音量"));
	m_comboxBroadCastBoxVolType.SetCurSel(0);
	m_sliderCtlBCBoxVol.SetRange(0, 15);
	m_sliderCtlBCBoxVol.SetPos(0);
}

void CParaSetDlg::IniComb(int * nid, int count)
{
	for(int i=0;i<count;i++)
	{
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("1"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("2"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("3"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("4"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("5"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("6"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("7"));
		((CComboBox *)GetDlgItem(nid[i]))->AddString(_T("8"));
	}
	///////////////////////////////////////////////////////////////
	((CComboBox *)GetDlgItem(nid[0]))->SetCurSel(6);
    ((CComboBox *)GetDlgItem(nid[1]))->SetCurSel(3);
	((CComboBox *)GetDlgItem(nid[2]))->SetCurSel(5);
	((CComboBox *)GetDlgItem(nid[3]))->SetCurSel(4);
	((CComboBox *)GetDlgItem(nid[4]))->SetCurSel(2);
	((CComboBox *)GetDlgItem(nid[5]))->SetCurSel(0);
	((CComboBox *)GetDlgItem(nid[6]))->SetCurSel(1);
	((CComboBox *)GetDlgItem(nid[7]))->SetCurSel(6);
}

void CParaSetDlg::OnBnClickedPrirece()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count=0;
	BYTE sendbuf[100]={0x7e,0x22,0x00,0x00};
	sendbuf[4]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,3);
	sendbuf[5]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,6);
	pSendSet->pPtr=sendbuf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

void CParaSetDlg::Refleshpri(BYTE * buf, int bufcount)
{
	int i = buf[7];
	int j = buf[8];
	((CComboBox *)GetDlgItem(IDC_WLBROCOMB))->SetCurSel(buf[4]-1);
	//((CComboBox *)GetDlgItem(IDC_TALKBOTH))->SetCurSel(buf[7]-1);
	((CComboBox *)GetDlgItem(IDC_HANDBROCOMB))->SetCurSel(buf[7]-1);
	((CComboBox *)GetDlgItem(IDC_CLOSEDOORTIPCOMB))->SetCurSel(buf[8]-1);
	((CComboBox *)GetDlgItem(IDC_DATABROCOMB))->SetCurSel(buf[9]-1);
	((CComboBox *)GetDlgItem(IDC_MEDIOPLAYCOMB))->SetCurSel(buf[10]-1);
	((CComboBox *)GetDlgItem(IDC_EMGBROCOMB))->SetCurSel(buf[11]-1);
	((CComboBox *)GetDlgItem(IDC_TESTAUDIOCOMB))->SetCurSel(buf[12] - 1);
	MessageBox(_T("获取优先级成功"));
}

void CParaSetDlg::OnBnClickedSetvoice()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	VolumeInfo m_VolumeInfo;
	ZeroMemory(&m_VolumeInfo, sizeof(VolumeInfo));
	GetVolumeValue(m_VolumeInfo);
	/////////////////////////////////////////////////////////////////////////////
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count=0;
	BYTE sendbuf[100]={0x7e,0x07,0x00,0x07};
	sendbuf[4]=m_VolumeInfo.iGlobal;////////////////////////////////////////////////有待修改。
	sendbuf[5]=m_VolumeInfo.iOcc;
	sendbuf[6]=m_VolumeInfo.iDva;
	sendbuf[7]=m_VolumeInfo.iMedia;
	sendbuf[8]=m_VolumeInfo.iDoor;
	sendbuf[9]=m_VolumeInfo.fApuBass;
	sendbuf[10]=m_VolumeInfo.fApuTreble;
	sendbuf[11]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,10);
	sendbuf[12]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,13);
	pSendSet->pPtr=sendbuf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);

}
void CParaSetDlg::GetVolumeValue(VolumeInfo &m_VolumeInfo)
{
	CString strValue;
	GetDlgItemText(IDC_STATIC_GLOBAL,strValue);
	char szANSIString [MAX_PATH]; 
	//WCHAR szANSIString [MAX_PATH];
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.iGlobal = atoi(szANSIString);
	//m_VolumeInfo.iGlobal = _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_OCC,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.iOcc =  atoi(szANSIString);
	//m_VolumeInfo.iOcc =  _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_DVA,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.iDva = atoi(szANSIString);
	//m_VolumeInfo.iDva = _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_MEDIA,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.iMedia = atoi(szANSIString);
	//m_VolumeInfo.iMedia = _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_DOOR,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.iDoor = atoi(szANSIString);
	//m_VolumeInfo.iDoor = _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_BASS,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	m_VolumeInfo.fApuBass = atoi(szANSIString);
	//m_VolumeInfo.fApuBass = _wtoi(szANSIString);
	GetDlgItemText(IDC_STATIC_TREBLE,strValue);
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	//m_VolumeInfo.fApuTreble = _wtoi(szANSIString);
	m_VolumeInfo.fApuTreble = atoi(szANSIString);
}

void CParaSetDlg::OnBnClickedGetvoice()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	BYTE Send[6];
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	memset(Send, 0, 6);
	memset(pSendSet, 0, sizeof(USB_DATAPACK));

	Send[0] = 0x7E;    //起始符
	Send[1] = 0x15;    //校对时间回读命令
	Send[2] = 0x00;    //数据长度1字节
	Send[3] = 0x00;    //数据长度2字节
	Send[4] = 0x55 - 0x15;
	Send[5] = 0x7E;   //结束符

	pSendSet->nLen = 6;
	pSendSet->pPtr = Send;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

void CParaSetDlg::OnNMCustomdrawSliderApuGlobal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int iValue = m_Gobal_Volume.GetPos();
	
	iValue = VOICE_MAX_DATA-iValue;
	
	CString strValue;
	strValue.Format(_T("%d"),iValue);
	((CStatic *)GetDlgItem(IDC_STATIC_GLOBAL))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderOcc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int iValue = m_Occ_Volume.GetPos();
	iValue = VOICE_MAX_DATA-iValue;
	CString strValue;
	strValue.Format(_T("%d"),iValue);
	((CStatic *)GetDlgItem(IDC_STATIC_OCC))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderDvaVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int iValue = m_Dva_Volume.GetPos();
	iValue = VOICE_MAX_DATA-iValue;
	CString strValue;
	strValue.Format(_T("%d"),iValue);
	((CStatic *)GetDlgItem(IDC_STATIC_DVA))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderMediaValume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int iValue = m_Media_Volume.GetPos();
	iValue = VOICE_MAX_DATA-iValue;
	CString strValue;
	strValue.Format(_T("%d"),iValue);
	((CStatic *)GetDlgItem(IDC_STATIC_MEDIA))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderDoorVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int iValue = m_Door_Volume.GetPos();
	iValue = VOICE_MAX_DATA-iValue;
	CString strValue;
	strValue.Format(_T("%d"),iValue);
	((CStatic *)GetDlgItem(IDC_STATIC_DOOR))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderApuBass(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int fValue = m_Apu_Bass.GetPos();
	fValue = VOICE_MAX_DATA-fValue;
	CString strValue;
	//strValue.Format(_T("%.1f"),fValue*1.5);
	strValue.Format(_T("%d"),fValue);
	((CStatic *)GetDlgItem(IDC_STATIC_BASS))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}

void CParaSetDlg::OnNMCustomdrawSliderApuTreble(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	int fValue = m_Apu_Treble.GetPos();
	fValue = VOICE_MAX_DATA-fValue;
	CString strValue;
	//strValue.Format(_T("%.1f"),fValue*1.5);
	strValue.Format(_T("%d"),fValue);
	((CStatic *)GetDlgItem(IDC_STATIC_TREBLE))->SetWindowText(strValue);
	UpdateData(FALSE);
	*pResult = 0;
}
void CParaSetDlg::RecVolumeInfo(BYTE * buf, int count)
{

	m_Gobal_Volume.SetPos(VOICE_MAX_DATA-buf[4]);

	m_Occ_Volume.SetPos(VOICE_MAX_DATA-buf[5]);

	m_Dva_Volume.SetPos(VOICE_MAX_DATA-buf[6]);

	m_Media_Volume.SetPos(VOICE_MAX_DATA-buf[7]);

	m_Door_Volume.SetPos(VOICE_MAX_DATA-buf[8]);

	m_Apu_Bass.SetPos(VOICE_MAX_DATA-buf[9]);

	m_Apu_Treble.SetPos(VOICE_MAX_DATA-buf[10]);
////////////////////////////////////////////////////////
	SetDlgItemInt(IDC_STATIC_GLOBAL,buf[4]);
	SetDlgItemInt(IDC_STATIC_OCC,buf[5]);
	SetDlgItemInt(IDC_STATIC_DVA,buf[6]);
	SetDlgItemInt(IDC_STATIC_MEDIA,buf[7]);
	SetDlgItemInt(IDC_STATIC_DOOR,buf[8]);
	SetDlgItemInt(IDC_STATIC_BASS,buf[9]);
	SetDlgItemInt(IDC_STATIC_TREBLE,buf[10]);
	MessageBox(_T("获取成功"));
}

void CParaSetDlg::RecBroadCastBoxVolumeInfo(BYTE *buf, int iCount)
{
#define PROTOCOL_DATALEN 9
	if (iCount < PROTOCOL_DATALEN)
	{
		return ;
	}

	for (int i = 0; i < 6; i++)
	{
		m_arrBroadCastBoxVol[i] = buf[4 + i];
	}

	int iCurBCBoxType = 0;
	iCurBCBoxType = m_comboxBroadCastBoxVolType.GetCurSel();
	if (iCurBCBoxType >= 0)
	{
		m_sliderCtlBCBoxVol.SetPos(m_arrBroadCastBoxVol[iCurBCBoxType]);
	}
}

void CParaSetDlg::OnBnClickedIni()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	BYTE Send[6];
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	memset(Send, 0, 6);
	memset(pSendSet, 0, sizeof(USB_DATAPACK));

	Send[0] = 0x7E;    //起始符
	Send[1] = 0x20;    //校对时间回读命令
	Send[2] = 0x00;    //数据长度1字节
	Send[3] = 0x00;    //数据长度2字节
	Send[4] = 0x55 - 0x20;
	Send[5] = 0x7E;   //结束符

	pSendSet->nLen = 6;
	pSendSet->pPtr = Send;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

int CParaSetDlg::HaveSamePro(int * buf)
{
	for(int i=0;i<8;i++)
	{
		for(int j=i+1;j<8;j++)
		{
			if(buf[i]==buf[j])
			{
				return 1;
			}
		}
	}
	return 0;
}

BOOL CParaSetDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CParaSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	
	if(nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID()!=IDC_SLIDER_GLOBAL
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_OCC
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_DVA
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_MEDIA
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_DOOR
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_BASS
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_TREBLE
		&& pWnd->GetDlgCtrlID()!=IDC_SLIDER_BCBOXVOLUME
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_GLOBAL
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_OCC
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_DVA
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_MEDIA
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_DOOR
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_BASS
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_TREBLE
		&& pWnd->GetDlgCtrlID()!=IDC_STATIC_BCBOXVOL)
	{
		//pDC->SelectObject(&m_font);
		pDC->SetTextColor(RGB(0,100,130)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
		//return m_SliderBrush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CParaSetDlg::OnBnClickedButtonSetbcboxvol()
{
	// TODO: Add your control notification handler code here
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}

	/////////////////////////////////////////////////////////////////////////////
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count=0;
	BYTE sendbuf[100]={0x7e,0x33,0x00,0x06};
	sendbuf[4]=m_arrBroadCastBoxVol[0];
	sendbuf[5]=m_arrBroadCastBoxVol[1];
	sendbuf[6]=m_arrBroadCastBoxVol[2];
	sendbuf[7]=m_arrBroadCastBoxVol[3];
	sendbuf[8]=m_arrBroadCastBoxVol[4];
	sendbuf[9]=m_arrBroadCastBoxVol[5];
	sendbuf[10]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,9);
	sendbuf[11]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,12);
	pSendSet->pPtr=sendbuf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

void CParaSetDlg::OnBnClickedButtonGetbcboxvol()
{
	// TODO: Add your control notification handler code here
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	BYTE Send[6];
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	memset(Send, 0, 6);
	memset(pSendSet, 0, sizeof(USB_DATAPACK));

	Send[0] = 0x7E;    //起始符
	Send[1] = 0x34;    //校对时间回读命令
	Send[2] = 0x00;    //数据长度1字节
	Send[3] = 0x00;    //数据长度2字节
	Send[4] = 0x55 - 0x34;
	Send[5] = 0x7E;   //结束符

	pSendSet->nLen = 6;
	pSendSet->pPtr = Send;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);
}

void CParaSetDlg::OnCbnSelchangeComboBroadcastboxType()
{
	// TODO: Add your control notification handler code here
	int iCurBCType = 0;
	iCurBCType = m_comboxBroadCastBoxVolType.GetCurSel();
	if ((iCurBCType >= 0) && (iCurBCType < 6))
	{
		int iVolValue = 0;
		iVolValue = m_arrBroadCastBoxVol[iCurBCType];

		m_sliderCtlBCBoxVol.SetPos(iVolValue);

		CString szVolValue = _T("");
		szVolValue.Format(_T("%d"), iVolValue);
		m_staticBCBoxVol.SetWindowText(szVolValue);
	}
}


void CParaSetDlg::OnNMCustomdrawSliderBcboxvolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int iCurBCType = 0;
	iCurBCType = m_comboxBroadCastBoxVolType.GetCurSel();
	if ((iCurBCType >= 0) && (iCurBCType < 6))
	{
		int iCurVolValue = 0;
		iCurVolValue = m_sliderCtlBCBoxVol.GetPos();

		m_arrBroadCastBoxVol[iCurBCType] = iCurVolValue;

		CString szVolValue = _T("");
		szVolValue.Format(_T("%d"), iCurVolValue);
		m_staticBCBoxVol.SetWindowText(szVolValue);
	}
	*pResult = 0;
}

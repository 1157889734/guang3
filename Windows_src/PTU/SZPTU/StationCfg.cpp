// StationCfg.cpp : implementation file
//

#include "stdafx.h"
#include "SZPTU.h"
#include "StationCfg.h"
#include "MainFrm.h"

//#define BASIC_TIPS_COUNT 11

// CStationCfg dialog

IMPLEMENT_DYNAMIC(CStationCfg, CDialog)

CStationCfg::CStationCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CStationCfg::IDD, pParent)
{
	m_uDispLan = 3;
	m_nPercet = 0;
	m_nselRow = 0;
	m_bOption = FALSE;
	m_font2.CreatePointFont(120,_T("Tahoma"));
}

CStationCfg::~CStationCfg()
{
	WriteFileStationInfo();
}

void CStationCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST4, m_listCtrl);
	DDX_Control(pDX, IDC_LIST_PRE_TIPS_SELECTED, m_listPreTipsSelected);
	DDX_Control(pDX, IDC_LIST_PRE_ADS_SELECTED, m_listPreAdsSelected);
	DDX_Control(pDX, IDC_LIST_ARRIVAL_ADS_SELECTED, m_listArrivalAdsSelected);
	DDX_Control(pDX, IDC_LIST_ARRIVAL_TIPS_SELECTED, m_listArrivalTipsSelected);
	DDX_Control(pDX, IDC_LIST_TIP, m_listTip);
	DDX_Control(pDX, IDC_LIST_AD, m_listAd);
	DDX_Control(pDX, IDC_BUTTON_SET_SYSLAN, m_btnSysCfg);
	DDX_Control(pDX, IDC_BUTTON_STATION_CFG, m_btnStationCfg);

	DDX_Control(pDX, IDC_BUTTON_PRE_TIPS_ADD, m_btnPreTipAdd);
	DDX_Control(pDX, IDC_BUTTON_PRE_TIPS_DEL, m_btnPreTipDel);
	DDX_Control(pDX, IDC_BUTTON_PRE_ADS_ADD, m_btnPreAdAdd);
	DDX_Control(pDX, IDC_BUTTON_PRE_ADS_DEL, m_btnPreAdDel);
	DDX_Control(pDX, IDC_BUTTON_ARRIVAL_TIPS_ADD, m_btnArrTipAdd);
	DDX_Control(pDX, IDC_BUTTON_ARRIVAL_TIPS_DEL, m_btnArrTipDel);
	DDX_Control(pDX, IDC_BUTTON_ARRIVAL_ADS_ADD, m_btnArrAdAdd);
	DDX_Control(pDX, IDC_BUTTON_ARRIVAL_ADS_DEL, m_btnArrAdDel);
}


BEGIN_MESSAGE_MAP(CStationCfg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_SYSLAN, &CStationCfg::OnBnClickedButtonSetSyslan)
	ON_BN_CLICKED(IDC_CHECK_CFG_ALL, &CStationCfg::OnBnClickedCheckCfgAll)
	ON_CBN_SELCHANGE(IDC_COMBO_CFG_LINE, &CStationCfg::OnCbnSelchangeComboCfgLine)
	ON_CBN_SELCHANGE(IDC_COMBO_CFG_LAN, &CStationCfg::OnCbnSelchangeComboCfgLan)
	ON_BN_CLICKED(IDC_BUTTON_PRE_TIPS_ADD, &CStationCfg::OnBnClickedButtonPreTipsAdd)
	ON_BN_CLICKED(IDC_BUTTON_PRE_ADS_ADD, &CStationCfg::OnBnClickedButtonPreAdsAdd)
	ON_BN_CLICKED(IDC_BUTTON_ARRIVAL_TIPS_ADD, &CStationCfg::OnBnClickedButtonArrivalTipsAdd)
	ON_BN_CLICKED(IDC_BUTTON_ARRIVAL_ADS_ADD, &CStationCfg::OnBnClickedButtonArrivalAdsAdd)
	ON_BN_CLICKED(IDC_BUTTON_STATION_CFG, &CStationCfg::OnBnClickedButtonStationCfg)
	ON_BN_CLICKED(IDC_BUTTON_PRE_TIPS_DEL, &CStationCfg::OnBnClickedButtonPreTipsDel)
	ON_BN_CLICKED(IDC_BUTTON_PRE_ADS_DEL, &CStationCfg::OnBnClickedButtonPreAdsDel)
	ON_BN_CLICKED(IDC_BUTTON_ARRIVAL_TIPS_DEL, &CStationCfg::OnBnClickedButtonArrivalTipsDel)
	ON_BN_CLICKED(IDC_BUTTON_ARRIVAL_ADS_DEL, &CStationCfg::OnBnClickedButtonArrivalAdsDel)
	ON_LBN_DBLCLK(IDC_LIST_TIP, &CStationCfg::OnLbnDblclkListTip)
	ON_LBN_DBLCLK(IDC_LIST_AD, &CStationCfg::OnLbnDblclkListAd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ADD_MSG, &CStationCfg::OnAddMsg)
	ON_MESSAGE(WM_DELETE_MSG, &CStationCfg::OnDeleteMsg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_LIST4, &CStationCfg::OnNMClickList4)
END_MESSAGE_MAP()


// CStationCfg message handlers


void CStationCfg::OnBnClickedButtonSetSyslan()
{
	//int iselMethod = ((CComboBox*)GetDlgItem(IDC_COMBO_METHOD))->GetCurSel();
	int isel = ((CComboBox*)GetDlgItem(IDC_COMBO_SYS_LAN))->GetCurSel();
	int isettype = ((CComboBox*)GetDlgItem(IDC_COMBO_SYS_LEDTYPE))->GetCurSel();

	//检测是否选择
	if (isel == -1 || isettype == -1)
	{
		//AfxMessageBox(_T("Please make a selection!!!"));
		MessageBox(_T("Please make a selection!") , _T("Prompt"), MB_ICONWARNING);
		return;
	}	

	int iselMethod = 0;
	if (!((CMainFrame*)AfxGetMainWnd())->m_comisopen && !((CMainFrame*)AfxGetMainWnd())->m_ptctcusb)
	{
		//AfxMessageBox(_T("Communication Port Not Opened!!!"));
		MessageBox(_T("Communication Port Not Opened!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else if(((CMainFrame*)AfxGetMainWnd())->m_comisopen)
	{
		iselMethod = 0;
	}
	else if (((CMainFrame*)AfxGetMainWnd())->m_ptctcusb)
	{
		iselMethod = 1;
	}
	else
	{
		return;
	}
	
	PIS_PACK_HEAD   pHead;
	memset(&pHead, 0 , sizeof(PIS_PACK_HEAD));

	if (isettype == LED_IDU)
	{
		pHead.bDstDevID = IDU_VALUE;
	}
	else if (isettype == LED_FDU)
	{
		pHead.bDstDevID = FDU_VALUE;
	}
	else
	{
		pHead.bDstDevID = SDU_VALUE;
	}

	pHead.bCMD = 0x04;
	pHead.bDstNetID = 0xFF;
	pHead.bSrcNetID = 0x00;
	pHead.bSrcDevID = 0x01;
	
	pHead.bDataLen = sizeof(COM_SYSCFG_PACK);
	COM_SYSCFG_PACK lanPack;
	lanPack.bType = 3;
	lanPack.bDisplayLan = isel+1;

	LPCOMM_DATAPACK lpComPack = new COMM_DATAPACK;

	lpComPack->nLen = sizeof(PIS_PACK_HEAD) + sizeof(COM_SYSCFG_PACK) +1;
	lpComPack->pPtr = new BYTE[lpComPack->nLen];

	memcpy(lpComPack->pPtr,&pHead,sizeof(pHead));
	memcpy(lpComPack->pPtr+sizeof(pHead),&lanPack,sizeof(lanPack));
	lpComPack->pPtr[sizeof(pHead) + sizeof(lanPack)] = theApp.GetCheckSum(lpComPack->pPtr,sizeof(pHead) + sizeof(lanPack));

	if (iselMethod == COM)
	{
		((CMainFrame*)AfxGetMainWnd())->SendHdlcComPack(lpComPack);//发送数据
	}
	else if (iselMethod == USB)
	{
		//usb数据
		BYTE pUsbDataBuffer[255] = {0};//构建一个缓冲包的数据
		USB_PACK_HEAD* pUsbHead = (USB_PACK_HEAD*)&pUsbDataBuffer[0];

		memcpy(pUsbHead->bData, lpComPack->pPtr, lpComPack->nLen);
		int nUsbTotalCount = sizeof(USB_PACK_HEAD) + lpComPack->nLen;
		//pUsbHead->bCMD = 0x19;
		pUsbHead->bCMD = CMD_TRANSFER;
		pUsbHead->bDataLenH = nUsbTotalCount << 8;
		pUsbHead->bDataLenL = nUsbTotalCount;

		BYTE bUsbCheckSum = theApp.GetCheckSum(pUsbDataBuffer, nUsbTotalCount);
		pUsbDataBuffer[nUsbTotalCount] = bUsbCheckSum;

		//对数据进行转码操作
		((CMainFrame*)AfxGetMainWnd())->AddHeadAndTail(pUsbDataBuffer ,nUsbTotalCount + 1);

		LPUSB_DATAPACK pUsbPack = new USB_DATAPACK;
		pUsbPack->nLen = ((CMainFrame*)AfxGetMainWnd())->m_nSendCnt;
		pUsbPack->pPtr = new BYTE[pUsbPack->nLen];
		memcpy(pUsbPack->pPtr, ((CMainFrame*)AfxGetMainWnd())->m_bUsbSendBuffer, pUsbPack->nLen);
		//发送USB数据
		if (((CMainFrame*)AfxGetMainWnd()) != NULL)
		{
			//为了确保本地控制器能接收到数据，每个数据的第一包需发两次
			((CMainFrame*)AfxGetMainWnd())->SendUSBData(pUsbPack);
			Sleep(200);
			//////////////////////////////////////////////////////////////////////////
			
			((CMainFrame*)AfxGetMainWnd())->SendUSBData(pUsbPack);		
		}

		if (pUsbPack->pPtr != NULL)
		{
			delete[] pUsbPack->pPtr;
			pUsbPack->pPtr = NULL;
		}
		if (pUsbPack != NULL)
		{
			delete pUsbPack;
			pUsbPack = NULL;
		}
	}

	//释放内存
	if (lpComPack->pPtr != NULL)
	{
		delete[] lpComPack->pPtr;
		lpComPack->pPtr = NULL;
	}
	if (lpComPack != NULL)
	{
		delete lpComPack;
		lpComPack = NULL;
	}

	MessageBox(_T("Download Finished!"),_T("Prompt"),MB_OK);
}

void CStationCfg::OnBnClickedCheckCfgAll()
{
#if 0
	int iSelLang = 	((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LAN))->GetCurSel();

	if (iSelLang ==-1)
	{
		return;
	}

	int nMake = m_listCtrl.GetSelectionMark();
	if (nMake==-1)
	{
		return;
	}
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_CFG_ALL))->GetCheck();
	int iCount = m_listCtrl.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_listCtrl.SetCheck(i,bCheck);
		if (i!=nMake)
		{
			OnlistCopyData(&m_listArr[iSelLang][i], &m_listArr[iSelLang][nMake], FALSE);
		}
	}
#endif	

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_CFG_ALL))->GetCheck();
	int iCount = m_listCtrl.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_listCtrl.SetCheck(i,bCheck);
	}
}


BOOL CStationCfg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
	m_listCtrl.SetExtendedStyle(dwStyle);	

	m_listCtrl.InsertColumn(0,_T("Station No."),LVCFMT_CENTER,70);
	m_listCtrl.InsertColumn(1,_T("English."),LVCFMT_LEFT,100);
	m_listCtrl.InsertColumn(2,_T("中文"),LVCFMT_LEFT,100);

	InitCombo();
	InitDlg();
	InitList();
    ///////////////////////////////////
	m_listPreTipsSelected.SetFont(&m_font2);
	m_listArrivalAdsSelected.SetFont(&m_font2);
	m_listArrivalTipsSelected.SetFont(&m_font2);
	m_listPreAdsSelected.SetFont(&m_font2);
	m_listTip.SetFont(&m_font2);
	m_listAd.SetFont(&m_font2);
	/////////////////////////////////////////////////////////////////////
	ReadFileStationInfo();
	/////////////////////////////////////////////////////////////////////
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStationCfg::InitCombo()
{
	((CComboBox *)GetDlgItem(IDC_COMBO_CFG_LINE))->AddString(_T("1"));
	((CComboBox *)GetDlgItem(IDC_COMBO_CFG_LINE))->AddString(_T("2"));
	((CComboBox *)GetDlgItem(IDC_COMBO_CFG_LINE))->AddString(_T("3"));
}

void CStationCfg::InitDlg()
{
	LoadMyImage(&m_btnStationCfg, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnSysCfg, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));

	LoadMyImage(&m_btnPreTipAdd, _T("//ImageSrc//Right_Normal.png"), _T("//ImageSrc//Right_Push.png"), _T("//ImageSrc//Right_Enter.png"), _T(""));
	LoadMyImage(&m_btnPreTipDel, _T("//ImageSrc//Left_Normal.png"), _T("//ImageSrc//Left_Push.png"), _T("//ImageSrc//Left_Enter.png"), _T(""));
	LoadMyImage(&m_btnPreAdAdd, _T("//ImageSrc//Right_Normal.png"), _T("//ImageSrc//Right_Push.png"), _T("//ImageSrc//Right_Enter.png"), _T(""));
	LoadMyImage(&m_btnPreAdDel, _T("//ImageSrc//Left_Normal.png"), _T("//ImageSrc//Left_Push.png"), _T("//ImageSrc//Left_Enter.png"), _T(""));
	LoadMyImage(&m_btnArrTipAdd, _T("//ImageSrc//Right_Normal.png"), _T("//ImageSrc//Right_Push.png"), _T("//ImageSrc//Right_Enter.png"), _T(""));
	LoadMyImage(&m_btnArrTipDel, _T("//ImageSrc//Left_Normal.png"), _T("//ImageSrc//Left_Push.png"), _T("//ImageSrc//Left_Enter.png"), _T(""));
	LoadMyImage(&m_btnArrAdAdd, _T("//ImageSrc//Right_Normal.png"), _T("//ImageSrc//Right_Push.png"), _T("//ImageSrc//Right_Enter.png"), _T(""));
	LoadMyImage(&m_btnArrAdDel, _T("//ImageSrc//Left_Normal.png"), _T("//ImageSrc//Left_Push.png"), _T("//ImageSrc//Left_Enter.png"), _T(""));
}

void CStationCfg::InitList()
{
	//往链表里添加固定数据
 	for (int n=0; n<2; n++)
 	{
 		for (int i=0; i<255; i++)
 		{
 			m_listArr[n][i].clear();
 			m_listArr[n][i].push_back(0xfd);
 			m_listArr[n][i].push_back(0xfe);
 			m_listArr[n][i].push_back(0xfc);
 			m_listArr[n][i].push_back(0xfd);
 			m_listArr[n][i].push_back(0xfe);
 			m_listArr[n][i].push_back(0xff);
 		}
 	}
	
	m_list.push_back(0xfd);
	m_list.push_back(0xfe);
	m_list.push_back(0xfc);
	m_list.push_back(0xfd);
	m_list.push_back(0xfe);
	m_list.push_back(0xff);

	//初始化所有ComboBox控件
	((CComboBox *)GetDlgItem(IDC_COMBO_SYS_LAN))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO_SYS_LEDTYPE))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO_CFG_LAN))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO_CFG_LINE))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO_PRE_ARRIVAL_COLOR))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO_ARRIVAL_COLOR))->SetCurSel(0);

	//初始化
	DoLoadAdsAndTipsList();
	DoLoadList();
}

void CStationCfg::OnCbnSelchangeComboCfgLine()
{
	DoLoadList();
}

void CStationCfg::DoLoadList()
{
	int iSelList = 	((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LINE))->GetCurSel();

	if (CB_ERR == iSelList)
	{
		return;
	}
	m_listCtrl.DeleteAllItems();
	//line
	int nSize = ((CMainFrame*)AfxGetMainWnd())->m_vLineStationName[iSelList].size();
	for (int i=0;i<nSize;i++)
	{
		CString strNo;
		XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineStationName[iSelList].at(i);

		strNo.Format(_T("%d"),i+1);
		m_listCtrl.InsertItem(i,strNo);
		m_listCtrl.SetItemText(i,1,strValue[LAN_EN]);
		m_listCtrl.SetItemText(i,2,strValue[LAN_IRAN]);		
	}
	int iCount = m_listCtrl.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_listCtrl.SetCheck(i,FALSE);
	}
	((CButton*)GetDlgItem(IDC_CHECK_CFG_ALL))->SetCheck(FALSE);

	//////////////////////////////////////////////////////////////////////////
	LayoutList();
}

void CStationCfg::LayoutList()
{
	int iColMinW1 = 70;
	int iColMinW2 = 70;
	int iCount = m_listCtrl.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		int iTemp;
		iTemp = m_listCtrl.GetStringWidth(m_listCtrl.GetItemText(i,1));
		if (iTemp > iColMinW1)
		{
			iColMinW1 = iTemp+20;
			m_listCtrl.SetColumnWidth(1,iColMinW1);
		}
		iTemp = m_listCtrl.GetStringWidth(m_listCtrl.GetItemText(i,2));
		if (iTemp > iColMinW2)
		{
			iColMinW2 = iTemp+20;
			m_listCtrl.SetColumnWidth(2,iColMinW2);
		}
	}
}

void CStationCfg::OnCbnSelchangeComboCfgLan()
{
	//TODO: Add your control notification handler code here
	DoLoadAdsAndTipsList();
}

void CStationCfg::DoLoadAdsAndTipsList()
{
	int iSelList = 	((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LAN))->GetCurSel();

	if (iSelList ==-1)
	{
		return;
	}

	if(0 == iSelList)
	{
		m_bEnglish = TRUE;
	}
	if (1 == iSelList)
	{
		m_bEnglish = FALSE;
	}

	DeleteTipsAndAdsItem(&m_listTip);
	DeleteTipsAndAdsItem(&m_listAd);
	DeleteTipsAndAdsItem(&m_listPreTipsSelected);
	DeleteTipsAndAdsItem(&m_listPreAdsSelected);
	DeleteTipsAndAdsItem(&m_listArrivalTipsSelected);
	DeleteTipsAndAdsItem(&m_listArrivalAdsSelected);

	//删除list中的新添加的数据
	list<BYTE>::iterator iter = m_list.begin();
	while (iter != m_list.end())
	{
		BYTE tmp = *iter;
		if(tmp < 0xfc)
		{
			m_list.erase(iter++);//在这里要使指针前进1个，否则迭代器失效
		}
		else
		{
			iter++;
		}
	}

	//vector<BYTE>::iterator iter = m_vector.begin();
	//while (iter != m_vector.end())
	//{
	//	BYTE tmp = *iter;
	//	if(tmp < 0xfc)
	//	{
	//		iter = m_vector.erase(iter);//在这里要使指针前进1个，否则迭代器失效
	//	}
	//	else
	//	{
	//		iter++;
	//	}
	//}

	int nSizeTip = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.size();
	for (int i=0;i<nSizeTip;i++)
	{
		CString strNo;
		XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(i);

		if (m_bEnglish )
		{
			/*m_listPreTips.InsertString(i, strValue[LAN_EN]);
			m_listArrivalTips.InsertString(i, strValue[LAN_EN]);*/
			m_listTip.InsertString(i, strValue[LAN_EN]);
		}
		else
		{
			//m_listPreTips.InsertString(i, strValue[LAN_IRAN]);
			//m_listArrivalTips.InsertString(i, strValue[LAN_IRAN]);
			m_listTip.InsertString(i, strValue[LAN_IRAN]);
			
		}		
	}

	int nSizeAd = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.size();
	for (int i=0;i<nSizeAd;i++)
	{
		CString strNo;
		XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(i);

		if (m_bEnglish )
		{
			//m_listPreAds.InsertString(i, strValue[LAN_EN]);
			//m_listArrivalAds.InsertString(i, strValue[LAN_EN]);
			m_listAd.InsertString(i, strValue[LAN_EN]);
		}
		else
		{
			//m_listPreAds.InsertString(i, strValue[LAN_IRAN]);
			//m_listArrivalAds.InsertString(i, strValue[LAN_IRAN]);
			m_listAd.InsertString(i, strValue[LAN_IRAN]);
			
		}		
	}
}


void CStationCfg::DeleteTipsAndAdsItem(CListBox* pWnd)
{
	for (int i=pWnd->GetCount();i != 0;i--)
	{
		pWnd->DeleteString(i-1);
	}
}

void CStationCfg::OnBnClickedButtonPreTipsAdd()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listTip.GetCurSel();

	if (CB_ERR == iSelList)
	{
		return;
	}
	XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(iSelList);

	int iCount=m_listPreTipsSelected.GetCount();
	if (iCount >= 4)
	{
		MessageBox(_T("You can add a maxnum of 4!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (m_bEnglish )
	{
		m_listPreTipsSelected.InsertString(iCount,strValue[LAN_EN]);
	}
	else
	{
		m_listPreTipsSelected.InsertString(iCount,strValue[LAN_IRAN]);
	}

	int nTipsCount = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.size();

	list<BYTE>::iterator iter;
	for(iter = m_list.begin(); iter != m_list.end();iter++)
	{
		if(*iter == 0xfe)
		{
			m_list.insert(iter,1,iSelList+nTipsCount+1);
			break;
		}
	}

	m_bOption = TRUE;
	//vector<BYTE>::iterator iter;
	//for (iter = m_vector.begin(); iter != m_vector.end(); iter ++)
	//{
	//	if(*iter == 0xfe)
	//	{
	//		m_vector.insert(iter,1,iSelList+1);
	//		break;
	//	}
	//}
}

void CStationCfg::OnBnClickedButtonPreAdsAdd()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listAd.GetCurSel();

	if (CB_ERR == iSelList)
	{
		return;
	}
	XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(iSelList);

	int iCount=m_listPreAdsSelected.GetCount();
	if (iCount >= 4)
	{
		MessageBox(_T("You can add a maxnum of 4!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (m_bEnglish )
	{
		m_listPreAdsSelected.InsertString(iCount,strValue[LAN_EN]);
	}
	else
	{
		m_listPreAdsSelected.InsertString(iCount,strValue[LAN_IRAN]);
	}

	list<BYTE>::iterator iter;
	for(iter = m_list.begin(); iter != m_list.end();iter++)
	{
		if(*iter == 0xfc)
		{
			m_list.insert(iter,1,iSelList+1);
			break;
		}
	}
	m_bOption = TRUE;
}

void CStationCfg::OnBnClickedButtonArrivalTipsAdd()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listTip.GetCurSel();

	if (iSelList == -1)
	{
		return;
	}
	XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(iSelList);

	int iCount=m_listArrivalTipsSelected.GetCount();
	if (iCount >= 4)
	{
		MessageBox(_T("You can add a maxnum of 4!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (m_bEnglish )
	{
		m_listArrivalTipsSelected.InsertString(iCount,strValue[LAN_EN]);
	}
	else
	{
		m_listArrivalTipsSelected.InsertString(iCount,strValue[LAN_IRAN]);
	}

	int nTipsCount = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.size();

	list<BYTE>::iterator iter;
	int nCount = 0;
	for(iter = m_list.begin(); iter != m_list.end();iter++)
	{
		if(*iter == 0xfe)
		{
			if(nCount != 0)
			{
				m_list.insert(iter,1,iSelList+nTipsCount+1);
				break;
			}
			nCount ++;
		}		
	}

	m_bOption = TRUE;
	//vector<BYTE>::iterator iter;
	//int nCount = 0;
	//for (iter = m_vector.begin(); iter != m_vector.end(); iter ++)
	//{
	//	if(*iter == 0xfe)
	//	{
	//		if (nCount != 0)
	//		{
	//			m_vector.insert(iter,1,iSelList+1);
	//			break;
	//		}
	//		nCount ++;
	//	}
	//}
}

void CStationCfg::OnBnClickedButtonArrivalAdsAdd()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listAd.GetCurSel();

	if (iSelList == -1)
	{
		return;
	}
	XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(iSelList);

	int iCount=m_listArrivalAdsSelected.GetCount();
	if (iCount >= 4)
	{
		MessageBox(_T("You can add a maxnum of 4!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (m_bEnglish )
	{
		m_listArrivalAdsSelected.InsertString(iCount,strValue[LAN_EN]);
	}
	else
	{
		m_listArrivalAdsSelected.InsertString(iCount,strValue[LAN_IRAN]);
	}

	list<BYTE>::iterator iter;
	for(iter = m_list.begin(); iter != m_list.end();iter++)
	{
		if(*iter == 0xff)
		{
			m_list.insert(iter,1,iSelList+1);
			break;
		}
	}

	//vector<BYTE>::iterator iter;
	//for (iter = m_vector.begin(); iter != m_vector.end(); iter ++)
	//{
	//	if(*iter == 0xff)
	//	{
	//		m_vector.insert(iter,1,iSelList+1);
	//		break;
	//	}
	//}
	m_bOption = TRUE;
}

void CStationCfg::OnBnClickedButtonStationCfg()
{
	// TODO: Add your control notification handler code here
	//int iSelMethod = ((CComboBox*)GetDlgItem(IDC_COMBO_METHOD2))->GetCurSel();
	int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LINE))->GetCurSel();
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LAN))->GetCurSel();
	int iSelPreClr = ((CComboBox*)GetDlgItem(IDC_COMBO_PRE_ARRIVAL_COLOR))->GetCurSel();
	int iSelArrivalClr = ((CComboBox*)GetDlgItem(IDC_COMBO_ARRIVAL_COLOR))->GetCurSel();
	int iselMethod = 0;

	if((iSelLan == -1) || (iSelLine == -1) || (iSelPreClr == -1) || (iSelArrivalClr == -1))
	{
		MessageBox(_T("Please make a selection!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	if (!((CMainFrame*)AfxGetMainWnd())->m_comisopen && !((CMainFrame*)AfxGetMainWnd())->m_ptctcusb)
	{
		//AfxMessageBox(_T("Communication Port Not Opened!!!"));
		MessageBox(_T("Communication Port Not Opened!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else if(((CMainFrame*)AfxGetMainWnd())->m_comisopen)
	{
		iselMethod = 0;
	}
	else if (((CMainFrame*)AfxGetMainWnd())->m_ptctcusb)
	{
		iselMethod = 1;
	}
	else
	{
		return;
	}

	int  bCheckGroup[MAX_PATH] = {-1};		//记录已经选中行
	int  iSelCout = 0;		//需要配置的行数
	int  iCout = m_listCtrl.GetItemCount();		//条目总数
	for (int i=0;i<iCout;i++)
	{
		if (m_listCtrl.GetCheck(i))
		{
			bCheckGroup[iSelCout] = i;		//选中，记录选中的列
			iSelCout++;
		}
	}

	if (0 == iSelCout)
	{
		MessageBox(_T("Please Select One Item!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	//为了确保本地控制器能接收到数据，站配置数据的第一包需发两次
	DownLoadStaticPic(iselMethod, bCheckGroup[0], iSelLine,  /*iSelLan*/0, iSelPreClr, iSelArrivalClr);
	Sleep(200);

	//////////////////////////////////////////////////////////////////////////

	for (int nLan=0; nLan<2; nLan++)
	{
		for (int i = 0; i < iSelCout; i++)
		{
			DownLoadStaticPic(iselMethod, bCheckGroup[i], iSelLine,  nLan/*iSelLan*/, iSelPreClr, iSelArrivalClr);
			m_nPercet = (i + 1) *100/iSelCout;
			((CProgressCtrl *)GetDlgItem(IDC_PROGRESS_CONFIG))->SetRange(0,100);
			((CProgressCtrl *)GetDlgItem(IDC_PROGRESS_CONFIG))->SetPos(m_nPercet);
			Sleep(100);
		}
	}
	
	
	int iRet = MessageBox(_T("Download Finished!"),_T("Prompt"),MB_OK);
	if (IDOK == iRet)
	{
		m_nPercet = 0;
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS_CONFIG))->SetRange(0,100);
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS_CONFIG))->SetPos(m_nPercet);
	}
}

void CStationCfg::DownLoadStaticPic(UINT nMethod 
									, UINT nStation 
									, UINT nLine /*= 0*/
									, UINT nLan /*= 0*/
									, UINT nPreClr /*= 0*/
									, UINT nArrivalClr /*= 0*/ )
{
	UINT nSize = m_listArr[nLan][nStation].size();
	BYTE * pData =new BYTE[nSize];
	memset(pData, 0, nSize);

	int i = 0;
	list <BYTE>::iterator iter = m_listArr[nLan][nStation].begin();
	while (iter != m_listArr[nLan][nStation].end())
	{
		pData[i++] = *iter++;
	}

	//UINT nSize = m_vector.size();
	//BYTE * pData =new BYTE[nSize];
	//memset(pData, 0, nSize);

	//int i = 0;
	//vector<BYTE>::iterator iter = m_vector.begin();
	//while (iter != m_vector.end())
	//{
	//	pData[i++] = *iter++;
	//}

	BYTE pDataBuffer[100] = {0};//构建一个缓冲包的数据
	PIS_PACK_HEAD   *pHead = (PIS_PACK_HEAD *)&pDataBuffer[0];
	STATION_CFG_PACK *stationCfgPack = (STATION_CFG_PACK *)&pDataBuffer[sizeof(PIS_PACK_HEAD)];
	memcpy(stationCfgPack->bData,pData, nSize);

	//pHead->bCMD = 0x04;
	pHead->bCMD = CMD_UPDATE;
	pHead->bDstNetID = 0xFF;
	pHead->bSrcNetID = 0x00;
	pHead->bSrcDevID = 0x01;
	pHead->bDstDevID = IDU_VALUE;		//站配置协议只针对内部屏
	pHead->bDataLen = nSize + sizeof(PIS_PACK_HEAD);

	stationCfgPack->bType = 4;
	stationCfgPack->bLineNo = nLine + 1;
	stationCfgPack->bStationNo = nStation + 1;
	stationCfgPack->bDisplayLan = nLan;
	stationCfgPack->bPreColor = nPreClr + 1;
	stationCfgPack->bArrivalColor = nArrivalClr + 1;

	int nTotalCount = sizeof(PIS_PACK_HEAD) + sizeof(STATION_CFG_PACK) + nSize;
	pDataBuffer[nTotalCount] = theApp.GetCheckSum(pDataBuffer, nTotalCount);
	LPCOMM_DATAPACK lpComPack = new COMM_DATAPACK;
	lpComPack->nLen = nTotalCount + 1;
	lpComPack->pPtr = new BYTE[lpComPack->nLen];
	memcpy(lpComPack->pPtr, pDataBuffer, lpComPack->nLen);
	
	if (COM == nMethod)		//通过串口发送数据
	{
		if (((CMainFrame*)AfxGetMainWnd()) != NULL)
		{
			((CMainFrame*)AfxGetMainWnd())->SendHdlcComPack(lpComPack);//发送数据
		}
	}
	else if (USB == nMethod)		//通过USB发送数据
	{
		BYTE pUsbDataBuffer[100] = {0};//构建一个缓冲包的数据
		USB_PACK_HEAD* pUsbHead = (USB_PACK_HEAD*)&pUsbDataBuffer[0];

		memcpy(pUsbHead->bData, lpComPack->pPtr, lpComPack->nLen);
		int nUsbTotalCount = sizeof(USB_PACK_HEAD) + lpComPack->nLen;
		pUsbHead->bCMD = CMD_TRANSFER;
		pUsbHead->bDataLenH = nUsbTotalCount << 8;
		pUsbHead->bDataLenL = nUsbTotalCount;

		BYTE bUsbCheckSum = theApp.GetCheckSum(pUsbDataBuffer, nUsbTotalCount);
		pUsbDataBuffer[nUsbTotalCount] = bUsbCheckSum;

		((CMainFrame*)AfxGetMainWnd())->AddHeadAndTail(pUsbDataBuffer ,nUsbTotalCount + 1);

		LPUSB_DATAPACK pUsbPack = new USB_DATAPACK;
		pUsbPack->nLen = ((CMainFrame*)AfxGetMainWnd())->m_nSendCnt;
		pUsbPack->pPtr = new BYTE[pUsbPack->nLen];
		memcpy(pUsbPack->pPtr, ((CMainFrame*)AfxGetMainWnd())->m_bUsbSendBuffer, pUsbPack->nLen);
		//发送USB数据
		if (((CMainFrame*)AfxGetMainWnd()) != NULL)
		{
			((CMainFrame*)AfxGetMainWnd())->SendUSBData(pUsbPack);
		}
		
		if (pUsbPack->pPtr != NULL)
		{
			delete[] pUsbPack->pPtr;
			pUsbPack->pPtr = NULL;
		}
		if (pUsbPack != NULL)
		{
			delete pUsbPack;
			pUsbPack = NULL;
		}
	}
	
	//释放内存
	if (pData != NULL)
	{
		delete[] pData;
		pData = NULL;
	}
	if (lpComPack->pPtr != NULL)
	{
		delete[] lpComPack->pPtr;
		lpComPack->pPtr = NULL;
	}
	if (lpComPack != NULL)
	{
		delete lpComPack;
		lpComPack = NULL;
	}
}

void CStationCfg::OnBnClickedButtonPreTipsDel()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listPreTipsSelected.GetCurSel();

	if (CB_ERR == iSelList)
	{
		return;
	}

	int iCount=m_listPreTipsSelected.GetCount();
	if (iCount < 0)
	{
		return;
	}

	list<BYTE>::iterator iter = m_list.begin();
	int n = 0;
	BOOL bFlag = FALSE;
	while (iter != m_list.end())
	{
		BYTE temp = *iter;
		if (temp == 0xfd)
		{
			bFlag = TRUE;		
		}
		if (bFlag)
		{
			if (n == iSelList + 1)
			{
				iter = m_list.erase( iter);
				break;
			}
			n++;
		}	
		iter ++;
	}

	//vector<BYTE>::iterator iter;
	//for(iter = m_vector.begin(); iter != m_vector.end();iter++)
	//{
	//	if (*iter == 0xfd)
	//	{
	//		iter = m_vector.erase( iter + iSelList + 1);
	//		break;
	//	}
	//	iter ++;
	//}

	m_listPreTipsSelected.DeleteString(iSelList);
}

void CStationCfg::OnBnClickedButtonPreAdsDel()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listPreAdsSelected.GetCurSel();

	if (iSelList == -1)
	{
		return;
	}

	int iCount=m_listPreAdsSelected.GetCount();
	if (iCount < 0)
	{
		return;
	}

	list<BYTE>::iterator iter = m_list.begin();
	int n = 0;
	BOOL bFlag = FALSE;
	while (iter != m_list.end())
	{
		BYTE temp = *iter;
		if (temp == 0xfe)
		{
			bFlag = TRUE;
		}
		if (bFlag)
		{
			if (n == iSelList + 1)
			{
				iter = m_list.erase( iter);
				break;
			}
			n++;
		}	
		iter ++;
	}

	//vector<BYTE>::iterator iter;
	//for(iter = m_vector.begin(); iter != m_vector.end();iter++)
	//{
	//	if (*iter == 0xfe)
	//	{
	//		iter = m_vector.erase( iter + iSelList + 1);
	//		break;
	//	}
	//	iter ++;
	//}

	m_listPreAdsSelected.DeleteString(iSelList);
	
}

void CStationCfg::OnBnClickedButtonArrivalTipsDel()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listArrivalTipsSelected.GetCurSel();

	if (iSelList == -1)
	{
		return;
	}

	int iCount=m_listArrivalTipsSelected.GetCount();
	if (iCount < 0)
	{
		return;
	}

	list<BYTE>::iterator iter = m_list.begin();
	int iFlag = 0;
	int n = 0;
	while (iter != m_list.end())
	{
		BYTE temp = *iter;
		if (temp == 0xfd)
		{
			iFlag ++;
		}
		if (iFlag == 2)
		{
			if (n == iSelList + 1)
			{
				iter = m_list.erase( iter);
				break;
			}
			n++;
		}
		iter ++;
	}

	m_bOption = TRUE;
	m_listArrivalTipsSelected.DeleteString(iSelList);
}

void CStationCfg::OnBnClickedButtonArrivalAdsDel()
{
	// TODO: Add your control notification handler code here
	int iSelList = m_listArrivalAdsSelected.GetCurSel();

	if (iSelList == -1)
	{
		return;
	}

	int iCount=m_listArrivalAdsSelected.GetCount();
	if (iCount < 0)
	{
		return;
	}

	list<BYTE>::iterator iter = m_list.begin();
	int iFlag = 0;
	int n = 0;
	while (iter != m_list.end())
	{
		BYTE temp = *iter;
		if (temp == 0xfe)
		{
			iFlag++;
		}
		if (iFlag == 2)
		{
			if (n == iSelList + 1)
			{
				iter = m_list.erase( iter);
				break;
			}
			n++;
		}		
		iter ++;
	}

	m_bOption = TRUE;
	m_listArrivalAdsSelected.DeleteString(iSelList);
}


BOOL CStationCfg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE||pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CStationCfg::OnLbnDblclkListTip()
{
	// TODO: Add your control notification handler code here
	//int iSel = m_listTip.GetCurSel();

	//CRect rc;
	//m_listTip.GetItemRect(iSel,&rc);
	//CPoint pt;
	//pt.x = rc.left + rc.Width()/2;
	//pt.y = rc.top + rc.Height()/2;

	//CMenu menu;
	//menu.LoadMenu(IDR_MENU_TIP);
	//CMenu *pMenu = menu.GetSubMenu(0);
	//if(pMenu!=NULL)
	//{
	//	m_listTip.ClientToScreen(&pt);
	//	if (iSel < 0)
	//	{
	//		pMenu->EnableMenuItem(ID_TIP_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//		pMenu->EnableMenuItem(ID_TIP_MODIFY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//	}
	//	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);   
	//}

}

void CStationCfg::OnLbnDblclkListAd()
{
	// TODO: Add your control notification handler code here
	//int iSel = m_listAd.GetCurSel();

	//CRect rc;
	//m_listAd.GetItemRect(iSel,&rc);
	//CPoint pt;
	//pt.x = rc.left + rc.Width()/2;
	//pt.y = rc.top + rc.Height()/2;

	//CMenu menu;
	//menu.LoadMenu(IDR_MENU_AD);
	//CMenu *pMenu = menu.GetSubMenu(0);
	//if(pMenu!=NULL)
	//{
	//	m_listAd.ClientToScreen(&pt);
	//	if (iSel < 0)
	//	{
	//		pMenu->EnableMenuItem(ID_AD_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//		pMenu->EnableMenuItem(ID_AD_MODIFY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//	}
	//	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this); 
	//	
	//}
}

void CStationCfg::OnTipAdd()
{
	// TODO: Add your command handler code here
}

void CStationCfg::OnAdAdd()
{
	// TODO: Add your command handler code here
	UINT nCount = m_listAd.GetCount();
	int iSel = m_listAd.GetCurSel();
	CRect rc;
	
	if (0 == nCount)
	{
		return;
	}
	else
	{
	}
}

void CStationCfg::OnAdModify()
{
	// TODO: Add your command handler code here
	int iSel = m_listAd.GetCurSel();
}

void CStationCfg::OnAdDelete()
{
	// TODO: Add your command handler code here
	int iSel = m_listAd.GetCurSel();
	int ret = MessageBox(_T("Are you sure you want to delete this advertisement?"),_T("Prompt"), MB_OKCANCEL);
	if (IDCANCEL == ret)
	{
		return;
	}

	//读配置文件
	CString strIni = _T("\\PTU.ini");
	strIni = theApp.GetAppPath() + strIni;
	CString szValue;
	GetPrivateProfileString(_T("AD"), _T("AdCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
	int nAdCount = _ttoi(szValue);

	//写配置文件
	CString szKey ;
	CString strValue;
	szKey.Format( _T("Ad%d"),iSel);
	WritePrivateProfileString(_T("AD"), szKey, NULL, strIni);
	for (int i = iSel + 1; i< nAdCount; i++)
	{
		CString strName;
		strName.Format(_T("AD%d"), i);
		GetPrivateProfileString(_T("AD"), strName, _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
		strValue = szValue;
		strName.Format(_T("AD%d"), i - 1);
		WritePrivateProfileString(_T("AD"), _T("AdCount"), strValue, strIni);
	}
	strValue.Format(_T("%d"),nAdCount - 1);
	WritePrivateProfileString(_T("AD"), _T("AdCount"), strValue, strIni);

	m_listAd.DeleteString(iSel);
}

LRESULT CStationCfg::OnAddMsg(WPARAM wParam, LPARAM lParam)
{
	DoLoadAdsAndTipsList();
	DoLoadList();
	return 0L;
}

LRESULT CStationCfg::OnDeleteMsg(WPARAM wParam, LPARAM lParam)
{
	DoLoadAdsAndTipsList();
	return 0L;
}

void CStationCfg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CString strBk;
	strBk = theApp.GetAppPath() + _T("//ImageSrc//StationConfig.jpg");
	//CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	Image  myImage(strBk.GetBuffer());   //创建制定大小的BMP位图文件
	Graphics graphics(dc);

	graphics.DrawImage( 
		&myImage, 
		Rect( 0, 0, rect.Width(), rect.Height()), 
		0, 
		0, 
		myImage.GetWidth(), 
		myImage.GetHeight(), 
		UnitPixel, 
		NULL, 
		NULL, 
		NULL
		) ;
}

HBRUSH CStationCfg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	//if(nCtlColor == CTLCOLOR_STATIC)
	//{
	//	pDC->SetTextColor(RGB(0,0,0)); 
	//	pDC->SetBkMode(TRANSPARENT);
	//	return (HBRUSH)::GetStockObject(NULL_BRUSH);
	//}
	if(CTLCOLOR_STATIC == nCtlColor)
	{
		if(IDC_CHECK_CFG_ALL == pWnd->GetDlgCtrlID())
		{
			//pDC->SetTextColor(RGB(255,0,0));
			//hbr = (HBRUSH)GetStockObject(WHITE_BRUSH)
			return hbr;
		}
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CStationCfg::OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult)
{	
	CSingleLock lock(&m_Mutex);
	lock.Lock();
 	 
//  	int nCount = m_listCtrl.GetItemCount();
//  	for (int i=0; i<nCount; i++)
//  	{
//  		if (m_listCtrl.GetCheck(i))
//  			m_listCtrl.SetCheck(i, FALSE);
//  	}

 	int iMark = m_listCtrl.GetSelectionMark();	
	if (iMark == -1)
	{
		return;
	}
	OnDeleteListBoxString(&m_listPreTipsSelected);
	OnDeleteListBoxString(&m_listArrivalTipsSelected);
	OnDeleteListBoxString(&m_listPreAdsSelected);
	OnDeleteListBoxString(&m_listArrivalAdsSelected);

	//m_listCtrl.SetCheck(iMark, TRUE);
	int nEnglish=0;
	int iSelList = 	((CComboBox*)GetDlgItem(IDC_COMBO_CFG_LAN))->GetCurSel();
	if(iSelList==0)
	{
		nEnglish = 0;
	}
	else
	{
		nEnglish = 1;
	}

// 	list<BYTE>::iterator iter = m_listArr[nEnglish][iMark].begin();
// 	for (; iter!=m_listArr[nEnglish][iMark].end(); iter++)
// 	{
// 		BYTE byteInfo =(BYTE)*iter;
// 		TRACE("%x ", byteInfo);
// 	}
// 	TRACE("\n");

 	OnSetPreTipsSelectedInfo(&m_listArr[nEnglish][iMark]);
 	OnSetArrivalTipsSelectedInfo(&m_listArr[nEnglish][iMark]);
  	OnSetPreAdsSelectedInfo(&m_listArr[nEnglish][iMark]);
  	OnSetArrivalAdsSelectedInfo(&m_listArr[nEnglish][iMark]);

	if (m_bOption)
	{
		OnlistCopyData(&m_listArr[nEnglish][m_nselRow], &m_list);		
	}
	OnlistCopyData(&m_list, &m_listArr[nEnglish][iMark], FALSE);

	m_nselRow = iMark;
	m_bOption = FALSE;

	lock.Unlock();
	*pResult = 0;
}

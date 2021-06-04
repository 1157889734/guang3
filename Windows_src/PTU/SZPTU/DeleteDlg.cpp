// DeleteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteDlg.h"
#include "SZPTU.h"
#include "MainFrm.h"

// CDeleteDlg dialog

IMPLEMENT_DYNAMIC(CDeleteDlg, CDialog)

CDeleteDlg::CDeleteDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDeleteDlg::IDD, pParent)
{
	m_brush2.CreateSolidBrush(RGB(232,249,255));

}

CDeleteDlg::~CDeleteDlg()
{
}

void CDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_RADIO_TIP, m_radioTip);
	DDX_Control(pDX, IDC_RADIO_AD, m_radioAd);
	DDX_Control(pDX, IDC_RADIO_BROADCAST, m_radioBroadcast);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
}


BEGIN_MESSAGE_MAP(CDeleteDlg, CDialog)
	ON_MESSAGE(WM_ADD_MSG, &CDeleteDlg::OnAddMsg)
	ON_MESSAGE(WM_DELETE_MSG, &CDeleteDlg::OnDeleteMsg)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDeleteDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_CHECK_ALL, &CDeleteDlg::OnBnClickedCheckAll)
	ON_BN_CLICKED(IDC_RADIO_AD, &CDeleteDlg::OnBnClickedRadioAd)
	ON_BN_CLICKED(IDC_RADIO_TIP, &CDeleteDlg::OnBnClickedRadioTip)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_BROADCAST, &CDeleteDlg::OnBnClickedRadioBroadcast)
END_MESSAGE_MAP()


// CDeleteDlg message handlers

BOOL CDeleteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
	m_list.SetExtendedStyle(dwStyle);	

	m_list.InsertColumn(0,_T("No."),LVCFMT_CENTER,70);
	m_list.InsertColumn(1,_T("English."),LVCFMT_LEFT,100);
	m_list.InsertColumn(2,_T("中文"),LVCFMT_LEFT,100);

	//((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(CB_OKAY);
	m_radioTip.SetCheck(BST_CHECKED);

	LoadMyImage(&m_btnDelete, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));

	DoLoadList();	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteDlg::LayoutList()
{
	int iColMinW1 = 70;
	int iColMinW2 = 70;
	int iCount = m_list.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		int iTemp;
		iTemp = m_list.GetStringWidth(m_list.GetItemText(i,1));
		if (iTemp > iColMinW1)
		{
			iColMinW1 = iTemp+20;
			m_list.SetColumnWidth(1,iColMinW1);
		}
		iTemp = m_list.GetStringWidth(m_list.GetItemText(i,2));
		if (iTemp > iColMinW2)
		{
			iColMinW2 = iTemp+20;
			m_list.SetColumnWidth(2,iColMinW2);
		}
	}
}

void CDeleteDlg::DoLoadList()
{
	//清空ListCtrl控件的内容
	m_list.DeleteAllItems();

	if (BST_CHECKED == m_radioTip.GetCheck())		//提示信息
	{
		ASSERT((CMainFrame*)AfxGetMainWnd());
		int nSize = (int)((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.size();
		for (int i=0;i<nSize;i++)
		{
			CString strNo;
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(i);
			strNo.Format(_T("%d"),i+1);
			m_list.InsertItem(i,strNo);
			m_list.SetItemText(i,1,strValue[LAN_EN]);
			m_list.SetItemText(i,2,strValue[LAN_IRAN]);		
		}
	}
	else if (BST_CHECKED == m_radioAd.GetCheck())
	{
		int nSize = (int)((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.size();
		for (int i=0;i<nSize;i++)
		{
			CString strNo;
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(i);
			strNo.Format(_T("%d"),i+1);
			m_list.InsertItem(i,strNo);
			m_list.SetItemText(i,1,strValue[LAN_EN]);
			m_list.SetItemText(i,2,strValue[LAN_IRAN]);		
		}
	}
	else if(m_radioBroadcast.GetCheck()==1)
	{
		int nSize = (int)((CMainFrame*)AfxGetMainWnd())->m_vLineBcName.size();
		for (int i=0;i<nSize;i++)
		{
			CString strNo;
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineBcName.at(i);
			strNo.Format(_T("%d"),i+1);
			m_list.InsertItem(i,strNo);
			m_list.SetItemText(i,1,strValue[LAN_EN]);
			m_list.SetItemText(i,2,strValue[LAN_IRAN]);		
		}

	}
	else
	{
		return;
	}

	int iCount = m_list.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_list.SetCheck(i,FALSE);
	}
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
	//////////////////////////////////////////////////////////////////////////
	LayoutList();
}

//删除广告和提示信息
void CDeleteDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	int iSelType = 0;
	if (BST_CHECKED == m_radioTip.GetCheck())
	{
		iSelType = 0;
	}
	else if (BST_CHECKED == m_radioAd.GetCheck())
	{
		iSelType = 1;
	}
	else if(BST_CHECKED == m_radioBroadcast.GetCheck())
	{
		iSelType =2;

	}
	else
	{
		return;
	}
	int  bCheckGroup[MAX_PATH] = {-1};		//记录已经选中行
	int  iSelCout = 0;		//需要配置的行数
	int  iCout = m_list.GetItemCount();		//条目总数
	for (int i=0;i<iCout;i++)
	{
		if (m_list.GetCheck(i))
		{
			bCheckGroup[iSelCout] = i;		//选中，记录选中的列
			iSelCout++;
		}
	}

	//判断选择需要删除的项是否为0
	if (0 == iSelCout)
	{
		MessageBox(_T("Please Select One Item to delete!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else
	{
		int iRet = MessageBox(_T("Are you sure to delete all the selected items?"), _T("Prompt"), MB_OKCANCEL);
		if (IDCANCEL == iRet)
		{
			return;
		}
		else if(IDOK == iRet)
		{
		}
		else
		{
			return;
		}
	}

	for (int i = iSelCout - 1; i >= 0 ; i--)
	{
		DeleteConfigItem(iSelType, bCheckGroup[i]);
	}
/*
	for (int i = iSelCout - 1; i >= 0 ; i--)
	{
		m_list.DeleteItem(bCheckGroup[i]);
	}
*/
	
	MessageBox(_T("Delete the selected items successfully!"), _T("Prompt"), MB_ICONWARNING);
	GetParent()->GetParent()->GetParent()->SendMessage(WM_DELETE_MSG, NULL, NULL);
	//GetParent()->GetParent()->SendMessage(WM_DELETE_MSG, NULL, NULL);
	GetParent()->SendMessage(WM_DELETE_MSG, NULL, NULL);

	DoLoadList();
}

void CDeleteDlg::OnBnClickedCheckAll()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	int iCount = m_list.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_list.SetCheck(i,bCheck);
	}
}


//从配置文件中删除信息
void CDeleteDlg::DeleteConfigItem(int iType, int iNo)
{
	if (0 == iType)
	{
		DeleteTipItem(iNo);
	}
	else if (1 == iType)
	{
		DeleteAdItem(iNo);
	}
	else if(2==iType)
	{
		DeleteBroadcastItem(iNo);

	}
	else
	{
		return;
	}
}

//从配置文件中删除选中的广告信息
void CDeleteDlg::DeleteAdItem(int iNo)
{
	CString strApp = _T("AD");	//the name of the section
	CString strKey;				//the name of the key 
	CString strValue;			//the buffer that receives the retrieved string	
	CString strWrite;			//a null-terminated string to be written to the file
	CString strFile = _T("\\PTU.ini");		//the name of the initialization file
	strFile = theApp.GetAppPath() + strFile;
	int iCount = 0;		//广告信息总数目

	//获取广告信息数目
	strKey = _T("AdCount");
	GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
	char szANSIString [MAX_PATH]; 
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL );
	iCount = atoi(szANSIString);

	//删除指定的广告信息
	if(iNo == iCount - 1)		//删除的是最后一条广告
	{
		strKey.Format(_T("Ad%d"), iNo);
		WritePrivateProfileString(strApp, strKey, NULL, strFile);
	}
	else
	{
		for(int i = iNo; i < iCount; i++)
		{
			if(i == iCount - 1)
			{
				strKey.Format(_T("Ad%d"), i);
				//strKey = _T("");
				WritePrivateProfileString(strApp, strKey, NULL, strFile);
			}
			else
			{
				strKey.Format(_T("Ad%d"), i + 1);
				GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
				strKey.Format(_T("Ad%d"), i);
				strWrite = strValue;
				WritePrivateProfileString(strApp, strKey, strWrite, strFile);
			}
		}
	}

	//更改广告信息的数目
	strKey = _T("AdCount");
	strWrite.Format(_T("%d"), iCount - 1);
	WritePrivateProfileString(strApp, strKey, strWrite, strFile);

}

//从配置文件中删除选中的提示信息
void CDeleteDlg::DeleteTipItem(int iNo)
{
	CString strApp = _T("ExtraTips");		// the name of the section
	CString strKey;				//the name of the key 
	CString strValue;			//the buffer that receives the retrieved string	
	CString strWrite;			//a null-terminated string to be written to the file
	CString strFile = _T("\\PTU.ini");		//the name of the initialization file
	strFile = theApp.GetAppPath() + strFile;
	int iCount = 0;		//提示信息总数目

	//获取提示信息数目
	strKey = _T("TipsCount");
	GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
	char szANSIString [MAX_PATH]; 
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL );
	iCount = atoi(szANSIString);

	//删除指定的提示信息
	if(iNo == iCount - 1)		//删除的是最后一条提示信息
	{
		strKey.Format(_T("Tips%d"), iNo);
		WritePrivateProfileString(strApp, strKey, NULL, strFile);
	}
	else
	{
		for(int i = iNo; i < iCount; i++)
		{
			if(i == iCount - 1)
			{
				strKey = _T("");
				strKey.Format(_T("Tips%d"),i);
				WritePrivateProfileString(strApp, strKey, NULL, strFile);
			}
			else
			{
				strKey.Format(_T("Tips%d"), i + 1);
				GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
				strKey.Format(_T("Tips%d"), i);
				strWrite = strValue;
				WritePrivateProfileString(strApp, strKey, strWrite, strFile);
			}

		}
	}

	//更改提示信息的数目
	strKey = _T("TipsCount");
	strWrite.Format(_T("%d"), iCount - 1);
	WritePrivateProfileString(strApp, strKey, strWrite, strFile);
}

BOOL CDeleteDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if( pMsg->message ==WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDeleteDlg::OnBnClickedRadioAd()
{
	// TODO: Add your control notification handler code here
	DoLoadList();
}

void CDeleteDlg::OnBnClickedRadioTip()
{
	// TODO: Add your control notification handler code here
	DoLoadList();
}

LRESULT CDeleteDlg::OnAddMsg(WPARAM wParam,LPARAM lParam)
{
	DoLoadList();
	return 0L;
}

LRESULT CDeleteDlg::OnDeleteMsg(WPARAM wParam,LPARAM lParam)
{
	DoLoadList();
	return 0L;
}

void CDeleteDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CString strBk;
	strBk = theApp.GetAppPath() + _T("//ImageSrc//Customize .png");
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

HBRUSH CDeleteDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
	
	// TODO:  Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC||pWnd->GetDlgCtrlID()==IDC_CHECK_ALL)
	{
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH)::GetStockObject(NULL_BRUSH);
		return m_brush2;
	}
    
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CDeleteDlg::OnBnClickedRadioBroadcast()
{
	// TODO: 在此添加控件通知处理程序代码
	DoLoadList();
}

void CDeleteDlg::DeleteBroadcastItem(int iNo)
{
	CString strApp = _T("BROADCAST");	//the name of the section
	CString strKey;				//the name of the key 
	CString strValue;			//the buffer that receives the retrieved string	
	CString strWrite;			//a null-terminated string to be written to the file
	CString strFile = _T("\\PTU.ini");		//the name of the initialization file
	strFile = theApp.GetAppPath() + strFile;
	int iCount = 0;		//广告信息总数目

	//获取广告信息数目
	strKey = _T("BroadCastCount");
	GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
	char szANSIString [MAX_PATH]; 
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL );
	iCount = atoi(szANSIString);

	//删除指定的广告信息
	if(iNo == iCount - 1)		//删除的是最后一条广告
	{
		strKey.Format(_T("BroadCast%d"), iNo);
		WritePrivateProfileString(strApp, strKey, NULL, strFile);
	}
	else
	{
		for(int i = iNo; i < iCount; i++)
		{
			if(i == iCount - 1)
			{
				strKey.Format(_T("BroadCast%d"), i);
				//strKey = _T("");
				WritePrivateProfileString(strApp, strKey, NULL, strFile);
			}
			else
			{
				strKey.Format(_T("BroadCast%d"), i + 1);
				GetPrivateProfileString(strApp, strKey, _T(""), strValue.GetBuffer(MAX_PATH), MAX_PATH, strFile);
				strKey.Format(_T("BroadCast%d"), i);
				strWrite = strValue;
				WritePrivateProfileString(strApp, strKey, strWrite, strFile);
			}
		}
	}

	//更改广告信息的数目
	strKey = _T("BroadCastCount");
	strWrite.Format(_T("%d"), iCount - 1);
	WritePrivateProfileString(strApp, strKey, strWrite, strFile);

}

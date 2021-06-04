// DownLoadLED.cpp : implementation file
//

#include "stdafx.h"
#include "DownLoadLED.h"
#include "SZPTU.h"
#include "MainFrm.h"
#include <Shlwapi.h>

//list列序号
#define LIST_COL_NO        0x00
#define LIST_COL_NAME_EN   0x01
#define LIST_COL_NAME_IRAN 0x02
#define LIST_COL_PATH_EN   0x03
#define LIST_COL_PATH_IRAN 0x04


// CDownLoadLED dialog

IMPLEMENT_DYNAMIC(CDownLoadLED, CDialog)

CDownLoadLED::CDownLoadLED(CWnd* pParent /*=NULL*/, LPWND_INFO pInfo /*=NULL*/)
	: CDialog(CDownLoadLED::IDD, pParent)
{
	m_isdownall=false;
	memset(&m_WndInfo, 0, sizeof(m_WndInfo));
	if (NULL != pInfo)
	{
		memcpy(&m_WndInfo, pInfo, sizeof(WND_INFO));
	}
	m_brush2.CreateSolidBrush(RGB(232,249,255));
}

CDownLoadLED::~CDownLoadLED()
{
}

void CDownLoadLED::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWN, m_list1);
	DDX_Control(pDX, IDC_PROGRESS_D_PROC, m_procCtrl);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_MULTI_DOWN, m_btnDownload);
	DDX_Control(pDX, IDC_BUTTON_MULTI_DOWNALL, m_btnDownloadAll);
}


BEGIN_MESSAGE_MAP(CDownLoadLED, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_DOWN, &CDownLoadLED::OnBnClickedButtonMultiDown)
	ON_CBN_SELCHANGE(IDC_COMBO_D_TYPE, &CDownLoadLED::OnCbnSelchangeComboDType)
	ON_BN_CLICKED(IDC_CHECK_ALL, &CDownLoadLED::OnBnClickedCheckAll)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDownLoadLED::OnBnClickedButtonClose)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_MULTI_DOWNALL, &CDownLoadLED::OnBnClickedButtonMultiDownall)
	ON_CBN_SELCHANGE(IDC_COMBO_D_CITYTYPE, &CDownLoadLED::OnCbnSelchangeComboDCitytype)
END_MESSAGE_MAP()


// CDownLoadLED message handlers

void CDownLoadLED::OnBnClickedButtonMultiDown()
{	
	int isel_type = ((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->GetCurSel();
	int isel_color = ((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->GetCurSel();
	int isel_lan = ((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->GetCurSel();
	int isel_screentype = ((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->GetCurSel();
	//int isel_method = m_WndInfo.uPort;
	m_WndInfo.uColor=isel_color;//红色
	m_WndInfo.uLan=isel_lan;//英语
	m_WndInfo.uType=isel_screentype;//内部屏
	DownOneType(isel_type);
	////////////////////////////////////////////////////////////////////////	
}

BOOL CDownLoadLED::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->EnableWindow(false);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
	if(m_isdownall==TRUE)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->EnableWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->EnableWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->EnableWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_CITYTYPE))->ShowWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_ADCOLOR))->ShowWindow(TRUE);
		((CStatic*)GetDlgItem(IDC_STATIC_ADCOLOR))->ShowWindow(TRUE);
		((CStatic*)GetDlgItem(IDC_STATIC_CITY))->ShowWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_MULTI_DOWNALL))->ShowWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_MULTI_DOWN))->ShowWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_CITYTYPE))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_ADCOLOR))->SetCurSel(0);
		((CButton*)GetDlgItem(IDC_CHECK_ALL))->EnableWindow(false);


	}
	//////////////////////////////////////////////////
	DWORD dwStyle = m_list1.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
	m_list1.SetExtendedStyle(dwStyle);	

	m_list1.InsertColumn(LIST_COL_NO,_T("No."),LVCFMT_CENTER,45);
	m_list1.InsertColumn(LIST_COL_NAME_EN,_T("English."),LVCFMT_LEFT,200);
	m_list1.InsertColumn(LIST_COL_NAME_IRAN,_T("中文"),LVCFMT_LEFT,200);
	m_list1.InsertColumn(LIST_COL_PATH_EN,_T("Bmp Path(English)."),LVCFMT_LEFT,0);
	m_list1.InsertColumn(LIST_COL_PATH_IRAN,_T("Bmp Path(中文)."),LVCFMT_LEFT,0);

	InitPanel();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDownLoadLED::InitPanel()
{
	//((CComboBox*)GetDlgItem(IDC_COMBO_D_METHOD))->SetCurSel(0);
	//((CComboBox*)GetDlgItem(IDC_COMBO_D_CLR))->SetCurSel(0);
	//((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
	//((CComboBox*)GetDlgItem(IDC_COMBO_D_LEDTYPE))->SetCurSel(0);

	//((CButton*)GetDlgItem(IDC_CHECK_TYPE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(0);

	LoadMyImage(&m_btnClose, _T("//ImageSrc//Close_Normal.png"), _T("//ImageSrc//Close_Push.png"), _T("//ImageSrc//Close_Enter.png"), _T(""));
	LoadMyImage(&m_btnDownload, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnDownloadAll, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));

	DoLoadList();	
}

void CDownLoadLED::DoLoadList()
{
	int iSelList = 	((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->GetCurSel();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
	m_list1.DeleteAllItems();
	CString strPicPathEn,strPicPathIran;
	strPicPathEn.Format(_T("%s\\Pic\\En\\"),theApp.GetAppPath());		//英语图片存储路径
	strPicPathIran.Format(_T("%s\\Pic\\Iran\\"),theApp.GetAppPath());	//波斯文图片存储路径
	CString strTemp;
	switch (iSelList)
	{
	case 0:
	case 1:
	case 2:
		{
			//line
			int nSize = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineStationName[iSelList].size();
			for (int i=0;i<nSize;i++)
			{
				CString strNo;
				XString strValue = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineStationName[iSelList].at(i);

				strNo.Format(_T("%d"),i+1);
				m_list1.InsertItem(i,strNo);
				m_list1.SetItemText(i,LIST_COL_NAME_EN,strValue[LAN_EN]);
				m_list1.SetItemText(i,LIST_COL_NAME_IRAN,strValue[LAN_IRAN]);

				strTemp.Format(_T("%sLine%d_Station%d.bmp"),strPicPathEn,iSelList+1,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,_T("Not Find The Bmp File"));
				}
				strTemp.Format(_T("%sLine%d_Station%d.bmp"),strPicPathIran,iSelList+1,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,_T("Not Find The Bmp File"));
				}
			}
		}
		break;
	case 3:
		{
			//tips
			int nSize = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineTipName.size();
			int nExtraSize = ((CMainFrame *)theApp.m_pMainWnd)->m_vAddTipsName.size();
			for (int i=0;i<nSize;i++)
			{
				CString strNo;
				XString strValue = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineTipName.at(i);

				strNo.Format(_T("%d"),i+1);
				m_list1.InsertItem(i,strNo);
				m_list1.SetItemText(i,LIST_COL_NAME_EN,strValue[LAN_EN]);
				m_list1.SetItemText(i,LIST_COL_NAME_IRAN,strValue[LAN_IRAN]);

				strTemp.Format(_T("%sTips%d.bmp"),strPicPathEn,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,_T("Not Find The Bmp File"));
				}
				strTemp.Format(_T("%sTips%d.bmp"),strPicPathIran,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,_T("Not Find The Bmp File"));
				}
			}

			for (int i=nSize;i<nSize+nExtraSize;i++)
			{
				CString strNo;
				XString strValue = ((CMainFrame *)theApp.m_pMainWnd)->m_vAddTipsName.at(i-nSize);

				strNo.Format(_T("%d"),i+1);
				m_list1.InsertItem(i,strNo);
				m_list1.SetItemText(i,LIST_COL_NAME_EN,strValue[LAN_EN]);
				m_list1.SetItemText(i,LIST_COL_NAME_IRAN,strValue[LAN_IRAN]);

				strTemp.Format(_T("%sTips%d.bmp"),strPicPathEn,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,_T("Not Find The Bmp File"));
				}
				strTemp.Format(_T("%sTips%d.bmp"),strPicPathIran,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,_T("Not Find The Bmp File"));
				}
			}
		}
		break;
	case 4:
		{
			//ad
			int nSize = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineAdName.size();
			for (int i=0;i<nSize;i++)
			{
				CString strNo;

				XString strValue = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineAdName.at(i);
				strNo.Format(_T("%d"),i+1);
				m_list1.InsertItem(i,strNo);
				m_list1.SetItemText(i,LIST_COL_NAME_EN,strValue[LAN_EN]);
				m_list1.SetItemText(i,LIST_COL_NAME_IRAN,strValue[LAN_IRAN]);

				strTemp.Format(_T("%sAd%d.bmp"),strPicPathEn,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,_T("Not Find The Bmp File"));
				}
				strTemp.Format(_T("%sAd%d.bmp"),strPicPathIran,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,_T("Not Find The Bmp File"));
				}
			}
		}
		break;
	case 5:
		{
			//bc
			int nSize = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineBcName.size();
			for (int i=0;i<nSize;i++)
			{
				CString strNo;

				XString strValue = ((CMainFrame *)theApp.m_pMainWnd)->m_vLineBcName.at(i);

				strNo.Format(_T("%d"),i+1);
				m_list1.InsertItem(i,strNo);
				m_list1.SetItemText(i,LIST_COL_NAME_EN,strValue[LAN_EN]);
				m_list1.SetItemText(i,LIST_COL_NAME_IRAN,strValue[LAN_IRAN]);

				strTemp.Format(_T("%sBc%d.bmp"),strPicPathEn,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_EN,_T("Not Find The Bmp File"));
				}
				strTemp.Format(_T("%sBc%d.bmp"),strPicPathIran,i+1);//已保存路径
				if(PathFileExists(strTemp))
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,strTemp);
				}
				else
				{
					m_list1.SetItemText(i,LIST_COL_PATH_IRAN,_T("Not Find The Bmp File"));
				}
			}
		}
		break;
	}
	//默认选中
	int iCount = m_list1.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		//m_list1.SetCheck(i,TRUE);
		m_list1.SetCheck(i,FALSE);
	}
	LayoutList();

}
void CDownLoadLED::OnCbnSelchangeComboDType()
{
    int isel=((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->GetCurSel();
	if(isel==0||isel==1||isel==2)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->EnableWindow(false);
		//((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->EnableWindow(TRUE);

	}
	if(isel==3)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->EnableWindow(false);
		//((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->EnableWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);

	}
	if(isel==4||isel==5)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->EnableWindow(false);
		((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);

	}
	
	DoLoadList();
}

void CDownLoadLED::OnBnClickedCheckAll()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	int iCount = m_list1.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_list1.SetCheck(i,bCheck);
	}
}

void CDownLoadLED::LayoutList()
{
	int iColMinW1 = 70;
	int iColMinW2 = 70;
    int iColMinW3 = 70;
	int iColMinW4 = 70;
	int iCount = m_list1.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		int iTemp;
		iTemp = m_list1.GetStringWidth(m_list1.GetItemText(i,LIST_COL_NAME_EN));
		if (iTemp > iColMinW1)
		{
			iColMinW1 = iTemp+20;
			m_list1.SetColumnWidth(LIST_COL_NAME_EN,iColMinW1);
		}
		iTemp = m_list1.GetStringWidth(m_list1.GetItemText(i,LIST_COL_NAME_IRAN));
		if (iTemp > iColMinW2)
		{
			iColMinW2 = iTemp+20;
			m_list1.SetColumnWidth(LIST_COL_NAME_IRAN,iColMinW2);
		}
		iTemp = m_list1.GetStringWidth(m_list1.GetItemText(i,LIST_COL_PATH_EN));
		if (iTemp > iColMinW3)
		{
			iColMinW3 = iTemp+20;
			m_list1.SetColumnWidth(LIST_COL_PATH_EN,iColMinW3);
		}
		iTemp = m_list1.GetStringWidth(m_list1.GetItemText(i,LIST_COL_PATH_IRAN));
		if (iTemp > iColMinW4)
		{
			iColMinW4 = iTemp+20;
			m_list1.SetColumnWidth(LIST_COL_PATH_IRAN,iColMinW4);
		}
	}
}

BOOL CDownLoadLED::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE||pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDownLoadLED::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if(m_isdownall)
	{
		CString strBk;
		strBk = theApp.GetAppPath() + _T("//ImageSrc//DownAllBk.png");
		//CClientDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		Image  myImage(strBk.GetBuffer());   //创建制定大小的BMP位图文件
		Graphics graphics(dc);

		graphics.DrawImage( 
			&myImage, 
			Rect( 0, 0, rect.Width(), rect.Height()), 
			10, 
			10, 
			myImage.GetWidth() - 18, 
			myImage.GetHeight() - 16, 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;

	}
	else
	{
		CString strBk;
		strBk = theApp.GetAppPath() + _T("//ImageSrc//DownByTypeBk.png");
		//CClientDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		Image  myImage(strBk.GetBuffer());   //创建制定大小的BMP位图文件
		Graphics graphics(dc);

		graphics.DrawImage( 
			&myImage, 
			Rect(0, 0, rect.Width(), rect.Height()), 
			10, 
			10, 
			myImage.GetWidth() - 18, 
			myImage.GetHeight() - 16, 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;

	}
	
}

void CDownLoadLED::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_CLOSE, NULL, NULL);
}

HBRUSH CDownLoadLED::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		//if (IDC_CHECK_ALL == pWnd->GetDlgCtrlID())
		//{
		//	return hbr;
		//}
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}
    if(pWnd->GetDlgCtrlID()==IDC_CHECK_ALL)
	{
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return m_brush2;
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CDownLoadLED::DownOneType(int isel_type)
{
	CString strMsg;
	int isel_clr = m_WndInfo.uColor;
	int isel_lan = m_WndInfo.uLan;
	int isel_ledtype = m_WndInfo.uType;

	int isel_method = 0;
	if (!((CMainFrame *)theApp.m_pMainWnd)->m_comisopen && !((CMainFrame *)theApp.m_pMainWnd)->m_ptctcusb)
	{
		//AfxMessageBox(_T("Communication Port Not Opened!!!"));
		MessageBox(_T("Communication Port Not Opened!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else if(((CMainFrame *)theApp.m_pMainWnd)->m_comisopen)
	{
		isel_method = 0;
	}
	else if (((CMainFrame *)theApp.m_pMainWnd)->m_ptctcusb)
	{
		isel_method = 1;
	}
	else
	{
		return;
	}

	//检查路径
	int  bCheckGroup[MAX_PATH] = {-1};//记录已经选中行
	int  iSelCout = 0;//需要下载的行数
	int  iCout = m_list1.GetItemCount();//条目总数
	for (int i=0;i<iCout;i++)
	{
		if (m_list1.GetCheck(i))
		{
			if (isel_lan == 0)
			{
				//英语
				if (!PathFileExists(m_list1.GetItemText(i,LIST_COL_PATH_EN)))
				{
					strMsg.Format(_T("Item %d English .Bmp File Not Exist,Please Set Lattice First!!!"),i+1);
					//AfxMessageBox(strMsg);
					MessageBox(strMsg, _T("Prompt"), MB_ICONWARNING);
					return;
				}
			}
			if (isel_lan == 1)
			{
				//IRAN
				if (!PathFileExists(m_list1.GetItemText(i,LIST_COL_PATH_IRAN)))
				{
					strMsg.Format(_T("Item %d Farsi .Bmp File Not Exist,Please Set Lattice First!!!"),i+1);
					//AfxMessageBox(strMsg);
					MessageBox(strMsg, _T("Prompt"), MB_ICONWARNING);
					return;
				}
			}
			bCheckGroup[iSelCout] = i;//选中，记录选中的列
			iSelCout++;
		}
	}

	//判断是否有选中
	if (0 == iSelCout)
	{
		MessageBox(_T("No item selected!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	CLatticeSet *pClittaceSet = (CLatticeSet*)this->GetParent();
	//开始下载
	for (int i =0;i<iSelCout;i++)
	{	
		switch (isel_ledtype)
		{
		case LED_IDU:
			{
				pClittaceSet->DownLoadStaticPic(i+1,iSelCout,m_list1.GetItemText(bCheckGroup[i],isel_lan + LIST_COL_PATH_EN),isel_method,IDU_VALUE,isel_type+1,bCheckGroup[i]+1,isel_clr + 1,isel_lan);
			}
			break;
		case LED_FDU:
			{
				pClittaceSet->DownLoadStaticPic(i+1,iSelCout,m_list1.GetItemText(bCheckGroup[i],isel_lan + LIST_COL_PATH_EN),isel_method,FDU_VALUE,isel_type+1,bCheckGroup[i]+1,isel_clr + 1,isel_lan);
			}
			break;
		case LED_SDU:
			{
				pClittaceSet->DownLoadStaticPic(i+1,iSelCout,m_list1.GetItemText(bCheckGroup[i],isel_lan + LIST_COL_PATH_EN),isel_method,SDU_VALUE,isel_type+1,bCheckGroup[i]+1,isel_clr + 1,isel_lan);					
			}
			break;
		default:
			break;
		}		
		m_procCtrl.SetRange(0,100);
		m_procCtrl.SetPos((i+1)*100/iSelCout);
	}
	strMsg.Format(_T("Download %d Items Finished!!!"),iSelCout);
	//AfxMessageBox(strMsg);
	if(m_isdownall==false)
	   MessageBox(strMsg, _T("Prompt"), MB_OK);
	m_procCtrl.SetPos(0);

}

void CDownLoadLED::SetDownAll(bool isdownall)
{
	m_isdownall=isdownall;
}
DWORD WINAPI CDownLoadLED::WorkThread( LPVOID lParam )
{
	CDownLoadLED *pDownLoadWnd = (CDownLoadLED*)lParam;
	ASSERT(NULL != pDownLoadWnd);	
	pDownLoadWnd->DownAllPic();//下载
	
	return 0;
}
void CDownLoadLED::OnBnClickedButtonMultiDownall()
{
	// TODO: 在此添加控件通知处理程序代码
	////////////////////////////////////////////////////////////////////
	DWORD dwID; 
	m_hThread = CreateThread(NULL,0,WorkThread,this,CREATE_SUSPENDED,&dwID);
	ResumeThread(m_hThread);//下载
	////////////////////////////////////////////////////////////////
	//DownAllPic();	
}

void CDownLoadLED::DownAllPic(void)
{
	int addcolor=((CComboBox*)GetDlgItem(IDC_COMBO_D_ADCOLOR))->GetCurSel();
	////////////////////////////////////////////////////////////////////

	int cityint=((CComboBox*)GetDlgItem(IDC_COMBO_D_CITYTYPE))->GetCurSel();
	switch (cityint)
	{
	case 0:
		{

			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			//MessageBox(_T("下载期间不能操作界面"));
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();

		}
		break;
	case 1:
		{

			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
		}
		break;
	case 2:
		{

			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
		}
		break;
	case 3:
		{

			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			///////////////////////////////////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			///////////////////////////////////////////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(0);
			DoLoadList();
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
			OnBnClickedCheckAll();
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			//////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(1);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
			/////////////////////////////////////////////////////////////
			((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(2);
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
			OnBnClickedButtonMultiDown();
			((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
			OnBnClickedButtonMultiDown();
		}
		break;
	}
	//////////////////////////////////////////////////////////////////////////
	((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(3);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(addcolor);
	DoLoadList();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
	OnBnClickedCheckAll();
	OnBnClickedButtonMultiDown();
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
	OnBnClickedButtonMultiDown();
	////////////////////////////////////////////////////////
	((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(4);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(addcolor);
	DoLoadList();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
	OnBnClickedCheckAll();
	OnBnClickedButtonMultiDown();
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
	OnBnClickedButtonMultiDown();
	////////////////////////////////////////////////////////
	((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(5);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_SCREENTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_D_COLOR))->SetCurSel(addcolor);
	DoLoadList();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(1);
	OnBnClickedCheckAll();
	OnBnClickedButtonMultiDown();
	((CComboBox*)GetDlgItem(IDC_COMBO_D_LAN))->SetCurSel(1);
	OnBnClickedButtonMultiDown();
	///////////////////////////////////////
	MessageBox(_T("The download is finished"));
}

void CDownLoadLED::OnCbnSelchangeComboDCitytype()
{
	// TODO: 在此添加控件通知处理程序代码
	int cityint=((CComboBox*)GetDlgItem(IDC_COMBO_D_CITYTYPE))->GetCurSel();
	if(cityint==0||cityint==1||cityint==2)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(cityint);
	}
	if(cityint==3)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_D_TYPE))->SetCurSel(0);

	}
	DoLoadList();
}

// CustomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OperateDlg.h"
#include "SZPTU.h"


// COperateDlg dialog

IMPLEMENT_DYNAMIC(COperateDlg, CDialog)

COperateDlg::COperateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperateDlg::IDD, pParent)
{
	m_brush2.CreateSolidBrush(RGB(232,249,255));
    m_font2.CreatePointFont(120,_T("Tahoma"));
}

COperateDlg::~COperateDlg()
{
}

void COperateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_TIP, m_radioTip);
	DDX_Control(pDX, IDC_RADIO_BROADCAST, m_radioBroadcast);
	DDX_Control(pDX, IDC_RADIO_AD, m_radioAd);
	DDX_Control(pDX, IDC_BUTTON_CUSTOM_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_AD, m_btnAd);
	DDX_Control(pDX, IDC_BUTTON_TIP, m_btnTip);
}


BEGIN_MESSAGE_MAP(COperateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM_ADD, &COperateDlg::OnBnClickedButtonCustomAdd)
	
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_TIP, &COperateDlg::OnBnClickedRadioTip)
	ON_BN_CLICKED(IDC_RADIO_AD, &COperateDlg::OnBnClickedRadioAd)
END_MESSAGE_MAP()


// COperateDlg message handlers

void COperateDlg::OnBnClickedButtonCustomAdd()
{
	// TODO: Add your control notification handler code here
	CString strIni = _T("\\PTU.ini");
	strIni = theApp.GetAppPath() + strIni;
	CString strEn, strIran, strWrite;
	((CEdit *)GetDlgItem(IDC_EDIT_CUSTOM_EN))->GetWindowText(strEn);
	((CEdit *)GetDlgItem(IDC_EDIT_CUSTOM_IRAN))->GetWindowText(strIran);
	
	if (_T("") == strEn)
	{
		MessageBox(_T("Please make sure the English information not empty!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (_T("") == strIran)
	{
		MessageBox(_T("Please make sure the Farsi information not empty!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	strWrite = strEn;
	strWrite += _T(";");
	strWrite += strIran;

	int iResult = MessageBox(_T("Are you sure to add the item into configuraion file!"), _T("Prompt"), MB_OKCANCEL);
	if (IDCANCEL == iResult)
	{
		return;
	}
	else 
	{
		if (BST_CHECKED == m_radioTip.GetCheck())
		{
			//读配置文件
			CString szValue;
			GetPrivateProfileString(_T("ExtraTips"), _T("TipsCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
			int nTipsCount = _ttoi(szValue);

			//写配置文件
			CString szKey ;
			szKey.Format( _T("Tips%d"),nTipsCount);
			WritePrivateProfileString(_T("ExtraTips"), szKey, strWrite, strIni);
			strWrite.Format(_T("%d"),nTipsCount + 1);
			WritePrivateProfileString(_T("ExtraTips"), _T("TipsCount"), strWrite, strIni);
			//MessageBox(_T("The tip  has been written to Configuration file, In oder to operate the new Tip, Strongly Recommended you to exit and run the Application again!"), _T("Prompt"), MB_ICONWARNING);
			MessageBox(_T("Add tip successfully!"), _T("Prompt"), MB_OK);
		}
		else if(BST_CHECKED == m_radioAd.GetCheck())
		{
			//读配置文件
			CString szValue;
			GetPrivateProfileString(_T("AD"), _T("AdCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
			int nAdCount = _ttoi(szValue);

			//写配置文件
			CString szKey ;
			szKey.Format( _T("Ad%d"),nAdCount);
			WritePrivateProfileString(_T("AD"), szKey, strWrite, strIni);
			strWrite.Format(_T("%d"),nAdCount + 1);
			WritePrivateProfileString(_T("AD"), _T("AdCount"), strWrite, strIni);
			//MessageBox(_T("The advertisement has been written to Configuration file, In oder to operate the new Advertisement, Strongly Recommended you to exit and run the Application again!"), _T("Prompt"), MB_ICONWARNING);
			MessageBox(_T("Add advertisement successfully!"), _T("Prompt"), MB_OK);
		}
		else if(m_radioBroadcast.GetCheck()==1)
		{
			//读配置文件
			CString szValue;
			GetPrivateProfileString(_T("BROADCAST"), _T("BroadCastCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
			int nAdCount = _ttoi(szValue);

			//写配置文件
			CString szKey ;
			szKey.Format( _T("BroadCast%d"),nAdCount);
			WritePrivateProfileString(_T("BROADCAST"), szKey, strWrite, strIni);
			strWrite.Format(_T("%d"),nAdCount + 1);
			WritePrivateProfileString(_T("BROADCAST"), _T("BroadCastCount"), strWrite, strIni);
			//MessageBox(_T("The advertisement has been written to Configuration file, In oder to operate the new Advertisement, Strongly Recommended you to exit and run the Application again!"), _T("Prompt"), MB_ICONWARNING);
			MessageBox(_T("Add BroadCast successfully!"), _T("Prompt"), MB_OK);
			

		}
		else
		{
			MessageBox(_T("Advertisement or Tip? Please make a choice!"), _T("Prompt"), MB_ICONWARNING);
			return;
		}

		GetParent()->GetParent()->GetParent()->SendMessage(WM_ADD_MSG, NULL, NULL);
		//GetParent()->GetParent()->SendMessage(WM_ADD_MSG, NULL, NULL);
		GetParent()->SendMessage(WM_ADD_MSG, NULL, NULL);
	}	
	
}

BOOL COperateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitDlg();
	((CEdit *)GetDlgItem(IDC_EDIT_CUSTOM_EN))->SetFont(&m_font2);
	((CEdit *)GetDlgItem(IDC_EDIT_CUSTOM_IRAN))->SetFont(&m_font2);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COperateDlg::InitDlg()
{
	m_radioTip.SetCheck(BST_CHECKED);

	LoadMyImage(&m_btnAdd, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnAd, _T("//ImageSrc//Check_Normal.png"), _T("//ImageSrc//Check_Push.png"), _T(""), _T(""));
	LoadMyImage(&m_btnTip, _T("//ImageSrc//Check_Normal.png"), _T("//ImageSrc//Check_Push.png"), _T(""), _T(""));
}

BOOL COperateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message ==WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_OEM_COMMA) || (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void COperateDlg::OnPaint()
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

HBRUSH COperateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC||pWnd->GetDlgCtrlID()==IDC_RADIO_TIP||pWnd->GetDlgCtrlID()==IDC_RADIO_AD)
	{
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return m_brush2;
	}
	
	//if (nCtlColor == CTLCOLOR_DLG)     
	//{     
	//	pDC->SetBkMode(TRANSPARENT);     
	//	hbr = (HBRUSH)GetStockObject(NULL_BRUSH);   
	//}  

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void COperateDlg::OnBnClickedRadioTip()
{
	// TODO: 在此添加控件通知处理程序代码
}

void COperateDlg::OnBnClickedRadioAd()
{
	// TODO: 在此添加控件通知处理程序代码
}

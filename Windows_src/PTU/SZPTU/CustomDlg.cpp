// CustomDlg.cpp : implementation file
//
#include "stdafx.h"
#include "CustomDlg.h"
#include "SZPTU.h"

#define	MAX_CUSTOM_TAB	2
// CCustomDlg dialog

IMPLEMENT_DYNAMIC(CCustomDlg, CDialog)

CCustomDlg::CCustomDlg(CWnd* pParent =NULL)
	: CDialog(CCustomDlg::IDD, pParent)
{

}

CCustomDlg::~CCustomDlg()
{
}

void CCustomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TAB_CUSTOM, m_tabCustom);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CCustomDlg, CDialog)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, &CCustomDlg::OnTcnSelchangeTabCustom)
	ON_MESSAGE(WM_ADD_MSG , &CCustomDlg::OnAddMsg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCustomDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCustomDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CCustomDlg::OnBnClickedButtonClose)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CCustomDlg message handlers
void CCustomDlg::InitDlg()
{
	LoadMyImage(&m_btnClose, _T("//ImageSrc//Close_Normal.png"), _T("//ImageSrc//Close_Push.png"), _T("//ImageSrc//Close_Enter.png"), _T(""));
	LoadMyImage(&m_btnAdd, _T("//ImageSrc//Add_Normal.png"), _T("//ImageSrc//Add_Push.png"), _T("//ImageSrc//Add_Enter.png"), _T(""));
	LoadMyImage(&m_btnDelete, _T("//ImageSrc//Delete_Normal.png"), _T("//ImageSrc//Delete_Push.png"), _T("//ImageSrc//Delete_Enter.png"), _T(""));
}

void CCustomDlg::InitTab()
{
	CRect childRect;
	//m_tabCustom.GetClientRect(childRect);

	GetClientRect(childRect);
	childRect.top += 100;
	childRect.bottom -= 20;
	childRect.left += 15;
	childRect.right -= 15;

	//m_tabCustom.InsertItem(0, _T("Add"));
	//m_tabCustom.InsertItem(1, _T("Delete"));
	//m_tabCustom.InsertItem(2, _T("Parameter Setting"));

	//m_cOperateDlg.Create(IDD_OPERATEDLG,&m_tabCustom);
	//m_cOperateDlg.MoveWindow(childRect);
	////m_cConfigDlg.Create(IDD_CONFIGURATIONDLG,&m_tabCustom);
	////m_cConfigDlg.MoveWindow(childRect);
	//m_cDeleteDlg.Create(IDD_DELETEDLG, &m_tabCustom);
	//m_cDeleteDlg.MoveWindow(childRect);	
	
	m_cOperateDlg.Create(IDD_OPERATEDLG, this);
	m_cOperateDlg.MoveWindow(childRect);
	m_cDeleteDlg.Create(IDD_DELETEDLG, this);
	m_cDeleteDlg.MoveWindow(childRect);

	DoTab(0);	
}
void CCustomDlg::DoTab(UINT nTab)
{
	BOOL bTab[MAX_CUSTOM_TAB];
	for (int i = 0; i<MAX_CUSTOM_TAB; i++)
	{
		bTab[i] = FALSE;
	}
	if ((nTab > MAX_CUSTOM_TAB) || (nTab < 0))
	{
		nTab = 0;
	}
	bTab[nTab] = TRUE;

	SetDlgState(&m_cOperateDlg, bTab[0]);
	//SetDlgState(&m_cConfigDlg, bTab[1]);
	SetDlgState(&m_cDeleteDlg, bTab[1]);
}
void CCustomDlg::SetDlgState(CWnd *pWnd, BOOL bShow)
{
	if (bShow)
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		pWnd->ShowWindow(SW_HIDE);
	}
}
BOOL CCustomDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	InitDlg();
	InitTab();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//void CCustomDlg::OnTcnSelchangeTabCustom(NMHDR *pNMHDR, LRESULT *pResult)
//{
	// TODO: Add your control notification handler code here
	//int isel = m_tabCustom.GetCurSel();
	//if(isel>=0)
	//{
	//	DoTab(isel);
	//}
	//*pResult = 0;
//}

LRESULT CCustomDlg::OnAddMsg(WPARAM wParam, LPARAM lParam)
{
	m_cDeleteDlg.SendMessage(WM_ADD_MSG, NULL, NULL);
	return 0L;
}
void CCustomDlg::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	DoTab(0);
}

void CCustomDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	DoTab(1);
}

void CCustomDlg::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_CLOSE, NULL, NULL);
}

void CCustomDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CString strBk;
	strBk = theApp.GetAppPath() + _T("//ImageSrc//CustomizeBk.png");
	//CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	Image  myImage(strBk.GetBuffer());   //创建制定大小的BMP位图文件
	Graphics graphics(dc);

	graphics.DrawImage( 
		&myImage, 
		Rect( 0, 0, rect.Width(), rect.Height()), 
		3, 
		3, 
		myImage.GetWidth() - 8, 
		myImage.GetHeight() - 8, 
		UnitPixel, 
		NULL, 
		NULL, 
		NULL
		) ;
}

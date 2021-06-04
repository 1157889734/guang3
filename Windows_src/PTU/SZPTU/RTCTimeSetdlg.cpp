// RTCTimeSetdlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "RTCTimeSetdlg.h"
#include "MainFrm.h"

// CRTCTimeSetdlg 对话框

IMPLEMENT_DYNAMIC(CRTCTimeSetdlg, CDialog)

CRTCTimeSetdlg::CRTCTimeSetdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTCTimeSetdlg::IDD, pParent)
	, m_longtime(0)
	, m_shorttime(0)
{
	CTime time=CTime::GetCurrentTime();
	m_longtime=time;
	m_shorttime=time;

}

CRTCTimeSetdlg::~CRTCTimeSetdlg()
{
}

void CRTCTimeSetdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_longtime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_shorttime);
}


BEGIN_MESSAGE_MAP(CRTCTimeSetdlg, CDialog)
	ON_BN_CLICKED(IDC_NATIVETIMESET, &CRTCTimeSetdlg::OnBnClickedNativetimeset)
	ON_BN_CLICKED(IDC_INTIMESET, &CRTCTimeSetdlg::OnBnClickedIntimeset)
	ON_BN_CLICKED(IDC_QUERYTIME, &CRTCTimeSetdlg::OnBnClickedQuerytime)
END_MESSAGE_MAP()


// CRTCTimeSetdlg 消息处理程序

void CRTCTimeSetdlg::OnBnClickedNativetimeset()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"));
		return;
	}
	CTime time=CTime::GetCurrentTime();
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x07;
	buf[2]=0x06;
	buf[3]=time.GetSecond();
	buf[4]=time.GetMinute();
	buf[5]=time.GetHour();
	buf[6]=time.GetDay();
	buf[7]=time.GetMonth();
	buf[8]=time.GetYear()%100;
	buf[9]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,8);
	buf[10]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,11);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CRTCTimeSetdlg::OnBnClickedIntimeset()
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
	UpdateData(TRUE);

	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x07;
	buf[2]=0x06;
	buf[3]=m_shorttime.GetSecond();
	buf[4]=m_shorttime.GetMinute();
	buf[5]=m_shorttime.GetHour();
	buf[6]=m_longtime.GetDay();
	buf[7]=m_longtime.GetMonth();
	buf[8]=m_longtime.GetYear()%100;
	buf[9]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,8);
	buf[10]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,11);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CRTCTimeSetdlg::ShowMsg(CString str)
{
	MessageBox(str);
}

void CRTCTimeSetdlg::OnBnClickedQuerytime()
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
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[10];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x07;
	buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);

}

void CRTCTimeSetdlg::ShowRecTimeMsg(CString str)
{
	SetDlgItemText(IDC_TIMESHOW,str);
}

BOOL CRTCTimeSetdlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

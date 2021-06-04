// PortLinkDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SZPTU.h"
#include "PortLinkDlg.h"
#include "MainFrm.h"

// CPortLinkDlg �Ի���

IMPLEMENT_DYNAMIC(CPortLinkDlg, CDialog)

CPortLinkDlg::CPortLinkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortLinkDlg::IDD, pParent)
{

}

CPortLinkDlg::~CPortLinkDlg()
{
}

void CPortLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CTCUSB, m_ctcbtn);
	DDX_Control(pDX, IDC_ATCUSB, m_atcbtn);
	DDX_Control(pDX, IDC_PCTCOM, m_combtn);
	DDX_Control(pDX, IDC_ATCRESET, m_atcrestbtn);
}


BEGIN_MESSAGE_MAP(CPortLinkDlg, CDialog)
	ON_BN_CLICKED(IDC_CTCUSB, &CPortLinkDlg::OnBnClickedCtcusb)
	ON_BN_CLICKED(IDC_ATCUSB, &CPortLinkDlg::OnBnClickedAtcusb)
	ON_BN_CLICKED(IDC_PCTCOM, &CPortLinkDlg::OnBnClickedPctcom)
	ON_BN_CLICKED(IDC_ATCRESET, &CPortLinkDlg::OnBnClickedAtcreset)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPortLinkDlg ��Ϣ�������

void CPortLinkDlg::OnBnClickedCtcusb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_CTCUSB,str);
	if(str.Compare(_T("����"))==0)
	{
		bool bl=((CMainFrame *)AfxGetMainWnd())->OpenCTCUsb();
		if(bl==false)
		{
			MessageBox(_T("�����������usb��ʧ��"),_T("Prompt"),MB_ICONWARNING);
		}
		else
		{
			SetDlgItemText(IDC_CTCUSB,_T("�Ͽ�"));
			((CMainFrame *)AfxGetMainWnd())->SetTimer(WM_TIME_CHECKCTCUSB,1000,NULL);
		}
	}
	else
	{
		((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->CloseDevice();
		SetDlgItemText(IDC_CTCUSB,_T("����"));
		((CMainFrame *)AfxGetMainWnd())->KillTimer(WM_TIME_CHECKCTCUSB);
		delete ((CMainFrame *)AfxGetMainWnd())->m_ptctcusb;		//added by amy 2012-8-15
		((CMainFrame *)AfxGetMainWnd())->m_ptctcusb = NULL;
		((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen = FALSE;		//added by amy 2012-8-15
	}
	///////////////////////////////////////////////////////////
	
}

void CPortLinkDlg::OnBnClickedAtcusb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_ATCUSB,str);
	if(str.Compare(_T("����"))==0)
	{
		bool bl=((CMainFrame *)AfxGetMainWnd())->OpenATCUsb();
		if(bl==false)
		{
			MessageBox(_T("��ATC������usb��ʧ��"),_T("Prompt"),MB_ICONWARNING);
		}
		else
		{
			SetDlgItemText(IDC_ATCUSB,_T("�Ͽ�"));
			((CMainFrame *)AfxGetMainWnd())->SetTimer(WM_TIME_CHECKATCUSB,1000,NULL);
		}
	}
	else
	{
		((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->CloseDevice();
		SetDlgItemText(IDC_ATCUSB,_T("����"));
		delete ((CMainFrame *)AfxGetMainWnd())->m_ptatcusb;		//added by amy 2012-8-15
		((CMainFrame *)AfxGetMainWnd())->m_ptatcusb = NULL;
		((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen = FALSE;		//added by amy 2012-8-15
		((CMainFrame *)AfxGetMainWnd())->KillTimer(WM_TIME_CHECKATCUSB);
	}
	
}

void CPortLinkDlg::OnBnClickedPctcom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strbtn;
	GetDlgItemText(IDC_PCTCOM,strbtn);
	if(strbtn.Compare(_T("����"))==0)
	{
		CString str;
		GetDlgItemText(IDC_COMCOMBOX,str);
		if(str.IsEmpty())
		{
			MessageBox(_T("��ѡ�񴮿�"),_T("Prompt"),MB_ICONWARNING);
			return;
		}
		int com=str.GetAt(3);
		com=com-'0';
		bool bl=((CMainFrame *)AfxGetMainWnd())->OpenComm(com);
		if(bl==false)
		{
			MessageBox(_T("��com��ʧ��"),_T("Prompt"),MB_ICONWARNING);
		}
		else
		{
			SetDlgItemText(IDC_PCTCOM,_T("�Ͽ�"));

		}

	}
	else
	{
		((CMainFrame *)AfxGetMainWnd())->m_com.ClosePort();
		SetDlgItemText(IDC_PCTCOM,_T("����"));

	}
	

}
void CPortLinkDlg::InitComPort()
{
	HKEY hKEY;
	DWORD dIndex = 0; 
	TCHAR strKey[MAX_PATH] = {0};
	TCHAR strValue[MAX_PATH] = {0};
	DWORD dwLen = MAX_PATH;
	DWORD type = REG_SZ;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), 0, KEY_READ, &hKEY) == ERROR_SUCCESS)
	{
		while (RegEnumValue(hKEY, dIndex, strKey, &dwLen, NULL, NULL, (LPBYTE)strValue, &dwLen) != ERROR_NO_MORE_ITEMS)
		{

			((CComboBox *)GetDlgItem(IDC_COMCOMBOX))->AddString(strValue);
			dIndex++;
			dwLen = MAX_PATH;
		}
		RegCloseKey(hKEY);
		((CComboBox *)GetDlgItem(IDC_COMCOMBOX))->SetCurSel(0);
	}
}
BOOL CPortLinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadMyImage(&m_ctcbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_atcbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_combtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_atcrestbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPortLinkDlg::OnBnClickedAtcreset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("��������ATCģ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[10];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x0e;
	buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CPortLinkDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CComboBox *)GetDlgItem(IDC_COMCOMBOX))->ResetContent();
    InitComPort();
	// TODO: �ڴ˴������Ϣ����������
}

BOOL CPortLinkDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	/*
	CString str;
	str=_T("Image\\���ӶԻ���.bmp");
	HBITMAP   hbitmap=(HBITMAP)::LoadImage(AfxGetInstanceHandle(), str,   IMAGE_BITMAP,0,0,   LR_CREATEDIBSECTION|LR_LOADFROMFILE); 
	if(hbitmap==NULL)   
	{
		MessageBox(_T("ͼƬ�Ҳ���"));
		return CDialog::OnEraseBkgnd(pDC);
	}
	CBitmap   m_bitmap; 
	m_bitmap.Attach(hbitmap);
	BITMAP   bmpstruct;
	m_bitmap.GetBitmap(&bmpstruct); 
	long   nWidth=bmpstruct.bmWidth; 
	long   nHeight=bmpstruct.bmHeight;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(&m_bitmap);  
	pDC->BitBlt(0,0,nWidth,nHeight,&memDC,0,0,SRCCOPY);
	return TRUE;*/
	
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CPortLinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	/*
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_GROUP)
	{
		pDC->SetTextColor(RGB(255,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);

	}*/
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		//pDC->SelectObject(&m_font);
		//pDC->SetTextColor(RGB(0,100,130)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
		//return m_SliderBrush;

	}
	// TODO:  �ڴ˸��� DC ���κ�����
    
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

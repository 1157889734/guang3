// SZPTU.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SZPTU.h"
#include "MainFrm.h"

#include "SZPTUDoc.h"
#include "SZPTUView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSZPTUApp

BEGIN_MESSAGE_MAP(CSZPTUApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSZPTUApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSZPTUApp ����

CSZPTUApp::CSZPTUApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSZPTUApp ����

CSZPTUApp theApp;
void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown, CString strEnter, CString strDisable);
void LoadMyImage(CMyButton *pMyBtn, CString strNormal, CString strDown , CString strEnter, CString strDisable)
{
	CString strTemp1;
	CString strTemp2;
	CString strTemp3;
	CString strTemp4;

	strTemp1 = theApp.GetAppPath() + strNormal;
	strTemp2 = theApp.GetAppPath() + strDown;
	strTemp3 = theApp.GetAppPath() + strEnter;
	strTemp4 = theApp.GetAppPath() + strDisable;

	pMyBtn->GetButtonImage(strTemp1.GetBuffer(), strTemp2.GetBuffer(), strTemp3.GetBuffer(), strTemp4.GetBuffer());

	strTemp1.ReleaseBuffer();
	strTemp2.ReleaseBuffer(); 
	strTemp3.ReleaseBuffer();
	strTemp4.ReleaseBuffer();
}
CString CSZPTUApp::GetAppPath()
{
	CString strAppPath;
	::GetModuleFileName(NULL, strAppPath.GetBuffer(256), 256);
	strAppPath.ReleaseBuffer();
	int nPos = strAppPath.ReverseFind('\\');
	strAppPath = strAppPath.Left(nPos);
	SetCurrentDirectory(strAppPath);

	return strAppPath;
}

// CSZPTUApp ��ʼ��

BOOL CSZPTUApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	AfxInitRichEdit2();
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSZPTUDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CSZPTUView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	int iWidth = GetSystemMetrics(SM_CXSCREEN);  
	int iHeight = GetSystemMetrics(SM_CYSCREEN); 
	m_pMainWnd->SetWindowText(_T("����������PTU���"));
	
	m_pMainWnd->CenterWindow();
    m_pMainWnd->MoveWindow((iWidth-PRO_X_SIZE)/2,(iHeight-PRO_Y_SIZE)/2,PRO_X_SIZE,PRO_Y_SIZE);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CSZPTUApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSZPTUApp ��Ϣ�������


int CSZPTUApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
    GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}

BYTE CSZPTUApp::GetCheckSum( BYTE *pData,int iLen )
{
	WORD wSum = 0x00;
	for (int i=0;i<iLen;i++)
	{
		wSum += *(pData+i);
	}
	wSum = 0x55 - (0x00FF & wSum);
	return (BYTE)wSum;
}
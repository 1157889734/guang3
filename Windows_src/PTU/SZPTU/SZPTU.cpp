// SZPTU.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSZPTUApp 构造

CSZPTUApp::CSZPTUApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSZPTUApp 对象

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

// CSZPTUApp 初始化

BOOL CSZPTUApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	AfxInitRichEdit2();
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSZPTUDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CSZPTUView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	int iWidth = GetSystemMetrics(SM_CXSCREEN);  
	int iHeight = GetSystemMetrics(SM_CYSCREEN); 
	m_pMainWnd->SetWindowText(_T("广州三号线PTU软件"));
	
	m_pMainWnd->CenterWindow();
    m_pMainWnd->MoveWindow((iWidth-PRO_X_SIZE)/2,(iHeight-PRO_Y_SIZE)/2,PRO_X_SIZE,PRO_Y_SIZE);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CSZPTUApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSZPTUApp 消息处理程序


int CSZPTUApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
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
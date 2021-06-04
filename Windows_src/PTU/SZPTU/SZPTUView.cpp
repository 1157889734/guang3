// SZPTUView.cpp : CSZPTUView 类的实现
//

#include "stdafx.h"
#include "SZPTU.h"

#include "SZPTUDoc.h"
#include "SZPTUView.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSZPTUView

IMPLEMENT_DYNCREATE(CSZPTUView, CView)

BEGIN_MESSAGE_MAP(CSZPTUView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSZPTUView 构造/析构

CSZPTUView::CSZPTUView()
{
	// TODO: 在此处添加构造代码
	m_szbkBmpPath = _T("//Image//主背景.bmp");
}

CSZPTUView::~CSZPTUView()
{
}

BOOL CSZPTUView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSZPTUView 绘制

void CSZPTUView::OnDraw(CDC* pDC)
{
	CSZPTUDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
    /////////////////////////////////////////////////////////
   
	CString str;
	str=((CMainFrame *)GetParent())->GetAppPath()+m_szbkBmpPath;
	HBITMAP   hbitmap=(HBITMAP)::LoadImage(AfxGetInstanceHandle(), str,   IMAGE_BITMAP,0,0,   LR_CREATEDIBSECTION|LR_LOADFROMFILE); 
	if(hbitmap==NULL)   
	{
		MessageBox(_T("图片找不到"));
		return;
		//return CDialog::OnEraseBkgnd(pDC);
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
	//return TRUE;
	// TODO: 在此处为本机数据添加绘制代码
}


// CSZPTUView 打印

BOOL CSZPTUView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CSZPTUView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CSZPTUView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清除过程
}


// CSZPTUView 诊断

#ifdef _DEBUG
void CSZPTUView::AssertValid() const
{
	CView::AssertValid();
}

void CSZPTUView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSZPTUDoc* CSZPTUView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSZPTUDoc)));
	return (CSZPTUDoc*)m_pDocument;
}
#endif //_DEBUG


// CSZPTUView 消息处理程序

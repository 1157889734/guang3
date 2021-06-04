// SZPTUView.cpp : CSZPTUView ���ʵ��
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSZPTUView ����/����

CSZPTUView::CSZPTUView()
{
	// TODO: �ڴ˴���ӹ������
	m_szbkBmpPath = _T("//Image//������.bmp");
}

CSZPTUView::~CSZPTUView()
{
}

BOOL CSZPTUView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CSZPTUView ����

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
		MessageBox(_T("ͼƬ�Ҳ���"));
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
	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CSZPTUView ��ӡ

BOOL CSZPTUView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CSZPTUView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CSZPTUView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CSZPTUView ���

#ifdef _DEBUG
void CSZPTUView::AssertValid() const
{
	CView::AssertValid();
}

void CSZPTUView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSZPTUDoc* CSZPTUView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSZPTUDoc)));
	return (CSZPTUDoc*)m_pDocument;
}
#endif //_DEBUG


// CSZPTUView ��Ϣ�������

// SZPTUDoc.cpp : CSZPTUDoc ���ʵ��
//

#include "stdafx.h"
#include "SZPTU.h"

#include "SZPTUDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSZPTUDoc

IMPLEMENT_DYNCREATE(CSZPTUDoc, CDocument)

BEGIN_MESSAGE_MAP(CSZPTUDoc, CDocument)
END_MESSAGE_MAP()


// CSZPTUDoc ����/����

CSZPTUDoc::CSZPTUDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CSZPTUDoc::~CSZPTUDoc()
{
}

BOOL CSZPTUDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CSZPTUDoc ���л�

void CSZPTUDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CSZPTUDoc ���

#ifdef _DEBUG
void CSZPTUDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSZPTUDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSZPTUDoc ����

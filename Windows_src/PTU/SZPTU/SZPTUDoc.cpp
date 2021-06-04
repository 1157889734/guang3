// SZPTUDoc.cpp : CSZPTUDoc 类的实现
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


// CSZPTUDoc 构造/析构

CSZPTUDoc::CSZPTUDoc()
{
	// TODO: 在此添加一次性构造代码

}

CSZPTUDoc::~CSZPTUDoc()
{
}

BOOL CSZPTUDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CSZPTUDoc 序列化

void CSZPTUDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CSZPTUDoc 诊断

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


// CSZPTUDoc 命令

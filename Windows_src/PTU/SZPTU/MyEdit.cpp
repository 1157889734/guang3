// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyEdit.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CMyEdit message handlers

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( ((nChar>=0x30)&&(nChar <=0x39))||(nChar == 0x08)) 
	{ 
		CEdit::OnChar(nChar, nRepCnt, nFlags); 
	} 
}



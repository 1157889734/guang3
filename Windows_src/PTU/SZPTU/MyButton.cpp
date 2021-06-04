#include "StdAfx.h"
#include "MyButton.h"


CMyButton::CMyButton( void )
{
	m_bmpDown = NULL;
	m_bmpUp = NULL;
	m_bmpNormal = NULL;
	m_bInit = FALSE;
	m_bIsPressed = FALSE;
	m_bIsEntered = FALSE;
}

CMyButton::~CMyButton( void )
{
	//if (NULL != m_bmpDown)
	//{
	//	delete m_bmpDown;
	//	m_bmpDown = NULL;
	//}
	//if (NULL != m_bmpUp)
	//{
	//	delete m_bmpUp;
	//	m_bmpUp = NULL;
	//}
	//if (NULL != m_bmpNormal)
	//{
	//	delete m_bmpNormal;
	//	m_bmpNormal = NULL;
	//}
	//if (NULL != m_bmpDisable)
	//{
	//	delete m_bmpDisable;
	//	m_bmpDisable = NULL;
	//}
}
BEGIN_MESSAGE_MAP( CMyButton, CButton )
	ON_WM_ERASEBKGND( )
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover) 
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	
END_MESSAGE_MAP( )

void CMyButton::GetButtonImage( WCHAR *wNormalPath 
							   , WCHAR *wDownPath /*= NULL*/
							   , WCHAR *wUpPath/*= NULL*/
							   , WCHAR *wDisablePath/*= NULL*/)
{
	m_bmpUp = new Bitmap(wUpPath);
	m_bmpDown = new Bitmap(wDownPath);
	m_bmpNormal = new Bitmap(wNormalPath);
	m_bmpDisable = new Bitmap(wDisablePath);
}
UINT GetTextBounds(CDC* pDC,const CString szText)
{
	CFont font;  

	//Requested font height in tenths of a point.

	//For instance, pass 120 to request a 12-point font.)

	//由上面解释可知，120这个参数是取其1/10为作为字体大小，

	//不过实际中计算中文时，发现比例为11:1，这个有待进一步研究

	font.CreatePointFont(12, _TEXT("行楷"));

	pDC->SelectObject(font);  
	CSize size= pDC->GetTextExtent(szText);

	/// 返回文本的宽高
	return size.cy;
}

void CMyButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	
	CFont*   pListBoxFont   =   GetFont(); 

	CDC *pmyDC = this->GetDC();
	pmyDC->SelectObject(pListBoxFont);  

	CRect        rect ;
	CString      strName ;
	StringFormat sf ;
	
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC ) ;
	Graphics g(pDC->m_hDC);

	this->GetClientRect(&rect);
	this->GetWindowText(strName);
	sf.SetAlignment( StringAlignmentCenter ) ;
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);
	//BOOL bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	//BOOL bDrawFocusRect = !(lpDrawItemStruct->itemState & ODS_NOFOCUSRECT);
	//BOOL bIsPressed =	(lpDrawItemStruct->itemState & ODS_SELECTED);
	//g.FillRectangle(&SolidBrush( Color( 255, 0, 255, 0 )),0,0,rect.Width(), rect.Height());
	
    int fonthigh=GetTextBounds(pDC,strName);
	int realwidth=(rect.Height()-fonthigh)/2;
	//判断按钮是否被禁用
	if (bIsDisabled)
	{
		g.DrawImage( 
			m_bmpDisable, 
			Rect( 0, 0, rect.Width(), rect.Height()), 
			0, 
			0, 
			m_bmpDown->GetWidth(), 
			m_bmpDown->GetHeight(), 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;
		CSize &cs = pmyDC->GetTextExtent( strName );
		int beginhigh=(rect.bottom-rect.top-cs.cy)/2;
		g.DrawString(strName, 
			strName.GetLength( ), 
			&Gdiplus::Font(L"行楷", 12, 0, UnitPixel, NULL), 
			RectF(0, beginhigh-1, (REAL)rect.right, (REAL)rect.bottom), 
			&sf, 
			&SolidBrush(Color(255, 255, 255, 255))) ;
		return;
	}
	if (m_bIsPressed && m_bIsEntered)
	{
		g.DrawImage( 
			m_bmpDown, 
			Rect( 0, 0, rect.Width(), rect.Height()), 
			0, 
			0, 
			m_bmpDown->GetWidth(), 
			m_bmpDown->GetHeight(), 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;
	}
	else if (!m_bIsPressed && m_bIsEntered)
	{
		g.DrawImage( 
			m_bmpUp, 
			Rect( 0, 0, rect.Width(), rect.Height()), 
			0, 
			0, 
			m_bmpUp->GetWidth(), 
			m_bmpUp->GetHeight(), 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;
	}
	else
	{
		g.DrawImage( 
			m_bmpNormal, 
			Rect( 0, 0, rect.Width(), rect.Height()), 
			0, 
			0, 
			m_bmpNormal->GetWidth(), 
			m_bmpNormal->GetHeight(), 
			UnitPixel, 
			NULL, 
			NULL, 
			NULL
			) ;
	}
	CSize &cs = pmyDC->GetTextExtent( strName );
	int beginhigh=(rect.bottom-rect.top-cs.cy)/2;
	g.DrawString(strName, 
		strName.GetLength( ), 
		&Gdiplus::Font(L"行楷", 12, 0, UnitPixel, NULL), 
		RectF(0, beginhigh-1, (REAL)rect.right, (REAL)rect.bottom), 
		&sf, 
		&SolidBrush(Color(255, 255,255, 255))) ;

}


BOOL CMyButton::OnEraseBkgnd( CDC* pDC )
{
	return FALSE ;
}

void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bIsPressed = TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CMyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bIsPressed = FALSE;
	CButton::OnLButtonUp(nFlags, point);
}

void CMyButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	CPoint pt;
	GetWindowRect(&rect);
	::GetCursorPos(&pt);

	if( m_bIsEntered == FALSE )                        //鼠标是否在按钮上
	{
		TRACKMOUSEEVENT tme ;
		ZeroMemory( &tme, sizeof( TRACKMOUSEEVENT ) ) ;
		tme.cbSize        = sizeof( tme ) ;
		tme.hwndTrack     = m_hWnd ;
		tme.dwFlags       = TME_LEAVE|TME_HOVER ;
		tme.dwHoverTime   = 1 ;
		m_bIsEntered = _TrackMouseEvent( &tme ) ;
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CMyButton::OnMouseHover( WPARAM wParam, LPARAM lParam )
{
	m_bIsEntered = TRUE ;

	this->Invalidate( ) ;
	return 0L;
}


LRESULT CMyButton::OnMouseLeave(WPARAM wParam, LPARAM lParam )
{
	m_bIsEntered = FALSE ;

	this->Invalidate( ) ;
	return 0L;
}

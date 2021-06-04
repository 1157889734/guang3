#pragma once
#include "afxwin.h"

class CMyButton :
	public CButton
{
public:
	CMyButton( void ) ;
	~CMyButton( void ) ;
	DECLARE_MESSAGE_MAP( )

public:
	Bitmap	*m_bmpUp;
	Bitmap	*m_bmpDown;
	Bitmap	*m_bmpNormal;
	Bitmap	*m_bmpDisable;
	BOOL	m_bInit;
	BOOL	m_bIsPressed;
	BOOL	m_bIsEntered;

public:
	//从主窗口得到图片的路径
	//void GetButtonImage( WCHAR *wchPath ) ;
	void GetButtonImage(WCHAR *wNormalPath 
		, WCHAR *wDownPath = NULL
		, WCHAR *wUpPath = NULL
		, WCHAR *wDisablePath = NULL
		);
	//void GetButtonImage(CString strNormal, CString strDown, CString strUp);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	afx_msg BOOL OnEraseBkgnd( CDC* pDC ) ;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam ) ;
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam ) ;
} ;


// SZPTU.h : SZPTU Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CSZPTUApp:
// �йش����ʵ�֣������ SZPTU.cpp
//

class CSZPTUApp : public CWinApp
{
public:
	CSZPTUApp();
    CString GetAppPath();
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	BYTE GetCheckSum( BYTE *pData,int iLen );
};

extern CSZPTUApp theApp;
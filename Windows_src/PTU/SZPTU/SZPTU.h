// SZPTU.h : SZPTU 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CSZPTUApp:
// 有关此类的实现，请参阅 SZPTU.cpp
//

class CSZPTUApp : public CWinApp
{
public:
	CSZPTUApp();
    CString GetAppPath();
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	BYTE GetCheckSum( BYTE *pData,int iLen );
};

extern CSZPTUApp theApp;
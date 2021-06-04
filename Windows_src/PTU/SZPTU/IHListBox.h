#pragma once


// CIHListBox

class CIHListBox : public CListBox
{
	DECLARE_DYNAMIC(CIHListBox)

public:
	CIHListBox();
	virtual ~CIHListBox();
	// 覆盖该方法以便添加水平滚动条
	int AddString( LPCTSTR lpszItem );
	int InsertString( int nIndex, LPCTSTR lpszItem );

	// 计算水平滚动条宽度
	void RefushHorizontalScrollBar( void );


protected:
	DECLARE_MESSAGE_MAP()
};



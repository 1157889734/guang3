#pragma once


// CIHListBox

class CIHListBox : public CListBox
{
	DECLARE_DYNAMIC(CIHListBox)

public:
	CIHListBox();
	virtual ~CIHListBox();
	// ���Ǹ÷����Ա����ˮƽ������
	int AddString( LPCTSTR lpszItem );
	int InsertString( int nIndex, LPCTSTR lpszItem );

	// ����ˮƽ���������
	void RefushHorizontalScrollBar( void );


protected:
	DECLARE_MESSAGE_MAP()
};



// SZPTUView.h : CSZPTUView ��Ľӿ�
//


#pragma once


class CSZPTUView : public CView
{
protected: // �������л�����
	CSZPTUView();
	DECLARE_DYNCREATE(CSZPTUView)

// ����
public:
	CSZPTUDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CSZPTUView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CString m_szbkBmpPath;

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // SZPTUView.cpp �еĵ��԰汾
inline CSZPTUDoc* CSZPTUView::GetDocument() const
   { return reinterpret_cast<CSZPTUDoc*>(m_pDocument); }
#endif


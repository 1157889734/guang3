// SZPTUDoc.h : CSZPTUDoc ��Ľӿ�
//


#pragma once


class CSZPTUDoc : public CDocument
{
protected: // �������л�����
	CSZPTUDoc();
	DECLARE_DYNCREATE(CSZPTUDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CSZPTUDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};



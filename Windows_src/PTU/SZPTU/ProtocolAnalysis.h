#pragma once

class CProtocolAnalysis
{
public:
	CProtocolAnalysis(void);
	~CProtocolAnalysis(void);

	virtual int GetElementCount(void ) = NULL;
	virtual void GetElementData(int nIndex, void *pStorage) = NULL;
	virtual CString GetElementName(int nIndex) = NULL;
	virtual CString GetElementDataDisplay(int nIndex) = NULL;

	void SetProtocolStream(const BYTE *pData, const int nLen);

protected:
	int m_nDataLen;
	BYTE *m_pData;
};
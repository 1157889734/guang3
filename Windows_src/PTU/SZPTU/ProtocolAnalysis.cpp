#include "StdAfx.h"
#include "ProtocolAnalysis.h"

CProtocolAnalysis::CProtocolAnalysis(void)
{
	m_pData = NULL;
	m_nDataLen = 0;
}

CProtocolAnalysis::~CProtocolAnalysis(void)
{
	if (NULL != m_pData)
	{
		free(m_pData);
		m_pData = NULL;
	}
}

void CProtocolAnalysis::SetProtocolStream(const BYTE *pData, const int nLen)
{
	if (NULL == pData || 0 == nLen)
		return;

	if (NULL == m_pData)
	{
		m_pData = (BYTE *)malloc(nLen);
		memcpy_s(m_pData, nLen, pData, nLen);
		m_nDataLen = nLen;
	}
	else if (m_nDataLen < nLen)
	{
		m_pData = (BYTE *)realloc(m_pData, nLen);
		memcpy_s(m_pData, nLen, pData, nLen);
		m_nDataLen = nLen;
	}
	else {
		memcpy_s(m_pData, nLen, pData, nLen);
		m_nDataLen = nLen;
	}
}

#include "StdAfx.h"
#include "PisProtocol.h"





const LPCTSTR ARRAY_STR_KEY_WORDS[] = {
	_T("×´Ì¬"),
};



CPisProtocol::CPisProtocol(void)
{
}

CPisProtocol::~CPisProtocol(void)
{
}

int CPisProtocol::GetElementCount(void )
{
	return PISDATA_SENTINEL; // ·µ»Ø±ß½ç
}

void CPisProtocol::GetElementData(int nIndex, void *pStorage)
{
	if (nIndex < 0 || nIndex > PISDATA_SENTINEL)
		return;

	SaloonDevStat *p = (SaloonDevStat *)pStorage;
	p->bVehicleNotDetected = 0;
	
	p->bDRMCfault = 0;

	switch(nIndex)
	{
	case 0:
		{
			p->bPECUfault_1 = (m_pData[23] >> 3) & 0x01;
			p->bPECUfault_2 = (m_pData[23] >> 4) & 0x01;;
			p->bPECUfault_3 = (m_pData[23] >> 5) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	case 1:
		{
			p->bPECUfault_1 = (m_pData[23] >> 7) & 0x01;
			p->bPECUfault_2 = (m_pData[24] >> 0) & 0x01;;
			p->bPECUfault_3 = (m_pData[24] >> 1) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	case 2:
		{
			p->bPECUfault_1 = (m_pData[24] >> 3) & 0x01;
			p->bPECUfault_2 = (m_pData[24] >> 4) & 0x01;;
			p->bPECUfault_3 = (m_pData[24] >> 5) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	case 3:
		{
			p->bPECUfault_1 = (m_pData[25] >> 1) & 0x01;
			p->bPECUfault_2 = (m_pData[25] >> 2) & 0x01;;
			p->bPECUfault_3 = (m_pData[25] >> 3) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	case 4:
		{
			p->bPECUfault_1 = (m_pData[25] >> 5) & 0x01;
			p->bPECUfault_2 = (m_pData[25] >> 6) & 0x01;;
			p->bPECUfault_3 = (m_pData[25] >> 7) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	case 5:
		{
			p->bPECUfault_1 = (m_pData[26] >> 3) & 0x01;
			p->bPECUfault_2 = (m_pData[26] >> 4) & 0x01;;
			p->bPECUfault_3 = (m_pData[26] >> 5) & 0x01;;

			p->bWDSfault_1 = 0;
			p->bWDSfault_2 = 0;
			p->bWDSfault_3 = 0;
			p->bWDSfault_4 = 0;
			break;
		}
	default:
		{
			break;
		}
	}
	
}

CString CPisProtocol::GetElementName(int nIndex)
{
	if (nIndex < 0 || nIndex >= PISDATA_SENTINEL)
		return NULL;

	return ARRAY_STR_KEY_WORDS[nIndex];
}

CString CPisProtocol::GetElementDataDisplay(int nIndex)
{
	CString strReturnVal = _T("");

	if (nIndex < 0 || nIndex > PISDATA_SENTINEL)
		return strReturnVal;

	SaloonDevStat stat;
	GetElementData(nIndex, &stat);

	strReturnVal.Format(_T("Vehicle[%d], DRMC[%d], WDS[%d%d%d%d], PECU[%d%d%d]"),
		stat.bVehicleNotDetected,
		stat.bDRMCfault,
		stat.bWDSfault_1, stat.bWDSfault_2, stat.bWDSfault_3, stat.bWDSfault_4,
		stat.bPECUfault_1, stat.bPECUfault_2, stat.bPECUfault_3);

	return strReturnVal;
}
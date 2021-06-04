#include "StdAfx.h"
#include "TmsProtocol.h"



const LPCTSTR ARRAY_STR_KEY_WORDS[] = {
	_T("ATO模式"),
	//_T("始发站"),
	_T("终点站"),
	_T("下一站"),
	_T("自动左侧门开"),
	_T("自动右侧门开"),
	_T("手动左侧门开"),
	_T("手动右侧门开")
};

const INT INT_STATION_INDEX_BASE = 101/*111*/;

CTmsProtocol::CTmsProtocol(void)
{
}

CTmsProtocol::~CTmsProtocol(void)
{
}

int CTmsProtocol::GetElementCount(void )
{
	return TCMSDATA_SENTINEL; // 返回边界
}

int nDestStation=0;
void CTmsProtocol::GetElementData(int nIndex, void *pStorage)
{
	if (m_pData[51]==6 || m_pData[51]==5)//des
	{
		if (m_pData[53] != nDestStation)
		{
			nDestStation = ((int)m_pData[53]);
			//TRACE("nDestStation=%d, m_pData[53]=%x\n", nDestStation, m_pData[53]);
		}
		
	}
	else if(m_pData[51]==4)//next
	{
		m_pData[53];
	}

	switch (nIndex)
	{
	case TCMSDATA_INFO_ON_FLAG:
		{
			int *pInt = (int *)pStorage;
			(*pInt) = m_pData[50] & 1;//m_pData[45] & 1;
		}
		break;
// 	case TCMSDATA_STATION_START:
// 		{
// 			int *pInt = (int *)pStorage;
// 			*pInt = m_pData[/*36*/70];
// 			//TRACE("%x %x %x %x\n", m_pData[70], m_pData[71], m_pData[72], m_pData[73]);
// 		}
// 		break;
	case TCMSDATA_STATION_DEST:
		{
			int *pInt = (int *)pStorage;
			*pInt = nDestStation;//m_pData[/*38*/53];
			//TRACE("TCMSDATA_STATION_DEST %d \n", *pInt);
		}
		break;
	case TCMSDATA_NEXT_STATION:
		{			
			if(m_pData[51]==4)
			{
				int *pInt = (int *)pStorage;
				*pInt = (int)m_pData[/*40*/53];
			}
		}
		break;
	case TCMSDATA_LEFT_DOOR_RELEASE:
		{
			int *pInt = (int *)pStorage;
			*pInt = (m_pData[50] >> 2) & 1;//(m_pData[45] >> 2) & 1;
		}
		break;
	case TCMSDATA_RIGHT_DOOR_RELEASE:
		{
			int *pInt = (int *)pStorage;
			*pInt = (m_pData[50] >> 3) & 1;//(m_pData[45] >> 1) & 1;
		}
		break;
	case TCMSDATA_LEFT_HANDLE_DOOR_RELEASE:
		{
			int *pInt = (int *)pStorage;
			*pInt = (m_pData[55] >> 4) & 1;
		}
		break;
	case TCMSDATA_RIGHT_HANDLE_DOOR_RELEASE:
		{
			int *pInt = (int *)pStorage;
			*pInt = (m_pData[55] >> 5) & 1;
		}
		break;
	default:
		break;
	}
}

CString CTmsProtocol::GetElementName(int nIndex)
{
	if (nIndex < 0 || nIndex >= TCMSDATA_SENTINEL)
		return NULL;

	return ARRAY_STR_KEY_WORDS[nIndex];
}

CString CTmsProtocol::GetElementDataDisplay(int nIndex)
{
	int nVal = 0;
	CString strReturnVal = _T("");
	GetElementData(nIndex, &nVal);

	switch (nIndex)
	{
	//case TCMSDATA_STATION_START:
	case TCMSDATA_NEXT_STATION:
		{
			int nStationIndex = nVal;//(nVal - INT_STATION_INDEX_BASE) / 3;
			if (nStationIndex > 0 && nStationIndex < INT_STATION_NUM)
			{				
				strReturnVal = ARRAY_STR_STATION_NAME[nStationIndex-1];
// 				USES_CONVERSION;
// 				TRACE(("nStationIndex = %d, strReturnVal=%s \n"), nStationIndex, W2A(strReturnVal));
			}
		}
		break;
	case TCMSDATA_STATION_DEST:
		{
			int iDestStationIndex = nVal;
			if (iDestStationIndex >= 0 && iDestStationIndex <= (INT_STATION_NUM - 1))
			{
				strReturnVal = ARRAY_STR_STATION_NAME[iDestStationIndex];
// 				USES_CONVERSION;
// 				TRACE(("nStationIndex = %d, strReturnVal=%s \n"), iDestStationIndex, W2A(strReturnVal));
			}
			break;
		}
	case TCMSDATA_LEFT_DOOR_RELEASE:
	case TCMSDATA_RIGHT_DOOR_RELEASE:
	case TCMSDATA_INFO_ON_FLAG:
	case TCMSDATA_LEFT_HANDLE_DOOR_RELEASE:
	case TCMSDATA_RIGHT_HANDLE_DOOR_RELEASE:
		{
			if (1 == nVal)
				strReturnVal = _T("是");
			else
				strReturnVal = _T("否");
		}
		break;
	default:
		break;
	}

	return strReturnVal;
}
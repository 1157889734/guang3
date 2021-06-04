
#include "stdafx.h"
#include "StationCfg.h"
#include "MainFrm.h"

void CStationCfg::OnDeleteListBoxString(CIHListBox *plistBox)
{
	plistBox->ResetContent();
}

void CStationCfg::OnSetPreTipsSelectedInfo(list<BYTE> *plist)
{
	list<BYTE>::iterator iter = plist->begin();
	int nTipsCount = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.size();
	BOOL bFlag = FALSE;
	while (iter != plist->end())
	{
		BYTE temp = *iter;
		if (temp == 0xfd)
		{
			bFlag = TRUE;	
			iter ++;
			continue;
		}
		if (temp == 0xfe)
		{
			break;
		}
		if (bFlag)
		{
			int nsel = (int)*iter;
			int iCount=m_listPreTipsSelected.GetCount();			
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(nsel-nTipsCount-1);
			if (m_bEnglish )
			{
				m_listPreTipsSelected.InsertString(iCount,strValue[LAN_EN]);
			}
			else
			{
				m_listPreTipsSelected.InsertString(iCount,strValue[LAN_IRAN]);
			}
		}	
		iter ++;
	}
}

void CStationCfg::OnSetArrivalTipsSelectedInfo(list<BYTE> *plist)
{
	list<BYTE>::iterator iter = plist->begin();
	int nTipsCount = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.size();
	int iFlag = 0;
	while (iter != plist->end())
	{
		BYTE temp = *iter;
		if (temp == 0xfd)
		{
			iFlag++;
			if (iFlag==2)
			{
				iter ++;
				continue;
			}
		}
		if (temp == 0xfe && iFlag==2)
		{
			break;
		}
		if (iFlag==2)
		{
			int nsel = (int)*iter;
			int iCount=m_listArrivalTipsSelected.GetCount();			
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(nsel-nTipsCount-1);
			if (m_bEnglish )
			{
				m_listArrivalTipsSelected.InsertString(iCount,strValue[LAN_EN]);
			}
			else
			{
				m_listArrivalTipsSelected.InsertString(iCount,strValue[LAN_IRAN]);
			}
		}	
		iter ++;
	}
}

void CStationCfg::OnSetPreAdsSelectedInfo(list<BYTE> *plist)
{
	list<BYTE>::iterator iter = plist->begin();
	BOOL bFlag = FALSE;
	while (iter != plist->end())
	{
		BYTE temp = *iter;
		if (temp == 0xfe)
		{
			bFlag = TRUE;	
			iter ++;
			continue;
		}
		if (temp == 0xfc)
		{
			break;
		}
		if (bFlag)
		{
			int nsel = (int)*iter;
			int iCount=m_listPreAdsSelected.GetCount();			
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(nsel-1);
			if (m_bEnglish )
			{
				m_listPreAdsSelected.InsertString(iCount,strValue[LAN_EN]);
			}
			else
			{
				m_listPreAdsSelected.InsertString(iCount,strValue[LAN_IRAN]);
			}
		}	
		iter ++;
	}
}

void CStationCfg::OnSetArrivalAdsSelectedInfo(list<BYTE> *plist)
{
	list<BYTE>::iterator iter = plist->begin();
	int iFlag = 0;
	while (iter != plist->end())
	{
		BYTE temp = *iter;
		if (temp == 0xfe)
		{
			iFlag++;	
			if (iFlag==2)
			{
				iter ++;
				continue;
			}			
		}
		if (temp == 0xff && iFlag==2)
		{
			break;
		}
		if (iFlag==2)
		{
			int nsel = (int)*iter;
			int iCount=m_listArrivalAdsSelected.GetCount();			
			XString strValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(nsel-1);
			if (m_bEnglish )
			{
				m_listArrivalAdsSelected.InsertString(iCount,strValue[LAN_EN]);
			}
			else
			{
				m_listArrivalAdsSelected.InsertString(iCount,strValue[LAN_IRAN]);
			}
		}	
		iter ++;
	}
}

void CStationCfg::OnlistCopyData(list<BYTE> *plistDes, list<BYTE> *plistSrc, BOOL bClear)
{	
	if (plistSrc->size()>=6)
	{
		plistDes->clear();
		list<BYTE>::iterator iter = plistSrc->begin();
		while (iter != plistSrc->end())
		{
			plistDes->push_back(*iter);
			iter++;
		}
	}		
	if (bClear)
	{
		plistSrc->clear();
		//插入6个默认值
		plistSrc->push_back(0xfd);
		plistSrc->push_back(0xfe);
		plistSrc->push_back(0xfc);
		plistSrc->push_back(0xfd);
		plistSrc->push_back(0xfe);
		plistSrc->push_back(0xff);
	}
}

typedef struct _tagStationCfgSaveInfo_
{
	BYTE bIndex;
	BYTE bLang;
	int nLeng; 
	BYTE bData[255];
}tagStationCfgSaveInfo;

void CStationCfg::ReadFileStationInfo()
{
	FILE *pf = fopen("G3_StationCfg.bin", "rb+");
	if (!pf)
	{
		return;
	}

	tagStationCfgSaveInfo StationInfo;
	memset(&StationInfo, 0x00, sizeof(tagStationCfgSaveInfo));
	while(!feof(pf))
	{
		int nRet = fread(&StationInfo, sizeof(tagStationCfgSaveInfo), 1, pf);
		if (nRet)
		{
			int nLang=StationInfo.bLang;
// 			if (StationInfo.bData[0]==0xfd && StationInfo.bData[1]==0xfe && StationInfo.bData[2]==0xfc && 
// 				StationInfo.bData[3]==0xfd && StationInfo.bData[4]==0xfe && StationInfo.bData[5]==0xff)
			if (StationInfo.bData[0]!=0 && StationInfo.bData[1]!=0 && StationInfo.bData[2]!=0 && 
				StationInfo.bData[3]!=0 && StationInfo.bData[4]!=0 && StationInfo.bData[5]!=0)
			{
				m_listArr[nLang][StationInfo.bIndex].clear();
			}

			for (int i=0; i<255; i++)
			{
				if (StationInfo.bData[i]!=0)
				{
					m_listArr[nLang][StationInfo.bIndex].push_back(StationInfo.bData[i]);
				}
				else
				{
					break;
				}
			}
		}
	}
	fclose(pf);
}

void CStationCfg::WriteFileStationInfo()
{
	FILE *pf = fopen("G3_StationCfg.bin", "wb+");
	if (!pf)
	{
		return;
	}	
	for (int nlang=0; nlang<2; nlang++)
	{
		for (int i=0; i<255; i++)
		{
			int iterIndex = 0;
			tagStationCfgSaveInfo StationInfo;
			memset(&StationInfo, 0x00, sizeof(tagStationCfgSaveInfo));

			StationInfo.bIndex = i;
			StationInfo.bLang = nlang;
			StationInfo.nLeng = m_listArr[nlang][i].size();

			list<BYTE>::iterator iter = m_listArr[nlang][i].begin();
			for (; iter!=m_listArr[nlang][i].end(); iter++)
			{
				BYTE byteInfo =(BYTE)*iter;
				StationInfo.bData[iterIndex] = byteInfo;
				//TRACE("%x ", StationInfo.bData[iterIndex]);
				iterIndex++;
			}
			//TRACE("\n");

			fwrite(&StationInfo, sizeof(tagStationCfgSaveInfo), 1, pf);
		}	
	}

	fclose(pf);
}
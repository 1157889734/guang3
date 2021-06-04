// DevStatusQueryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "DevStatusQueryDlg.h"


#include "TmsProtocol.h"
#include "PisProtocol.h"

#include "MainFrm.h"



// CDevStatusQueryDlg 对话框

IMPLEMENT_DYNAMIC(CDevStatusQueryDlg, CDialog)

CDevStatusQueryDlg::CDevStatusQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDevStatusQueryDlg::IDD, pParent)
{
	ptbuf=NULL;
	
	m_pTcmsProtocol = new CTmsProtocol;
	m_pPiscProtocol = new CPisProtocol;
	m_nCurrViewStyle = SHOW_LIST_STYLE_NONE;
	m_bStopQuerData = FALSE;
}

CDevStatusQueryDlg::~CDevStatusQueryDlg()
{
	if(ptbuf!=NULL)
	{
		free(ptbuf);
		ptbuf=NULL;
	}
	if (NULL != m_pTcmsProtocol)
	{
		delete m_pTcmsProtocol;
		m_pTcmsProtocol = NULL;
	}
	if (NULL != m_pPiscProtocol)
	{
		delete m_pPiscProtocol;
		m_pPiscProtocol = NULL;
	}
}

void CDevStatusQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_RICHEDIT21, m_rich);
}


BEGIN_MESSAGE_MAP(CDevStatusQueryDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECTFILE_BUTTON, &CDevStatusQueryDlg::OnBnClickedSelectfileButton)
	ON_BN_CLICKED(IDC_SELECTLOCALFILE, &CDevStatusQueryDlg::OnBnClickedSelectlocalfile)
	ON_BN_CLICKED(IDC_BUTTON1, &CDevStatusQueryDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDevStatusQueryDlg::OnNMClickList1)
	
	ON_BN_CLICKED(IDC_CLEANSCREEN, &CDevStatusQueryDlg::OnBnClickedCleanscreen)
	ON_BN_CLICKED(IDC_RTCSET, &CDevStatusQueryDlg::OnBnClickedRtcset)
	ON_BN_CLICKED(IDC_CLEANSCREENFILE, &CDevStatusQueryDlg::OnBnClickedCleanscreenfile)
	ON_WM_CLOSE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDevStatusQueryDlg 消息处理程序

BOOL CDevStatusQueryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_dlgHistoricalView.Create(CDlgHistoricalView::IDD, this);
	m_dlgHistoricalView.ShowWindow(SW_HIDE);

	DWORD dwStyle = m_list.GetExtendedStyle();
	//dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
	m_list.SetExtendedStyle(dwStyle);	


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDevStatusQueryDlg::OnBnClickedSelectfileButton()
{
	// TODO: 在此添加控件通知处理程序代码
	m_selfiledlg.DoModal();
}

void CDevStatusQueryDlg::OnBnClickedSelectlocalfile()
{
	m_bStopQuerData = TRUE;
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog openFile(TRUE, NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("TXT Files (*.txt)|*.txt|"),NULL,0);
	if (IDOK == openFile.DoModal())
	{
		binFilePath = openFile.GetPathName();
		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText(binFilePath);
	}	
    //ShowListItem(_T("1"),_T("2"),_T("3"));
}

void CDevStatusQueryDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_list.DeleteAllItems();
	if(ptbuf!=NULL)
	{
		free(ptbuf);
		ptbuf=NULL;
	}
	CString pathstr;
    GetDlgItemText(IDC_EDIT_FILEPATH,pathstr);

	int nViewStyle = GetCompatibleViewStyle(pathstr);
	m_nCurrViewStyle = nViewStyle;
	if (SHOW_LIST_STYLE_NONE == nViewStyle)
		return;

	CProtocolAnalysis *pAnalysizer = GetCompatibleProtocolAnalysizer(pathstr);

	BOOL blflag=file.Open(pathstr,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead);
	if(blflag==false)
	{
		MessageBox(_T("文件路径错误"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	int count = file.GetLength();
	filecount=count;
	ptbuf=(BYTE*)malloc(count);
	file.Read(ptbuf,count);
	int totalcounterror=0;
	///////////////////////////////////////////////////
	switch (nViewStyle)
	{
	case SHOW_LIST_BASIC_INFO:
		// 时间 
		ShowTimeTxtData();
		break;
	case SHOW_LIST_TCMS_DATA:
		// TMS数据
		ShowProtocolTxtData(nViewStyle);
		break;
	case SHOW_LIST_PISC_DATA:
		// PIS数据
		ShowProtocolTxtData(nViewStyle);
		break;
	default:
		break;
	}

	file.Close();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	return;
}

void CDevStatusQueryDlg::ShowTimeTxtData(void )
{
	// 初始化列表头
	int nColumnCount = m_list.GetHeaderCtrl()->GetItemCount();
	for (int n = 0; n < nColumnCount; n++)
		m_list.DeleteColumn(0);

	m_list.InsertColumn(0, _T("No."));
	m_list.SetColumnWidth(0, 40);
	
	m_list.InsertColumn(1, _T("时间类型"));
	m_list.SetColumnWidth(1, 150);
	
	m_list.InsertColumn(2, _T("时间值"));
	m_list.SetColumnWidth(2, 200);

	// 插入列表数据
	int nDataErrorCount = 0;
	for (int n = 0; n < filecount; /*empty*/)
	{
		if (ptbuf[n] == 0x68)
		{
			CString strIndex = _T("");
			CString strTimeType = _T("");
			CString strTimeVal = _T("");
			switch (ptbuf[n + 1])
			{
			case 0x01:
				strTimeType = _T("启动时间");
				break;
			case 0x02:
				strTimeType = _T("系统修改前时间");
				break;
			case 0x03:
				strTimeType = _T("系统修改后时间");
				break;
			default:
				// 协议解析错误
				break;
			}

			if (strTimeType.Compare(_T("")) == 0)
			{
				nDataErrorCount ++;
				n++;
				continue;
			}
			else{
				strTimeVal.Format(_T("20%02d-%02d-%02d   %02d:%02d:%02d"), ptbuf[n + 7], ptbuf[n + 6], ptbuf[n + 5],
					ptbuf[n + 4], ptbuf[n + 3], ptbuf[n + 2]);
				int nIndex = m_list.GetItemCount();
				strIndex.Format(_T("%d"), nIndex + 1);

				m_list.InsertItem(nIndex, strIndex);
				m_list.SetItemText(nIndex, 1, strTimeType);
				m_list.SetItemText(nIndex, 2, strTimeVal);
				m_list.SetItemData(nIndex, (DWORD_PTR)(ptbuf + n));

				n += 8;
				continue;
			}
		}
		else{
			nDataErrorCount ++;
			n++;
			continue;
		}
	}

	// 打印统计信息
	CString str;
	str.Format(_T("出现%d次协议不对称数据"), nDataErrorCount);
	SetDlgItemText(IDC_STATIC_ERROR, str);
}

int g_nViewStyle = 0;
UINT StartProtocolData(LPVOID pParam)
{
	CDevStatusQueryDlg *pQuerDlg = (CDevStatusQueryDlg*)pParam;
	int nErrorCount=0;
	if (pQuerDlg)
	{
		if (g_nViewStyle==SHOW_LIST_TCMS_DATA)
		{
			nErrorCount = pQuerDlg->InsertTMSDataToPanle();
		}
		else if (g_nViewStyle == SHOW_LIST_PISC_DATA)
		{
			nErrorCount = pQuerDlg->InsertPISCDataToPanle();
		}		
	}


	// 打印统计信息
	CString str;
	str.Format(_T("出现%d次协议不对称数据"), nErrorCount);
	pQuerDlg->SetDlgItemText(IDC_STATIC_ERROR, str);

	pQuerDlg->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);

	return 0;
}

void CDevStatusQueryDlg::ShowProtocolTxtData(int nViewStyle)
{
	m_bStopQuerData = FALSE;
	g_nViewStyle = nViewStyle;
	AfxBeginThread(StartProtocolData, this);	
}

int CDevStatusQueryDlg::InsertPISCDataToPanle()
{
	CProtocolAnalysis *pAnalysis = m_pPiscProtocol;
	int nColumnWidth = 250;

	// 初始化列表头
	int nColumnCount = m_list.GetHeaderCtrl()->GetItemCount();
	for (int n = 0; n < nColumnCount; n++)
		m_list.DeleteColumn(0);

	m_list.InsertColumn(0, _T("No."));
	m_list.SetColumnWidth(0, 40);

	m_list.InsertColumn(1, _T("开始时间"));
	m_list.SetColumnWidth(1, 80);

	m_list.InsertColumn(2, _T("结束时间"));
	m_list.SetColumnWidth(2, 80);

	m_list.InsertColumn(3, _T("重复数量"));
	m_list.SetColumnWidth(3, 80);

	int nCount = pAnalysis->GetElementCount();
	for (int n = 0; n < nCount; n++)
	{
		int nColumnIndex = m_list.GetHeaderCtrl()->GetItemCount();
		m_list.InsertColumn(nColumnIndex, pAnalysis->GetElementName(n));
		m_list.SetColumnWidth(nColumnIndex, nColumnWidth);
	}

	// 插入列表数据
	int nDataErrorCount = 0;
	for (int n = 0; n < filecount; /*empty*/)
	{
		if (m_bStopQuerData)
		{
			break;
		}
		if (0x68 == ptbuf[n])
		{
			if ((n + 10) >= filecount)
			{
				nDataErrorCount ++;
				n ++;
				continue;
			}
			if (0x68 != ptbuf[n + 8])
			{
				nDataErrorCount ++;
				n ++;
				continue;
			}
			if (ptbuf[15+n]==0xff)
			{
				CString strIndex = _T("");
				CString strTimeBegin = _T("");
				CString strTimeEnd = _T("");
				CString strRepeatTimes = _T("");
				strTimeBegin.Format(_T("%02d:%02d:%02d"), ptbuf[n + 3], ptbuf[n + 2], ptbuf[n + 1]);
				strTimeEnd.Format(_T("%02d:%02d:%02d"), ptbuf[n + 6], ptbuf[n + 5], ptbuf[n + 4]);
				strRepeatTimes.Format(_T("%d"), ptbuf[n + 7]);
				int nIndex = m_list.GetItemCount();
				strIndex.Format(_T("%d"), nIndex + 1);


				m_list.InsertItem(nIndex, strIndex);
				m_list.SetItemText(nIndex, 1, strTimeBegin);
				m_list.SetItemText(nIndex, 2, strTimeEnd);
				m_list.SetItemText(nIndex, 3, strRepeatTimes);
				pAnalysis->SetProtocolStream(ptbuf + n + 10, ptbuf[n + 9]);

				for (int it = 0; it < nCount; it++)
				{
					CString strDisplay ;//= pAnalysis->GetElementDataDisplay(it);
					strDisplay.Format(_T("State1[%02X], State2[%02X], State3[%02X]"), ptbuf[n +11], ptbuf[n +12], ptbuf[n +13]);
					m_list.SetItemText(nIndex, 4 + it ,strDisplay);
				}
				m_list.SetItemData(nIndex, (DWORD_PTR)(ptbuf + n));
			}
			n += (10 + ptbuf[n + 9]);		
		}
		else
		{
			nDataErrorCount ++;
			n ++;
		}
	}
	return nDataErrorCount;
}

int CDevStatusQueryDlg::InsertTMSDataToPanle()
{
	CProtocolAnalysis *pAnalysis = m_pTcmsProtocol;
	int nColumnWidth = 100;

	// 初始化列表头
	int nColumnCount = m_list.GetHeaderCtrl()->GetItemCount();
	for (int n = 0; n < nColumnCount; n++)
		m_list.DeleteColumn(0);

	m_list.InsertColumn(0, _T("No."));
	m_list.SetColumnWidth(0, 40);

	m_list.InsertColumn(1, _T("开始时间"));
	m_list.SetColumnWidth(1, 80);

	m_list.InsertColumn(2, _T("结束时间"));
	m_list.SetColumnWidth(2, 80);

	m_list.InsertColumn(3, _T("重复数量"));
	m_list.SetColumnWidth(3, 80);

	int nCount = pAnalysis->GetElementCount();
	for (int n = 0; n < nCount; n++)
	{
		int nColumnIndex = m_list.GetHeaderCtrl()->GetItemCount();
		m_list.InsertColumn(nColumnIndex, pAnalysis->GetElementName(n));
		m_list.SetColumnWidth(nColumnIndex, nColumnWidth);
	}

	// 插入列表数据
	int nDataErrorCount = 0;
	for (int n = 0; n < filecount; /*empty*/)
	{
		if (m_bStopQuerData)
		{
			break;
		}
		if (0x68 == ptbuf[n])
		{
			if ((n + 10) >= filecount)
			{
				nDataErrorCount ++;
				n ++;
				continue;
			}
			if (0x68 != ptbuf[n + 8])
			{
				nDataErrorCount ++;
				n ++;
				continue;
			}
			if (ptbuf[60+n]==1)
			{
				CString strIndex = _T("");
				CString strTimeBegin = _T("");
				CString strTimeEnd = _T("");
				CString strRepeatTimes = _T("");
				strTimeBegin.Format(_T("%02d:%02d:%02d"), ptbuf[n + 3], ptbuf[n + 2], ptbuf[n + 1]);
				strTimeEnd.Format(_T("%02d:%02d:%02d"), ptbuf[n + 6], ptbuf[n + 5], ptbuf[n + 4]);
				strRepeatTimes.Format(_T("%d"), ptbuf[n + 7]);
				int nIndex = m_list.GetItemCount();
				strIndex.Format(_T("%d"), nIndex + 1);


				m_list.InsertItem(nIndex, strIndex);
				m_list.SetItemText(nIndex, 1, strTimeBegin);
				m_list.SetItemText(nIndex, 2, strTimeEnd);
				m_list.SetItemText(nIndex, 3, strRepeatTimes);
				pAnalysis->SetProtocolStream(ptbuf + n + 10, ptbuf[n + 9]);

				for (int it = 0; it < nCount; it++)
				{
					CString strDisplay = pAnalysis->GetElementDataDisplay(it);
					m_list.SetItemText(nIndex, 4 + it ,strDisplay);
				}
				m_list.SetItemData(nIndex, (DWORD_PTR)(ptbuf + n));
			}

			//if (ptbuf[60+n]==1)
			//{
			//	for (int nn=60; nn<65; nn++)
			//	{
			//		TRACE("%x ", ptbuf[n+nn]);
			//	}
			//	TRACE("%x %x\n", ptbuf[n], ptbuf[n+10]);
			//}		

			//ASSERT(ptbuf[n + 9] == 0x35);
			n += (10 + ptbuf[n + 9]);		
		}
		else
		{
			nDataErrorCount ++;
			n ++;
		}
	}
	return nDataErrorCount;
}

void CDevStatusQueryDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;            
	if(pNMListView->iItem != -1)            
	{                 
		CString strData = _T("");
		CString strTemp = _T("");
		BYTE *pData = (BYTE *)m_list.GetItemData(pNMListView->iItem);
		if (NULL == pData)
			return;

		int nDataLen = 0;
		if (m_nCurrViewStyle == SHOW_LIST_TCMS_DATA || m_nCurrViewStyle == SHOW_LIST_PISC_DATA)
		{
			nDataLen = 10 + pData[9];
		}
		else if (m_nCurrViewStyle == SHOW_LIST_BASIC_INFO)
		{
			nDataLen = 8;
		}
		else
		{
			MessageBox(_T("无法解析的内容"), _T("数据解析"), MB_OK);
			return;
		}

		for (int n = 0; n < nDataLen; n++)
		{
			strTemp.Format(_T("%02X "), pData[n]);
			strData += strTemp;
		}
		m_rich.GetWindowText(strTemp);
		m_rich.SetWindowText(strTemp+_T("\r\n\r\n")+strData);
		int nIndex = m_rich.GetLineCount();
		m_rich.LineScroll(nIndex, 0);

		if (m_nCurrViewStyle == SHOW_LIST_TCMS_DATA)
		{
			m_dlgHistoricalView.ShowWindow(SW_SHOW);

			tagTmsInfo stTmsInfo;
			memset(&stTmsInfo, 0x00, sizeof(tagTmsInfo));
			stTmsInfo.bAto = pData[50]&1;
			stTmsInfo.bleft = (pData[50] >> 2) & 1;;
			stTmsInfo.bright = (pData[50] >> 3) & 1;

			int nMake = m_list.GetSelectionMark();
			CString szDesStation = m_list.GetItemText(nMake, 5);
			CString szNextStation = m_list.GetItemText(nMake, 6);

			m_dlgHistoricalView.OnTcmsStatusDisplay(stTmsInfo,szDesStation,szNextStation);
			//m_dlgHistoricalView.OnTcmsStatusDisplay(pData + 10, pData[9]);
		}
		else if (m_nCurrViewStyle == SHOW_LIST_PISC_DATA)
		{
			m_dlgHistoricalView.ShowWindow(SW_SHOW);
			m_dlgHistoricalView.OnPiscStatusDisplay(pData, pData[9]);
		}
	}

	*pResult = 0;
	
}

void CDevStatusQueryDlg::OnBnClickedCleanscreen()
{
	// TODO: 在此添加控件通知处理程序代码
	m_rich.SetWindowText(_T(""));
}

void CDevStatusQueryDlg::OnBnClickedRtcset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_rtctimesetdlg.DoModal();
}

void CDevStatusQueryDlg::OnBnClickedCleanscreenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	m_list.DeleteAllItems();

}

int CDevStatusQueryDlg::GetCompatibleViewStyle(const CString &strFileName)
{
	CString strTemp = strFileName;
	strTemp.MakeUpper();

	int nStyle = SHOW_LIST_STYLE_NONE;
	
	if (strTemp.Find(_T("TIME.TXT")) != -1)
	{
		nStyle = SHOW_LIST_BASIC_INFO;
	}
	else if (strTemp.Find(_T("TCMS.TXT")) != -1)
	{
		nStyle = SHOW_LIST_TCMS_DATA;
	}
	else if (strTemp.Find(_T("PISC.TXT")) != -1)
	{
		nStyle = SHOW_LIST_PISC_DATA;
	}
	else if (strTemp.Find(_T("TBA.TXT")) != -1)
	{
		nStyle = SHOW_LIST_PISC_DATA;
	}

	return nStyle;
};

CProtocolAnalysis * CDevStatusQueryDlg::GetCompatibleProtocolAnalysizer(const CString &strFileName)
{
	CProtocolAnalysis *p = NULL;

	CString strTemp = strFileName;
	strTemp.MakeUpper();

	if (strTemp.Find(_T("TIME.TXT")) != -1)
	{
		p = NULL;
	}
	else if (strTemp.Find(_T("TCMS.TXT")) != -1)
	{
		p = m_pTcmsProtocol;
	}
	else if (strTemp.Find(_T("PICS.TXT")) != -1)
	{
		p = m_pPiscProtocol;
	}
	else if (strTemp.Find(_T("TBA.TXT")) != -1)
	{
		p = m_pPiscProtocol;
	}

	return p;
}


void CDevStatusQueryDlg::OnClose()
{
	::SendMessage(m_dlgHistoricalView.GetSafeHwnd(), WM_CLOSE, NULL, NULL);

	CDialog::OnClose();
}

void CDevStatusQueryDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
}

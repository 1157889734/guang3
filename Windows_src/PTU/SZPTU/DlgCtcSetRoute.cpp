// DlgCtcSetRoute.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "DlgCtcSetRoute.h"
#include "MainFrm.h"


// CDlgCtcSetRoute 对话框

IMPLEMENT_DYNAMIC(CDlgCtcSetRoute, CDialog)


const UINT TIMER_COMMU_TIMEOUT = 1000;
const UINT ELAPSE_COMMU_TIMEOUT = 3000;


const int IMG_INDEX_ARROW_DOWN = 0;
const int IMG_INDEX_ARROW_CROSS = 1;
const int IMG_INDEX_ARROW_UP = 2;











CDlgCtcSetRoute::CDlgCtcSetRoute(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCtcSetRoute::IDD, pParent)
{
	m_arrayStationNotify = new DWORD[INT_STATION_NUM];
	memset(m_arrayStationNotify, 0, INT_STATION_NUM * sizeof(DWORD));
	m_arrayStationNotifyTemp = new DWORD[INT_STATION_NUM];
	memset(m_arrayStationNotifyTemp, 0, INT_STATION_NUM * sizeof(DWORD));
}

CDlgCtcSetRoute::~CDlgCtcSetRoute()
{
	if (NULL != m_arrayStationNotify)
	{
		delete [] m_arrayStationNotify;
		m_arrayStationNotify = NULL;
	}
	if (NULL != m_arrayStationNotifyTemp)
	{
		delete [] m_arrayStationNotifyTemp;
		m_arrayStationNotifyTemp = NULL;
	}
}

void CDlgCtcSetRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTC_ROUTES, m_listRoutes);
	DDX_Control(pDX, IDC_COMBO_ROUTE_TERMINUS, m_combTerminus);
	DDX_Control(pDX, IDC_LIST_ROUTE_TABLE, m_listNotifySet);
	DDX_Control(pDX, IDC_STC_CTC_ROUTE_QUERY_PROCESS, m_stcRouteQueryProcess);
	DDX_Control(pDX, IDC_STC_CTC_ROUTE_SYNC_PROCESS, m_stcRouteSyncProcess);
}


BEGIN_MESSAGE_MAP(CDlgCtcSetRoute, CDialog)
	ON_BN_CLICKED(IDC_BTN_CTC_SETTING_READ, &CDlgCtcSetRoute::OnBnClickedBtnCtcSettingRead)
	ON_BN_CLICKED(IDC_BTN_ROUTE_SETTING_SYNC, &CDlgCtcSetRoute::OnBnClickedBtnRouteSettingSync)
	ON_BN_CLICKED(IDC_BTN_NEW_ROUTE_SETTING, &CDlgCtcSetRoute::OnBnClickedBtnNewRouteSetting)
	ON_BN_CLICKED(IDC_BTN_ROUTE_SETTING_CLEAR, &CDlgCtcSetRoute::OnBnClickedBtnRouteSettingClear)
	ON_BN_CLICKED(IDC_BTN_DELETE_ROUTE_SETTING, &CDlgCtcSetRoute::OnBnClickedBtnDeleteRouteSetting)
	ON_STN_CLICKED(IDC_BTN_SELECT_ALL, &CDlgCtcSetRoute::OnStnClickedBtnSelectAll)
	ON_STN_CLICKED(IDC_BTN_SELECT_REVERSE, &CDlgCtcSetRoute::OnStnClickedBtnSelectReverse)
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTE_TERMINUS, &CDlgCtcSetRoute::OnCbnSelchangeComboTerminus)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CTC_ROUTES, &CDlgCtcSetRoute::OnNMClickListRoutes)

	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgCtcSetRoute 消息处理程序


BOOL CDlgCtcSetRoute::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初始化终点站下拉框
	m_combTerminus.ResetContent();
	m_combTerminus.AddString(_T("--请选择--"));
	m_combTerminus.SetCurSel(0);
	for (UINT n = 0; n < INT_STATION_NUM; n++)
	{
		m_combTerminus.AddString(ARRAY_STR_STATION_NAME[n]);
	}

	// 初始化终点站报站选择列表
	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_imglstRouteTable.Attach(hList);
	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BMP_UPDOWN_ARROW);
	m_imglstRouteTable.Add(&cBmp, RGB(255,0, 255));
	m_listNotifySet.SetImageList(&m_imglstRouteTable, LVSIL_SMALL);
	m_listNotifySet.SetExtendedStyle(m_listNotifySet.GetStyle()|LVS_EX_CHECKBOXES);
	m_listNotifySet.SetColumnWidth(0, 130);

	m_stcRouteQueryProcess.ShowWindow(SW_HIDE);
	m_stcRouteSyncProcess.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}




void CDlgCtcSetRoute::OnBnClickedBtnCtcSettingRead()
{
	if (!((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
		::MessageBox(this->GetSafeHwnd(), _T("请先连接CTC模块。"), _T("查询交路配置"), MB_OK);
		return;
	}

	BYTE bufSend[BUFSIZ];
	memset(bufSend, 0, sizeof(bufSend));

	WORD nLen = 0;
	bufSend[nLen++] = 0x7E;
	bufSend[nLen++] = CMD_QUERY_CTC_ROUTE;
	bufSend[nLen++] = 0x00;
	bufSend[nLen++] = 0x00;
	bufSend[nLen++] = SumUp(bufSend + 1, nLen - 1);
	bufSend[nLen++] = 0x7E;
	int nTransCodeLen = ((CMainFrame *)AfxGetMainWnd())->SwitchSendData(bufSend, nLen);
	USB_DATAPACK pack;
	pack.pPtr = bufSend;
	pack.nLen = nTransCodeLen;
	BOOL bRes = ((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(&pack);

	if (bRes)
	{
		m_nCurrSyncStationCode = 1;
		SetTimer(TIMER_COMMU_TIMEOUT, ELAPSE_COMMU_TIMEOUT, NULL);
	}
	else 
	{
		::MessageBox(this->GetSafeHwnd(), _T("同步数据包发送失败。"), _T("同步交路配置"), MB_OK);
	}
}

void CDlgCtcSetRoute::OnBnClickedBtnRouteSettingSync()
{
	if (!((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
		::MessageBox(this->GetSafeHwnd(), _T("请先连接CTC模块。"), _T("同步交路配置"), MB_OK);
		return;
	}

	m_nCurrSyncStationCode = 1;
	SendRouteSetting();
}

BOOL CDlgCtcSetRoute::SendRouteSetting(void )
{
	KillTimer(TIMER_COMMU_TIMEOUT);
	if (m_nCurrSyncStationCode > INT_STATION_NUM)
	{
		m_stcRouteSyncProcess.ShowWindow(SW_HIDE);
		::MessageBox(this->GetSafeHwnd(), _T("交路配置同步成功。"), _T("同步交路配置"), MB_OK);
		return TRUE;
	}
	else{
		m_stcRouteSyncProcess.ShowWindow(SW_SHOW);
		CString strShow;
		strShow.Format(_T("同步进度: %d/%d"), m_nCurrSyncStationCode, INT_STATION_NUM);
		m_stcRouteSyncProcess.SetWindowText(strShow);
	}


	BYTE bufSend[BUFSIZ];
	memset(bufSend, 0, sizeof(bufSend));

	BYTE byteStationCode = m_nCurrSyncStationCode;
	DWORD dwNotificationSet = m_arrayStationNotify[m_nCurrSyncStationCode - 1];
	m_nCurrSyncStationCode++;

	WORD nLen = 0;
	bufSend[nLen++] = 0x7E;
	bufSend[nLen++] = CMD_SET_CTC_ROUTE;
	bufSend[nLen++] = 0x00;
	bufSend[nLen++] = 0x00;
	bufSend[nLen++] = byteStationCode;
	bufSend[nLen++] = (BYTE)((dwNotificationSet) & 0xff);
	bufSend[nLen++] = (BYTE)((dwNotificationSet >> 8) & 0xff);
	bufSend[nLen++] = (BYTE)((dwNotificationSet >> 16) & 0xff);
	bufSend[nLen++] = (BYTE)((dwNotificationSet >> 24) & 0xff);

	bufSend[nLen++] = 0x00;        /* reserved for CTC reply */
	bufSend[2] = HIBYTE(nLen - 4); /* fill the length field to calculate CRC */
	bufSend[3] = LOBYTE(nLen - 4);
	bufSend[nLen++] = SumUp(bufSend + 1, nLen - 1);
	bufSend[nLen++] = 0x7E;

	int nTransCodeLen = ((CMainFrame *)AfxGetMainWnd())->SwitchSendData(bufSend, nLen);
	USB_DATAPACK pack;
	pack.pPtr = bufSend;
	pack.nLen = nTransCodeLen;
	BOOL bRes = ((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(&pack);

	if (bRes)
	{
		SetMainFrameCTCCommandType(CMD_SET_CTC_ROUTE);
		SetTimer(TIMER_COMMU_TIMEOUT, ELAPSE_COMMU_TIMEOUT, NULL);
		return TRUE;
	}
	else
	{
		::MessageBox(this->GetSafeHwnd(), _T("同步数据包发送失败。"), _T("同步交路配置"), MB_OK);
		return FALSE;
	}
}

void CDlgCtcSetRoute::OnBnClickedBtnNewRouteSetting()
{
	BYTE byteStation = 0;
	DWORD dwNotification = 0;
	if (!CollectOneWayRouteSetting(byteStation, dwNotification))
	{
		MessageBox(_T("\r\n请选择正确的终点站。\r\n\r\n"), _T("添加配置"), MB_OK);
		return;
	}

	int nListItemCount = m_listRoutes.GetItemCount();
	CString strStationName = ARRAY_STR_STATION_NAME[byteStation - 1];
	for (int n = 0; n < nListItemCount; n++)
	{
		if (byteStation == m_listRoutes.GetItemData(n))
		{
			CString strTip;
			strTip.Format(_T("\r\n交路[%s]配置已经存在。是否替换?\r\n\r\n"), strStationName);
			int nRes = MessageBox(strTip, _T("添加配置"), MB_YESNO);
			if (IDYES == nRes)
			{
				m_arrayStationNotify[byteStation - 1] = dwNotification;
				return;
			}
			else
			{
				return;
			}
		}
	}

	 int nRes = m_listRoutes.InsertItem(nListItemCount, strStationName);
	 m_listRoutes.SetItemData(nListItemCount, byteStation);

	 m_arrayStationNotify[byteStation - 1] = dwNotification;
}

void CDlgCtcSetRoute::OnBnClickedBtnRouteSettingClear()
{
	int nRes = MessageBox(_T("\r\n此操作不可恢复。 确实要清空配置吗? \r\n\r\n"), _T("清空配置"), MB_YESNO);
	if (IDYES == nRes)
	{
		m_listRoutes.DeleteAllItems();
		memset(m_arrayStationNotify, 0, INT_STATION_NUM * sizeof(DWORD));
	}
}

void CDlgCtcSetRoute::OnBnClickedBtnDeleteRouteSetting()
{
	int nSel = m_listRoutes.GetSelectionMark();
	if (-1 == nSel) /* no item was selected */
	{
		int nRes = ::MessageBox(this->GetSafeHwnd(), _T("\r\n请选择要删除的项。\r\n\r\n"), _T("删除配置"), MB_YESNO);
		return; 
	}

	CString strTip;
	strTip.Format(_T("\r\n确实要删除交路配置[%s]吗? \r\n\r\n"), m_listRoutes.GetItemText(nSel, 0));
	int nRes = ::MessageBox(this->GetSafeHwnd(), strTip, _T("删除配置"), MB_YESNO);
	if (IDYES == nRes)
	{
		BYTE byteStationCode = (BYTE)m_listRoutes.GetItemData(nSel);
		m_arrayStationNotify[byteStationCode - 1] = 0;
		m_listRoutes.DeleteItem(nSel);
	}
}

void CDlgCtcSetRoute::OnStnClickedBtnSelectAll()
{
	int nCount = m_listNotifySet.GetItemCount();
	for (int n = 0; n < nCount; n++)
	{
		m_listNotifySet.SetCheck(n);
	}
}

void CDlgCtcSetRoute::OnStnClickedBtnSelectReverse()
{
	int nCount = m_listNotifySet.GetItemCount();
	for (int n = 0; n < nCount; n++)
	{
		m_listNotifySet.SetCheck(n, !m_listNotifySet.GetCheck(n));
	}
}

void CDlgCtcSetRoute::OnCbnSelchangeComboTerminus()
{
	int nCurrSel = m_combTerminus.GetCurSel();

	m_listNotifySet.DeleteAllItems();
	LVITEM lvi;
	CString strItemName;
	for (int n = 0; n < INT_STATION_NUM; n++)
	{
		strItemName = ARRAY_STR_STATION_NAME[n];
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = n;
		lvi.iSubItem = 0;
		lvi.pszText = strItemName.GetBuffer();
		strItemName.ReleaseBuffer();

		if ((n + 1) > nCurrSel)
		{
			lvi.iImage = IMG_INDEX_ARROW_UP % 3;
		}
		else if ((n + 1) == nCurrSel)
		{
			lvi.iImage = IMG_INDEX_ARROW_CROSS % 3;
		}
		else {
			lvi.iImage = IMG_INDEX_ARROW_DOWN % 3;
		}


		int nRes = m_listNotifySet.InsertItem(&lvi);
	}
}

void CDlgCtcSetRoute::OnNMClickListRoutes(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem >= 0)
	{
		/* show data when mouse clicked a item */
		int nStationCode = (int )m_listRoutes.GetItemData(pNMListView->iItem);
		ShowRouteNotifySet(nStationCode, m_arrayStationNotify[nStationCode - 1]);
	}

	*pResult = 0;
}


BYTE CDlgCtcSetRoute::SumUp(const BYTE *pData, const int nLen)
{
	BYTE Sum = 0;
	for (int n = 0; n < nLen; n++)
	{
		Sum += pData[n];
	}
	return 0x55 - Sum;
}

void CDlgCtcSetRoute::UpdateRouteList(BYTE *tempBuf, int bufcount)
{
	// 更新进度
	DWORD dwNotificationSet = 0;
	dwNotificationSet |= (tempBuf[5]);
	dwNotificationSet |= (tempBuf[6] << 8);
	dwNotificationSet |= (tempBuf[7] << 16);
	dwNotificationSet |= (tempBuf[8] << 24);
	int nTerminus = tempBuf[4];
	if (nTerminus > INT_STATION_NUM) // 站代码错误
		return; 
	else if (ARRAY_INT_STATION_CODE[0] == nTerminus) // 下载开始
	{
		memset(m_arrayStationNotifyTemp, 0, INT_STATION_NUM * sizeof(DWORD));
		m_nUpdatedStationNum = 1;
	}
	else {
		m_arrayStationNotifyTemp[nTerminus - 1] = dwNotificationSet;
	}

	// 更新界面显示
	if (INT_STATION_NUM > m_nCurrSyncStationCode)
	{
		m_stcRouteQueryProcess.ShowWindow(SW_SHOW);
		CString strShow;
		strShow.Format(_T("同步进度: %d/%d"), m_nCurrSyncStationCode, INT_STATION_NUM);
		m_stcRouteQueryProcess.SetWindowText(strShow);

	}
	else if (INT_STATION_NUM <= m_nCurrSyncStationCode)
	{
		m_stcRouteQueryProcess.ShowWindow(SW_HIDE);
		::MessageBox(this->GetSafeHwnd(), _T("交路查询完成。"), _T("交路配置查询"), MB_OK);
	}
	m_nCurrSyncStationCode ++;

	// 进度完成 刷新列表
	if (nTerminus == INT_STATION_NUM)
	{
		m_listRoutes.DeleteAllItems();
		InitializeListNotifySet();

		memcpy_s(m_arrayStationNotify, INT_STATION_NUM * sizeof(DWORD),
			m_arrayStationNotifyTemp, INT_STATION_NUM * sizeof(DWORD));

		for (int n = 0; n < INT_STATION_NUM; n++)
		{
			if (0 != m_arrayStationNotify[n])
			{
				int nIndex = m_listRoutes.GetItemCount();
				m_listRoutes.InsertItem(nIndex, ARRAY_STR_STATION_NAME[n]);
				m_listRoutes.SetItemData(nIndex, ARRAY_INT_STATION_CODE[n]);
			}
		}
	}
}

void CDlgCtcSetRoute::ShowRouteNotifySet(BYTE byteStation, DWORD dwNotificationSet)
{
	m_combTerminus.SetCurSel(byteStation);
	OnCbnSelchangeComboTerminus();

	for (int n = 0; n < INT_STATION_NUM; n++)
	{
		DWORD dwMask = (0x01 << n) & dwNotificationSet;
		m_listNotifySet.SetCheck(n, dwMask);
	}
}

BOOL CDlgCtcSetRoute::CollectOneWayRouteSetting(OUT BYTE &byteStation, OUT DWORD &dwNotificationSet)
{
	byteStation = m_combTerminus.GetCurSel();

	/* return for invalid station code */
	if (1 > byteStation || INT_STATION_NUM < byteStation)
		return FALSE; 

	/* collect notification sets from the list control */
	dwNotificationSet = 0;
	for (int n = 0; n < INT_STATION_NUM; n++)
	{
		if (m_listNotifySet.GetCheck(n))
		{
			dwNotificationSet |= (0x01 << n);
		}
	}

	return TRUE;
}

void CDlgCtcSetRoute::InitializeListRoutes(void )
{
}

void CDlgCtcSetRoute::InitializeListNotifySet(void )
{
	m_combTerminus.SetCurSel(0);
	OnCbnSelchangeComboTerminus();
}

void CDlgCtcSetRoute::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_COMMU_TIMEOUT:
		KillTimer(TIMER_COMMU_TIMEOUT);
		SetMainFrameCTCCommandType(0);
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgCtcSetRoute::SetMainFrameCTCCommandType(int cmd)
{
	((CMainFrame *)AfxGetMainWnd())->m_nCurrCTCCmdType = cmd;
}
void CDlgCtcSetRoute::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
}

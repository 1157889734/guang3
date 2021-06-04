// SelFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "SelFileDlg.h"
#include "MainFrm.h"

// CSelFileDlg 对话框

IMPLEMENT_DYNAMIC(CSelFileDlg, CDialog)

CSelFileDlg::CSelFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelFileDlg::IDD, pParent)
{
	m_filesize=0;
	m_addsize=0;

}

CSelFileDlg::~CSelFileDlg()
{
}

void CSelFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(CSelFileDlg, CDialog)
	ON_BN_CLICKED(IDC_SENDSELFILECOMD, &CSelFileDlg::OnBnClickedSendselfilecomd)
	ON_BN_CLICKED(IDC_DOWNLOAD, &CSelFileDlg::OnBnClickedDownload)
	ON_MESSAGE(WM_FILE_SELECT,&CSelFileDlg::OnRecvFileData)
	ON_MESSAGE(WM_FILE_DOWN_FINISH,&CSelFileDlg::OnRecvFileFinish)
	ON_MESSAGE(WM_FILE_DOWN_DATA,&CSelFileDlg::OnRecvFileCount)
	ON_BN_CLICKED(IDC_DELETEFILE, &CSelFileDlg::OnBnClickedDeletefile)
	ON_BN_CLICKED(IDC_FORMAT, &CSelFileDlg::OnBnClickedFormat)
	ON_BN_CLICKED(IDC_DOWNLOADCANCLE, &CSelFileDlg::OnBnClickedDownloadcancle)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSelFileDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CSelFileDlg 消息处理程序

void CSelFileDlg::OnBnClickedSendselfilecomd()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	((CMainFrame *)AfxGetMainWnd())->m_filesize.RemoveAll();
	((CMainFrame *)AfxGetMainWnd())->m_filebufarray.RemoveAll();
	m_list.DeleteAllItems();
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
    BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x09;
    buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);

}

BOOL CSelFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
   
	m_list.SetExtendedStyle(dwStyle);
	
	m_list.InsertColumn(0,_T("No"),LVCFMT_CENTER,45);
	m_list.InsertColumn(1,_T("文件"),LVCFMT_LEFT,200);
	m_list.InsertColumn(2,_T("文件大小(字节)"),LVCFMT_LEFT,200);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelFileDlg::OnBnClickedDownload()
{
	// TODO: 在此添加控件通知处理程序代码

	////////////////////////////////////////////////
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	int selcount=0;
	int selindex=0;
    int iCount = m_list.GetItemCount();
	for(int j=0;j<iCount;j++)
	{
	    if(m_list.GetCheck(j)==TRUE)
		  {
			  selcount++;
			  selindex=j;
			 
		  }
	 }
	if(selcount>1)
	{
		MessageBox(_T("不能同时多个文件下载"),_T("Prompt"),MB_ICONWARNING);
		return ;
	}
	if(selcount==0)
	{
		MessageBox(_T("请选择你要下载的文件"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////
    if(selindex==0||selindex==1)
	{
		MessageBox(_T("请选择文件下载"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////////
    SendHeartData();
	SetTimer(WM_HEART_DATA,1000,NULL);
	///////////////////////////////////////////////////

	USES_CONVERSION;
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	CString strpath=m_list.GetItemText(selindex,1);
	char * pFilePath = T2A(strpath);   
	BYTE buf[100];
	buf[0]=0x7e;
	buf[1]=1+strlen(pFilePath);
	buf[2]=0x0a;
	memcpy(buf+3,pFilePath,strlen(pFilePath));
    buf[3+strlen(pFilePath)]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2+strlen(pFilePath));
	buf[4+strlen(pFilePath)]=0x7e;
	int count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5+strlen(pFilePath));
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
    //////////////////////////////////////////////
	CString strsize=m_list.GetItemText(selindex,2);
	m_filesize=_ttoi(strsize);
    m_progress.SetRange(0,50);
	m_progress.SetPos(0);
	m_addsize=0;


}
LRESULT CSelFileDlg::OnRecvFileData( WPARAM wParam,LPARAM lParam)
{
	//for(int i=0;i<(CMainFrame *)AfxGetMainWnd())->m_filebufarray)
	/////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////
	int amount=((CMainFrame *)AfxGetMainWnd())->m_filebufarray.GetCount();
	for(int i=0;i<amount;i++)
	{
		int count=m_list.GetItemCount();
		CString str;
		str.Format(_T("%d"),count+1);
		m_list.InsertItem(count,str);
		m_list.SetItemText(count,1,((CMainFrame *)AfxGetMainWnd())->m_filebufarray.GetAt(i));
		m_list.SetItemText(count,2,((CMainFrame *)AfxGetMainWnd())->m_filesize.GetAt(i));

	}
		
	return 1;
}
LRESULT CSelFileDlg::OnRecvFileFinish(WPARAM wParam,LPARAM lParam)
{
	int flag=(int)wParam;
	if(flag==1)
	    MessageBox(_T("下载完成"));
	if(flag==2)
		MessageBox(_T("文件正在使用中"),_T("Prompt"),MB_ICONWARNING);
	if(flag==3)
		MessageBox(_T("文件已取消"),_T("Prompt"),MB_ICONWARNING);
	if(flag==4)
		MessageBox(_T("TMS数据传输中，不能下载文件"),_T("Prompt"),MB_ICONWARNING);
	KillTimer(WM_HEART_DATA);
	return 1;
}
LRESULT CSelFileDlg::OnRecvFileCount(WPARAM wParam,LPARAM lParam)
{
	int pcount=(int)wParam;
    m_addsize+=pcount;
	int pos=m_addsize*1.0/m_filesize*50;
	m_progress.SetPos(pos);
	return 1;

}
void CSelFileDlg::OnBnClickedDeletefile()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	int selcount=0;
	int selindex=0;
	int iCount = m_list.GetItemCount();
	for(int j=0;j<iCount;j++)
	{
		if(m_list.GetCheck(j)==TRUE)
		{
			selcount++;
			selindex=j;

		}
	}
	if(selcount>1)
	{
		MessageBox(_T("不能同时多个文件删除"),_T("Prompt"),MB_ICONWARNING);
		return ;
	}
	if(selcount==0)
	{
		MessageBox(_T("请选择要删除的文件"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////////
	USES_CONVERSION;
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	CString strpath=m_list.GetItemText(selindex,1);
	char * pFilePath = T2A(strpath);   
	BYTE buf[100];
	buf[0]=0x7e;
	buf[1]=1+strlen(pFilePath);
	buf[2]=0x0d;
	memcpy(buf+3,pFilePath,strlen(pFilePath));
	buf[3+strlen(pFilePath)]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2+strlen(pFilePath));
	buf[4+strlen(pFilePath)]=0x7e;
	int count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5+strlen(pFilePath));
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CSelFileDlg::OnBnClickedFormat()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	((CMainFrame *)AfxGetMainWnd())->m_filesize.RemoveAll();
	((CMainFrame *)AfxGetMainWnd())->m_filebufarray.RemoveAll();
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x0c;
	buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);

}

void CSelFileDlg::OnBnClickedDownloadcancle()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{

	}
	else
	{
		MessageBox(_T("请先连接ATC模块"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	((CMainFrame *)AfxGetMainWnd())->m_filesize.RemoveAll();
	((CMainFrame *)AfxGetMainWnd())->m_filebufarray.RemoveAll();
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x0b;
	buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CSelFileDlg::SendHeartData(void)
{
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0xff;
	buf[3]=0x55;
	buf[4]=0x7e;
	
	pSendSet->pPtr=buf;
	pSendSet->nLen=5;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CSelFileDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    SendHeartData();
	CDialog::OnTimer(nIDEvent);
}

void CSelFileDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */ 
		) 
	{ 
		TRACE("Item %d is checked\n", pNMLV->iItem); 
		int nCount = m_list.GetItemCount();

		for (int i=0;i < nCount;i++)
		{
			if(i!=pNMLV->iItem)
			  m_list.SetCheck(i,false);

		}
		  
		

	} 
	else if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */ 
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */ 
		) 
	{ 
		//TRACE("Item %d is unchecked\n", pNMLV->iItem); 
	} 
	else 
	{ 
		//TRACE("Item %d does't change the check-status\n", pNMLV->iItem); 
	} 

	
	*pResult = 0;



	// TODO: 在此添加控件通知处理程序代码
	
}

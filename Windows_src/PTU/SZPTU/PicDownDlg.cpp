 // PicDownDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "PicDownDlg.h"
#include "MainFrm.h"

// CPicDownDlg 对话框

IMPLEMENT_DYNAMIC(CPicDownDlg, CDialog)

CPicDownDlg::CPicDownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPicDownDlg::IDD, pParent)
	, m_gridhigh(16)
	, m_gridwidth(64)
	, m_radiotxt(TRUE)
{
	m_msgcount=0;

}

CPicDownDlg::~CPicDownDlg()
{
}

void CPicDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CUSTOM1, m_pGrid);
	DDX_Text(pDX, IDC_GridHigh, m_gridhigh);
	//DDV_MinMaxUInt(pDX, m_gridhigh, 3, 24);
	DDX_Text(pDX, IDC_GridWidth, m_gridwidth);
	//DDV_MinMaxUInt(pDX, m_gridwidth, 2, 200);
	DDX_Check(pDX, IDC_CHECKTEXT, m_radiotxt);

	DDX_Control(pDX, IDC_DESCRIBLE, m_desbtn);
	DDX_Control(pDX, IDC_BUTTON2, m_selectfont);
	DDX_Control(pDX, IDC_LOADPIC, m_impottn);
	DDX_Control(pDX, IDC_SETGRIDWIDTH, m_setsizebtn);
	DDX_Control(pDX, IDC_CLEARGRID, m_clearbtn);
	DDX_Control(pDX, IDC_PICSAVE, m_savebtn);
	DDX_Control(pDX, IDC_LEDPARASET, m_creensetbtn);
	DDX_Control(pDX, IDC_SIGLEDOWN, m_singledownbtn);
	DDX_Control(pDX, IDC_BUTTON13, m_typedownbtn);
}


BEGIN_MESSAGE_MAP(CPicDownDlg, CDialog)
	ON_BN_CLICKED(IDC_PICSAVE, &CPicDownDlg::OnBnClickedPicsave)
	ON_BN_CLICKED(IDC_LEDPARASET, &CPicDownDlg::OnBnClickedLedparaset)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_PICTYPECOMB, &CPicDownDlg::OnCbnSelchangePictypecomb)
	ON_CBN_SELCHANGE(IDC_PICLANCOMB, &CPicDownDlg::OnCbnSelchangePiclancomb)
	ON_BN_CLICKED(IDC_SETGRIDWIDTH, &CPicDownDlg::OnBnClickedSetgridwidth)
	ON_BN_CLICKED(IDC_CLEARGRID, &CPicDownDlg::OnBnClickedCleargrid)
	ON_BN_CLICKED(IDC_UP, &CPicDownDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWM, &CPicDownDlg::OnBnClickedDowm)
	ON_BN_CLICKED(IDC_LEFT, &CPicDownDlg::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_RIGHT, &CPicDownDlg::OnBnClickedRight)
	ON_BN_CLICKED(IDC_DESCRIBLE, &CPicDownDlg::OnBnClickedDescrible)
	ON_BN_CLICKED(IDC_CHECKTEXT, &CPicDownDlg::OnBnClickedChecktext)
	ON_BN_CLICKED(IDC_LOADPIC, &CPicDownDlg::OnBnClickedLoadpic)
	ON_BN_CLICKED(IDC_SIGLEDOWN, &CPicDownDlg::OnBnClickedSigledown)
	ON_BN_CLICKED(IDC_BUTTON13, &CPicDownDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON2, &CPicDownDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_STATIONNOCOMB, &CPicDownDlg::OnCbnSelchangeStationnocomb)
	ON_CBN_SELCHANGE(IDC_OTHERNOCOMB, &CPicDownDlg::OnCbnSelchangeOthernocomb)
	ON_CBN_SELCHANGE(IDC_STREETCOMB, &CPicDownDlg::OnCbnSelchangeStreetcomb)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CPicDownDlg 消息处理程序

void CPicDownDlg::OnBnClickedPicsave()
{
	// TODO: 在此添加控件通知处理程序代码
	
    int type=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel();
	int lan=((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->GetCurSel();
	int index;
    if(((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->IsWindowEnabled())
        index=GetDlgItemInt(IDC_STATIONNOCOMB);
	else
		index=GetDlgItemInt(IDC_OTHERNOCOMB);
	m_picsavedlg.SetParates(type,lan,index);
	if(m_picsavedlg.DoModal()==IDCANCEL)
	{
		return;
	}
	///////////////////////////////////////////////////
	CString lanstr,typestr;
	if(m_picsavedlg.pictype==0)
	{
		typestr=_T("station");
	}
	if(m_picsavedlg.pictype==1)
	{
		typestr=_T("emg");
	}
	if(m_picsavedlg.pictype==2)
	{
		typestr=_T("msg");
	}
	///////////////////////////////////////
	if(m_picsavedlg.languageint==0)
	{
		lanstr=_T("CH");
	}
	if(m_picsavedlg.languageint==1)
	{
		lanstr=_T("EN");
	}
	CString strno;
	strno.Format(_T("%d"),m_picsavedlg.picNO);
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+lanstr+_T("\\")+typestr+_T("\\")+typestr+_T("_")+strno+_T(".bmp");
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	SaveClttice(iColumnCount, iRowCount, path);
	OnCbnSelchangePictypecomb();
}
void CPicDownDlg::SaveClttice( int iPicWidth,int iPicHeight,CString sSavepath )
{
	Pen mypen2(Color::Black);            //创建黑色画笔
	Pen myWhitePen2(Color::White);      //创建白色画笔
	Bitmap tempImage2(iPicWidth,iPicHeight,PixelFormat24bppRGB);      //根据点阵大小创建位图

	Pen *myPen = &mypen2;
	Pen * myWhitePen = &myWhitePen2;
	Bitmap *tempImage = &tempImage2;

	CClientDC dc(this);
	Graphics graphics(dc);
	Graphics *g = graphics.FromImage(tempImage);
	g->Clear(Color::White);                       //将新建的图像背景转换为白色

	for(int i = 0; i <= (iPicWidth - 1); i++)       //列循环
	{
		for(int j = 0; j<= (iPicHeight - 1); j++)   //行循环
		{
			GV_ITEM Item;

			Item.mask = GVIF_TEXT;
			Item.row = j;
			Item.col = i;
			Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
			m_pGrid.GetItem(&Item);
			if (Item.crBkClr == RGB(255,0,0)) //点阵为红色
			{
				g->DrawRectangle(myPen,i,j,1,1);             //画黑色
			}

			else
			{
				g->DrawRectangle(myWhitePen, i, j, 1, 1);    //画白色                 
			}    
		}
	}

	//保存路径宽字符转换
	WCHAR   *wChar; 
	wChar = sSavepath.Trim().AllocSysString(); 

	CLSID clsid;
	int nRet=0;
	nRet = GetEncoderClsid(L"image/bmp",&clsid);//得到CLSID
	//USES_CONVERSION;
	if(nRet>=0)
	{  
		CString sMsg;
		Status status = tempImage->Save(wChar,&clsid);	
		if(Ok != status)
		{
			sMsg.Format(_T("\nFailed to save image in %s file\n GDI+ Error: %u"),sSavepath,status);
			//AfxMessageBox(sMsg);
			MessageBox(sMsg , _T("提示"), MB_ICONWARNING);
		}
		else
		{
			//AfxMessageBox(_T("Save config information successfully!"));
			//MessageBox(_T("Save config information successfully") , _T("提示"), MB_OK);
			MessageBox(_T("保存位图成功！") , _T("提示"), MB_OK);
		}
	}
	if (myPen != NULL)
	{
		
		myPen = NULL;
	}
	if (myWhitePen != NULL)
	{
		
		myWhitePen = NULL;
	}
	if (tempImage != NULL)
	{
		
		tempImage = NULL;
	}
}
int CPicDownDlg::GetEncoderClsid( const WCHAR *format, CLSID *pClsid )
{
	int nRet=-1;
	ImageCodecInfo*pCodecInfo=NULL;
	UINT nNum=0,nSize=0;
	GetImageEncodersSize(&nNum,&nSize);
	if(nSize<0)
	{
		return nRet;
	}
	pCodecInfo=new ImageCodecInfo[nSize];
	if(pCodecInfo==NULL)
	{
		return nRet;
	}
	GetImageEncoders(nNum,nSize,pCodecInfo);
	for(UINT i=0;i<nNum;i )
	{
		if(wcscmp(pCodecInfo[i].MimeType,format)==0)
		{
			*pClsid=pCodecInfo[i].Clsid;
			nRet=i;

			delete[]pCodecInfo;
			return nRet;
		}
		else
		{
			continue;
		}
	}
	delete[] pCodecInfo;
	return nRet;
}
void CPicDownDlg::OnBnClickedLedparaset()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strstreetcob;
	GetDlgItemText(IDC_STREETCOMB,strstreetcob);
    ((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->SetCurSel(2);
	OnCbnSelchangePictypecomb();

	if(strstreetcob.IsEmpty())
	{
		MessageBox(_T("请先连接设备"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	if(IDOK!=m_ledparasetdlg.DoModal())
	{
		return;
	}
	int lansel=m_ledparasetdlg.m_lanselint;
	int msgcount=m_ledparasetdlg.m_mescount;
	if(msgcount>m_msgcount)
	{
		MessageBox(_T("输入的数据大于消息图片数量"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	/////////////////////////////////////////////////////
	BYTE buf[100];
	buf[0]=0x7e;
	buf[1]=0xff;//目的网络号
	buf[2]=0xf6;//目的设备
	buf[3]=0x00;//原网络号
	buf[4]=0x01;//原设备号
	buf[5]=0x04;//cmd字段
	buf[6]=3;
	buf[7]=3;
	buf[8]=lansel;
	buf[9]=msgcount;
	buf[10]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,9);
	buf[11]=0x7e;
	CString strstreetcomb;
	GetDlgItemText(IDC_STREETCOMB,strstreetcomb);
	if(strstreetcomb.Compare(_T("COM"))==0)
	{
		int count;
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,9+buf[6]);
		((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
	}
	else
	{
		BYTE bufusb[100];
		int count;
		USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		bufusb[0]=0x7e;
		bufusb[1]=0x19;
		bufusb[2]=0;
		bufusb[3]=10;
		memcpy(bufusb+4,buf+1,10);
		bufusb[14]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(bufusb,1,13);
		bufusb[15]=0x7e;
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(bufusb,buf[6]+13);
		pSendSet->pPtr=bufusb;
		pSendSet->nLen=count;
	    ((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
		Sleep(200);
		((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
		
		free(pSendSet);

	}

}

void CPicDownDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	//OnCbnSelchangePictypecomb();
	CString str;
	GetDlgItemText(IDC_STREETCOMB,str);
	if(str.IsEmpty())
	{
		

	}
	
}

BOOL CPicDownDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CComboBox *)GetDlgItem(IDC_STREETCOMB))->ResetContent();
    if(((CMainFrame *)AfxGetMainWnd())->m_comisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("COM"));

	}
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
        ((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("CTCUSB"));
	}
	// TODO:  在此添加额外的初始化
    ((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->AddString(_T("站信息"));
	((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->AddString(_T("紧急广播"));
	((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->AddString(_T("消息"));
	((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->SetCurSel(0);
	////////////////////////////////////////////////////////////////////////
	((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->AddString(_T("中文"));
	((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->AddString(_T("英文"));
	((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->SetCurSel(0);
    ///////////////////////////////////////////////////////////////
	((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->AddString(_T("红"));
	((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->AddString(_T("绿"));
	((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->AddString(_T("橙"));
	((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->SetCurSel(0);
	///////////////////////////////////
	SetLatticeSize(64,16);
	OnCbnSelchangePictypecomb();
	////////////////////////////////////////////////////////////////////////
	LoadMyImage(&m_desbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_selectfont, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_impottn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_setsizebtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_clearbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_savebtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_creensetbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_singledownbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));
	LoadMyImage(&m_typedownbtn, _T("\\Image\\Button_Normal.png"), _T("\\Image\\Button_Push.png"), _T("\\Image\\Button_Enter.png"), _T(""));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPicDownDlg::OnCbnSelchangePictypecomb()
{
	// TODO: 在此添加控件通知处理程序代码
	if(GetPicFile())
	{
		SetNOArray();
		//////////////////////////////
		SetNOComb();
	}
	else
	{
		m_Filenamearray.RemoveAll();
		m_NOarray.RemoveAll();
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->ResetContent();
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->ResetContent();

	}
	//////////////////////////////
	

	
	
}
void CPicDownDlg::SetNOComb()
{
	int type=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel();
	int ctid;
	if(type==0)
	{
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->EnableWindow(false);
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->ResetContent();
        ctid=IDC_STATIONNOCOMB;
	}
	if(type==1||type==2)
	{
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->EnableWindow(false);
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->ResetContent();
		ctid=IDC_OTHERNOCOMB;
	}
	for(int i=0;i<m_NOarray.GetCount();i++)
	{
		((CComboBox *)GetDlgItem(ctid))->AddString(m_NOarray.GetAt(i));

	}
	m_msgcount=m_NOarray.GetCount();

}
void CPicDownDlg::SetNOArray()
{
	m_NOarray.RemoveAll();
	int size=m_Filenamearray.GetCount();
	for(int i=0;i<size;i++)
	{
		CString str=m_Filenamearray.GetAt(i);
		int indexbegin=str.Find('_');
		int indexend=str.Find('.');
		CString strNO=str.Mid(indexbegin+1,indexend-indexbegin-1);
		m_NOarray.Add(strNO);
	}
	//////////////////////////////////////////////////
	int *buf=(int *)malloc(size*sizeof(int));
	
	for(int i=0;i<size;i++)
	{
		buf[i]=_ttoi(m_NOarray.GetAt(i));
	}
	//////////////////////////////
	for(int i=0;i<size;i++)
	{
		for(int j=i+1;j<size;j++)
		{
			if(buf[i]>buf[j])
			{
				int data=buf[i];
				buf[i]=buf[j];
				buf[j]=data;
			}
		}
	}
    //////////////////////////////////////
    m_NOarray.RemoveAll();
	for(int i=0;i<size;i++)
	{
		CString str;
		str.Format(_T("%d"),buf[i]);
        m_NOarray.Add(str);
	}
	delete buf;
}
BOOL CPicDownDlg::GetPicFile()
{
	CString typestr,lanstr;
	int type=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel();
	if(type==0)
	{
		typestr=_T("station");
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->EnableWindow(false);
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->EnableWindow(TRUE);
	}
	if(type==1)
	{
		typestr=_T("emg");
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->EnableWindow(false);
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->EnableWindow(TRUE);
	}
	if(type==2)
	{
		typestr=_T("msg");
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->EnableWindow(false);
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->EnableWindow(TRUE);
	}
	/////////////////////////////////////////
    int lan=((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->GetCurSel();
	if(lan==0)
	{
		lanstr=_T("CH");
	}
	if(lan==1)
	{
		lanstr=_T("EN");
	}
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+lanstr+_T("\\")+typestr;
    return GetFileFromDir(path);

}
BOOL CPicDownDlg::GetFileFromDir(CString csDirPath)
{
	m_Filenamearray.RemoveAll();
	csDirPath+=_T("\\*.bmp");
	HANDLE file;
	WIN32_FIND_DATA fileData;
	char line[1024];
	char fn[1000];
	//mbstowcs(fn,csDirPath.GetBuffer(),999);
	file = FindFirstFile(csDirPath.GetBuffer(), &fileData);
	if(file==INVALID_HANDLE_VALUE)
	{
		return false;

	}
	m_Filenamearray.Add(fileData.cFileName);
	bool bState = false;
	bState = FindNextFile(file, &fileData);
	while(bState){
		//wcstombs(line,(const char*)fileData.cFileName,259);
		m_Filenamearray.Add(fileData.cFileName);
		bState = FindNextFile(file, &fileData);
	}
	return TRUE;
}
void CPicDownDlg::OnCbnSelchangePiclancomb()
{
	// TODO: 在此添加控件通知处理程序代码
	if(GetPicFile())
	{
		SetNOArray();
		//////////////////////////////
		SetNOComb();
	}
	else
	{
		m_Filenamearray.RemoveAll();
		m_NOarray.RemoveAll();
		((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->ResetContent();
		((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->ResetContent();

	}
}
void CPicDownDlg::SetLatticeSize(int iWidth, int iHeight)
{
	m_pGrid.EnableDragAndDrop(false);
	try {
		m_pGrid.SetRowCount(iHeight); //设置行数为k行
		m_pGrid.SetColumnCount(iWidth);   //k列
		m_pGrid.SetFixedRowCount(0);   //标题行为一行
		m_pGrid.SetFixedColumnCount(0);  //同上
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
	//m_pGrid.AutoSize();

	//--------------设置行列距------------------
	for(int a=0;a<m_pGrid.GetRowCount();a++)
	{
		m_pGrid.SetRowHeight(a,5);  //设置各行高
	}
	for(int a=0;a<m_pGrid.GetColumnCount();a++)
	{
		m_pGrid.SetColumnWidth(a,5); //设置各列宽
	}
}
void CPicDownDlg::OnBnClickedSetgridwidth()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strValue;
	GetDlgItemText(IDC_GridWidth,strValue);
	/////////////////////////////////////////////////
	//检测非法字符
	int nCount = strValue.GetLength(); // 获得字符个数
	for ( int i = 0; i < nCount; i ++ )
	{
		if ( 0 == isdigit( strValue.GetAt(i) ) ) // 不是数字就置标志位
		{
			MessageBox(_T("必须输入整数数字!"),_T("Prompt"),MB_ICONWARNING);
			return;// 退出
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	if(GetDlgItemInt(IDC_GridHigh)!=16)
	{
		MessageBox(_T("高度不等于16！"),_T("Prompt"),MB_ICONWARNING);
		//UpdateData(false);
		return;
	}
	if(GetDlgItemInt(IDC_GridWidth)%8)
	{
		MessageBox(_T("宽度不能被8整除，请输入8的整数倍数字！"),_T("Prompt"),MB_ICONWARNING);
		//UpdateData(false);
		return;
	}
	UpdateData(TRUE);
	SetLatticeSize(m_gridwidth,m_gridhigh);

}

void CPicDownDlg::OnBnClickedCleargrid()
{
	// TODO: 在此添加控件通知处理程序代码
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	if ((iRowCount> 0) & (iColumnCount > 0))
	{
		m_pGrid.SetRowCount(0);
		m_pGrid.SetColumnCount(0);
	}
	///////////////////////////////////
	OnBnClickedSetgridwidth();
}

void CPicDownDlg::OnBnClickedUp()
{
	// TODO: 在此添加控件通知处理程序代码
	MoveBmp(1);
}

void CPicDownDlg::OnBnClickedDowm()
{
	// TODO: 在此添加控件通知处理程序代码
	MoveBmp(2);
}

void CPicDownDlg::OnBnClickedLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	MoveBmp(3);
}

void CPicDownDlg::OnBnClickedRight()
{
	// TODO: 在此添加控件通知处理程序代码
	MoveBmp(4);
}

void CPicDownDlg::MoveBmp(int direction)//1，2,3,4表示上下左右
{
	int iRowCount, iColumnCount;
	iRowCount = m_pGrid.GetRowCount();
	iColumnCount = m_pGrid.GetColumnCount();

	switch (direction)
	{
	case 1:
		for (int j = 1; j <= (iRowCount - 1); j++)
		{
			for (int i = 0; i <= (iColumnCount - 1); i++)
			{
				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = j;
				Item.col = i;
				Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
				m_pGrid.GetItem(&Item);
				if (Item.crBkClr == RGB(255,0,0))
				{
					DrawPic(j-1,i,1);
				}
				else if(Item.crBkClr == RGB(255,255,255))
				{
					DrawPic(j-1,i,0);
				}
				if (j == (iRowCount - 1))
				{
					DrawPic(j,i,0);
				}
			}
		}
		break;
	case 2:
		for (int j = (iRowCount - 1); j > 0; j--)
		{
			for (int i = (iColumnCount - 1); i >= 0; i--)
			{
				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = j-1;
				Item.col = i;
				Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
				m_pGrid.GetItem(&Item);
				if (Item.crBkClr == RGB(255,0,0))
				{
					DrawPic(j,i,1);
				}
				else if(Item.crBkClr == RGB(255,255,255))
				{
					DrawPic(j,i,0);
				}
				if (j == 1)
				{
					DrawPic(j-1,i,0);
				}		
			}
		}
		break;
	case 3:
		for(int i = 1; i <= (iColumnCount - 1); i++)
		{
			for (int j = 0; j <= (iRowCount - 1); j++)
			{
				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = j;
				Item.col = i;
				Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
				m_pGrid.GetItem(&Item);

				if (Item.crBkClr == RGB(255,0,0))
				{
					DrawPic(j,i-1,1);
				}
				else 
				{
					DrawPic(j,i-1,0);

				}
				if (i == (iColumnCount - 1))
				{
					DrawPic(j,i,0);
				}
			}
		}
		break;
	case 4:
		for (int i = (iColumnCount - 1); i > 0; i--)
		{
			for (int j = (iRowCount - 1); j >= 0; j--)
			{
				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = j;
				Item.col = i-1;
				Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
				m_pGrid.GetItem(&Item);
				if (Item.crBkClr == RGB(255,0,0))
				{
					DrawPic(j,i,1);
				}
				else if(Item.crBkClr == RGB(255,255,255))
				{
					DrawPic(j,i,0);
				}
				if (i== 1)
				{
					DrawPic(j,i-1,0);
				}		
			}
		}
		break;
	}
	Invalidate(false);
}
void CPicDownDlg::DrawPic( int iRow,int iCol,int iFlag )
{
	GV_ITEM Item;
	Item.mask = GVIF_TEXT;
	Item.row = iRow;
	Item.col = iCol;
	COLORREF clr ;
	if (iFlag)
	{
		clr = RGB(255, 0, 0);
	}
	else
	{
		clr = RGB(255, 255, 255);
	}

	Item.crBkClr = clr;               // or - m_Grid.SetItemBkColour(row, col, clr);
	//Item.crFgClr = RGB(255,0,0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
	Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
	m_pGrid.SetItem(&Item);
}
void CPicDownDlg::OnBnClickedDescrible()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sCharText;
	GetDlgItemText(IDC_TXT,sCharText);

	//int nRow = m_pGrid.GetRowCount();
	int nRow=32;
	//int nCol = m_pGrid.GetColumnCount();
	int nCol = 2000;
	////////////////////////////////////////////////
	CClientDC dc(this); 
	//CClientDC dc(this); 
	Bitmap myImage2(nCol, nRow, PixelFormat24bppRGB);   //创建制定大小的BMP位图文件
	Bitmap *myImage=&myImage2;
	//Graphics graphics(dc);
	Graphics *g = Graphics::FromImage(myImage);

	g->Clear(Color::White);           //将新建的图像背景转换为白色

	PointF drawPoint =  PointF(0, 0);                        //二维描绘的坐标

	Gdiplus::SolidBrush SolidBrush2(Gdiplus::Color(255,0,0));

	//获取当前字体类型
	static CFont m_font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));		//added by Amy
	GetDlgItem(IDC_TXT)->GetFont()->GetLogFont(&lf);
	m_font.DeleteObject();
	m_font.CreateFontIndirect(&lf);

	//Gdiplus::FontFamily fontFamily(m_sFontName);
	//Gdiplus::Font myFont(&fontFamily,16,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
	Gdiplus::Font gdiplusFont( dc, m_font ); 
	
	//宽字符转换
	WCHAR   *wChar = NULL; 
	const char *pFilePathName =  (char *)sCharText.Trim().GetBuffer();
	wChar = sCharText.Trim().AllocSysString(); 
	WCHAR string[MAX_PATH*100];
	wcscpy(string, wChar);
	g->DrawString(string, (UINT)wcslen(string),&gdiplusFont,drawPoint,&SolidBrush2);		
    //////////////////////////////////////
	int end=0,begin=0;
	int Hend=0,Hbegin=0;
	BOOL isfirstflag=TRUE;
	//循环描绘点阵
	int width=myImage->GetWidth();
	int higth=myImage->GetHeight();

	// TommyTest
	//g->DrawImage(myImage, 0, 0, myImage->GetWidth(), myImage->GetHeight());
	//return ;

	for (UINT i = 0; i <= (myImage->GetWidth() - 1); i++)
	{
		for (UINT j = 0; j <= (myImage->GetHeight() - 1); j++)
		{
			Color pixelColor;
			myImage->GetPixel(i, j,&pixelColor);

			if (pixelColor.GetValue() == 0xffffffff)
			{
				//DrawPic(j,i,0);
			}
			else if (pixelColor.GetValue() == 0xFFFF0000)
			{
				//DrawPic(j,i,1);
				if(isfirstflag)
				{
					begin=j;
					isfirstflag=false;
				}
				if(j<begin)
					begin=j;
				else if(j>end)
					end=j;
				//////////////////////////////////
				if(Hbegin==0)
					Hbegin=Hend=i;
				else if(i<Hbegin)
					Hbegin=i;
				else if(i>Hend)
					Hend=i;

			}
		}
	}
	if(end-begin>16)
	{
		MessageBox(_T("高度大于16"),_T("Prompt"),MB_ICONWARNING);
		return;

	}
	if(Hend>1880)
	{
		MessageBox(_T("宽度大于2000"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	int mapwith=Hend+1+(8-(Hend+1)%8);
    SetLatticeSize(mapwith,16);
	//SetDlgItemInt(IDC_GridWidth,mapwith);
	//////////////////////////////////////////////////////(myImage->GetWidth() - 1)
	for (UINT i = 0; i <= mapwith-1; i++)
	{
		int len=(16-(end-begin+1))/2;
		if(begin>=len)
		{
			for (UINT j = begin-len; j <= begin-len+15; j++)
			{
				Color pixelColor;
				myImage->GetPixel(i, j,&pixelColor);

				if (pixelColor.GetValue() == 0xffffffff)
				{
					DrawPic(j-begin+len,i,0);
				}
				else if (pixelColor.GetValue() == 0xFFFF0000)
				{
					DrawPic(j-begin+len,i,1);

				}
			}

		}
		else
		{
			for (UINT i = 0; i <= mapwith-1; i++)
			{
				for (UINT j = 0; j <= 15; j++)//////////(myImage->GetHeight() - 1)
				{
					Color pixelColor;
					myImage->GetPixel(i, j,&pixelColor);

					if (pixelColor.GetValue() == 0xffffffff)
					{
						DrawPic(j,i,0);
					}
					else if (pixelColor.GetValue() == 0xFFFF0000)
					{
						DrawPic(j,i,1);
						
					}
				}
			}
			//////////////////////////////////////////////
            for(int k=0;k<len-begin;k++)
			{
				OnBnClickedDowm();

			}
			break;

		}
		
	}
	/*
	int withlength=(mapwith-(Hend-Hbegin+1))/2;
	if(Hbegin>=withlength)
	{
		for(int mu=0;mu<Hbegin-withlength;mu++)
		{
			OnBnClickedLeft();
		}

	}
	else
	{
		for(int mu=0;mu<withlength-Hbegin;mu++)
		{
			OnBnClickedRight();
		}

	}*/
	int widthlat=Hend-Hbegin+1;
	int addwdth=(8-widthlat%8)%8;
	int leftaddwidth=addwdth/2;
	if(Hbegin>leftaddwidth)
	{
		for(int mu=0;mu<Hbegin-leftaddwidth;mu++)
		{
			OnBnClickedLeft();
		}

	}
	else
	{
		for(int mu=0;mu<leftaddwidth-Hbegin;mu++)
		{
			OnBnClickedRight();
		}

	}
	SetLatticeSize(widthlat+addwdth,16);
	SetDlgItemInt(IDC_GridWidth,widthlat+addwdth);
    /////////////////////////////////////////////////////////////

	//释放位图资源
	Invalidate(FALSE);
}

void CPicDownDlg::OnBnClickedChecktext()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_radiotxt)
	{
		((CEdit *)GetDlgItem(IDC_TXT))->EnableWindow(TRUE);
	}
    else
	{
		((CEdit *)GetDlgItem(IDC_TXT))->EnableWindow(false);

	}

}

void CPicDownDlg::OnBnClickedLoadpic()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sFilePathName;
	CString szFilterFDlg = _T("Bitmap (*.bmp)|*.bmp||");
	CFileDialog fileDlg( TRUE, NULL, NULL,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilterFDlg,  NULL );

	if(fileDlg.DoModal() == IDOK)
	{
		sFilePathName = fileDlg.GetPathName();
		ImportStationPic(sFilePathName);
		((CButton *)GetDlgItem(IDC_CHECKTEXT))->SetCheck(0);
		((CEdit *)GetDlgItem(IDC_TXT))->EnableWindow(false);
	}


}
void CPicDownDlg::ImportStationPic( CString strFileName )
{
	char szANSIString [MAX_PATH]; 
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strFileName, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 

	//检测文件是否存在
	FILE *fp = fopen(szANSIString, "r");
	if(NULL == fp)
	{
		m_pGrid.SetRowCount(0);
		m_pGrid.SetColumnCount(0);
		return;
	} 

	WCHAR   *wChar; 
	wChar= strFileName.Trim().AllocSysString();
	//Gdiplus::Bitmap *bmAllImage = new Gdiplus::Bitmap(wChar);
	Bitmap mybit(wChar);

	////设置列宽//设置行高


	//设置列宽、行高
	UINT nWidth = mybit.GetWidth();
	UINT nHeight =mybit.GetHeight();

	//若图片宽度不为8的倍数，将其宽度设为8的倍数
	if(nWidth % 8)
	{
		nWidth = (nWidth/8 + 1)*8;
	}

	//设置网格控件的宽高
	SetLatticeSize( nWidth, nHeight);

	//清除单元格内容
	for(int i = 0;i <= (nWidth - 1); i++)
	{
		for (int j = 0; j <= (nHeight - 1); j++)
		{
			DrawPic(j,i,0);
		}
	}

	//绘制点阵图
	for (int i = 0; i <= (nWidth - 1); i++)
	{
		for (int j = 0; j <= (nHeight - 1); j++)
		{
			Color pixelColor;
			mybit.GetPixel(i, j,&pixelColor);

			if (pixelColor.GetValue() == 0xffffffff)
			{
				DrawPic(j,i,0);
			}
			else 
			{
				DrawPic(j,i,1);
			}
		}
	}

	//释放位图内存
	/*
	if (bmAllImage != NULL)
	{
	delete bmAllImage;
	bmAllImage = NULL;
	}*/
	//关闭文件 
	fclose(fp); 

	Invalidate(FALSE);
}
void CPicDownDlg::OnBnClickedSigledown()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strstreetcob;
	GetDlgItemText(IDC_STREETCOMB,strstreetcob);

	if(strstreetcob.IsEmpty())
	{
		MessageBox(_T("请先连接设备"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	GetBmpByte();
	WORD wPicWidth = (WORD)m_pGrid.GetColumnCount();
	int nBmpCount = m_vBmpByte.size();
	BYTE bXor = m_vBmpByte.at(0);
	for (int i=1;i<nBmpCount;i++)
	{
		bXor = bXor^m_vBmpByte[i];
	}
	WORD wAddCheckSum = 0;
	//计算数据部份的Add和
	for (int i=0;i<nBmpCount; i++ )
	{
		wAddCheckSum = wAddCheckSum + m_vBmpByte.at(i);
	}
	//将校验的三个字节放到图片数据中
	m_vBmpByte.push_back(bXor);
	m_vBmpByte.push_back((BYTE)(wAddCheckSum & 0x00FF));//放入低位
	m_vBmpByte.push_back((BYTE)(wAddCheckSum>>8));//放入高位
	//实际图片数据为nBmpCount + 3
	nBmpCount +=3;
	//////////////////////////////////////////////////////////////////
	//设置数据分包大小200byte
	int nPicMaxPack = 200;
	int nPackCount = 1; //数据分包大小
	if ((nBmpCount % nPicMaxPack) == 0)
	{
		nPackCount = (nBmpCount / nPicMaxPack);//整除
	}
	else 
	{
		nPackCount = (nBmpCount / nPicMaxPack) + 1;
	}
	////////////////////////////////////////////////////////////////////////////////////
	int index=0;
	for(int i=0;i<nPackCount;i++)
	{
		//USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		BYTE buf[255],bufusb[300];
		memset(buf,0,255);
		buf[0]=0x7e;
		buf[1]=0xff;//目的网络号
		buf[2]=0xf6;//目的设备
		buf[3]=0x00;//原网络号
		buf[4]=0x01;//原设备号
		buf[5]=0x04;//cmd字段
		if(i+1<nPackCount)//应用层数据长度
			buf[6]=8+nPicMaxPack;
		else
			buf[6]=8+nBmpCount-(nPackCount-1)*nPicMaxPack;
		buf[7]=2;//升级类型
		bFlag bmyFlag;
		bmyFlag.bEn=((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->GetCurSel();//。。。。。。。。。。。。。。。。。。。。。语言需要通过获取
		if (1 == nPackCount)
		{
			bmyFlag.bLastPack = 1;
			bmyFlag.bStart = 1;
			bmyFlag.bFirstPack = 1;
			bmyFlag.bStop = 1;
		}
		else if (i == 0)
		{
			bmyFlag.bFirstPack = 1;
			bmyFlag.bLastPack = 0;
			bmyFlag.bStart = 1;
			bmyFlag.bStop = 0;
		}
		else if (i == nPackCount-1)
		{
			bmyFlag.bFirstPack = 0;
			bmyFlag.bLastPack = 1;
			bmyFlag.bStart = 0;
			bmyFlag.bStop = 1;
		}
		else
		{
			bmyFlag.bFirstPack = 0;
			bmyFlag.bLastPack = 0;
			bmyFlag.bStart = 0;
			bmyFlag.bStop = 0;
		}
		//buf[8]=bmyFlag.bFirstPack*128+bmyFlag.bLastPack*64+bmyFlag.bStart*32+bmyFlag.bStop*16+bmyFlag.bEn*8;//标志位
        buf[8]=bmyFlag.bFirstPack*4+bmyFlag.bLastPack*8+bmyFlag.bStart+bmyFlag.bStop*2+bmyFlag.bEn*16;
		buf[9]=i+1;//当前包
		buf[10]=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel()+1;//类型。。。。。。。。。。。。。。。。。。。。。。需要读取
		if(((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->IsWindowEnabled())
			buf[11]=GetDlgItemInt(IDC_STATIONNOCOMB);
		else
			buf[11]=GetDlgItemInt(IDC_OTHERNOCOMB);
		if(buf[11]==0)
		{
			MessageBox(_T("图片序号不正确"),_T("Prompt"),MB_ICONWARNING);
			return;
		}
		//图片序号。。。。。。。。。。。。。。。。。。。。。。。需要读取

		wPicWidth = (wPicWidth/8)*8;//必须为8的整数倍
		buf[12] = (BYTE)(0x00FF & wPicWidth);
		buf[13] = BYTE(wPicWidth>>8);
		buf[14]=((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->GetCurSel()+1;//颜色。。。。。。。。。。。。。。。。。。。。。需要读取
		for(int j=0;j<buf[6]-8;j++)
		{
			buf[15+j]=m_vBmpByte.at(index++);
		}
		/////////////////////////////////计算校原和。
        buf[7+buf[6]]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,6+buf[6]);
		buf[8+buf[6]]=0x7e;
		CString strstreetcomb;
		GetDlgItemText(IDC_STREETCOMB,strstreetcomb);
		if(strstreetcomb.Compare(_T("COM"))==0)
		{
			int count;
			count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,9+buf[6]);
			((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
		}
		else
		{
			int count;
			USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
			bufusb[0]=0x7e;
			bufusb[1]=0x19;
			bufusb[2]=(buf[6]+7)>>8;
			bufusb[3]=buf[6]+7;
			memcpy(bufusb+4,buf+1,buf[6]+7);
			bufusb[buf[6]+11]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(bufusb,1,buf[6]+10);
			bufusb[buf[6]+12]=0x7e;
			count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(bufusb,buf[6]+13);
			pSendSet->pPtr=bufusb;
			pSendSet->nLen=count;
			((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
			if(bmyFlag.bStart == 1)
			{
				Sleep(200);
				((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
			}
			free(pSendSet);

		}
		/*
		if(bmyFlag.bStart == 1)
		{
			Sleep(200);
			((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);

		}*/
		
		/*pSendSet->pPtr=buf;
		pSendSet->nLen=count;
		((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
		free(pSendSet);*/
	}
}
byte Littlebag(byte b7,byte b6,byte b5,byte b4,byte b3,byte b2,byte b1,byte b0)
{
	byte bResult;
	bResult = 0x00;
	bResult = b0 | bResult;
	bResult = (b1 << 1) | bResult;
	bResult = (b2 << 2) | bResult;
	bResult = (b3 << 3) | bResult;
	bResult = (b4 << 4) | bResult;
	bResult = (b5 << 5) | bResult;
	bResult = (b6 << 6) | bResult;
	bResult = (b7 << 7) | bResult;
	return bResult;
}

void CPicDownDlg::GetBmpByte()//获取点阵数据
{
	m_vBmpByte.clear();
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	//判断点阵区域是否设置完毕
	if ((iRowCount == 0) && (iColumnCount == 0))
	{
		return;
	}
	int ictrl;//从左到右扫描
	byte bArry[8] ={0}; 
	for (int i = 0; i <= (iRowCount - 1); i++)
	{
		for (int j = 0; j < (iColumnCount / 8); j++)
		{
			ictrl = 0;
			do
			{
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = i;
				Item.col = j * 8 + ictrl;
				Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
				m_pGrid.GetItem(&Item);
				if (Item.crBkClr == RGB(255,0,0)) //点阵为红色
				{
					bArry[7-ictrl] = 1; 
					ictrl++;        
				}
				else
				{
					bArry[7-ictrl] = 0; 
					ictrl++;                
				}

			}while(ictrl <= 7);
			m_vBmpByte.push_back(Littlebag(bArry[7], bArry[6], bArry[5], bArry[4], bArry[3], bArry[2], bArry[1], bArry[0]));
		}   
	}

}
void CPicDownDlg::OnBnClickedButton13()
{
	// TODO: 在此添加控件通知处理程序代码
   int isel=((CComboBox *)GetDlgItem(IDC_PICCOLORCOMB))->GetCurSel()+1;
   int streetint=((CComboBox *)GetDlgItem(IDC_STREETCOMB))->GetCurSel();
   m_downbytypedlg.SetColorint(isel,streetint);
   m_downbytypedlg.DoModal();
}

void CPicDownDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	GetDlgItem(IDC_TXT)->GetFont()->GetLogFont(&lf);
	//使用按钮的当前字体初始化字体对话框
	CFontDialog dlgFontDlg(&lf);
	//CFontDialog dlgFontDlg;
	//显示字体选择对话框
	if (dlgFontDlg.DoModal() == IDOK)
	{
		//如果用户在字体选择对话框中单击了“确定”按钮
		//则将按钮ID_BUTTON_DEMODE的标题文本字体设置为所选定的字体

		dlgFontDlg.GetCurrentFont(&lf);
		static CFont m_font;
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
		GetDlgItem(IDC_TXT)->SetFont(&m_font);
	}
}

void CPicDownDlg::OnCbnSelchangeStationnocomb()
{
	// TODO: 在此添加控件通知处理程序代码
	CString picindex;
	int isel=((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_STATIONNOCOMB))->GetLBText(isel,picindex);
	
	
	CString typestr,lanstr;
	int type=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel();
	if(type==0)
	{
		typestr=_T("station");
		
	}
	if(type==1)
	{
		typestr=_T("emg");
		
	}
	if(type==2)
	{
		typestr=_T("msg");
		
	}
	/////////////////////////////////////////
	int lan=((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->GetCurSel();
	if(lan==0)
	{
		lanstr=_T("CH");
	}
	if(lan==1)
	{
		lanstr=_T("EN");
	}
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+lanstr+_T("\\")+typestr+_T("\\")+typestr+_T("_")+picindex+_T(".bmp");
    ImportStationPic(path);


}

void CPicDownDlg::OnCbnSelchangeOthernocomb()
{
	// TODO: 在此添加控件通知处理程序代码
	CString picindex;
	int isel=((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_OTHERNOCOMB))->GetLBText(isel,picindex);


	CString typestr,lanstr;
	int type=((CComboBox *)GetDlgItem(IDC_PICTYPECOMB))->GetCurSel();
	if(type==0)
	{
		typestr=_T("station");

	}
	if(type==1)
	{
		typestr=_T("emg");

	}
	if(type==2)
	{
		typestr=_T("msg");

	}
	/////////////////////////////////////////
	int lan=((CComboBox *)GetDlgItem(IDC_PICLANCOMB))->GetCurSel();
	if(lan==0)
	{
		lanstr=_T("CH");
	}
	if(lan==1)
	{
		lanstr=_T("EN");
	}
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+lanstr+_T("\\")+typestr+_T("\\")+typestr+_T("_")+picindex+_T(".bmp");
	ImportStationPic(path);
}

void CPicDownDlg::OnCbnSelchangeStreetcomb()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CPicDownDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CComboBox *)GetDlgItem(IDC_STREETCOMB))->ResetContent();
	if(((CMainFrame *)AfxGetMainWnd())->m_comisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("COM"));

	}
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("CTCUSB"));
	}
	// TODO: 在此处添加消息处理程序代码
}

HBRUSH CPicDownDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		//pDC->SelectObject(&m_font);
		//pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
		//return m_SliderBrush;

	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

BOOL CPicDownDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

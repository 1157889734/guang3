// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "SZPTU.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(200, OnLinkDev)
	ON_COMMAND(201, OnParaSet)
	ON_COMMAND(202, OnPicUpdata)
	ON_COMMAND(203, OnProgramUpdata)
	ON_COMMAND(204, OnBreakQuery)
	ON_COMMAND(205, OnTMS)
	ON_COMMAND(206, OnTMSQuery)
	ON_COMMAND(207, OnBreakQueryTime)
	ON_COMMAND(208, OnSetRoute)
	ON_COMMAND(209, OnStationInfo)
	ON_MESSAGE(WM_RECV_USB_DATA,&CMainFrame::OnRecvUsbData)
	ON_MESSAGE(WM_RECV_COM_DATA,&CMainFrame::OnRecvComData)
	ON_MESSAGE(WM_USB_CHECKERROR,&CMainFrame::OnUsbCheckError)
	ON_MESSAGE(WM_ADD_MSG, &CMainFrame::OnAddMsg)
	ON_MESSAGE(WM_DELETE_MSG, &CMainFrame::OnDeleteMsg)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	isfileopen=false;
	m_nRecCntUsb=0;
	m_nRecCntComm=0;
	m_nRecCntUsbATC=0;
	m_ptctcusb=NULL;
	m_ptatcusb=NULL;
	m_ctcusbisopen=false;
	m_atcusbisopen=false;
	m_comisopen=false;
	isatcbufempty=TRUE;
	m_nCurrCTCCmdType = 0;
	//file3.Open(_T("ALLDATA.TXT"),CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
}

CMainFrame::~CMainFrame()
{
}
CString CMainFrame::GetAppPath()
{
	CString strAppPath;
	::GetModuleFileName(NULL, strAppPath.GetBuffer(256), 256);
	strAppPath.ReleaseBuffer();
	int nPos = strAppPath.ReverseFind('\\');
	strAppPath = strAppPath.Left(nPos);
	SetCurrentDirectory(strAppPath);

	return strAppPath;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CImageList img;
	CString str;
	if(!m_wndReBar.Create(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		return -1;
	}
	if (!m_wndToolBar.CreateEx(this))
	{
		return -1;      
	}

	//����Ĵ��������ð�ť�Ŀ�Ⱥͳ���
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(80,120);
	//����Ĵ���������"��"��λͼ
	HINSTANCE hInstance =AfxGetInstanceHandle( );

	img.Create(25+10, 25+10, ILC_COLOR32|ILC_MASK,2,2);
	img.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\�����豸.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\LEDͼ������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��ʷ���ݲ�ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
//	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\�豸״̬��ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\ʵʱ���ϲ�ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\imgFont.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\station.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//

	m_wndToolBar.GetToolBarCtrl().SetHotImageList(&img);
	img.Detach();	

	//����Ĵ���������"��"��λͼ
	img.Create(23+10, 23+10, ILC_COLOR16|ILC_MASK, 2,2);
	img.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\�����豸.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\LEDͼ������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��������.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\��ʷ���ݲ�ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
// 	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\�豸״̬��ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\ʵʱ���ϲ�ѯ.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\imgFont.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	img.Add((HICON)LoadImage(hInstance, GetAppPath()+_T("\\Image\\station.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));//
	//img.Add(AfxGetApp()->LoadIcon(IDI_ICON6));//
	

	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();

	//�ı�����
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT |CBRS_TOOLTIPS | TBSTYLE_TRANSPARENT|TBBS_CHECKBOX );
	m_wndToolBar.SetButtons(NULL,8);	
	// set up each toolbar button�������� 
	int nIndexPage = 0;
	m_wndToolBar.SetButtonInfo(nIndexPage, 200, TBSTYLE_BUTTON, 0);
	m_wndToolBar.SetButtonText(nIndexPage++, _T("�����豸"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 201, TBSTYLE_BUTTON, 1);
	m_wndToolBar.SetButtonText(nIndexPage++,  _T("��������"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 203, TBSTYLE_BUTTON, 3);
	m_wndToolBar.SetButtonText(nIndexPage++,  _T("��������"));	

	m_wndToolBar.SetButtonInfo(nIndexPage, 204, TBSTYLE_BUTTON, 4);
	m_wndToolBar.SetButtonText(nIndexPage++, _T("���ݲ�ѯ"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 206, TBSTYLE_BUTTON, 5);
	m_wndToolBar.SetButtonText(nIndexPage++,_T("״̬��ѯ"));

//	m_wndToolBar.SetButtonInfo(nIndexPage, 207, TBSTYLE_BUTTON, 6);
// 	m_wndToolBar.SetButtonText(nIndexPage++, _T("���ϲ�ѯ"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 208, TBSTYLE_BUTTON, 2);
	m_wndToolBar.SetButtonText(nIndexPage++,_T("��·����"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 202, TBSTYLE_BUTTON, 6);
	m_wndToolBar.SetButtonText(nIndexPage++,  _T("�ֿ�����"));

	m_wndToolBar.SetButtonInfo(nIndexPage, 209, TBSTYLE_BUTTON, 7);
	m_wndToolBar.SetButtonText(nIndexPage++,  _T("վ����Ϣ"));

	
	CRect rectToolBar;
	//�õ���ť�Ĵ�С
	m_wndToolBar.GetItemRect(0, &rectToolBar);
	//���ð�ť�Ĵ�С
	m_wndToolBar.SetSizes(rectToolBar.Size(), CSize(20+10,23+10));
	m_wndReBar.AddBar(&m_wndToolBar);

	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(rbbi);		//����Ǳ�����
	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE|RBBIM_BACKGROUND;
	//�������Ŀ��
	rbbi.cxMinChild = rectToolBar.Width();
	//�߶�
	rbbi.cyMinChild = rectToolBar.Height();
	//�������д�����Ϊ���������뱳��λͼ����ע����rbbi.fMask��RBBIM_BACKGROUND��־
	rbbi.hbmBack = LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));

	rbbi.cx = rbbi.cxIdeal = rectToolBar.Width() * 10;

	m_wndReBar.GetReBarCtrl().SetBandInfo(0, &rbbi);

	//////////////////////////////////////////////////////////////
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
    /////////////////////////////////////////////
	
	CMenu *pMenu = GetMenu();  
	if (NULL!=pMenu)  
	{  
	   DelAllMenu(pMenu->GetSafeHmenu());  
    }  
	
	// TODO: �������Ҫ��������ͣ������ɾ��������
	ReadCfgFromIni();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.hMenu = NULL;//ȥ���˵�
	cs.style&=~WS_MAXIMIZEBOX; //���ð�����󻯰�ť 
	cs.style&=~WS_THICKFRAME;//��ֹ�û��ı䴰�ڴ�С

	return TRUE;
}


// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnLinkDev()
{
	if(m_portlinkdlg.m_hWnd==NULL)
	{
		m_portlinkdlg.Create(IDD_PORTLINK_DLG,this);
	}
	m_portlinkdlg.ShowWindow(SW_SHOW);
	m_portlinkdlg.BringWindowToTop();
	m_portlinkdlg.CenterWindow(this);

}
void CMainFrame::OnParaSet()
{
	if(m_parasetdlg.m_hWnd==NULL)
	{
		m_parasetdlg.Create(IDD_PARASETDLG,this);
	}
	m_parasetdlg.ShowWindow(SW_SHOW);
	m_parasetdlg.BringWindowToTop();
	m_parasetdlg.CenterWindow(this);

}

void CMainFrame::OnPicUpdata()
{	
	if(m_picdowndlg.m_hWnd==NULL)
	{
		//m_picdowndlg.Create(IDD_PICDOWNDLG,this);
		m_picdowndlg.Create(IDD_DIALOG_LATTICE,this);
	}
	if(m_cStationCfg.m_hWnd==NULL)
	{
		m_cStationCfg.Create(IDD_DIALOG_STATION_CFG,this);
	}
	m_picdowndlg.ShowWindow(SW_SHOW);
	m_picdowndlg.BringWindowToTop();
	m_picdowndlg.CenterWindow(this);

}

void CMainFrame::OnStationInfo()
{
	if(m_cStationCfg.m_hWnd==NULL)
	{
		m_cStationCfg.Create(IDD_DIALOG_STATION_CFG,this);
	}
	if(m_picdowndlg.m_hWnd==NULL)
	{
		m_picdowndlg.Create(IDD_DIALOG_LATTICE,this);
	}
	m_cStationCfg.ShowWindow(SW_SHOW);
	m_cStationCfg.BringWindowToTop();
	m_cStationCfg.CenterWindow(this);
}

void CMainFrame::OnProgramUpdata()
{
	if(m_programdlwndlg.m_hWnd==NULL)
	{
		m_programdlwndlg.Create(IDD_PRGDOWNDLG,this);
	}
	m_programdlwndlg.SetUsbComPt(m_ptctcusb,m_ptatcusb,&m_com);
	m_programdlwndlg.ShowWindow(SW_SHOW);
	m_programdlwndlg.BringWindowToTop();
	m_programdlwndlg.CenterWindow(this);
}

void CMainFrame::OnBreakQuery()
{
	if(m_devstatusquerydlg.m_hWnd==NULL)
	{
		m_devstatusquerydlg.Create(IDD_BREAKQUERYDLG,this);
	}
	m_devstatusquerydlg.ShowWindow(SW_SHOW);
	m_devstatusquerydlg.BringWindowToTop();
	m_devstatusquerydlg.CenterWindow(this);

}

void CMainFrame::OnTMS()
{

}
void CMainFrame::OnTMSQuery()
{
	
	if(m_DeviceQueryCTCDlg.m_hWnd==NULL)
	{
		m_DeviceQueryCTCDlg.Create(IDD_DEVICE_QUERY_CTC,this);
	}
	m_DeviceQueryCTCDlg.ShowWindow(SW_SHOW);
	m_DeviceQueryCTCDlg.BringWindowToTop();
	m_DeviceQueryCTCDlg.CenterWindow(this);

}
void CMainFrame::OnBreakQueryTime()
{
	
    if(m_BreakQueryTimeDlg.m_hWnd==NULL)
	{
		m_BreakQueryTimeDlg.Create(IDD_BREAKQUERYTIME,this);
	}
	m_BreakQueryTimeDlg.ShowWindow(SW_SHOW);
	m_BreakQueryTimeDlg.BringWindowToTop();
	m_BreakQueryTimeDlg.CenterWindow(this);

}

void CMainFrame::OnSetRoute()
{
	if (m_dlgCtcSetRout.GetSafeHwnd() == NULL)
	{
		m_dlgCtcSetRout.Create(IDD_DLG_CTC_SET_ROUTING,this);
	}
	m_dlgCtcSetRout.ShowWindow(SW_SHOW);
	m_dlgCtcSetRout.BringWindowToTop();
	m_dlgCtcSetRout.CenterWindow(this);
}


void CMainFrame::DelAllMenu(HMENU hMenu)
{
	int Menucount = GetMenuItemCount(hMenu);  
	for (int i = Menucount-1;i>-1;i--)  
    {  
	    DeleteMenu(hMenu,i, MF_BYPOSITION);  
	}
}

bool CMainFrame::OpenCTCUsb(void)
{
	m_ptctcusb = new CHidUSB(this, 0x04B4, 0x0000);	//�ڴ�й© deleted by Amy 2012-9-17
	bool bl= m_ptctcusb->OpenDevice();
	m_ctcusbisopen=bl;
	return bl;
}

bool CMainFrame::OpenATCUsb(void)
{
	m_ptatcusb = new CHidUSB(this, 0x04b4, 0x5720);	//�ڴ�й© deleted by Amy 2012-9-17
	bool bl= m_ptatcusb->OpenDevice();
	m_atcusbisopen=bl;
	return bl;
}
LRESULT CMainFrame::OnUsbCheckError( WPARAM wParam,LPARAM lParam)
{
	MessageBox(_T("����У��ʧ��"),_T("Prompt"),MB_ICONWARNING);
	return 1;
}
LRESULT CMainFrame::OnRecvComData( WPARAM wParam,LPARAM lParam)
{
	ASSERT(&m_com == (CHDLCSeries*)wParam);
	COMM_DATAPACK *pptr = (COMM_DATAPACK *) lParam;			//???�ܷ���ȷ��ȡ����
    BYTE buf[200];
	memset(buf,0,200);
	memcpy(buf,pptr->pPtr,pptr->nLen);
	for (int i = 0; i < pptr->nLen; i++)
	{
		if (0 == m_nRecCntComm)
		{
			if (0x7E == *(pptr->pPtr + i))
			{
				if (m_nRecCntComm >= 512)
				{
					return 0;
				}
				m_bRecBufferComm[m_nRecCntComm++] = *(pptr->pPtr + i);

			} 
			else
			{
				continue;
			}
		}
		else
		{
			if (m_nRecCntComm >= 512 )
			{
				return 0;
			}
			m_bRecBufferComm[m_nRecCntComm++] = *(pptr->pPtr + i);

			if (0x7E == *(pptr->pPtr + i))//��������
			{
				BYTE *tempBuf = (BYTE *)malloc(m_nRecCntComm);
				memset(tempBuf, 0, m_nRecCntComm);
				memcpy(tempBuf, m_bRecBufferComm,m_nRecCntComm); 

				//������Ϣ
				//SendMessage(WM_PROC_COM_DATA, (WPARAM)tempBuf, (LPARAM)m_nRecCntComm);//����ֱ���ú���
				OnProcComData(tempBuf,m_nRecCntComm);
				memset(m_bRecBufferComm, 0, sizeof(m_bRecBufferComm));
				m_nRecCntComm = 0;
				if(tempBuf != NULL)
				{
					free(tempBuf);
					tempBuf = NULL;
				}

			}
		}
	}

	return 1;
}
int CMainFrame::OnProcComData(BYTE *tempBuf,int bufcount)//����0����ʾЧ��ʧ�ܣ�1��ʾִ�гɹ�
{
	BYTE bAdd = 0; //�ۼӺ� 
	BYTE *pData = (BYTE *)malloc(bufcount); //////////�����õĶ�������������档����Ϊidx��
	int idx = 0; 
	for (int i = 0; i < bufcount; i++) 
	{ 
		if (0x7F == *(tempBuf + i) && 0x80 == *(tempBuf + i + 1)) 
		{ 
			pData[idx++] = 0x7E; 
			i++; 
		} 
		else if (0x7F == *(tempBuf + i) && 0x81 == *(tempBuf + i + 1)) 
		{ 
			pData[idx++] = 0x7F; 
			i++; 
		} 
		else 
		{ 
			pData[idx++] = *(tempBuf + i); 
		} 
	} 

	for (int i = 1; i < idx - 2; i++) 
	{ 
		bAdd += pData[i]; 
	} 
	bAdd = 0x55 - bAdd; 
	if (bAdd != pData[idx-2])//�ۼӺʹ��� 
	{ 
		return 0; 
	} 
	switch (*(pData + 5))
	{
	case CMD_VERSION:		//�汾������ʱ���ѯ	
		RecVerLcuState(pData,idx);
		break;


	default:
		break;
	}
	//�ͷ��ڴ�
	if (pData != NULL)
	{
		free(pData);
		pData = NULL;
	}	 
	return 1;
}
LRESULT CMainFrame::OnRecvUsbData( WPARAM wParam,LPARAM lParam)
{
   
	//ASSERT(m_pusb == (CHidUSB *)wParam);
    CHidUSB *ptusb=(CHidUSB *)wParam;
	USB_DATAPACK *pptr = (USB_DATAPACK *) lParam;			//???�ܷ���ȷ��ȡ����
    if(m_ptctcusb==ptusb)
	{
		
		for (int i = 0; i < pptr->nLen; i++)
		{
			if (0 == m_nRecCntUsb)
			{
				if (0x7E == *(pptr->pPtr + i))
				{
					if (m_nRecCntUsb >= 512)
					{
						return 0;
					}
					m_bRecBufferUsb[m_nRecCntUsb++] = *(pptr->pPtr + i);

				} 
				else
				{
					continue;
				}
			}
			else
			{
				if (m_nRecCntUsb >= 512 )
				{
					return 0;
				}
				m_bRecBufferUsb[m_nRecCntUsb++] = *(pptr->pPtr + i);

				if (0x7E == *(pptr->pPtr + i))//��������
				{

					BYTE *tempBuf = (BYTE *)malloc(m_nRecCntUsb);
					memset(tempBuf, 0, m_nRecCntUsb);
					memcpy(tempBuf, m_bRecBufferUsb,m_nRecCntUsb); 

					//������Ϣ
					//SendMessage(WM_PROC_COM_DATA, (WPARAM)tempBuf, (LPARAM)m_nRecCntComm);//����ֱ���ú���
					OnProcUsbData(tempBuf,m_nRecCntUsb);

					memset(m_bRecBufferUsb, 0, sizeof(m_bRecBufferUsb));
					m_nRecCntUsb = 0;
					if(tempBuf != NULL)
					{
						free(tempBuf);
						tempBuf = NULL;
					}

				}
			}
		}

	}
	 if(m_ptatcusb==ptusb)
	 {
		 /*
        MessageBox(_T(""));
		 char *buf=(char *)malloc(9);
		 memset(buf,0,9);
		 memcpy(buf,pptr->pPtr,8);
		 buf[8]='\0';
		 USES_CONVERSION;
		 CString str = A2T(buf);
		 m_programdlwndlg.ShowRichMsg(str);
		 free(buf);
		 buf = NULL;*/
		 
		 /*
		 file3.SeekToEnd();
		 file3.Write(pptr->pPtr,pptr->nLen);*/
		 
		 /////////////////////////
		 memcpy(m_bRecBufferUsbATC+m_nRecCntUsbATC,pptr->pPtr,pptr->nLen);
		 m_nRecCntUsbATC+=pptr->nLen;
		 int usbcountindex=0;
		 if(m_bRecBufferUsbATC[0]!=0x7e)
		 {
			 if(m_nRecCntUsbATC>0)
			 {
				 for(int mn=0;mn<m_nRecCntUsbATC;mn++)
				 {
					 if(m_bRecBufferUsbATC[mn]==0x7e)
					 {
						usbcountindex=mn;
						break;
					 }
				 }
				 ///////////////////////////////////////////////////////////////
				 if(usbcountindex==0)
				 {
					 m_nRecCntUsbATC=0;
					 return 0;
				 }
				 else
				 {
					 int kt=0;
					 for(int nj=usbcountindex;nj<m_nRecCntUsbATC;nj++)
					 {
						 m_bRecBufferUsbATC[kt]=m_bRecBufferUsbATC[nj];
						 kt++;
					 }
					 m_nRecCntUsbATC=kt;

				 }
				
			 }
			 else
			 {
				 m_nRecCntUsbATC=0;
				 return 0;
			 }
			
		 }
		 if(m_nRecCntUsbATC>=700)//��������������
		 {
			 m_nRecCntUsbATC=0;
			 return 0;
		 }
		 if(m_bRecBufferUsbATC[1]+4<=m_nRecCntUsbATC)
		 {
			 BYTE *tempBuf = (BYTE *)malloc(m_bRecBufferUsbATC[1]+4);
			 memset(tempBuf, 0, m_bRecBufferUsbATC[1]+4);
			 memcpy(tempBuf, m_bRecBufferUsbATC,m_bRecBufferUsbATC[1]+4); 
			 ///////////////////////////////////////////////////////////////////����У׼��
			 BYTE checksum=VerifyFun(tempBuf,1,m_bRecBufferUsbATC[1]+1);
			 if(checksum!=tempBuf[m_bRecBufferUsbATC[1]+2]||tempBuf[m_bRecBufferUsbATC[1]+3]!=0x7e)
			 {
				 /////////////////////////////////////////////д�뱾���ļ���ͬʱֹͣ���䡣
				 /*
                 CFile file2;
				 file2.Open(_T("ERROR.TXT"),CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
				 file2.Write(tempBuf,m_bRecBufferUsbATC[1]+4);
				 file2.Close();
				 m_devstatusquerydlg.m_selfiledlg.OnBnClickedDownloadcancle();
				 exit(0);*/


				 //////////////////////////////////////////////
				 int next7eindex=0;
				 for(int mn=1;mn<m_nRecCntUsbATC;mn++)
				 {
					 if(m_bRecBufferUsbATC[mn]==0x7e)
					 {
						 next7eindex=mn;
						 break;
					 }
				 }
				 if(next7eindex==0)
				 {
					 m_nRecCntUsbATC=0;
					
				 }
				 else
				 {
					 int kt=0;
					 for(int nj=next7eindex;nj<m_nRecCntUsbATC;nj++)
					 {
						 m_bRecBufferUsbATC[kt]=m_bRecBufferUsbATC[nj];
						 kt++;
					 }
					 m_nRecCntUsbATC=kt;
				 }

                 //PostMessage(WM_USB_CHECKERROR, NULL, NULL);
				

			 }
			 else
			 {
				 OnProcUsbDataATC(tempBuf,m_bRecBufferUsbATC[1]+4);//���������������������������������޸�
				 int k=0;
				 for(int m=m_bRecBufferUsbATC[1]+4;m<m_nRecCntUsbATC;m++)
				 {
					 m_bRecBufferUsbATC[k]=m_bRecBufferUsbATC[m];
					 k++;
				 }
				 m_nRecCntUsbATC=k;
			 }
			 
			 if(tempBuf != NULL)
			 {
				 free(tempBuf);
				 tempBuf = NULL;
			 }
		 }
         ////////////////////////////////////////////////
		 
		 
	 }

	return 1;
}
int CMainFrame::OnProcUsbData(BYTE *tempBuf,int bufcount)
{
	BYTE bAdd = 0; //�ۼӺ� 
	BYTE *pData = (BYTE *)malloc(bufcount); //////////�����õĶ�������������档����Ϊidx��
	int idx = 0; 
	for (int i = 0; i < bufcount; i++) 
	{ 
		if (0x7F == *(tempBuf + i) && 0x80 == *(tempBuf + i + 1)) 
		{ 
			pData[idx++] = 0x7E; 
			i++; 
		} 
		else if (0x7F == *(tempBuf + i) && 0x81 == *(tempBuf + i + 1)) 
		{ 
			pData[idx++] = 0x7F; 
			i++; 
		} 
		else 
		{ 
			pData[idx++] = *(tempBuf + i); 
		} 
	} 

	for (int i = 1; i < idx - 2; i++) 
	{ 
		bAdd += pData[i]; 
	} 
	bAdd = 0x55 - bAdd; 
	if (bAdd != pData[idx-2])//�ۼӺʹ��� 
	{ 
		return 0; 
	} 
	switch (*(pData + 1))
	{

	case CMD_VOLUME_QUERY://�����ض�
		RecVolumeInfo(pData,idx);
		break;
	case CMD_BC_PRIOR_QUERY://���ȼ�ˢ��
		RecPriorInfo(pData,idx);
		break;
	case CMD_ERROR_CODE_QUERY://�豸״̬�ض�,�쳣��ѯ
		RecEquipmentState(pData,idx);
		break;
	case CMD_VER_CTC_QUERY://�汾��ѯ��ͨ��ctc
		RecVerCtcState(pData,idx);
		break;
	case CMD_SET_SUCCESS://����Ӧ��
		if (CMD_SET_CTC_ROUTE == m_nCurrCTCCmdType)
		{
			m_nCurrCTCCmdType = 0; // ��ǰָ����� ����ָ���
			m_dlgCtcSetRout.SendRouteSetting();
			break;
		}
		MessageBox(_T("���óɹ���"),_T("��ʾ"),MB_OK);
		break;
	case CMD_QUERY_CTC_ROUTE: // ��·����
		RecCtcSetRoute(pData, idx);
	case CMD_QUERY_BCBOXVOLUME:
		{
			RecBroadCastBoxVolumeInfo(pData, idx);
			break;
		}
	default: 

		break;
	}
	//�ͷ��ڴ�
	if (pData != NULL)
	{
		free(pData);
		pData = NULL;
	}	 
	return 1;
}
void CMainFrame::RecVolumeInfo(BYTE *tempBuf,int bufcount)
{
	m_parasetdlg.RecVolumeInfo(tempBuf,bufcount);

}
void CMainFrame::RecEquipmentState(BYTE *tempBuf,int bufcount)
{
	m_DeviceQueryCTCDlg.RecStateInfo(tempBuf,bufcount);

}
void CMainFrame::RecPriorInfo(BYTE *tempBuf,int bufcount)
{
	m_parasetdlg.Refleshpri(tempBuf,bufcount);

}
void CMainFrame::RecVerCtcState(BYTE *tempBuf,int bufcount)
{
	m_programdlwndlg.RefleshVersionUSB(tempBuf,bufcount);

}
void CMainFrame::RecVerLcuState(BYTE *tempBuf,int bufcount)
{
	m_programdlwndlg.RefleshVersion(tempBuf,bufcount);

}

void CMainFrame::RecCtcSetRoute(BYTE *tempBuf,int bufcount)
{
	m_dlgCtcSetRout.UpdateRouteList(tempBuf, bufcount);
}

void CMainFrame::RecBroadCastBoxVolumeInfo(BYTE *tempBuf, int bufcount)
{
	m_parasetdlg.RecBroadCastBoxVolumeInfo(tempBuf, bufcount);
}

BYTE CMainFrame::VerifyFun(BYTE * buf, int beginint , int endint)
{
	BYTE sum=0;
	for(int i=beginint;i<=endint;i++)
	{
		sum+=buf[i];

	}
	return 0x55-sum;
	
}
int CMainFrame::SwitchSendData(BYTE * buf, int count)
{
	int nIndex =0;
    BYTE nbuf[300];
	memset(nbuf, 0, 300);
	//BYTE *pSourcePack = (BYTE *)pRec->pPtr;

	nbuf[nIndex++] = 0x7E;
	//�Դӵڶ����ֽڿ�ʼ�������ڶ����ֽڵ����ݽ���ת�����
	for (int i = 1; i < count -1; i++)
	{
		if(0x7E == buf[i])
		{
			nbuf[nIndex++] = 0x7F;
			nbuf[nIndex++] = 0x80;
		}
		else if (0x7F == buf[i])
		{
			nbuf[nIndex++] = 0x7F;
			nbuf[nIndex++] = 0x81;
		}
		else
		{
			nbuf[nIndex++] = buf[i];
		}
	}
	nbuf[nIndex++] = 0x7E;
    memcpy(buf,nbuf,nIndex);
	return nIndex;
}

bool CMainFrame::OpenComm(int com)
{
	BOOL flg;
	m_com.SetOwner(this);
	flg=m_com.OpenPort(com);
	m_comisopen=flg;
	return flg;
}

void CMainFrame::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
    if(m_ctcusbisopen)
	{
		m_ptctcusb->CloseDevice();
	}
	if(m_atcusbisopen)
	{
		m_ptatcusb->CloseDevice();
	}
	if(m_comisopen)
	{
		m_com.ClosePort();
	}
	CFrameWnd::OnClose();
}

void CMainFrame::OnProcUsbDataATC(BYTE * tempBuf, int count)
{
    CString strprogram,strtimemsg,strtime;
	
	switch (tempBuf[2])
	{
	    case 0x07://ʱ���ѯ
			strtime.Format(_T("20%d-%d-%d %d:%d:%d"),tempBuf[8],tempBuf[7],tempBuf[6],tempBuf[5],tempBuf[4],tempBuf[3]);
			m_devstatusquerydlg.m_rtctimesetdlg.ShowRecTimeMsg(strtime);
			break;
	    case 0x06://ʱ���趨
			if(tempBuf[3]==0x5A)
				strtimemsg=_T("���óɹ�");
			if(tempBuf[3]==0x55)
				strtimemsg=_T("����ʧ��");
            m_devstatusquerydlg.m_rtctimesetdlg.ShowMsg(strtimemsg);
			break;
	    case 0x01://��ѯ����
			
			strprogram.Format(_T("%02X %02X %02X"),tempBuf[3],tempBuf[4],tempBuf[5]);
            m_programdlwndlg.ShowRichMsg(strprogram);
			
			break;
 		case 0x02://�汾��ѯ��
 			{
 				char *ptbuf;
 				ptbuf=(char *)malloc(tempBuf[1]);
 				memset(ptbuf,0,tempBuf[1]);
 				memcpy(ptbuf,tempBuf+3,tempBuf[1]-1);
 				USES_CONVERSION;
 				CString str = A2T(ptbuf);
 				m_programdlwndlg.ShowRichMsg(str);
 
 			}
 			
 			break;

		case 0x09://��ѯ�ļ���Ϣ
			if(tempBuf[3]==1)
			{
				if(tempBuf[4]==0x5a)
					m_filebufarray.Add(_T("sd���в���׼����"));
				if(tempBuf[4]==0x5c)
					m_filebufarray.Add(_T("sd��δ����"));
				if(tempBuf[4]==0x5b)
                    m_filebufarray.Add(_T("sd��δ����δ׼����"));
				//////////////////////////////////////////////////////
				if(tempBuf[5]==0x5a)
					m_filesize.Add(_T("д����"));
				else
					m_filesize.Add(_T("δд����"));
			}
			if(tempBuf[3]==2)
			{
				CString str;
				UINT sizeint;
				sizeint=tempBuf[4]*16*1024*1024+tempBuf[5]*64*1024+tempBuf[6]*256+tempBuf[7];
				str.Format(_T("������%d"),sizeint);
				m_filebufarray.Add(str);
				//////////////////////////////////////////////////////
				sizeint=tempBuf[8]*16*1024*1024+tempBuf[9]*64*1024+tempBuf[10]*256+tempBuf[11];
				str.Format(_T("��ʣ������%d"),sizeint);
				m_filesize.Add(str);

			}
			if(tempBuf[3]==4)
			{
				char *buf=(char *)malloc(count+1);
				memcpy(buf,tempBuf+8,tempBuf[1]-6);
				buf[tempBuf[1]-6]='\0';
				USES_CONVERSION;
				CString str = A2T(buf);
				m_filebufarray.Add(str);
				free(buf);
				buf=NULL;
				//////////////////////////////////////////////////////////////
				UINT sizeint;
				sizeint=tempBuf[4]*16*1024*1024+tempBuf[5]*64*1024+tempBuf[6]*256+tempBuf[7];
				str.Format(_T("%d"),sizeint);
                m_filesize.Add(str);
				
			}
			if(tempBuf[3]==5)
			{
				
				::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_SELECT, NULL, NULL);//�յ��ļ���Ϣ

			}
			break;
		    ////////////////////////////////////////////////////////////
		case 0x0a://���ص����ļ�
			if(tempBuf[3]==1)
			{
				char *buf=(char *)malloc(count+1);
				memcpy(buf,tempBuf+6,count-8);
				buf[count-8]='\0';
				USES_CONVERSION;
				CString str = A2T(buf);
				if(str.Compare(_T("0:/TIME.TXT"))==0 || str.Compare(_T("0:/TCMS.TXT"))==0
					|| str.Compare(_T("0:/PISC.TXT"))==0)
				{
					CString path=GetAppPath()+_T("\\DATA");
					if(!PathIsDirectory(path))
					{
						CreateDirectory(GetAppPath()+_T("\\DATA"),NULL);
						
					}
                    int leftindex=str.Find('/');
					CString filename=str.Mid(leftindex+1,str.GetLength()-leftindex-1);
					path+=_T("\\")+filename;
					m_filepath=path;
					file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
					isfileopen=TRUE;
					free(buf);
					buf=NULL;

				}
				else
				{
					int leftindex=str.Find('/');
					int rightindex=str.Find('/',leftindex+1);
					CString strmsg=str.Mid(leftindex+1,rightindex-leftindex-1);
					CString filename=str.Mid(rightindex+1,str.GetLength()-rightindex-1);
					/////////////////////////////////////////////////////////////
					CString path=GetAppPath()+_T("\\DATA")+_T("\\")+strmsg;
					if(!PathIsDirectory(path))
					{
						CreateDirectory(GetAppPath()+_T("\\DATA"),NULL);
						CreateDirectory(GetAppPath()+_T("\\DATA")+_T("\\")+strmsg,NULL);

					}
					path+=_T("\\")+filename;
					m_filepath=path;
					/////////////////////////////////////////
					file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
					isfileopen=TRUE;
					free(buf);
					buf=NULL;

				}
				
			}
			if(tempBuf[3]==2)
			{
				if (isfileopen)
				{
					file.Write(tempBuf+6,count-8);
					////////////////////////////////////////////////////////////
					int countsend=count-8;
					::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_DOWN_DATA,  WPARAM(countsend), NULL);//�ļ���������

				}
				
			}
			if(tempBuf[3]==3)
			{
				if(isfileopen)
				{
					file.Close();
					isfileopen=false;
				}
				int flag=1;
				::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_DOWN_FINISH,  WPARAM(flag), NULL);//�ļ��������
				
			}
			if(tempBuf[3]==5)
			{
				int flag=2;
				::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_DOWN_FINISH,  WPARAM(flag), NULL);//�ļ�����ʹ��

			}
			if(tempBuf[3]==6)
			{
				int flag=4;
				::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_DOWN_FINISH,  WPARAM(flag), NULL);//TMS���ݴ����У���������

			}
			break;
		case 0x0b:
			if(tempBuf[3]==0x5a)
			{
				if(isfileopen)
				{
					file.Close();
					isfileopen=false;
				}
				int flag=3;
				::SendMessage(m_devstatusquerydlg.m_selfiledlg.m_hWnd, WM_FILE_DOWN_FINISH,  WPARAM(flag), NULL);//�ļ�ȡ���ɹ�
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                DeleteFile(m_filepath); 

			}
			break;
		case 0x08:
			if(tempBuf[3]==0x01)
			{
				BYTE bufdevicestatus[3];
				bufdevicestatus[0]=tempBuf[4];
				bufdevicestatus[1]=tempBuf[5];
				bufdevicestatus[2]=tempBuf[6];
				::SendMessage(m_BreakQueryTimeDlg.m_hWnd, WM_DEVICE_STATUS, WPARAM((BYTE *)bufdevicestatus), NULL);//����ģ����Ϣ
				break;
			}
			if(tempBuf[6]==0x33/*0x02*/)
			{
				BYTE *pttmsbuf=(BYTE *)malloc(count);
				memcpy(pttmsbuf,tempBuf,count);
				::SendMessage(m_BreakQueryTimeDlg.m_hWnd, WM_TMS_INFO, WPARAM((BYTE *)pttmsbuf), NULL);//����TMS��Ϣ
                free(pttmsbuf);
				pttmsbuf=NULL;
				break;
			}
			if(tempBuf[6]==0xCC/*0x03*/)
			{
				BYTE *pttmsbuf=(BYTE *)malloc(count);
				memcpy(pttmsbuf,tempBuf,count);
				::SendMessage(m_BreakQueryTimeDlg.m_hWnd, WM_PIS_INFO, WPARAM((BYTE *)pttmsbuf), NULL);//����PIS��Ϣ
				free(pttmsbuf);
				pttmsbuf=NULL;
				break;
			}
			if(tempBuf[3]==0x04)
			{
				BYTE *pttmsbuf=(BYTE *)malloc(tempBuf[1]-6);
				memcpy(pttmsbuf,tempBuf+4,tempBuf[1]-6);
				::SendMessage(m_BreakQueryTimeDlg.m_hWnd, WM_485_INFO, WPARAM((BYTE *)pttmsbuf), NULL);//����485��Ϣ
				free(pttmsbuf);
				pttmsbuf=NULL;
				break;
			}
			break;

	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent==WM_TIME_CHECKCTCUSB)
	{
		if(m_ctcusbisopen)
		{
			if (!m_ptctcusb->CheckDevice())
			{
				KillTimer(WM_TIME_CHECKCTCUSB);
				MessageBox(_T("CTC USB ͨ��ʧ��!"), _T("Prompt"), MB_ICONWARNING);
				delete m_ptctcusb;		//added by amy 2012-8-15
				m_ptctcusb = NULL;
				m_ctcusbisopen = FALSE;		//added by amy 2012-8-15
				((CButton *)(m_portlinkdlg.GetDlgItem(IDC_CTCUSB)))->SetWindowText(_T("����")); //added by amy 2012-8-15
				//exit(0);
			}
		}

	}
	if(nIDEvent==WM_TIME_CHECKATCUSB)
	{
		if(m_atcusbisopen)
		{
			if (!m_ptatcusb->CheckDevice())
			{
				KillTimer(WM_TIME_CHECKATCUSB);
				MessageBox(_T("ATC USB ͨ��ʧ��!"), _T("Prompt"), MB_ICONWARNING);
				delete m_ptatcusb;		//added by amy 2012-8-15
				m_ptatcusb = NULL;
				m_atcusbisopen = FALSE;		//added by amy 2012-8-15
				((CButton *)(m_portlinkdlg.GetDlgItem(IDC_ATCUSB)))->SetWindowText(_T("����")); //added by amy 2012-8-15
				//exit(0);
			}

		}

	}
	
	
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::ReadCfgFromIni()
{
	CString szValue;
	CString tmpStr;
	CString strIni = _T("\\PTU.ini");
	strIni = theApp.GetAppPath() + strIni;

	//���vector����
	m_vLineStationName[0].clear();
	m_vLineStationName[1].clear();
	m_vLineStationName[2].clear();

	m_vLineTipName.clear();
	m_vLineBcName.clear();
	m_vLineAdName.clear();
	m_vAddTipsName.clear();

	GetPrivateProfileString(_T("StationsInfo"), _T("StationLine"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
	GetPrivateProfileString(_T("StationsInfo"), _T("StationLineCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);

	//char szANSIString [MAX_PATH]; 
	//WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, szValue, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	int m_nStationLineCount = _ttoi(szValue);
	for (int i = 0; i<m_nStationLineCount; ++i)
	{
		CString csStationLine,csStationNum;
		csStationLine.Format(_T("StationLine%d"),i+1);
		GetPrivateProfileString(csStationLine, _T("StationCount"), _T(""),  szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
		szValue.ReleaseBuffer();

		int nStationCount = _ttoi(szValue);
		for (int j=0; j<nStationCount; j++)
		{
			csStationNum.Format(_T("Station%d"),j);
			GetPrivateProfileString(csStationLine, csStationNum, _T(""),  szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
			szValue.ReleaseBuffer();
			int nPos = szValue.Find(_T(";"));
			XString xStationName;
			if (nPos != -1)
			{
				xStationName[LAN_EN] = szValue.Left(nPos); //Ӣ��
				xStationName[LAN_IRAN] = szValue.Mid(nPos+1);//������
				/*
				CString strrighttoleft;
				CString striran=szValue.Mid(nPos+1);
				int leniran=striran.GetLength();
				for(int i=0;i<leniran;i++)
				{
				CString bstr=striran.Mid(leniran-1-i,1);
				strrighttoleft+=bstr;
				}
				xStationName[LAN_IRAN] =strrighttoleft;*/
				m_vLineStationName[i].push_back(xStationName);
			}
		}		
	}

	//��ȡ��ʾ��
	GetPrivateProfileString(_T("TIP"), _T("TipsCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
	szValue.ReleaseBuffer();
	int nTipCout = _ttoi(szValue);

	for (int i=0 ;i<nTipCout; i++)
	{
		tmpStr.Format(_T("Tips%d"),i);
		GetPrivateProfileString(_T("TIP"), tmpStr, _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
		szValue.ReleaseBuffer();
		XString xTips;
		int nPos = szValue.Find(_T(";"));
		if (nPos != -1)
		{
			xTips[LAN_EN] = szValue.Left(nPos);
			xTips[LAN_IRAN] = szValue.Mid(nPos+1);
			m_vLineTipName.push_back(xTips);
		}
	}

	//��ȡ���ӵ���ʾ��
	GetPrivateProfileString(_T("ExtraTips"), _T("TipsCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
	szValue.ReleaseBuffer();
	int nExtraTipsCount = _ttoi(szValue);
	for (int i=0 ;i<nExtraTipsCount; i++)
	{
		tmpStr.Format(_T("Tips%d"),i);
		GetPrivateProfileString(_T("ExtraTips"), tmpStr, _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
		szValue.ReleaseBuffer();
		XString xExtraTips;
		int nPos = szValue.Find(_T(";"));
		if (nPos != -1)
		{
			xExtraTips[LAN_EN] = szValue.Left(nPos);
			xExtraTips[LAN_IRAN] = szValue.Mid(nPos+1);
			m_vAddTipsName.push_back(xExtraTips);
		}
	}

	//��ȡ���
	GetPrivateProfileString(_T("AD"), _T("AdCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
	szValue.ReleaseBuffer();
	int nAdCount = _ttoi(szValue);
	for (int i=0 ;i<nAdCount; i++)
	{
		tmpStr.Format(_T("Ad%d"),i);
		GetPrivateProfileString(_T("AD"), tmpStr, _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
		szValue.ReleaseBuffer();
		XString xAd;
		int nPos = szValue.Find(_T(";"));
		if (nPos != -1)
		{
			xAd[LAN_EN] = szValue.Left(nPos);
			xAd[LAN_IRAN] = szValue.Mid(nPos+1);
			m_vLineAdName.push_back(xAd);
		}
	}

	//��ȡ�㲥
	GetPrivateProfileString(_T("BROADCAST"), _T("BroadCastCount"), _T(""), szValue.GetBuffer(MAX_PATH), MAX_PATH, strIni);
	szValue.ReleaseBuffer();
	int nBcCount = _ttoi(szValue);
	for (int i=0 ;i<nBcCount; i++)
	{
		tmpStr.Format(_T("BroadCast%d"),i);
		GetPrivateProfileString(_T("BROADCAST"), tmpStr, _T(""), szValue.GetBuffer(1024), 1024, strIni);
		szValue.ReleaseBuffer();
		XString xBc;
		int nPos = szValue.Find(_T(";"));
		if (nPos != -1)
		{
			xBc[LAN_EN] = szValue.Left(nPos);
			xBc[LAN_IRAN] = szValue.Mid(nPos+1);
			m_vLineBcName.push_back(xBc);
		}
	}
}

LRESULT CMainFrame::OnAddMsg(WPARAM wParam, LPARAM lParam)
{
	ReadCfgFromIni();
	m_picdowndlg.SendMessage(WM_ADD_MSG, NULL, NULL);
	m_cStationCfg.SendMessage(WM_ADD_MSG, NULL, NULL);
	return 0L;
}

LRESULT CMainFrame::OnDeleteMsg(WPARAM wParam, LPARAM lParam)
{
	ReadCfgFromIni();
	m_picdowndlg.SendMessage(WM_DELETE_MSG, NULL, NULL);
	m_cStationCfg.SendMessage(WM_DELETE_MSG, NULL, NULL);
	return 0L;
}

void CMainFrame::AddHeadAndTail(BYTE *pData, int nLen, BOOL bNeed7E)
{
	if(nLen > USB_BUFFER_SIZE)
	{
		return;
	}

	int nIndex =0;
	memset(m_bUsbSendBuffer,0,USB_BUFFER_SIZE);

	if (!bNeed7E)
	{
		m_bUsbSendBuffer[nIndex++] = 0x7E;
		for (int i = 1; i < (nLen-1) ; i++)
		{
			if (0x7E == *(pData+i))
			{
				m_bUsbSendBuffer[nIndex++] = 0x7F;
				m_bUsbSendBuffer[nIndex++] = 0x80;
			}
			else if (0x7F == *(pData+i))
			{
				m_bUsbSendBuffer[nIndex++] = 0x7F;
				m_bUsbSendBuffer[nIndex++] = 0x81;
			}
			else
			{
				m_bUsbSendBuffer[nIndex++] = *(pData+i);
			}
		}
		m_bUsbSendBuffer[nIndex++] = 0x7E;
		m_nSendCnt = nIndex;
	}
	else
	{
		m_bUsbSendBuffer[nIndex++] = 0x7E;				

		for (int i = 0; i < nLen ; i++)
		{
			if (0x7E == *(pData+i))
			{
				m_bUsbSendBuffer[nIndex++] = 0x7F;
				m_bUsbSendBuffer[nIndex++] = 0x80;
			}
			else if (0x7F == *(pData+i))
			{
				m_bUsbSendBuffer[nIndex++] = 0x7F;
				m_bUsbSendBuffer[nIndex++] = 0x81;
			}
			else
			{
				m_bUsbSendBuffer[nIndex++] = *(pData+i);
			}
		}
		m_bUsbSendBuffer[nIndex++] = 0x7E;
		m_nSendCnt = nIndex;
	}
}

//����USB����
BOOL CMainFrame::SendUSBData(LPUSB_DATAPACK pPack)
{
	//DWORD viewTest = (DWORD)m_cHidUSB;
	m_ptctcusb->SendData(pPack);

	return TRUE;
}

BOOL CMainFrame::SendHdlcComPack( LPCOMM_DATAPACK lpCOM_DATAPACK )
{
	if (!m_comisopen)
	{
		return FALSE;
	}
	BOOL bRet = m_com.WriteHDLC(lpCOM_DATAPACK->pPtr,lpCOM_DATAPACK->nLen, TRUE);

	//CString strShow;
	//CString strTemp;
	//for (int i=0;i<lpCOM_DATAPACK->nLen;i++)
	//{
	//	strTemp.Format(_T("%02X "),*(lpCOM_DATAPACK->pPtr +i));
	//	strShow += strTemp;
	//}	
	return bRet;
}
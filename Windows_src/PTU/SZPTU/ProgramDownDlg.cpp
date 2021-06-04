// ProgramDownDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SZPTU.h"
#include "ProgramDownDlg.h"
#include "MainFrm.h"

// CProgramDownDlg �Ի���

IMPLEMENT_DYNAMIC(CProgramDownDlg, CDialog)

CProgramDownDlg::CProgramDownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgramDownDlg::IDD, pParent)
{
	m_pFileBuffer = NULL;

}

CProgramDownDlg::~CProgramDownDlg()
{
	if (m_pFileBuffer != NULL)
	{
		delete[]m_pFileBuffer;
	}
}

void CProgramDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DOWNPROGRESS, m_procDownload);
	DDX_Control(pDX, IDC_RICHEDIT21, m_rich);
}


BEGIN_MESSAGE_MAP(CProgramDownDlg, CDialog)
	ON_BN_CLICKED(IDC_VERQUERY, &CProgramDownDlg::OnBnClickedVerquery)
	ON_BN_CLICKED(IDC_SELECTFILE, &CProgramDownDlg::OnBnClickedSelectfile)
	ON_BN_CLICKED(IDC_BEGINDOWN, &CProgramDownDlg::OnBnClickedBegindown)
	ON_MESSAGE(WM_BEGINEND_DDATA,&CProgramDownDlg::OnSendBeginEndData)
	ON_MESSAGE(WM_MID_DDATA,&CProgramDownDlg::OnSendMidData)
	ON_MESSAGE(WM_ATC_DATA,&CProgramDownDlg::DownLoadATCProgram)
	
	ON_BN_CLICKED(IDC_ATCQUERY, &CProgramDownDlg::OnBnClickedAtcquery)
	ON_CBN_SELCHANGE(IDC_STREETCOMB, &CProgramDownDlg::OnCbnSelchangeStreetcomb)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CProgramDownDlg::OnBnClickedButton1)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CProgramDownDlg ��Ϣ�������
DWORD WINAPI CProgramDownDlg::WorkThread( LPVOID lParam )
{
	CProgramDownDlg *pDownLoadWnd = (CProgramDownDlg*)lParam;
	ASSERT(NULL != pDownLoadWnd);	
	pDownLoadWnd->DownLoadProgram();//����
	return 0;
}
void CProgramDownDlg::DownLoadProgram()
{
	if(m_bType==0)
	{
		DownLoadATCProgram(NULL,NULL);
		//SendMessage(WM_ATC_DATA,NULL,NULL);
	}
	else
	{
		DownLoadLEDMAPProgram();
	}
    GetDlgItem(IDC_BEGINDOWN)->EnableWindow(TRUE);

}
void CProgramDownDlg::OnBnClickedVerquery()
{
	ClearVersion();
	CString strmsg;
	GetDlgItemText(IDC_STREETCOMB,strmsg);
	if(strmsg.Compare(_T("COM"))==0)
	{
		BYTE count;

		BYTE sendbuf[100]={0x7e,0xff,0xff,0xff,0xff,0x05,0x00};

		sendbuf[7]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(sendbuf,1,6);
		sendbuf[8]=0x7e;
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(sendbuf,9);

		((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(sendbuf,count);

	}
	else if(strmsg.Compare(_T("CTC USB"))==0)
	{
		BYTE Send[7];
		USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		memset(Send, 0, 7);
		memset(pSendSet, 0, sizeof(USB_DATAPACK));

		Send[0] = 0x7E;    //��ʼ��
		Send[1] = 0x30;    //У��ʱ��ض�����
		Send[2] = 0x00;    //���ݳ���1�ֽ�
		Send[3] = 0x01;    //���ݳ���2�ֽ�
		Send[4] = 0xff;
		Send[5] = 0x55-(0x30+0x01+0xff);   //������
		Send[6]=0x7e;

		pSendSet->nLen = 7;
		pSendSet->pPtr = Send;
		((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
		free(pSendSet);

	}
	else
	{
		MessageBox(_T("��ѡ��COM����CTC USBͨ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//DownLoadStartEnd(1);

}

void CProgramDownDlg::OnBnClickedSelectfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog openFile(TRUE, NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Bin Files (*.bin)|*.bin|"),NULL,0);
	if (IDOK == openFile.DoModal())
	{
		CString binFilePath = openFile.GetPathName();
		GetDlgItem(IDC_EDIT_UPDATE_PATH)->SetWindowText(binFilePath);
	}
}

void CProgramDownDlg::OnBnClickedBegindown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strstreetcomb,strtypecomb;
    GetDlgItemText(IDC_STREETCOMB,strstreetcomb);
	GetDlgItemText(IDC_TYPECOMB,strtypecomb);
	if(strstreetcomb.IsEmpty())
	{
		MessageBox(_T("���ȴ�����"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	////////////////////////////////////
	if(strtypecomb.IsEmpty())
	{
		MessageBox(_T("����ѡ����������"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	if(strstreetcomb.Compare(_T("COM"))==0&&strtypecomb.Compare(_T("ATCģ��"))==0)
	{
		MessageBox(_T("ѡ������ͺ�ͨ����ƥ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	if(strstreetcomb.Compare(_T("CTC USB"))==0&&strtypecomb.Compare(_T("ATCģ��"))==0)
	{
		MessageBox(_T("ѡ������ͺ�ͨ����ƥ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	if(strstreetcomb.Compare(_T("ATC USB"))==0&&strtypecomb.Compare(_T("������"))==0)
	{
		MessageBox(_T("ѡ������ͺ�ͨ����ƥ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	if(strstreetcomb.Compare(_T("ATC USB"))==0&&strtypecomb.Compare(_T("��̬��ͼ"))==0)
	{
		MessageBox(_T("ѡ������ͺ�ͨ����ƥ��"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	/////////////////////////////////////////
	CString strUpdateFile,strMsg;
	GetDlgItem(IDC_EDIT_UPDATE_PATH)->GetWindowText(strUpdateFile);
	if (strUpdateFile.IsEmpty())
	{
		MessageBox(_T("��ѡ�������ļ���"), _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	HANDLE hFile = CreateFile(strUpdateFile,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		//strMsg.Format(_T("Can't Open The Bin File %s ,Please Check!!!"),strUpdateFile);
		strMsg.Format(_T("��ָ��·��%s�ļ�ʧ�ܣ����飡"),strUpdateFile);
		MessageBox(strMsg, _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	DWORD  dwFileLen = 0;
	dwFileLen = GetFileSize(hFile,NULL);
	if (dwFileLen <=4)
	{
		//strMsg.Format(_T("File Size Error!!!"));
		strMsg.Format(_T("�ļ�̫С����"));
		MessageBox(strMsg, _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	if (m_pFileBuffer != NULL)
	{
		delete []m_pFileBuffer;
		m_pFileBuffer = NULL;
	}
	m_pFileBuffer = new BYTE[dwFileLen+4];
	ZeroMemory(m_pFileBuffer,dwFileLen+4);
	DWORD NumberOfBytesRead = 0;
	ReadFile(hFile,m_pFileBuffer,dwFileLen,&NumberOfBytesRead,NULL);

	if (NumberOfBytesRead != dwFileLen)
	{
		//strMsg.Format(_T("Read File Error!!!"));
		strMsg.Format(_T("��ȡ�ļ�����"));
		MessageBox(strMsg, _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	int nMode = dwFileLen % 4;
	if (nMode != 0)
	{
		for (int i = 0 ;i< 4 - nMode;i++)
		{
			m_pFileBuffer[dwFileLen + i] = 0xFF;
		}
		dwFileLen += (4 - nMode);//�ļ��ܳ�������
	}
	m_nDownloadSize = dwFileLen; //��Ҫ���ص��ֽ���
	if(hFile != NULL)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	int iLedType = ((CComboBox*)GetDlgItem(IDC_TYPECOMB))->GetCurSel();
	switch(iLedType)
	{
	case 0://������
		{
			m_bType = 0xf6;
			break;
		}
	case 1://��̬��ͼ
		{
			m_bType = 0xf4;
			break;
		}
	case 2://ATCģ��
		{
			m_bType=0x00;
			break;
		}
	case 3://���ؿ�����
		{
			m_bType = 0x03;
			break;
		}
	case 4://�㲥����ģ��
		{
			m_bType = 0x02;
			break;
		}
	default:
		{
			break;
		}
	}

	DWORD dwID; 
	m_hThread = CreateThread(NULL,0,WorkThread,this,CREATE_SUSPENDED,&dwID);
	GetDlgItem(IDC_BEGINDOWN)->EnableWindow(FALSE);
	ResumeThread(m_hThread);//����
	//////////////////////////////////////////////////
}

BOOL CProgramDownDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    ((CComboBox *)GetDlgItem(IDC_STREETCOMB))->ResetContent();
    
	///////////////////////////////////////////////////////////////////////////
	if(((CMainFrame *)AfxGetMainWnd())->m_comisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("COM"));

	}
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("CTC USB"));
	}
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("ATC USB"));

	}
	///////////////////////////////////////////////////////////////////////////////
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->ResetContent();
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("������"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("��̬��ͼ"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("ATCģ��"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("���ؿ�����"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("�㲥����ģ��"));
	
	
	//////////////////////////////////////
    ((CComboBox *)GetDlgItem(IDC_KSNO))->ResetContent();
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("1"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("2"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("3"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("4"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("5"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->AddString(_T("6"));
	((CComboBox *)GetDlgItem(IDC_KSNO))->SetCurSel(0);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

LRESULT CProgramDownDlg::DownLoadATCProgram(WPARAM wParam,LPARAM lParam)
{
	DownLoadAtcStartEnd(1);

	////////////////////////////////////////////
	Sleep(150);
	int nPicMaxPack=200;//ÿ����С
	int nPackCount=0;//����С

	///////////////////////////////////////
	if ((m_nDownloadSize % nPicMaxPack) == 0)
	{
		nPackCount = (m_nDownloadSize / nPicMaxPack);//����
	}
	else 
	{
		nPackCount = (m_nDownloadSize / nPicMaxPack) + 1;
	}
	m_procDownload.SetRange(0,nPackCount);
	int index=0;
    for(int i=0;i<nPackCount;i++)
	{
		USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		BYTE buf[300];
		
		buf[0]=0x7e;
		if(i+1<nPackCount)//���ݳ���
			buf[1]=4+nPicMaxPack;
		else
			buf[1]=4+m_nDownloadSize-(nPackCount-1)*nPicMaxPack;
		buf[2]=0x01;
		buf[3]=0x02;
		buf[4]=(i+1)>>8;
		buf[5]=i+1;
		for(int j=0;j<buf[1]-4;j++)
		{
			buf[6+j]=m_pFileBuffer[index++];
		}
		buf[2+buf[1]]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,buf[1]+1);
		buf[3+buf[1]]=0x7e;
		int count;
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,4+buf[1]);
		pSendSet->pPtr=buf;
		pSendSet->nLen=count;
		//((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
		m_ptatcusb->SendData2(pSendSet);
		free(pSendSet);
		m_procDownload.SetPos(i+1);
        //Sleep(100);

	}
	////////////////////////////////////////
	DownLoadAtcStartEnd(3);
	MessageBox(_T("�������"));
    return 1;
}

void CProgramDownDlg::DownLoadLEDMAPProgram(void)
{
	// /����ʼ��
	int flag=1;
	//SendMessage(WM_BEGINEND_DDATA,(WPARAM)(int *)&flag,NULL);
	DownLoadStartEnd(1);
	Sleep(3000);
	//SendMessage(WM_BEGINEND_DDATA,(WPARAM)(int *)&flag,NULL);
	DownLoadStartEnd(1);
	Sleep(3000);
   ///////////////////////////////////////////////////
   // SendMessage(WM_MID_DDATA,NULL,NULL);
	OnSendMidData(NULL,NULL);
	//////////////////////////////////////////////////////
	flag=3;
	//SendMessage(WM_BEGINEND_DDATA,(WPARAM)(int *)&flag,NULL);
	DownLoadStartEnd(3);
	// /��������
	////////////////////////////////////////////////////////
	MessageBox(_T("�������"));

}

void CProgramDownDlg::DownLoadStartEnd(int flag)
{
	int nPicMaxPack=100;
	int count;
	BYTE buf[300];
	BYTE bufusb[100];
	buf[0]=0x7e;
	buf[1]=0xff;
	buf[2]=m_bType;//Ŀ���豸��
	buf[3]=0x00;
	buf[4]=0x01;
	buf[5]=0x04;//CMD
	buf[6]=8;//Ӧ�ò�����
	buf[7]=1;//��������
	buf[8]=flag;//������־
	buf[9] = m_nDownloadSize;
	buf[10] = m_nDownloadSize>>8;
	buf[11]= m_nDownloadSize>>16;
	buf[12] = m_nDownloadSize>>24;
	buf[13]=0;
	buf[14]=0;
	buf[15]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,14);
	buf[16]=0x7e;
	CString strstreetsel;
	GetDlgItemText(IDC_STREETCOMB,strstreetsel);
	if(strstreetsel.Compare(_T("CTC USB"))==0)
	{
		USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
		bufusb[0]=0x7e;
		bufusb[1]=0x19;
		bufusb[2]=0x00;
		bufusb[3]=15;
		memcpy(bufusb+4,buf+1,15);////////////////////////////////////�޸�
		bufusb[19]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(bufusb,1,18);
		bufusb[20]=0x7e;
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(bufusb,21);
		pSendSet->pPtr=bufusb;
		pSendSet->nLen=count;
		//((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
		m_ptctcusb->SendData(pSendSet);
		free(pSendSet);

	}
	else if(strstreetsel.Compare(_T("COM"))==0)
	{
		count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,17);

		//((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
		m_ptcom->WritePort(buf,count);
	}
	
}

void CProgramDownDlg::RefleshVersion(BYTE * tempBuf, int bufcount)
{
	//////////////////////////////////////////////
	CString str;
	str.Format(_T("%02X"),tempBuf[7]);
	SetDlgItemText(IDC_MAPEDIT1,str);
	str.Format(_T("%02X"),tempBuf[8]);
	SetDlgItemText(IDC_MAPEDIT2,str);
	str.Format(_T("%02X"),tempBuf[9]);
	SetDlgItemText(IDC_MAPEDIT3,str);
	str.Format(_T("%02X"),tempBuf[10]);
	SetDlgItemText(IDC_MAPEDIT4,str);
	str.Format(_T("%02X"),tempBuf[11]);
	SetDlgItemText(IDC_MAPEDIT5,str);
	str.Format(_T("%02X"),tempBuf[12]);
	SetDlgItemText(IDC_MAPEDIT6,str);
	str.Format(_T("%02X"),tempBuf[13]);
	SetDlgItemText(IDC_MAPEDIT7,str);
	str.Format(_T("%02X"),tempBuf[14]);
	SetDlgItemText(IDC_MAPEDIT8,str);
	str.Format(_T("%02X"),tempBuf[15]);
	SetDlgItemText(IDC_MAPEDIT9,str);
	str.Format(_T("%02X"),tempBuf[16]);
	SetDlgItemText(IDC_MAPEDIT10,str);
	//////////////////////////////////////////////
	str.Format(_T("%02X"),tempBuf[17]);
	SetDlgItemText(IDC_LEDEDIT1,str);
	str.Format(_T("%02X"),tempBuf[18]);
	SetDlgItemText(IDC_LEDEDIT2,str);
	str.Format(_T("%02X"),tempBuf[19]);
	SetDlgItemText(IDC_LEDEDIT3,str);
	str.Format(_T("%02X"),tempBuf[20]);
	SetDlgItemText(IDC_LEDEDIT4,str);
	
}
void CProgramDownDlg::RefleshVersionUSB(BYTE * tempBuf, int bufcount)
{
	
    int index=((CComboBox *)GetDlgItem(IDC_KSNO))->GetCurSel();
	CString str;
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT1,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT2,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT3,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT4,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT5,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT6,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT7,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT8,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT9,str);
	str.Format(_T("%02X"),tempBuf[4+5*index]);
	SetDlgItemText(IDC_MAPEDIT10,str);
	//////////////////////////////////////////////
	str.Format(_T("%02X"),tempBuf[5+5*index]);
	SetDlgItemText(IDC_LEDEDIT1,str);
	str.Format(_T("%02X"),tempBuf[6+5*index]);
	SetDlgItemText(IDC_LEDEDIT2,str);
	str.Format(_T("%02X"),tempBuf[7+5*index]);
	SetDlgItemText(IDC_LEDEDIT3,str);
	str.Format(_T("%02X"),tempBuf[8+5*index]);
	SetDlgItemText(IDC_LEDEDIT4,str);
	

}
void CProgramDownDlg::DownLoadAtcStartEnd(int flag)
{
	int count=0;
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	BYTE buf[300];
	buf[0]=0x7e;
	buf[1]=8;
	buf[2]=0x01;
	buf[3]=flag;
	buf[4]=0;
	buf[5]=0;
	buf[9] = m_nDownloadSize;
	buf[8] = m_nDownloadSize>>8;
	buf[7]= m_nDownloadSize>>16;
	buf[6] = m_nDownloadSize>>24;
	buf[10]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,9);
	buf[11]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,12);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	//CHidUSB *ptATCusb=((CMainFrame *)AfxGetMainWnd())->m_ptatcusb;
	m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);

}
LRESULT CProgramDownDlg::OnSendBeginEndData(WPARAM wParam,LPARAM lParam)
{
	int *flag=(int *)wParam;
    DownLoadStartEnd(*flag);
	return 1;

}
LRESULT CProgramDownDlg::OnSendMidData(WPARAM wParam,LPARAM lParam)
{
	int nPicMaxPack=200;//ÿ����С
	int nPackCount=0;//����С

	///////////////////////////////////////
	if ((m_nDownloadSize % nPicMaxPack) == 0)
	{
		nPackCount = (m_nDownloadSize / nPicMaxPack);//����
	}
	else 
	{
		nPackCount = (m_nDownloadSize / nPicMaxPack) + 1;
	}
	m_procDownload.SetRange(0,nPackCount);
	int index=0;
	for(int i=0;i<nPackCount;i++)
	{

		BYTE buf[300];
		BYTE bufusb[300];
		int count;
		
		buf[0]=0x7e;
		buf[1]=0xff;
		buf[2]=m_bType;//Ŀ���豸��
		buf[3]=0x00;
		buf[4]=0x01;
		buf[5]=0x04;//CMD
		if(i+1<nPackCount)//Ӧ�ò����ݳ���
			buf[6]=8+nPicMaxPack;
		else
			buf[6]=8+m_nDownloadSize-(nPackCount-1)*nPicMaxPack;
		buf[7]=1;//��������
		buf[8]=2;//������־
		buf[9] = m_nDownloadSize;
		buf[10] = m_nDownloadSize>>8;
		buf[11]= m_nDownloadSize>>16;
		buf[12] = m_nDownloadSize>>24;
		buf[13]= i+1;
		buf[14] = (i+1)>>8;
		for(int j=0;j<buf[6]-8;j++)
		{
			buf[15+j]=m_pFileBuffer[index++];
		}
		/////////////////////////////////����Уԭ�͡�
		buf[7+buf[6]]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,6+buf[6]);
		buf[8+buf[6]]=0x7e;
		CString strstreetsel;
		GetDlgItemText(IDC_STREETCOMB,strstreetsel);
		if(strstreetsel.Compare(_T("CTC USB"))==0)
		{
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
			//((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
			m_ptctcusb->SendData(pSendSet);
			free(pSendSet);

		}
		else if(strstreetsel.Compare(_T("COM"))==0)
		{
			count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,9+buf[6]);

			//BOOL bl=((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
			BOOL bl=m_ptcom->WritePort(buf,count);
			if(bl==false)
			{
				MessageBox(_T("����дʧ��"),_T("Prompt"),MB_ICONWARNING);
				break;
			}
		}

      
		Sleep(150);
		//Sleep(250);
		m_procDownload.SetPos(i+1);
	}
	return 1;
}

void CProgramDownDlg::ShowRichMsg(CString str)
{
	CString temp;
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+_T("\r")+str);
	m_rich.LineScroll(m_rich.GetLineCount()); 
}

int CProgramDownDlg::SetUsbComPt(CHidUSB * m_ptctcusb, CHidUSB * m_ptatcusb, CHDLCSeries * m_ptcom)
{
	this->m_ptatcusb=m_ptatcusb;
	this->m_ptctcusb=m_ptctcusb;
	this->m_ptcom=m_ptcom;
	return 0;
}

void CProgramDownDlg::OnBnClickedAtcquery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strstreetsel;
	GetDlgItemText(IDC_STREETCOMB,strstreetsel);
	if(strstreetsel.Compare(_T("ATC USB"))!=0)
	{
		MessageBox(_T("����ѡ��ATC_USBͨ��"),_T("Prompt"),MB_ICONWARNING);
		return;

	}
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	int count;
	BYTE buf[50];
	buf[0]=0x7e;
	buf[1]=0x01;
	buf[2]=0x02;
	buf[3]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,2);
	buf[4]=0x7e;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,5);
	pSendSet->pPtr=buf;
	pSendSet->nLen=count;
	((CMainFrame *)AfxGetMainWnd())->m_ptatcusb->SendData2(pSendSet);
	free(pSendSet);
}

void CProgramDownDlg::OnCbnSelchangeStreetcomb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString picindex;
	int isel=((CComboBox *)GetDlgItem(IDC_STREETCOMB))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_STREETCOMB))->GetLBText(isel,picindex);
	if(picindex.Compare(_T("COM"))==0)
	{
		((CStatic *)GetDlgItem(IDC_STATIC_VERSION))->ShowWindow(false);
		((CComboBox *)GetDlgItem(IDC_KSNO))->ShowWindow(false);

	}
	else
	{
		((CStatic *)GetDlgItem(IDC_STATIC_VERSION))->ShowWindow(TRUE);
		((CComboBox *)GetDlgItem(IDC_KSNO))->ShowWindow(TRUE);

	}
	
}

void CProgramDownDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
}

void CProgramDownDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_rich.SetWindowText(_T(""));
}

void CProgramDownDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	((CComboBox *)GetDlgItem(IDC_STREETCOMB))->ResetContent();

	///////////////////////////////////////////////////////////////////////////
	if(((CMainFrame *)AfxGetMainWnd())->m_comisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("COM"));

	}
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("CTC USB"));
	}
	if(((CMainFrame *)AfxGetMainWnd())->m_atcusbisopen)
	{
		((CComboBox *)GetDlgItem(IDC_STREETCOMB))->AddString(_T("ATC USB"));

	}
}

void CProgramDownDlg::ClearVersion(void)
{
	CString str=_T("");
	
	SetDlgItemText(IDC_MAPEDIT1,str);
	
	SetDlgItemText(IDC_MAPEDIT2,str);
	
	SetDlgItemText(IDC_MAPEDIT3,str);
	
	SetDlgItemText(IDC_MAPEDIT4,str);
	
	SetDlgItemText(IDC_MAPEDIT5,str);
	
	SetDlgItemText(IDC_MAPEDIT6,str);

	SetDlgItemText(IDC_MAPEDIT7,str);

	SetDlgItemText(IDC_MAPEDIT8,str);

	SetDlgItemText(IDC_MAPEDIT9,str);

	SetDlgItemText(IDC_MAPEDIT10,str);
	//////////////////////////////////////////////

	SetDlgItemText(IDC_LEDEDIT1,str);

	SetDlgItemText(IDC_LEDEDIT2,str);

	SetDlgItemText(IDC_LEDEDIT3,str);

	SetDlgItemText(IDC_LEDEDIT4,str);
}

HBRUSH CProgramDownDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if(nCtlColor == CTLCOLOR_STATIC && IDC_EDIT_UPDATE_PATH != pWnd->GetDlgCtrlID())
	{
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

// DownByTypeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SZPTU.h"
#include "DownByTypeDlg.h"
#include "MainFrm.h"

// CDownByTypeDlg �Ի���

IMPLEMENT_DYNAMIC(CDownByTypeDlg, CDialog)

CDownByTypeDlg::CDownByTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownByTypeDlg::IDD, pParent)
{
	colint=1;
	streetint=0;

}

CDownByTypeDlg::~CDownByTypeDlg()
{
}

void CDownByTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listAllDown);
	DDX_Control(pDX, IDC_PROGRESS1, m_procCtrl);
}


BEGIN_MESSAGE_MAP(CDownByTypeDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKALL, &CDownByTypeDlg::OnBnClickedCheckall)
	ON_CBN_SELCHANGE(IDC_TYPECOMB, &CDownByTypeDlg::OnCbnSelchangeTypecomb)
	ON_CBN_SELCHANGE(IDC_LANCOMB, &CDownByTypeDlg::OnCbnSelchangeLancomb)
	ON_BN_CLICKED(IDC_DOWNBUTTON, &CDownByTypeDlg::OnBnClickedDownbutton)
END_MESSAGE_MAP()


// CDownByTypeDlg ��Ϣ�������
BOOL CDownByTypeDlg::GetFileFromDir(CString csDirPath)
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
	while(bState)
	{
		//wcstombs(line,(const char*)fileData.cFileName,259);
		m_Filenamearray.Add(fileData.cFileName);
		bState = FindNextFile(file, &fileData);
	}
	return TRUE;
}
void CDownByTypeDlg::SetNOArray()
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
	buf=NULL;
}
BOOL CDownByTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("վ��Ϣ"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("�����㲥"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->AddString(_T("��Ϣ"));
	((CComboBox *)GetDlgItem(IDC_TYPECOMB))->SetCurSel(0);
	////////////////////////////////////////////////////////////////////////
	((CComboBox *)GetDlgItem(IDC_LANCOMB))->AddString(_T("����"));
	((CComboBox *)GetDlgItem(IDC_LANCOMB))->AddString(_T("Ӣ��"));
	((CComboBox *)GetDlgItem(IDC_LANCOMB))->SetCurSel(0);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DWORD dwStyle = m_listAllDown.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT ;
	m_listAllDown.SetExtendedStyle(dwStyle);	

	m_listAllDown.InsertColumn(0,_T("no"),LVCFMT_CENTER,45);
	m_listAllDown.InsertColumn(1,_T("ͼƬ����"),LVCFMT_LEFT,200);
	m_listAllDown.InsertColumn(2,_T("ͼƬ·��"),LVCFMT_LEFT,200);
	////////////////////////////////////////////////////////////
	((CButton*)GetDlgItem(IDC_CHECKALL))->SetCheck(FALSE);
    //////////////////////////////////////////////////////////////
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+_T("CH")+_T("\\")+_T("station");
	GetFileFromDir(path);
	SetNOArray();
    SetListValues(0,0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDownByTypeDlg::SetListValues(int itype,int ilan)//012����ʾ3������
{
	m_listAllDown.DeleteAllItems();
	int ncount=m_NOarray.GetCount();
	CString strlan,strname;
	if(ilan==0)
		strlan=_T("CH");
	else
		strlan=_T("EN");
	if(itype==0)
		strname=_T("station");
	else if(itype==1)
		strname=_T("emg");
	else
		strname=_T("msg");
	for(int i=0;i<ncount;i++)
	{
		CString str;
		str.Format(_T("%d"),i+1);
		m_listAllDown.InsertItem(i,str);
		str=strname+_T("_")+m_NOarray.GetAt(i)+_T(".bmp");
		m_listAllDown.SetItemText(i,1,str);
		str=_T("\\pic\\")+strlan+_T("\\")+strname+_T("\\")+str;
		m_listAllDown.SetItemText(i,2,str);

	}
	
	
}

void CDownByTypeDlg::OnBnClickedCheckall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECKALL))->GetCheck();
	int iCount = m_listAllDown.GetItemCount();
	for (int i=0;i<iCount;i++)
	{
		m_listAllDown.SetCheck(i,bCheck);
	}
}

void CDownByTypeDlg::OnCbnSelchangeTypecomb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CButton*)GetDlgItem(IDC_CHECKALL))->SetCheck(0);
	int itype=((CComboBox *)GetDlgItem(IDC_TYPECOMB))->GetCurSel();
	int ilan=((CComboBox *)GetDlgItem(IDC_LANCOMB))->GetCurSel();
	CString strlan,strname;
	if(ilan==0)
		strlan=_T("CH");
	else
		strlan=_T("EN");
	if(itype==0)
		strname=_T("station");
	else if(itype==1)
		strname=_T("emg");
	else
		strname=_T("msg");
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+strlan+_T("\\")+strname;
	GetFileFromDir(path);
	SetNOArray();
	SetListValues(itype,ilan);

}

void CDownByTypeDlg::OnCbnSelchangeLancomb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CButton*)GetDlgItem(IDC_CHECKALL))->SetCheck(0);
	int itype=((CComboBox *)GetDlgItem(IDC_TYPECOMB))->GetCurSel();
	int ilan=((CComboBox *)GetDlgItem(IDC_LANCOMB))->GetCurSel();
	CString strlan,strname;
	if(ilan==0)
		strlan=_T("CH");
	else
		strlan=_T("EN");
	if(itype==0)
		strname=_T("station");
	else if(itype==1)
		strname=_T("emg");
	else
		strname=_T("msg");
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+strlan+_T("\\")+strname;
	GetFileFromDir(path);
	SetNOArray();
	SetListValues(itype,ilan);

}

void CDownByTypeDlg::OnBnClickedDownbutton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    int iCount = m_listAllDown.GetItemCount();
	int iselcount=0;
	int posint=0;
	for(int j=0;j<iCount;j++)
	{
		if(m_listAllDown.GetCheck(j))
		{
			iselcount++;
		}
	}
	if(iselcount==0)
	{
		MessageBox(_T("��ѡ�����ص�ͼƬ"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	m_procCtrl.SetRange(0,iselcount);
	m_procCtrl.SetPos(0);
	//DownStartPack();
	//Sleep(200);
	int isdownpic=0;
	for(int j=0;j<iCount;j++)
	{
		if(m_listAllDown.GetCheck(j)==false)
		{
			continue;
		}
		isdownpic++;
		CString str=m_listAllDown.GetItemText(j,2);
        CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+str;
        GetBmpPix(path);
		////////////////////////////////////////////////////
		//�������ݷְ���С200byte
		int nPicMaxPack = 200;
		int nPackCount = 1; //���ݷְ���С
		if ((nBmpCount % nPicMaxPack) == 0)
		{
			nPackCount = (nBmpCount / nPicMaxPack);//����
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
			buf[1]=0xff;//Ŀ�������
			buf[2]=0xf6;//Ŀ���豸
			buf[3]=0x00;//ԭ�����
			buf[4]=0x01;//ԭ�豸��
			buf[5]=0x04;//cmd�ֶ�
			if(i+1<nPackCount)//Ӧ�ò����ݳ���
				buf[6]=8+nPicMaxPack;
			else
				buf[6]=8+nBmpCount-(nPackCount-1)*nPicMaxPack;
			buf[7]=2;//��������
			bFlag bmyFlag;
			bmyFlag.bEn=((CComboBox *)GetDlgItem(IDC_LANCOMB))->GetCurSel();//������������������������������������������������Ҫͨ����ȡ
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
			if(j==0&&bmyFlag.bFirstPack==1)
			{
				bmyFlag.bStart = 1;
			}
			else
			{
				bmyFlag.bStart = 0;

			}
			if(isdownpic==iselcount&&bmyFlag.bLastPack==1)
			{
				bmyFlag.bStop = 1;

			}
			else
			{
				bmyFlag.bStop = 0;

			}
			buf[8]=bmyFlag.bFirstPack*4+bmyFlag.bLastPack*8+bmyFlag.bStart+bmyFlag.bStop*2+bmyFlag.bEn*16;//��־λ

			buf[9]=i+1;//��ǰ��
			buf[10]=((CComboBox *)GetDlgItem(IDC_TYPECOMB))->GetCurSel()+1;//���͡���������������������������������������������Ҫ��ȡ
			int indexbegin=str.Find('_');
			int indexend=str.Find('.');
			CString strNO=str.Mid(indexbegin+1,indexend-indexbegin-1);
			
			buf[11]=_ttoi(strNO);
			//ͼƬ��š�����������������������������������������������Ҫ��ȡ

			iPicColumns = (iPicColumns/8)*8;//����Ϊ8��������
			buf[12] = (BYTE)(0x00FF & iPicColumns);
			buf[13] = BYTE(iPicColumns>>8);
			buf[14]=colint;//��ɫ��������������������������������������������Ҫ��ȡ
			for(int j=0;j<buf[6]-8;j++)
			{
				buf[15+j]=m_vMapData.at(index++);
			}
			/////////////////////////////////����Уԭ�͡�
			buf[7+buf[6]]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,6+buf[6]);
			buf[8+buf[6]]=0x7e;
			CString strstreet;
			GetParent()->GetDlgItemText(IDC_STREETCOMB,strstreet);
			if(strstreet.IsEmpty())
			{
				MessageBox(_T("���������豸"),_T("Prompt"),MB_ICONWARNING);
				return;
			}
			if(strstreet.Compare(_T("COM"))==0)
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
			///////////////////////////////////////////////////////
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
		////////////////////////////////////////////////////
        m_procCtrl.SetPos(++posint);
		
	}
	//DownEndPack();/////////////////////////////
	MessageBox(_T("�������"));
}
byte CDownByTypeDlg::Littlebag(byte b7,byte b6,byte b5,byte b4,byte b3,byte b2,byte b1,byte b0)
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
void CDownByTypeDlg::GetBmpPix(CString strFileName)
{
	WCHAR   *wChar; 
	wChar = strFileName.Trim().AllocSysString();

	Gdiplus::Bitmap *pBitMap = new Bitmap(wChar);
	iPicColumns = pBitMap->GetWidth();
	int iPicRows = pBitMap->GetHeight();
    m_vMapData.clear();
#pragma region 	ѭ��ɨ�����

	int ictrl = 0;//������ɨ��
	byte bArry[8] ={0}; 
	
	for (int i = 0; i <= (iPicRows - 1); i++)
	{
		for (int j = 0; j <= (iPicColumns - 1); j += 8)
		{
			ictrl = 0;
			do
			{
				Color pixelColor;
				//pBitMap->GetPixel(i, ictrl + j,&pixelColor);  //deleted by Amy 2012-8-20
				pBitMap->GetPixel(ictrl + j, i, &pixelColor);
				if (pixelColor.GetValue() == 0xffffffff)
				{
					//bArry[ictrl] = 0;		//deleted by Amy 2012-8-20
					bArry[7-ictrl] = 0; 
					ictrl++;     
				}

				else
				{
					//bArry[ictrl] = 1;		//deleted by Amy 2012-8-20
					bArry[7-ictrl] = 1; 
					ictrl++;                
				}    

			}
			while (ictrl <= 7);
			m_vMapData.push_back(Littlebag(bArry[7], bArry[6], bArry[5], bArry[4], bArry[3], bArry[2], bArry[1], bArry[0]));
		}
	}
#pragma endregion 

#pragma region ����ͼƬ����У��
	//ȡ����
	int nBmpCount = m_vMapData.size();
	if(nBmpCount < 1) 
	{
		return;
	}
	//��������XOR
	BYTE bXor = m_vMapData.at(0);
	for (int i=1;i<nBmpCount;i++)
	{
		bXor = bXor^m_vMapData[i];
	}
	WORD wAddCheckSum = 0;
	//�������ݲ��ݵ�Add��
	for (int i=0;i<nBmpCount; i++ )
	{
		wAddCheckSum = wAddCheckSum + m_vMapData.at(i);
	}
	//��У��������ֽڷŵ�ͼƬ������
	m_vMapData.push_back(bXor);
	m_vMapData.push_back((BYTE)(wAddCheckSum & 0x00FF));//�����λ
	m_vMapData.push_back((BYTE)(wAddCheckSum>>8));//�����λ
	//ʵ��ͼƬ����ΪnBmpCount + 3
	nBmpCount +=3;
    this->nBmpCount=nBmpCount;
#pragma endregion
	
}

void CDownByTypeDlg::SetColorint(int colorint,int streetint)
{
	this->colint=colorint;
	this->streetint=streetint;
}

void CDownByTypeDlg::ShowListItem(CString strbegintime, CString strendtime, CString strcount)
{
}

void CDownByTypeDlg::DownStartPack(void)
{
	BYTE buf[255];
	memset(buf,0,255);
	buf[0]=0x7e;
	buf[1]=0xff;//Ŀ�������
	buf[2]=0xf6;//Ŀ���豸
	buf[3]=0x00;//ԭ�����
	buf[4]=0x01;//ԭ�豸��
	buf[5]=0x04;//cmd�ֶ�
	
	buf[6]=2;

	buf[7]=2;//��������
	bFlag bmyFlag;
	bmyFlag.bEn=((CComboBox *)GetDlgItem(IDC_LANCOMB))->GetCurSel();//������������������������������������������������Ҫͨ����ȡ
	bmyFlag.bLastPack = 0;
	bmyFlag.bStart = 1;
	bmyFlag.bFirstPack = 0;
	bmyFlag.bStop = 0;
	buf[8]=bmyFlag.bFirstPack*4+bmyFlag.bLastPack*8+bmyFlag.bStart+bmyFlag.bStop*2+bmyFlag.bEn*16;//��־λ
	/////////////////////////////////����Уԭ�͡�
	buf[9]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,8);
	buf[10]=0x7e;
	int count;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,11);
    ((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
    
}

void CDownByTypeDlg::DownEndPack(void)
{
	BYTE buf[255];
	memset(buf,0,255);
	buf[0]=0x7e;
	buf[1]=0xff;//Ŀ�������
	buf[2]=0xf6;//Ŀ���豸
	buf[3]=0x00;//ԭ�����
	buf[4]=0x01;//ԭ�豸��
	buf[5]=0x04;//cmd�ֶ�

	buf[6]=2;

	buf[7]=2;//��������
	bFlag bmyFlag;
	bmyFlag.bEn=((CComboBox *)GetDlgItem(IDC_LANCOMB))->GetCurSel();//������������������������������������������������Ҫͨ����ȡ
	bmyFlag.bLastPack = 0;
	bmyFlag.bStart = 0;
	bmyFlag.bFirstPack = 0;
	bmyFlag.bStop = 1;
	buf[8]=bmyFlag.bFirstPack*4+bmyFlag.bLastPack*8+bmyFlag.bStart+bmyFlag.bStop*2+bmyFlag.bEn*16;//��־λ
	/////////////////////////////////����Уԭ�͡�
	buf[9]=((CMainFrame *)AfxGetMainWnd())->VerifyFun(buf,1,8);
	buf[10]=0x7e;
	int count;
	count=((CMainFrame *)AfxGetMainWnd())->SwitchSendData(buf,11);
	((CMainFrame *)AfxGetMainWnd())->m_com.WritePort(buf,count);
}

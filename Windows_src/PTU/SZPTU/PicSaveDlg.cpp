// PicSaveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SZPTU.h"
#include "PicSaveDlg.h"
#include "MainFrm.h"

// CPicSaveDlg �Ի���

IMPLEMENT_DYNAMIC(CPicSaveDlg, CDialog)

CPicSaveDlg::CPicSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPicSaveDlg::IDD, pParent)
{
	pictype=0;languageint=0; picNO=0;
	m_piccount=0;

}

CPicSaveDlg::~CPicSaveDlg()
{
}

void CPicSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPicSaveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPicSaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPicSaveDlg ��Ϣ�������

void CPicSaveDlg::SetParates(int pictype, int languageint, int picNO)
{
	this->pictype=pictype;
	this->languageint=languageint;
	this->picNO=picNO;
}

BOOL CPicSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->ResetContent();
	((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->AddString(_T("վ��Ϣ"));
	((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->AddString(_T("�����㲥"));
	((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->AddString(_T("��Ϣ"));
	((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->SetCurSel(pictype);
	////////////////////////////////////////////////////////////////////////
    ((CComboBox *)GetDlgItem(IDC_PICLANSEL))->ResetContent();
	((CComboBox *)GetDlgItem(IDC_PICLANSEL))->AddString(_T("����"));
	((CComboBox *)GetDlgItem(IDC_PICLANSEL))->AddString(_T("Ӣ��"));
	((CComboBox *)GetDlgItem(IDC_PICLANSEL))->SetCurSel(languageint);
	//////////////////////////////////////////////////////////////////////////
	if(picNO==0)
	{
		m_piccount=GetPicCount();
		SetDlgItemInt(IDC_PICNOSEL,m_piccount+1);
	}
	else
	{
		SetDlgItemInt(IDC_PICNOSEL,picNO);

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPicSaveDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strValue;
	GetDlgItemText(IDC_PICNOSEL,strValue);
	/////////////////////////////////////////////////
	//���Ƿ��ַ�
	int nCount = strValue.GetLength(); // ����ַ�����
	for ( int i = 0; i < nCount; i ++ )
	{
		if ( 0 == isdigit( strValue.GetAt(i) ) ) // �������־��ñ�־λ
		{
			MessageBox(_T("����������������!"),_T("Prompt"),MB_ICONWARNING);
			return;// �˳�
		}
	}
	////////////////////////////////////////////////////////////////////
	pictype=((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->GetCurSel();
	languageint=((CComboBox *)GetDlgItem(IDC_PICLANSEL))->GetCurSel();
	m_piccount=GetPicCount();
	picNO=GetDlgItemInt(IDC_PICNOSEL);
	if(m_piccount<picNO)
	{
		MessageBox(_T("�밴��ŵ���˳�򱣴�ͼƬ"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
    if(picNO==0)
	{
		MessageBox(_T("��Ų��ܵ���0"),_T("Prompt"),MB_ICONWARNING);
		return;

	}
	
	OnOK();
}
int CPicSaveDlg::GetFileCount(CString csDirPath)
{
	int count=0;
	csDirPath+=_T("\\*.bmp");
	HANDLE file;
	WIN32_FIND_DATA fileData;
	char line[1024];
	char fn[1000];
	//mbstowcs(fn,csDirPath.GetBuffer(),999);
	file = FindFirstFile(csDirPath.GetBuffer(), &fileData);
	if(file==INVALID_HANDLE_VALUE)
	{
		return 0;

	}
	count++;
	bool bState = false;
	bState = FindNextFile(file, &fileData);
	while(bState){
		//wcstombs(line,(const char*)fileData.cFileName,259);
		count++;
		bState = FindNextFile(file, &fileData);
	}
	return count;
}
int CPicSaveDlg::GetPicCount()
{
	CString typestr,lanstr;
	int type=((CComboBox *)GetDlgItem(IDC_PICTYPESEL))->GetCurSel();
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
	int lan=((CComboBox *)GetDlgItem(IDC_PICLANSEL))->GetCurSel();
	if(lan==0)
	{
		lanstr=_T("CH");
	}
	if(lan==1)
	{
		lanstr=_T("EN");
	}
	CString path=((CMainFrame *)AfxGetMainWnd())->GetAppPath()+_T("\\pic\\")+lanstr+_T("\\")+typestr;
	return GetFileCount(path);

}
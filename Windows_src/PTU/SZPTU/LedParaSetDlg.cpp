// LedParaSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SZPTU.h"
#include "LedParaSetDlg.h"


// CLedParaSetDlg �Ի���

IMPLEMENT_DYNAMIC(CLedParaSetDlg, CDialog)

CLedParaSetDlg::CLedParaSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLedParaSetDlg::IDD, pParent)
	, m_lanselint(2)
	, m_mescount(1)
{
	

}

CLedParaSetDlg::~CLedParaSetDlg()
{
}

void CLedParaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBOLAN, m_lanselint);
	DDX_Text(pDX, IDC_EDIT1, m_mescount);
}


BEGIN_MESSAGE_MAP(CLedParaSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLedParaSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLedParaSetDlg ��Ϣ�������

void CLedParaSetDlg::OnBnClickedOk()
{
	CString str;
	GetDlgItemText(IDC_EDIT1,str);
	for(int i=0;i<str.GetLength();i++)
	{
		UCHAR bcode=str.GetAt(i);
		if(bcode>='0'&&bcode<='9')
		{

		}
		else
		{
			MessageBox(_T("������������ֵ"),_T("Prompt"),MB_ICONWARNING);
			return;
		}
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

BOOL CLedParaSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    ((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("��������ʾ"));
	((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("��Ӣ����ʾ"));
	((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("����Ӣ����ʾ"));
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    ((CComboBox *)GetDlgItem(IDC_COMBOLAN))->SetCurSel(2);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

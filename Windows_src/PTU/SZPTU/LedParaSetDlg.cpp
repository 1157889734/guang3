// LedParaSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "LedParaSetDlg.h"


// CLedParaSetDlg 对话框

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


// CLedParaSetDlg 消息处理程序

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
			MessageBox(_T("请输入整数数值"),_T("Prompt"),MB_ICONWARNING);
			return;
		}
	}
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

BOOL CLedParaSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    ((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("仅中文显示"));
	((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("仅英文显示"));
	((CComboBox *)GetDlgItem(IDC_COMBOLAN))->AddString(_T("中文英文显示"));
	// TODO:  在此添加额外的初始化
    ((CComboBox *)GetDlgItem(IDC_COMBOLAN))->SetCurSel(2);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

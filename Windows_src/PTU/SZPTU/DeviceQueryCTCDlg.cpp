// DeviceQueryCTCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SZPTU.h"
#include "DeviceQueryCTCDlg.h"
#include "MainFrm.h"

// CDeviceQueryCTCDlg 对话框

IMPLEMENT_DYNAMIC(CDeviceQueryCTCDlg, CDialog)

CDeviceQueryCTCDlg::CDeviceQueryCTCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceQueryCTCDlg::IDD, pParent)
{
	m_ctccount=0;
	memset(m_ctcbuf,0,(16+6)*4);
	memset(m_ksbuf1,0,15*4);
	memset(m_ksbuf2,0,15*4);
	memset(m_ksbuf3,0,15*4);
	memset(m_ksbuf4,0,15*4);
	memset(m_ksbuf5,0,15*4);
	memset(m_ksbuf6,0,15*4);
	int staticidctc[16+6]={IDC_KS1,IDC_KS2,IDC_KS3,IDC_KS4,IDC_KS5,IDC_KS6,IDC_CTC1,IDC_CTC2,IDC_DVA1,IDC_DVA2,IDC_APU1,IDC_APU2,IDC_DCP1,IDC_DCP2,IDC_ATC1,IDC_ATC2,
	IDC_IO1,IDC_IO2,IDC_IO3,IDC_IO4,IDC_IO5,IDC_IO6};
    memcpy(m_staticidctc,staticidctc,(16+6)*4);
	
	int staticidks1[15]={IDC_BJQ_STATUS11,IDC_BJQ_STATUS12,IDC_BJQ_STATUS13,IDC_BJQ_ERROR11,IDC_BJQ_ERROR12,IDC_BJQ_ERROR13,
		IDC_AMP11,IDC_AMP12,IDC_SPK11,IDC_SPK12,IDC_WDS11,IDC_WDS12,IDC_WDS13,IDC_WDS14,IDC_DMP1};
	memcpy(m_staticidks1,staticidks1,15*4);
	int staticidks2[15]={IDC_BJQ_STATUS21,IDC_BJQ_STATUS22,IDC_BJQ_STATUS23,IDC_BJQ_ERROR21,IDC_BJQ_ERROR22,IDC_BJQ_ERROR23,
		IDC_AMP21,IDC_AMP22,IDC_SPK21,IDC_SPK22,IDC_WDS21,IDC_WDS22,IDC_WDS23,IDC_WDS24,IDC_DMP2};
	memcpy(m_staticidks2,staticidks2,15*4);
	int staticidks3[15]={IDC_BJQ_STATUS31,IDC_BJQ_STATUS32,IDC_BJQ_STATUS33,IDC_BJQ_ERROR31,IDC_BJQ_ERROR32,IDC_BJQ_ERROR33,
		IDC_AMP31,IDC_AMP32,IDC_SPK31,IDC_SPK32,IDC_WDS31,IDC_WDS32,IDC_WDS33,IDC_WDS34,IDC_DMP3};
	memcpy(m_staticidks3,staticidks3,15*4);
	int staticidks4[15]={IDC_BJQ_STATUS41,IDC_BJQ_STATUS42,IDC_BJQ_STATUS43,IDC_BJQ_ERROR41,IDC_BJQ_ERROR42,IDC_BJQ_ERROR43,
		IDC_AMP41,IDC_AMP42,IDC_SPK41,IDC_SPK42,IDC_WDS41,IDC_WDS42,IDC_WDS43,IDC_WDS44,IDC_DMP4};
	memcpy(m_staticidks4,staticidks4,15*4);
	int staticidks5[15]={IDC_BJQ_STATUS51,IDC_BJQ_STATUS52,IDC_BJQ_STATUS53,IDC_BJQ_ERROR51,IDC_BJQ_ERROR52,IDC_BJQ_ERROR53,
		IDC_AMP51,IDC_AMP52,IDC_SPK51,IDC_SPK52,IDC_WDS51,IDC_WDS52,IDC_WDS53,IDC_WDS54,IDC_DMP5};
	memcpy(m_staticidks5,staticidks5,15*4);
	int staticidks6[15]={IDC_BJQ_STATUS61,IDC_BJQ_STATUS62,IDC_BJQ_STATUS63,IDC_BJQ_ERROR61,IDC_BJQ_ERROR62,IDC_BJQ_ERROR63,
		IDC_AMP61,IDC_AMP62,IDC_SPK61,IDC_SPK62,IDC_WDS61,IDC_WDS62,IDC_WDS63,IDC_WDS64,IDC_DMP6};
    memcpy(m_staticidks6,staticidks6,15*4);

}

CDeviceQueryCTCDlg::~CDeviceQueryCTCDlg()
{
}

void CDeviceQueryCTCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeviceQueryCTCDlg, CDialog)
	ON_BN_CLICKED(IDC_QUERY, &CDeviceQueryCTCDlg::OnBnClickedQuery)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDeviceQueryCTCDlg 消息处理程序

void CDeviceQueryCTCDlg::OnBnClickedQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CMainFrame *)AfxGetMainWnd())->m_ctcusbisopen==false)
	{
		MessageBox(_T("请先连接中央控制器"),_T("Prompt"),MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////////////
	
	BYTE Send[6];
	USB_DATAPACK *pSendSet = (USB_DATAPACK *)malloc(sizeof(USB_DATAPACK));
	memset(Send, 0, 6);
	memset(pSendSet, 0, sizeof(USB_DATAPACK));

	Send[0] = 0x7E;    //起始符
	Send[1] = 0x16;    //校对时间回读命令
	Send[2] = 0x00;    //数据长度1字节
	Send[3] = 0x00;    //数据长度2字节
	Send[4] = 0x55 - 0x16;
	Send[5] = 0x7E;   //结束符

	pSendSet->nLen = 6;
	pSendSet->pPtr = Send;
	((CMainFrame *)AfxGetMainWnd())->m_ptctcusb->SendData(pSendSet);
	free(pSendSet);

}

void CDeviceQueryCTCDlg::RecStateInfo(BYTE * tempBuf, int bufcount)
{
	//第9位开始。
	//进行显示。
	
	/*
	
	CFile file;
	file.Open(_T("pis.txt"),CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate); 
	file.SeekToEnd();

	file.Write(tempBuf,bufcount);*/

	if(bufcount!=62)
	{
		//MessageBox(_T("这不是主设备"));
		return;
	}
    ///////////////////////////////////////
	if(m_ctccount==0xff)
	{
		m_ctccount=0;
	}
	m_ctccount++;
	SetDlgItemInt(IDC_STATIC_CTCCOUNT,m_ctccount);
	///////////////////////////////////////////////
	
	int ctcbuf[16+6]={tempBuf[9]&1,(tempBuf[9]>>1)&1,(tempBuf[9]>>2)&1,(tempBuf[9]>>3)&1,(tempBuf[9]>>4)&1,
		(tempBuf[9]>>5)&1,(tempBuf[9]>>6)&1,(tempBuf[9]>>7)&1,(tempBuf[10])&1,(tempBuf[10]>>1)&1,(tempBuf[10]>>2)&1,(tempBuf[10]>>3)&1,(tempBuf[10]>>4)&1,
		(tempBuf[10]>>5)&1,(tempBuf[11])&1,(tempBuf[11]>>1)&1, (tempBuf[11]>>2)&1,(tempBuf[11]>>3)&1,(tempBuf[11]>>4)&1,(tempBuf[11]>>5)&1,(tempBuf[11]>>6)&1,(tempBuf[11]>>7)&1};
	int staticidctc[16+6]={IDC_KS1,IDC_KS2,IDC_KS3,IDC_KS4,IDC_KS5,IDC_KS6,IDC_CTC1,IDC_CTC2,IDC_DVA1,IDC_DVA2,IDC_APU1,IDC_APU2,IDC_DCP1,IDC_DCP2,IDC_ATC1,IDC_ATC2,
	IDC_IO1,IDC_IO2,IDC_IO3,IDC_IO4,IDC_IO5,IDC_IO6};
    
	memcpy(m_ctcbuf,ctcbuf,(16+6)*4);
	for(int i=0;i<16+6;i++)
	{
		SetDlgItemInt(staticidctc[i],ctcbuf[i]);
	}
	//////////////////////////////////////////////////////////////////////////////////////////
// 	int ksbuf1[15]={(tempBuf[12])&3,(tempBuf[12]>>2)&3,(tempBuf[12]>>4)&3,(tempBuf[13]>>4)&1,(tempBuf[13]>>5)&1,(tempBuf[13]>>6)&1,
// 	    (tempBuf[14])&1,(tempBuf[14]>>1)&1,(tempBuf[14]>>2)&1,(tempBuf[14]>>3)&1,(tempBuf[15])&1,(tempBuf[15]>>1)&1,(tempBuf[15]>>2)&1,(tempBuf[15]>>3)&1,
// 	    (tempBuf[16])&1};
	int ksbuf1[15]={(tempBuf[12] >> 6)&3,(tempBuf[12]>>4)&3,(tempBuf[12]>>2)&3,(tempBuf[13]>>7)&1,(tempBuf[13]>>6)&1,(tempBuf[13]>>5)&1,
		(tempBuf[14])&1,(tempBuf[14]>>1)&1,(tempBuf[14]>>2)&1,(tempBuf[14]>>3)&1,(tempBuf[15])&1,(tempBuf[15]>>1)&1,(tempBuf[15]>>2)&1,(tempBuf[15]>>3)&1,
		(tempBuf[16])&1};
	int staticidks1[15]={IDC_BJQ_STATUS11,IDC_BJQ_STATUS12,IDC_BJQ_STATUS13,IDC_BJQ_ERROR11,IDC_BJQ_ERROR12,IDC_BJQ_ERROR13,
	     IDC_AMP11,IDC_AMP12,IDC_SPK11,IDC_SPK12,IDC_WDS11,IDC_WDS12,IDC_WDS13,IDC_WDS14,IDC_DMP1};
	if(ctcbuf[0]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf1[i]=1;
		}
	}
	memcpy(m_ksbuf1,ksbuf1,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks1[i],ksbuf1[i]);
	}
	//////////////////////////////////////////////////////////////////////////
// 	int ksbuf2[15]={(tempBuf[12+11])&3,(tempBuf[12+11]>>2)&3,(tempBuf[12+11]>>4)&3,(tempBuf[13+11]>>4)&1,(tempBuf[13+11]>>5)&1,(tempBuf[13+11]>>6)&1,
// 		(tempBuf[14+11])&1,(tempBuf[14+11]>>1)&1,(tempBuf[14+11]>>2)&1,(tempBuf[14+11]>>3)&1,(tempBuf[15+11])&1,(tempBuf[15+11]>>1)&1,(tempBuf[15+11]>>2)&1,(tempBuf[15+11]>>3)&1,
// 		(tempBuf[16+11])&1};

	int ksbuf2[15]={(tempBuf[12+8] >> 6)&3,(tempBuf[12+8]>>4)&3,(tempBuf[12+8]>>2)&3,(tempBuf[13+8]>>7)&1,(tempBuf[13+8]>>6)&1,(tempBuf[13+8]>>5)&1,
		(tempBuf[14+8])&1,(tempBuf[14+8]>>1)&1,(tempBuf[14+8]>>2)&1,(tempBuf[14+8]>>3)&1,(tempBuf[15+8])&1,(tempBuf[15+8]>>1)&1,(tempBuf[15+8]>>2)&1,(tempBuf[15+8]>>3)&1,
		(tempBuf[16+8])&1};
	int staticidks2[15]={IDC_BJQ_STATUS21,IDC_BJQ_STATUS22,IDC_BJQ_STATUS23,IDC_BJQ_ERROR21,IDC_BJQ_ERROR22,IDC_BJQ_ERROR23,
		IDC_AMP21,IDC_AMP22,IDC_SPK21,IDC_SPK22,IDC_WDS21,IDC_WDS22,IDC_WDS23,IDC_WDS24,IDC_DMP2};
	if(ctcbuf[1]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf2[i]=1;
		}
	}
	memcpy(m_ksbuf2,ksbuf2,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks2[i],ksbuf2[i]);
	}
	//////////////////////////////////////////////////////////////////////////
// 	int ksbuf3[15]={(tempBuf[12+22])&3,(tempBuf[12+22]>>2)&3,(tempBuf[12+22]>>4)&3,(tempBuf[13+22]>>4)&1,(tempBuf[13+22]>>5)&1,(tempBuf[13+22]>>6)&1,
// 		(tempBuf[14+22])&1,(tempBuf[14+22]>>1)&1,(tempBuf[14+22]>>2)&1,(tempBuf[14+22]>>3)&1,(tempBuf[15+22])&1,(tempBuf[15+22]>>1)&1,(tempBuf[15+22]>>2)&1,(tempBuf[15+22]>>3)&1,
// 		(tempBuf[16+22])&1};

	int ksbuf3[15]={(tempBuf[12+16] >> 6)&3,(tempBuf[12+16]>>4)&3,(tempBuf[12+16]>>2)&3,(tempBuf[13+16]>>7)&1,(tempBuf[13+16]>>6)&1,(tempBuf[13+16]>>5)&1,
		(tempBuf[14+16])&1,(tempBuf[14+16]>>1)&1,(tempBuf[14+16]>>2)&1,(tempBuf[14+16]>>3)&1,(tempBuf[15+16])&1,(tempBuf[15+16]>>1)&1,(tempBuf[15+16]>>2)&1,(tempBuf[15+16]>>3)&1,
		(tempBuf[16+16])&1};
	int staticidks3[15]={IDC_BJQ_STATUS31,IDC_BJQ_STATUS32,IDC_BJQ_STATUS33,IDC_BJQ_ERROR31,IDC_BJQ_ERROR32,IDC_BJQ_ERROR33,
		IDC_AMP31,IDC_AMP32,IDC_SPK31,IDC_SPK32,IDC_WDS31,IDC_WDS32,IDC_WDS33,IDC_WDS34,IDC_DMP3};
	if(ctcbuf[2]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf3[i]=1;
		}
	}
	memcpy(m_ksbuf3,ksbuf3,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks3[i],ksbuf3[i]);
	}
	//////////////////////////////////////////////////////////////////////////
// 	int ksbuf4[15]={(tempBuf[12+33])&3,(tempBuf[12+33]>>2)&3,(tempBuf[12+33]>>4)&3,(tempBuf[13+33]>>4)&1,(tempBuf[13+33]>>5)&1,(tempBuf[13+33]>>6)&1,
// 		(tempBuf[14+33])&1,(tempBuf[14+33]>>1)&1,(tempBuf[14+33]>>2)&1,(tempBuf[14+33]>>3)&1,(tempBuf[15+33])&1,(tempBuf[15+33]>>1)&1,(tempBuf[15+33]>>2)&1,(tempBuf[15+33]>>3)&1,
// 		(tempBuf[16+33])&1};

	int ksbuf4[15]={(tempBuf[12+24] >> 6)&3,(tempBuf[12+24]>>4)&3,(tempBuf[12+24]>>2)&3,(tempBuf[13+24]>>7)&1,(tempBuf[13+24]>>6)&1,(tempBuf[13+24]>>5)&1,
		(tempBuf[14+24])&1,(tempBuf[14+24]>>1)&1,(tempBuf[14+24]>>2)&1,(tempBuf[14+24]>>3)&1,(tempBuf[15+24])&1,(tempBuf[15+24]>>1)&1,(tempBuf[15+24]>>2)&1,(tempBuf[15+24]>>3)&1,
		(tempBuf[16+24])&1};
	int staticidks4[15]={IDC_BJQ_STATUS41,IDC_BJQ_STATUS42,IDC_BJQ_STATUS43,IDC_BJQ_ERROR41,IDC_BJQ_ERROR42,IDC_BJQ_ERROR43,
		IDC_AMP41,IDC_AMP42,IDC_SPK41,IDC_SPK42,IDC_WDS41,IDC_WDS42,IDC_WDS43,IDC_WDS44,IDC_DMP4};
	if(ctcbuf[3]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf4[i]=1;
		}
	}
	memcpy(m_ksbuf4,ksbuf4,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks4[i],ksbuf4[i]);
	}
	//////////////////////////////////////////////////////////////////////////
// 	int ksbuf5[15]={(tempBuf[12+44])&3,(tempBuf[12+44]>>2)&3,(tempBuf[12+44]>>4)&3,(tempBuf[13+44]>>4)&1,(tempBuf[13+44]>>5)&1,(tempBuf[13+44]>>6)&1,
// 		(tempBuf[14+44])&1,(tempBuf[14+44]>>1)&1,(tempBuf[14+44]>>2)&1,(tempBuf[14+44]>>3)&1,(tempBuf[15+44])&1,(tempBuf[15+44]>>1)&1,(tempBuf[15+44]>>2)&1,(tempBuf[15+44]>>3)&1,
// 		(tempBuf[16+44])&1};

	int ksbuf5[15]={(tempBuf[12+32] >> 6)&3,(tempBuf[12+32]>>4)&3,(tempBuf[12+32]>>2)&3,(tempBuf[13+32]>>7)&1,(tempBuf[13+32]>>6)&1,(tempBuf[13+32]>>5)&1,
		(tempBuf[14+32])&1,(tempBuf[14+32]>>1)&1,(tempBuf[14+32]>>2)&1,(tempBuf[14+32]>>3)&1,(tempBuf[15+32])&1,(tempBuf[15+32]>>1)&1,(tempBuf[15+32]>>2)&1,(tempBuf[15+32]>>3)&1,
		(tempBuf[16+32])&1};
	int staticidks5[15]={IDC_BJQ_STATUS51,IDC_BJQ_STATUS52,IDC_BJQ_STATUS53,IDC_BJQ_ERROR51,IDC_BJQ_ERROR52,IDC_BJQ_ERROR53,
		IDC_AMP51,IDC_AMP52,IDC_SPK51,IDC_SPK52,IDC_WDS51,IDC_WDS52,IDC_WDS53,IDC_WDS54,IDC_DMP5};
	if(ctcbuf[4]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf5[i]=1;
		}
	}
	memcpy(m_ksbuf5,ksbuf5,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks5[i],ksbuf5[i]);
	}
	//////////////////////////////////////////////////////////////////////////
// 	int ksbuf6[15]={(tempBuf[12+55])&3,(tempBuf[12+55]>>2)&3,(tempBuf[12+55]>>4)&3,(tempBuf[13+55]>>4)&1,(tempBuf[13+55]>>5)&1,(tempBuf[13+55]>>6)&1,
// 		(tempBuf[14+55])&1,(tempBuf[14+55]>>1)&1,(tempBuf[14+55]>>2)&1,(tempBuf[14+55]>>3)&1,(tempBuf[15+55])&1,(tempBuf[15+55]>>1)&1,(tempBuf[15+55]>>2)&1,(tempBuf[15+55]>>3)&1,
// 		(tempBuf[16+55])&1};

	int ksbuf6[15]={(tempBuf[12+40] >> 6)&3,(tempBuf[12+40]>>4)&3,(tempBuf[12+40]>>2)&3,(tempBuf[13+40]>>7)&1,(tempBuf[13+40]>>6)&1,(tempBuf[13+40]>>5)&1,
		(tempBuf[14+40])&1,(tempBuf[14+40]>>1)&1,(tempBuf[14+40]>>2)&1,(tempBuf[14+40]>>3)&1,(tempBuf[15+40])&1,(tempBuf[15+40]>>1)&1,(tempBuf[15+40]>>2)&1,(tempBuf[15+40]>>3)&1,
		(tempBuf[16+40])&1};
	int staticidks6[15]={IDC_BJQ_STATUS61,IDC_BJQ_STATUS62,IDC_BJQ_STATUS63,IDC_BJQ_ERROR61,IDC_BJQ_ERROR62,IDC_BJQ_ERROR63,
		IDC_AMP61,IDC_AMP62,IDC_SPK61,IDC_SPK62,IDC_WDS61,IDC_WDS62,IDC_WDS63,IDC_WDS64,IDC_DMP6};
	if(ctcbuf[5]==1)
	{
		for(int i=0;i<15;i++)
		{
			ksbuf6[i]=1;
		}
	}
	memcpy(m_ksbuf6,ksbuf6,15*4);
	for(int i=0;i<15;i++)
	{
		SetDlgItemInt(staticidks6[i],ksbuf6[i]);
	}
    /////////////////////////////////
	for(int i=0;i<16+6;i++)
	{
		ChangColor(staticidctc[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks1[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks2[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks3[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks4[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks5[i]);
	}
	for(int i=0;i<15;i++)
	{
		ChangColor(staticidks6[i]);
	}
	////////////////////////////////////////////
	//Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
}
void CDeviceQueryCTCDlg::ChangColor(int nID/*, int iState*/)
{
	//static int nCount = 0;
	//m_uFaultState[nID] = iState;
	((CStatic *)GetDlgItem(/*IDC_STATIC0 + */nID))->Invalidate();

}
HBRUSH CDeviceQueryCTCDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		int nID = pWnd->GetDlgCtrlID();
		int groupint;
		int index=GetIDIndex(nID,groupint);
        int values=0;
		if(index==-1)
		{
			if(nID==IDC_EXAMNORMAL)
				pDC->SetBkColor(RGB(0,255,0));
			if(nID==IDC_EXAMERROR)
				pDC->SetBkColor(RGB(255,0,0));
			if(nID==IDC_EXAMFREE)
				pDC->SetBkColor(RGB(0,255,0));
			if(nID==IDC_EXAMBAOJIN)
				pDC->SetBkColor(RGB(255,0,0));
			if(nID==IDC_EXAMTALK)
				pDC->SetBkColor(RGB(0,0,255));

			return hbr;
		}
		if(groupint==0)
		{
			values=m_ctcbuf[index];
		}
		if(groupint==1)
		{
			values=m_ksbuf1[index];
		}
		if(groupint==2)
		{
			values=m_ksbuf2[index];
		}
		if(groupint==3)
		{
			values=m_ksbuf3[index];
		}
		if(groupint==4)
		{
			values=m_ksbuf4[index];
		}
		if(groupint==5)
		{
			values=m_ksbuf5[index];
		}
		if(groupint==6)
		{
			values=m_ksbuf6[index];
		}
		////////////////////////////////////////////////
		if(values==1)
		{
			pDC->SetBkColor(RGB(255,0,0));
		}
		else if(values==0)
		{
			pDC->SetBkColor(RGB(0,255,0));
		}
		else
		{
			pDC->SetBkColor(RGB(0,0,255));
		}
		
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

int CDeviceQueryCTCDlg::GetIDIndex(int staticid, int & groupint)
{
	for(int i=0;i<16+6;i++)
	{
		if(staticid==m_staticidctc[i])
		{
			groupint=0;
			return i;
		}
	}
	//////////////////////////////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks1[i])
		{
			groupint=1;
			return i;
		}
	}
	/////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks2[i])
		{
			groupint=2;
			return i;
		}
	}
	/////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks3[i])
		{
			groupint=3;
			return i;
		}
	}
	/////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks4[i])
		{
			groupint=4;
			return i;
		}
	}
	/////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks5[i])
		{
			groupint=5;
			return i;
		}
	}
	/////////////////////////////
	for(int i=0;i<15;i++)
	{
		if(staticid==m_staticidks6[i])
		{
			groupint=6;
			return i;
		}
	}
	/////////////////////////////
	return -1;
}

void CDeviceQueryCTCDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(232, 249, 255));
}

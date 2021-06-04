// LittaceSet.cpp : implementation file
//

#include "stdafx.h"
#include "SZPTU.h"
#include "LittaceSet.h"
#include "MainFrm.h"
#include <Shlwapi.h>
#include "CustomDlg.h"
#include "DownLoadLED.h"

// CLatticeSet dialog

IMPLEMENT_DYNAMIC(CLatticeSet, CDialog)

CLatticeSet::CLatticeSet(CWnd* pParent /*=NULL*/)
	: CDialog(CLatticeSet::IDD, pParent),m_hBitmap(NULL),m_bmImage(NULL)
{
	m_inputeditidshow=IDC_EDIT_INPUT_TEXT;
}

CLatticeSet::~CLatticeSet()
{
}

void CLatticeSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_GRID, m_pGrid);
	DDX_Control(pDX, IDC_EDIT_LATTICE_WIDTH, m_LatticeWidth);
	DDX_Control(pDX, IDC_EDIT_LATTICE_HEIGHT, m_LatticeHeight);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spinWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spinHeight);
	DDX_Control(pDX, IDC_PROGRESS1, m_proCtrl);
	DDX_Control(pDX, IDC_BUTTON_DES, m_btnDescribe);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnCustomize);
	DDX_Control(pDX, IDC_BUTTON_ALL_DOWN, m_btnDownAll);
	DDX_Control(pDX, IDC_BUTTON_DOWN_BY_TYPE, m_btnDownByType);
	DDX_Control(pDX, IDC_BUTTON_DOWN_SINGLE, m_btnDownSingle);
	DDX_Control(pDX, IDC_BUTTON_SET_GRID, m_btnSetGrid);
	DDX_Control(pDX, IDC_BUTTON_IMPORT_IMAGE, m_btnImportImage);
	DDX_Control(pDX, IDC_BUTTON_FONT, m_btnFont);
	DDX_Control(pDX, IDC_BUTTON_SAVE_LATTICE, m_btnSaveBmp);
	DDX_Control(pDX, IDC_BUTTON_CLEAR_LATTICE, m_btnClearBmp);
	DDX_Control(pDX, IDC_BUTTON_UP, m_btnUp);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_BUTTON_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_btnRight);
	////added by amy
	//DDX_Control(pDX, IDC_BUTTON_SET_GRID, m_btnSet);
}


BEGIN_MESSAGE_MAP(CLatticeSet, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_GRID, &CLatticeSet::OnBnClickedButtonSetGrid)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, &CLatticeSet::OnCbnSelchangeComboMode)
	ON_BN_CLICKED(IDC_BUTTON_DES, &CLatticeSet::OnBnClickedButtonDes)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LATTICE, &CLatticeSet::OnBnClickedButtonClearLattice)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CLatticeSet::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CLatticeSet::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CLatticeSet::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CLatticeSet::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_IMAGE, &CLatticeSet::OnBnClickedButtonImportImage)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LATTICE, &CLatticeSet::OnBnClickedButtonSaveLattice)
	ON_BN_CLICKED(IDC_BUTTON_DOWN_SINGLE, &CLatticeSet::OnBnClickedButtonDownSingle)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CLatticeSet::OnCbnSelchangeComboDownloadType)
	//ON_BN_CLICKED(IDC_BUTTON_LOAD, &CLatticeSet::OnBnClickedButtonLoad)
	ON_CBN_SELCHANGE(IDC_COMBO_LINE_NO, &CLatticeSet::OnCbnSelchangeComboLineNo)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION_NO, &CLatticeSet::OnCbnSelchangeComboStationNo)
	ON_CBN_SELCHANGE(IDC_COMBO_LAN, &CLatticeSet::OnCbnSelchangeComboLan)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_LIST, &CLatticeSet::OnCbnSelchangeComboDataList)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, &CLatticeSet::OnCbnSelchangeComboMode)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &CLatticeSet::OnBnClickedButtonFont)
	ON_BN_CLICKED(IDC_BUTTON_DOWN_BY_TYPE, &CLatticeSet::OnBnClickedButtonDownByType)
	ON_BN_CLICKED(IDC_BUTTON_ALL_DOWN, &CLatticeSet::OnBnClickedButtonAllDown)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CLatticeSet::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CLatticeSet::OnBnClickedButtonDelete)
	ON_MESSAGE(WM_ADD_MSG, &CLatticeSet::OnAddMsg)
	ON_MESSAGE(WM_DELETE_MSG, &CLatticeSet::OnDeleteMsg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_EDIT_INPUT_TEXT, &CLatticeSet::OnEnChangeEditInputText)
END_MESSAGE_MAP()


// CLatticeSet message handlers

BOOL CLatticeSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UDACCEL udaccel;
	memset(&udaccel, 0, sizeof(udaccel));
	udaccel.nInc = 8;
	udaccel.nSec = 0;
	m_spinWidth.SetBuddy(GetDlgItem(IDC_EDIT_LATTICE_WIDTH));
	m_spinWidth.SetAccel(1, &udaccel);
	m_spinWidth.SetRange(0,STATION_WIDTH);
	m_spinWidth.SetPos(160); 

	m_spinHeight.SetBuddy(GetDlgItem(IDC_EDIT_LATTICE_HEIGHT));
	m_spinHeight.SetRange(0,MAX_HEIGHT);
	m_spinHeight.SetPos(16);

	DlgInit();
	DoLoadCfg();
	//////////////////////////////////
	CRect rectR;
	GetDlgItem(IDC_EDIT_INPUT_TEXT)->GetWindowRect(&rectR);//��ȡ�ؼ��������Ļ��λ��
	ScreenToClient(rectR);//ת��Ϊ�Ի����ϵ����λ��
	GetDlgItem(IDC_EDIT_INPUT_TEXT2)->MoveWindow(rectR);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//������������λͼ��Դ
void CLatticeSet::LoadBmpImage()
{
	CString strNormal;
	CString strDown;
	CString strUp;
	CString strDisable;

	LoadMyImage(&m_btnFont, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T("//ImageSrc//Button_Disable.png"));
	LoadMyImage(&m_btnCustomize, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnDescribe, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T("//ImageSrc//Button_Disable.png"));
	LoadMyImage(&m_btnImportImage, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"),_T("//ImageSrc//Button_Disable.png"));
	LoadMyImage(&m_btnSetGrid, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T("//ImageSrc//Button_Disable.png"));
	LoadMyImage(&m_btnDownAll, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnDownByType, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnDownSingle, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnClearBmp, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	LoadMyImage(&m_btnSaveBmp, _T("//ImageSrc//Button_Normal.png"), _T("//ImageSrc//Button_Push.png"), _T("//ImageSrc//Button_Enter.png"), _T(""));
	
	LoadMyImage(&m_btnUp, _T("//ImageSrc//Up_Normal.png"), _T("//ImageSrc//Up_Push.png"), _T("//ImageSrc//Up_Enter.png"), _T(""));
	LoadMyImage(&m_btnDown, _T("//ImageSrc//Down_Normal.png"), _T("//ImageSrc//Down_Push.png"), _T("//ImageSrc//Down_Enter.png"), _T(""));
	LoadMyImage(&m_btnLeft, _T("//ImageSrc//Left_Normal.png"), _T("//ImageSrc//Left_Push.png"), _T("//ImageSrc//Left_Enter.png"), _T(""));
	LoadMyImage(&m_btnRight, _T("//ImageSrc//Right_Normal.png"), _T("//ImageSrc//Right_Push.png"), _T("//ImageSrc//Right_Enter.png"), _T(""));
}

void CLatticeSet::OnBnClickedButtonSetGrid()
{
	CString str;
	GetDlgItem(IDC_EDIT_LATTICE_WIDTH)->GetWindowText(str);
	int nWidth = GetDlgItemInt(IDC_EDIT_LATTICE_WIDTH);
	int nHeight = GetDlgItemInt(IDC_EDIT_LATTICE_HEIGHT);
	int isel = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	switch (isel)
	{
	case LINEINFO:
		{
			if (nWidth > AD_WIDTH)
			{
				//AfxMessageBox(_T("The Max Width of Station Picture is 256!"));
				MessageBox(_T("The Max Width of Station Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
				nWidth = STATION_WIDTH;
			}
		}
		break;
	case TIP:
		{
			if (nWidth > AD_WIDTH)
			{
				//AfxMessageBox(_T("The Max Width of Tip Picture is 1024!"));
				MessageBox(_T("The Max Width of Tip Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
				nWidth = TIP_WIDTH;
			}
		}
		break;
	case AD:
		{
			if (nWidth > AD_WIDTH)
			{
				//AfxMessageBox(_T("The Max Width of Ad Picture is 4096!"));
				MessageBox(_T("The Max Width of Ad Picture is 4096!!"), _T("Prompt"), MB_ICONWARNING);
				nWidth = AD_WIDTH;
			}
		}
		break;
	case BROADCAST:
		{
			if (nWidth > BC_WIDTH)
			{
				//AfxMessageBox(_T("The Max Width of Broadcast Picture is 4096!"));
				MessageBox(_T("The Max Width of Ad Picture is 4096!!"), _T("Prompt"), MB_ICONWARNING);
				nWidth = BC_WIDTH;
			}
		}
		break;
	}
	if (nHeight > MAX_HEIGHT)
	{
		//AfxMessageBox(_T("The Max Height of Picture is 16!"));
		MessageBox(_T("The Max Height of Picture is 16!"), _T("Prompt"), MB_ICONWARNING);
		nHeight = MAX_HEIGHT;
	}

	if (nWidth%8)
	{
		MessageBox(_T("The Width of Grid must be a Mutiple of 8!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	SetLatticeSize(nWidth,nHeight);
}

void CLatticeSet::SetLatticeSize( int iWidth,int iHeight )
{
	CString str;
	if (iWidth > MAX_WIDTH)
	{
		//AfxMessageBox(_T("Max Width is 4096!"));
		MessageBox(_T("Max Width is 4096!"), _T("Prompt"), MB_ICONWARNING);
		iWidth = MAX_WIDTH;
	}
	str.Format(_T("%d"),iWidth);
	GetDlgItem(IDC_EDIT_LATTICE_WIDTH)->SetWindowText(str);
	if (iHeight > MAX_HEIGHT)
	{
		//AfxMessageBox(_T("Max Height is 16!"));
		MessageBox(_T("Max Height is 16!"), _T("Prompt"), MB_ICONWARNING);
		iHeight = MAX_HEIGHT;
	}
	str.Format(_T("%d"),iHeight);
	GetDlgItem(IDC_EDIT_LATTICE_HEIGHT)->SetWindowText(str);

	m_pGrid.SetEditable(false);
	//m_pGrid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0)); //��ɫ����
	m_pGrid.EnableDragAndDrop(false);
	try {
		m_pGrid.SetRowCount(iHeight); //��������Ϊk��
		m_pGrid.SetColumnCount(iWidth);   //k��
		m_pGrid.SetFixedRowCount(0);   //������Ϊһ��
		m_pGrid.SetFixedColumnCount(0);  //ͬ��
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
	//m_pGrid.AutoSize();

	//--------------�������о�------------------
	
		for(int a=0;a<m_pGrid.GetRowCount();a++)
		{
			m_pGrid.SetRowHeight(a,5);  //���ø��и�
		}
		for(int a=0;a<m_pGrid.GetColumnCount();a++)
		{
			m_pGrid.SetColumnWidth(a,5); //���ø��п�
		}	
}


void CLatticeSet::DlgInit()
{
	LoadBmpImage();		//������������λͼ��Դ

	((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->EnableWindow(FALSE);

	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_LED_TYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_LED_COLOR))->SetCurSel(0);

	
	((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_STATION_NO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_MODE))->SetCurSel(0);	

	GetDlgItem(IDC_BUTTON_IMPORT_IMAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DES)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_INPUT_TEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_LED_COLOR)->EnableWindow(FALSE);

	(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT2)->ShowWindow(FALSE);

	SetLatticeSize(96,16);		//��ʼ��
}


void CLatticeSet::DrawPic( int iRow,int iCol,int iFlag )
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

void CLatticeSet::OnCbnSelchangeComboMode()
{
	// TODO: Add your control notification handler code here
	CComboBox *pSelect = (CComboBox*)GetDlgItem(IDC_COMBO_MODE);
	if (pSelect->GetCurSel() == 0)
	{
		GetDlgItem(IDC_BUTTON_IMPORT_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DES)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_INPUT_TEXT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_IMPORT_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DES)->EnableWindow(FALSE);
		//GetDlgItem(IDC_EDIT_INPUT_TEXT)->EnableWindow(FALSE);
	}
}

void CLatticeSet::OnBnClickedButtonDes()
{
	int iselect = ((CComboBox*)GetDlgItem(IDC_COMBO_MODE))->GetCurSel();
	if (iselect == 0)
	{
		//�ַ�ģʽ
		CString sCharText;
		GetDlgItemText(m_inputeditidshow,sCharText);
	    
		int nRow = m_pGrid.GetRowCount();
		int nCol = m_pGrid.GetColumnCount();

		if ((nRow == 0) && (nCol == 0))        //�жϵ��������Ƿ��������
		{
			/*AfxMessageBox(_T("Lattice has not been set yet, Please make a lattice first"));*/
			MessageBox(_T("Lattice has not been set yet, Please make a lattice first"), _T("Prompt"), MB_ICONWARNING);
			return;
		}
		else
		{
			if (sCharText.IsEmpty())      //�ж��Ƿ�������Ч������
			{
				//AfxMessageBox(_T("The text has not been written yet, Please write the text and then describe it"));
				MessageBox(_T("The text has not been written yet, Please write the text and then describe it"), _T("Prompt"), MB_ICONWARNING);
				(CEdit *)GetDlgItem(m_inputeditidshow)->SetFocus();
				return;
			}
			else
			{
				CClientDC dc(GetDlgItem(IDC_STATIC_TEXT)); 
				Bitmap  *myImage =  new Bitmap(4100, 32, PixelFormat24bppRGB);   //�����ƶ���С��BMPλͼ�ļ�
				Graphics graphics(dc);
				Graphics *g = Graphics::FromImage(myImage);

				g->Clear(Color::White);           //���½���ͼ�񱳾�ת��Ϊ��ɫ

				PointF drawPoint =  PointF(0, 0);                        //��ά��������

				Gdiplus::SolidBrush SolidBrush2(Gdiplus::Color(255,0,0));

				//��ȡ��ǰ��������
				static CFont m_font;
				LOGFONT lf;
				memset(&lf, 0, sizeof(LOGFONT));		//added by amy
				GetDlgItem(m_inputeditidshow)->GetFont()->GetLogFont(&lf);
				m_font.DeleteObject();
				m_font.CreateFontIndirect(&lf);
			
				//Gdiplus::FontFamily fontFamily(m_sFontName);
				//Gdiplus::Font myFont(&fontFamily,16,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
				Gdiplus::Font gdiplusFont( dc, m_font ); 
				//���ַ�ת��
				WCHAR   *wChar = NULL; 
				const char *pFilePathName =  (char *)sCharText.Trim().GetBuffer();
				wChar = sCharText.Trim().AllocSysString(); 
				WCHAR string[MAX_PATH*100];
				wcscpy(string, wChar);
				g->DrawString(string, (UINT)wcslen(string),&gdiplusFont,drawPoint,&SolidBrush2);		
				
				//ѭ��������
				/*
				for (UINT i = 0; i <= (myImage->GetWidth() - 1); i++)
				{
					for (UINT j = 0; j <= (myImage->GetHeight() - 1); j++)
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
				}*/
				int end=0,begin=0;
				int Hend=0,Hbegin=0;
				BOOL isfirstflag=TRUE;
				//ѭ��������
				int width=myImage->GetWidth();
				int higth=myImage->GetHeight();
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
					MessageBox(_T("Max Height is 16!"));
					return;

				}
				/////////////////////////////////////////
				
				int maxwithlat=0;
				int typesel=((CComboBox *)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
				if(typesel==0)
					maxwithlat=256;
				else if(typesel==1)
					maxwithlat=1024;
				else if(typesel==2)
					maxwithlat=4096;
				else if(typesel==3)
					maxwithlat=4096;
				/////////////////////////////////////////
				
				int mapwith=Hend+1+(8-(Hend+1)%8);
				if(mapwith>maxwithlat)
				{
					CString strwithshowmsg;
					strwithshowmsg.Format(_T("Max Width is %d"),maxwithlat);
					MessageBox(strwithshowmsg);
					return;
				}
				SetLatticeSize(mapwith,16);
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
							OnBnClickedButtonDown();

						}
						break;

					}

				}
				//////////////////////////////////////////////
				/*
                int withlength=(mapwith-(Hend-Hbegin+1))/2;
				if(Hbegin>=withlength)
				{
					for(int mu=0;mu<Hbegin-withlength;mu++)
					{
						OnBnClickedButtonLeft();
					}

				}
				else
				{
					for(int mu=0;mu<withlength-Hbegin;mu++)
					{
						OnBnClickedButtonRight();
					}

				}*/
				int widthlat=Hend-Hbegin+1;
				int addwdth=(8-widthlat%8)%8;
				int leftaddwidth=addwdth/2;
				if(Hbegin>leftaddwidth)
				{
					for(int mu=0;mu<Hbegin-leftaddwidth;mu++)
					{
						OnBnClickedButtonLeft();
					}

				}
				else
				{
					for(int mu=0;mu<leftaddwidth-Hbegin;mu++)
					{
						OnBnClickedButtonRight();
					}

				}
				SetLatticeSize(widthlat+addwdth,16);
				//SetDlgItemInt(IDC_EDIT_LATTICE_WIDTH,widthlat+addwdth);
				//�ͷ�λͼ��Դ
				if(myImage != NULL)
				{
					delete myImage;
					myImage = NULL;
				}
			}
		}
		m_pGrid.Invalidate(FALSE);
		
	}
	else
	{
		//����ͼƬģʽ
		int nRow = m_pGrid.GetRowCount();
		int nCol = m_pGrid.GetColumnCount();
		if ((nRow == 0) || (nCol == 0))        //�жϵ��������Ƿ��������
		{
			//AfxMessageBox(_T("Please Set Lattice First!"));
			MessageBox(_T("Please Set Lattice First!"), _T("Prompt"), MB_ICONWARNING);
			return;
		}
		else if (m_bmImage == NULL)
		{
			//AfxMessageBox(_T("Please Import Valid Image First!"));
			MessageBox(_T("Please Import Valid Image First!"), _T("Prompt"), MB_ICONWARNING);
			return;
		}
		for (UINT i = 0; i <= (m_bmImage->GetWidth() - 1); i++)
		{
			for (UINT j = 0; j <= (m_bmImage->GetHeight() - 1); j++)
			{
				Color pixelColor;
				m_bmImage->GetPixel(i, j,&pixelColor);

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
		if (m_bmImage != NULL)
		{
			delete m_bmImage;
			m_bmImage = NULL;
		}
	}
}

void CLatticeSet::OnBnClickedButtonClearLattice()
{
	// TODO: Add your control notification handler code here
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	if ((iRowCount> 0) & (iColumnCount > 0))
	{
		m_pGrid.SetRowCount(0);
		m_pGrid.SetColumnCount(0);
	}
}

void CLatticeSet::OnBnClickedButtonUp()
{
	// TODO: Add your control notification handler code here
	int iRowCount, iColumnCount;
	iRowCount = m_pGrid.GetRowCount();
	iColumnCount = m_pGrid.GetColumnCount();

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
	m_pGrid.Invalidate(false);
}

void CLatticeSet::OnBnClickedButtonLeft()
{
	// TODO: Add your control notification handler code here
	int iRowCount, iColumnCount;
	iRowCount = m_pGrid.GetRowCount();
	iColumnCount = m_pGrid.GetColumnCount();

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
	m_pGrid.Invalidate(false);
}

void CLatticeSet::OnBnClickedButtonRight()
{
	// TODO: Add your control notification handler code here
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();

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
	m_pGrid.Invalidate(false);
}

void CLatticeSet::OnBnClickedButtonDown()
{
	// TODO: Add your control notification handler code here
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();

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
	m_pGrid.Invalidate(false);
}

void CLatticeSet::OnBnClickedButtonImportImage()
{
	// TODO: Add your control notification handler code here
	CString sFilePathName;
	CString szFilterFDlg = _T("Bitmap (*.bmp)|*.bmp||");
	WCHAR *filename;
	CFileDialog fileDlg( TRUE, NULL, NULL,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilterFDlg,  NULL );

	if(fileDlg.DoModal() == IDOK)
	{
		sFilePathName = fileDlg.GetPathName();

		//����ͼƬ
		loadImageFromPath(sFilePathName);

		filename= sFilePathName.Trim().AllocSysString(); 
		m_bmImage = new Bitmap(filename);

		int iSelType = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();

		UINT uWidth = m_bmImage->GetWidth();
		UINT uHeight = m_bmImage->GetHeight();	

		//deleted by Amy 2013-01-14
		//if (uWidth > MAX_WIDTH)
		//{
		//	switch (iSelType)
		//	{
		//	case LINEINFO:
		//		//AfxMessageBox(_T("The Max Width of the Picture is 256,the Max height is 16"));
		//		MessageBox(_T("The Max Width of the Picture is 256!"), _T("Prompt"), MB_ICONWARNING);
		//		uWidth = STATION_WIDTH;
		//		break;
		//	case TIP:
		//		//AfxMessageBox(_T("The Max Width of the Picture is 1024,the Max height is 16"));
		//		MessageBox(_T("The Max Width of the Picture is 1024!"), _T("Prompt"), MB_ICONWARNING);
		//		uWidth = TIP_WIDTH;
		//		break;
		//	case AD:
		//		//AfxMessageBox(_T("The Max Width of the Picture is 4096,the Max height is 16"));
		//		MessageBox(_T("The Max Width of the Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
		//		uWidth = AD_WIDTH;
		//		break;
		//	case BROADCAST:
		//		//AfxMessageBox(_T("The Max Width of the Picture is 4096,the Max height is 16"));
		//		MessageBox(_T("The Max Width of the Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
		//		uWidth = BC_WIDTH;
		//		break;
		//	default:
		//		uWidth = MAX_WIDTH;
		//		break;
		//	}
		//}

		switch (iSelType)
		{
		case LINEINFO:
			if (uWidth > STATION_WIDTH)
			{
				MessageBox(_T("The Max Width of the Picture is 256!"), _T("Prompt"), MB_ICONWARNING);
				uWidth = STATION_WIDTH;
			}
			break;
		case TIP:
			if (uWidth > TIP_WIDTH)
			{
				MessageBox(_T("The Max Width of the Picture is 1024!"), _T("Prompt"), MB_ICONWARNING);
				uWidth = TIP_WIDTH;
			}
			break;
		case AD:
			if (uWidth > AD_WIDTH)
			{
				MessageBox(_T("The Max Width of the Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
				uWidth = AD_WIDTH;
			}
			break;
		case BROADCAST:
			if (uWidth > BC_WIDTH)
			{
				MessageBox(_T("The Max Width of the Picture is 4096!"), _T("Prompt"), MB_ICONWARNING);
				uWidth = BC_WIDTH;
			}
			break;
		default:
			uWidth = MAX_WIDTH;
			break;
		}

		if (uHeight > MAX_HEIGHT)
		{
			MessageBox(_T("the Max height of the Picture is 16!"), _T("Prompt"), MB_ICONWARNING);
			uHeight = MAX_HEIGHT;
		}

		if (0 != (uWidth%8))
		{
			uWidth -= uWidth%8;
		}

		////�����п�//�����и�
		//SetLatticeSize( m_bmImage->GetWidth(), m_bmImage->GetHeight());
		SetLatticeSize( uWidth, uHeight);

		//�����Ԫ������
		for(UINT i = 0;i <= (/*m_bmImage->GetWidth()*/uWidth - 1); i++)
		{
			for (UINT j = 0; j <= (/*m_bmImage->GetHeight()*/uHeight - 1); j++)
			{
				DrawPic(j,i,0);
			}
		}
		OnBnClickedButtonDes();//����
	}

	m_pGrid.Invalidate(false);
}

void CLatticeSet::loadImageFromPath( CString sPath )
{
	if (m_bmImage != NULL)
	{
		delete m_bmImage;
		m_bmImage = NULL;
	}

	//ͼƬ����
	m_hBitmap = (HBITMAP)LoadImage(::AfxGetInstanceHandle(),sPath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (m_hBitmap == NULL)
	{
		return;
	}
	else if (m_hBitmap != NULL)
	{
		//ShowImage();
	}	
}

void CLatticeSet::SaveClttice( int iPicWidth,int iPicHeight,CString sSavepath )
{
	Pen *myPen = new Pen(Color::Black);            //������ɫ����
	Pen * myWhitePen = new Pen(Color::White);      //������ɫ����
	Bitmap *tempImage = new Bitmap(iPicWidth,iPicHeight,PixelFormat24bppRGB);      //���ݵ����С����λͼ
	CClientDC dc(GetDlgItem(IDC_STATIC_TEXT));
	Graphics graphics(dc);
	Graphics *g = graphics.FromImage(tempImage);
	g->Clear(Color::White);                       //���½���ͼ�񱳾�ת��Ϊ��ɫ

	for(int i = 0; i <= (iPicWidth - 1); i++)       //��ѭ��
	{
		for(int j = 0; j<= (iPicHeight - 1); j++)   //��ѭ��
		{
			GV_ITEM Item;

			Item.mask = GVIF_TEXT;
			Item.row = j;
			Item.col = i;
			Item.mask |= (GVIF_BKCLR|GVIF_FGCLR);
			m_pGrid.GetItem(&Item);
			if (Item.crBkClr == RGB(255,0,0)) //����Ϊ��ɫ
			{
				g->DrawRectangle(myPen,i,j,1,1);             //����ɫ
			}

			else
			{
				g->DrawRectangle(myWhitePen, i, j, 1, 1);    //����ɫ                 
			}    
		}
	}

	//����·�����ַ�ת��
	WCHAR   *wChar; 
	wChar = sSavepath.Trim().AllocSysString(); 

	CLSID clsid;
	int nRet=0;
	nRet = GetEncoderClsid(L"image/bmp",&clsid);//�õ�CLSID
	//USES_CONVERSION;
	if(nRet>=0)
	{  
		CString sMsg;
		Status status = tempImage->Save(wChar,&clsid);	
		if(Ok != status)
		{
			sMsg.Format(_T("\nFailed to save image in %s file\n GDI+ Error: %u"),sSavepath,status);
			//AfxMessageBox(sMsg);
			MessageBox(sMsg , _T("Prompt"), MB_ICONWARNING);
		}
		else
		{
			//AfxMessageBox(_T("Save config information successfully!"));
			MessageBox(_T("Save config information successfully") , _T("Prompt"), MB_OK);
		}
	}
	if (myPen != NULL)
	{
		delete myPen;
		myPen = NULL;
	}
	if (myWhitePen != NULL)
	{
		delete myWhitePen;
		myWhitePen = NULL;
	}
	if (tempImage != NULL)
	{
		delete tempImage;
		tempImage = NULL;
	}
}

int CLatticeSet::GetEncoderClsid( const WCHAR *format, CLSID *pClsid )
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
void CLatticeSet::OnBnClickedButtonSaveLattice()
{	
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	if ((iRowCount == 0) || (iColumnCount == 0))
	{
		//AfxMessageBox(_T("Lattice has not been set yet, can not be saved!"));
		MessageBox(_T("Lattice has not been set yet, can not be saved!") , _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else
	{ 
		int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
		int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->GetCurSel();
		int iStationNum = ((CComboBox*)GetDlgItem(IDC_COMBO_STATION_NO))->GetCurSel();
		int iDownloadType = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
		int iDataIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->GetCurSel();

		CString strPicPath,strPicName,strTemp;
		if (iSelLan == LAN_EN)
		{
			strPicPath.Format(_T("%s\\Pic\\En\\"),theApp.GetAppPath());
		}
		else
		{
			strPicPath.Format(_T("%s\\Pic\\Iran\\"),theApp.GetAppPath());
		}
		switch (iDownloadType)
		{
		case LINEINFO:
			{
				strPicName.Format(_T("%sLine%d_Station%d.bmp"),strPicPath,iSelLine+1,iStationNum+1);
			}
			break;
		case TIP:
			{
				strPicName.Format(_T("%sTips%d.bmp"),strPicPath,iDataIndex+1);
				if (iDataIndex == -1) return;
			}
			break;
		case AD:
			{
				strPicName.Format(_T("%sAd%d.bmp"),strPicPath,iDataIndex+1);
				if (iDataIndex == -1) return;
			}
			break;
		case BROADCAST:
			{
				strPicName.Format(_T("%sBc%d.bmp"),strPicPath,iDataIndex+1);
				if (iDataIndex == -1) return;
			}
			break;
		}
		//ƴ����������·��
		DeleteFile(strPicName);
		SaveClttice(iColumnCount, iRowCount, strPicName);
		/////////////////////////////////////////////////////////////////
		CString strgrounp,strkey,strvalue;
		CString strIni = _T("\\PTU.ini");
		strIni = theApp.GetAppPath() + strIni;
		switch (iDownloadType)
		{
		case 0:
			{
				if(iSelLine==0)
					strgrounp=_T("StationLine1");
				if(iSelLine==1)
					strgrounp=_T("StationLine2");
				if(iSelLine==2)
					strgrounp=_T("StationLine3");
				strkey.Format(_T("Station%d"),iStationNum);
			}
			break;
		case 1:
			{
				
				GetPrivateProfileString(_T("TIP"), _T("TipsCount"), _T(""), strvalue.GetBuffer(MAX_PATH), MAX_PATH, strIni);	
                strvalue.ReleaseBuffer();
				int tipcount = _ttoi(strvalue);
				if(iDataIndex>=tipcount)
				{
					strgrounp=_T("ExtraTips");
					strkey.Format(_T("Tips%d"),iDataIndex-tipcount);
				}
				else
				{
					strgrounp=_T("TIP");
					strkey.Format(_T("Tips%d"),iDataIndex);

				}
			}
			break;
		case 2:
			{
				strgrounp=_T("AD");
				strkey.Format(_T("Ad%d"),iDataIndex);
			}
			break;
		case 3:
			{
				strgrounp=_T("BROADCAST");
				strkey.Format(_T("BroadCast%d"),iDataIndex);

			}
			break;
		}
		GetPrivateProfileString(strgrounp, strkey, _T(""), strvalue.GetBuffer(1024/*MAX_PATH*/), 1024, strIni);	
        strvalue.ReleaseBuffer();
		CString stredit;
        CString strwritevalue;
		
		GetDlgItemText(m_inputeditidshow,stredit);
		if(iSelLan==0)
		{
			int index=strvalue.Find(_T(";"));
			
			CString strIran=strvalue.Mid(index);

			strwritevalue=stredit+strIran;
		}
		if(iSelLan==1)
		{
			int index=strvalue.Find(_T(";"));
			CString strenglish=strvalue.Mid(0,index+1);
			strwritevalue=strenglish+stredit;
		}
		WritePrivateProfileString(strgrounp, strkey, strwritevalue, strIni);

		((CMainFrame *)GetParent())->ReadCfgFromIni();
		//(((CPTUDlg *)GetParent())->m_cStationCfg).SendMessage(WM_ADD_MSG, NULL, NULL);		
	}
}

void CLatticeSet::OnBnClickedButtonDownSingle()
{	
	int iSelColor = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_COLOR))->GetCurSel();//���ص���ɫ
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//���ص�����
	//int iSelDowaloadMethod = ((CComboBox*)GetDlgItem(IDC_COMBO_DOWNLOAD_METHOD))->GetCurSel();//���ط�ʽ
	int iSelDowaloadType = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();//���ص���������
	int iSelLedType = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_TYPE))->GetCurSel();//��������
	int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->GetCurSel();//վ�����
	int iStationNum = ((CComboBox*)GetDlgItem(IDC_COMBO_STATION_NO))->GetCurSel();//վ�����
	int iSelDataList = ((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->GetCurSel(); //��վ�������������

	//if (iSelDowaloadMethod == USB)
	//{
	//	if ( !((CPTUDlg*)AfxGetMainWnd())->m_bOpenUSB)
	//	{
	//		AfxMessageBox(_T("USB Device Not Opened!!!"));
	//		return;
	//	}
	//}
	//else if (iSelDowaloadMethod == COM)
	//{
	//	if ( !((CPTUDlg*)AfxGetMainWnd())->m_bOpenCom)
	//	{
	//		AfxMessageBox(_T("Com Port Not Opened!!!"));
	//		return;
	//	}
	//}

	int iSelDowaloadMethod;
	if ((!((CMainFrame*)AfxGetMainWnd())->m_comisopen) && (!((CMainFrame*)AfxGetMainWnd())->m_ctcusbisopen))
	{
		//AfxMessageBox(_T("Communication Port Not Opened!!!"));
		MessageBox(_T("Communication Port Not Opened!!!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	else if(((CMainFrame*)AfxGetMainWnd())->m_comisopen)
	{
		iSelDowaloadMethod = 0;
	}
	else if (((CMainFrame*)AfxGetMainWnd())->m_ctcusbisopen)
	{
		iSelDowaloadMethod = 1;
	}
	else
	{
		return;
	}
	
	GetBmpByte();//��ȡ��������
	WORD wPicWidth = (WORD)m_pGrid.GetColumnCount();
	WORD wPicHeight = (WORD)m_pGrid.GetRowCount();

	int nBmpCount = m_vBmpByte.size();
	if(nBmpCount < 1) 
	{
		//AfxMessageBox(_T("Bmp Data Error!"));
		MessageBox(_T("Bitmap Data Error!") , _T("Prompt"), MB_ICONWARNING);
		return;
	}
	//��������XOR
	BYTE bXor = m_vBmpByte.at(0);
	for (int i=1;i<nBmpCount;i++)
	{
		bXor = bXor^m_vBmpByte[i];
	}
	WORD wAddCheckSum = 0;
	//�������ݲ��ݵ�Add��
	for (int i=0;i<nBmpCount; i++ )
	{
		wAddCheckSum = wAddCheckSum + m_vBmpByte.at(i);
	}
	//��У��������ֽڷŵ�ͼƬ������
	m_vBmpByte.push_back(bXor);
	m_vBmpByte.push_back((BYTE)(wAddCheckSum & 0x00FF));//�����λ
	m_vBmpByte.push_back((BYTE)(wAddCheckSum>>8));//�����λ
	BYTE t = m_vBmpByte.at(m_vBmpByte.size()-1);
	//ʵ��ͼƬ����ΪnBmpCount + 3
	nBmpCount +=3;

	
	//�������ݷְ���С200byte
	int nPicMaxPack = 200;
	int nPackCount = 1; //���ݷְ���С
	BOOL bSendFinshed = FALSE;
	//������ɣ����˳�while����Ҫ���Ƿְ����������ֹ��������
	//����а�������255�ģ����С�ְ���С
	while (!bSendFinshed)
	{
		//�����������ݵİ���
		if ((nBmpCount % nPicMaxPack) == 0)
		{
			nPackCount = (nBmpCount / nPicMaxPack);//����
		}
		else 
		{
			nPackCount = (nBmpCount / nPicMaxPack) + 1;
		}
		int nPicDataAddr = 0;//��ȡͼƬ���ݵĿ�ʼλ��
		//////////////////////////////////////////////////////////////////////////
		for (int iPackIndex=1;iPackIndex <= nPackCount;iPackIndex++)
		{
			BYTE pDataBuffer[255] = {0};//����һ�������������
			PIS_PACK_HEAD   *pHead = (PIS_PACK_HEAD *)&pDataBuffer[0];
			COM_PIC_PACK *pPicPack = (COM_PIC_PACK*)&pDataBuffer[sizeof(PIS_PACK_HEAD)];

			pHead->bDstNetID = 0xFF;	
			if (iSelLedType == LED_FDU)
			{
				pHead->bDstDevID=  FDU_VALUE;
			}
			else if (iSelLedType == LED_SDU)
			{
				pHead->bDstDevID=  SDU_VALUE;
			}
			else
			{
				pHead->bDstDevID= IDU_VALUE;
			}
			pHead->bSrcNetID = 0x00;
			pHead->bSrcDevID = 0x01;
			//pHead->bCMD = 0x04;//��ʾΪ����	
			pHead->bCMD = CMD_UPDATE;//��ʾΪ����

			//////////////////////////////////////////////////////////////////////////
			pPicPack->bColor = iSelColor+1;
			pPicPack->bPackNum = iPackIndex;
			pPicPack->bType = 2;//ͼƬ
			wPicWidth = (wPicWidth/8)*8;//����Ϊ8��������
			pPicPack->bWidthHi = BYTE(wPicWidth>>8);
			pPicPack->bWidthLow = (BYTE)(0x00FF & wPicWidth);
			pPicPack->bDataType = iSelDowaloadType+1;
			pPicPack->bFlag.bEn = iSelLan;
			//iPackIndex
			if (1 == nPackCount)
			{
				pPicPack->bFlag.bLastPack = 1;
				pPicPack->bFlag.bStart = 1;
				pPicPack->bFlag.bFirstPack = 1;
				pPicPack->bFlag.bStop = 1;
			}
			else if (iPackIndex == 1)
			{
				pPicPack->bFlag.bFirstPack = 1;
				pPicPack->bFlag.bLastPack = 0;
				pPicPack->bFlag.bStart = 1;
				pPicPack->bFlag.bStop = 0;
			}
			else if (iPackIndex == nPackCount)
			{
				pPicPack->bFlag.bFirstPack = 0;
				pPicPack->bFlag.bLastPack = 1;
				pPicPack->bFlag.bStart = 0;
				pPicPack->bFlag.bStop = 1;
			}
			else
			{
				pPicPack->bFlag.bFirstPack = 0;
				pPicPack->bFlag.bLastPack = 0;
				pPicPack->bFlag.bStart = 0;
				pPicPack->bFlag.bStop = 0;
			}
			//pPicPack->bDataType = iSelDowaloadType+1;
			//��ȡ��������
			if (iSelDowaloadType == LINEINFO)
			{
				pPicPack->bDataType = iSelLine+1;    //����
				pPicPack->bDataIndex = iStationNum+1;//�ڼ�վ
			}
			else
			{
				if (iSelDataList == -1)
				{
					AfxMessageBox(_T("Please Select a DataList Data!"));
					return;
				}
				pPicPack->bDataType = iSelDowaloadType+3;//����
				pPicPack->bDataIndex = iSelDataList+1;//�������
			}
			//��ʼ��ȡһ����ͼƬ����
			int nPicDataReadCount = 0;//��ǰ��ͼƬ����
			for (int nSavePos=0 ; (nPicDataAddr < nBmpCount) && nPicDataReadCount < nPicMaxPack; nSavePos++)
			{
				pPicPack->bData[nSavePos] = m_vBmpByte.at(nPicDataAddr);//��ȡͼƬ�������ݵ�����
				nPicDataReadCount++;
				nPicDataAddr++;
			}

			WORD wAppDatalen = (WORD)(nPicDataReadCount + sizeof(COM_PIC_PACK));//Ӧ�ò�����ΪͼƬ���ݣ�����ͷ
			pHead->bDataLen =(BYTE)(wAppDatalen);

			int nTotalCount = sizeof(PIS_PACK_HEAD) + sizeof(COM_PIC_PACK) + nPicDataReadCount;//ȫ�����ݳ���
			//ͳ�������ַ�����Ҫת����ַ�������ж�ת����������ݳ��ȳ���255�������ݰ��ĳ���Ҫ���٣����������
			int nConverCharCout=0;
			for (int i=0;i<nTotalCount;i++)
			{
				if (pDataBuffer[i] == 0x7E || pDataBuffer[i] == 0x7F)
				{
					nConverCharCout++;
				}
			}
			//�ж��ܳ���
			if ((nTotalCount + nConverCharCout) > 252 )
			{
				nPicMaxPack =- 5;//�����ȼ���5���ֽ�
				break;//�ص�whileѭ�����
			}
			BYTE bCheckSum = theApp.GetCheckSum(pDataBuffer,nTotalCount);//����У���
			LPCOMM_DATAPACK pHdlcPack = new COMM_DATAPACK;

			pHdlcPack->nLen = nTotalCount+1; //��Ҫ����һ��У������
			pHdlcPack->pPtr = new BYTE[pHdlcPack->nLen];
			pDataBuffer[nTotalCount] = bCheckSum;//�����������У���ֽ�
			memcpy(pHdlcPack->pPtr,pDataBuffer,nTotalCount+1);
			//((CPTUDlg*)AfxGetMainWnd())->SendHdlcComPack(pHdlcPack);//��������

			if (iSelDowaloadMethod == USB)
			{
				BYTE pUsbDataBuffer[255] = {0};//����һ�������������
				USB_PACK_HEAD* pUsbHead = (USB_PACK_HEAD*)&pUsbDataBuffer[0];
				memcpy(pUsbHead->bData, pHdlcPack->pPtr, pHdlcPack->nLen);		//��ͼƬ���ݼ��뻺�����

				int nUsbTotalCount = sizeof(USB_PACK_HEAD) + pHdlcPack->nLen;
				//pUsbHead->bCMD = 0x19;
				pUsbHead->bCMD = CMD_TRANSFER;
				pUsbHead->bDataLenH = nUsbTotalCount << 8;
				pUsbHead->bDataLenL = nUsbTotalCount;

				BYTE bUsbCheckSum = theApp.GetCheckSum(pUsbDataBuffer, nUsbTotalCount);
				pUsbDataBuffer[nUsbTotalCount] = bUsbCheckSum;
				//LPUSB_DATAPACK pUsbPack = new USB_DATAPACK;
				//pUsbPack->nLen = nUsbTotalCount + 1;
				//pUsbPack->pPtr = new BYTE[pUsbPack->nLen];
				//memcpy(pUsbPack->pPtr, pUsbDataBuffer, pUsbPack->nLen);
			
				//((CPTUDlg*)AfxGetMainWnd())->SendUSBData(((CPTUDlg*)AfxGetMainWnd())->AddHeadAndTail(pUsbPack));

				((CMainFrame *)AfxGetMainWnd())->AddHeadAndTail(pUsbDataBuffer, nUsbTotalCount + 1);

				LPUSB_DATAPACK pUsbPack = new USB_DATAPACK;
				pUsbPack->nLen = ((CMainFrame *)AfxGetMainWnd())->m_nSendCnt;
				pUsbPack->pPtr = new BYTE[pUsbPack->nLen];
				memcpy(pUsbPack->pPtr, ((CMainFrame *)AfxGetMainWnd())->m_bUsbSendBuffer, pUsbPack->nLen);

				((CMainFrame *)AfxGetMainWnd())->SendUSBData(pUsbPack);
				
				//Ϊ��ȷ�����ؿ������ܽ��յ����ݣ�ͼƬ���ݵĵ�һ���跢����
				if (1 == pPicPack->bFlag.bStart)
				{
					Sleep(200);
					((CMainFrame *)AfxGetMainWnd())->SendUSBData(pUsbPack);
				}
				//////////////////////////////////////////////////////////////////////////

				if (pUsbPack->pPtr != NULL)
				{
					delete[] pUsbPack->pPtr;
					pUsbPack->pPtr = NULL;
				}
				if (pUsbPack != NULL)
				{
					delete pUsbPack;
					pUsbPack = NULL;
				}			
				if (pHdlcPack->pPtr != NULL)
				{
					delete[] pHdlcPack->pPtr;
					pHdlcPack->pPtr = NULL;
				}
				if (pHdlcPack != NULL)
				{
					delete pHdlcPack;
					pHdlcPack = NULL;
				}
			}
			else if(iSelDowaloadMethod == COM)
			{
				((CMainFrame*)AfxGetMainWnd())->SendHdlcComPack(pHdlcPack);//��������
				if (1 == pPicPack->bFlag.bStart)
				{
					Sleep(200);
					((CMainFrame*)AfxGetMainWnd())->SendHdlcComPack(pHdlcPack);//��������
				}

				//�ͷ��ڴ�
				if (pHdlcPack->pPtr != NULL)
				{
					delete[] pHdlcPack->pPtr;
					pHdlcPack->pPtr = NULL;
				}
				if (pHdlcPack != NULL)
				{
					delete pHdlcPack;
					pHdlcPack = NULL;
				}
			}
			m_proCtrl.SetRange(0,100);
			int nPercent = iPackIndex *100/nPackCount;
			m_proCtrl.SetPos(nPercent);

			Sleep(200);//�ӳ�200����
		}
		//AfxMessageBox(_T("Download Finished!!!"));
		MessageBox(_T("Download Finished!"), _T("Prompt"), MB_OK);
		m_proCtrl.SetPos(0);
		//����ְ��������򽫰��Ĵ�С��ԭ���ļ�ȥ5���ֽ�
		bSendFinshed = TRUE;
	}//whileѭ��
}

//��λתΪ�ֽ�
byte Littlebyte(byte b7,byte b6,byte b5,byte b4,byte b3,byte b2,byte b1,byte b0)
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

void CLatticeSet::GetBmpByte()
{
	m_vBmpByte.clear();
	int iRowCount = m_pGrid.GetRowCount();
	int iColumnCount = m_pGrid.GetColumnCount();
	//�жϵ��������Ƿ��������
	if ((iRowCount == 0) && (iColumnCount == 0))
	{
		return;
	}
	int ictrl;//������ɨ��
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
				if (Item.crBkClr == RGB(255,0,0)) //����Ϊ��ɫ
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
			m_vBmpByte.push_back(Littlebyte(bArry[7], bArry[6], bArry[5], bArry[4], bArry[3], bArry[2], bArry[1], bArry[0]));
		}   
	}
}

void CLatticeSet::OnCbnSelchangeComboDownloadType()
{
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
	int isel = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();

	GetDlgItem(IDC_COMBO_LINE_NO)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_STATION_NO)->EnableWindow(FALSE);
	//GetDlgItem(IDC_EDIT_STATION_TEXT)->EnableWindow(FALSE);

	CString strTemp;
	if (isel == LINEINFO)
	{
		m_spinWidth.SetRange(0,STATION_WIDTH);
		GetDlgItem(IDC_COMBO_LINE_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_STATION_NO)->EnableWindow(TRUE);
		//GetDlgItem(IDC_EDIT_STATION_TEXT)->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_LED_COLOR)->EnableWindow(FALSE);
		OnCbnSelchangeComboStationNo();		//added by amy 2012-8-22
	}
	else if (isel == TIP)
	{
		m_spinWidth.SetRange(0,TIP_WIDTH);
		UINT nTipsCount = ((CMainFrame *)AfxGetMainWnd())->m_vLineTipName.size();
		UINT nExtraTipsCount = ((CMainFrame *)AfxGetMainWnd())->m_vAddTipsName.size();

		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->ResetContent();
		GetDlgItem(IDC_COMBO_LED_COLOR)->EnableWindow(FALSE);
		for (UINT i=0;i< nTipsCount;i++)
		{
			//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineTipName.at(i);
			//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
			strTemp.Format(_T("%d"), i + 1);
			((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->AddString(strTemp);
		}
		if (nExtraTipsCount > 0)
		{
			for (UINT i=0;i< nExtraTipsCount;i++)
			{
				//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vAddTipsName.at(i);
				//strTemp.Format(_T("%d,%s"),i+1+nTipsCount,xValue[(LAN_TYPE)iSelLan]);
				strTemp.Format(_T("%d"), i + 1 + nTipsCount);
				((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->AddString(strTemp);
			}
		}
		else
		{
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->SetCurSel(0);	//added by amy 2012-8-22
		OnCbnSelchangeComboDataList();		//added by amy 2012-8-22
	}
	else if (isel == AD)
	{
		m_spinWidth.SetRange(0,AD_WIDTH);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_LED_COLOR)->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->ResetContent();

		int nCount = ((CMainFrame *)AfxGetMainWnd())->m_vLineAdName.size();
		if(nCount > 0)
		{
			for (UINT i=0;i< nCount;i++)
			{
				//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineAdName.at(i);
				//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
				strTemp.Format(_T("%d"), i + 1);
				((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->AddString(strTemp);
			}
			((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->SetCurSel(0);	//added by amy 2012-8-22
		}
		else 
		{
		}
		OnCbnSelchangeComboDataList();		//added by amy 2012-8-22
	}
	else if(BROADCAST == isel)
	{
		m_spinWidth.SetRange(0,BC_WIDTH);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_LED_COLOR)->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->ResetContent();

		int nCount = ((CMainFrame *)AfxGetMainWnd())->m_vLineBcName.size();
		//�жϹ㲥����Ϊ��
		if (nCount > 0)
		{
			for (UINT i=0;i< nCount;i++)
			{
				//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineBcName.at(i);
				//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
				strTemp.Format(_T("%d"), i + 1);
				((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->AddString(strTemp);
			}	
			((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->SetCurSel(0);	//added by amy 2012-8-22
		}
		else
		{
		}
		OnCbnSelchangeComboDataList();		//added by amy 2012-8-22
	}
}

void CLatticeSet::OnBnClickedButtonLoad()
{
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
	int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->GetCurSel();
	int nStationNum = ((CComboBox*)GetDlgItem(IDC_COMBO_STATION_NO))->GetCurSel();
	CString strPicPath,strPicName,strTemp;
	if (iSelLan == LAN_EN)
	{
		strPicPath.Format(_T("%s\\Pic\\En\\"),theApp.GetAppPath());
	}
	else
	{
		strPicPath.Format(_T("%s\\Pic\\Iran\\"),theApp.GetAppPath());
	}
	strPicName.Format(_T("%sLine%d_Station%d.bmp"),strPicPath,iSelLine+1,nStationNum+1);
	if (!PathFileExists(strPicName))
	{
		strTemp.Format(_T("Bmp %s Can't Found,Please Set Lattice First!!!"),strPicName);
		//AfxMessageBox(strTemp);
		MessageBox(strTemp, _T("Prompt"), MB_ICONWARNING);
		return;
	}	
	ImportStationPic(strPicName);
}

void CLatticeSet::DoLoadCfg()
{
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
	int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->GetCurSel();
	int nOldSel = ((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->GetCurSel();

	//��ʼ��վ��
	int nStationNum = ((CMainFrame *)AfxGetMainWnd())->m_vLineStationName[iSelLine].size();
	if (nStationNum <= 0)
	{
		((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->ResetContent();
		OnCbnSelchangeComboStationNo();			//added by amy 2012-8-22
		return;
	}

	((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->ResetContent();
	for (int i = 1; i <= nStationNum; ++i)
	{
		CString sStationNum;
		sStationNum.Format(_T("%d"),i);
		((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->AddString(sStationNum);
	}
   
    ((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->SetCurSel(0);///////////////////////////////////
  
      
	XString sStationName = ((CMainFrame *)AfxGetMainWnd())->m_vLineStationName[iSelLine].at(0);
	//GetDlgItem(IDC_EDIT_STATION_TEXT)->SetWindowText(sStationName[(LAN_TYPE)iSelLan]);

	//////////////////////////////////////////////////////////////////////////
	int iDownloadType = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	CComboBox *pComSelectType = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST);
	nOldSel = pComSelectType->GetCurSel();
	CString strTemp;
	if (iDownloadType == LINEINFO)
	{
		pComSelectType->EnableWindow(FALSE);
		pComSelectType->SetCurSel(0);
		OnCbnSelchangeComboStationNo();			//added by amy 2012-8-22

	}
	else if (iDownloadType == TIP)
	{
		pComSelectType->EnableWindow(TRUE);
		pComSelectType->ResetContent();

		UINT uCount = ((CMainFrame *)AfxGetMainWnd())->m_vAddTipsName.size() + ((CMainFrame *)AfxGetMainWnd())->m_vLineTipName.size();
		for (UINT i=0;i< uCount;i++)
		{
			//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineTipName.at(i);
			//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
			strTemp.Format(_T("%d"), i + 1);
			pComSelectType->AddString(strTemp);
		}
		pComSelectType->SetCurSel(0);
		OnCbnSelchangeComboDataList();			//added by amy 2012-8-22
	}
	else if (iDownloadType == AD)
	{
		pComSelectType->EnableWindow(TRUE);
		pComSelectType->ResetContent();
		for (UINT i=0;i< ((CMainFrame *)AfxGetMainWnd())->m_vLineAdName.size();i++)
		{
			//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineAdName.at(i);
			//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
			strTemp.Format(_T("%d"), i + 1);
			pComSelectType->AddString(strTemp);
		}
		pComSelectType->SetCurSel(0);
		OnCbnSelchangeComboDataList();			//added by amy 2012-8-22
	}
	else if(BROADCAST == iDownloadType)
	{
		pComSelectType->EnableWindow(TRUE);
		pComSelectType->ResetContent();
		for (UINT i=0;i< ((CMainFrame *)AfxGetMainWnd())->m_vLineBcName.size();i++)
		{
			//XString xValue = ((CPTUDlg *)AfxGetMainWnd())->m_vLineBcName.at(i);
			//strTemp.Format(_T("%d,%s"),i+1,xValue[(LAN_TYPE)iSelLan]);
			strTemp.Format(_T("%d"), i + 1);
			pComSelectType->AddString(strTemp);
		}	
		pComSelectType->SetCurSel(0);
		OnCbnSelchangeComboDataList();			//added by amy 2012-8-22
	}
}

void CLatticeSet::OnCbnSelchangeComboLineNo()
{
	DoLoadCfg();
}

void CLatticeSet::OnCbnSelchangeComboStationNo()
{
	// TODO: Add your control notification handler code here
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
	int iSelLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO))->GetCurSel();
	int iSel = ((CComboBox *)GetDlgItem(IDC_COMBO_STATION_NO))->GetCurSel();

	int nCount = ((CMainFrame *)AfxGetMainWnd())->m_vLineStationName[iSelLine].size();
	if(nCount <= 0)
	{
		OnBnClickedButtonClearLattice();
		GetDlgItem(m_inputeditidshow)->SetWindowText(_T(""));
		return;
	}

	XString sStationName = ((CMainFrame *)AfxGetMainWnd())->m_vLineStationName[iSelLine].at(iSel);
	//GetDlgItem(IDC_EDIT_STATION_TEXT)->SetWindowText(sStationName[(LAN_TYPE)iSelLan]);
	GetDlgItem(m_inputeditidshow)->SetWindowText(sStationName[(LAN_TYPE)iSelLan]);

	CString strPicPath,strPicName,strTemp;
	if (iSelLan == LAN_EN)
	{
		strPicPath.Format(_T("%s\\Pic\\En\\"),theApp.GetAppPath());
	}
	else
	{
		strPicPath.Format(_T("%s\\Pic\\Iran\\"),theApp.GetAppPath());
	}
	strPicName.Format(_T("%sLine%d_Station%d.bmp"),strPicPath,iSelLine+1,iSel+1);
	if (!PathFileExists(strPicName))
	{
		strTemp.Format(_T("Bmp %s Can't Found,Please Set Lattice First!!!"),strPicName);
		//AfxMessageBox(strTemp);
		MessageBox(strTemp, _T("Prompt"), MB_ICONWARNING);
		OnBnClickedButtonDes();
		return;
	}	
	ImportStationPic(strPicName);//
}

void CLatticeSet::OnCbnSelchangeComboLan()
{
  
#if 0
	int iSelLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//ѡ������
	if(iSelLan==0)
	{
		(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT)->ShowWindow(TRUE);
		(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT2)->ShowWindow(FALSE);
		m_inputeditidshow=IDC_EDIT_INPUT_TEXT;

	}
	if(iSelLan==1)
	{
		(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT)->ShowWindow(FALSE);
		(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT2)->ShowWindow(TRUE);
		m_inputeditidshow=IDC_EDIT_INPUT_TEXT2;

	}
#endif
	(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT)->ShowWindow(TRUE);
	(CEdit *)GetDlgItem(IDC_EDIT_INPUT_TEXT2)->ShowWindow(FALSE);
	m_inputeditidshow=IDC_EDIT_INPUT_TEXT;
	DoLoadCfg();
}

void CLatticeSet::OnCbnSelchangeComboDataList()
{
	XString xValue;
	CString strPicPath,strPicName,strTemp;

	int iSellan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();			//ѡ�������
	int iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_DATA_LIST))->GetCurSel();		//ѡ������
	int iSelType = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();		//ѡ�������

	if ((CB_ERR == iSellan) || (CB_ERR == iSelType))
	{
		return;
	}	

	//����ѡ������Ի�ȡλͼ�����·��
	if (LAN_EN == iSellan )
	{
		strPicPath.Format(_T("%s\\Pic\\En\\"),theApp.GetAppPath());
	}
	else 
	{
		strPicPath.Format(_T("%s\\Pic\\Iran\\"),theApp.GetAppPath());
	}

	//����ѡ�������������ؼ�����ʾλͼ��Ϣ
	if (iSelType == TIP)
	{
		if (CB_ERR == iSel)
		{
			OnBnClickedButtonClearLattice();
			GetDlgItem(m_inputeditidshow)->SetWindowText(_T(""));
			return;
		}

		UINT nTipsCount = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.size();
		if (iSel < nTipsCount)
		{
			xValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineTipName.at(iSel);
		}
		else
		{
			xValue = ((CMainFrame*)AfxGetMainWnd())->m_vAddTipsName.at(iSel - nTipsCount );
		}	
		strPicName.Format(_T("%sTips%d.bmp"),strPicPath,iSel+1);

	}
	else if (iSelType == AD)
	{
		if (CB_ERR == iSel)
		{
			OnBnClickedButtonClearLattice();
			GetDlgItem(m_inputeditidshow)->SetWindowText(_T(""));
			return;
		}

		xValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineAdName.at(iSel);
		strPicName.Format(_T("%sAd%d.bmp"),strPicPath,iSel+1);
	}
	else if (iSelType == BROADCAST)
	{
		if (CB_ERR == iSel)
		{
			OnBnClickedButtonClearLattice();
			GetDlgItem(m_inputeditidshow)->SetWindowText(_T(""));
			return;
		}

		 xValue = ((CMainFrame*)AfxGetMainWnd())->m_vLineBcName.at(iSel);
		 strPicName.Format(_T("%sBc%d.bmp"),strPicPath,iSel+1);
	}
	else
	{
		return;
	}
	//MessageBox(xValue[(LAN_TYPE)iSellan]);
	GetDlgItem(m_inputeditidshow)->SetWindowText(xValue[(LAN_TYPE)iSellan]);

	if (!PathFileExists(strPicName))
	{
		strTemp.Format(_T("Can't Find %s Bmp!!!"),strPicName);
		MessageBox(strTemp, _T("Prompt"), MB_ICONWARNING);
		OnBnClickedButtonDes();
		return;
	}

	//��ָ��·���ж�ȡλͼ����ʾ������ؼ���
	ImportStationPic(strPicName);
}

void CLatticeSet::ImportStationPic( CString strFileName )
{
	char szANSIString [MAX_PATH]; 
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, strFileName, -1, szANSIString, sizeof(szANSIString), NULL, NULL ); 
	
	//����ļ��Ƿ����
	FILE *fp = fopen(szANSIString, "r");
	if(NULL == fp)
	{
		m_pGrid.SetRowCount(0);
		m_pGrid.SetColumnCount(0);
		return;
	} 

	WCHAR   *wChar; 
	wChar= strFileName.Trim().AllocSysString();

	Bitmap * bmAllImage = new Bitmap(wChar);

	//�����п��и�
	UINT nWidth = bmAllImage->GetWidth();
	UINT nHeight = bmAllImage->GetHeight();

	//��ͼƬ��Ȳ�Ϊ8�ı�������������Ϊ8�ı���
	if(nWidth % 8)
	{
		nWidth = (nWidth/8 + 1)*8;
	}
    
	//��������ؼ��Ŀ��
	SetLatticeSize( nWidth, nHeight);
	

	//�����Ԫ������
	for(int i = 0;i <= (nWidth - 1); i++)
	{
		for (int j = 0; j <= (nHeight - 1); j++)
		{
			DrawPic(j,i,0);
		}
	}

	//���Ƶ���ͼ
	for (int i = 0; i <= (nWidth - 1); i++)
	{
		for (int j = 0; j <= (nHeight - 1); j++)
		{
			Color pixelColor;
			bmAllImage->GetPixel(i, j,&pixelColor);

			if (pixelColor.GetValue() == 0xffffffff) //0xff000000
			{
				DrawPic(j,i,0);
			}
			else 
			{
				DrawPic(j,i,1);
			}
		}
	}

	//�ͷ�λͼ�ڴ�
	if (bmAllImage != NULL)
	{
		delete bmAllImage;
		bmAllImage = NULL;
	}
	//�ر��ļ� 
	fclose(fp); 

	m_pGrid.Invalidate(FALSE);
}
void CLatticeSet::OnBnClickedButtonFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	GetDlgItem(m_inputeditidshow)->GetFont()->GetLogFont(&lf);
	//ʹ�ð�ť�ĵ�ǰ�����ʼ������Ի���
	CFontDialog dlgFontDlg(&lf);
	//��ʾ����ѡ��Ի���
	if (dlgFontDlg.DoModal() == IDOK)
	{
		//����û�������ѡ��Ի����е����ˡ�ȷ������ť
		//�򽫰�ťID_BUTTON_DEMODE�ı����ı���������Ϊ��ѡ��������

		dlgFontDlg.GetCurrentFont(&lf);
		static CFont m_font;
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
		GetDlgItem(m_inputeditidshow)->SetFont(&m_font);
	}
}

//�����ѱ����ͼƬ����ͼƬ·�����������ͣ�������ţ���ʾ��ɫ,��������Э���������ͼƬ����Э��
void CLatticeSet::DownLoadStaticPic(int nNo
									, int nCount
									, CString strFileName
									, UINT nMethod
									, UINT nLedType
									, UINT nType /*= 1*/
									, UINT nNum /*= 1*/
									, UINT nClr /*= 1*/
									, UINT nLan /*= 0*/ )
{
	//����ļ��Ƿ����
	if (!PathFileExists(strFileName))
	{
		return;
	}

	WCHAR   *wChar; 
	wChar = strFileName.Trim().AllocSysString();

	Gdiplus::Bitmap *pBitMap = new Bitmap(wChar);
	int iPicColumns = pBitMap->GetWidth();
	int iPicRows = pBitMap->GetHeight();

	vector<BYTE> m_vMapData;

	
#pragma region 	ѭ��ɨ�����

	int ictrl = 0;//������ɨ��
	byte bArry[8] ={0}; 
	//for (int i = 0; i <= (iPicRows - 1); i++)
	//{
	//	for (int j = 0; j < (iPicColumns / 8); j++)
	//	{
	//		ictrl = 0;
	//		do
	//		{
	//			Color pixelColor;
	//			pBitMap->GetPixel(i, ictrl + j*8,&pixelColor);
	//			ARGB rgb =pixelColor.GetValue();
	//			if ( rgb == 0xffffffff)
	//			{
	//				bArry[7-ictrl] = 1; 
	//				ictrl++;      
	//			}
	//			else
	//			{
	//				bArry[7-ictrl] = 0; 
	//				ictrl++;     
	//			}
	//		}while(ictrl <= 7);
	//		m_vMapData.push_back(Littlebag(bArry[7], bArry[6], bArry[5], bArry[4], bArry[3], bArry[2], bArry[1], bArry[0]));
	//	}   
	//}

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
			m_vMapData.push_back(Littlebyte(bArry[7], bArry[6], bArry[5], bArry[4], bArry[3], bArry[2], bArry[1], bArry[0]));
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

#pragma endregion


#pragma region �ְ�����
	//�������ݷְ���С200byte
	int nPicMaxPack = 200;
	int nPackCount = 1; //���ݷְ���С
	BOOL bSendFinshed = FALSE;

	//������ɣ����˳�while����Ҫ���Ƿְ����������ֹ��������
	//����а�������255�ģ����С�ְ���С
	while (!bSendFinshed)
	{
		//�����������ݵİ���
		if ((nBmpCount % nPicMaxPack) == 0)
		{
			nPackCount = (nBmpCount / nPicMaxPack);//����
		}
		else 
		{
			nPackCount = (nBmpCount / nPicMaxPack) + 1;
		}
		int nPicDataAddr = 0;//��ȡͼƬ���ݵĿ�ʼλ��
		//////////////////////////////////////////////////////////////////////////
		for (int iPackIndex=1;iPackIndex <= nPackCount;iPackIndex++)
		{
			BYTE pDataBuffer[255] = {0};//����һ�������������
			PIS_PACK_HEAD   *pHead = (PIS_PACK_HEAD *)&pDataBuffer[0];
			COM_PIC_PACK *pPicPack = (COM_PIC_PACK*)&pDataBuffer[sizeof(PIS_PACK_HEAD)];
			pHead->bDstNetID = 0xFF;				
			pHead->bDstDevID=  nLedType;
			pHead->bSrcNetID = 0x00;
			pHead->bSrcDevID = 0x01;
			pHead->bCMD = 0x04;//��ʾΪ����	
			//////////////////////////////////////////////////////////////////////////
			pPicPack->bType = 2;//ͼƬ
			pPicPack->bColor = nClr;//��ɫ
			pPicPack->bPackNum = iPackIndex;
			WORD wPicWidth = (iPicColumns/8)*8;//����Ϊ8��������
			pPicPack->bWidthHi = BYTE(wPicWidth>>8);
			pPicPack->bWidthLow = (BYTE)(0x00FF & wPicWidth);
			pPicPack->bFlag.bEn = nLan;//����
			//iPackIndex
			//if (1 == nPackCount)
			//{
			//	pPicPack->bFlag.bLastPack = 1;
			//	pPicPack->bFlag.bStart = 1;
			//	pPicPack->bFlag.bFirstPack = 1;
			//	pPicPack->bFlag.bStop = 1;
			//}
			//else if (iPackIndex == 1)
			//{
			//	pPicPack->bFlag.bFirstPack = 1;
			//	pPicPack->bFlag.bLastPack = 0;
			//	pPicPack->bFlag.bStart = 1;
			//	pPicPack->bFlag.bStop = 0;
			//}
			//else if (iPackIndex == nPackCount)
			//{
			//	pPicPack->bFlag.bFirstPack = 0;
			//	pPicPack->bFlag.bLastPack = 1;
			//	pPicPack->bFlag.bStart = 0;
			//	pPicPack->bFlag.bStop = 1;
			//}
			//else
			//{
			//	pPicPack->bFlag.bFirstPack = 0;
			//	pPicPack->bFlag.bLastPack = 0;
			//	pPicPack->bFlag.bStart = 0;
			//	pPicPack->bFlag.bStop = 0;
			//}

			//��������PTU�������Э��,bFlag��־λֵ������ʾ��
			//bit0��1��ʾ������ʼ��־
			//bit1��1��ʾ����������־
			//bit2��1��ʾ����ͼƬ��1����ʼ��0��ʾ�ǵ�1�����ݡ�
			//bit3��1��ʾ����ͼƬ���1�����ݣ�0��ʾ�����1�����ݡ�

			if(nCount == 1)
			{
				if(1 == nPackCount)
				{
					pPicPack->bFlag.bFirstPack = 1;
					pPicPack->bFlag.bLastPack = 1;
					pPicPack->bFlag.bStart = 1;
					pPicPack->bFlag.bStop = 1;
				}
				else
				{
					if(1 == iPackIndex)
					{
						pPicPack->bFlag.bFirstPack = 1;
						pPicPack->bFlag.bLastPack = 0;
						pPicPack->bFlag.bStart = 1;
						pPicPack->bFlag.bStop = 0;
					}
					if(nPackCount == iPackIndex)
					{
						pPicPack->bFlag.bFirstPack = 0;
						pPicPack->bFlag.bLastPack = 1;
						pPicPack->bFlag.bStart = 0;
						pPicPack->bFlag.bStop = 1;
					}
					if((iPackIndex > 1) && (iPackIndex < nPackCount))
					{
						pPicPack->bFlag.bFirstPack = 0;
						pPicPack->bFlag.bLastPack = 0;
						pPicPack->bFlag.bStart = 0;
						pPicPack->bFlag.bStop = 0;
					}
				}
			}
			else
			{
				if(1 == nNo)
				{
					if(1 == nPackCount)
					{
						pPicPack->bFlag.bFirstPack = 1;
						pPicPack->bFlag.bLastPack = 1;
						pPicPack->bFlag.bStart = 1;
						pPicPack->bFlag.bStop = 0;
					}
					else
					{
						if(1 == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 1;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 1;
							pPicPack->bFlag.bStop = 0;
						}
						if(nPackCount == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 1;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
						if((iPackIndex > 1) && (iPackIndex < nPackCount))
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
					}
				}
				if(nCount == nNo)
				{
					if(1 == nPackCount)
					{
						pPicPack->bFlag.bFirstPack = 1;
						pPicPack->bFlag.bLastPack = 1;
						pPicPack->bFlag.bStart = 0;
						pPicPack->bFlag.bStop = 1;
					}
					else
					{
						if(1 == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 1;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
						if(nPackCount == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 1;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 1;
						}
						if((iPackIndex > 1) && (iPackIndex < nPackCount))
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
					}
				}
				if((nNo > 1) &&(nNo < nCount))
				{
					if(1 == nPackCount)
					{
						pPicPack->bFlag.bFirstPack = 1;
						pPicPack->bFlag.bLastPack = 1;
						pPicPack->bFlag.bStart = 0;
						pPicPack->bFlag.bStop = 0;
					}
					else
					{
						if(1 == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 1;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
						if(nPackCount == iPackIndex)
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 1;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
						if((iPackIndex > 1) && (iPackIndex < nPackCount))
						{
							pPicPack->bFlag.bFirstPack = 0;
							pPicPack->bFlag.bLastPack = 0;
							pPicPack->bFlag.bStart = 0;
							pPicPack->bFlag.bStop = 0;
						}
					}
				}
			}
			
			//��ȡ��������
			pPicPack->bDataType = nType;
			pPicPack->bDataIndex = nNum; //�������

			//��ʼ��ȡһ����ͼƬ����
			int nPicDataReadCount = 0;   //��ǰ��ͼƬ����
			for (int nSavePos = 0 ; (nPicDataAddr < nBmpCount) && (nPicDataReadCount < nPicMaxPack); nSavePos++)
			{
				pPicPack->bData[nSavePos] = m_vMapData.at(nPicDataAddr);//��ȡͼƬ�������ݵ�����
				nPicDataReadCount++;
				nPicDataAddr++;
			}		

			WORD wAppDatalen = (WORD)(nPicDataReadCount + sizeof(COM_PIC_PACK));//Ӧ�ò�����ΪͼƬ���ݣ�����ͷ
			pHead->bDataLen =(BYTE)(wAppDatalen);

			int nTotalCount = sizeof(PIS_PACK_HEAD) + sizeof(COM_PIC_PACK) + nPicDataReadCount;//ȫ�����ݳ���
			//ͳ�������ַ�����Ҫת����ַ�������ж�ת����������ݳ��ȳ���255�������ݰ��ĳ���Ҫ���٣����������
			int nConverCharCout=0;
			for (int i=0;i<nTotalCount;i++)
			{
				if (pDataBuffer[i] == 0x7E || pDataBuffer[i] == 0x7F)
				{
					nConverCharCout++;
				}
			}
			//�ж��ܳ���
			if ((nTotalCount + nConverCharCout) > 252 )
			{
				nPicMaxPack =- 5;//�����ȼ���5���ֽ�
				break;//�ص�whileѭ�����
			}

			BYTE bCheckSum = theApp.GetCheckSum(pDataBuffer,nTotalCount);//����У���
			LPCOMM_DATAPACK pHdlcPack = new COMM_DATAPACK;
			pHdlcPack->nLen = nTotalCount+1; //��Ҫ����һ��У������
			pHdlcPack->pPtr = new BYTE[pHdlcPack->nLen];
			pDataBuffer[nTotalCount] = bCheckSum;//�����������У���ֽ�
			memcpy(pHdlcPack->pPtr,pDataBuffer,nTotalCount+1);

			if (nMethod == USB)
			{
				BYTE pUsbDataBuffer[255] = {0};//����һ�������������
				USB_PACK_HEAD* pUsbHead = (USB_PACK_HEAD*)&pUsbDataBuffer[0];
				//BYTE * p = NULL;
				//p = &pUsbDataBuffer[sizeof(USB_PACK_HEAD)];
				BYTE * p = &pUsbDataBuffer[sizeof(USB_PACK_HEAD)];
				memcpy(p, pHdlcPack->pPtr, pHdlcPack->nLen);
				p = NULL;

				int nUsbTotalCount = sizeof(USB_PACK_HEAD) + pHdlcPack->nLen;
				//pUsbHead->bCMD = 0x19;
				pUsbHead->bCMD = CMD_TRANSFER;
				pUsbHead->bDataLenH = nUsbTotalCount << 8;
				pUsbHead->bDataLenL = nUsbTotalCount;

				BYTE bUsbCheckSum = theApp.GetCheckSum(pUsbDataBuffer, nUsbTotalCount);
				pUsbDataBuffer[nUsbTotalCount] = bUsbCheckSum;

				//added by Amy 2012-9-13
				//���������֡ͷ֡β���ҽ���ת�����
				((CMainFrame *)theApp.m_pMainWnd)->AddHeadAndTail(pUsbDataBuffer, nUsbTotalCount + 1);

				LPUSB_DATAPACK pUsbPack = new USB_DATAPACK;
				pUsbPack->nLen = ((CMainFrame *)theApp.m_pMainWnd)->m_nSendCnt;
				pUsbPack->pPtr = new BYTE[pUsbPack->nLen];
// 				for (int i=0; i<pUsbPack->nLen; i++)
// 				{
// 					TRACE("%x ", ((CMainFrame *)theApp.m_pMainWnd)->m_bUsbSendBuffer[i]);
// 				}			
// 				TRACE("\n");
				memcpy(pUsbPack->pPtr, ((CMainFrame *)theApp.m_pMainWnd)->m_bUsbSendBuffer, pUsbPack->nLen);
				//����USB����
				((CMainFrame *)theApp.m_pMainWnd)->SendUSBData(pUsbPack);
				Sleep(500);

				////Ϊ��ȷ�����ؿ������ܽ��յ����ݣ�ÿ�����ݵĵ�һ���跢����
				if(1 == pPicPack->bFlag.bStart)
				{
					Sleep(200);
					((CMainFrame *)theApp.m_pMainWnd)->SendUSBData(pUsbPack);
				}
				//////////////////////////////////////////////////////////////////////////

				//�ͷ��ڴ�
				if (pUsbPack->pPtr != NULL)
				{
					delete[] pUsbPack->pPtr;
					pUsbPack->pPtr = NULL;
				}
				if (pUsbPack != NULL)
				{
					delete pUsbPack;
					pUsbPack = NULL;
				}	
				if (pHdlcPack->pPtr != NULL)
				{
					delete[] pHdlcPack->pPtr;
					pHdlcPack->pPtr = NULL;
				}
				if (pHdlcPack != NULL)
				{
					delete pHdlcPack;
					pHdlcPack = NULL;
				}
			}
			else if(nMethod == COM)
			{
				((CMainFrame *)theApp.m_pMainWnd)->SendHdlcComPack(pHdlcPack);//��������
				if(1 == pPicPack->bFlag.bStart)
				{
					Sleep(200);
					((CMainFrame *)theApp.m_pMainWnd)->SendHdlcComPack(pHdlcPack);//��������
				}

				//�ͷ��ڴ�
				if (pHdlcPack->pPtr != NULL)
				{
					delete[] pHdlcPack->pPtr;
					pHdlcPack->pPtr = NULL;
				}
				if (pHdlcPack != NULL)
				{
					delete pHdlcPack;
					pHdlcPack = NULL;
				}				
			}

			int nPercent = iPackIndex *100/nPackCount;
			Sleep(200);//�ӳ�200����
		}

		//����ְ��������򽫰��Ĵ�С��ԭ���ļ�ȥ5���ֽ�
		bSendFinshed = TRUE;
	}//whileѭ��
#pragma endregion 

	if(pBitMap != NULL)
	{
		delete pBitMap;
		pBitMap = NULL;
	}
	return;
}
void CLatticeSet::OnBnClickedButtonDownByType()
{
	WND_INFO wndInfo;
	memset(&wndInfo, 0 ,sizeof(WND_INFO));

	//wndInfo.uPort = ((CComboBox*)GetDlgItem(IDC_COMBO_DOWNLOAD_METHOD))->GetCurSel();//���ط�ʽ
	wndInfo.uColor = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_COLOR))->GetCurSel();//���ص���ɫ
	wndInfo.uLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//���ص�����
	wndInfo.uType = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_TYPE))->GetCurSel();//���ص���������

	CDownLoadLED dlgDownLed(this, &wndInfo);
	dlgDownLed.DoModal();

}

void CLatticeSet::OnBnClickedButtonAllDown()
{
	WND_INFO wndInfo;
	memset(&wndInfo, 0 ,sizeof(WND_INFO));

	//wndInfo.uPort = ((CComboBox*)GetDlgItem(IDC_COMBO_DOWNLOAD_METHOD))->GetCurSel();//���ط�ʽ
	wndInfo.uColor = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_COLOR))->GetCurSel();//���ص���ɫ
	wndInfo.uLan = ((CComboBox*)GetDlgItem(IDC_COMBO_LAN))->GetCurSel();//���ص�����
	wndInfo.uType = ((CComboBox*)GetDlgItem(IDC_COMBO_LED_TYPE))->GetCurSel();//���ص���������

	CDownLoadLED dlgDownLed(this, &wndInfo);
	dlgDownLed.SetDownAll(TRUE);
	dlgDownLed.DoModal();
}

BOOL CLatticeSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE||pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}



void CLatticeSet::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	//COperateDlg dlg;
 	CCustomDlg dlg(this);
 	dlg.DoModal();
}

void CLatticeSet::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	CString strMsg;	
	//int iType = 0;
	int iSelType = ((CComboBox *)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	int iSelNo = ((CComboBox *)GetDlgItem(IDC_COMBO_DATA_LIST))->GetCurSel();

	if (CB_ERR == iSelType)
	{
		MessageBox(_T("Please choose the type!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}
	if (CB_ERR == iSelNo)
	{
		MessageBox(_T("Please choose the item!"), _T("Prompt"), MB_ICONWARNING);
		return;
	}

	switch (iSelType)
	{
	case AD:
		strMsg.Format(_T("Are you sure to delete item %d of the Advertisement?"), iSelNo + 1);
		break;
	case TIP:
		strMsg.Format(_T("Are you sure to delete item %d of the Tip?"), iSelNo + 1);
		break;
	default:
		break;
	}
	
	int iRet = MessageBox(strMsg, _T("Prompt"), MB_OKCANCEL);
	//�����Ի���õ��ķ���ֵ
	switch (iRet)
	{
	case IDOK:
		DeleteConfigItem(iSelType, iSelNo);
		break;
	case IDCANCEL:
		break;
	default:
		break;
	}
}

//�������ļ���ɾ����Ϣ
void CLatticeSet::DeleteConfigItem(int iType, int iNo)
{
	CString tmpStr,szValue;
	CString strIni = _T("\\PTU.ini");
	strIni = theApp.GetAppPath() + strIni;

	if (AD == iType)
	{
	}
	else if (TIP == iType)
	{
	}
	else
	{
		return;
	}
	int iRet = ((CComboBox *)GetDlgItem(IDC_COMBO_DATA_LIST))->DeleteString(iNo);
	if (CB_ERR == iRet)
	{
		return;
	}
}

LRESULT CLatticeSet::OnDeleteMsg(WPARAM wParam, LPARAM lParam)
{
	DoLoadCfg();
	return 0L;
}

LRESULT CLatticeSet::OnAddMsg(WPARAM wParam, LPARAM lParam)
{
	DoLoadCfg();
	return 0L;
}

void CLatticeSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
 
	   CString strBk;
	   strBk = theApp.GetAppPath() + _T("//ImageSrc//LatticeSet.jpg");
	   //CClientDC dc(this);
	   CRect rect;
	   GetClientRect(&rect);
	   Image  myImage(strBk.GetBuffer());   //�����ƶ���С��BMPλͼ�ļ�
	   Graphics graphics(dc);

	   graphics.DrawImage( 
		   &myImage, 
		   Rect( 0, 0, rect.Width(), rect.Height()), 
		   0, 
		   0, 
		   myImage.GetWidth(), 
		   myImage.GetHeight(), 
		   UnitPixel, 
		   NULL, 
		   NULL, 
		   NULL
		   ) ;	
}

HBRUSH CLatticeSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(0,0,0)); 
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CLatticeSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
/*	CString strBk;
	strBk = theApp.GetAppPath() + _T("//ImageSrc//LatticeSet.jpg");
	//CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	Image myImage(strBk.GetBuffer());   //�����ƶ���С��BMPλͼ�ļ�
	
	
	Graphics graphics(*pDC);

	graphics.DrawImage( 
		&myImage, 
		Rect( 0, 0, rect.Width(), rect.Height()), 
		0, 
		0, 
		myImage.GetWidth(), 
		myImage.GetHeight(), 
		UnitPixel, 
		NULL, 
		NULL, 
		NULL
		) ;
	
	
	*/
	return CDialog::OnEraseBkgnd(pDC);
	return TRUE;
}

void CLatticeSet::OnEnChangeEditInputText()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

#pragma once


// CDlgHistoricalView �Ի���

class CDlgHistoricalView : public CDialog
{
	DECLARE_DYNAMIC(CDlgHistoricalView)

public:
	CDlgHistoricalView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgHistoricalView();

	
	void OnTcmsStatusDisplay(tagTmsInfo stTmsInfo,CString szDesStation, CString szNextStation);
	void OnTcmsStatusDisplay(BYTE *pData, int nLen);
	void OnPiscStatusDisplay(BYTE *pData, int nLen);

	void InitDisplay(void );

// �Ի�������
	enum { IDD = IDD_HISTORICAL_DATA_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	int GetIDIndex(int staticid, int & groupint);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


private:
	int m_SaloonStatus[6],m_Saloonstatic[6];      
	int m_PECUStatus[18],m_PECUStatic[18];
	int m_WDSStatus[24],m_WDSStatic[24];
	int m_DRMCStatus[6],m_DRMCStatic[6];
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};

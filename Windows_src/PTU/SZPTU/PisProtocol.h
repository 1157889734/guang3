#pragma once
#include "protocolanalysis.h"

typedef struct stSaloonDevStat{
	bool bVehicleNotDetected;

	bool bDRMCfault;

	bool bWDSfault_1;
	bool bWDSfault_2;
	bool bWDSfault_3;
	bool bWDSfault_4;

	bool bPECUfault_1;
	bool bPECUfault_2;
	bool bPECUfault_3;
}SaloonDevStat;

class CPisProtocol :
	public CProtocolAnalysis
{
public:
	typedef enum enumDataFieldIndex{
		PISDATA_SALOON_1_STATUS = 0,
		PISDATA_SENTINEL,
	}DataFieldIndex;

	CPisProtocol(void);
	~CPisProtocol(void);

	virtual int GetElementCount(void );

	virtual void GetElementData(int nIndex, void *pStorage);
	virtual CString GetElementName(int nIndex);
	virtual CString GetElementDataDisplay(int nIndex);
};

#pragma once


#include "ProtocolAnalysis.h"

class CTmsProtocol: public CProtocolAnalysis
{
public:
	typedef enum enumDataFieldIndex{
		TCMSDATA_INFO_ON_FLAG,
		/*TCMSDATA_STATION_START,*/
		TCMSDATA_STATION_DEST,
		TCMSDATA_NEXT_STATION,
		TCMSDATA_LEFT_DOOR_RELEASE,
		TCMSDATA_RIGHT_DOOR_RELEASE,
		TCMSDATA_LEFT_HANDLE_DOOR_RELEASE,
		TCMSDATA_RIGHT_HANDLE_DOOR_RELEASE,
		TCMSDATA_SENTINEL,
	}DataFieldIndex;

	CTmsProtocol(void);
	~CTmsProtocol(void);

	virtual int GetElementCount(void );

	virtual void GetElementData(int nIndex, void *pStorage);
	virtual CString GetElementName(int nIndex);
	virtual CString GetElementDataDisplay(int nIndex);
};

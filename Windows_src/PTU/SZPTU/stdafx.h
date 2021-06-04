// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ��������Ϊ�ʺ� Windows Me ����߰汾����Ӧֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

/////////////////////////////////////////////////////////////////////

// const int INT_STATION_NUM = 30;
// const CString ARRAY_STR_STATION_NAME[INT_STATION_NUM] = {
// 	_T("1.��خ�㳡"),
// 	_T("2.����"),
// 	_T("3.��Ϫ��¡"),
// 	_T("4.��ʯ"),
// 	_T("5.�Ü�"),
// 	_T("6.����"),
// 	_T("7.����"),
// 	_T("8.�ʹ�"),
// 	_T("9.�����"),
// 	_T("10.�齭�³�"),
// 	_T("11.������·"),
// 	_T("12.�ֺ���"),
// 	_T("13.���ݶ�վ"),
// 	_T("14.����"),
// 	_T("15.÷��԰"),
// 	_T("16.�����Ϸ�ҽԺ"),
// 	_T("17.ͬ��"),
// 	_T("18.��̩"),
// 	_T("19.���ƴ����"),
// 	_T("20.������"),
// 	_T("21.����"),
// 	_T("22.�˺�"),
// 	_T("23.����"),
// 	_T("24.������"),
// 	_T("25.������"),
// 	_T("26.ʯ����"),
// 	_T("27.�ڶ�"),
// 	_T("28.��ʦ"),
// 	_T("29.��ɽ"),
// 	_T("30.��ӿ���վ")
// };

const int INT_STATION_NUM = 85;
const CString ARRAY_STR_STATION_NAME[INT_STATION_NUM] = {
	_T("1.����"),
	_T("2.����"),
	_T("3.�Ü�"),
	_T("4.�Ü�"),
	_T("5.��ʯ"),
	_T("6.��ʯ"),
	_T("7.��ʯ"),
	_T("8.��ʯ"),
	_T("9.��Ϫ"),
	_T("10.��Ϫ"),
	_T("11.����"),
	_T("12.����"),
	_T("13.��خ�㳡"),
	_T("14.��خ�㳡"),
	_T("15.��خ�㳡"),
	_T("16.��ӿ���վ"),
	_T("17.��ӿ���վ"),
	_T("18.��ӿ���վ"),
	_T("19.��ӿ���վ"),
	_T("20.��ɽ"),
	_T("21.��ɽ"),
	_T("22.��ʦ"),
	_T("23.��ʦ"),
	_T("24.�ڶ�"),
	_T("25.�ڶ�"),
	_T("26.ʯ����"),
	_T("27.ʯ����"),
	_T("28.���ݶ�վ"),
	_T("29.���ݶ�վ"),
	_T("30.���ݶ�վ"),
	_T("31.�ֺ���"),
	_T("32.�ֺ���"),
	_T("33.������·"),
	_T("34.������·"),
	_T("35.������·"),
	_T("36.������·"),
	_T("37.������·"),
	_T("38.������·"),
	_T("39.�齭�³�"),
	_T("40.�齭�³�"),
	_T("41.�����"),
	_T("42.�����"),
	_T("43.�ʹ�"),
	_T("44.�ʹ�"),
	_T("45.�ʹ�"),
	_T("46.����"),
	_T("47.����"),
	_T("48.Test Track West End"),
	_T("49.Test Track East End"),
	_T("50.Test Track Turnback"),
	_T("51.������"),
	_T("52.������"),
	_T("53.���ƴ����"),
	_T("54.���ƴ����"),
	_T("55.��̩"),
	_T("56.��̩"),
	_T("57.ͬ��"),
	_T("58.ͬ��"),
	_T("59.ͬ��"),
	_T("60.ͬ��"),
	_T("61.�����Ϸ�ҽԺ"),
	_T("62.�����Ϸ�ҽԺ"),
	_T("63.÷��԰"),
	_T("64.÷��԰"),
	_T("65.����"),
	_T("66.����"),
	_T("67.������"),
	_T("68.������"),
	_T("69.������"),
	_T("70.������"),
	_T("71.������"),
	_T("72.������"),
	_T("73.������"),
	_T("74.������"),
	_T("75.����"),
	_T("76.����"),
	_T("77.����"),
	_T("78.�˺�"),
	_T("79.�˺�"),
	_T("80.����"),
	_T("81.����"),
	_T("82.����"),
	_T("83.Test Track West End"),
	_T("84.Test Track East End"),
	_T("85.Test Track Turnback"),
};

const int ARRAY_INT_STATION_CODE[INT_STATION_NUM] = {
	1, 2, 3, 4, 5, 
	6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 
	16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 
	26, 27, 28, 29, 30,
};

//����
typedef struct VolumeInfo            
{
	int iGlobal;			    //������
	int iOcc;                   //OCC
	int iDva;                   //DVA
	int iMedia;                 //MEDIA
	int iDoor;                  //DOOR
	int fApuBass;				//APU ����
	int fApuTreble;				//APU ����
}VolumeInfo, *LPVolumeInfo;

typedef struct tag_Datapack  //USB���ݰ�
{
	BYTE *pPtr;          // USB���յ�������
	int nLen;			 // ���ݳ���
} PORT_DATAPACK, *LPPORT_DATAPACK;
typedef struct tagUSB_Datapack  //USB���ݰ�
{
	BYTE *pPtr;          // USB���յ�������
	int nLen;			 // ���ݳ���
} USB_DATAPACK, *LPUSB_DATAPACK;
typedef struct tagComm_Datapack//�������ݰ�
{
	BYTE *pPtr;          // ���ڽ��յ�������
	int nLen;			 // ���ݳ���
} COMM_DATAPACK, *LPCOMM_DATAPACK;
#define  WM_RECV_USB_DATA (WM_USER+100)
#define  WM_CLOSE_USB_DEV (WM_USER+101)
#define WM_RECV_COM_DATA (WM_USER+102)
#define WM_FILE_SELECT   (WM_USER+103)
#define WM_FILE_DOWN_FINISH (WM_USER+104)
#define WM_DEVICE_STATUS  (WM_USER+105)
#define WM_TMS_INFO       (WM_USER+106)
#define WM_PIS_INFO       (WM_USER+107)
#define WM_485_INFO       (WM_USER+108)
#define WM_BEGINEND_DDATA (WM_USER+109)
#define WM_MID_DDATA (WM_USER+110)
#define WM_ATC_DATA  (WM_USER+111)
#define WM_FILE_DOWN_DATA  (WM_USER+112)
#define WM_USB_CHECKERROR  (WM_USER+113)
#define WM_TIME_CHECKCTCUSB (WM_USER+114)
#define WM_TIME_CHECKATCUSB (WM_USER+115)
#define WM_HEART_DATA (WM_USER+116)

#define WM_ADD_MSG				(WM_USER + 1009)	//��ӹ�����ʾ����Ϣ
#define WM_DELETE_MSG			(WM_USER + 1010)	//ɾ��������ʾ����Ϣ

///////////////////////////////////////////////
#define CMD_VER_CTC_QUERY 0x30
#define CMD_QUERY_CTC_ROUTE 0x32
#define CMD_SET_CTC_ROUTE   0x31
#define CMD_QUERY_BCBOXVOLUME 0x34
#define CMD_VERSION 0x05
////////////////////////////////////////////////////////////////////
#define COMM_BUFFER_SIZE  512

//���¶����������Ϊ�������������,��������
#define CMD_ARRIVE_DIS			0x05	 	//��վ�㲥����
#define CMD_LEAVE_DIS			0x06	    //��վ�㲥����
#define CMD_SET_VOLUME			0x07	    //��������
#define CMD_VOLUME_QUERY		0x15	    //��ѯ����
#define CMD_SPECIAL_NUM			0x0B	    //��������㲥����
#define CMD_SET_BC_PRIOR		0x0C	    //���ù㲥���ȼ�
#define CMD_ERROR_CODE_QUERY	0x16		//��ѯ�쳣�洢��Ϣ
#define CMD_INIT				0x20	    //�ָ�Ĭ��ֵ
#define CMD_SET_SUCCESS			0x21		//���óɹ�����
#define CMD_BC_PRIOR_QUERY		0x22		//��ѯ�㲥���ȼ�
#define CMD_TRANSFER			0x19		//������ת��
#define CMD_VER_QUERY			0x26		//��ѯ�յ�վ���汾

/////////////////////////////////////////////////
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#define PRO_X_SIZE  1020

#define PRO_Y_SIZE  680

//ͼƬ�����
#define MAX_WIDTH		4096
#define STATION_WIDTH	256
#define TIP_WIDTH		1024
#define AD_WIDTH		4096
#define BC_WIDTH		4096

//ͼƬ���߶�
#define MAX_HEIGHT		16

//ö�ٶ���
enum DOWNLOAD_TYPE{LINEINFO,TIP,AD,BROADCAST};		//������������
enum FONT_CLR{CLR_RED,CLR_GREEN,CLR_ORG};			//������ɫ
enum LED_TYPE{LED_IDU,LED_FDU,LED_SDU,LED_DRMD};	//LED������
enum DOWN_METHOD{COM,USB};		//���ط�ʽ

//����Э��������ֵ
#define IDU_VALUE 0xF6		//�ڲ���
#define FDU_VALUE 0xF7		//�յ�վ��
#define SDU_VALUE 0xF8		//�ಿ��
#define MAP_VALUE 0xF4		//��̬��ͼ

//CMD ����
#define CMD_UPDATE  0x04	//������������
#define CMD_VERSION 0x05	//��̬��ͼ��IDU��SDU�汾��ѯ����

//����
enum LAN_TYPE{LAN_EN,LAN_IRAN};

//ͨ���ַ�������������
typedef struct tagXString 
{
	CString strLanEN;	//Ӣ��
	CString strLanIRAN;	//IRAN��
	CString &operator[](LAN_TYPE lan_type)
	{
		if (lan_type == LAN_EN)
		{
			return strLanEN;
		}
		else
		{
			return strLanIRAN;
		}
	}
}XString;

//����ǰ��ָ������
typedef struct tagPIS_PACK_HEAD 
{
	BYTE         bDstNetID;			//Ŀ�������
	BYTE         bDstDevID;			//Ŀ���豸��
	BYTE         bSrcNetID;			//Դ�����
	BYTE         bSrcDevID;			//Դ�豸��
	BYTE         bCMD;				//������
	BYTE         bDataLen;			//Ӧ�ò����ݳ���
	BYTE         bData[0];			//Ӧ�ò�����
}PIS_PACK_HEAD;

//�������ݸ�ʽ
typedef struct tagCOM_PIC_PACK
{
	BYTE bType;           //1����2ͼƬ��3ϵͳ�������ݣ�4����������
	struct  
	{
		BYTE bStart:1;    //������ʼ
		BYTE bStop:1;     //��������
		BYTE bFirstPack:1;//�Ƿ��һ������
		BYTE bLastPack:1; //�Ƿ����һ������
		BYTE bEn:1;       //�Ƿ�Ӣ��
		BYTE bReserve:3;  //����
	}bFlag;               //������־
	BYTE bPackNum;        //������
	BYTE bDataType;       //�������ͣ���1����վ����2����2����վ����3����3����վ����4���뵽վ��ʾ������	5���������	6�������㲥����
	BYTE bDataIndex;      //���������ţ���������վ���ݣ���1Ϊ��һ������
	BYTE bWidthLow;       //ͼƬ��ȵ��ֽڣ�8��������
	BYTE bWidthHi;        //���ֽ�
	BYTE bColor;          //��ɫ
	BYTE bData[0];        //����ֵ���ɱ䳤
}COM_PIC_PACK;

//��������Э��
typedef struct tabCOM_UPDATE_PACK 
{
	BYTE bType;        //1����2ͼƬ��3ϵͳ�������ݣ�4����������
	BYTE bFlag;        //1��ʼ���� 2���ݰ� 3��������
	BYTE bLenL0;       //0���ֽڳ���
	BYTE bLenL1;       //1
	BYTE bLenH0;       //2
	BYTE bLenH1;       //3
	BYTE bPackNumL;    //0���ֽڰ���
	BYTE bPackNumH;    //1
	BYTE bData[0];     //����
}COM_UPDATE_PACK;

//����Э��
typedef struct tagCOM_SYSCFG_PACK 
{
	BYTE bType;       //1����2ͼƬ��3ϵͳ�������ݣ�4����������
	BYTE bDisplayLan; //1Ӣ�ģ�2���ʣ�3����
	BYTE bData[0];    //

}COM_SYSCFG_PACK;


//��վ����
typedef struct tagSTATION_CFG_PACK 
{
	BYTE bType;			//1����2ͼƬ��3ϵͳ�������ݣ�4����������
	BYTE bLineNo;		//��
	BYTE bStationNo;	//վ���
	BYTE bDisplayLan;	//��ʾ����
	BYTE bPreColor;		//Ԥ��վ��ʾ��ɫ	
	BYTE bArrivalColor;	//��վ��ʾ��ɫ
	BYTE bData[0];		//

}STATION_CFG_PACK;
//0xfd  0xfe  0xfc  0xfd  0xfe  0xff

//���ݸ��Ӵ��ڵĲ����б�
typedef struct tagWnd_Info
{
	//UINT uPort;			//ͨ�ŷ�ʽ
	UINT uColor;		//LED��ʾ��ɫ
	UINT uLan;			//LED��ʾ����
	UINT uType;			//LED������
}WND_INFO, *LPWND_INFO;


//ʹ��USB�����������ͨ�ŵı�׼ͷ
typedef struct tagUSB_PACK_HEAD 
{
	BYTE bCMD;      //����
	BYTE bDataLenH; //Ӧ�����ݳ��ȸ��ֽ�
	BYTE bDataLenL; //Ӧ�����ݳ��ȵ��ֽ�
	BYTE bData[0];  //Ӧ������
}USB_PACK_HEAD;


typedef struct _tagTmsInfo_
{
	BYTE bAto;
	BYTE bleft;
	BYTE bright;
}tagTmsInfo;

/*PISC ����3�ֽڶ���

//   0      1        2        3       4       5       6        7
//srck1  srck2   srck3    srck4   srck5   srck6   crck1    crck2
BYTE status1_srck1:1; // 86
BYTE status1_srck2:1;
BYTE status1_srck3:1;
BYTE status1_srck4:1;
BYTE status1_srck5:1;
BYTE status1_srck6:1;
BYTE status2_srck1:1;
BYTE status2_srck2:1;


//   0      1        2      3        4       5          6         7
//dva1   dva2      apu1   apu2     dcp1    dcp2    record1   record2
BYTE status2_dva1:1; // 87
BYTE status2_dva2:1;
BYTE status2_apu1:1;
BYTE status2_apu2:1;
BYTE status2_dcp1:1;
BYTE status2_dcp2:1;
BYTE status2_record1:1;
BYTE status2_record2:1;


//   0      1        2      3        4       5          6         7
//atc1   atc2      srck7  srck8    mvb1    mvb2
BYTE status3_atc1:1; //88
BYTE status3_atc2:1;
BYTE io1:1;
BYTE io1:1;
BYTE io1:1;
BYTE io1:1;
BYTE io1:2;
io1

*/




///////////////////////
#include <gdiplus.h>				//��GDI+ͷ�ļ����뵽������
#pragma comment(lib, "gdiplus.lib") //��GDI+��lib�ļ����뵽������
using namespace Gdiplus;
///////////////////////



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



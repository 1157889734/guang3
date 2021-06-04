// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将它更改为适合 Windows Me 或更高版本的相应值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类

/////////////////////////////////////////////////////////////////////

// const int INT_STATION_NUM = 30;
// const CString ARRAY_STR_STATION_NAME[INT_STATION_NUM] = {
// 	_T("1.番禺广场"),
// 	_T("2.市桥"),
// 	_T("3.汉溪长隆"),
// 	_T("4.大石"),
// 	_T("5.厦滘"),
// 	_T("6.沥滘"),
// 	_T("7.大塘"),
// 	_T("8.客村"),
// 	_T("9.赤岗塔"),
// 	_T("10.珠江新城"),
// 	_T("11.体育西路"),
// 	_T("12.林和西"),
// 	_T("13.广州东站"),
// 	_T("14.燕塘"),
// 	_T("15.梅花园"),
// 	_T("16.京西南方医院"),
// 	_T("17.同和"),
// 	_T("18.永泰"),
// 	_T("19.白云大道北"),
// 	_T("20.禾望岗"),
// 	_T("21.龙归"),
// 	_T("22.人和"),
// 	_T("23.高增"),
// 	_T("24.机场南"),
// 	_T("25.机场北"),
// 	_T("26.石牌桥"),
// 	_T("27.岗顶"),
// 	_T("28.华师"),
// 	_T("29.五山"),
// 	_T("30.天河客运站")
// };

const int INT_STATION_NUM = 85;
const CString ARRAY_STR_STATION_NAME[INT_STATION_NUM] = {
	_T("1.沥滘"),
	_T("2.沥滘"),
	_T("3.厦滘"),
	_T("4.厦滘"),
	_T("5.大石"),
	_T("6.大石"),
	_T("7.大石"),
	_T("8.大石"),
	_T("9.汉溪"),
	_T("10.汉溪"),
	_T("11.市桥"),
	_T("12.市桥"),
	_T("13.番禺广场"),
	_T("14.番禺广场"),
	_T("15.番禺广场"),
	_T("16.天河客运站"),
	_T("17.天河客运站"),
	_T("18.天河客运站"),
	_T("19.天河客运站"),
	_T("20.五山"),
	_T("21.五山"),
	_T("22.华师"),
	_T("23.华师"),
	_T("24.岗顶"),
	_T("25.岗顶"),
	_T("26.石牌桥"),
	_T("27.石牌桥"),
	_T("28.广州东站"),
	_T("29.广州东站"),
	_T("30.广州东站"),
	_T("31.林和西"),
	_T("32.林和西"),
	_T("33.体育西路"),
	_T("34.体育西路"),
	_T("35.体育西路"),
	_T("36.体育西路"),
	_T("37.体育西路"),
	_T("38.体育西路"),
	_T("39.珠江新城"),
	_T("40.珠江新城"),
	_T("41.赤岗塔"),
	_T("42.赤岗塔"),
	_T("43.客村"),
	_T("44.客村"),
	_T("45.客村"),
	_T("46.大塘"),
	_T("47.大塘"),
	_T("48.Test Track West End"),
	_T("49.Test Track East End"),
	_T("50.Test Track Turnback"),
	_T("51.禾望岗"),
	_T("52.禾望岗"),
	_T("53.白云大道北"),
	_T("54.白云大道北"),
	_T("55.永泰"),
	_T("56.永泰"),
	_T("57.同和"),
	_T("58.同和"),
	_T("59.同和"),
	_T("60.同和"),
	_T("61.京西南方医院"),
	_T("62.京西南方医院"),
	_T("63.梅花园"),
	_T("64.梅花园"),
	_T("65.燕塘"),
	_T("66.燕塘"),
	_T("67.机场北"),
	_T("68.机场北"),
	_T("69.机场北"),
	_T("70.机场北"),
	_T("71.机场南"),
	_T("72.机场北"),
	_T("73.机场南"),
	_T("74.机场南"),
	_T("75.高增"),
	_T("76.高增"),
	_T("77.高增"),
	_T("78.人和"),
	_T("79.人和"),
	_T("80.龙归"),
	_T("81.龙归"),
	_T("82.龙归"),
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

//音量
typedef struct VolumeInfo            
{
	int iGlobal;			    //总音量
	int iOcc;                   //OCC
	int iDva;                   //DVA
	int iMedia;                 //MEDIA
	int iDoor;                  //DOOR
	int fApuBass;				//APU 低音
	int fApuTreble;				//APU 高音
}VolumeInfo, *LPVolumeInfo;

typedef struct tag_Datapack  //USB数据包
{
	BYTE *pPtr;          // USB接收到的数据
	int nLen;			 // 数据长度
} PORT_DATAPACK, *LPPORT_DATAPACK;
typedef struct tagUSB_Datapack  //USB数据包
{
	BYTE *pPtr;          // USB接收到的数据
	int nLen;			 // 数据长度
} USB_DATAPACK, *LPUSB_DATAPACK;
typedef struct tagComm_Datapack//串口数据包
{
	BYTE *pPtr;          // 串口接收到的数据
	int nLen;			 // 数据长度
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

#define WM_ADD_MSG				(WM_USER + 1009)	//添加广告或提示语消息
#define WM_DELETE_MSG			(WM_USER + 1010)	//删除广告或提示语消息

///////////////////////////////////////////////
#define CMD_VER_CTC_QUERY 0x30
#define CMD_QUERY_CTC_ROUTE 0x32
#define CMD_SET_CTC_ROUTE   0x31
#define CMD_QUERY_BCBOXVOLUME 0x34
#define CMD_VERSION 0x05
////////////////////////////////////////////////////////////////////
#define COMM_BUFFER_SIZE  512

//以下定义的数命令为中央控制器数据,命令类型
#define CMD_ARRIVE_DIS			0x05	 	//到站广播距离
#define CMD_LEAVE_DIS			0x06	    //离站广播距离
#define CMD_SET_VOLUME			0x07	    //设置音量
#define CMD_VOLUME_QUERY		0x15	    //查询音量
#define CMD_SPECIAL_NUM			0x0B	    //设置特殊广播次数
#define CMD_SET_BC_PRIOR		0x0C	    //设置广播优先级
#define CMD_ERROR_CODE_QUERY	0x16		//查询异常存储信息
#define CMD_INIT				0x20	    //恢复默认值
#define CMD_SET_SUCCESS			0x21		//设置成功命令
#define CMD_BC_PRIOR_QUERY		0x22		//查询广播优先级
#define CMD_TRANSFER			0x19		//升级包转发
#define CMD_VER_QUERY			0x26		//查询终点站屏版本

/////////////////////////////////////////////////
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#define PRO_X_SIZE  1020

#define PRO_Y_SIZE  680

//图片最大宽度
#define MAX_WIDTH		4096
#define STATION_WIDTH	256
#define TIP_WIDTH		1024
#define AD_WIDTH		4096
#define BC_WIDTH		4096

//图片最大高度
#define MAX_HEIGHT		16

//枚举定义
enum DOWNLOAD_TYPE{LINEINFO,TIP,AD,BROADCAST};		//下载数据类型
enum FONT_CLR{CLR_RED,CLR_GREEN,CLR_ORG};			//字体颜色
enum LED_TYPE{LED_IDU,LED_FDU,LED_SDU,LED_DRMD};	//LED屏类型
enum DOWN_METHOD{COM,USB};		//下载方式

//下载协议中屏的值
#define IDU_VALUE 0xF6		//内部屏
#define FDU_VALUE 0xF7		//终点站屏
#define SDU_VALUE 0xF8		//侧部屏
#define MAP_VALUE 0xF4		//动态地图

//CMD 类型
#define CMD_UPDATE  0x04	//程序升级命令
#define CMD_VERSION 0x05	//动态地图、IDU、SDU版本查询命令

//语种
enum LAN_TYPE{LAN_EN,LAN_IRAN};

//通用字符串，两种语言
typedef struct tagXString 
{
	CString strLanEN;	//英语
	CString strLanIRAN;	//IRAN语
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

//升级前的指定类型
typedef struct tagPIS_PACK_HEAD 
{
	BYTE         bDstNetID;			//目的网络号
	BYTE         bDstDevID;			//目的设备号
	BYTE         bSrcNetID;			//源网络号
	BYTE         bSrcDevID;			//源设备号
	BYTE         bCMD;				//命令字
	BYTE         bDataLen;			//应用层数据长度
	BYTE         bData[0];			//应用层数据
}PIS_PACK_HEAD;

//升级数据格式
typedef struct tagCOM_PIC_PACK
{
	BYTE bType;           //1程序，2图片，3系统配置数据，4端配置数据
	struct  
	{
		BYTE bStart:1;    //升级开始
		BYTE bStop:1;     //升级结束
		BYTE bFirstPack:1;//是否第一包数据
		BYTE bLastPack:1; //是否最后一包数据
		BYTE bEn:1;       //是否英文
		BYTE bReserve:3;  //无用
	}bFlag;               //升级标志
	BYTE bPackNum;        //包序列
	BYTE bDataType;       //数据类型：第1条线站数据2：第2条线站数据3：第3条线站数据4：离到站提示语数据	5：广告数据	6：紧急广播数据
	BYTE bDataIndex;      //相关数据序号，例如是线站数据，则1为第一端数据
	BYTE bWidthLow;       //图片宽度底字节，8的整数倍
	BYTE bWidthHi;        //高字节
	BYTE bColor;          //颜色
	BYTE bData[0];        //数据值，可变长
}COM_PIC_PACK;

//程序升级协议
typedef struct tabCOM_UPDATE_PACK 
{
	BYTE bType;        //1程序，2图片，3系统配置数据，4端配置数据
	BYTE bFlag;        //1开始下载 2数据包 3结速下载
	BYTE bLenL0;       //0四字节长度
	BYTE bLenL1;       //1
	BYTE bLenH0;       //2
	BYTE bLenH1;       //3
	BYTE bPackNumL;    //0两字节包数
	BYTE bPackNumH;    //1
	BYTE bData[0];     //数据
}COM_UPDATE_PACK;

//配置协议
typedef struct tagCOM_SYSCFG_PACK 
{
	BYTE bType;       //1程序，2图片，3系统配置数据，4端配置数据
	BYTE bDisplayLan; //1英文，2伊朗，3两者
	BYTE bData[0];    //

}COM_SYSCFG_PACK;


//单站配置
typedef struct tagSTATION_CFG_PACK 
{
	BYTE bType;			//1程序，2图片，3系统配置数据，4端配置数据
	BYTE bLineNo;		//线
	BYTE bStationNo;	//站序号
	BYTE bDisplayLan;	//显示语言
	BYTE bPreColor;		//预到站显示颜色	
	BYTE bArrivalColor;	//到站显示颜色
	BYTE bData[0];		//

}STATION_CFG_PACK;
//0xfd  0xfe  0xfc  0xfd  0xfe  0xff

//传递给子窗口的参数列表
typedef struct tagWnd_Info
{
	//UINT uPort;			//通信方式
	UINT uColor;		//LED显示颜色
	UINT uLan;			//LED显示语言
	UINT uType;			//LED屏类型
}WND_INFO, *LPWND_INFO;


//使用USB与中央控制器通信的标准头
typedef struct tagUSB_PACK_HEAD 
{
	BYTE bCMD;      //命令
	BYTE bDataLenH; //应用数据长度高字节
	BYTE bDataLenL; //应用数据长度低字节
	BYTE bData[0];  //应用数据
}USB_PACK_HEAD;


typedef struct _tagTmsInfo_
{
	BYTE bAto;
	BYTE bleft;
	BYTE bright;
}tagTmsInfo;

/*PISC 数据3字节定义

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
#include <gdiplus.h>				//将GDI+头文件加入到工程中
#pragma comment(lib, "gdiplus.lib") //将GDI+的lib文件加入到工程中
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



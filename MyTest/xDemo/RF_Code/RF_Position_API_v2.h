#pragma once

#include "StdAfx.h"
//#include "vld.h"

//#include "TestDllSrc.h"

typedef struct __RF_INFO_TAG
{
	long lTagID;

	bool bAlive;
	int  nPower;

	long lTime;

	UINT uiLocateReader; 
	float fLocateRssi;

	int  nLocateAnt;   // LocateAnt 和 LocateRegion 只有一个会有数值
	int  nLocateRegion;

	float fWorld_X;
	float fWorld_Y;

}RF_INFO_TAG ;

typedef struct __RF_INFO_READER
{
	int ReaderID;
	char Ip[20];
	bool IsNomal;

}RF_INFO_READER;

//---------------------------IN----------------------------------------
typedef struct __RF_INFO_READER_IN
{
	int ID;

	char IP[16];      //网络IP
	int Port;        //端口号
	
	float x;         //接收器位置
	float y;

}RF_INFO_READER_IN;


typedef struct __RF_INFO_REGION_RECT_IN
{
	long left;
	long top;

	long right;
	long bottom;

	long TagID[4];
	long ReaID[4];
	//RF_Rect_In_M Rect;

}RF_INFO_REGION_RECT_IN, RF_INFO_REGION_RECT;

typedef struct __RF_INFO_REGION_LINE_IN
{
	float P1x;       //  P1 --------------------- P2
	float P1y;

	float P2x;
	float P2y;

	UINT P1_ReaderID;  
	UINT P2_ReaderID;   

	UINT P1_TagID;
	UINT P2_TagID;
}RF_INFO_REGION_LINE_IN, RF_INFO_REGION_LINE;

typedef struct __RF_INFO_REGION_POINT_IN
{
	float x;       //  P1 --------------------- P2
	float y;

	UINT uiReaderID;
	UINT uiTriggerID;

}RF_INFO_REGION_POINT_IN, RF_INFO_REGION_POINT;

typedef struct __RF_INFO_REGION
{
	int nIndex;
	int nRectType;

	union
	{
		RF_INFO_REGION_RECT  stRect;
		RF_INFO_REGION_LINE  stLine;
		RF_INFO_REGION_POINT stPoint;
	};
}RF_INFO_REGION;

typedef struct __RF_INFO_RSSI
{
	long lReaderId;

	long lTagId;

	int nUpdataCount;   //  当前卡更新的次数 Max 10000

	long lTime; // 读取时的时间

	UCHAR byPower;  //卡的电量  最大为3， 为1时报警

	UCHAR byRssi; // 记录过去5个强度值

	int nAntID;     // 125k天线id

	int nAntRssi;   // 125k天线强度

}RF_INFO_RSSI_OUT;


enum RF_REGION_STYLE
{
	RF_REGION_STYLE_NONE = 0,
	RF_REGION_STYLE_RECT = 1,
	RF_REGION_STYLE_LINE = 2,
	RF_REGION_STYLE_POINT = 3,
	RF_REGION_STYLE_RECT_MIRROR = 4
};

/************************************************************************/
/*                            数据回调                                  */
/************************************************************************/

typedef int (__stdcall* RF_CALLBACK_TAG_SINGLE)(RF_INFO_TAG  tag, DWORD dwHandle);

typedef int (__stdcall* RF_CALLBACK_TAG)(RF_INFO_TAG * tagList, int tagListNum, DWORD dwHandle);

typedef int (__stdcall* RF_CALLBACK_READER)(RF_INFO_READER* readerList, int readerListNum, DWORD dwHandle);

typedef int (__stdcall* RF_GET_REGION_INFO_FUNC)(RF_INFO_REGION* regionList, int regionNum, DWORD dwHandle);

typedef int (__stdcall* RF_GET_RSSI_INFO_FUNC)(RF_INFO_RSSI_OUT* pInfoList, int nInfoNum, DWORD dwHandle);

typedef int (__stdcall* RF_GET_VSS_INFO_FUNC)(UINT uiRegionId, RF_REGION_STYLE emStyle, float* pVss, int nVssLen, float fMaxValue, float fMinValue, DWORD dwHandle);
 
/************************************************************************/
/*                            接口                                      */
/************************************************************************/

int __stdcall apiRF_Init();

void __stdcall apiRF_UnInit();

void __stdcall apiRF_TStartup();

void __stdcall apiRF_TClose();

//////////////////////////////////////////////////////////////////////////

bool __stdcall apiRF_AddReader(const RF_INFO_READER_IN& ReaderInfo);

UINT __stdcall apiRF_CreateRectRegion(const RF_INFO_REGION_RECT_IN* pAttr);     //返回 ID  0则失败

UINT __stdcall apiRF_CreateMirrorRectRegion(const RF_INFO_REGION_RECT_IN* pAttr);     //返回 ID  0则失败

UINT __stdcall apiRF_CreateLineRegion(const RF_INFO_REGION_LINE_IN& attr);     //返回 ID  0则失败

UINT __stdcall apiRF_CreatePointRegion(int readerId, float pointX, float pointY);     //返回 ID  0则失败

int __stdcall apiRF_CreateAntPoint(int antId, float pointX, float pointY); //成功返回0

void __stdcall apiRF_Clear();  //清除内部已添加的所有参数 必须在停止检测后执行

//////////////////////////////////////////////////////////////////////////

int __stdcall apiSetUserHandle(DWORD hUser);

int __stdcall apiSetSingleTagCallBack(RF_CALLBACK_TAG_SINGLE cb);

int __stdcall apiSetTagCallBack(RF_CALLBACK_TAG cb);

int __stdcall apiSetReaderCallBack(RF_CALLBACK_READER cb);


///////////////////////////////////////////////////////////////////////////

//typedef int (__stdcall* RF_AFTER_GET_READER_INFOS)(RF_INFO_READER* readerList, int readerListNum, DWORD dwHandle);

int __stdcall apiGetReadersInfo(RF_CALLBACK_READER pAfterGetFunc, DWORD hUser); // 负责释放回调内存, 同步回调

int __stdcall apiGetRegionsInfo(RF_GET_REGION_INFO_FUNC pAfterGetFunc, DWORD hUser); // 负责释放回调内存, 同步回调

int __stdcall apiSetRssiInfoCallBack(RF_GET_RSSI_INFO_FUNC pAfterGetFunc, DWORD hUser); // 异步回调

int __stdcall apiGetRegionsVssInfo(UINT uiRegionId, RF_GET_VSS_INFO_FUNC pAfterGetFunc, DWORD hUser);  // 同步回调
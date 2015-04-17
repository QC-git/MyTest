#include "StdAfx.h"
//#include "vld.h"


#pragma once

#include "RF_Position_API_v2.h"
#include "RF_Positon.h"
//  [12/18/2013 RPK-QCY]

extern RF_CReaderManager* g_pRF_Device;
extern RF_CAlgManager* g_pRF_ALgManager;

typedef struct RF_Tag_AdminInfo_Model
{
	long TagID;

	bool Alive;              //是否活跃
	bool IsRef;              //是否作为参考标签
	int Power;

	int SSR;      //  Stronger Signal Reader
	float SSRssi;  //  Stronger Signal Rssi
	CTime DataTime;
	CTime AlgTime;

	int RR;      //  Recent Region
	CPoint RegionPoint;
	CPoint AlgWorldPoint;
	CPoint OldAlgWorldPoint[10];
	CPoint CurWorldPoint;

}RF_Tag_AdminInfo_M;

typedef struct RF_Reader_AdminInfo_Model
{
	int ID;

	char IP[16];             //输入信息
	int Port;

	float LocateX;
	float LocateY;

	bool IsOpen;              //状态位  输出信息
	bool IsHaveData;

}RF_Reader_AdminInfo_M;

// int __stdcall sapiGetTagInfo(RF_Tag_AdminInfo_M* pTag);
// int __stdcall sapiGetReaderInfo(RF_Reader_AdminInfo_M* pReader);

//  [12/18/2013 RPK-QCY]
PVOID __stdcall sapiGetRssiDBMaddr();

PVOID __stdcall sapiGetRegionDBMaddr();

PVOID __stdcall sapiGetReaderDBMaddr();

PVOID __stdcall sapiGetTagDBMaddr();


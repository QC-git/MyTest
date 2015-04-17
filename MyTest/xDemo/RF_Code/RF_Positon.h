#pragma once

#include "StdAfx.h"
#include "RF_Device.h"
#include "RF_RegionFactory.h"
#include "RF_Ant125Manager.h"
#include "RF_TagManager.h"
#include "AFD_RF_API.h"
#include "RF_Position_API_v2.h"

typedef struct RegionTagRssiList_Model
{
	int roomIndex;
	
	int rssiNum;
	float Rssi[4];   //数值从小到大排列
	long compareData;
}RegionTagRssiListM,*PRegionTagRssiListM;

struct RF_Region_s
{
	int nRoom;  // > 0
	RF_REGION_STYLE style;

	RF_CRegionBase* pRegion;
};



#define RF_REGION_MAX_NUM 100

class RF_CAlgManager:public CRF_TagManager_M
	, protected MDoubleThread
{
public:   //  不可引用public函数
	RF_CAlgManager(HANDLE hDevice);
	~RF_CAlgManager();

	bool IsManagerInit() const;

	bool Startup_TRfAlgorithm();
	int  Close_TRfAlgorithm();

	bool Startup_TRfPostPro();
	int  Close_TRfPostPro();

public:
	UINT CreateRectRegion(RF_RectRegionAttr_s* Par);  // return RegionID  暂时不用
	UINT CreateMirrorRectRegion(RF_RectRegionAttr_s* Par);
	UINT CreateLineRegion(RF_LineRegionAttr_s attr);
	UINT CreatePointRegion(RF_PointRegionAttr_s attr);
	void Del_AllRegion(); 
	//return count
	int GetRegionsInfo(RF_GET_REGION_INFO_FUNC pAfterGetFunc, DWORD hUser);

	int GetRegionsVssInfo(UINT uiRegionId, RF_GET_VSS_INFO_FUNC pAfterGetFunc, DWORD hUser);

	int SetUserHandle(DWORD hHandle);
	int SetSingleTagInfoCallBack(RF_CALLBACK_TAG_SINGLE cb);
	int SetAllTagInfoCallBack(RF_CALLBACK_TAG cb);

	//非线程安全
	int CheckRegionID(int nID); // return:room+1   0为无效；
	RF_CRegionBase* GetRegionHandle(int nID, RF_Region_s* pInfo_out=NULL); //  返回 region handle

	

protected:

	bool m_AlgThreadRunStatus;
	int m_nSticks1;
	//Override
	void roundPro1();

	void BuiltRegionVSS();
	void GenerateTagInfo(bool bOptRefTag=false, bool bOptUnAliveTag=false);
	int  GenerateSingleTagInfo(long lTagId, bool bOptRefTag, bool bOptUnAliveTag, RF_INFO_TAG& stOutInfo);

	//-------------------------------

	bool m_PostThreadRunStatus;
	int m_nSticks2;
	//Override
	void roundPro2();

public:
	//优先级:  ant > pointRegion > lineRegion > mirror rectRegion > rectRegion
	int GetTag_AllRegionRssiList(long nTagID, PRegionTagRssiListM pRegionTagRssiList);  //返回 有效信息数量    指针建议分配100
	
	int GetRecentRegion(long nTagID);   //获取待测卡所在关联性最大的区域  返回值  区域索引号                 
	
	//bool GetTagXY(long nTag, int nRegionID, PVOID pRssiData, PVOID pReaderData, floatP* RegionPoint, floatP* WorldPoint); 
	
	//bool dB_to_XY_IrregularShape(PVOID pRssiData, long TagID, CPoint* AlgPoint);

private:  
	bool m_bIsClassInit;
	bool m_bIsThreadStart;   //线程启动标志  启动则屏蔽外部信息

	RF_CReaderManager* m_pDev;

	CRF_RegionFactory_M* m_pRegionFac;
	RF_Region_s* m_pRegionList;
	int GetNewReigonRoom();  // > 0
	int GetOldReigonRoom(int oldId); // > 0   

	DWORD m_hUserHandle;
	RF_CALLBACK_TAG m_cbAllTagInfo;
	RF_CALLBACK_TAG_SINGLE m_cbSingleTagInfo;  

	CRF_Map_M<RF_Ant125Attr_s> m_antArray;

public:
	int CreateAnt(int antId, RF_Ant125Attr_s attr);

	int GetAntPoint(int antId, floatP* pData_out);

};

floatP GetNextPoint(floatP& SrcPoint, floatP& DstPoint);

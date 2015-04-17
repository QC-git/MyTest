#pragma once

#include "RF_Math.h"

#define UNTAG_MAX_TIMEOUT          30
#define UNTAG_EFFECT_MIN_RSSI      75
#define UNTAG_MOVE_SPEED         6.6f
#define UNTAG_MOVE_UPDATA_TIME    500
#define UNTAG_MOVE_MAX_RANGE       60

struct RF_STagInfo
{
	long TagID;

	bool Alive;              //是否活跃
	bool IsRef;              //是否作为参考标签
	int Power;

	UINT SSR;      //  最强信号的接收器
	float SSRssi;  //  最强信号的强度
	long DataTime;  // 获取数据时的时间
	long AlgTime;  //  经过计算后的时间

	int RR;      //  Recent Region
	int Ant;

	//floatP RegionPoint;

	floatP AlgWorldPoint;   //通过算法算出来的坐标
	//floatP CurWorldPoint;   //经过线性过滤后的坐标
};

class CRF_TagManager_M  // 射频卡  信息池
{
public:
	CRF_TagManager_M();
	~CRF_TagManager_M();

	bool IsPoolInit() const;

	static const int TAG_MAX_ROOM=1000;

	//----------------------

	bool IsTagRef(long lTagID);

	int  Get_AllTagInfo(
		RF_STagInfo* pInfo_out, 
		bool bIsOutputRefTag=false,
		bool bIsOutputUnAliveTag=false);  // return count

	int Get_TagInfo(long lTagID, RF_STagInfo* pData_out);

protected:  // 存读取时需要加互斥和初始化判断
	UINT Get_TagRoom(long lTagID, bool bIsAutoDel=false);   //return room+1 ;

	bool SetTagRef(UINT uiRoom);

	void SetTagUnAlive(UINT uiRoom);
	bool IsTagAlive(UINT uiRoom);

	void SaveSSRex(UINT uiRoom, int SSR, float fSSRssi, int nPower, long tDataTime);
	bool SaveRRex(UINT uiRoom, int nRR, int nAnt, floatP AlgWorldPoint);

	//----------------------------------------

	//floatP GetNextPoint(floatP& SrcPoint, floatP& DstPoint);
private:
	bool m_bIsPoolInit;

	CRITICAL_SECTION m_cs;
	RF_STagInfo* m_tagList;

};
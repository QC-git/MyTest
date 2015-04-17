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

	bool Alive;              //�Ƿ��Ծ
	bool IsRef;              //�Ƿ���Ϊ�ο���ǩ
	int Power;

	UINT SSR;      //  ��ǿ�źŵĽ�����
	float SSRssi;  //  ��ǿ�źŵ�ǿ��
	long DataTime;  // ��ȡ����ʱ��ʱ��
	long AlgTime;  //  ����������ʱ��

	int RR;      //  Recent Region
	int Ant;

	//floatP RegionPoint;

	floatP AlgWorldPoint;   //ͨ���㷨�����������
	//floatP CurWorldPoint;   //�������Թ��˺������
};

class CRF_TagManager_M  // ��Ƶ��  ��Ϣ��
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

protected:  // ���ȡʱ��Ҫ�ӻ���ͳ�ʼ���ж�
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
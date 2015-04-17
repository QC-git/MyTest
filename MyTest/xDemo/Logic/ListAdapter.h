#pragma once

#include "stdafx.h"
#include "RF_Position_API_v2.h"

#include <set>
#include <map>
#include <hash_map>
#include "x_container.h"

//typedef map<long lKey, long lTagId> MAP_TAG_T;

//map<long, long> hello;
#define  WM_REQUEST_LIST_UPDATA			WM_USER+10001

#define MD_STRING_RSSI      "ǿ��"
#define MD_STRING_COUNT     "����"
#define MD_STRING_TIME      "ʱ��"
#define MD_STRING_POWER     "����"
#define MD_STRING_ANT       "����"
#define MD_STRING_ANTRSSI   "����ǿ��"


class CRfListAdapter  // ��ά�б�������
{
public:
	CRfListAdapter(HWND hWnd, CListCtrl* pListCtrl);
	~CRfListAdapter();

	struct SPack 
	{
		//int nIndex;

		//RF_INFO_RSSI_OUT stInfo;

		long ReaId;
		long TagId;

		RF_INFO_RSSI_OUT stInfo;
	};

	typedef X::CValueHashMap<long, int> ReaMap_t;
	typedef X::CValueHashMap<long, int> TagMap_t;

	typedef X::CValueHashMap<long, SPack> PackMap_t;

	typedef X::CValueSet<long> PackIndexSet_T;

public:
	int AddNewUpData(long lColumnId, long lItemId, RF_INFO_RSSI_OUT* pDataPack);

	void Clear();

	void UpDataList(const char* strIndex); //���ڸ��ⲿ���ڵ���

private:

	long lLastTime;

	CListCtrl* m_pListCtrl;

	HWND m_hWnd; 

	bool m_bIsStopRecieveInfo;

	CRITICAL_SECTION m_csLock;

	int m_nInsertCount;

	ReaMap_t m_ReaMap;
	TagMap_t m_TeaMap;
	int m_nReaCreateCount;
	int m_nTagCreateCount;

	PackMap_t m_PackMap;
	int m_nPackCount;
	PackIndexSet_T m_PackIndexSet;
};
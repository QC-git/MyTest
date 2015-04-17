
#include "x_container.h"

#include "x_util.h"

_SERVOCE_DOOR_BEGIN

Container::Container()
{
	;
}

Container::~Container()
{
	;
}

U32_T Container::Size()
{
	LOGW_F("Container::Size(), no implement");
	return 0;
}

VOID_T Container::Clear()
{
	LOGW_F("Container::Clear(), no implement");
	;
}

ContainerIter::ContainerIter()
{
	;
}

ContainerIter::~ContainerIter()
{
	;
}

VOID_T ContainerIter::Reset()
{
	LOGW_F("Container::Reset(). no implement");
}

// ≤‚ ‘ CValueSet
ERR_T ContainerTest1()
{
	LOGD_F("ContainerTest1()£¨ CValueSet");

	CValueSet<long> tSet;
	int nSize;

	LOGD_F("-----------1-----------");

	for (int n=0; n<50; n++)
	{
		BOOL_T bSuc = tSet.Add(n);
		nSize = tSet.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}


	LOGD_F("-----------2-----------");

	for (int n=25; n<75; n++)
	{
		BOOL_T bSuc = tSet.Add(n);
		nSize = tSet.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}

	LOGD_F("-----------3-----------");

	for (int n=0; n<100; n++)
	{
		BOOL_T bSuc = tSet.IsExist(n);
		nSize = tSet.Size();
		LOGD_F(" n = %d, IsExist = %d, nSize = %d", n, bSuc, nSize);
	}

	LOGD_F("-----------4-----------");

	CValueSet<long>::Iter tSetIter(tSet);
	int nCount = 0;
	long lRelVaule;
	while( tSetIter.Next(lRelVaule) )
	{
		nCount++;
		LOGD_F(" nCount = %d, lRelVaule = %d ", nCount, lRelVaule);
	}

	tSet.Clear();
	nSize = tSet.Size();
	LOGD_F(" nSize = %d ", nSize);

	tSetIter.Reset();
	while( tSetIter.Next(lRelVaule) )
	{
		nCount++;
		LOGD_F(" nCount = %d, lRelVaule = %d ", nCount, lRelVaule);
	}

	return 0;
}

// ≤‚ ‘ CValueHashMap
ERR_T ContainerTest2()
{
	LOGD_F("ContainerTest1()£¨ CValueHashMap");

	CValueHashMap<long, int> tMap;
	int nSize;

	LOGD_F("-----------1-----------");

	for (int n=0; n<100; n++)
	{
		BOOL_T bSuc = tMap.Insert(n, n+1);
		nSize = tMap.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}

	LOGD_F("-----------2-----------");

	for (int n=100; n<150; n++)
	{
		BOOL_T bSuc = tMap.Insert(n, n+1001);
		nSize = tMap.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}

	LOGD_F("-----------3-----------");

	for (int n=125; n<175; n++)
	{
		BOOL_T bSuc = tMap.Updata(n, n+2001);
		nSize = tMap.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}

	LOGD_F("-----------4-----------");

	
	for (int n=100; n<200; n++)
	{
		int nValue;
		BOOL_T bSuc = tMap.Find(n, &nValue);
		nSize = tMap.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d, nValue = %d", n, bSuc, nSize, nValue);
	}

	LOGD_F("-----------5-----------");

	CValueHashMap<long, int>::Iter tMapIter(tMap);
	int nCount = 0;
	long RelKey;
	int RelVaule;
	while( tMapIter.Next(RelKey, RelVaule) )
	{
		nCount++;
		LOGD_F(" nCount = %d, RelKey = %d, RelVaule = %d ", nCount, RelKey, RelVaule);
	}

	tMap.Clear();
	nSize = tMap.Size();
	LOGD_F(" nSize = %d ", nSize);

	LOGD_F("-----------6-----------");

	tMapIter.Reset();
	while( tMapIter.Next(RelKey, RelVaule) )
	{
		nCount++;
		LOGD_F(" nCount = %d, RelKey = %d, RelVaule = %d ", nCount, RelKey, RelVaule);
	}

	for (int n=0; n<100; n++)
	{
		BOOL_T bSuc = tMap.Insert(n, n+3001);
		nSize = tMap.Size();
		LOGD_F(" n = %d, bSuc = %d, nSize = %d", n, bSuc, nSize);
	}

	tMapIter.Reset();
	while( tMapIter.Next(RelKey, RelVaule) )
	{
		nCount++;
		LOGD_F(" nCount = %d, RelKey = %d, RelVaule = %d ", nCount, RelKey, RelVaule);
	}

	return 0;
}

_SERVICE_IMPLEMENT ERR_T ContainerTest()
{
	LOGD_F("ContainerTest()");

	X::CValueSet<int> cSet;
	X::CValueSet<int>::StlSet_t cSet1;
	X::CValueSet<int>::Iter cIter(cSet);

	X::CValueHashMap<int, int*> m_cMap;
	X::CValueHashMap<int, int*>::Iter m_cMapIter(m_cMap);

	//ContainerTest1();

	ContainerTest2();

	return 0;
}

_SERVOCE_DOOR_END
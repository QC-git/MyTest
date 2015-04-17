
#ifdef X_SERVICE_USE_PRAGMAONCE
#pragma once
#endif

#ifndef _SERVOCE_DOOR_CONTAINER_HEADER
#define _SERVOCE_DOOR_CONTAINER_HEADER

#include <set>
#include <map>
#include <hash_map>

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

/*
************************************************************************************************************************
*                                                    Container
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS Container
{
public:
	Container();
	virtual ~Container();

public:
	virtual U32_T  Size();
	virtual VOID_T Clear();

};

/*
************************************************************************************************************************
*                                                    ContainerIter
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS ContainerIter
{
public:
	ContainerIter();
	virtual ~ContainerIter();

public:
	virtual VOID_T Reset();

};

/*
************************************************************************************************************************
*                                                    CValueSet
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
template<class _Value>
class _SERVICE_TAMPLATE_CLASS CValueSet: public Container
{
public:
	CValueSet() { m_set.clear(); }
	~CValueSet() {;}

	typedef std::set<_Value>            StlSet_t;
	typedef typename StlSet_t::iterator StlSetIter_t;

public:
	// Override
	U32_T Size() 
	{ 
		int n = m_set.size(); 
		return n>=0 ? (U32_T)n : 0 ;
	}
	
	// Override
	VOID_T Clear()
	{ 
		m_set.clear(); 
	}

	BOOL_T IsExist(_Value value)
	{
		StlSetIter_t old_iter = m_set.find(value);

		return old_iter != m_set.end() ? TRUE : FALSE;
	}

	BOOL_T Add(_Value value)
	{
		if (IsExist(value)) return TRUE;  //检查是否已经存在

		return (BOOL_T)m_set.insert(value).second;
	}

	//Value GetValueByIndex(int nIndex) { return m_set[nIndex]; }

public:
	class Iter: public ContainerIter
	{
	public:
		Iter(CValueSet& set)
			: m_set(set.m_set)
		{
			m_set_iter = m_set.begin();
		}
		~Iter() {;}

	public:
		// Override
		VOID_T Reset() 
		{ 
			m_set_iter = m_set.begin(); 
		}

		BOOL_T Next(_Value& value) 
		{
			if ( m_set_iter == m_set.end() )
				return FALSE;

			value = *m_set_iter;

			m_set_iter++;
			return TRUE;
		}

	private:
		StlSetIter_t m_set_iter;
		StlSet_t& m_set;

	};

	friend class CValueSet::Iter;

private:

	StlSet_t m_set;
};

/*
************************************************************************************************************************
*                                                    CValueMapHash
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
template<class _Key, class _Value>
class _SERVICE_TAMPLATE_CLASS CValueHashMap: public Container
{
public:
	CValueHashMap() { m_map.clear(); }
	~CValueHashMap() {;}

	typedef std::hash_map<_Key, _Value>   StlMap_t;
	typedef typename StlMap_t::iterator   StlMapIter_t;
	typedef std::pair<_Key, _Value>       StlMapPair_t;
	//typedef std::pair<StlMapIter_t, bool> StlMapRet_t;

public:
	// Override
	U32_T Size() 
	{ 
		int n = m_map.size(); 
		return n>=0 ? (U32_T)n : 0 ;
	}

	// Override
	VOID_T Clear()
	{ 
		m_map.clear(); 
	}

	BOOL_T IsExist(_Key key)
	{
		StlMapIter_t old_iter = m_map.find(key);

		return old_iter != m_map.end() ? TRUE : FALSE;
	}

	BOOL_T Find(_Key key, _Value* pvalue = NULL)
	{
		StlMapIter_t old_iter = m_map.find(key);

		if ( old_iter != m_map.end() )
		{
			if (pvalue)
			{
				*pvalue = old_iter->second;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL_T Insert(_Key key, _Value value)
	{
		StlMapIter_t old_iter = m_map.find(key);
		if ( old_iter == m_map.end() )
		{
			return (BOOL_T)m_map.insert(StlMapPair_t(key, value)).second;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL_T Updata(_Key key, _Value value)
	{
		StlMapIter_t old_iter = m_map.find(key);
		if ( old_iter != m_map.end() )
		{
			old_iter->second = value;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL_T UpdataEx(_Key key, _Value value)
	{
		StlMapIter_t old_iter = m_map.find(key);
		if ( old_iter != m_map.end() )
		{
			old_iter->second = value;
			return TRUE;
		}
		else
		{
			return (BOOL_T)m_map.insert(StlMapPair_t(key, value)).second;
		}
	}


public:
	class Iter: public ContainerIter
	{
	public:
		Iter(CValueHashMap& map)
			: m_map(map.m_map)
		{
			m_map_iter = m_map.begin();
		}
		~Iter() {;}

	public:
		// Override
		VOID_T Reset() 
		{ 
			m_map_iter = m_map.begin(); 
		}

		BOOL_T Next(_Key& key, _Value& value)
		{
			if ( m_map_iter == m_map.end() )
				return FALSE;

			StlMapPair_t data = *m_map_iter;
			key = data.first;
			value = data.second;

			m_map_iter++;
			return TRUE;
		}

	private:
		StlMapIter_t m_map_iter;
		StlMap_t& m_map;
	};

	friend class CValueHashMap::Iter;	

private:
	StlMap_t m_map;

};

/*
************************************************************************************************************************
*                                                    ContainerTest
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT ERR_T ContainerTest();

_SERVOCE_DOOR_END
	//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif
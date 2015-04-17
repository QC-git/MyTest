#include "StdAfx.h"

#include "RF_TagManager.h"


/************************************************************************/
/*                           CRF_TagInfoPool_M                          */
/*                              public                                  */
/************************************************************************/

CRF_TagManager_M::CRF_TagManager_M()
	:m_bIsPoolInit(false)
{
	TRACE("\n CRF_TagInfoPool_M ");

	m_tagList=new RF_STagInfo[TAG_MAX_ROOM];
	if ( m_tagList==NULL )
	{
		return;
	}
	memset( m_tagList, 0, sizeof(RF_STagInfo)*TAG_MAX_ROOM );

	m_bIsPoolInit=true;
	InitializeCriticalSection(&m_cs);

	TRACE("\n CRF_TagInfoPool_M end ");
};

CRF_TagManager_M::~CRF_TagManager_M()
{
	if (m_tagList)
	{
		delete[] m_tagList;
	}

	DeleteCriticalSection(&m_cs);
	TRACE("\n CRF_TagInfoPool_M::~CRF_TagInfoPool_M \n");
}

bool CRF_TagManager_M::IsPoolInit() const
{
	return m_bIsPoolInit;
}

//////////////////////////////////////////////////////////////////////////

bool CRF_TagManager_M::IsTagRef(long lTagID)
{
	if ( !IsPoolInit() )
	{
		return FALSE;
	}

	bool bIsRef=false;

	if (lTagID)
	{
		EnterCriticalSection(&m_cs);
		for (int n=0; n<TAG_MAX_ROOM; n++)
		{
			if ( lTagID==m_tagList[n].TagID )
			{
				bIsRef=m_tagList[n].IsRef;
			}
		}
		LeaveCriticalSection(&m_cs);
	}

	return bIsRef;

}

int  CRF_TagManager_M::Get_AllTagInfo(
	RF_STagInfo* pInfo_out, 
	bool bIsOutputRefTag/* =false */,
	bool bIsOutputUnAliveTag/* =false */)
{
	if ( !IsPoolInit() )
	{
		return 0;
	}

	int nCount=0;

	EnterCriticalSection(&m_cs);
	for (int n=0; n<TAG_MAX_ROOM; n++)
	{
		if ( m_tagList[n].TagID==0 )
		{
			break;
		}

		if (!bIsOutputUnAliveTag)
		{
			if ( m_tagList[n].Alive==false )
			{
				continue;
			}
		}

		if (!bIsOutputRefTag)
		{
			if ( m_tagList[n].IsRef=true )
			{
				continue;
			}
		}

		*pInfo_out=m_tagList[n];
		nCount++;

	}
	LeaveCriticalSection(&m_cs);

	return nCount;
}

int CRF_TagManager_M::Get_TagInfo(long lTagID, RF_STagInfo* pData_out)
{
	if ( !IsPoolInit() )
	{
		return -1;
	}

	if ( lTagID==0 || NULL == pData_out )
	{
		return -2;
	}

	UINT uiRoom=0;
	EnterCriticalSection(&m_cs);
	for (int n=0; n<TAG_MAX_ROOM; n++)
	{
		if (m_tagList[n].TagID==lTagID)
		{
			(*pData_out) = m_tagList[n];
			uiRoom = n+1;
			break;;
		}
	}

	LeaveCriticalSection(&m_cs);

	if ( 0 == uiRoom )
	{
		return -3;
	}

	return 0;

}

/************************************************************************/
/*                            Protected                                 */
/************************************************************************/

UINT CRF_TagManager_M::Get_TagRoom(long lTagID, bool bIsAutoDel /* =false */ )   //return room+1 ;
{
	if ( !IsPoolInit() )
	{
		return 0;
	}

	if (lTagID==0)
	{
		return 0;
	}

	UINT uiRoom=0;
	EnterCriticalSection(&m_cs);
	for (int n=0; n<TAG_MAX_ROOM; n++)
	{
		if ( lTagID == m_tagList[n].TagID)
		{
			uiRoom=n+1;
			break;
		}
	}

	if ( 0 == uiRoom )
	{
		for (int n=0; n<TAG_MAX_ROOM; n++)
		{
			if ( 0 == m_tagList[n].TagID )
			{
				m_tagList[n].TagID=lTagID;
				uiRoom=n+1;
				break;
			}
		}
	}


	if (bIsAutoDel)
	{
		memset(m_tagList, 0, sizeof(RF_STagInfo)*TAG_MAX_ROOM);
		m_tagList[0].TagID=lTagID;
		uiRoom=1;
	}
	LeaveCriticalSection(&m_cs);

	return uiRoom;
}

bool CRF_TagManager_M::SetTagRef(UINT uiRoom)
{
	if ( !IsPoolInit() )
	{
		return FALSE;
	}

	if (uiRoom)
	{
		EnterCriticalSection(&m_cs);
		m_tagList[uiRoom-1].IsRef=true;
		LeaveCriticalSection(&m_cs);

		return TRUE;
	}

	return FALSE;
}

void CRF_TagManager_M::SetTagUnAlive(UINT uiRoom)
{
	EnterCriticalSection(&m_cs);
	m_tagList[uiRoom-1].Alive=false;
	LeaveCriticalSection(&m_cs);
}

bool CRF_TagManager_M::IsTagAlive(UINT uiRoom)
{
	bool bIsAlive=false;
	if (uiRoom)
	{
		EnterCriticalSection(&m_cs);
		bIsAlive=m_tagList[uiRoom-1].Alive;
		LeaveCriticalSection(&m_cs);
	}

	return bIsAlive;
}

void CRF_TagManager_M::SaveSSRex(UINT uiRoom, int nSSR, float fSSRssi, int nPower, long tDataTime)
{
	//判断射频卡是否活跃

	bool bActive=true;
	if ( fSSRssi>UNTAG_EFFECT_MIN_RSSI ) //强度过小
	{
		bActive=false;
	}

	long tAlgTime = time(NULL); //算术时间
	long tOffTime = tAlgTime - tDataTime;
	if ( tOffTime > UNTAG_MAX_TIMEOUT ) //数据时间太久未更新
	{
		TRACE("\n tag = %x tOffTime = %d > UNTAG_MAX_TIMEOUT ", m_tagList[uiRoom-1].TagID, tOffTime);
		bActive=false;
	}

	EnterCriticalSection(&m_cs);
	RF_STagInfo* p=&(m_tagList[uiRoom-1]);
	p->SSR=nSSR;
	p->SSRssi=fSSRssi;
	p->Power=nPower;
	p->DataTime=tDataTime;
	p->AlgTime=tAlgTime;
	p->Alive=bActive;
	LeaveCriticalSection(&m_cs);

	//return TRUE;
}

bool CRF_TagManager_M::SaveRRex(UINT uiRoom, int nRR, int nAnt, floatP AlgWorldPoint)
{
	if ( !IsPoolInit() )
	{
		return FALSE;
	}

	if (uiRoom)
	{
		EnterCriticalSection(&m_cs);
		RF_STagInfo* p=&(m_tagList[uiRoom-1]);

		p->RR=nRR;
		p->Ant=nAnt;
		p->AlgWorldPoint=AlgWorldPoint;

		LeaveCriticalSection(&m_cs);

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "ListAdapter.h"
//#include "RfInfoDlg.h"
#include "RkUtil.h"
 
//////////////////////////////////////////////////////////////////////////

CRfListAdapter::CRfListAdapter(HWND hWnd, CListCtrl* pListCtrl)
	: lLastTime(0)
	, m_pListCtrl(pListCtrl) //不能为空
	, m_hWnd(hWnd)
	, m_bIsStopRecieveInfo(false)
	//, m_hashDataPool((size_t)1000)
	, m_nInsertCount(0)
	, m_nReaCreateCount(0)
	, m_nTagCreateCount(0)
	, m_nPackCount(0)
{
	Clear();
	//m_setRea.
	InitializeCriticalSection(&m_csLock);
}

CRfListAdapter::~CRfListAdapter()
{
	DeleteCriticalSection(&m_csLock);
}

int CRfListAdapter::AddNewUpData(long lColumnId, long lItemId, RF_INFO_RSSI_OUT* pDataPack)
{
	//LOG_F("CRfListAdapter::AddNewUpData()");

	if ( 0 == lColumnId || 0 == lItemId || NULL == pDataPack )
		return -1;

	m_nInsertCount++;

	int nStatus = 0;
	bool bSuc = false;
	EnterCriticalSection(&m_csLock);
	do 
	{
		nStatus--;
		long lReakey = lColumnId;
		int nReaIndex;
		if (!m_ReaMap.Find(lReakey, &nReaIndex))
		{
			m_nReaCreateCount++;
			nReaIndex = m_nReaCreateCount;
			LOG_F("CRfListAdapter::AddNewUpData(), m_nReaCreateCount = %d ", m_nReaCreateCount);

			if (!m_ReaMap.Insert(lReakey, nReaIndex))
			{
				LOGERR_F("CRfListAdapter::AddNewUpData(), m_ReaMap.Add(), fail");
				break;
			}
		}

		nStatus--;
		long lTagKey = lItemId;
		int nTagIndex;
		if (!m_TeaMap.Find(lTagKey, &nTagIndex))
		{
			m_nTagCreateCount++;
			nTagIndex = m_nTagCreateCount;
			LOG_F("CRfListAdapter::AddNewUpData(), m_nTagCreateCount = %d ", m_nTagCreateCount);

			if (!m_TeaMap.Insert(lTagKey, nTagIndex))
			{
				LOGERR_F("CRfListAdapter::AddNewUpData(), m_ReaMap.Add(), fail");
				break;
			}
		}

		nStatus--;
		long lDataKey = nReaIndex*1000+nTagIndex;
		SPack stPack;

		stPack.ReaId = lReakey;
		stPack.TagId = lTagKey;
		stPack.stInfo = *pDataPack;//->byRssi;

		if ( !m_PackIndexSet.Add(lDataKey) )
		{
			LOGERR_F("CRfListAdapter::AddNewUpData(), m_PackIndexSet.Add(), fail");
			break;
		}

		if (!m_PackMap.Find(lDataKey))
		{
			if (!m_PackMap.Insert(lDataKey, stPack))
			{
				LOGERR_F("CRfListAdapter::AddNewUpData(), m_PackMap.Add(), fail");
				break;
			}

			m_nPackCount++;
			LOG_F("CRfListAdapter::AddNewUpData(), m_nPackCount = %d, lDataKey = %d", m_nPackCount, lDataKey);
		}
		else
		{
			if (!m_PackMap.Updata(lDataKey, stPack))
			{
				LOGERR_F("CRfListAdapter::AddNewUpData(), m_PackMap.Add(), fail");
				break;
			}
		}

		

		bSuc = true;
	}while(false);
	LeaveCriticalSection(&m_csLock);


	long lCurTime = time(NULL);
	if ( lCurTime != lLastTime )  //每秒更新一次
	{
		lLastTime = lCurTime;

		//更新列表
		//UpDataList();
		m_bIsStopRecieveInfo = true;  // 停止更新数据
		::PostMessage(m_hWnd, WM_REQUEST_LIST_UPDATA, 0, 0);  // 发送消息给窗口用于更新列表

	}

	return bSuc ? 0 : nStatus;
}

void CRfListAdapter::Clear()
{
	//m_setRea.clear();
	//m_setTag.clear();

	m_PackMap.Clear();

	m_ReaMap.Clear();
	m_TeaMap.Clear();

	m_PackIndexSet.Clear();

}

void CRfListAdapter::UpDataList(const char* strIndex)
{
	//LOG_F("CRfListAdapter::UpDataList()");

	CListCtrl* pList = m_pListCtrl;
	//pList->DeleteAllItems();


	EnterCriticalSection(&m_csLock);

	int nItemCount = pList->GetItemCount();
	CString str;

	int nTagNum = m_TeaMap.Size();

	if ( nItemCount < nTagNum )
	{
		int nAddCnt = nTagNum - nItemCount;
		for (int n=0; n<nAddCnt; n++)
		{
			str.Format("");
			pList->InsertItem(0, str);
		}
	}

	//LongSetIter_T iter;
	str.Format("ID");
	pList->SetItemText(0, 0, str);

	ReaMap_t::Iter iterTag(m_TeaMap);
	long lTagKey;
	int  nTagIndex;

	int nReaNum = m_ReaMap.Size();
	ReaMap_t::Iter iterRea(m_ReaMap);
	long lReaKey;
	int  nReaIndex;

	for (int n=0; n<nTagNum; n++)
	{
		if (!iterTag.Next(lTagKey, nTagIndex))
		{
			LOGERR_F("CRfListAdapter::UpDataList(), iterTag.Next() fail");
			break;
		}

		str.Format("%x", lTagKey);
		pList->SetItemText(n+1, 0, str);
	}
	
	////////////////////////////////////////

	
	for (int n=0; n<nReaNum; n++)
	{
		if (!iterRea.Next(lReaKey, nReaIndex))
		{
			LOGERR_F("CRfListAdapter::UpDataList(), iterRea.Next() fail");
			break;
		}

		lReaKey = lReaKey + 80000000;
		str.Format("%d", lReaKey);
		pList->SetItemText(0,n+1,str);
	}

	iterTag.Reset();
	SPack stPack;
	for (int i=0; i<nTagNum; i++)
	{
		if (!iterTag.Next(lTagKey, nTagIndex))
		{
			LOGERR_F("CRfListAdapter::UpDataList(), iterTag.Next() fail");
			break;
		}

		iterRea.Reset();
		for (int j=0; j<nReaNum; j++)
		{
			if (!iterRea.Next(lReaKey, nReaIndex))
			{
				LOGERR_F("CRfListAdapter::UpDataList(), iterRea.Next() fail");
				break;
			}

			long lDataKey = nReaIndex*1000 + nTagIndex;
			if (!m_PackIndexSet.IsExist(lDataKey)) continue;

			if ( !m_PackMap.Find(lDataKey, &stPack) )
			{
				LOGERR_F("CRfListAdapter::UpDataList(), m_PackMap.Find() fail, lDataKey = %d", lDataKey);
				//break;
			}
			else if ( stPack.ReaId != lReaKey || stPack.TagId != lTagKey )
			{
				LOGERR_F("CRfListAdapter::UpDataList(), stPack.ReaId != lReaKey || stPack.TagId != lTagKey");
			}
			else
			{
				RF_INFO_RSSI_OUT* pData = &stPack.stInfo;
				if ( 0 == strcmp(strIndex, MD_STRING_RSSI) )
				{
					str.Format("%d", pData->byRssi);
				}
				else if ( 0 == strcmp(strIndex, MD_STRING_COUNT) )
				{
					str.Format("%d", pData->nUpdataCount);
				}
				else if ( 0 == strcmp(strIndex, MD_STRING_TIME) )
				{
					CTime cTime(pData->lTime);
					str = cTime.Format("%H:%M:%S");
				}
				else if ( 0 == strcmp(strIndex, MD_STRING_POWER) )
				{
					str.Format("%d", pData->byPower);
				}
				else if ( 0 == strcmp(strIndex, MD_STRING_ANT) )
				{
					str.Format("%d", pData->nAntID);
				}
				else if ( 0 == strcmp(strIndex, MD_STRING_ANTRSSI) )
				{
					str.Format("%d", pData->nAntRssi);
				}
				else
				{
					str.Format("???");
				}
				pList->SetItemText(i+1,j+1,str);
			}
		
		}
	}


	LeaveCriticalSection(&m_csLock);
	//Spa

	m_bIsStopRecieveInfo = false;
}
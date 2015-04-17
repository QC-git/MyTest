#include "StdAfx.h"
#include "RF_Math.h"

#include "RF_Positon.h"

#define NOMAL_MODE                  0
#define MIRROR_MODE                 1
#define CUR_ALG_MODE   MIRROR_MODE

extern RF_CReaderManager* g_pRF_Device;


floatP GetNextPoint(floatP& SrcPoint, floatP& DstPoint)
{
	floatP nstPoint=::GetNextPoint(
		SrcPoint,
		DstPoint,
		UNTAG_MOVE_SPEED,
		UNTAG_MOVE_UPDATA_TIME,
		UNTAG_MOVE_MAX_RANGE,
		1);

	return nstPoint;
}

/************************************************************************/
/*                           CRF_AlgManager_M                           */
/*                              public                                  */
/************************************************************************/

RF_CAlgManager::RF_CAlgManager(HANDLE hDevice)
	:m_bIsClassInit(false),m_bIsThreadStart(false)
{
	TRACE("\n CRF_AlgManager_M");

	m_AlgThreadRunStatus = false;
	m_PostThreadRunStatus = false;

	m_pDev=(RF_CReaderManager*)hDevice;

	m_pRegionFac =new CRF_RegionFactory_M(hDevice);
	m_pRegionList = new RF_Region_s[RF_REGION_MAX_NUM];
	if (m_pRegionList)
	{
		memset( m_pRegionList, 0, sizeof(RF_Region_s)*(RF_REGION_MAX_NUM));
	}

	m_hUserHandle = NULL;
	m_cbAllTagInfo = NULL;
	m_cbSingleTagInfo = NULL;

	m_bIsClassInit=true;

	TRACE("\n CRF_AlgManager_M end");
}

RF_CAlgManager::~RF_CAlgManager()
{
	TRACE("\n ~CRF_AlgManager_M");

	Close_TRfAlgorithm();
	Close_TRfPostPro();

	Del_AllRegion();

	if ( m_pRegionList )
	{
		delete[] m_pRegionList;
	}
	if ( m_pRegionFac )
	{
		delete m_pRegionFac;
	}
	
	TRACE("\n ~CRF_AlgManager_M end");
}

bool RF_CAlgManager::IsManagerInit() const
{
	return m_bIsClassInit;
}

UINT RF_CAlgManager::CreateRectRegion(RF_RectRegionAttr_s* Par)
{
	TRACE("\n CRF_AlgManager_M::CreateRectRegion");

	if ( NULL == m_pRegionFac || NULL == m_pRegionList )
	{
		//TRACE("\n CRF_AlgManager_M::CreateRectRegion NULL == m_pRegionFac || NULL == m_pRegionList");
		return 0;
	}

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		//TRACE("\n CRF_AlgManager_M::CreateRectRegion m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return 0;
	}

	// 寻找空房间
	int nRoom = GetNewReigonRoom();
	if ( 0 == nRoom )
	{
		TRACE("\n CRF_AlgManager_M::CreateRectRegion 0 == nRoom ");
		return 0;
	}
	RF_Region_s* pRoom = &m_pRegionList[nRoom-1];

	//创建区域
	CRF_RectRegion_M* pRegion = (CRF_RectRegion_M*)m_pRegionFac->CreateRegion(RF_REGION_STYLE_RECT);
	if ( NULL == pRegion )
	{
		TRACE("\n CRF_AlgManager_M::CreateRectRegion NULL == pRegion");
		return 0;
	}
	if ( 0 != pRegion->SetAttr(*Par) )
	{
		TRACE("\n CRF_AlgManager_M::CreateRectRegion 0 != pRegion->SetAttr(*pRectRegion)");

		m_pRegionFac->DestroyRegion(pRegion);
		return 0;
	}

	//赋值
	pRoom->nRoom = nRoom;
	pRoom->style = RF_REGION_STYLE_RECT;
	pRoom->pRegion = pRegion;

	//将输入的卡号设为餐开卡标志
	SetTagRef( Get_TagRoom(Par->ltTagID) );
	SetTagRef( Get_TagRoom(Par->lbTagID) );
	SetTagRef( Get_TagRoom(Par->rtTagID) );
	SetTagRef( Get_TagRoom(Par->rbTagID) );

	return nRoom;
}

UINT RF_CAlgManager::CreateMirrorRectRegion(RF_RectRegionAttr_s* Par)
{
	TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion");

	if ( NULL == m_pRegionFac || NULL == m_pRegionList )
	{
		//TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion NULL == m_pRegionFac || NULL == m_pRegionList");
		return 0;
	}

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		//TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return 0;
	}

	// 寻找空房间
	int nRoom = GetNewReigonRoom();
	if ( 0 == nRoom )
	{
		TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion 0 == nRoom ");
		return 0;
	}
	RF_Region_s* pRoom = &m_pRegionList[nRoom-1];

	//创建区域
	CRF_MirrorRectRegion* pRegion = (CRF_MirrorRectRegion*)m_pRegionFac->CreateRegion(RF_REGION_STYLE_RECT_MIRROR);
	if ( NULL == pRegion )
	{
		TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion NULL == pRegion");
		return 0;
	}
	if ( 0 != pRegion->SetAttr(*Par) )
	{
		TRACE("\n CRF_AlgManager_M::CreateMirrorRectRegion 0 != pRegion->SetAttr(*pRectRegion)");

		m_pRegionFac->DestroyRegion(pRegion);
		return 0;
	}

	//赋值
	pRoom->nRoom = nRoom;
	pRoom->style = RF_REGION_STYLE_RECT_MIRROR;
	pRoom->pRegion = pRegion;

	//将输入的卡号设为餐开卡标志
	SetTagRef( Get_TagRoom(Par->ltTagID) );
	SetTagRef( Get_TagRoom(Par->lbTagID) );
	SetTagRef( Get_TagRoom(Par->rtTagID) );
	SetTagRef( Get_TagRoom(Par->rbTagID) );

	return nRoom;
}

UINT RF_CAlgManager::CreateLineRegion(RF_LineRegionAttr_s attr)
{
	TRACE("\n CRF_AlgManager_M::CreateLineRegion");

	if ( NULL == m_pRegionFac || NULL == m_pRegionList )
	{
		//TRACE("\n CRF_AlgManager_M::CreateLineRegion NULL == m_pRegionFac || NULL == m_pRegionList");
		return 0;
	}

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		//TRACE("\n CRF_AlgManager_M::CreateLineRegion m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return 0;
	}

	// 寻找空房间
	int nRoom = GetNewReigonRoom();
	if ( 0 == nRoom )
	{
		TRACE("\n CRF_AlgManager_M::CreateLineRegion 0 == nRoom ");
		return 0;
	}
	RF_Region_s* pRoom = &m_pRegionList[nRoom-1];

	//创建区域
	CRF_LineRegion_M* pRegion = (CRF_LineRegion_M*)m_pRegionFac->CreateRegion(RF_REGION_STYLE_LINE);
	if ( NULL == pRegion )
	{
		TRACE("\n CRF_AlgManager_M::CreateLineRegion NULL == pRegion");
		return 0;
	}
	if ( 0 != pRegion->Region_Init(attr) )
	{
		TRACE("\n CRF_AlgManager_M::CreateLineRegion 0 != pRegion->Region_Init(attr)");

		m_pRegionFac->DestroyRegion(pRegion);
		return 0;
	}

	//赋值
	pRoom->nRoom = nRoom;
	pRoom->style = RF_REGION_STYLE_LINE;
	pRoom->pRegion = pRegion;

	//将输入的卡号设为餐开卡标志
	SetTagRef( Get_TagRoom(attr.P1_TagID) );
	SetTagRef( Get_TagRoom(attr.P2_TagID) );

	return nRoom;
}

UINT RF_CAlgManager::CreatePointRegion(RF_PointRegionAttr_s attr)
{
	TRACE("\n CRF_AlgManager_M::CreatePointRegion");

	if ( NULL == m_pRegionFac || NULL == m_pRegionList )
	{
		//TRACE("\n CRF_AlgManager_M::CreatePointRegion NULL == m_pRegionFac || NULL == m_pRegionList");
		return 0;
	}

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		//TRACE("\n CRF_AlgManager_M::CreatePointRegion m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return 0;
	}

	// 寻找空房间
	int nRoom = GetNewReigonRoom();
	if ( 0 == nRoom )
	{
		TRACE("\n CRF_AlgManager_M::CreatePointRegion 0 == nRoom ");
		return 0;
	}
	RF_Region_s* pRoom = &m_pRegionList[nRoom-1];

	//创建区域
	CRF_PointRegion_M* pRegion = (CRF_PointRegion_M*)m_pRegionFac->CreateRegion(RF_REGION_STYLE_POINT);
	if ( NULL == pRegion )
	{
		TRACE("\n CRF_AlgManager_M::CreatePointRegion NULL == pRegion");
		return 0;
	}
	if ( 0 != pRegion->SetRegionAttr(attr) )
	{
		TRACE("\n CRF_AlgManager_M::CreatePointRegion 0 != pRegion->SetRegionAttr(attr)");

		m_pRegionFac->DestroyRegion(pRegion);
		return 0;
	}

	//赋值
	pRoom->nRoom = nRoom;
	pRoom->style = RF_REGION_STYLE_POINT;
	pRoom->pRegion = pRegion;

// 	//将输入的卡号设为餐开卡标志
// 	SetTagRef( Get_TagRoom(attr.P1_TagID) );
// 	SetTagRef( Get_TagRoom(attr.P2_TagID) );

	return nRoom;
}

void RF_CAlgManager::Del_AllRegion()
{
	TRACE("\n CRF_AlgManager_M::Del_AllRegion");

	if ( NULL == m_pRegionFac || NULL == m_pRegionList )
	{
		TRACE("\n CRF_AlgManager_M::Del_AllRegion NULL == m_pRegionFac || NULL == m_pRegionList");
		return;
	}

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		TRACE("\n CRF_AlgManager_M::Del_AllRegion m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return;
	}

	RF_CRegionBase* pRegion;
	for (int n=0;n<RF_REGION_MAX_NUM;n++)
	{
		if ( 0 == m_pRegionList[n].nRoom )
		{
			break;
		}

		pRegion = m_pRegionList[n].pRegion;
		m_pRegionFac->DestroyRegion(pRegion);
	}
	memset(m_pRegionList, 0, sizeof(RF_Region_s)*RF_REGION_MAX_NUM);

}

int RF_CAlgManager::GetRegionsInfo(RF_GET_REGION_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	RF_INFO_REGION* pOutList = new RF_INFO_REGION[RF_REGION_MAX_NUM];

	int nCount = 0;

	if ( NULL == pOutList ) return -1;
	memset(pOutList, 0, sizeof(RF_INFO_REGION)* RF_REGION_MAX_NUM);

	RF_CRegionBase* pRegion;
	RF_RectRegionAttr_s  stRectInfo;
	RF_LineRegionAttr_s  stLineInfo;
	RF_PointRegionAttr_s stPointInfo;
	RF_INFO_REGION* p;
	for (int n=0;n<RF_REGION_MAX_NUM;n++)
	{
		if ( 0 == m_pRegionList[n].nRoom )
		{
			break;
		}

		pRegion = m_pRegionList[n].pRegion;
		
		switch(pRegion->GetStyle())
		{
		case RF_REGION_STYLE_RECT:
			{
				memset(&stRectInfo, 0, sizeof(RF_RectRegionAttr_s));

				if ( 0 == ((CRF_RectRegion_M*)pRegion)->ReadAttr(&stRectInfo) )
				{
					p = &pOutList[nCount];

					p->nIndex = pRegion->GetId();
					p->nRectType = 0;

					p->stRect.left   = stRectInfo.left;
					p->stRect.right  = stRectInfo.right;
					p->stRect.top    = stRectInfo.top;
					p->stRect.bottom = stRectInfo.bottom;

					p->stRect.ReaID[0] = stRectInfo.lbReaderID;
					p->stRect.ReaID[1] = stRectInfo.rbReaderID;
					p->stRect.ReaID[2] = stRectInfo.rtReaderID;
					p->stRect.ReaID[3] = stRectInfo.ltReaderID;

					p->stRect.TagID[0] = stRectInfo.lbTagID;
					p->stRect.TagID[1] = stRectInfo.rbTagID;
					p->stRect.TagID[2] = stRectInfo.rtTagID;
					p->stRect.TagID[3] = stRectInfo.ltTagID;

					nCount++;
				}

			}break;
		case RF_REGION_STYLE_RECT_MIRROR:
			{
				memset(&stRectInfo, 0, sizeof(RF_RectRegionAttr_s));

				if ( 0 == ((CRF_MirrorRectRegion*)pRegion)->ReadAttr(&stRectInfo) )
				{
					p = &pOutList[nCount];

					p->nIndex = pRegion->GetId();
					p->nRectType = 1;

					p->stRect.left   = stRectInfo.left;
					p->stRect.right  = stRectInfo.right;
					p->stRect.top    = stRectInfo.top;
					p->stRect.bottom = stRectInfo.bottom;

					p->stRect.ReaID[0] = stRectInfo.lbReaderID;
					p->stRect.ReaID[1] = stRectInfo.rbReaderID;
					p->stRect.ReaID[2] = stRectInfo.rtReaderID;
					p->stRect.ReaID[3] = stRectInfo.ltReaderID;

					p->stRect.TagID[0] = stRectInfo.lbTagID;
					p->stRect.TagID[1] = stRectInfo.rbTagID;
					p->stRect.TagID[2] = stRectInfo.rtTagID;
					p->stRect.TagID[3] = stRectInfo.ltTagID;

					nCount++;
				}
			}break;
		case RF_REGION_STYLE_LINE:
			{
				memset(&stLineInfo, 0, sizeof(RF_LineRegionAttr_s));

				if ( true == ((CRF_LineRegion_M*)pRegion)->Read_RegionAttr(&stLineInfo) )
				{
					p = &pOutList[nCount];

					p->nIndex = pRegion->GetId();
					p->nRectType = 2;

					p->stLine.P1x = stLineInfo.P1.x;
					p->stLine.P1y = stLineInfo.P1.y;
					p->stLine.P2x = stLineInfo.P2.x;
					p->stLine.P2y = stLineInfo.P2.y;

					p->stLine.P1_ReaderID = stLineInfo.P1_ReaderID;
					p->stLine.P2_ReaderID = stLineInfo.P2_ReaderID;

					p->stLine.P1_TagID = stLineInfo.P1_TagID;
					p->stLine.P2_TagID = stLineInfo.P2_TagID;

					nCount++;
				}

			}break;
		case RF_REGION_STYLE_POINT:
			{
				memset(&stPointInfo, 0, sizeof(RF_PointRegionAttr_s));

				if ( 0 == ((CRF_PointRegion_M*)pRegion)->GetRegionAttr(&stPointInfo) )
				{
					p = &pOutList[nCount];

					p->nIndex = pRegion->GetId();
					p->nRectType = 3;

					p->stPoint.x = stPointInfo.readerPoint.x;
					p->stPoint.y = stPointInfo.readerPoint.y;

					p->stPoint.uiReaderID = stPointInfo.readerId;
					p->stPoint.uiTriggerID = 0;

					nCount++;
				}
			}break;
		default:break;
		}

	}

	int nAntRoomSize = m_antArray.Size();
	RF_Ant125Attr_s stAntInfo;
	for (int n=0; n<nAntRoomSize; n++)
	{
// 		if ( 0 != m_antArray.Read(n+1, &stAntInfo) ) break;
// 
// 		p = &pOutList[nCount];
// 
// 		p->nIndex = n+1;
// 		p->nRectType = 4;
// 		p->stPoint.x = m_antArray[n].antPonit.x;
// 		p->stPoint.y = m_antArray[n].antPonit.y;
// 
// 		nCount++;

		stAntInfo = m_antArray[n];

		p = &pOutList[nCount];
		p->nIndex = 0;//stAntInfo.nAntId;
		p->nRectType = 4;
		p->stPoint.x = stAntInfo.antPonit.x;
		p->stPoint.y = stAntInfo.antPonit.y;
		p->stPoint.uiReaderID = 0;
		p->stPoint.uiTriggerID = (unsigned int)stAntInfo.nAntId;

		nCount++;
	}

	if ( nCount >0 )
	{
		(*pAfterGetFunc)(pOutList, nCount, hUser);
	}
	else
	{
		delete[] pOutList;
	}

	return nCount;
}

int RF_CAlgManager::GetRegionsVssInfo(UINT uiRegionId, RF_GET_VSS_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	int nStatus = 0;

	RF_CRegionBase* pRegion;
	do
	{
		nStatus--;
		if ( 0 == uiRegionId || NULL == pAfterGetFunc ) break; 

		nStatus--;
		bool bFindSuc = false;  // 是否找到该区域
		bool bReadSuc = false ;  // 是否成功读取
		for (int n=0;n<RF_REGION_MAX_NUM;n++)
		{
			if ( (int)uiRegionId != m_pRegionList[n].nRoom )
			{
				continue;
			}

			
			pRegion = m_pRegionList[n].pRegion;
			RF_REGION_STYLE emRegionStyle = pRegion->GetStyle();
			switch(emRegionStyle)
			{
			case RF_REGION_STYLE_RECT:
				{
					bFindSuc = true;

					int nBuffLen = 4*31*31;
					float* pVssBuff = new float[nBuffLen];
					float fMaxValue;
					float fMinValue;

					int nLen = ((CRF_RectRegion_M*)pRegion)->Read_VSS(0, pVssBuff, &fMaxValue, &fMinValue);
					if ( nLen == nBuffLen )
					{
						// 回调输出数据
						(*pAfterGetFunc)(uiRegionId, emRegionStyle, pVssBuff, nBuffLen, fMaxValue, fMinValue, hUser);

						bReadSuc = true;
					}

					delete[] pVssBuff;
				}break;
			case RF_REGION_STYLE_RECT_MIRROR:
				{
					bFindSuc = true;

					int nBuffLen = 4*31*31;
					float* pVssBuff = new float[nBuffLen];
					float fMaxValue;
					float fMinValue;

					int nLen  = ((CRF_MirrorRectRegion*)pRegion)->Read_VSS(0, pVssBuff, &fMaxValue, &fMinValue);
					if ( nLen == nBuffLen )
					{
						// 回调输出数据
						(*pAfterGetFunc)(uiRegionId, emRegionStyle, pVssBuff, nBuffLen, fMaxValue, fMinValue, hUser);

						bReadSuc = true;
					}

					delete[] pVssBuff;
				}break;
			case RF_REGION_STYLE_LINE:
				{
					bFindSuc = true;

					int nBuffLen = 2*31;
					float* pVssBuff = new float[nBuffLen];
					float fMaxValue;
					float fMinValue;

					int nRet = ((CRF_LineRegion_M*)pRegion)->Read_VSS(0, pVssBuff, &fMaxValue, &fMinValue);
					if ( nRet == nBuffLen )
					{
						// 回调输出数据
						(*pAfterGetFunc)(uiRegionId, emRegionStyle, pVssBuff, nBuffLen, fMaxValue, fMinValue, hUser);

						bReadSuc = true;
					}

					delete[] pVssBuff;
				}break;
			default:break;
			}

			break;
		}
		if (!bFindSuc) break;  //-2

		nStatus--;
		if (!bReadSuc) break;  //-3

		return 0;
	}while(false);

	return nStatus;
}

int RF_CAlgManager::SetUserHandle(DWORD hHandle)
{
	if ( true == m_AlgThreadRunStatus || 
		true == m_PostThreadRunStatus )
	{
		return -1;
	}

	m_hUserHandle = hHandle;

	return 0;
}

int RF_CAlgManager::SetSingleTagInfoCallBack(RF_CALLBACK_TAG_SINGLE cb)
{
	if ( true == m_AlgThreadRunStatus || 
		true == m_PostThreadRunStatus )
	{
		return -1;
	}

	m_cbSingleTagInfo = cb;

	return 0;
}

int RF_CAlgManager::SetAllTagInfoCallBack(RF_CALLBACK_TAG cb)
{
	if ( true == m_AlgThreadRunStatus || 
		true == m_PostThreadRunStatus )
	{
		return -1;
	}

	m_cbAllTagInfo = cb;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

int RF_CAlgManager::CheckRegionID(int nID)
{
	if ( !IsManagerInit() )
	{
		return 0;
	}

	return GetOldReigonRoom(nID);
}

RF_CRegionBase* RF_CAlgManager::GetRegionHandle(int nID, RF_Region_s* pInfo_out /* =NULL */)
{
	if ( NULL == m_pRegionList || nID <= 0 )
	{
		TRACE("\n CRF_AlgManager_M::Get_RegionInfo NULL == m_pRegionList || nID <= 0");
		return NULL;
	}

	int nRoom = GetOldReigonRoom(nID);
	if ( 0 == nRoom )
	{
		return NULL;
	}

	if (pInfo_out)
	{
		*pInfo_out = m_pRegionList[nRoom-1];
	}

	return m_pRegionList[nRoom-1].pRegion;
}


//////////////////////////////////////////////////////////////////////////

bool RF_CAlgManager::Startup_TRfAlgorithm()
{
	if ( !IsManagerInit() )
	{
		return FALSE;
	}

	m_AlgThreadRunStatus = true;
	return MDoubleThread::TStartup1();
}

int  RF_CAlgManager::Close_TRfAlgorithm()
{
	if ( !IsManagerInit() )
	{
		return -1;
	}
	MDoubleThread::TClose1();
	m_AlgThreadRunStatus = false;

	return 1;
}

bool RF_CAlgManager::Startup_TRfPostPro()
{
	if ( !IsManagerInit() )
	{
		return FALSE;
	}

	m_PostThreadRunStatus = true;
	return MDoubleThread::TStartup2();
}

int  RF_CAlgManager::Close_TRfPostPro()
{
	if ( !IsManagerInit() )
	{
		return -1;
	}

	MDoubleThread::TClose2();
	m_PostThreadRunStatus = false;

	return 1;
}

/************************************************************************/
/*                              protected                               */
/************************************************************************/

void RF_CAlgManager::roundPro1()
{
	m_nSticks1++;
	if (m_nSticks1<0 || m_nSticks1>300)
	{
		m_nSticks1=0;

		CTime tt;
		tt=CTime::GetCurrentTime();  // 30s显现一次
		TRACE("\n (CRF_AlgManager_M::roundPro1) %d:%d:%d ", tt.GetHour(), tt.GetMinute(),  tt.GetSecond() );
	}

	//---------------------------------------

	if (m_nSticks1%10==0)
	{
		BuiltRegionVSS();
	}

		
	if (m_nSticks1%3==0)
	{
		GenerateTagInfo();
	}

// 	if (m_nSticks1%5==0)
// 	{
// 		GenerateTag_AntInfo();
// 	}

	Sleep(100);
}

void RF_CAlgManager::BuiltRegionVSS()
{
	RF_CRegionBase* pRegion;
	for (int n=0; n<RF_REGION_MAX_NUM; n++)
	{
		if ( 0 == m_pRegionList[n].nRoom )
		{
			continue;
		}

		pRegion =  m_pRegionList[n].pRegion;
		if ( NULL == pRegion )
		{
			TRACE("\n CRF_AlgManager_M::BuiltRegionVSSM id = %d, NULL == pRegion", m_pRegionList[n].nRoom);
			continue;
		}
		pRegion->BuildVSS();
	}
}

void RF_CAlgManager::GenerateTagInfo(bool bOptRefTag /* =false */, bool bOptUnAliveTag /* =false */ )
{
	int nTagID_Num=0;
	long pTagID_List[TAG_MAX_ROOM];

	memset(pTagID_List, 0, sizeof(long)*TAG_MAX_ROOM);
	nTagID_Num=(int)( m_pDev->Get_TagIDList(pTagID_List) );


	//临时存储算完的卡，打包回调回去
	RF_INFO_TAG pTagList_Out[TAG_MAX_ROOM];
	int nTagCount_Out = 0;
	memset(pTagList_Out, 0, sizeof(RF_INFO_TAG)*TAG_MAX_ROOM);

	long lTagId;
	int nAlgRet;
	for (int n=0; n<nTagID_Num; n++)  //遍历卡， 对每个卡进行单独的计算
	{
		lTagId = pTagID_List[n];
		if ( 0 == lTagId ) continue;

		RF_INFO_TAG* pCur = &pTagList_Out[nTagCount_Out];
		nAlgRet = GenerateSingleTagInfo(lTagId, bOptRefTag, bOptUnAliveTag, *pCur);
		if ( 0 != nAlgRet )
		{
			continue;
		}

		nTagCount_Out++;

		if (m_cbSingleTagInfo)
		{
			(*m_cbSingleTagInfo)(*pCur, m_hUserHandle);
		}

	}

	//全卡回调
 	if ( nTagCount_Out > 0 && m_cbAllTagInfo)
	{
		m_cbAllTagInfo(pTagList_Out, nTagCount_Out, m_hUserHandle);
	}

}

int RF_CAlgManager::GenerateSingleTagInfo(long lTagId, bool bOptRefTag, bool bOptUnAliveTag, RF_INFO_TAG& stOutInfo)
 {
	if ( 0 == lTagId ) return -1;

	int nStatus = 0;

	RF_STagInfo stTag;
	RF_SDataUnit stDataUnit;  //内部数据单元
	memset(&stTag, 0, sizeof(RF_STagInfo));
	memset(&stDataUnit, 0, sizeof(RF_SDataUnit));

	bool bSuc;
	do 
	{
		nStatus--;  // 获取当前id的相关数据
		UINT uiTagRoom = Get_TagRoom(lTagId);
		if ( 0 == uiTagRoom ) break; // 不会出现

		/*-----  procedure1   -----*/

		nStatus--;  //读取当前卡最强的接收器信号
		stTag.TagID = lTagId;
		if ( 1 != m_pDev->ReadTagForStrongestRssis(stTag.TagID, 1, &(stTag.SSR) ) )
		{
			SetTagUnAlive(uiTagRoom);		
		}
		else
		{


			if ( false == m_pDev->ReadCompletelyData(stTag.SSR, stTag.TagID, &stDataUnit) )
			{
				break;
			}

			stTag.SSRssi=stDataUnit.fRelRssi;
			stTag.Power=stDataUnit.tyPower;
			stTag.DataTime=stDataUnit.lTime;

			CRF_TagManager_M::SaveSSRex(
				uiTagRoom,
				stTag.SSR,
				stTag.SSRssi,
				stTag.Power,
				stTag.DataTime);
		}

		/*-----  procedure2   -----*/

// 		if ( !bOptRefTag && IsTagRef(stTag.TagID) ) //是否计算参考卡
// 		{
// 			continue;
// 		}
// 		if ( !bOptUnAliveTag && !IsTagAlive(uiTagRoom) ) //是否计算非活跃的卡
// 		{
// 			continue;
// 		}

		/*-----  procedure3   -----*/

		nStatus--; 
		int nAntID = m_pDev->ReadAntId(stTag.TagID);
		floatP stCurPonit = {0};
		if ( nAntID > 0 && 0 == GetAntPoint(nAntID, &stCurPonit) )  // 检查激发天线是否有注册
		{
			//采用天线坐标

			RF_STagInfo stAntTagInfo;
			memset(&stAntTagInfo, 0, sizeof(RF_STagInfo));

			if ( 0 != CRF_TagManager_M::Get_TagInfo(stTag.TagID, &stAntTagInfo) ) // 获取该卡的相关信息
			{
				break;
			}


			//线性矫正
			floatP stRelPoint = {-1,-1};
			stRelPoint = GetNextPoint(stAntTagInfo.AlgWorldPoint, stCurPonit);

			//存储
			CRF_TagManager_M::SaveRRex(uiTagRoom, 0, nAntID, stRelPoint);

			//输出数据
			memset(&stOutInfo, 0, sizeof(stOutInfo));
			stOutInfo.lTagID = stTag.TagID;

			stOutInfo.bAlive = stAntTagInfo.Alive;
			stOutInfo.nPower = stAntTagInfo.Power;
			stOutInfo.lTime  = stAntTagInfo.AlgTime;

			stOutInfo.uiLocateReader = stAntTagInfo.SSR;
			stOutInfo.fLocateRssi = stAntTagInfo.SSRssi;

			stOutInfo.nLocateRegion = 0;
			stOutInfo.nLocateAnt = nAntID;

			stOutInfo.fWorld_X = stRelPoint.x;
			stOutInfo.fWorld_Y = stRelPoint.y;
		}
		else
		{
			nStatus--; 

			/*-----  procedure4   -----*/
			//没有激发起激发，启用区域计算

			//寻找最近区域
			int nRR = GetRecentRegion(stTag.TagID);
			if ( nRR <= 0 ) break;

			//TRACE("\n TagID = %x, GetRecentRegion = %d", t_Tag.TagID, nRR);

			//接入算法
			RF_CRegionBase* pRegion = m_pRegionList[nRR-1].pRegion;
			if ( NULL == pRegion )
			{
				TRACE("\n CRF_AlgManager_M::GenerateTagInfo(), error NULL == pRegion");
				break;
			}

			floatP stAlgPoint;
			if ( 0 != pRegion->dB_to_XY(stTag.TagID, &stAlgPoint) )
			{
				break;
			}
			//TRACE("\n TagID = %x, curP.x = %.2f, curP.y = %.2f", t_Tag.TagID, curP.x, curP.y);



			RF_STagInfo tagInfo;
			memset(&tagInfo, 0, sizeof(RF_STagInfo));

			if ( 0 != CRF_TagManager_M::Get_TagInfo(stTag.TagID, &tagInfo) )
			{
				TRACE("\n error: 0 != CRF_TagManager_M::Get_TagInfo(t_Tag.TagID, &tagInfo)");
				break;
			}


			//线性矫正
			floatP stRelPoint = {-1,-1};
			stRelPoint = GetNextPoint(tagInfo.AlgWorldPoint, stAlgPoint);

			//存储
			CRF_TagManager_M::SaveRRex(uiTagRoom, nRR, 0, stRelPoint);

			//输出数据
			memset(&stOutInfo, 0, sizeof(stOutInfo));
			stOutInfo.lTagID = stTag.TagID;

			stOutInfo.bAlive = tagInfo.Alive;
			stOutInfo.nPower = tagInfo.Power;
			stOutInfo.lTime  = tagInfo.AlgTime;

			stOutInfo.uiLocateReader = tagInfo.SSR;
			stOutInfo.fLocateRssi = tagInfo.SSRssi;

			stOutInfo.nLocateRegion = nRR;
			stOutInfo.nLocateAnt = 0;

			stOutInfo.fWorld_X = stRelPoint.x;
			stOutInfo.fWorld_Y = stRelPoint.y;

		}

		return 0;
	}while(false);

	return nStatus;
}

//////////////////////////////////////////////////////////////////////////

void RF_CAlgManager::roundPro2()
{
	m_nSticks2++;
	if (m_nSticks2<0 || m_nSticks2>300)
	{
		m_nSticks2=0;

		CTime tt;
		tt=CTime::GetCurrentTime();  // 30s显现一次
		TRACE("\n (TRfPostPro) %d:%d:%d ", tt.GetHour(), tt.GetMinute(),  tt.GetSecond() );
	}

	//---------------------------------------



// 	if (m_nSticks2%5==0)  //指定时间间隔 将数据发送出去
// 	{
// 		if (m_tagCallBack)
// 		{
// 
// 		}
// 	}

	Sleep(100);
}

int RF_CAlgManager::GetTag_AllRegionRssiList(long nTagID, RegionTagRssiListM* pRegionTagRssiList_Out)
{
	//TRACE("\n CRF_AlgManager_M::GetRegionTagRssiList");
	int nRetCount = 0;

	int nRegionPoint = 0;
	RF_CRegionBase* pRegion = NULL;
	for (int n=0; n<RF_REGION_MAX_NUM; n++)
	{
		if ( 0 == m_pRegionList[n].nRoom )
		{
			continue;
		}

		pRegion = m_pRegionList[n].pRegion;
		if ( NULL == pRegion )
		{
			TRACE("\n CRF_AlgManager_M::GetRegionTagRssiList m_pRegionList[n]  n = %,  NULL == pRegion", n);
			continue;
		}

		//检测区域设备是否正常
		if ( 0 != pRegion->CheckRegionDevice() )
		{
			//TRACE("\n GetTag_AllRegionRssiList region index = %d, CheckRegionDevice fail ", n+1);
			continue;
		}

		//匹配接收器数量
		switch(m_pRegionList[n].style)
		{
		case RF_REGION_STYLE_RECT:
			nRegionPoint = 4;
			break;
		case RF_REGION_STYLE_LINE:
			nRegionPoint = 2;
			break;
		case RF_REGION_STYLE_POINT:
			nRegionPoint = 1;
			break;
		case RF_REGION_STYLE_RECT_MIRROR:
			nRegionPoint = 2;
			break;
		default:
			break;
		}

		if ( 0 == nRegionPoint )
		{
			TRACE("\n CRF_AlgManager_M::GetRegionTagRssiList m_pRegionList[n]  n = %,  0 == nRegionPoint", n);
			continue;
		}

		if ( nRegionPoint != pRegion->GetRssiList(nTagID, pRegionTagRssiList_Out[nRetCount].Rssi) )
		{
			//TRACE("\n CRF_AlgManager_M::GetRegionTagRssiList m_pRegionList[%d],  nRegionPoint != pRegion->GetRssiList", n);
			continue;
		}
		pRegionTagRssiList_Out[nRetCount].roomIndex = n+1;
		pRegionTagRssiList_Out[nRetCount].rssiNum = nRegionPoint;

		nRetCount++;
	}
	return nRetCount;
}

int RF_CAlgManager::GetRecentRegion(long nTagID)
{
	if (nTagID==0)
	{
		return 0;
	}

	RegionTagRssiListM pList[RF_REGION_MAX_NUM];
	memset(pList, 0, sizeof(RegionTagRssiListM)*RF_REGION_MAX_NUM);
	int Num=GetTag_AllRegionRssiList(nTagID, pList);
	if (Num <= 0)
	{
		return 0;
	}

	//打印
// 	for (int n=0; n<Num; n++)
// 	{
// 		TRACE("\n roomIndex = %d, rssiNum = %d", 
// 			pList[n].roomIndex, pList[n].rssiNum);
// 		TRACE("\n");
// 		for (int k=0; k<pList[n].rssiNum; k++)
// 		{
// 			TRACE("\t %.2f", pList[n].Rssi[k]);
// 		}
// 	}

	//排序
	float* pRssi;
	int rssiNum;

	int curRR = 0; // > 0
	long temp = 100*100*10000;
	long speData;
	for (int n=0; n<Num; n++)
	{
		pRssi = pList[n].Rssi;
		rssiNum = pList[n].rssiNum;
		speData = 0;

		make_sort(pRssi, rssiNum, 0);

// 		for (int k=0; k<rssiNum; k++)
// 		{
// 			TRACE("\t %.2f", pRssi[k]);
// 		}

		switch(rssiNum)
		{
		case 1: speData = (long)( pRssi[0] * 1000000); break;
		case 2: speData = (long)( pRssi[0] * 1000000 + pRssi[1] * 10000 ); break;
		case 4: speData = (long)( pRssi[0] * 1000000 + pRssi[1] * 10000 + pRssi[2] * 100 + pRssi[3] ); break;
		default:
			TRACE("\n CRF_AlgManager_M::GetRecentRegion error switch(rssiNum) default");
			speData = 0; break;
		}

		//比较
		if ( speData <= 0 )
		{
			//TRACE("\n CRF_AlgManager_M::GetRecentRegion error  speData <= 0");
			continue;
		}

		if ( speData < temp )
		{
			temp = speData;
			curRR = pList[n].roomIndex;
		}

	}

	return curRR;
}

// bool CRF_AlgManager_M::GetTagXY(long nTag, int nRegionID, PVOID pRssiData, PVOID pReaderData, floatP* RegionPoint, floatP* WorldPoint)
// {
// //	CRF_RssiDatBase_M* pData=(CRF_RssiDatBase_M*)pRssiData;
// 
// 	int nRoom=CheckRegionID(nRegionID);
// 	if (nRoom<=0)
// 	{
// 		return FALSE;
// 	}
// 
// 	nRoom--;
// 	if (m_pRfRegionList[nRoom].Style==1)
// 	{
// 		CRF_RectRegionBase_M* pRectRegion=(CRF_RectRegionBase_M*)(m_pRfRegionList[nRoom].Info);
// 
// 		(*RegionPoint)=pRectRegion->dB_to_XY2(nTag, pRssiData, pReaderData, CUR_ALG_MODE, 1);
// 		(*WorldPoint)=pRectRegion->CorrectXY(*RegionPoint);
// 
// 	}
// 
// 	return TRUE;
// }

// bool CRF_AlgManager_M::dB_to_XY_IrregularShape(PVOID pRssiData, long TagID, CPoint* AlgPoint)
// {
// 	int nSelPoint=6;
// 	int nSelReaderN=3;
// 
// 	floatP* pRefPoint=new floatP[nSelPoint];
// 	float* pRefRssi=new float[nSelPoint*nSelReaderN];
// 	long* pRefTagID=new long[nSelPoint];
// 	UINT* pReaderID=new UINT[nSelReaderN];
// 	if ( pRefPoint==NULL || pRefRssi==NULL || pRefTagID==NULL || pReaderID==NULL )    //只管内部不管外部
// 	{
// 		delete[] pRefPoint;
// 		delete[] pRefRssi;
// 		delete[] pRefTagID;
// 		delete[] pReaderID;
// 		return FALSE;
// 	}
// 
// 	pRefTagID[0]=0x80010233;
// 	pRefPoint[0].x=0;                          // 暂时手动设定
// 	pRefPoint[0].y=0;
// 
// 	pRefTagID[1]=0x80010234;
// 	pRefPoint[1].x=15;
// 	pRefPoint[1].y=0;
// 
// 	pRefTagID[2]=0x80010235;
// 	pRefPoint[2].x=15;
// 	pRefPoint[2].y=30;
// 
// 	pRefTagID[3]=0x80010236;
// 	pRefPoint[3].x= 0;
// 	pRefPoint[3].y=30;
// 
// 	pRefTagID[4]=0x80010216;
// 	pRefPoint[4].x=15;
// 	pRefPoint[4].y=15;
// 
// 	pRefTagID[5]=0x80010217;
// 	pRefPoint[5].x= 0;
// 	pRefPoint[5].y=15;
// 
// 	CRF_ReaderManager_M* p= g_pRF_Device;
// 	int nCount=p->ReadTag_StrongestRssis(TagID, 3, pReaderID);
// 	if (nCount==0)
// 	{
// 		delete[] pRefPoint;
// 		delete[] pRefRssi;
// 		delete[] pRefTagID;
// 		delete[] pReaderID;
// 		return FALSE;
// 	}
// 
// 	for (int j=0; j<nSelReaderN; j++)
// 		for(int i=0; i<nSelPoint; i++)
// 		{
// 			pRefRssi[j*nSelPoint+i]=p->ReadRssi(pReaderID[j], pRefTagID[i]);
// 		}
// 
// 		float fTagRssi[3];
// 		fTagRssi[0]=p->ReadRssi(pReaderID[0], TagID);
// 		fTagRssi[1]=p->ReadRssi(pReaderID[1], TagID);
// 		fTagRssi[2]=p->ReadRssi(pReaderID[2], TagID);
// 		floatP RelP=dB_to_XY_IrregularShape_opt( fTagRssi, nSelPoint, pRefRssi, pRefPoint );
// 
// 		CPoint RelPoint;
// 		RelPoint.x=(long)(RelP.x*100);
// 		RelPoint.y=(long)(RelP.y*100);
// 		*AlgPoint=RelPoint;
// 
// 		delete[] pRefPoint;
// 		delete[] pRefRssi;
// 		delete[] pRefTagID;
// 		delete[] pReaderID;
// 		return TRUE;
// 
// }


int RF_CAlgManager::GetNewReigonRoom()
{
	if ( NULL == m_pRegionList )
	{
		return 0;
	}

	for (int n=0; n<RF_REGION_MAX_NUM; n++)
	{
		if ( 0 == m_pRegionList[n].nRoom )
		{
			return n+1;
		}
	}

	return 0;
}

int RF_CAlgManager::GetOldReigonRoom(int oldId)
{
	if ( NULL == m_pRegionList || 0 == oldId )
	{
		return 0;
	}

	for (int n=0; n<RF_REGION_MAX_NUM; n++)
	{
		if ( oldId == m_pRegionList[n].nRoom )
		{
			return n+1;
		}
	}

	return 0;
}

int RF_CAlgManager::CreateAnt(int antId, RF_Ant125Attr_s attr)
{
	TRACE("\n CRF_AlgManager_M::CreateAnt");

	if ( m_AlgThreadRunStatus || m_PostThreadRunStatus )
	{
		TRACE("\n CRF_AlgManager_M::CreateAnt m_AlgThreadRunStatus || m_PostThreadRunStatus ");
		return -1;
	}

	return m_antArray.Save(antId, attr);
}

int RF_CAlgManager::GetAntPoint(int antId, floatP* pData_out)
{
	RF_Ant125Attr_s data;
	memset(&data, 0, sizeof(RF_Ant125Attr_s));
	if ( 0 != m_antArray.Read(antId, &data) )
	{
		return -1;
	}

	(*pData_out) = data.antPonit;

	return 0;
}
#include "stdafx.h"
#include "RfServece.h"   


#pragma comment(lib, "RopeokTrans_Server.lib")

/////////////////////////////////////RfService/////////////////////////////////////////

bool g_bIsUserMfcMsgBox = true;

int RfService::s_nCreateCount = 0;

RfService* RfService::CreateService()
{
	if ( 0 != s_nCreateCount )
	{
		return NULL;
	}

	if ( 0 != apiRF_Init() )
	{
		return NULL;
	}

	RfService* p =new RfService;
	if (p)
	{
		s_nCreateCount++;
	}

	return p;
}

void RfService::DestoryService(RfService* pRfService)
{
	if (pRfService)
	{
		delete pRfService;

		s_nCreateCount--;
		if ( s_nCreateCount <= 0 )
		{
			apiRF_UnInit();
		}
	}
}

RfService::RfService()
	: m_bIsRfStart(false)
	, m_pTagInfoCallBack(NULL)
	, m_hTagInfoCallBackUser(NULL)
{
	//DB_LOG_F("RfService::RfService()");

	m_pTrans = new RfTransport;
	m_pFilter = new RfExportTagFilter;
	m_pDbWriter = new RfDataBaseWriter;

	// 一个空类class A{};的大小为什么是1，因为如果不是1，当定义这个类的对象数组时候A objects[5]; objects[0]和objects[1]就在同一个地址处，就无法区分。
// 	int nSize = sizeof(RfDataBaseWriter);  // 有虚拟析构 大小为4，没有则大小为1
// 	nSize = sizeof(int);
// 	nSize = 0;
}

RfService::~RfService()
{
	//DB_LOG_F("RfService::RfService()");

	StopCheck();

	delete m_pTrans;
	delete m_pFilter;
	delete m_pDbWriter;
}

int RfService::StartCheck(DataBaseReader* pDataReader)
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	int nStatus = 0;
	do 
	{
// 		nStatus--;  // 检查连接状态
// 		if ( DataBaseLogin::LOGIN_STATUS_CONECTED != pData->GetDataBaseLoginPtr()->LoginStatus() ) break; // 不需要检查连接状态

		nStatus--;   //检查数据
		VEC_PREADER_T& vecReaders = pDataReader->m_vec_Readers;
		int nReaderNum = vecReaders.size();
		if ( nReaderNum <= 0 ) break;

		VEC_PREGION_T& vecRegions = pDataReader->m_vec_Regions;
		int nRegionNum = vecRegions.size();
		if ( nRegionNum <= 0 ) break;

		nStatus--;  // 添加参数
		int nReaderAddSucCount = 0;
		int nNomalReaderCount = 0;
		RfReader_RW* pReader;
		for (int n=0; n<nReaderNum; n++)
		{
			pReader = vecReaders[n];
			const RF_INFO_READER_IN& stInfo = pReader->Info();

			if ( false == pReader->IsNomalReader() ) break;

			nNomalReaderCount++;
			if ( true == apiRF_AddReader(stInfo) )
			{
				nReaderAddSucCount++;
			}
			else
			{
				LOGW_F("apiRF_AddReader() fail, id = %d, ip = %s, port = %d",
					stInfo.ID, stInfo.IP, stInfo.Port);
			}
		}
		if ( 0 != nNomalReaderCount && nReaderAddSucCount != nNomalReaderCount ) break; 

		nStatus--;
		int nRegionAddSucCount = 0;
		for (int n=0; n<nRegionNum; n++)
		{
			const RfRegion_RW::SData& stInfo = (vecRegions[n]->Info());
			int nIndex;
			switch(stInfo.nRectType)
			{
			case 0:
				{
					nIndex = apiRF_CreateRectRegion(&stInfo.stRect);
					LOG_F("apiRF_CreateRectRegion(), n = %d, nIndex = %d", n, nIndex);

					if ( nIndex > 0 )
						nRegionAddSucCount++;
				}break;
			case 1:
				{
					nIndex = apiRF_CreateMirrorRectRegion(&stInfo.stRect);
					LOG_F("apiRF_CreateMirrorRectRegion(), n = %d, nIndex = %d", n, nIndex);

					if ( nIndex > 0 )
						nRegionAddSucCount++;
				}break;
			case 2:
				{
					nIndex = apiRF_CreateLineRegion(stInfo.stLine);
					LOG_F("apiRF_CreateLineRegion(), n = %d, nIndex = %d", n, nIndex);

					if ( nIndex > 0 )
						nRegionAddSucCount++;
				}break;
			case 3:
				{
					const RF_INFO_REGION_POINT_IN* pPoint = &stInfo.stPoint;
					nIndex = apiRF_CreatePointRegion(pPoint->uiReaderID, pPoint->x, pPoint->y);
					LOG_F("apiRF_CreatePointRegion(), n = %d, nIndex = %d", n, nIndex);

					if ( nIndex > 0 )
						nRegionAddSucCount++;
				}break;
			case 4:
				{
					const RF_INFO_REGION_POINT_IN* pAnt = &stInfo.stPoint;
					nIndex = apiRF_CreateAntPoint(pAnt->uiReaderID, pAnt->x, pAnt->y);
					LOG_F("apiRF_CreateAntPoint(), n = %d, nIndex = %d", n, nIndex);

					if ( 0 != nIndex )
						nRegionAddSucCount++;
				}break;
			default: break;
			}
		}
		if  ( nRegionAddSucCount != nRegionNum ) break; 		
		
		nStatus--; //启动检测线程
		//apiRF_TStartup();

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

int RfService::StartCheck(RfDevVarsInFile_RW* pDataReader)
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	RfDevVarsInFile_RW::SData* pData = NULL;

	int nStatus = 0;
	do 
	{
		nStatus--;   //检查数据
		pData = pDataReader->GetDataPtr();
		if ( NULL == pData || pData->nReaderNum <= 0 )
		{
			break;
		}

		nStatus--;  // 添加接收器信息

		RF_INFO_READER_IN* pReaderInfo;

		int nIndex;
		for (nIndex=0; nIndex<pData->nReaderNum; nIndex++)
		{
			pReaderInfo = &pData->pReaderList[nIndex];

			if ( false == apiRF_AddReader(*pReaderInfo) )
			{
				LOGW_F("apiRF_AddReader() fail, id = %d, ip = %s, port = %d",
					pReaderInfo->ID, pReaderInfo->IP, pReaderInfo->Port);
				break;
			}
		}
		if ( nIndex != pData->nReaderNum ) break; 

		nStatus--;  // 添加区域信息

		if ( false == pData->bIsUseReaderPoint ) break;  // 没有任何区域信息

		UINT nRegionId;
		for (nIndex=0; nIndex<pData->nReaderNum; nIndex++)
		{
			pReaderInfo = &pData->pReaderList[nIndex];
			nRegionId = apiRF_CreatePointRegion(pReaderInfo->ID, pReaderInfo->x, pReaderInfo->y);
			
			if ( nRegionId > 0 )
			{
				LOG_F("apiRF_CreatePointRegion(), nReaderId = %d, nRegionId = %d",  pReaderInfo->ID, nRegionId);
			}
			else
			{
				LOGW_F("apiRF_CreatePointRegion() fail, nReaderId = %d", pReaderInfo->ID);
				break;
			}
		}
		if ( nIndex != pData->nReaderNum ) break; 


// 		nStatus--;  // 添加125k点
// 		if ( 1 == pData->bIsAutoAdd125kPoint )
// 		{
// 			for (nIndex=0; nIndex<pData->nReaderNum; nIndex++)
// 			{
// 				pReaderInfo = &pData->pReaderList[nIndex];
// 				nRegionId = apiRF_CreatePointRegion(pReaderInfo->ID, pReaderInfo->x, pReaderInfo->y);
// 
// 				if ( nRegionId > 0 )
// 				{
// 					LOG_F("apiRF_CreatePointRegion(), nReaderId = %d, nRegionId = %d",  pReaderInfo->ID, nRegionId);
// 				}
// 				else
// 				{
// 					LOGW_F("apiRF_CreatePointRegion() fail, nReaderId = %d", pReaderInfo->ID);
// 					break;
// 				}
// 			}
// 			if ( nIndex != pData->nReaderNum ) break; 
// 		}


		nStatus--; //启动检测线程
		apiSetTagCallBack(m_pTagInfoCallBack);
		apiSetUserHandle(m_hTagInfoCallBackUser);
		apiRF_TStartup();

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

int RfService::StartTestCheck()
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	int nStatus = 0;
	do 
	{
		nStatus--;
		RF_INFO_READER_IN stInfo;

		stInfo.ID = 1;
		sprintf(stInfo.IP, "192.168.8.191");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 2;
		sprintf(stInfo.IP, "192.168.8.192");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 3;
		sprintf(stInfo.IP, "192.168.8.193");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 4;
		sprintf(stInfo.IP, "192.168.8.194");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		nStatus--;
		int nIndex;
		RF_INFO_REGION_RECT_IN stRect;
		memset(&stRect, 0, sizeof(RF_INFO_REGION_RECT_IN));

		stRect.left = 10; stRect.right = 40; stRect.top = 10; stRect.bottom = 40;
		stRect.ReaID[0] = 1; stRect.TagID[0] = 0x83012251;
		stRect.ReaID[1] = 2; stRect.TagID[1] = 0x83012252;
		stRect.ReaID[2] = 3; stRect.TagID[2] = 0x83012253;
		stRect.ReaID[3] = 4; stRect.TagID[3] = 0x83012254;
		nIndex = apiRF_CreateRectRegion(&stRect);
		LOG_F("apiRF_CreateRectRegion(),  nIndex = %d",  nIndex);
		if ( nIndex <= 0 ) break;

		nStatus--; //启动检测线程
		apiRF_TStartup();

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

int RfService::StartTestCheck2()
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	int nStatus = 0;
	do 
	{
		nStatus--;
		RF_INFO_READER_IN stInfo;

		stInfo.ID = 1;
		sprintf(stInfo.IP, "192.168.8.191");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 4;
		sprintf(stInfo.IP, "192.168.8.194");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 2;
		sprintf(stInfo.IP, "192.168.8.192");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

// 		stInfo.ID = 3;
// 		sprintf(stInfo.IP, "192.168.8.193");
// 		stInfo.Port = 5000;
// 		stInfo.x = 0;
// 		stInfo.y = 0;
// 		if ( false == apiRF_AddReader(stInfo) ) break;

		nStatus--;
		if (false)
		{
			int nIndex;
			RF_INFO_REGION_RECT_IN stRect;
			memset(&stRect, 0, sizeof(RF_INFO_REGION_RECT_IN));

			stRect.left = 10; stRect.right = 40; stRect.top = 10; stRect.bottom = 40;
			stRect.ReaID[0] = 1; //stRect.TagID[0] = 0x83012251;
			stRect.ReaID[1] = 2; //stRect.TagID[1] = 0x83012252;
			stRect.ReaID[2] = 3; //stRect.TagID[2] = 0x83012253;
			stRect.ReaID[3] = 4; //stRect.TagID[3] = 0x83012254;
			nIndex = apiRF_CreateRectRegion(&stRect);
			LOG_F("apiRF_CreateRectRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;
		}
		else if (false)
		{
			int nIndex;
			RF_INFO_REGION_LINE_IN stLine;
			memset(&stLine, 0, sizeof(RF_INFO_REGION_LINE_IN));

			stLine.P1x = 40;
			stLine.P1y = 10;

			stLine.P2x = 40;
			stLine.P2y = 40;

			stLine.P1_ReaderID = 2;
			stLine.P2_ReaderID = 3;

			nIndex = apiRF_CreateLineRegion(stLine);
			LOG_F("apiRF_CreateLineRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;
		}
		else
		{
			int nIndex;

			nIndex = apiRF_CreatePointRegion(1, 10, 10);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

			nIndex = apiRF_CreatePointRegion(2, 30, 20);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

			nIndex = apiRF_CreatePointRegion(4, 110, 110);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

// 			nIndex = apiRF_CreatePointRegion(3, 130, 120);
// 			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
// 			if ( nIndex <= 0 ) break;

		}
		
		apiSetTagCallBack(m_pTagInfoCallBack);
		apiSetUserHandle(m_hTagInfoCallBackUser);

		//nStatus--;
		//if ( NULL == m_pTrans || 0 != m_pTrans->StarTrans() ) break;  // 启动网络传输

		nStatus--; //启动检测线程
		apiRF_TStartup();

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

int RfService::StartTestCheck3()
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	int nStatus = 0;
	do 
	{
		nStatus--;
		RF_INFO_READER_IN stInfo;

		stInfo.ID = 5;
		sprintf(stInfo.IP, "192.168.8.195");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 11;
		sprintf(stInfo.IP, "192.168.8.196");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		stInfo.ID = 21;
		sprintf(stInfo.IP, "192.168.8.197");
		stInfo.Port = 5000;
		stInfo.x = 0;
		stInfo.y = 0;
		if ( false == apiRF_AddReader(stInfo) ) break;

		nStatus--;
		{
			int nIndex;

			// 点区域

			nIndex = apiRF_CreatePointRegion(5, 50, 50);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

			nIndex = apiRF_CreatePointRegion(11, 100, 100);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

			nIndex = apiRF_CreatePointRegion(21, 200, 200);
			LOG_F("apiRF_CreatePointRegion(),  nIndex = %d",  nIndex);
			if ( nIndex <= 0 ) break;

			// 125K 点区域

			nIndex = apiRF_CreateAntPoint(11, 110, 110);
			LOG_F("apiRF_CreateAntPoint(),  nIndex = %d",  nIndex);
			if ( nIndex != 0 ) break;

			nIndex = apiRF_CreateAntPoint(12, 120, 120);
			LOG_F("apiRF_CreateAntPoint(),  nIndex = %d",  nIndex);
			if ( nIndex != 0 ) break;

			nIndex = apiRF_CreateAntPoint(13, 130, 130);
			LOG_F("apiRF_CreateAntPoint(),  nIndex =%d",  nIndex);
			if ( nIndex != 0 ) break;

			nIndex = apiRF_CreateAntPoint(21, 210, 210);
			LOG_F("apiRF_CreateAntPoint(),  nIndex = %d",  nIndex);
			if ( nIndex != 0 ) break;
		}

		apiSetTagCallBack(m_pTagInfoCallBack);
		apiSetUserHandle(m_hTagInfoCallBackUser);

		//nStatus--;
		//if ( NULL == m_pTrans || 0 != m_pTrans->StarTrans() ) break;  // 启动网络传输

		nStatus--; //启动检测线程
		apiRF_TStartup();

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

int RfService::StartCheck2(DataBaseReader* pDataBaseReader)
{
	if ( true == m_bIsRfStart )
	{
		return -1;
	}

	int nStatus = 0;
	do 
	{
		nStatus--;   // 添加接收器数据
		{
			DataBaseReader::READER_MAP_T& cMap = pDataBaseReader->m_cReaderMap;
			DataBaseReader::READER_MAP_T::Iter cIter(cMap);
			RF_INFO_READER_IN stInfo;
			int nKey;
			bool bSuc;
			while( cIter.Next(nKey, stInfo) )
			{
				bSuc = apiRF_AddReader(stInfo);
				LOG_F("apiRF_AddReader() bSuc = %d, id = %d, ip = %s, port = %d",
					bSuc, stInfo.ID, stInfo.IP, stInfo.Port);
			}

		}
		
		nStatus--;  // 添加点区域数据
		{
			DataBaseReader::REGION_POINT_MAP_T& cMap = pDataBaseReader->m_cRegionPointMap;
			DataBaseReader::REGION_POINT_MAP_T::Iter cIter(cMap);
			RF_INFO_REGION_POINT_IN stInfo;
			int nKey;
			bool bSuc;
			int nIndex;
			while( cIter.Next(nKey, stInfo) )
			{
				if ( 0 != stInfo.uiReaderID )
				{
					nIndex = apiRF_CreatePointRegion(stInfo.uiReaderID, stInfo.x, stInfo.y);
					LOG_F("apiRF_CreatePointRegion(), nIndex = %d, uiReaderID = %d, x = %.2f, y = %.2f", 
						nIndex, stInfo.uiReaderID, stInfo.x, stInfo.y);
				}
				else
				{
					nIndex = apiRF_CreateAntPoint((int)stInfo.uiTriggerID, stInfo.x, stInfo.y);
					LOG_F("apiRF_CreateAntPoint(), nIndex = %d, uiTriggerID = %d, x = %.2f, y = %.2f", 
						nIndex, stInfo.uiTriggerID, stInfo.x, stInfo.y);

				}
			}

		}

		nStatus--; //启动检测线程
		{
			apiRF_TStartup();
		}
		

		m_bIsRfStart = true;
		return 0;
	} while (false);

	StopCheck();
	return nStatus;
}

void RfService::StopCheck()
{
// 	//关闭网络传输
// 	if (m_pTrans)
// 	{
// 		m_pTrans->StopTrans();
// 	}

	//关闭检测线程
	apiRF_TClose(); 

	//清除内部所有数据
	apiRF_Clear();

	//重置标志位
	m_bIsRfStart = false;
}

void RfService::SetTagInfoCallBack(RF_CALLBACK_TAG pTagInfoCallBack, DWORD hTagInfoCallBackUser)
{
	m_pTagInfoCallBack = pTagInfoCallBack;
	m_hTagInfoCallBackUser = hTagInfoCallBackUser;
}

void RfService::TestXmlInfo()
{
	int nTagNum = 30;
	RF_INFO_TAG* pTagList = new RF_INFO_TAG[nTagNum];
	memset(pTagList, 0, sizeof(RF_INFO_TAG)*nTagNum);
	RF_INFO_TAG* pTag;
	for(int n=0; n<nTagNum; n++)
	{
		pTag = &pTagList[n];

		pTag->lTagID = 0x90010300 + n + 1;

		pTag->nPower = 3;
		pTag->lTime = time(NULL);
		pTag->uiLocateReader = 0;

		pTag->fLocateRssi = 99;

		pTag->fWorld_X = 1.123 * (n+1);
		pTag->fWorld_Y = 2.321 * (n+1);

	}

	int nRet = m_pTrans->InsertTagInfoToSendQueueByXml(pTagList, nTagNum);
	LOG_F("nRet = %d", nRet);

	delete[] pTagList;

	/////////////////////////////////////

	nTagNum = 10;
	pTagList = new RF_INFO_TAG[nTagNum];
	(pTagList, 0, sizeof(RF_INFO_TAG)*nTagNum);
	for(int n=0; n<nTagNum; n++)
	{
		pTag = &pTagList[n];

		pTag->lTagID = 0x90010300 + n + 1;

		pTag->nPower = 3;
		pTag->lTime = time(NULL);
		pTag->uiLocateReader = 0;

		pTag->fLocateRssi = 99;

		pTag->fWorld_X = 1.123 * (n+1);
		pTag->fWorld_Y = 2.321 * (n+1);

	}

	nRet = m_pTrans->InsertTagInfoToSendQueueByXml(pTagList, nTagNum);
	LOG_F("nRet = %d", nRet);

	delete[] pTagList;

	/////////////////////////////////////////////////////////////

	int nReaNum = 30;
	RF_INFO_READER* pReaderList = new RF_INFO_READER[nReaNum];
	memset(pReaderList, 0, sizeof(RF_INFO_READER)*nReaNum);
	RF_INFO_READER* pReader;
	for(int n=0; n<nReaNum; n++)
	{
		pReader = &pReaderList[n];

		pReader->ReaderID = n+1;
		sprintf(pReader->Ip, "192.168.8.%d", n+191);

		pReader->IsNomal = true;
	}

	nRet = m_pTrans->InsertReaInfoToSendQueueByXml(pReaderList, nReaNum);
	LOG_F("nRet = %d", nRet);

	delete[] pReaderList;

}

int RfService::StartTransService()
{
	ASSERT(m_pTrans);
	if ( NULL == m_pTrans )
	{
		return -1;
	}

	if ( 0 != m_pTrans->Init(TRANS_TCP, "127.0.0.1", 8111) )
	{
		return -2;
	}

	if ( 0 != m_pTrans->StarTrans(20, 1024*10) )
	{
		StopTransService();
		return -3;
	}

	return 0;
}

void RfService::StopTransService()
{
	ASSERT(m_pTrans);
	if ( NULL == m_pTrans )
	{
		return;
	}

	m_pTrans->StopTrans();

	m_pTrans->UnInit();

}

/////////////////////////////////////RfXmlHanlder/////////////////////////////////////////

RfXmlHanlder::RfXmlHanlder(RfTransport* pTrans)
	: m_pSentQue(NULL)
{
	m_pSentQue = &pTrans->m_SendingQ;
}

RfXmlHanlder::~RfXmlHanlder()
{
	;
}

int RfXmlHanlder::InsertTagInfo(RF_INFO_TAG* pTagInfoList, int nTagNum)
{
	int nStatus = 0;

	do
	{
		nStatus--;
		if ( NULL == pTagInfoList || nTagNum <= 0 )
		{
			break;
		}

		nStatus--;
		
		TiXmlDocument* pDoc = NULL;
		TiXmlElement* pRootObj = NULL;
		TiXmlElement* pGroupObj = NULL;

		RF_INFO_TAG* pTagInfo;
		TiXmlElement* pObj = NULL;
		TiXmlPrinter* pXmlData = NULL;
		char sId[10];
		int nInd;
		for (nInd=0; nInd<nTagNum; nInd++)
		{
			if ( NULL == pDoc )
			{
				pDoc = new TiXmlDocument;
				if ( NULL == pDoc )
				{
					break;
				}

				pRootObj = new TiXmlElement("RF-SERVER-ROOT");
				if ( NULL == pRootObj )
				{
					break;
				}
				pDoc->LinkEndChild(pRootObj);

				pGroupObj = new TiXmlElement("READER-GROUP");
				if ( NULL == pGroupObj )
				{
					break;
				}

				pRootObj->LinkEndChild(pGroupObj);

			}

			pTagInfo = &pTagInfoList[nInd];
			pObj = new TiXmlElement("TAG");
			if ( NULL == pObj ) 
			{
				break;
			}

			int nId = pTagInfo->lTagID;
			_snprintf(sId, 10, "%x", pTagInfo->lTagID);
			sId[9] = '\0';
			pObj->SetAttribute("id", sId);

			int nScale = 100;
			int nPosX = (int)(pTagInfo->fWorld_X * 100);
			int nPosY = (int)(pTagInfo->fWorld_Y * 100);
			pObj->SetAttribute("scale", nScale);
			pObj->SetAttribute("pos_x", nPosX);
			pObj->SetAttribute("pos_y", nPosY);

			int nPower = pTagInfo->nPower;
			int nReader = (int)pTagInfo->uiLocateReader;
			int nTime = (int)pTagInfo->lTime;
			pObj->SetAttribute("power",  nPower);
			pObj->SetAttribute("reader", nReader);
			pObj->SetAttribute("time",   nTime);

			pGroupObj->LinkEndChild(pObj);

			if ( 0 == (nInd+1)%MAX_OBJ_NUM || (nInd+1) == nTagNum )
			{
				pXmlData = new TiXmlPrinter;
				if ( NULL == pXmlData || false == pDoc->Accept(pXmlData) )
				{
					break;
				}
				
#ifdef MY_RFSERVER_DEBUG
// 				int nSize = pXmlData->Size();
// 				char* pStr = (char*)pXmlData->CStr();
// 				int nCurP = 0;
// 				char chOld;
// 
// 				LOG_F("RfXmlHanlder::InsertTagInfo(), size = %d, xml = \n", nSize);
// 				while( nCurP < nSize)
// 				{
// 					if ( nCurP + 500 < nSize )
// 					{
// 						chOld = pStr[500];
// 						pStr[500] = '\0';
// 					}
// 					
// 
// 					TRACE("%s", pStr);
// 
// 					if ( nCurP + 500 < nSize )
// 					{
// 						pStr[500] = chOld;
// 
// 						nCurP+=500;
// 						pStr += 500;
// 					}
// 					else
// 					{
// 						break;
// 					}
// 				
// 				}
// 
// 				TRACE("\n");

#endif

				//数据插入队列
				m_pSentQue->InsertNode((char*)pXmlData->CStr(), pXmlData->Size());

				// 释放资源
				delete pXmlData;
				delete pDoc;

				pXmlData = NULL;
				pDoc = NULL;
			}

		}
		if ( NULL != pDoc ) 
			delete pDoc;
		if ( NULL != pXmlData )
			delete pXmlData;
		if ( nInd != nTagNum )
		{
			break;
		}


		nStatus = 0;
	}while(false);

	return nStatus;
}

int RfXmlHanlder::InsertReaInfo(RF_INFO_READER* pReaderInfoList, int nReaderNum)
{
	int nStatus = 0;

	do
	{
		nStatus--;
		if ( NULL == pReaderInfoList || nReaderNum <= 0 )
		{
			break;
		}

		nStatus--;

		TiXmlDocument* pDoc = NULL;
		TiXmlElement* pRootObj = NULL;
		TiXmlElement* pGroupObj = NULL;

		RF_INFO_READER* pReaderInfo;
		TiXmlElement* pObj = NULL;
		TiXmlPrinter* pXmlData = NULL;

		int nInd;
		for (nInd=0; nInd<nReaderNum; nInd++)
		{
			if ( NULL == pDoc )
			{
				pDoc = new TiXmlDocument;
				if ( NULL == pDoc )
				{
					break;
				}

				pRootObj = new TiXmlElement("RF-SERVER-ROOT");
				if ( NULL == pRootObj )
				{
					break;
				}
				pDoc->LinkEndChild(pRootObj);

				pGroupObj = new TiXmlElement("READER-GROUP");
				if ( NULL == pGroupObj )
				{
					break;
				}

				pRootObj->LinkEndChild(pGroupObj);
			}

			pReaderInfo = &pReaderInfoList[nInd];
			pObj = new TiXmlElement("READER");
			if ( NULL == pObj ) 
			{
				break;
			}

			int nId = pReaderInfo->ReaderID;
			pObj->SetAttribute("id", nId);

			pObj->SetAttribute("ip", pReaderInfo->Ip);

			int bIsNomal = (int)pReaderInfo->IsNomal;
			pObj->SetAttribute("status",  bIsNomal);

			pGroupObj->LinkEndChild(pObj);

			if ( 0 == (nInd+1)%MAX_OBJ_NUM || (nInd+1) == nReaderNum )
			{
				pXmlData = new TiXmlPrinter;
				if ( NULL == pXmlData || false == pDoc->Accept(pXmlData) )
				{
					break;
				}

#ifdef MY_RFSERVER_DEBUG

// 				int nSize = pXmlData->Size();
// 				char* pStr = (char*)pXmlData->CStr();
// 				int nCurP = 0;
// 				char chOld;
// 
// 				LOG_F("RfXmlHanlder::InsertTagInfo(), size = %d, xml = \n", nSize);
// 				while( nCurP < nSize)
// 				{
// 					if ( nCurP + 500 < nSize )
// 					{
// 						chOld = pStr[500];
// 						pStr[500] = '\0';
// 					}
// 
// 
// 					TRACE("%s", pStr);
// 
// 					if ( nCurP + 500 < nSize )
// 					{
// 						pStr[500] = chOld;
// 
// 						nCurP+=500;
// 						pStr += 500;
// 					}
// 					else
// 					{
// 						break;
// 					}
// 
// 				}
// 
// 				TRACE("\n");

#endif

				//数据插入队列
				m_pSentQue->InsertNode((char*)pXmlData->CStr(), pXmlData->Size());

				// 释放资源
				delete pXmlData;
				delete pDoc;

				pXmlData = NULL;
				pDoc = NULL;
			}

		}
		if ( NULL != pDoc ) 
			delete pDoc;
		if ( NULL != pXmlData )
			delete pXmlData;
		if ( nInd != nReaderNum )
		{
			break;
		}


		nStatus = 0;
	}while(false);

	return nStatus;
}

/////////////////////////////////////RfTransport/////////////////////////////////////////

const int DEFAULT_DUR_SEND = 40; // 40ms
const int DEFAULT_FAILCOUNT = 30; // 连续发送DEFAULT_FAILCOUNT次数据失败，则认为当前的连接时无效的。

RfTransport::RfTransport()
	: m_pRpkTrans(NULL)
	, m_nDurSend(DEFAULT_DUR_SEND)
	, m_pTransBuf(NULL)
	, m_cXmlHanlder(this)
	//, m_bInitialize(false)
{
	this->SetThreadName("RfTransport");

	m_pTransBuf = new char[1024*10];
	//LOG_F("RfTransport::RfTransport()");
}

RfTransport::~RfTransport()
{
	StopTrans();
	UnInit();
	delete[] m_pTransBuf;
}

int RfTransport::Init(TRANS_METHOD transMethod, char* pszIPOwner, unsigned short nPortOwner)
{
	int nRet = 0;
	HANDLE hThd = 0;
	if ( NULL != m_pRpkTrans )
	{
		return 0;;
	}

	int nStatus = 0;
	CRopeokServer* pServer = NULL;
	do
	{
		nStatus--;
		pServer = new CRopeokServer;
		if ( NULL == pServer ) break;

		nStatus--;
		nRet = pServer->Initialize(transMethod, pszIPOwner, nPortOwner, 0, 0, CALLBACK_ENABLE);
		if ( RPKSUCCESS != nRet ) break;
	
		pServer->SetReceCallBack((ReceiveProc)ServerReceiveProc, (void*)this);
		pServer->SetAcceptCallBack(NULL, NULL);
		pServer->SetCheckPacketType(NULL, NULL);
		// 启动控制信息传输服务. add by lutf@20120504

		m_pRpkTrans = pServer;
		return 0;
	}while(false);

	delete pServer;

	return nStatus;	
}

void RfTransport::UnInit()
{
	if (m_pRpkTrans)
	{
		CRopeokServer* pServer = (CRopeokServer*)m_pRpkTrans;
		pServer->DoRelease();

		delete pServer;
		m_pRpkTrans = NULL;
	}
		
}

int RfTransport::StarTrans(int nQLen,int nQNodeSize)
{
	int nStatus = 0;

	int nRet;
	do
	{
		nStatus--;
		if ( nQLen <= 0 || nQNodeSize <= 0 ) break;

		nStatus--;
		m_SendingQ.Release();
		nRet = m_SendingQ.Initialize(nQLen, nQNodeSize);
		if ( 0 != nRet ) break;

		nStatus--;
		m_SendingQEmergent.Release();
		nRet = m_SendingQEmergent.Initialize(nQLen, nQNodeSize);
		if ( 0 != nRet ) break;

		if ( false == X::CRoundThread::TStartup() ) break;

		return 0;
	}while(false);

	return nStatus;
}

void RfTransport::StopTrans()
{
	X::CRoundThread::TClose();

	m_SendingQ.Release();
	m_SendingQEmergent.Release();

}

void RfTransport::DoRoundPro()
{
	int nRet;
	int nTransLen;
	std::set<SOCKET>::iterator itB, itE;
	std::vector<SOCKET> vRmSocket;
	std::vector<SOCKET>::iterator itBv, itEv;

	// Normal Data
	nRet = m_SendingQ.TakeNode(m_pTransBuf, &nTransLen);
	if ( nRet == 0 && m_pRpkTrans )
	{
		// Success
		itB = m_setConSockets.begin();
		itE = m_setConSockets.end();
		for ( ; itB != itE; itB++)
		{
			nRet = m_pRpkTrans->DoTransData((*itB), m_pTransBuf, nTransLen);
			if (nRet != 0)
			{// Record the incorrect sockets.					
				if (m_mConSktFailCount.find((*itB)) == m_mConSktFailCount.end())
					m_mConSktFailCount.insert(std::pair<SOCKET, int>((*itB), 0));
				m_mConSktFailCount[(*itB)]++;
				if (m_mConSktFailCount[(*itB)] >= DEFAULT_FAILCOUNT)
				{
					vRmSocket.push_back((*itB));
				}
			}
			else
			{
				if (m_mConSktFailCount.find((*itB)) == m_mConSktFailCount.end())
					m_mConSktFailCount.insert(std::pair<SOCKET, int>((*itB), 0));
				m_mConSktFailCount[(*itB)] = 0; // recount from 0
			}
			// DBGWrite(LOG_LEVEL_WARNING, "SendingProcess | DoTransData | nRet %d\n", nRet);
		}
		// Remove the incorrect sockets.
		if (vRmSocket.size() > 0)
		{
			itBv = vRmSocket.begin();
			itEv = vRmSocket.end();
			for ( ; itBv != itEv; itBv++)
			{
				if (m_pRpkTrans != NULL)
					m_pRpkTrans->CloseTheSocket((*itBv));
				m_setConSockets.erase((*itBv));
				m_mConSktFailCount.erase((*itBv));
				//	DBGWrite(LOG_LEVEL_NORMAL, "Do erase socket %d\n", (*itBv));
			}
			vRmSocket.clear();
		}// end if
	}

	// For emergent data. The emergent node's size is the same as normal node's size, 
	// so here, we direct use pTransBuf.
	nRet = m_SendingQEmergent.TakeNode(m_pTransBuf, &nTransLen);
	if ( nRet == 0 && m_pRpkTrans )
	{
		// Success
		itB = m_setConSockets.begin();
		itE = m_setConSockets.end();
		for ( ; itB != itE; itB++)
		{
			nRet = m_pRpkTrans->DoTransData((*itB), m_pTransBuf, nTransLen);
			if (nRet == 0)
			{// Record the incorrect sockets.
				if (m_mConSktFailCount.find((*itB)) == m_mConSktFailCount.end())
					m_mConSktFailCount.insert(std::pair<SOCKET, int>((*itB), 0));
				m_mConSktFailCount[(*itB)]++;
				if (m_mConSktFailCount[(*itB)] >= DEFAULT_FAILCOUNT)
				{
					vRmSocket.push_back((*itB));
				}
			}
			else
			{
				if (m_mConSktFailCount.find((*itB)) == m_mConSktFailCount.end())
					m_mConSktFailCount.insert(std::pair<SOCKET, int>((*itB), 0));
				m_mConSktFailCount[(*itB)] = 0; // recount from 0
			}
		}

		// Remove the incorrect sockets.
		if (vRmSocket.size() > 0)
		{
			itBv = vRmSocket.begin();
			itEv = vRmSocket.end();
			for ( ; itBv != itEv; itBv++)
			{
				if (m_pRpkTrans != NULL)
					m_pRpkTrans->CloseTheSocket((*itBv));
				m_setConSockets.erase((*itBv));
				m_mConSktFailCount.erase((*itBv));
			}
			vRmSocket.clear();
		}

	}// end is nRet

	Sleep(m_nDurSend);
}


int RfTransport::InsertTagInfoToSendQueue(RF_INFO_TAG* pTagInfoList, int nTagNum)
{
	int nRet =0;

	if ( pTagInfoList && nTagNum > 0)
	{
		//传输给客户端
		int i, nIndex=0;
		int nSendCount =0;
		time_t tNow = time(NULL);
		STagPack ObjsInfo = {0};
		ObjsInfo.nLen = sizeof(STagPack);
		ObjsInfo.nType = TRANS_REGION_TAGS;
		ObjsInfo.nCheckA = TRANS_CHECK_CODE_A;
		ObjsInfo.nCheckB = TRANS_CHECK_CODE_B;
		ObjsInfo.lMovingTime = tNow;

		nSendCount = nTagNum/MAX_OBJ_NUM;
		RF_INFO_TAG* pTmpTagInfo = pTagInfoList;
		nIndex =0;
		memset(ObjsInfo.Objs, 0, sizeof(ObjsInfo.Objs));
		for (i=0; i< nTagNum; i++)
		{
			// 不激活不传递
			if ( 0 == pTmpTagInfo->bAlive )
			{
				pTmpTagInfo++;
				continue;
			}

			memcpy(&ObjsInfo.Objs[nIndex], pTmpTagInfo, sizeof(RF_INFO_TAG));

			nIndex++;
			if (nIndex >= MAX_OBJ_NUM)
			{
				ObjsInfo.nObjNum = MAX_OBJ_NUM;
				m_SendingQ.InsertNode((char*)&ObjsInfo, ObjsInfo.nLen);
				memset(ObjsInfo.Objs, 0, sizeof(ObjsInfo.Objs));
				nIndex =0;
				Sleep(10);
			}

			pTmpTagInfo++;
		}


		// 发送剩余的
		if (nIndex > 0)
		{
			ObjsInfo.nObjNum = nIndex;
			m_SendingQ.InsertNode((char*)&ObjsInfo, ObjsInfo.nLen);
			Sleep(10);
		}

		nRet =1;
	}
	return nRet;
}


int RfTransport::InsertReaInfoToSendQueue(RF_INFO_READER* pReaderInfoList, int nReaderNum)
{
	int nRet =0;

	if ( pReaderInfoList && nReaderNum > 0)
	{
		//传输给客户端
		int i, nIndex=0;
		int nSendCount =0;
		SReaPack ObjsInfo = {0};
		ObjsInfo.nLen = sizeof(SReaPack);
		ObjsInfo.nType = TRANS_READER_NUMS;
		ObjsInfo.nCheckA = TRANS_CHECK_CODE_A;
		ObjsInfo.nCheckB = TRANS_CHECK_CODE_B;

		nSendCount = nReaderNum/MAX_OBJ_NUM;
		RF_INFO_READER* pTmpReader = pReaderInfoList;
		for (i=0; i<nSendCount; i++)
		{
			memset(ObjsInfo.Objs, 0, sizeof(ObjsInfo.Objs));
			for (nIndex=0;nIndex<MAX_OBJ_NUM;nIndex++)
			{
				ObjsInfo.Objs[nIndex].ReaderID = pTmpReader->ReaderID;
				ObjsInfo.Objs[nIndex].IsNomal = pTmpReader->IsNomal;
				pTmpReader++;
			}

			ObjsInfo.nObjNum = MAX_OBJ_NUM;
			m_SendingQ.InsertNode((char*)&ObjsInfo, ObjsInfo.nLen);
			Sleep(10);
		}	

		// 发送剩余的
		nSendCount = nReaderNum%MAX_OBJ_NUM;
		memset(ObjsInfo.Objs, 0, sizeof(ObjsInfo.Objs));
		for (nIndex=0;nIndex<nSendCount;nIndex++)
		{
			ObjsInfo.Objs[nIndex].ReaderID = pTmpReader->ReaderID;
			ObjsInfo.Objs[nIndex].IsNomal = pTmpReader->IsNomal;
			pTmpReader++;
		}
		ObjsInfo.nObjNum = nSendCount;
		m_SendingQ.InsertNode((char*)&ObjsInfo, ObjsInfo.nLen);

		nRet =1;

	}
	return nRet;
}


int RfTransport::InsertTagInfoToSendQueueByXml(RF_INFO_TAG* pTagInfoList, int nTagNum)
{
	return m_cXmlHanlder.InsertTagInfo(pTagInfoList, nTagNum);
}


int RfTransport::InsertReaInfoToSendQueueByXml(RF_INFO_READER* pReaderInfoList, int nReaderNum)
{
	return m_cXmlHanlder.InsertReaInfo(pReaderInfoList, nReaderNum);
}

void RfTransport::ServerReceiveProc2(SOCKET sockCon, char *pDataBuf, int nLen)
{
	TRANSHEADER stTransHeader = {0};

	if ( nLen < sizeof(TRANSHEADER) )
	{
		LOGW_F("RfTransport::ServerReceiveProc2(), nLen < sizeof(TRANSHEADER)");
	}

	memcpy(&stTransHeader, pDataBuf, sizeof(TRANSHEADER));

	switch (stTransHeader.nType)
	{
	case TRANS_INITIAL_REQUEST:
		{
			//DBGWrite(LOG_LEVEL_ERROR, "Initial Request | sockCon %d\n", sockCon);
			//pServer->MsgProcess_InitialRequest(sockCon, pDataBuf, nLen);
			LOG_F("RfTransport::ServerReceiveProc2(), nType = TRANS_INITIAL_REQUEST");
			m_setConSockets.insert(sockCon);
		}
		break;

	case TRANS_HEART_PACKAGE:
		{// Add by lutf@20120502
// 			HEART_PACKAGE transHeart = {0};
// 			HEART_PACKAGE transHeartRespond = {0};
// 			//
// 			if (nLen < sizeof(transHeart))
// 				return;
// 			memcpy(&transHeart, pDataBuf, sizeof(transHeart));
// 			//DBGWrite(LOG_LEVEL_ERROR, "Initial Request | sockCon %d\n", sockCon);
// 			// Respond Heart Package.
// 			transHeartRespond.nType = TRANS_HEART_PACKAGE;
// 			transHeartRespond.nCheckA =TRANS_CHECK_CODE_A;
// 			transHeartRespond.nCheckB = TRANS_CHECK_CODE_B;
// 			transHeartRespond.nVSourceID = transHeart.nVSourceID;			
// 			//	transHeartRespond.nErrFlg = (pServer->GetDetectStatus(transHeart.nVSourceID) == RKSDK_DETECTSTATUS_NONE)? TRANS_EFAULT : TRANS_SUCCESS;
// 			transHeartRespond.nLen = sizeof(transHeartRespond);
// 			// The mapping of device id and request sockets. Add by lutf@20120524
// 			pServer->InsertSocket2Device(transHeart.nVSourceID, sockCon);
// 			//
// 			int nRet = pServer->m_RopeokServer.DoTransData(sockCon, (char*)(&transHeartRespond), transHeartRespond.nLen);			
// 			if (nRet != RPKSUCCESS)
// 			{// Record the incorrect sockets.
// 				if (pServer->UpdateFailCount(sockCon, 1))				
// 				{ // Really close the socket here.
// 					pServer->RmTransFailCount(sockCon);
// 					pServer->m_RopeokServer.CloseTheSocket(sockCon);					
// 				}				
// 			}
// 			else
// 			{// recount from 0
// 				pServer->UpdateFailCount(sockCon, 0);
// 			}
		}
		break;
	default:
		LOGW_F("RfTransport::ServerReceiveProc2(), unknown type, nType = %d", stTransHeader.nType);
		break;
	}

}


////////////////////////////////////RfTagGoodsToPeopleBindList//////////////////////////////////////

int RfTagGoodsToPeopleBindList::Updata()
{
	return LoadVirtualData();
}

int RfTagGoodsToPeopleBindList::LoadVirtualData()
{
	m_cMap.Clear();

	BOOL_T bAddSuc;
	SBindInfo stBindInfo;
	SBindInfo stDummy;
	do 
	{
		stBindInfo.lTagId     = 0x83012257;
		stBindInfo.lBindTagId = 0x82013563;
		
		if( !m_cMap.Find(stBindInfo.lTagId) )
		{
			bAddSuc = m_cMap.Insert(stBindInfo.lTagId, stBindInfo);
			if ( false == bAddSuc ) break;
		}
		
		stBindInfo.lTagId     = 0x83012258;
		stBindInfo.lBindTagId = 0x82013564;
		if( !m_cMap.Find(stBindInfo.lTagId) )
		{
			bAddSuc = m_cMap.Insert(stBindInfo.lTagId, stBindInfo);
			if ( false == bAddSuc ) break;
		}

		return 0;
	} while (false);


	return -1;
}

////////////////////////////////////RfTagFilterManage//////////////////////////////////////


RfExportTagFilter::RfExportTagFilter()
{
	;
}

RfExportTagFilter::~RfExportTagFilter()
{
	;
}

void RfExportTagFilter::Filter(RF_INFO_TAG* tagList, int tagListNum, ExportTagInfoMap_T& cDataMap)
{
	RF_INFO_TAG* pTag;
	RF_INFO_TAG* pOldTag;

	RfTagGoodsToPeopleBindList::BingTagMap* pBindListMap = m_cBindList.GetList();
	RfTagGoodsToPeopleBindList::SBindInfo stBindInfo;
	RF_INFO_TAG* pBindTag;

	m_cBindList.Updata();

	for (int n=0; n<tagListNum; n++)
	{
		pTag = new RF_INFO_TAG;
		if ( NULL == pTag ) 
		{
			LOGW_F(" RfExportTagFilter::Filter(), NULL == pTag");
			break;
		}
		memcpy(pTag, &tagList[n], sizeof(RF_INFO_TAG));

		if ( true == pTag->bAlive &&
			 TRUE == pBindListMap->Find(pTag->lTagID, &stBindInfo) )  // 该id在过滤器中已注册，需处理
		{
			if ( TRUE == cDataMap.Find(stBindInfo.lBindTagId, &pBindTag) &&
				 true == pBindTag->bAlive )
			{
				LOG_F("RfExportTagFilter::Filter() trigger, tag1 = %x, tag2 = %x", pTag->lTagID, pBindTag->lTime);
				long lDifTime = pTag->lTime - pBindTag->lTime;
				if ( lDifTime <= 3 || lDifTime >= -3 )  // 比较时间相似度
				{
					if ( 0 != pTag->nLocateRegion )
					{
						tagList[n].fWorld_X = pTag->fWorld_X = pBindTag->fWorld_X;
						tagList[n].fWorld_Y = pTag->fWorld_Y = pBindTag->fWorld_Y;
					}
				}
			}
		}

		if ( TRUE == cDataMap.Find(pTag->lTagID, &pOldTag) )
		{
			delete pOldTag;
			if ( false == cDataMap.Updata(pTag->lTagID, pTag) )
			{
				LOGW_F(" RfExportTagFilter::Filter, false == cDataMap.Updata()");
				delete pTag;
				break;
			}
		}
		else
		{
			if ( false == cDataMap.Insert(pTag->lTagID, pTag) )
			{
				LOGW_F(" RfExportTagFilter::Filter, false == cDataMap.Insert()");
				delete pTag;
				break;
			}
		}
		
	}


}


RfDataBaseWriter::RfDataBaseWriter()
{
	;
}

RfDataBaseWriter::~RfDataBaseWriter()
{
	;
}

void RfDataBaseWriter::UpdataTagInfoToDb(RF_INFO_TAG* tagList, int tagListNum, ExportTagInfoMap_T& cDataMap)
{
	RF_INFO_TAG* pTag;

	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB2;
	SACommand saCmd;
	SAString strSql;

	do 
	{
		nStatus--;
		if ( !pCon->IsAlive() )
		{
			break;  // 检查连接状态
		}

		if ( tagListNum <= 0 )
		{
			break;
		}

		nStatus--;
		for (int n=0; n<tagListNum; n++)
		{
			;
		}

		return;
	} while (false);

}

void RfDataBaseWriter::UpdataReaderInfoToDb(RF_INFO_READER* readerList, int readerListNum, ExportReaderInfoMap_T& cDataMap)
{
	;
}
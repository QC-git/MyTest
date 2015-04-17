#include "stdafx.h"


#include "RF_Device.h"
//#include "DBWindow.h"
#include <list>
/************************************************************************/
/*                             CRF_DataPool                             */
/*                               Public                                 */
/************************************************************************/

RF_CDataPool::RF_CDataPool()
{
	TRACE("\n CRF_DataPool_M ");
	m_bPoolInit=false;   

	m_pDataBase_List=new RF_SDataUnit[REA_ROOM_NUM*TAG_ROOM_NUM];  //开辟 接收器 x 射频卡卡数量的 数据房间
	m_pReaID_List=new UINT[REA_ROOM_NUM];  // 接收器ID 列表
	m_pTagID_List=new long[TAG_ROOM_NUM];  // 射频卡ID 列表     通过该表可从上 获取到到 某接收器的单个射频卡的数据

	m_pTagAntInfoList = new RF_DataBase_TagEx_M[TAG_ROOM_NUM];

	if ( m_pDataBase_List && m_pReaID_List && m_pTagID_List && m_pTagAntInfoList )  //清零
	{
		memset(m_pDataBase_List, 0, sizeof(RF_SDataUnit)*REA_ROOM_NUM*TAG_ROOM_NUM);
		memset(m_pReaID_List, 0, sizeof(UINT)*REA_ROOM_NUM);
		memset(m_pTagID_List, 0, sizeof(long)*TAG_ROOM_NUM);

		memset(m_pTagAntInfoList, 0, sizeof(RF_DataBase_TagEx_M)*TAG_ROOM_NUM);

		m_bPoolInit=true;  //初始化 成功标志
	}

	InitializeCriticalSection(&m_csData);  // 初始化 互斥量，用于线程同步
}

RF_CDataPool::~RF_CDataPool()
{
	TRACE("\n ~CRF_DataPool ");

	m_bPoolInit=false;  //释放资源

	if (m_pDataBase_List)
	{
		delete[] m_pDataBase_List;
	}
	if (m_pReaID_List)
	{
		delete[] m_pReaID_List;
	}
	if (m_pTagID_List)
	{
		delete[] m_pTagID_List;
	}

	if (m_pTagAntInfoList)
	{
		delete[] m_pTagAntInfoList;
	}

	DeleteCriticalSection(&m_csData);
}


float RF_CDataPool::ReadRssi(UINT ReaID, long TagID)
{
	bool bDummy;
	return ReadRssi(ReaID, TagID, bDummy);
}

/*
	func: 根据 接收器id和射频卡id 读取强度值
	ret : Rssi
*/
float RF_CDataPool::ReadRssi(UINT ReaID, long TagID, bool& bIsOutIfTime)
{
	if ( !m_bPoolInit )
	{
		return -1.0f;
	}

	bIsOutIfTime = false;
	RF_SDataUnit stData;
	int nRoom = 0;

	EnterCriticalSection(&m_csData);
	nRoom=GetRoomIndex(ReaID, TagID);  //获取 数据存放地址的索引
	if ( nRoom > 0 )
	{
		stData = m_pDataBase_List[nRoom-1];
	}
	LeaveCriticalSection(&m_csData);

	if ( nRoom > 0 )
	{
		if ( stData.lTime - time(NULL) > 3 ) 
			bIsOutIfTime = true;
		return stData.fRelRssi;
	}

	return -1.0f;
}

/*
	func: 返回所有数据
	ret : if have
*/
bool RF_CDataPool::ReadCompletelyData(UINT ReaID, long TagID, RF_SDataUnit* pData_out)
{
	if ( !m_bPoolInit || NULL == pData_out )
	{
		return FALSE;
	}

	int nRoom = 0;
	EnterCriticalSection(&m_csData);
	nRoom=GetRoomIndex(ReaID, TagID);
	if ( nRoom > 0 )
	{
		*pData_out = m_pDataBase_List[nRoom-1];
	}
	LeaveCriticalSection(&m_csData);
	
	return nRoom > 0 ? true : false;
}

/*
	func: 读取 标签卡 数个最强的强度   强度大小从大到小排列
	ret : 信息数量
*/
UINT RF_CDataPool::ReadTagForStrongestRssis(long TagID, UINT uiInfoNum, UINT* pReaderID_out, float* pRssi_out/* =NULL */)
{
	if ( !m_bPoolInit )
	{
		return 0;
	}

	//--------------------------------------
	if ( 0 == TagID || uiInfoNum <= 0 || NULL == pReaderID_out )
	{
		return 0;
	}

	UINT pReaderIdList[REA_ROOM_NUM];
	float pRssiList[REA_ROOM_NUM];

	memset(pReaderIdList ,0, sizeof(UINT)*REA_ROOM_NUM);
	memset(pRssiList, 0, sizeof(float)*REA_ROOM_NUM);

	int nCount=0;  //记录数量
	long lCurTime = time(NULL);

	int nRoom;
	RF_SDataUnit* pData;
	long lSaveTime;
	EnterCriticalSection(&m_csData);
	for (int n=0; n<REA_ROOM_NUM; n++)  //从 pool 中读取数据 
	{
		if ( 0 == m_pReaID_List[n] )
		{
			break;
		}

// 		if ( 4 == m_pReaID_List[n] && 0x83012259 == TagID )
// 		{
// 			TRACE("");
// 		}

		//----------获得强度---------- 
		nRoom = GetRoomIndex(m_pReaID_List[n], TagID);
		if ( nRoom > 0 )
		{
			pData = &m_pDataBase_List[nRoom-1];

			lSaveTime = pData->lTime; // 判断时间是否超时
			long lDifTime = lCurTime - lSaveTime;
			if ( lDifTime < 0 ) // 时间倒退 更新存储时间
			{
				TRACE("\n RF_CDataPool::ReadTagForStrongestRssis(), lDifTime < 0, TagID = %x, ReaId, = %d", TagID, m_pReaID_List[n]);
				pData->lTime = lCurTime; 
			}
			else if ( lDifTime > 3 ) // 时间超时，当前数据不予考虑
			{
				continue;
			}

			if ( pData->fRelRssi <10 ) // 数值过小不考虑 
			{
				continue;
			}
			
			pReaderIdList[nCount] = m_pReaID_List[n];
			pRssiList[nCount] = pData->fRelRssi;

			nCount++;
		}
	}
	LeaveCriticalSection(&m_csData);
	if ( 0 == nCount )
	{
		return 0;
	}

	/*  排序  */
	UINT tt_ID;
	float tt_Rssi;
	for (int n=0; n<nCount-1; n++)
		for (int k=n+1; k<nCount; k++)
	{
		if ( pRssiList[n]>pRssiList[k] )
		{
			tt_ID=pReaderIdList[n];
			pReaderIdList[n]=pReaderIdList[k];
			pReaderIdList[k]=tt_ID;

			tt_Rssi=pRssiList[n];
			pRssiList[n]=pRssiList[k];
			pRssiList[k]=tt_Rssi;
		}
	}

	/*  输出  */
	int Num = ( nCount <= (int)uiInfoNum ) ? nCount : (int)uiInfoNum;
	for (int n=0; n<Num; n++)
	{
		pReaderID_out[n]=pReaderIdList[n];
		if (pRssi_out) pRssi_out[n]=pRssiList[n];	
	}

	return Num;
}

/*
	func: 获取当前所有 射频id卡 号
	ret : 信息数量
*/
UINT RF_CDataPool::Get_TagIDList(long* lTagID_out)
{
	if ( !m_bPoolInit )
	{
		return 0;
	}

	//--------------------------------------
	int nCount=0;

	EnterCriticalSection(&m_csData);
	for (int n=0; n<TAG_ROOM_NUM; n++)
	{
		if ( 0 == m_pTagID_List[n] )
		{
			break;
		}
		lTagID_out[nCount]=m_pTagID_List[n];
		nCount++;
	}
	LeaveCriticalSection(&m_csData);

	return nCount;

}

/*
	func: 获取当前所有 接收器id卡 号
	ret : 信息数量
*/
UINT RF_CDataPool::Get_ReaIDList(UINT* uiReaID_out)
{
	if ( !m_bPoolInit )
	{
		return 0;
	}

	//--------------------------------------
	int nCount=0;

	EnterCriticalSection(&m_csData);
	for (int n=0; n<REA_ROOM_NUM; n++)
	{
		if ( 0 == m_pReaID_List[n] )
		{
			break;
		}
		uiReaID_out[nCount]=m_pReaID_List[n];
		nCount++;
	}
	LeaveCriticalSection(&m_csData);

	return nCount;
}


int RF_CDataPool::ReadAntId(int nTagId)
{
	RF_DataBase_AntEx_M stInfo;
	bool bIsOutTime;

	if ( 0 == ReadAntData(nTagId, stInfo, bIsOutTime) )
	{
		if (!bIsOutTime)
		{
			return stInfo.nAntID;
		}
	}

	return 0;
}


int RF_CDataPool::ReadAntRssi(int nTagId)
{
	RF_DataBase_AntEx_M stInfo;
	bool bIsOutTime;

	if ( 0 == ReadAntData(nTagId, stInfo, bIsOutTime) )
	{
		return stInfo.nAntRssi;
	}

	return -1;
}


int RF_CDataPool::ReadAntData(int nTagId, RF_DataBase_AntEx_M& stInfo_out, bool& bIsOutTime)
{
	if ( NULL == m_pTagAntInfoList || 0 == nTagId )
	{
		return -1;
	}

	RF_DataBase_TagEx_M* p;
	bIsOutTime = true;
	int n = 0;
	EnterCriticalSection(&m_csData);
	for(n=0; n<TAG_ROOM_NUM; n++)
	{
		p = &m_pTagAntInfoList[n];
		if ( nTagId == p->nTagId )
		{
			stInfo_out = p->stAntInfo;

			long lDifTime = time(NULL) - p->stAntInfo.nAntTime;
			if ( lDifTime >= 0 && lDifTime <= 3 )  //时间偏差在3秒之内
			{
				bIsOutTime = false;
			}
			break;
		}
	}
	LeaveCriticalSection(&m_csData);

	if ( TAG_ROOM_NUM != n ) 
		return 0;
	return -2;
}

/************************************************************************/
/*                           Protected                                  */
/************************************************************************/

bool RF_CDataPool::SaveData(
	UINT uiReaID, 
	long lTagID,
	UCHAR byRssi, 
	UCHAR byPower,
	int nAntID,
	int nAntRssi)
{
	if ( !m_bPoolInit )
	{
		return FALSE;
	}

	bool bSuc = false;

	EnterCriticalSection(&m_csData);
	do
	{
		if (byRssi<10)
		{
			break;
		}

		long nRoom=GetRoomIndex(uiReaID, lTagID, true);
		if ( nRoom <= 0 )
		{
			TRACE("\n SaveData: 0 == nRoom to CLear ");
			Clear();                                     //内存池已满， 清除所有数据
			nRoom=GetRoomIndex(uiReaID, lTagID, true);  

			if ( nRoom <= 0 )
			{
				break;
			}
		}

		nRoom--;
		RF_SDataUnit* pData=&(m_pDataBase_List[nRoom]);
		(pData->nUpdataCount)++;
		if ( pData->nUpdataCount >10000 ) (pData->nUpdataCount) = 0;

// 		if ( 4 == uiReaID && 0x83012259 == lTagID )
// 		{
// 			TRACE("");
// 		}

		pData->lTime= time(NULL);
		pData->tyPower=byPower;

		for (int n=1; n<5; n++)  //存储数据
		{
			pData->Rssi[n-1]=pData->Rssi[n];
		}
		pData->Rssi[4]=byRssi;

		if ( 0 != pData->Rssi[0] )
		{
			float tmp[4];
			tmp[0]=(float)(pData->Rssi[4]);
			tmp[1]=(float)(pData->Rssi[3]);
			tmp[2]=(float)(pData->Rssi[2]);
			tmp[3]=(float)(pData->Rssi[1]);
			pData->fRelRssi=GetMedium(tmp,4);

			/* test */
// 			if ( uiReaID==4 && lTagID==0x80010217 )
// 			{
// 				int tdb=nRoom;
// 			}
			//float tt=pData->relRssi;   //* test
		}

		bSuc = true;
	}while(false);
	LeaveCriticalSection(&m_csData);

	if (bSuc) return TRUE;

	return FALSE;
}

bool RF_CDataPool::SaveAntData(int nTagId, int nAntId, int nAntRssi)
{
	if ( !m_bPoolInit )
	{
		return FALSE;
	}

	bool bSuc = false;

	EnterCriticalSection(&m_csData);
	do
	{
		if ( NULL == m_pTagAntInfoList ||  0 == nTagId )//|| 0 == nAntId )
		{
			break;
		}

		RF_DataBase_TagEx_M* p;
		int nRoom = 0;
		for (int n=0; n<TAG_ROOM_NUM; n++)  //判断是否已有
		{
			p = &m_pTagAntInfoList[n];

			if ( nTagId == p->nTagId )
			{
				nRoom = n+1;
				break;
			}
		}

		if ( 0 == nRoom )  // 没有则添加新的
		{
			for (int n=0; n<TAG_ROOM_NUM; n++)  //判断是否已有
			{
				p = &m_pTagAntInfoList[n];

				if ( 0 == p->nTagId )
				{
					nRoom = n+1;
					break;
				}
			}

			if ( 0 == nRoom )  //找不到 空间  返回
			{
				TRACE("\n CRF_DataPool_M::SaveAntData  0 == nRoom ");
				break;
			}

		}

		p = &m_pTagAntInfoList[--nRoom];
		p->nTagId = nTagId;
		p->stAntInfo.nAntID = nAntId;
		p->stAntInfo.nAntRssi = nAntRssi;
		p->stAntInfo.nAntTime = time(NULL);

		bSuc = true;
	}while(false);
	LeaveCriticalSection(&m_csData);

	if(bSuc) return TRUE;

	return FALSE;
}

/************************************************************************/
/*                           Private                                    */
/************************************************************************/

long RF_CDataPool::GetRoomIndex(UINT uiReaID, long lTagID, bool bIsAutoAdd)
{
	if ( 0 == uiReaID || 0 == lTagID )
	{
		return 0;
	}

	int nRoomNum=0;
	int nReaRoom=0;
	int nTagRoom=0;

	//----------------查看是否已经开辟房间---------------------

	for(int n=0; n<REA_ROOM_NUM; n++)
	{
		if ( m_pReaID_List[n]==uiReaID )
		{
			nReaRoom=n+1;
			break;
		}
	}
	for(int n=0; n<TAG_ROOM_NUM; n++)
	{
		if ( m_pTagID_List[n]==lTagID )
		{
			nTagRoom=n+1;
			break;
		}
	}

	//----------------------开辟新房间-------------------------

	if (bIsAutoAdd)
	{
		if ( nReaRoom==0 )
		{
			for(int n=0; n<REA_ROOM_NUM; n++)
			{
				if ( m_pReaID_List[n]==0 )
				{
					m_pReaID_List[n]=uiReaID;
					nReaRoom=n+1;
					break;
				}
			}
		}

		if ( nTagRoom==0 )
		{
			for(int n=0; n<TAG_ROOM_NUM; n++)
			{
				if ( m_pTagID_List[n]==0 )
				{
					m_pTagID_List[n]=lTagID;
					nTagRoom=n+1;
					break;
				}
			}
		}
	}


	//----------------------换算房间号-------------------------

	if ( nReaRoom>0 && nTagRoom>0 )
	{
		nReaRoom--;
		nTagRoom--;
		return (nReaRoom*TAG_ROOM_NUM+nTagRoom+1);
	}

	return 0;
}

void RF_CDataPool::Clear()
{
	TRACE("\n CRF_DataPool::Remove_AllDat() \n");

	memset(m_pDataBase_List, 0, sizeof(RF_SDataUnit)*REA_ROOM_NUM*TAG_ROOM_NUM);
	memset(m_pReaID_List, 0, sizeof(UINT)*REA_ROOM_NUM);
	memset(m_pTagID_List, 0, sizeof(long)*TAG_ROOM_NUM);

}

bool RF_CDataPool::GetPriData(RF_SDataUnit* pData_out)
{
	return FALSE;
}


/************************************************************************/
/*                           CReaderManager_M                           */
/*                               Public                                 */
/************************************************************************/

RF_CReaderManager* RF_CReaderManager::CreateNew(int nManagerMode)
{
	RF_CReaderManager* p = new RF_CReaderManager(nManagerMode);
	if (p)
	{
		if ( p->IsInitSuc() ) return p;

		delete p;
	}

	return NULL;
}

RF_CReaderManager::RF_CReaderManager(int nManagerMode)
	: READER_MANAGE_MODE(nManagerMode)
	, m_bManagerInit(false)
	, m_bIsThreadStart(false)
	, m_hUserHandle(NULL)
	, m_cbAllReaderInfo(NULL)
	, m_pReaderList(NULL)
	, m_pRfPackList(NULL)
	, m_pRfPackBuff(NULL)
	, m_psentBuff(NULL)
	, m_RssiInfoCallBack(NULL)
	, m_RssiInfoCallBackUser(NULL)
	, m_nNextOpenIndex(0)
	, m_nReaderStatusInfoNum(0)
	, m_nReaderCount(0)
{
	TRACE("\n RF_CReaderManager ");

	//---------------------------------------

	do
	{
		m_pReaderList = new RF_ReaderBase_M[READER_MAX_NUM];
		if ( NULL == m_pReaderList ) break;
		memset(m_pReaderList, 0, sizeof(RF_ReaderBase_M)*READER_MAX_NUM);

		m_pReaderStatusInfo = new RF_INFO_READER[READER_MAX_NUM];
		if ( NULL == m_pReaderStatusInfo ) break;
		memset(m_pReaderStatusInfo, 0, sizeof(RF_INFO_READER)*READER_MAX_NUM);

		m_pRfPackList=new char[1024];
		if ( NULL == m_pRfPackList ) break;

		m_pRfPackBuff=new RfPacketEx[500];
		if ( m_pRfPackBuff==NULL ) break;

		m_psentBuff=new char[ sizeof(RfPacketEx)*500+sizeof(UINT)*2+sizeof(char) ];
		if ( m_psentBuff==NULL ) break;

		m_bManagerInit=true;
	}while(false);

	InitializeCriticalSection(&m_csManager);

}

RF_CReaderManager::~RF_CReaderManager()
{
	TRACE("\n ~RF_CReaderManager \n");
	//------------------关闭线程----------------------

	CloseThread();

	//------------------释放内存----------------------

	m_bManagerInit=false;

	if (m_pReaderList)       delete[] m_pReaderList;

	if (m_pReaderStatusInfo) delete[] m_pReaderStatusInfo;

	if (m_pRfPackList)       delete[] m_pRfPackList;
	
	if (m_pRfPackBuff)       delete[] m_pRfPackBuff;

	if (m_psentBuff)         delete[] m_psentBuff;

	//------------------清除互斥----------------------
	DeleteCriticalSection(&m_csManager);
}

bool RF_CReaderManager::Add_ReaderInfo(UINT uiID, const char* sIP, UINT uiPort, float fPos_X, float fPos_Y)
{
	if ( false != m_bIsThreadStart )  
	{
		return FALSE;
	}

	//--------------------------------

	if ( 0 == uiID || NULL == sIP || strlen(sIP)>16 || 0 == uiPort ) // 检查参数
	{
		return FALSE;
	}

	RF_ReaderBase_M* p;

	/*       查看以前是否有相同ID       */
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		p = &m_pReaderList[n];

		if (  0 == p->ReaderID )
		{
			break;
		}

		if ( uiID == p->ReaderID )
		{
			strcpy(p->IP, sIP);
			p->Port = uiPort;
			p->Location.x = fPos_X;
			p->Location.y = fPos_Y;

			return TRUE;
		}
	}

	/*       增加新的       */
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		p = &m_pReaderList[n];

		if ( 0 == p->ReaderID )
		{
			p->ReaderID = uiID;
			strcpy(p->IP, sIP);
			p->Port = uiPort;
			p->Location.x = fPos_X;
			p->Location.y = fPos_Y;

			m_nReaderCount++;

			return TRUE;
		}
	}

	return FALSE;
}

/*
	func: 获取接收器列表
	ret ：》0 接收器个数    -1，fail
*/
int RF_CReaderManager::Get_ReaderInfo(RF_ReaderBase_M* pData_out)
{
	if ( NULL == pData_out )
	{
		return -1;
	}

	int nCount = 0;
	EnterCriticalSection(&m_csManager);
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		if ( 0 != m_pReaderList[n].ReaderID )
		{		
			pData_out[nCount++]=m_pReaderList[n];
		}
	}
	LeaveCriticalSection(&m_csManager);

	return nCount;
}

bool RF_CReaderManager::IsReaderNomal(UINT uiID)
{
	if ( 0 == uiID )
	{
		return FALSE;
	}

	bool bIsNomal=false;
	RF_ReaderBase_M* pReader;

	EnterCriticalSection(&m_csManager);
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		pReader = &m_pReaderList[n];
		if ( uiID == pReader->ReaderID )
		{
			bIsNomal = ( pReader->IsOpen && pReader->IsHavData );
			break;
		}
	}
	LeaveCriticalSection(&m_csManager);

	return bIsNomal;
}

//////////////////////////////////////////////////////////////////////////

int RF_CReaderManager::StartThread() 
{

	int nIndex=0;
	if ( READER_MANAGE_MODE==READERMANAGER_NORMALMODE )
	{
		AFD_RF_Reader_LinkRecover(1500);

		if ( Startup_TReaderOpen() )
		{
			nIndex+=1;
		}
		if ( Startup_TDataTransmit() )
		{
			nIndex+=2;
		}
	}
	else if ( READER_MANAGE_MODE==READERMANAGER_SERVERMODE )
	{
		if ( Startup_TReaderOpen() )
		{
			nIndex+=1;
		}
		if ( Startup_TDataTransmit() )
		{
			nIndex+=2;
		}
		if ( CTCP_ServerToListen_M::TStartup() )
		{
			nIndex+=3;
		}
	}
	else if ( READER_MANAGE_MODE==READERMANAGER_CLIENTMODE )
	{
		if ( CTCP_Client_M::TStartup() )
		{
			nIndex+=4;
		}
	}
	else if ( READER_MANAGE_MODE==READERMANAGER_SOCKET_TESTMODE )
	{
		if ( Startup_TReaderOpen() )
		{
			nIndex+=1;
		}
		if ( Startup_TDataTransmit() )
		{
			nIndex+=2;
		}
		if ( CTCP_ServerToListen_M::TStartup() )
		{
			nIndex+=3;
		}
		if ( CTCP_Client_M::TStartup() )
		{
			nIndex+=4;
		}
	}
	

	return nIndex;
}

bool RF_CReaderManager::CloseThread()
{
	Close_TDataTransmit();
	Close_TReaderOpen();

	CTCP_ServerToListen_M::TClose();

	CTCP_Client_M::TClose();

	m_bIsThreadStart=false;

	return TRUE;
}

void RF_CReaderManager::ClearAll()
{
	memset(m_pReaderList, 0, sizeof(RF_ReaderBase_M)*READER_MAX_NUM);

	RF_CDataPool::Clear();
}

int RF_CReaderManager::GetReadersInfo(RF_CALLBACK_READER pAfterGetFunc, DWORD hUser)
{
	RF_INFO_READER* pOutList = new RF_INFO_READER[READER_MAX_NUM];
	int nCount = 0;

	if (pOutList)
	{
		memset(pOutList, 0, sizeof(RF_INFO_READER) * READER_MAX_NUM);

		EnterCriticalSection(&m_csManager);
		for (int n=0; n<READER_MAX_NUM; n++)
		{
			if ( 0 != m_pReaderList[n].ReaderID )
			{		
				pOutList[nCount].ReaderID = m_pReaderList[n].ReaderID;
				sprintf(pOutList[nCount].Ip, m_pReaderList[n].IP);
				pOutList[nCount].IsNomal = ( m_pReaderList[n].IsHavData && m_pReaderList[n].IsOpen );

				nCount++;
			}
		}
		LeaveCriticalSection(&m_csManager);

		if ( nCount > 0 )
		{
			(*pAfterGetFunc)(pOutList, nCount, hUser);
		}
		else
		{
			delete[] pOutList;
		}

	}

	return nCount;
}

int RF_CReaderManager::SetRssiInfoCallBack(RF_GET_RSSI_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	EnterCriticalSection(&m_csManager);

	m_RssiInfoCallBack = pAfterGetFunc;
	m_RssiInfoCallBackUser = hUser;

	LeaveCriticalSection(&m_csManager);

	return 0;
}

/************************************************************************/
/*                           Protected                                  */
/************************************************************************/

bool RF_CReaderManager::Startup_TReaderOpen()
{
	return MDoubleThread::TStartup1();
}

void  RF_CReaderManager::Close_TReaderOpen()
{
	MDoubleThread::TClose1();

	AFD_RF_Reader_CloseAll();
	if (m_pReaderList)
	{
		for(int n=0; n<READER_MAX_NUM; n++)
		{
			if ( 0 != m_pReaderList )
			{
				m_pReaderList[n].IsOpen = false;
				m_pReaderList[n].IsHavData = false;
			}
		}
	}

}

bool RF_CReaderManager::Startup_TDataTransmit()
{
	return MDoubleThread::TStartup2();
}

void  RF_CReaderManager::Close_TDataTransmit()
{
	MDoubleThread::TClose2();
}

/************************************************************************/
/*                           Private                                    */
/************************************************************************/

void RF_CReaderManager::roundPro1()
{
// 	m_nSticks1++;
// 	if ( m_nSticks1 < 0 || m_nSticks1 > 100 )
// 	{
// 		m_nSticks1=0;
// 	}
// 	if ( 0 == (m_nSticks1%10) )
// 	{
// 		OpenAllReader();
// 	}	
// 
// 	Sleep(100);

	OpenNextReader();
	Sleep(20);

}

bool RF_CReaderManager::OpenReader(UINT uiID,const char* sIP, UINT uiPort)
{
	TRACE("\n RF_CReaderManager::OpenReader(), start, uiID = %d, ip = %s, port = %d ", 
		uiID, sIP, uiPort);

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);//win32的函数
	long lCostTimeInMs = sysTime.wSecond*1000 + sysTime.wMilliseconds;
	long lCostTimeInS = time(NULL);

	char buff[100];
	_snprintf(buff, 99,  "%d:2:%s:%d", uiID, sIP, uiPort);
	buff[99] = '\0';

	AFD_RF_Reader_Close(uiID);
	bool bSuc = AFD_RF_Reader_Open(buff) == 0 ? false : true;

	GetSystemTime(&sysTime);//win32的函数
	lCostTimeInMs = sysTime.wSecond*1000 + sysTime.wMilliseconds - lCostTimeInMs;
	lCostTimeInS = time(NULL) - lCostTimeInS;

	TRACE("\n RF_CReaderManager::OpenReader(), end  , uiID = %d, ip = %s, port = %d, lCostTimeInMs = %d, lCostTimeInS = %d, bSuc = %d", 
		uiID, sIP, uiPort, lCostTimeInMs, lCostTimeInS, bSuc);

	if (bSuc) return TRUE;
	return FALSE;
}

void RF_CReaderManager::OpenNextReader()
{
	int nReaderIndex = m_nNextOpenIndex;

	if ( 0 == m_nNextOpenIndex )
	{
		m_nReaderStatusInfoNum = 0;
	}

	m_nNextOpenIndex++;
	if ( m_nNextOpenIndex >= READER_MAX_NUM )
	{
		m_nNextOpenIndex = 0;
	}

	RF_ReaderBase_M  stReader;
	RF_ReaderBase_M*  pReader = &stReader;
	bool bIsNomal = false;

	do
	{
		EnterCriticalSection(&m_csManager);
		stReader = m_pReaderList[nReaderIndex];
		LeaveCriticalSection(&m_csManager);

		if ( 0 == pReader->ReaderID ) // 该索引没有装载参数，直接返回
		{
			Sleep(20);
			return;
		}

		if ( true == pReader->IsOpen ) // 该设备已经打开
		{
			// 		pReaderInfo_out = &listReaderInfo_out[nReaderInfo_count++];
			// 		pReaderInfo_out->ReaderID = pReader->ReaderID;
			// 		pReaderInfo_out->IsNomal = ( pReader->IsHavData && pReader->IsOpen);
			// 		strcpy(pReaderInfo_out->Ip, pReader->IP);
			// 
			// 		nHavOpen_Num++;

			bIsNomal =  pReader->IsHavData && pReader->IsOpen;

			break;
		}

		bool bOpenSuc = OpenReader(pReader->ReaderID, pReader->IP, pReader->Port);

		EnterCriticalSection(&m_csManager);
		m_pReaderList[nReaderIndex].gotDataTime = time(NULL);
		m_pReaderList[nReaderIndex].IsOpen = bOpenSuc;
		LeaveCriticalSection(&m_csManager);

		bIsNomal =  pReader->IsHavData && bOpenSuc;

	}while(false);

	
	// 记录信息
	RF_INFO_READER* pStatusInfo = &m_pReaderStatusInfo[m_nReaderStatusInfoNum];
	m_nReaderStatusInfoNum++;

	pStatusInfo->ReaderID = stReader.ReaderID;
	strcpy(pStatusInfo->Ip, stReader.IP);
	pStatusInfo->IsNomal = bIsNomal;

	if ( m_nReaderStatusInfoNum == m_nReaderCount )
	{
		if ( m_cbAllReaderInfo && m_nReaderStatusInfoNum > 0 )
		{
			m_cbAllReaderInfo(m_pReaderStatusInfo, m_nReaderStatusInfoNum, m_hUserHandle);
		}

		m_nReaderStatusInfoNum = 0;
	}

}

int RF_CReaderManager::OpenAllReader()
{

	int nHavOpen_Num=0;

	int nReaderInfo_count = 0;
	RF_INFO_READER* pReaderInfo_out;
	RF_INFO_READER listReaderInfo_out[READER_MAX_NUM];
	memset(&listReaderInfo_out, 0, sizeof(RF_INFO_READER)*READER_MAX_NUM);

	RF_ReaderBase_M stReader;
	RF_ReaderBase_M* pReader = &stReader;

	bool bOpenSta=false;
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		EnterCriticalSection(&m_csManager);
		stReader = m_pReaderList[n];
		LeaveCriticalSection(&m_csManager);

		if ( 0 == pReader->ReaderID )
		{
			continue;
		}

		if ( true == pReader->IsOpen )
		{
			pReaderInfo_out = &listReaderInfo_out[nReaderInfo_count++];
			pReaderInfo_out->ReaderID = pReader->ReaderID;
			pReaderInfo_out->IsNomal = ( pReader->IsHavData && pReader->IsOpen);
			strcpy(pReaderInfo_out->Ip, pReader->IP);

			nHavOpen_Num++;
			continue;
		}

		bOpenSta=OpenReader(pReader->ReaderID, pReader->IP, pReader->Port);  //该函数 会堵塞

		if (bOpenSta)
			nHavOpen_Num++;

		

		EnterCriticalSection(&m_csManager);
		m_pReaderList[n].gotDataTime = time(NULL);
		m_pReaderList[n].IsOpen=bOpenSta;
		LeaveCriticalSection(&m_csManager);

		pReaderInfo_out = &listReaderInfo_out[nReaderInfo_count++];
		pReaderInfo_out->ReaderID = pReader->ReaderID;
		pReaderInfo_out->IsNomal = ( pReader->IsHavData && pReader->IsOpen);
		strcpy(pReaderInfo_out->Ip, pReader->IP);

		Sleep(50);	
	}

	//回调接收器状态信息
	if ( m_cbAllReaderInfo && nReaderInfo_count > 0 )
	{
		m_cbAllReaderInfo(listReaderInfo_out, nReaderInfo_count, m_hUserHandle);
	}

	return nHavOpen_Num;
	
}

void RF_CReaderManager::roundPro2()
{
	m_nSticks2++;
	if (m_nSticks2<0 || m_nSticks2>100)
	{
		m_nSticks2=0;
	}
	if (m_nSticks2%5==0)  // 500ms 读取一次
	{
		DataTransmit();
	}	

	Sleep(100);
}

int RF_CReaderManager::DataTransmit()   //  READERMANAGER_NORMALMODE and READERMANAGER_SERVERMODE
{

	memset(m_pRfPackList, 0, 1024);
	int nListNum = AFD_ReaderComm_GetCommList(m_pRfPackList);
	if ( nListNum < 0 )
	{
		return 0;
	}
	//TRACE("\n AFD_ReaderComm_GetCommList nListNum = %d, m_pRfPackList = %s", nListNum, m_pRfPackList);

	RF_ReaderBase_M tt;
	long t_lTagID;
	UCHAR t_byRssi;
	UCHAR t_byPower;
	int nTagInfo_Num;
	long t_time;

	RFID_Packet_t* t_pPack;
	int AntId;
	int AntRssi;
	int UserInfo[4];

	EnterCriticalSection(&m_csManager);
	for (int n=0; n<READER_MAX_NUM; n++)
	{
		tt=m_pReaderList[n];
		if ( 0 == tt.ReaderID || !(tt.IsOpen) )
		{
			continue;;
		}

		nTagInfo_Num=0;
		nTagInfo_Num=AFD_ReaderComm_GetTags(tt.ReaderID, (char*)m_pRfPackBuff, 500);
		t_time = time(NULL);
		if ( nTagInfo_Num<=0 )
		{
			if ( ( t_time - m_pReaderList[n].gotDataTime ) >= 3 ) // 超时时间
			{
				TRACE("\n ReaderID = %d: out of time ", tt.ReaderID);
				m_pReaderList[n].IsHavData = false;
			}

			continue;
		}
		m_pReaderList[n].gotDataTime = t_time;
		m_pReaderList[n].IsHavData = true;

		//TRACE("\n AFD_ReaderComm_GetTags id = %d, nTagInfo_Num = %d", tt.ReaderID, nTagInfo_Num);

		if ( READER_MANAGE_MODE!=READERMANAGER_SOCKET_TESTMODE )
		{
			for (int n=0; n<nTagInfo_Num; n++)
			{
				t_pPack = &m_pRfPackBuff[n].pk;

				t_lTagID= *( (long*)(t_pPack->Tag_id) );
				t_byRssi=t_pPack->Rssi;
				t_byPower=t_pPack->TagPw;

				AntId = t_pPack->Usr_Info[2]*16 + t_pPack->Usr_Info[3];
				AntRssi = t_pPack->Usr_Info[1];

				/* 存储数据到  数据池 */
				RF_CDataPool::SaveData(
					tt.ReaderID,
					t_lTagID,
					t_byRssi,
					t_byPower,
					AntId,
					AntRssi);

				RF_CDataPool::SaveAntData(t_lTagID, AntId, AntRssi);

				if (m_RssiInfoCallBack)
				{
					RF_INFO_RSSI_OUT stOutData;

					stOutData.lReaderId = (long)tt.ReaderID;
					stOutData.lTagId = (long)t_lTagID;
					stOutData.nUpdataCount = 0;

					stOutData.lTime = t_time;
					stOutData.byPower = t_byPower;
					stOutData.byRssi = t_byRssi;

					stOutData.nAntID = AntId;
					stOutData.nAntRssi = AntRssi;

					(*m_RssiInfoCallBack)(&stOutData, 1, m_RssiInfoCallBackUser);
				}

			}
		}
		
		if ( READER_MANAGE_MODE==READERMANAGER_SERVERMODE || READER_MANAGE_MODE==READERMANAGER_SOCKET_TESTMODE )
		{
			/* socket发送数据 */
			SOCKET sockComm_list[MAX_LINK_NUM];
			
			UINT uiCount=CTCP_ServerToListen_M::Get_AllsockComm(sockComm_list);

// 			int test_resentTicks=10;
// 			int Num=nTagInfo_Num*30;

			memset(m_psentBuff, 0, sizeof(RfPacketEx)*500+sizeof(UINT)*2+sizeof(char) );
			memcpy(m_psentBuff, "!", sizeof(char));
			memcpy(m_psentBuff+sizeof(char), &(tt.ReaderID), sizeof(UINT));
			memcpy(m_psentBuff+sizeof(char)+sizeof(UINT), &(nTagInfo_Num), sizeof(UINT));

			//for (int t=0; t<30; t++)  //压力测试成功
			//{
			//	memcpy(m_psentBuff+sizeof(char)+sizeof(UINT)*2+sizeof(RfPacketEx)*nTagInfo_Num*t, m_pRfPackBuff, sizeof(RfPacketEx)*nTagInfo_Num);
			//}
			memcpy(m_psentBuff+sizeof(char)+sizeof(UINT)*2, m_pRfPackBuff, sizeof(RfPacketEx)*nTagInfo_Num);

			UINT t_sentlen=sizeof(char)+sizeof(UINT)*2+sizeof(RfPacketEx)*nTagInfo_Num;
			int nIndex=0;
			for (int t=0; t<(int)uiCount; t++)
			{
				nIndex=send(sockComm_list[t], m_psentBuff, t_sentlen+1, 0);
				if ( nIndex<=0 )
				{
					TRACE("\n CRF_ReaderManager_M: sent Error ! \n");
				    SOCKADDR_IN sentAddr;
					if ( Get_sockCommAddr(sockComm_list[t], &sentAddr) )
					{
						TRACE("\n CRF_ReaderManager_M: sent Error (%s,%d) ! \n", inet_ntoa(sentAddr.sin_addr), sentAddr.sin_port);
					}
				}
			}
		}
		
	}

	LeaveCriticalSection(&m_csManager);
	Sleep(10);

	return 1L;

}

bool RF_CReaderManager::OnRecieved(
	UINT uiReaID,
	long lTagID,
	UCHAR byRssi,
	UCHAR byPower)
{
	return SaveData(
		uiReaID, 
		lTagID,
		byRssi,
		byPower,
		0,
		0);
}
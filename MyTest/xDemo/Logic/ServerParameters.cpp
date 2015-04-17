#include "stdafx.h"


#include "x_util.h"
#include "RkUtil.h"
#include "ServerParameters.h"

//#include "DataBaseOperater.h"

//extern SAConnection g_cSaConnection;

////////////////////////////////////DataBaseParameter_ReadAndWrite//////////////////////////////////////


DataBase_RW::DataBase_RW()
	: m_pCon1(&g_cDB)
	, m_pCon2(&g_cDB2)
{
	;
}

DataBase_RW::~DataBase_RW()
{
	;
}



////////////////////////////////////RfReader_ReadAndWrite//////////////////////////////////////

RfReader_RW::RfReader_RW()
	: m_bIsReadSuc(false)
	, m_lDataBaseSn(0L)
	, m_bIsNomalReader(true)
{
	;
};

RfReader_RW::~RfReader_RW()
{
	;
}

int RfReader_RW::Read_FromDataBase(SACommand* pSaReader)
{
	//SAConnection* pSa = DataBaseParameter_ReadAndWrite::SaConnection();

	if ( NULL == pSaReader || true == m_bIsReadSuc )
	{
		return -1;
	}

	memset(&m_info, 0, sizeof(RF_INFO_READER_IN));
	RF_INFO_READER_IN* pInfo = &m_info;
	
	pInfo->ID = atoi(pSaReader->Field("Receiver_ID").asString().GetMultiByteChars());

	const char* pIp = pSaReader->Field("Receiver_IP").asString().GetMultiByteChars();
	int nIpLen = strlen(pIp);
	if ( 0 == nIpLen || nIpLen > 15 )
	{
		return -2;
	}
	strcpy(pInfo->IP, pIp);

	pInfo->Port = pSaReader->Field("Receiver_Port").asLong();
	pInfo->x = (float)pSaReader->Field("Pos_x").asDouble();
	pInfo->y = (float)pSaReader->Field("Pos_y").asDouble();
	m_lDataBaseSn = pSaReader->Field("ReceiverInfo_ID").asLong();

	if ( 1 == nIpLen )
	{
		m_bIsNomalReader = false;
	}

	TRACE("\n");
	TRACE("\n sn = %d, id = %d, ip = %s, port = %d", m_lDataBaseSn, pInfo->ID, pInfo->IP, pInfo->Port);
	TRACE("\n          x = %.2f, y = %.2f", pInfo->x, pInfo->y);

	m_bIsReadSuc = true;
	return 0;
}

////////////////////////////////////DataBaseParameter_ReadAndWrite//////////////////////////////////////


RfRegion_RW::RfRegion_RW()
	//: m_nDataStyle(0)
	//, m_lDataBaseSn(0L)
	: m_bIsRead(false)
	, m_bIsReadLink(false)
{
	;
}

RfRegion_RW::~RfRegion_RW()
{
	;
}

int RfRegion_RW::Read_FromDataBase(SACommand* pSaReader)
{
	if ( NULL == pSaReader )
	{
		return -1;
	}

	TRACE("\n Read_FromDataBase() ");

	m_bIsRead = false;
	SData* p = &m_info;
	memset(p, 0, sizeof(SData));

	{ 
		p->Rect_ID = pSaReader->Field("Rect_ID").asLong();
		//p->Map_ID = pSaReader->Field("Map_ID").asLong();
		p->nRectType = pSaReader->Field("Rect_Type").asLong();

		TRACE("\n");
		TRACE("\n Rect_ID = %d, Style = %d", p->Rect_ID, p->nRectType);
		
		switch(p->nRectType)
		{
		case 0:
		case 1:
			{
				RF_INFO_REGION_RECT_IN* p1 = &p->stRect;

				p1->left   = (long)pSaReader->Field("LeftTop_Pos_X").asDouble();
				p1->right  = (long)pSaReader->Field("RightTop_Pos_X").asDouble();

				p1->top    = (long)pSaReader->Field("LeftBottom_Pos_Y").asDouble();
				p1->bottom = (long)pSaReader->Field("LeftTop_Pos_Y").asDouble();

				TRACE("\n Rect   left = %d, right = %d, top = %d, bottom = %d",
					p1->left, p1->right, p1->top, p1->bottom);

			}
			break;
		case 2:
			{
				RF_INFO_REGION_LINE_IN* p2 = &p->stLine;

				p2->P1x = (float)pSaReader->Field("LineHead_Pos_X").asDouble();
				p2->P1y = (float)pSaReader->Field("LineHead_Pos_Y").asDouble();

				p2->P2x = (float)pSaReader->Field("LineTail_Pos_X").asDouble();
				p2->P2y = (float)pSaReader->Field("LineTail_Pos_Y").asDouble();

				TRACE("\n Line   P1x = %.2f, P1y = %.2f, P2x = %.2f, P2y = %.2f",
					p2->P1x, p2->P1y, p2->P2x, p2->P2y);

			}
			break;
		case 3:
		case 4:
			{
				RF_INFO_REGION_POINT_IN* p3 = &p->stPoint;
				
				p3->x = (float)pSaReader->Field("Point_Pos_X").asDouble();
				p3->y = (float)pSaReader->Field("Point_Pos_Y").asDouble();

				TRACE("\n Point   x = %.2f, y = %.2f ", p3->x, p3->y);
			}
			break;
		default:
			return -2;
		}


// 		p->dfLeftTop_Pos_X = pSaReader->Field("LeftTop_Pos_X").asDouble();
// 		p->dfLeftTop_Pos_Y = pSaReader->Field("LeftTop_Pos_Y").asDouble();
// 		p->dfLeftTop_Pos_Z = pSaReader->Field("LeftTop_Pos_Z").asDouble();
// 		p->dfRightTop_Pos_X = pSaReader->Field("RightTop_Pos_X").asDouble();
// 		p->dfRightTop_Pos_Y = pSaReader->Field("RightTop_Pos_Y").asDouble();
// 		p->dfRightTop_Pos_Z = pSaReader->Field("RightTop_Pos_Z").asDouble();
// 		p->dfLeftBottom_Pos_X = pSaReader->Field("LeftBottom_Pos_X").asDouble();
// 		p->dfLeftBottom_Pos_Y = pSaReader->Field("LeftBottom_Pos_Y").asDouble();
// 		p->dfLeftBottom_Pos_Z = pSaReader->Field("LeftBottom_Pos_Z").asDouble();
// 		p->dfRightBottom_Pos_X = pSaReader->Field("RightBottom_Pos_X").asDouble();
// 		p->dfRightBottom_Pos_Y = pSaReader->Field("RightBottom_Pos_Y").asDouble();
// 		p->dfRightBottom_Pos_Z = pSaReader->Field("RightBottom_Pos_Z").asDouble();
// 
// 		p->dfP1_Pos_X = pSaReader->Field("LineHead_Pos_X").asDouble();
// 		p->dfP1_Pos_Y = pSaReader->Field("LineHead_Pos_Y").asDouble();
// 		p->dfP1_Pos_Z = pSaReader->Field("LineHead_Pos_Z").asDouble();
// 		p->dfP2_Pos_X = pSaReader->Field("LineTail_Pos_X").asDouble();
// 		p->dfP2_Pos_Y = pSaReader->Field("LineTail_Pos_Y").asDouble();
// 		p->dfP2_Pos_Z = pSaReader->Field("LineTail_Pos_Z").asDouble();
// 
// 		p->dfP0_Pos_X = pSaReader->Field("Point_Pos_X").asDouble();
// 		p->dfP0_Pos_Y = pSaReader->Field("Point_Pos_Y").asDouble();
// 		p->dfP0_Pos_Z = pSaReader->Field("Point_Pos_Z").asDouble();
// 
// 		TRACE("\n");
// 		TRACE("\n Rect_ID = %d, Map_ID = %d", p->Rect_ID, p->Map_ID);
// 		TRACE("\n Style = %d", p->nRectType);
// 
// 		TRACE("\n Rect   left: x = %.2f, y = %.2f, z = %.2f", p->dfLeftTop_Pos_X, p->dfLeftTop_Pos_Y, p->dfLeftTop_Pos_Z);
// 		TRACE("\n       right: x = %.2f, y = %.2f, z = %.2f", p->dfRightTop_Pos_X, p->dfRightTop_Pos_Y, p->dfRightTop_Pos_Z);
// 		TRACE("\n         top: x = %.2f, y = %.2f, z = %.2f", p->dfLeftBottom_Pos_X, p->dfLeftBottom_Pos_Y, p->dfLeftBottom_Pos_Z);
// 		TRACE("\n      bottom: x = %.2f, y = %.2f, z = %.2f", p->dfRightBottom_Pos_X, p->dfRightBottom_Pos_Y, p->dfRightBottom_Pos_Z);
// 
// 		TRACE("\n Line     p1: x = %.2f, y = %.2f, z = %.2f", p->dfP1_Pos_X, p->dfP1_Pos_Y, p->dfP1_Pos_Z);
// 		TRACE("\n          p2: x = %.2f, y = %.2f, z = %.2f", p->dfP2_Pos_X, p->dfP2_Pos_Y, p->dfP2_Pos_Z);
// 
// 		TRACE("\n Point    p1: x = %.2f, y = %.2f, z = %.2f", p->dfP0_Pos_X, p->dfP0_Pos_Y, p->dfP0_Pos_Z);

		//pSaReader->Close();
		//if ( 0 != ReadLink(p->Rect_ID, p->nRectType, &stData) )
		//{
		//	return -2;
		//}

	}

	m_bIsRead = true;
	return 0;
}


int RfRegion_RW::ReadLink_FromDataBase()
{
	SData* p = &m_info;
	int nRectStyle = p->nRectType;
	int nLinkRectId = p->Rect_ID;

	m_bIsReadLink = false;
	int nRet = -1;

	switch(nRectStyle)
	{
	case 0:
	case 1:
		nRet = ReadLink_Rect(nLinkRectId, &p->stRect);
		break;
	case 2:
		nRet = ReadLink_Line(nLinkRectId, &p->stLine);
		break;
	case 3:
	case 4:
		nRet = ReadLink_Point(nLinkRectId, &p->stPoint);
		break;
	default:
		break;
	}

	if ( 0 == nRet )
	{
		m_bIsReadLink = true;
	}

	return nRet;
}


int RfRegion_RW::ReadLink_Rect(int nLinkRectId, RF_INFO_REGION_RECT_IN* pSink)
{
	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB;
	SACommand saCmd;
	SAString strSql;

	do
	{
		nStatus--;
		if ( !pCon->IsAlive() )
		{
			break;  // 检查连接状态
		}

		nStatus--;
		strSql.Format(
			"select Rect_ID, Receiver_ID, TestTag_ID from RectLink as a"
			" inner join ReceiverInfo as b on a.ReceiverInfo_ID = b.ReceiverInfo_ID"
			" inner join TestTagInfo as c on a.TestTagInfo_ID = c.TestTagInfo_ID"
			" where a.Rect_ID = %d", 
			nLinkRectId);
		if ( false == pCon->Excute(saCmd, strSql) )
		{
			break;
		}

		nStatus--;
		{
			if ( false == saCmd.FetchNext() )   // 第1条
			{
				break;
			}
			pSink->ReaID[0] = saCmd.Field("Receiver_ID").asLong();
			pSink->TagID[0] = strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			if ( false == saCmd.FetchNext() )   // 第2条
			{
				break;
			}
			pSink->ReaID[3] = saCmd.Field("Receiver_ID").asLong();
			pSink->TagID[3] = strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			if ( false == saCmd.FetchNext() )   // 第3条
			{
				break;
			}
			pSink->ReaID[2] = saCmd.Field("Receiver_ID").asLong();
			pSink->TagID[2] = strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			if ( false == saCmd.FetchNext() )   // 第4条
			{
				break;
			}
			pSink->ReaID[1] = saCmd.Field("Receiver_ID").asLong();
			pSink->TagID[1] = strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			TRACE("\n");
			TRACE("\n rect link reader id ");
			TRACE("\n nLinkRegionId = %d ", nLinkRectId);
			TRACE("\n %d:%d:%d:%d ", 
				pSink->ReaID[0], pSink->ReaID[1], pSink->ReaID[2], pSink->ReaID[3]);
			TRACE("\n rect link tag id ");
			TRACE("\n %x:%x:%x:%x ", 
				pSink->TagID[0], pSink->TagID[1], pSink->TagID[2], pSink->TagID[3]);
		}

		saCmd.Close();
		return 0;
	}while(false);

	saCmd.Close();
	return nStatus;
}

int RfRegion_RW::ReadLink_Line(int nLinkRectId, RF_INFO_REGION_LINE_IN* pSink)
{
	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB;
	SACommand saCmd;
	SAString strSql;

	do
	{
		nStatus--;
		if ( !pCon->IsAlive() )
		{
			break;  // 检查连接状态
		}

		nStatus--;
		strSql.Format(
			"select Rect_ID, Receiver_ID, TestTag_ID from RectLink as a"
			" inner join ReceiverInfo as b on a.ReceiverInfo_ID = b.ReceiverInfo_ID"
			" inner join TestTagInfo as c on a.TestTagInfo_ID = c.TestTagInfo_ID"
			" where a.Rect_ID = %d", 
			nLinkRectId);
		if ( false == pCon->Excute(saCmd, strSql) )
		{
			break;
		}

		nStatus--;
		{
			if ( false == saCmd.FetchNext() )   // 第1条
			{
				break;
			}
			pSink->P1_ReaderID = (UINT)saCmd.Field("Receiver_ID").asLong();
			pSink->P1_TagID    = (UINT)strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			if ( false == saCmd.FetchNext() )   // 第2条
			{
				break;
			}
			pSink->P2_ReaderID = (UINT)saCmd.Field("Receiver_ID").asLong();
			pSink->P2_TagID    = (UINT)strtoul(saCmd.Field("TestTag_ID").asString().GetMultiByteChars(), 0, 16);	

			TRACE("\n");
			TRACE("\n line link reader id ");
			TRACE("\n nLinkRegionId = %d ", nLinkRectId);
			TRACE("\n %d:%d ", pSink->P1_ReaderID, pSink->P2_ReaderID);
			TRACE("\n line link tag id ");
			TRACE("\n %x:%x ", pSink->P1_TagID, pSink->P2_TagID);
		}

		saCmd.Close();
		return 0;
	}while(false);

	saCmd.Close();
	return nStatus;
}

int RfRegion_RW::ReadLink_Point(int nLinkRectId, RF_INFO_REGION_POINT_IN* pSink)
{
	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB;
	SACommand saCmd;
	SAString strSql;

	do
	{
		nStatus--;
		if ( !pCon->IsAlive() )
		{
			break;  // 检查连接状态
		}

		nStatus--;
		strSql.Format(
			"select Rect_ID, Receiver_ID, TestTag_ID from RectLink as a"
			" inner join ReceiverInfo as b on a.ReceiverInfo_ID = b.ReceiverInfo_ID"
			" inner join TestTagInfo as c on a.TestTagInfo_ID = c.TestTagInfo_ID"
			" where a.Rect_ID = %d", 
			nLinkRectId);
		if ( false == pCon->Excute(saCmd, strSql) )
		{
			break;
		}

		nStatus--;
		{
			if ( false == saCmd.FetchNext() )   // 第1条
			{
				break;
			}
			pSink->uiReaderID = (UINT)saCmd.Field("Receiver_ID").asLong();

			TRACE("\n");
			TRACE("\n point link reader id ");
			TRACE("\n nLinkRegionId = %d ", nLinkRectId);
			TRACE("\n %d ", pSink->uiReaderID);
		}

		saCmd.Close();
		return 0;
	}while(false);

	saCmd.Close();
	return nStatus;

}


////////////////////////////////////DataBaseInfo_ReadAndWrite//////////////////////////////////////

DataBaseInfo_RW::DataBaseInfo_RW()
	: m_sFilePath(NULL)
	, m_bIsHaveInfo(false)
	, m_pReadInfoCallBack(NULL)
{
	memset(&m_stInfo, 0, sizeof(SInfo));
	memset(&m_stStorageDbInfo, 0, sizeof(SInfo));

	char sFilePath[256]={0};
	RKGUI_GetModuleAppPath(sFilePath);
	sFilePath[255] = '\0';

	if ( strlen(sFilePath) > 1 )
	{
		strcat(sFilePath, SERVICE_PARAMS_FILENAME);
		SetSaveFilePath(sFilePath);
	}

}

DataBaseInfo_RW::~DataBaseInfo_RW()	
{
	Clear();
	delete m_sFilePath;
}

int DataBaseInfo_RW::Read()
{
	int nStatus = 0;

	char sName[32]; 
	char sDbName[32]; 
	char sDbUser[32]; 
	char sDbPwd[32]; 
	char sDbIP[16];
	char sDbPort[6];

	Clear();
	m_bIsHaveInfo = false;
	SInfo* p;

	do
	{
		nStatus--;
		if ( NULL == m_sFilePath ) break;

		nStatus--;  // 保存在缓存中

		DWORD dwRet;
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_NAME,
			"", sName,   32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_NAME,
			"", sDbName, 32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_USER, 
			"", sDbUser, 32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_PASSWORD, 
			"", sDbPwd,  32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_IP, 
			"", sDbIP,   16, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_PORT, 
			"", sDbPort,  6, m_sFilePath);

		p = &m_stInfo;
		p->sName   = X::StrDup_f(sName);
		p->sDbName = X::StrDup_f(sDbName);
		p->sDbUser = X::StrDup_f(sDbUser);
		p->sDbPwd  = X::StrDup_f(sDbPwd);
		p->sDbIP   = X::StrDup_f(sDbIP);
		p->sDbPort = X::StrDup_f(sDbPort);
		if( NULL == p->sName   ||
			NULL == p->sDbName ||
			NULL == p->sDbUser ||
			NULL == p->sDbPwd  ||
			NULL == p->sDbIP   ||
			NULL == p->sDbPort ) break;


		///////////////////////////////////

		nStatus--;  // 保存在缓存中

		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_NAME,
			"", sName,   32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_DB_NAME,
			"", sDbName, 32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_DB_USER, 
			"", sDbUser, 32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_DB_PASSWORD, 
			"", sDbPwd,  32, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_DB_IP, 
			"", sDbIP,   16, m_sFilePath);
		dwRet = GetPrivateProfileString(SERVICE_PARAMS_POSITION_MANAGE_SECTION, SERVICE_PARAMS_DB_PORT, 
			"", sDbPort,  6, m_sFilePath);

		p = &m_stStorageDbInfo;
		p->sName   = X::StrDup_f(sName);
		p->sDbName = X::StrDup_f(sDbName);
		p->sDbUser = X::StrDup_f(sDbUser);
		p->sDbPwd  = X::StrDup_f(sDbPwd);
		p->sDbIP   = X::StrDup_f(sDbIP);
		p->sDbPort = X::StrDup_f(sDbPort);
		if( NULL == p->sName   ||
			NULL == p->sDbName ||
			NULL == p->sDbUser ||
			NULL == p->sDbPwd  ||
			NULL == p->sDbIP   ||
			NULL == p->sDbPort ) break;

		// 		DB_LOG_F("读取数据库配置文件成功");
		// 		p = &m_stInfo;
		// 		DB_LOG_F("数据库1");
		// 		DB_LOG_F("	");

		//回调
		if ( NULL != m_pReadInfoCallBack )
		{
			m_pReadInfoCallBack->AfterGetInfo(m_stInfo, m_stStorageDbInfo);
		}

		m_bIsHaveInfo = true;
		return 0;
	}while(false);

	DB_LOG_F("读取数据库配置文件失败");

	return nStatus;
}

// int DataBaseInfo_ReadAndWrite::Save(
// 	const char* sName, 
// 	const char* sDbName, 
// 	const char* sDbUser, 
// 	const char* sDbPwd, 
// 	const char* sDbIP, 
// 	const char* sDbPort)
// {
// 	int nStatus = 0;
// 
// 	do
// 	{
// 		nStatus--;   // 检查参数
// 		if ( false == CheckStrLen_f(sName,   32) ||
// 			 false == CheckStrLen_f(sDbName, 32) ||
// 			 false == CheckStrLen_f(sDbUser, 32) ||
// 			 false == CheckStrLen_f(sDbPwd,  32) ||
// 			 false == CheckStrLen_f(sDbIP,   15) ||
// 			 false == CheckStrLen_f(sDbPort,  5) ) break;
// 
// 		nStatus--;  // 保存在缓存中
// 		m_bIsHaveInfo = false;
// 		Clear();
// 		SInfo* p = &m_stInfo;
// 		p->sName   = StrDup_f(sName);
// 		p->sDbName = StrDup_f(sDbName);
// 		p->sDbUser = StrDup_f(sDbUser);
// 		p->sDbPwd  = StrDup_f(sDbPwd);
// 		p->sDbIP   = StrDup_f(sDbIP);
// 		p->sDbPort = StrDup_f(sDbPort);
// 		if( NULL == p->sName   ||
// 			NULL == p->sDbName ||
// 			NULL == p->sDbUser ||
// 			NULL == p->sDbPwd  ||
// 			NULL == p->sDbIP   ||
// 			NULL == p->sDbPort ) break;
// 		m_bIsHaveInfo = true;
// 
// 		nStatus--;  // 检查文件路径
// 		if ( NULL == m_sFilePath ) break;
// 
// 		//nStatus--;  // 打开文件
// 		//FILE* pFile = fopen(m_sFilePath, "w+");
// 		//if ( NULL == pFile ) break;
// 
// 		nStatus--; // 保存在文件中
// 		bool bSuc;
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_NAME,
// 			p->sName, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_NAME,
// 			p->sDbName, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_USER,
// 			p->sDbUser, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_PASSWORD,
// 			p->sDbPwd, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_IP,
// 			p->sDbIP, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		bSuc = WritePrivateProfileString(SERVICE_PARAMS_SECTION, SERVICE_PARAMS_DB_PORT,
// 			p->sDbPort, m_sFilePath);
// 		if (!bSuc) break;
// 
// 		return 0;
// 	}while(false);
// 
// 	return nStatus;
// }

void DataBaseInfo_RW::SetSaveFilePath(const char* sFilePath)
{
	if (sFilePath)
	{
		delete m_sFilePath;
		m_sFilePath = X::StrDup_f(sFilePath);
	}
}

void DataBaseInfo_RW::Clear()
{
	SInfo* p = &m_stInfo;

	// 	p->sName[0]   = '\0';
	// 	p->sDbName[0] = '\0';
	//  	p->sDbUser[0] = '\0';
	// 	p->sDbPwd[0]  = '\0';
	// 	p->sDbIP[0]   = '\0';
	// 	p->nDbPort    = 0;

	delete p->sName;
	delete p->sDbName;
	delete p->sDbUser;
	delete p->sDbPwd;
	delete p->sDbIP;
	delete p->sDbPort;

	memset(p, 0, sizeof(SInfo));

	p = &m_stStorageDbInfo;

	// 	p->sName[0]   = '\0';
	// 	p->sDbName[0] = '\0';
	//  	p->sDbUser[0] = '\0';
	// 	p->sDbPwd[0]  = '\0';
	// 	p->sDbIP[0]   = '\0';
	// 	p->nDbPort    = 0;

	delete p->sName;
	delete p->sDbName;
	delete p->sDbUser;
	delete p->sDbPwd;
	delete p->sDbIP;
	delete p->sDbPort;

	memset(p, 0, sizeof(SInfo));

}


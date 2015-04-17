#include "stdafx.h"

#include "MySqlCon.h"

#include "x_util.h"

#ifdef _DEBUG
#pragma comment(lib, "sqlapid.lib")
#else
#pragma comment(lib, "sqlapi.lib")
#endif


CMyDataBase g_cDB;
CMyDataBase g_cDB2;
//SAConnection g_cSaStorageManageConnection; // 出入库数据库连接

CMyDataBase::~CMyDataBase()
{
	if ( m_cCon.isConnected() )
		m_cCon.Destroy();
}

bool CMyDataBase::IsConnected()
{
	X::CAutoLock cLock(&m_cLock);

	return m_cCon.isConnected();
}

bool CMyDataBase::IsAlive()
{
	X::CAutoLock cLock(&m_cLock);

	return m_cCon.isAlive();
}

bool CMyDataBase::Connect(SAString& saDb, SAString& saUser, SAString& saPwd)
{
	X::CAutoLock cLock(&m_cLock);

	SAConnection* pCon = &m_cCon;
	try
	{
		if ( pCon->isConnected() )
		{
			pCon->Destroy(); // 断开重连
		}
		pCon->Connect(saDb, saUser, saPwd, SA_MySQL_Client);
	}
	catch(SAException &e)
	{
		SAString saMsg;
		saMsg = e.ErrText().GetMultiByteChars();

		LOGERR_F("CMyDataBase::Connect() fail, saMsg = %s", saMsg);
		//DB_LOG_F("CMyDataBase::Connect() fail, saMsg = %s", saMsg);
		pCon->Destroy();

		return FALSE;
	}

	return TRUE;
}

bool CMyDataBase::Excute(SACommand& saCmd, SAString& saStr)
{
	X::CAutoLock cLock(&m_cLock);

	SAConnection* pCon = &m_cCon;
	try
	{
		saCmd.setConnection(pCon);
		saCmd.setCommandText(saStr);
		saCmd.Execute();
	}
	catch(SAException &e)
	{
		SAString saMsg;
		saMsg = e.ErrText().GetMultiByteChars();

		LOGERR_F("CMyDataBase::Connect() fail, saMsg = %s", saMsg);
		//DB_LOG_F("CMyDataBase::Excute() fail, saMsg = %s", saMsg);
		saCmd.Close();

		return FALSE;
	}

	return TRUE;
}
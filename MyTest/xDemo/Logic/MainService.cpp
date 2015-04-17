#include "stdafx.h"
#include "MainService.h"

#include "MySqlCon.h"
#include "RfServece.h"

 #pragma comment(lib, "xService.lib")

////////////////////////////////////CMyProService//////////////////////////////////////

CMainService::CMainService()
	: m_nMonitorCount(0)
	, m_bIsNeedConnect1(false)
	, m_bIsNeedConnect2(false)
	, m_bIsReadDbConfig(false)
	, m_bIsFatalError(false)
	, m_bIsAutoConnectDb(false)
	, m_pDog(NULL)
	, m_pLogin(NULL)
	, m_pDataBaseReader(NULL)
	, m_pRfService(NULL)
	, m_pTestClient(NULL)
	, m_pFileDevInfoReader(NULL)
	, m_bConnectFailCount1(0)
	, m_bConnectFailCount2(0)
{
	;
}

CMainService::~CMainService()
{
	;
}

int CMainService::OnInit()
{
	DB_LOG_F("CMainService::OnInit()");

	m_pDog = CRkWatchDog::Create();
	ASSERT(m_pDog);

	if ( false == m_pDog->CheckIsOnlyOneApp() )
	{
		return -1;
	}

	m_pDog->EnterPro();

	return 0;
}

void CMainService::OnUnInit()
{
	DB_LOG_F("CMainService::OnUnInit()");

	m_pDog->LeavePro();
	delete m_pDog;

}

void CMainService::OnCreate()
{
	DB_LOG_F("CMainService::OnCreate()");

	m_pLogin = new DataBaseLogin;
	ASSERT(m_pLogin);

	m_pDataBaseReader = new DataBaseReader(m_pLogin);
	ASSERT(m_pDataBaseReader);

	m_pRfService = RfService::CreateService();
	ASSERT(m_pRfService);

	m_pTestClient = new CRfServiceClient;
	ASSERT(m_pTestClient);

	m_pFileDevInfoReader = new RfDevVarsInFile_RW;
	ASSERT(m_pFileDevInfoReader);

	CHAR_T* p = new CHAR_T[100];
}

void CMainService::OnDestroy()
{
	DB_LOG_F("CMainService::OnDestroy()");

	m_pRfService->StopCheck();

	delete m_pDataBaseReader;
	m_pDataBaseReader = NULL;

	delete m_pLogin;
	m_pLogin = NULL;

	RfService::DestoryService(m_pRfService);
	m_pRfService = NULL;

	delete m_pTestClient;
	m_pTestClient = NULL;

	delete m_pFileDevInfoReader;
	m_pFileDevInfoReader = NULL;

}

void CMainService::OnMonitor()
{
	m_nMonitorCount++;
	if ( m_nMonitorCount > 99 )
	{
		m_nMonitorCount = 0;
	}

	if (m_bIsFatalError)
	{
		return; // 发生致命错误
	}

	// 	SAConnection* pCon1 = &g_cSaConnection;
	// 	SAConnection* pCon2 = &g_cSaStorageManageConnection;

	if ( 0 == m_nMonitorCount%2 )  //  1秒
	{
		if ( !m_bIsReadDbConfig && m_bIsAutoConnectDb )
		{
			if ( 0 != m_pLogin->ReadDbConfig() )
			{
				DB_LOG_F("读取数据库配置信息失败，请检查配置文件后关闭重启");
				m_bIsFatalError = true;
				return;
			}
			else
			{
				DB_LOG_F("成功读取数据库配置");
				m_bIsReadDbConfig = true;

				m_bIsNeedConnect1 = true;
				m_bIsNeedConnect2 = true;

				m_bConnectFailCount1 = 0;
				m_bConnectFailCount2 = 0;
			}

			if (m_bIsNeedConnect1)
			{
				DB_LOG_F("开始连接数据库1");
				if ( 0 != m_pLogin->ConnectToDb(1) )
				{
					DB_LOG_F("连接数据库1 失败");
					m_bConnectFailCount1++;
				}
				else
				{
					DB_LOG_F("连接数据库1 成功");
					m_bIsNeedConnect1 = false;
				}
			}

			if (m_bIsNeedConnect2)
			{
				DB_LOG_F("开始连接数据库2");
				if ( 0 != m_pLogin->ConnectToDb(2) )
				{
					DB_LOG_F("连接数据库2 失败");
					m_bConnectFailCount2++;
				}
				else
				{
					DB_LOG_F("连接数据库2 成功");
					m_bIsNeedConnect2 = false;
				}
			}
		}
	}

	if ( 0 == m_nMonitorCount%20 )  // 5秒
	{
		LOG_F("ProService::OnMonitor(), 5S");

		if ( m_bIsReadDbConfig && m_bIsAutoConnectDb )
		{
			if (!m_bIsNeedConnect1)
			{
				// 检查数据库1 状态
				if ( false == m_pLogin->IsAlive(1) )
				{
					DB_LOG_F("数据库1 异常， 准备重连");
					m_bIsNeedConnect1 = true;
				}
				else
				{
					//DB_LOG_F("数据库1 工作正常");
				}
			}
			if (!m_bIsNeedConnect2)
			{
				// 检查数据库2 状态
				if ( false == m_pLogin->IsAlive(2) )
				{
					DB_LOG_F("数据库2 异常, 准备重连");
					m_bIsNeedConnect2 = true;
				}
				else
				{
					//DB_LOG_F("数据库2 工作正常");
				}
			}
		}

		if ( m_bIsNeedConnect1 && m_bConnectFailCount1 <= 5 )
		{
			DB_LOG_F("开始连接数据库1");
			if ( 0 != m_pLogin->ConnectToDb(1) )
			{
				DB_LOG_F("连接数据库1 失败");
				m_bConnectFailCount1++;
			}
			else
			{
				DB_LOG_F("连接数据库1 成功");
				m_bIsNeedConnect1 = false;
			}
		}

		if ( m_bIsNeedConnect2 && m_bConnectFailCount2 <= 5 )
		{
			DB_LOG_F("开始连接数据库2");
			if ( 0 != m_pLogin->ConnectToDb(2) )
			{
				DB_LOG_F("连接数据库2 失败");
				m_bConnectFailCount2++;
			}
			else
			{
				DB_LOG_F("连接数据库2 成功");
				m_bIsNeedConnect2 = false;
			}
		}

	}


}

int CMainService::DoStart()
{
	if ( false == m_pLogin->IsAlive(1) || false == m_pLogin->IsAlive(2) )
	{
		return 1; // 数据库未连接，先等待
	}

	if ( false == m_pLogin->IsLogin() )
	{
		if ( 0 != m_pLogin->Login2(MY_SUPER_USERNAME, MY_SUPER_PASSWORD) )
		{
			return -1;
		}
	}


	int nRet;

	nRet = m_pDataBaseReader->ReadFromDataBase2();
	if ( 0 != nRet )
	{
		m_pLogin->Logout2(MY_SUPER_USERNAME, MY_SUPER_PASSWORD);
		DB_LOG_F("读取数据库失败, nRet = %d", nRet);
		return -1;
	}

	if ( 0 != m_pRfService->StartCheck(m_pDataBaseReader) )
	{
		m_pLogin->Logout2(MY_SUPER_USERNAME, MY_SUPER_PASSWORD);
		DB_LOG_F("启动服务失败, nRet = %d", nRet);
		return -1;
	}

	return 0;
}

CMainService g_cService;
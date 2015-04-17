#include "stdafx.h"
#include "DataBaseOperater.h"

#include "MySqlCon.h"
#include "RfServece.h"



BOOL Ping(LPCTSTR szTarget, DWORD dwLimitTime, BOOL& bIsFound, DWORD &dwTime)
{
	BOOL bSuccess = FALSE;
	dwTime = INFINITE;
	bIsFound = FALSE;

	if(szTarget == NULL)
	{
		TRACE0("Target Is NULL\n");
		return FALSE;
	}

	if ( dwLimitTime <= 0 || dwLimitTime > 10000 )
	{
		dwLimitTime = 10000;
	}

	TCHAR szCmdLine[80];
	if(_sntprintf(szCmdLine, sizeof(szCmdLine) / sizeof(TCHAR),
		_T("ping.exe -n 1 %s"), szTarget) == sizeof(szCmdLine) / sizeof(TCHAR))
	{
		TRACE0("Target Is Too Long\n");
		return FALSE;
	}
	HANDLE hWritePipe = NULL;
	HANDLE hReadPipe = NULL;
	HANDLE hWriteShell = NULL;
	HANDLE hReadShell = NULL;
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if(CreatePipe(&hReadPipe, &hReadShell, &sa, 0)
		&& CreatePipe(&hWriteShell, &hWritePipe, &sa, 0))
	{
		STARTUPINFO           si;
		memset(&si, 0, sizeof(si));
		si.cb          = sizeof(si);
		si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.hStdInput   = hWriteShell;
		si.hStdOutput  = hReadShell;
		si.hStdError   = hReadShell;
		si.wShowWindow = SW_HIDE;
		PROCESS_INFORMATION   pi;
		memset(&pi, 0, sizeof(pi));
		int nMin = -1, nMax = -1, nAvg = -1;
		if(CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			if(WaitForSingleObject(pi.hProcess, dwLimitTime) == WAIT_OBJECT_0)
			{
				TCHAR szBuffer[1024];
				DWORD dwBytes;
				if(ReadFile(hReadPipe, szBuffer, sizeof(szBuffer), &dwBytes, NULL))
				{
					szBuffer[dwBytes] = '\0';
					LPTSTR lpszTime = NULL;
					lpszTime = _tcsstr(szBuffer, _T("Request timed out"));
					if ( NULL == lpszTime )
					{
						lpszTime = _tcsstr(szBuffer, _T("请求超时"));
					}

					if(lpszTime == NULL)
					{
						lpszTime = _tcsstr(szBuffer, _T("Minimum"));
						if ( NULL == lpszTime )
						{
							lpszTime = _tcsstr(szBuffer, _T("最短"));
						}

						if( NULL != lpszTime )
						{
							bSuccess = TRUE;
							bIsFound = TRUE;
						}
						else
						{
							TRACE0("PING format error");
						}
					}
					else
					{
						TRACE0("PING is Time Out\n");
						bSuccess = TRUE;
					}
				}
			}
			else
			{
				TRACE1("Process(%d) is Time Out\n", pi.dwProcessId);
				TerminateProcess(pi.hProcess, 0);
			}
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			//TRACE3(_T("Minimum = %dms, Maximum = %dms, Average = %dms\n"), nMin, nMax, nAvg);
		}
	}
	if(hWritePipe != NULL)
		CloseHandle(hWritePipe);
	if(hReadPipe != NULL)
		CloseHandle(hReadPipe);
	if(hWriteShell != NULL)
		CloseHandle(hWriteShell);
	if(hReadShell != NULL)
		CloseHandle(hReadShell);
	return bSuccess;
}


//////////////////////////////////DataBaseLogin////////////////////////////////////////

DataBaseLogin::DataBaseLogin()
	: m_emLoginStatus(LOGIN_STATUS_NO_CONNECTED)
	, m_sUser(NULL)
	, m_sPwd(NULL)
	, m_pLoginInfoCallBack(NULL)
{
	//DB_LOG_F("DataBaseOperater::DataBaseOperater()");

	m_pThreadCreater = new X::CThreadCreater;
	if (m_pThreadCreater)
	{
		m_pThreadCreater->SetThreadName("class DataBaseLogin");
	}

}

DataBaseLogin::~DataBaseLogin()
{
	//DB_LOG_F("~DataBaseOperater::DataBaseOperater()");

	ResetUserInfo();

	if (m_pThreadCreater)
	{
		m_bThreadExit = true;
		delete m_pThreadCreater;
	}

	//  	if ( g_cDB.isConnected() )
	//  	{
	//  		g_cDB.Disconnect();
	//  	}

	//g_cSaConnection.Destroy();
}

int DataBaseLogin::Login(const char* sUser, const char* sPwd, bool bIsUseThread)
{
	int nStatus = 0;

	do
	{
		nStatus--; //规则：登录必须是数据库未连接状态, 登出数据必须断开
		if (  LOGIN_STATUS_NO_CONNECTED != m_emLoginStatus )  break;

		nStatus--; // 记录用户名密码
		if ( 0 != AssignUserInfo(sUser, sPwd) )  break;

		nStatus--;
		if ( 0 != AssignDbInfo() ) break;

		nStatus--;
		if ( 0 != Login(bIsUseThread) ) break;

		return 0;
	}while(false);

	return nStatus;
}

int DataBaseLogin::Logout(const char* sUser, const char* sPwd)
{
	int nStatus = 0;

	do
	{
		nStatus--; //规则：登录必须是数据库未连接状态, 登出数据必须断开
		//if (  LOGIN_STATUS_CONECTED != m_emLoginStatus )  break;

		nStatus--; // 记录用户名密码
		if ( 0 != AssignUserInfo(sUser, sPwd) )  break;

		nStatus--;
		if ( 0 != Logout() ) break;

		return 0;
	}while(false);

	return nStatus;
}

int DataBaseLogin::Login2(const char* sUser, const char* sPwd)
{
	ASSERT(LOGIN_STATUS_NO_CONNECTED==m_emLoginStatus);
	if ( 0 == CheckUserInfo(sUser, sPwd) )
	{
		m_emLoginStatus = LOGIN_STATUS_CONECTED;
		return 0;
	}
	else
	{
		m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED;
		return -1;
	}

}

int DataBaseLogin::Logout2(const char* sUser, const char* sPwd)
{
	ASSERT(LOGIN_STATUS_CONECTED==m_emLoginStatus);
	if ( LOGIN_STATUS_CONECTED == m_emLoginStatus )
	{
		if ( 0 == CheckUserInfo(sUser, sPwd) )
		{
			m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED;
			return 0;
		}
	}

	return -1;
}

int DataBaseLogin::ConnectToDb(int nIndex)
{
	SAString sDbInfo, sDbUser, sDbPwd;
	DataBaseInfo_RW::SInfo* pInfo = NULL;
	CMyDataBase* pCon;

	if ( 1 == nIndex )
	{
		pInfo  = m_cDataBaseInfo.Info();
		pCon   = &g_cDB;
	}
	else if( 2 == nIndex )
	{
		pInfo  = m_cDataBaseInfo.StorageDbInfo();
		pCon   = &g_cDB2;
	}
	else
	{        
		return -1;
	}

	sDbInfo.Format("%s@%s", pInfo->sDbIP, pInfo->sDbName);
	sDbUser.Format("%s", pInfo->sDbUser);
	sDbPwd.Format("%s", pInfo->sDbPwd);

	DB_LOG_F("连接数据库 信息");
	DB_LOG_F("	sDBInfo = %s", sDbInfo);
	DB_LOG_F("	sDbUser = %s", sDbUser);
	DB_LOG_F("	sDbPwd  = %s", sDbPwd);

	return pCon->Connect(sDbInfo, sDbUser, sDbPwd) ? 0 : -1;

}

bool DataBaseLogin::IsAlive(int nIndex)
{
	//m_lock.Lock();

	if ( 1 == nIndex )
	{
		return g_cDB.IsAlive();
	}
	else if ( 2 == nIndex )
	{
		return g_cDB2.IsAlive();
	}

	return FALSE;

	// 	bool b1 = 
	// 	bool b2 = 
	// 	//m_lock.Unlock();
	// 
	// 	return b1&b2;
}


int DataBaseLogin::Login(bool bIsUseThread)
{
	int nRet = 0;
	m_emLoginStatus = LOGIN_STATUS_CONECTING;
	do 
	{
		if (bIsUseThread)
		{
			if ( NULL == m_pThreadCreater || TRUE == m_pThreadCreater->IsHasThreadRun()  ) // 已经有线程在运行
			{
				nRet = -1;
				break;
			}

			// 连接数据库并等待连接
			m_bThreadExit = false;
			if ( 0 != m_pThreadCreater->CreateThread(DataBaseLogin::ConnectThread, this) )  
			{
				nRet = -2;
				break;
			}
		}
		else
		{
			m_bThreadExit = false;
			ConnectSerThread2();
			if ( LOGIN_STATUS_CONECTED != m_emLoginStatus )
			{
				nRet = -3;
				break;
			}
		}

		return 0;
	} while (false);

	m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED;
	return nRet;
}

int DataBaseLogin::Logout(bool bIsCheckUser)
{
	return -1;
	// 	int nStatus = 0;
	// 	
	// 	SAConnection* pConnection = &g_cDB;
	// 	SAConnection* pConnection2 = &g_cSaStorageManageConnection;
	// 	do
	// 	{
	// 		//检查数据库连接  必定连接
	// // 		nStatus--;
	// // 		if ( !pConnection->isConnected() )
	// // 		{
	// // 			LOGERR_F("DataBaseOperater::Logout(), !pConnection->isConnected()");
	// // 			break;
	// // 		}
	// 
	// 		//检查用户名密码
	// 		nStatus--;
	// 		if ( bIsCheckUser && 0 != CheckUserInfo(m_sUser, m_sPwd) ) break;
	// 
	// 		//停止所有后台线程
	// 		//nStatus--;
	// 
	// 		//销毁数据库连接
	// 		//nStatus--;
	// 		pConnection->Destroy();
	// 		pConnection2->Destroy();
	// 		m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED;
	// 
	// 		return 0;
	// 	}while(false);
	// 	
	// 	return nStatus;
}



BOOL DataBaseLogin::Ping()
{
	DataBaseInfo_RW::SInfo* pInfo  = m_cDataBaseInfo.Info();
	DataBaseInfo_RW::SInfo* pInfo2 = m_cDataBaseInfo.StorageDbInfo();
	if ( NULL == pInfo || NULL == pInfo2 )
	{
		return -1;
	}

	// LPCTSTR szTarget, DWORD dwLimitTime, BOOL& bIsFound, DWORD &dwTime
	const char* sIp1 = pInfo->sDbIP;
	const char* sIp2 = pInfo2->sDbIP;  

	DWORD dwLimiTime = 500;
	BOOL bIsFound;
	DWORD dwDymmy;

	if ( FALSE == ::Ping((LPCTSTR)sIp1, dwLimiTime, bIsFound, dwDymmy) || FALSE == bIsFound )
	{
		DB_LOG_F("Ping(), sIp1 = %s, fail", sIp1);
		return FALSE;
	}

	if ( FALSE == ::Ping((LPCTSTR)sIp2, dwLimiTime, bIsFound, dwDymmy) || FALSE == bIsFound )
	{
		DB_LOG_F("Ping(), sIp2 = %s, fail", sIp2);
		return FALSE;
	}

	return TRUE;
}

int DataBaseLogin::CheckUserInfo(const char* sCheckUser, const char* sCheckPwd)
{
	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB;
	SACommand saCmd;
	SAString strSql;

	do
	{
		nStatus--;
		if ( NULL == sCheckUser || NULL == sCheckPwd )  break;  // 检查连接状态

		nStatus--;  // 比较数据库密码
		bool bReadSuc = false;
		long lUserId = 0;
		const char* sRealUser = NULL;
		const char* sRealPwd  = NULL;
		long lUserLevel;
		long lUserLimite;

		strSql.Format("Select * from t_qwry where XH = '%s'", sCheckUser);
		if ( pCon->IsAlive() && true == pCon->Excute(saCmd, strSql) )
		{
			if (saCmd.FetchNext()) //只使用第一个数据
			{
				lUserId     = saCmd.Field("ID").asLong();
				sRealUser   = saCmd.Field("XH").asString().GetMultiByteChars();
				sRealPwd    = saCmd.Field("XH").asString().GetMultiByteChars();
				lUserLevel  = 0;
				lUserLimite = 0;

				if( 0 == StrCmp(sCheckPwd, sRealPwd) ) 	bReadSuc = true;
			}

			saCmd.Close();
		}

		nStatus--; // 比较超级密码
		if ( !bReadSuc )
		{
			if( 0 == StrCmp(sCheckUser, MY_SUPER_USERNAME) && 0 == StrCmp(sCheckPwd, MY_SUPER_PASSWORD) )
			{
				bReadSuc = true;
			}

			if ( !bReadSuc )
			{
				break;
			}
		}

		return 0;
	}while(false);

	return nStatus;
}

int DataBaseLogin::AssignUserInfo(const char* sUser, const char* sPwd)
{
	ResetUserInfo();
	m_sUser = X::StrDup_f(sUser);
	m_sPwd = X::StrDup_f(sPwd);
	if ( NULL == m_sUser || NULL == m_sPwd )
	{
		ResetUserInfo();
		return -1;
	}

	return 0;
}

void DataBaseLogin::ResetUserInfo()
{
	delete m_sUser;
	delete m_sPwd;

	m_sUser = NULL;
	m_sPwd = NULL;
}

int DataBaseLogin::AssignDbInfo()
{
	return -1;
// 	DataBaseInfo_ReadAndWrite::SInfo* pInfo  = m_cDataBaseInfo.Info();
// 	DataBaseInfo_ReadAndWrite::SInfo* pInfo2 = m_cDataBaseInfo.StorageDbInfo();
// 	if ( NULL == pInfo || NULL == pInfo2 )
// 	{
// 		return -1;
// 	}
// 
// 	m_sastrDBInfo.Format("%s@%s", pInfo->sDbIP, pInfo->sDbName);;
// 	m_sastrDbUser.Format("%s", pInfo->sDbUser);;
// 	m_sastrDbPassword.Format("%s", pInfo->sDbPwd);
// 
// 	pInfo = pInfo2;
// 
// 	m_sastrDBInfo2.Format("%s@%s", pInfo->sDbIP, pInfo->sDbName);;
// 	m_sastrDbUser2.Format("%s", pInfo->sDbUser);;
// 	m_sastrDbPassword2.Format("%s", pInfo->sDbPwd);
// 
// 	return 0;
}

void DataBaseLogin::ConnectSerThread2()
{
	int nRet = 0;
	bool bDoConnect = false;

	//连接
	// 	SAConnection* pSaConection = &g_cDB;  // 主数据库
	// 	SAConnection* pSaConection2 = &g_cSaStorageManageConnection;  // 出入库数据库
	// 	do 
	// 	{
	// 		//SAString sastrDBInfo, sastrUserName, sastrSerPwd;
	// 		//sastrDBInfo.Format("%s@%s", "127.0.0.1", "3DGISRFID");
	// 		//sastrUserName.Format("%s", "root");
	// 		//sastrSerPwd.Format("%s", "summer");
	// 
	// 		DB_LOG_F("开始连接数据库");
	// 
	// 		try
	// 		{
	// 			DB_LOG_F("数据库1");
	// 			DB_LOG_F("	sDBInfo = %s", m_sastrDBInfo);
	// 			DB_LOG_F("	sDbUser = %s", m_sastrDbUser);
	// 			DB_LOG_F("	sDbPassword = %s", m_sastrDbPassword);
	// 
	// 			if ( pSaConection->isConnected() )
	// 			{
	// 				pSaConection->Destroy(); // 断开重连
	// 			}
	// 			pSaConection->Connect(m_sastrDBInfo, m_sastrDbUser, m_sastrDbPassword, SA_MySQL_Client);
	// 
	// 			DB_LOG_F("数据库2");
	// 			DB_LOG_F("	sDBInfo = %s", m_sastrDBInfo2);
	// 			DB_LOG_F("	sDbUser = %s", m_sastrDbUser2);
	// 			DB_LOG_F("	sDbPassword = %s", m_sastrDbPassword2);
	// 
	// 			if ( pSaConection2->isConnected() )
	// 			{
	// 				pSaConection2->Destroy();
	// 			}
	// 			//if( !pSaConection2->isConnected() || !pSaConection2->isAlive() )
	// 				pSaConection2->Connect(m_sastrDBInfo2, m_sastrDbUser2, m_sastrDbPassword2, SA_MySQL_Client);
	// 
	// 		}
	// 		catch(SAException &e)
	// 		{
	// 			SAString sastrErrMsg;
	// 			sastrErrMsg = e.ErrText().GetMultiByteChars();
	// 
	// 			pSaConection->Destroy();
	// 			pSaConection2->Destroy();
	// 
	// 			nRet = -1;
	// 			break;
	// 		}
	// 
	// 		// 等待连接
	// 		int nCount = 50;
	// 		while(!m_bThreadExit)
	// 		{
	// 			bDoConnect = 
	// 				pSaConection->isConnected() & pSaConection->isAlive() &
	// 				pSaConection2->isConnected() & pSaConection2->isAlive();
	// 			if ( true == bDoConnect ) 
	// 			{
	// // 
	// // 				TRACE("DataBaseLogin::Check(), %d:%d:%d:%d ", b1, b2, b3, b4);
	// 				ResetAliveCount();
	// 				break;
	// 			}
	// 
	// 			Sleep(100);
	// 			if ( --nCount <= 0 ) break; // 检测超时
	// 		}
	// 		if(!bDoConnect)
	// 		{
	// 			pSaConection->Destroy();
	// 			pSaConection2->Destroy();  //未连接则直接摧毁， 避免影响下次连接
	// 
	// 			DB_LOG_F("连接超时");
	// 
	// 			nRet = -2;
	// 			break;
	// 		}
	// 
	// 		//检查用户名和密码  无效则直接销毁
	// 		if ( 0 != CheckUserInfo(m_sUser, m_sPwd) )
	// 		{
	// 			pSaConection->Destroy();
	// 			pSaConection2->Destroy(); //未连接则直接摧毁， 避免影响下次连接
	// 
	// 			DB_LOG_F("用户名无效");
	// 
	// 			bDoConnect = false;
	// 			nRet = -3;
	// 			break;
	// 		}
	// 
	// 	} while (false);
	// 	
	// 	// 回调
	// 	if (m_pLoginInfoCallBack)
	// 	{
	// 		m_pLoginInfoCallBack->OnLogionStatus(bDoConnect, (void*)nRet);  // 无论成功还是失败都回调	
	// 	}		
	// 	
	// 	if (!bDoConnect)  // 在线程结尾处赋值，避免异常
	// 	{
	// 		DB_LOG_F("数据库连接失败");
	// 
	// 		m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED;
	// 		LOGW_F("m_emLoginStatus = LOGIN_STATUS_NO_CONNECTED, nRet = %d", nRet);
	// 	}
	// 	else
	// 	{
	// 		DB_LOG_F("数据库连接成功");
	// 
	// 		m_emLoginStatus = LOGIN_STATUS_CONECTED;
	// 		LOG_F("m_emLoginStatus = LOGIN_STATUS_CONECTED");
	// 	}

	//delete pSaConection;
	//LOG_F("DataBaseLogin::ConnectSerThread2(), nRet = %d", nRet);
}



//////////////////////////////////DataBaseReader////////////////////////////////////////

DataBaseReader::DataBaseReader(DataBaseLogin* pLogin)
	: m_pReadDataCallBack(NULL)
	, m_pReadDataCallBack2(NULL)
	, m_pLogin(pLogin)
{
	//DB_LOG_F("DataBaseReader::DataBaseReader()");

	ASSERT( NULL != m_pLogin );

	m_vec_Readers.clear();
	m_vec_Regions.clear();

	FreeAllMap();
}

DataBaseReader::~DataBaseReader()
{
	//DB_LOG_F("~DataBaseReader::DataBaseReader()");

	FreeReadersAndRegions();
	FreeAllMap();
	//g_cSaConnection.Destroy();
}


int DataBaseReader::ReadFromDataBase()
{
	FreeReadersAndRegions();

	// 获得nLocateServerID
	int nServerID = -1;
	if ( 0 != ReadServerId(&nServerID) && nServerID <= 0 )
	{
		return -1;
	}
	
	//读取接收器数据
	if ( 0 != ReadReaders() )
	{
		return -2;
	}

	//读取区域数据
	if ( 0 != ReadRegions(nServerID) )
	{
		return -3;
	}

	//读取数据库关联表数据
	if ( 0 != ReadRegionsForLink() )
	{
		return -4;
	}

	return 0;
}

int DataBaseReader::ReadFromDataBase2()
{
	int nStatus = 0;

	CMyDataBase* pCon = &g_cDB2;
	
	SAString strSql;
	int nCount;

	RF_INFO_READER_IN stReaderInfo;
	RF_INFO_REGION_POINT_IN stReaderRegion;
	RF_INFO_REGION_POINT_IN stTrigerRegion;

	FreeAllMap();

	do 
	{
		nStatus--;
		if ( !pCon->IsAlive() )
		{
			break;  // 检查连接状态
		}

		nStatus--;
		{
			SACommand saCmd;

			strSql.Format("SELECT m_id, m_ip, m_port FROM rf_reader");
			if ( false == pCon->Excute(saCmd, strSql) )
			{
				break;
			}

			nCount = 0;
			while (saCmd.FetchNext())
			{
				memset(&stReaderInfo, 0, sizeof(stReaderInfo));

				stReaderInfo.ID = (int)saCmd.Field("m_id").asLong();
				strncpy_s(stReaderInfo.IP, saCmd.Field("m_ip").asString().GetMultiByteChars(), 16); stReaderInfo.IP[15] = '\0';
				stReaderInfo.Port = (int)saCmd.Field("m_port").asLong();
				stReaderInfo.x = 0.0f;
				stReaderInfo.y = 0.0f;

				if ( stReaderInfo.ID <= 0 ||
					false == m_cReaderMap.Insert(stReaderInfo.ID, stReaderInfo) )
				{
					DB_LOG_F("error DataBaseReader::ReadFromDataBase2(), nStatus = %d, stReaderInfo.ID = %d", nStatus, stReaderInfo.ID);
					continue;
				}

				// 标记
				nCount++;
				if ( nCount > 100 )  // 最多读取100条
				{
					DB_LOG_F("warn DataBaseReader::ReadFromDataBase2(), nStatus = %d, nCount > 100", nStatus);
					break;
				}
			}
			saCmd.Close();
		}
		

		nStatus--;
		{
			SACommand saCmd;
			nCount = 0;

			// 添加接收器点区域
			strSql.Format("SELECT a.m_id, b.m_pos_floor, b.m_pos_x, b.m_pos_y, b.m_scale FROM rf_reader AS a "
				"INNER JOIN rf_staticpoint AS b ON a.m_pos_linkid = b.m_id");
			if ( false == pCon->Excute(saCmd, strSql) )
			{
				break;
			}

			while (saCmd.FetchNext())
			{
				memset(&stReaderRegion, 0, sizeof(stReaderRegion));

				stReaderRegion.uiReaderID = (unsigned int)saCmd.Field("m_id").asLong();
				stReaderRegion.x = (float)saCmd.Field("m_pos_x").asLong();
				stReaderRegion.y = (float)saCmd.Field("m_pos_y").asLong();
				stReaderRegion.uiTriggerID = 0;

				if ( stReaderRegion.uiReaderID == 0 ||
					false == m_cReaderMap.Find((int)stReaderRegion.uiReaderID) ||
					false == m_cRegionPointMap.Insert(nCount+1, stReaderRegion))
				{
					DB_LOG_F("error DataBaseReader::ReadFromDataBase2(), nStatus = %d, nCount = %d, uiReaderID = %d, x = %.2f, y = %.2f", 
						nStatus, nCount, stReaderRegion.uiReaderID, stReaderRegion.x, stReaderRegion.y);

					continue;
				}

				nCount++;
				if ( nCount > 100 )  // 最多读取100条
				{
					DB_LOG_F("warn DataBaseReader::ReadFromDataBase2(), nStatus = %d, nCount > 100", nStatus);
					break;
				}
			}
			saCmd.Close();
		}
		
		nStatus--;
		{
			SACommand saCmd;

			// 添加触发器点区域
			strSql.Format("SELECT a.m_id, a.m_readerid, b.m_pos_floor, b.m_pos_x, b.m_pos_y, b.m_scale FROM rf_trigger AS a "
				"INNER JOIN rf_staticpoint AS b ON a.m_pos_linkid = b.m_id");
			if ( false == pCon->Excute(saCmd, strSql) )
			{
				break;
			}

			int nReaderId;
			while (saCmd.FetchNext())
			{
				memset(&stReaderRegion, 0, sizeof(stReaderRegion));

				stReaderRegion.uiReaderID = 0;
				stReaderRegion.x = (float)saCmd.Field("m_pos_x").asLong();
				stReaderRegion.y = (float)saCmd.Field("m_pos_y").asLong();
				stReaderRegion.uiTriggerID = (unsigned int)saCmd.Field("m_id").asLong();
				nReaderId = (int)saCmd.Field("m_readerid").asLong();

				if ( stReaderRegion.uiTriggerID == 0 ||
					false == m_cReaderMap.Find(nReaderId) ||
					false == m_cRegionPointMap.Insert(nCount+1, stReaderRegion))
				{
					DB_LOG_F("error DataBaseReader::ReadFromDataBase2(), nStatus = %d, nCount = %d, uiTriggerID = %d, nReaderId = %d, x = %.2f, y = %.2f", 
						nStatus, nCount, stReaderRegion.uiTriggerID, nReaderId, stReaderRegion.x, stReaderRegion.y);

					continue;
				}

				nCount++;
				if ( nCount > 200 )
				{
					DB_LOG_F("warn DataBaseReader::ReadFromDataBase2(), nStatus = %d, nCount > 200", nStatus);
					break;
				}
			}
			saCmd.Close();
		}

		if ( NULL != m_pReadDataCallBack2 )
		{
			m_pReadDataCallBack2->OnReadData(m_cReaderMap, m_cRegionPointMap, m_cRegionLineMap, m_cRegionRectMap); // 回调
		}

#ifdef _DEBUG
		PrintData(0);
#endif
		
		return 0;
	} while (false);

// 	if ( nStatus < -1 )
// 	{
// 		saCmd.Close();
// 	}

	FreeAllMap();

	return nStatus;
}

void DataBaseReader::PrintData(int nIndex)
{
	{
		DataBaseReader::READER_MAP_T& cMap = m_cReaderMap;
		DataBaseReader::READER_MAP_T::Iter cIter(cMap);
		RF_INFO_READER_IN stInfo;
		int nKey;

		int nCnt = 1;
		while( cIter.Next(nKey, stInfo) )
		{
			LOG_F("m_cReaderMap nCnt = %d, id = %d, ip = %s, port = %d",
				nCnt, stInfo.ID, stInfo.IP, stInfo.Port);


			nCnt++;
		}

	}


	{
		DataBaseReader::REGION_POINT_MAP_T& cMap = m_cRegionPointMap;
		DataBaseReader::REGION_POINT_MAP_T::Iter cIter(cMap);
		RF_INFO_REGION_POINT_IN stInfo;
		int nKey;
		
		int nCnt = 1;
		while( cIter.Next(nKey, stInfo) )
		{
			LOG_F("m_cRegionPointMap, nCnt = %d, uiReaderID = %d, uiTriggerID = %d, x = %.2f, y = %.2f", 
				nCnt, stInfo.uiReaderID, stInfo.uiTriggerID, stInfo.x, stInfo.y);

			nCnt++;
		}
	}
}

void DataBaseReader::FreeReadersAndRegions()
{
	if (!m_vec_Readers.empty())
	{
		VEC_ITER_PREADER_T vec_iter = m_vec_Readers.begin();
		RfReader_RW* p = NULL;
		int nCount = 0;

		while( vec_iter != m_vec_Readers.end() )
		{
			p = *vec_iter;
			delete p;

			vec_iter++;
			nCount++;
		}

	}
	m_vec_Readers.clear();

	if (!m_vec_Regions.empty())
	{
		VEC_ITER_PREGION_T vec_iter = m_vec_Regions.begin();
		RfRegion_RW* p = NULL;
		int nCount = 0;

		while( vec_iter != m_vec_Regions.end() )
		{
			p = *vec_iter;
			delete p;

			vec_iter++;
			nCount++;
		}
	}
	m_vec_Regions.clear();

}

int DataBaseReader::ReadServerId(int* pOutServerId)
{
	char sHostName[64] = {0};
	gethostname(sHostName, sizeof(sHostName));

	if ( strlen(sHostName) <= 0 )
	{
		return -1;
	}

	int nServerID = -1;
	CString addr;
	struct hostent* pHost = gethostbyname(sHostName);

	CMyDataBase* pCon = &g_cDB;
	SACommand saCmd;
	SAString strSql;

	for(int j=0; (pHost!=NULL)&&(pHost->h_addr_list[j]!=NULL); j++) 
	{ 
		addr=inet_ntoa(*(struct   in_addr*)pHost-> h_addr_list[j]); 
		if (!addr.IsEmpty())
		{
			strSql.Format("SELECT * FROM LocalServerInfo where LocalSvr_IP='%s'", addr);
			if ( false == pCon->Excute(saCmd, strSql) )
			{
				saCmd.Close();
				return -2;
			}

			if( saCmd.FetchNext() )
			{
				nServerID = saCmd.Field("LocalSvr_ID").asLong();
				saCmd.Close();
				break;
			}
			saCmd.Close();
		}			 
	} 

	if ( nServerID < 0 )
	{
		return -3;
	}

	*pOutServerId = nServerID;

	return 0;
}

int DataBaseReader::ReadReaders()
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
		strSql.Format("select * from ReceiverInfo");
		if ( false == pCon->Excute(saCmd, strSql) )
		{
			saCmd.Close();
			break;
		}

		int nCount = 0;
		RfReader_RW* pReader;

		while (saCmd.FetchNext())
		{
			pReader = new RfReader_RW; 
			if ( NULL == pReader )
			{
				break;
			}

			if ( 0 != pReader->Read_FromDataBase(&saCmd) )
			{
				TRACE("\n DataBaseOperater::ReadFromDataBase(), ReadDataBase() reader, fail ");
				delete pReader;
				break;
			}
			m_vec_Readers.push_back(pReader);

			// 标记
			nCount++;
			if ( nCount > 100 )  // 最多读取100条
			{
				break;
			}
		}
		saCmd.Close();
		
		if ( nCount >= 0 && NULL != m_pReadDataCallBack )
		{
			VEC_ITER_PREADER_T vec_iter = m_vec_Readers.begin();
			m_pReadDataCallBack->OnReadReaderData(m_vec_Readers.size(), vec_iter); // 回调
		}


		return 0;
	}while(false);

	return nStatus;
}

int DataBaseReader::ReadRegions(int nServerId)
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
		strSql.Format("select * from RectInfo where LocalSvr_ID=%d", nServerId);
		if ( false == pCon->Excute(saCmd, strSql) )
		{
			saCmd.Close();
			break;
		}

		int nCount = 0;
		RfRegion_RW* pRegion;

		while (saCmd.FetchNext())
		{
			pRegion = new RfRegion_RW;
			if ( NULL == pRegion )
			{
				break;
			}

			if ( 0 != pRegion->Read_FromDataBase(&saCmd) )
			{
				TRACE("\n DataBaseOperater::ReadFromDataBase(), ReadDataBase() region, fail ");
				delete pRegion;
				break;
			}
			m_vec_Regions.push_back(pRegion);

			// 标记
			nCount++;
			if ( nCount > 100 )  // 最多读取100条
			{
				break;
			}
		}
		saCmd.Close();

// 		if ( nCount >= 0 && NULL != m_pReadDataCallBack )
// 		{
// 			VEC_ITER_PREADER_T vec_iter = m_vec_Readers.begin();
// 			m_pReadDataCallBack->OnReadReaderData(m_vec_Readers.size(), vec_iter); // 回调
// 		}


		return 0;
	}while(false);

	return nStatus;
}

int DataBaseReader::ReadRegionsForLink()
{
	RfRegion_RW* pRegion;

	int nVecSize = m_vec_Regions.size();
	int nReadSucCount = 0;
	for (int n=0; n<nVecSize; n++)
	{
		pRegion = m_vec_Regions[n];
		ASSERT(pRegion);
		if ( 0 == pRegion->ReadLink_FromDataBase() )
		{
			nReadSucCount++;
		}
	}
	if ( 0 == nReadSucCount || nReadSucCount != nVecSize )
	{
		return -1;
	}
	TRACE("\n DataBaseOperater::ReadFromDataBase(), ReadLinkDataBase() region, nVecSize = %d, nReadSucCount = %d",
		nVecSize, nReadSucCount);
	if ( NULL != m_pReadDataCallBack )
	{
		VEC_ITER_PREGION_T vec_iter = m_vec_Regions.begin(); 
		m_pReadDataCallBack->OnReadRegionData(m_vec_Regions.size(), vec_iter);
	}

	return 0;
}

void DataBaseReader::FreeAllMap()
{
	m_cReaderMap.Clear();

	m_cRegionPointMap.Clear();
	m_cRegionLineMap.Clear();
	m_cRegionRectMap.Clear();

}

//////////////////////////////////////RfDeviceParameter_ReadAndWrite////////////////////////////////////

RfDevVarsInFile_RW::RfDevVarsInFile_RW()
	: m_bIsReadSuc(false)
	, m_pCallBack(NULL)
{
	memset(&m_stData, 0, sizeof(SData));
}

RfDevVarsInFile_RW::~RfDevVarsInFile_RW()
{
	FreeData();
}

int RfDevVarsInFile_RW::Read()
{
	int nStatus = 0;

	FreeData();  // 释放之前的数据


	char sFilePath[256]={0};
	RKGUI_GetModuleAppPath(sFilePath);
	sFilePath[255] = '\0';
	if ( strlen(sFilePath) <= 0 ) 
	{
		return -1;
	}
	strcat(sFilePath, RFDEV_PARAMS_FILEPATH);

	const char* pFilePath = sFilePath;


	SData* pData = &m_stData;
	RF_INFO_READER_IN* pReaderList = NULL;

	int nErrRet;
	do 
	{
		nStatus--;
		if ( NULL == pFilePath ) break;

		nStatus--;
		nErrRet = ReadBaseData(pFilePath);
		if ( 0 != nErrRet ) nErrRet;

		nStatus--;
		int nReadNum = pData->nReaderNum;
		if ( nReadNum <= 0 ) break;

		pReaderList = new RF_INFO_READER_IN[nReadNum];
		int nReaderCnt;
		for (nReaderCnt = 0; nReaderCnt<nReadNum; nReaderCnt++)
		{
			nErrRet = ReadReaderData(pFilePath, nReaderCnt, &pReaderList[nReaderCnt]);
			if ( 0 != nErrRet ) break;
		}
		if ( nReaderCnt != nReadNum ) break;

		pData->pReaderList = pReaderList;

		nStatus--;
		{
			if ( NULL != m_pCallBack ) // 整理数据并回调
			{
				m_pCallBack->OnReadReaderData(pData);
			}
		}

		m_bIsReadSuc = true;
		return 0;
	} while (false);

	if (pReaderList)
	{
		delete[] pReaderList;
	}

	return nStatus;
}

void RfDevVarsInFile_RW::FreeData()
{
	if (m_bIsReadSuc)
	{
		if (m_stData.pReaderList)
		{
			delete[] m_stData.pReaderList;
		}
	}

	m_bIsReadSuc = false;

}

int RfDevVarsInFile_RW::ReadBaseData(const char* pFilePath)
{
	int nStatus = 0;

	SData* pData = &m_stData;
	RF_INFO_READER_IN* pReaderList = NULL;

	const int nMaxChar = 8;

	char sReaderNum[nMaxChar+1];
	char sIsUseReaderPoint[nMaxChar+1];
	char sIsAutoAdd125kPoint[nMaxChar+1];

	DWORD dwRet;
	do 
	{
		nStatus--;
		dwRet = GetPrivateProfileString(RFDEV_PARAMS_BASE_SEC, RFDEV_PARAMS_BASE_SEC_READER_NUM,
			"", sReaderNum, nMaxChar, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(RFDEV_PARAMS_BASE_SEC, RFDEV_PARAMS_BASE_SEC_USE_READER_PONINT,
			"", sIsUseReaderPoint, nMaxChar, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(RFDEV_PARAMS_BASE_SEC, RFDEV_PARAMS_BASE_SEC_USE_READER_PONINT,
			"", sIsAutoAdd125kPoint, nMaxChar, pFilePath);
		if ( 0 == dwRet ) break;

		sReaderNum[nMaxChar] = '\0';      // 保护，避免字符串溢出
		sIsUseReaderPoint[nMaxChar] = '\0';
		sIsAutoAdd125kPoint[nMaxChar] = '\0';

		pData->nReaderNum = atoi(sReaderNum);
		pData->bIsUseReaderPoint = atoi(sIsUseReaderPoint);
		pData->bIsAutoAdd125kPoint = atoi(sIsAutoAdd125kPoint);

		return 0;
	} while (false);

	return nStatus;
}

int RfDevVarsInFile_RW::ReadReaderData(const char* pFilePath, int nIndex, RF_INFO_READER_IN* pSink)
{
	int nStatus = 0;

	SData* pData = &m_stData;
	RF_INFO_READER_IN* pReader = pSink;

	char sId[16];
	char sIp[16];     
	char sPort[8];     
	char sPosX[32];         
	char sPosY[32];

	CString sSecStr;
	sSecStr.Format("%s_%d", RFDEV_PARAMS_READER_SEC, nIndex+1);

	DWORD dwRet;
	do 
	{
		nStatus--;
		dwRet = GetPrivateProfileString(sSecStr, RFDEV_PARAMS_READER_SEC_ID,
			"", sId, 16, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(sSecStr, RFDEV_PARAMS_READER_SEC_IP,
			"", sIp, 16, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(sSecStr, RFDEV_PARAMS_READER_SEC_PORT,
			"", sPort, 8, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(sSecStr, RFDEV_PARAMS_READER_SEC_POINT_X,
			"", sPosX, 32, pFilePath);
		if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(sSecStr, RFDEV_PARAMS_READER_SEC_POINT_Y,
			"", sPosY, 32, pFilePath);
		if ( 0 == dwRet ) break;

		sId[16-1] = '\0';      // 保护，避免字符串溢出
		sIp[16-1] = '\0';
		sPort[8-1] = '\0';
		sPosX[32-1] = '\0';
		sPosY[32-1] = '\0';

		pReader->ID = atoi(sId);
		strcpy(pReader->IP, sIp);
		pReader->Port = atoi(sPort);

		pReader->x = atof(sPosX);
		pReader->y = atof(sPosY);

		return 0;
	} while (false);

	return nStatus;
}



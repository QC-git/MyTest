#include "stdafx.h"


#include "RkUtil.h"
#include "x_util.h"
#include "x_thread.h"

#include "MyWatchDog.h"

//////////////////////////////CRkWatchDogThread///////////////////////////////////

class CRkWatchDogThread : public X::CRoundThread 
{
public:
	CRkWatchDogThread();
	~CRkWatchDogThread();

	void ReFreshTime();

public:
	//Override
	void DoRoundPro();

private:
	X::CLock m_cLock;

	long m_lLastFeedTime;

};

CRkWatchDogThread::CRkWatchDogThread()
	: m_lLastFeedTime(0)
{
	this->SetThreadName("CRkWatchDogThread");
}

CRkWatchDogThread::~CRkWatchDogThread()
{
	;
}

void CRkWatchDogThread::DoRoundPro()
{
#if !RK_NOFEEDDOG_ENABLE
	m_cLock.Lock();
	long lCurTime = time(NULL);
	if ( 0 == m_lLastFeedTime )
	{
		m_lLastFeedTime = lCurTime;
	}
	else
	{
		long lDirTime = lCurTime - m_lLastFeedTime;
		if ( lDirTime < 0 ) // 时间发生倒退， 重置时间
		{
			m_lLastFeedTime = lCurTime;
		}
		else if ( lDirTime > RK_FEEDDOG_INTERVAL )  // 喂狗时间超时
		{
			DB_LOG_F("CRkWatchDogThread::DoRoundPro(), lDirTime > RK_FEEDDOG_INTERVAL, exit()");
			exit(1);
		}
	}
	m_cLock.Unlock();
#endif
	Sleep(50);
}

void CRkWatchDogThread::ReFreshTime()
{
	m_cLock.Lock();
	m_lLastFeedTime = time(NULL);
	m_cLock.UnLock();
}

CRkWatchDogThread g_cWatchDogThread;

//////////////////////////////CRkWatchDog///////////////////////////////////

#define RK_WATCHDOG_PARAMS_FILE_NAME  "watchdog_parameters.ini" 

#define RK_WATCHDOG_PARAMS_SEC  "watchdog_parameters"    // 是否使用接收器参数的坐标

#define RK_WATCHDOG_PARAMS_SEC_DOGNAME            "dog_name"
#define RK_WATCHDOG_PARAMS_SEC_ENTER_PRO_STATUS   "enter_pro_status"

#define RK_WATCHDOG_PARAMS_SEC_LOGIN_STATUS   "login_status"
#define RK_WATCHDOG_PARAMS_SEC_SERVE_STATUS   "serve_status"

CWatchDogParameters::CWatchDogParameters()
	: m_bIsReadFileSuc(false)
{
	memset(&m_stData, 0, sizeof(m_stData));
	ResetData();
}

CWatchDogParameters::~CWatchDogParameters()
{
	;
}

int CWatchDogParameters::Read()
{
	int nStatus = 0;

	SData* pData = &m_stData;

	char sFilePath[256]={0};
	RKGUI_GetModuleAppPath(sFilePath);
	sFilePath[255] = '\0';
	if ( strlen(sFilePath) <= 0 ) 
	{
		return -1;
	}
	strcat(sFilePath, RK_WATCHDOG_PARAMS_FILE_NAME);

	const char* pFilePath = sFilePath;

	char sDogName[64];
	char sEnterProStatus[8];
	char sLoginStatus[8];
	char sServeStatus[8];

	DWORD dwRet;
	do 
	{
		nStatus--;
		dwRet = GetPrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_DOGNAME,
			"", sDogName, 63, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_ENTER_PRO_STATUS,
			"", sEnterProStatus, 7, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_LOGIN_STATUS,
			"", sLoginStatus, 7, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = GetPrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_SERVE_STATUS,
			"", sServeStatus, 7, pFilePath);
		//if ( 0 == dwRet ) break;

		sDogName[63] = '\0';
		sEnterProStatus[7] = '\0';
		sLoginStatus[7] = '\0';
		sServeStatus[7] = '\0';

		ResetData(); // 重置存储数据内容

		strcpy(pData->sDogName, sDogName);
		pData->bLastProStatus = (BOOL)atoi(sEnterProStatus);

		pData->bIsEverLogin = (BOOL)atoi(sLoginStatus);
		pData->bIsEverStartService = (BOOL)atoi(sServeStatus);

		m_bIsReadFileSuc = true;
		return 0;
	} while (false);

	return nStatus;
}


int CWatchDogParameters::Write()
{
	int nStatus = 0;

	SData* pData = &m_stData;

	char sFilePath[256]={0};
	RKGUI_GetModuleAppPath(sFilePath);
	sFilePath[255] = '\0';
	if ( strlen(sFilePath) <= 0 ) 
	{
		return -1;
	}
	strcat(sFilePath, RK_WATCHDOG_PARAMS_FILE_NAME);

	const char* pFilePath = sFilePath;

	char sDogName[64];
	char sEnterProStatus[8];
	char sLoginStatus[8];
	char sServeStatus[8];

	_snprintf(sDogName, 63, "%s", pData->sDogName);
	_snprintf(sEnterProStatus, 7, "%d", pData->bEnterProStatus);
	_snprintf(sLoginStatus, 7, "%d", pData->bLogin);
	_snprintf(sServeStatus, 7, "%d", pData->bStartService);

	sDogName[63] = '\0';
	sEnterProStatus[7] = '\0';
	sLoginStatus[7] = '\0';
	sServeStatus[7] = '\0';

	DWORD dwRet;
	do 
	{
		nStatus--;
		dwRet = WritePrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_DOGNAME,
			sDogName, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = WritePrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_ENTER_PRO_STATUS,
			sEnterProStatus, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = WritePrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_LOGIN_STATUS,
			sLoginStatus, pFilePath);
		//if ( 0 == dwRet ) break;

		nStatus--;
		dwRet = WritePrivateProfileString(RK_WATCHDOG_PARAMS_SEC, RK_WATCHDOG_PARAMS_SEC_SERVE_STATUS,
			sServeStatus, pFilePath);
		//if ( 0 == dwRet ) break;

		return 0;
	} while (false);

	return nStatus;
}

void CWatchDogParameters::ResetData()
{
	m_stData.sDogName[0]    = '\0';
	m_stData.sServerName[0] = '\0';
	m_stData.bEnterProStatus = false;

	m_bIsReadFileSuc = false;
}

//////////////////////////////CRkWatchDog///////////////////////////////////

#define CNPlatMessage        "RKServerWatch Init Message."
#define CNRKServerPipe       "\\\\.\\Pipe\\RKServerWatch"

int CRkWatchDog::s_nCreateCount = 0;

CRkWatchDog* CRkWatchDog::Create()
{
	if ( s_nCreateCount > 0 )  // 只能创建一个
	{
		return NULL;
	}

	CRkWatchDog* pDog = new CRkWatchDog;
	if ( NULL != pDog )
	{
		s_nCreateCount++;
		return pDog;
	}

	return NULL;
}

CRkWatchDog::CRkWatchDog()
{
	;
}

CRkWatchDog::~CRkWatchDog()
{
	;
}

BOOL CRkWatchDog::RegisterServer(BOOL bRegister)
{
	DB_LOG_F("CRfWatchDog::RegisterServer(), bRegister = %d", bRegister);

	// 反注册服务
	char szAppName[MAX_PATH] ={0};
	char szDogExeName[MAX_PATH] ={0};
	char szPipe[MAX_PATH]={0};
	if (bRegister)
	{
		RKGUI_GetModuleAppPath(szDogExeName);
		strcat(szDogExeName, "iRKServerWatch.exe");

		DB_LOG_F("    WinExec(), szDogExeName = %s", szDogExeName);
		UINT uiRet = WinExec(szDogExeName, SW_SHOW);
		DB_LOG_F("    WinExec(), uiRet = %d", uiRet);

		RegisterWindowMessage(CNPlatMessage);         // 注册Crash消息
	}

	strcpy(szPipe, CNRKServerPipe);
	if( GetModuleFileName(RKGUI_NULL, szAppName,RKGUI_MAX_PATH) > 0)
	{
		RKGUI_RegisterServer(bRegister, szPipe, szAppName);
	}
	else
	{
		RKGUI_RegisterServer(bRegister, szPipe, RK_RF_SERVER);
	}

	return TRUE;
}

BOOL_T CRkWatchDog::CheckIsOnlyOneApp()
{
	return (BOOL_T)RKGUI_CreateOnlyOneApp( ((RKGUI_wchar_t*)RK_RF_SERVER_MUTEX));
}

int CRkWatchDog::EnterPro()
{
	if ( false == StartWatchThread() )
	{
		return -1;
	}

	Sleep(20);  // 线程必须启动

#if RK_WATCHDOG_ENABLE
	RegisterServer(TRUE);  // 注册看门狗
#endif

	if ( 0 != m_cParams.Read() )
	{
		return -2;
	}

	CWatchDogParameters::SData* pData = m_cParams.GetDataPtr();

	pData->bEnterProStatus = 1;
	m_cParams.Write(); 

	return pData->bLastProStatus ? 1 : 0;  // 正常时，该标志位应是0;
}

void CRkWatchDog::LeavePro()
{
#if RK_WATCHDOG_ENABLE
	RegisterServer(FALSE);
#endif

	StopWatchThread();


	CWatchDogParameters::SData* pData = m_cParams.GetDataPtr();

	pData->bEnterProStatus = 0;  // 将标志清0以标志正常退出

	m_cParams.Write();  // 写入文件

}

void CRkWatchDog::Feed()
{
	g_cWatchDogThread.ReFreshTime();
}

BOOL_T CRkWatchDog::StartWatchThread()
{
	return g_cWatchDogThread.TStartup();
}

void CRkWatchDog::StopWatchThread()
{
	g_cWatchDogThread.TClose();
}

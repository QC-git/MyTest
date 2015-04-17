
#include "x_thread.h"
#include "x_util.h"

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

///////////////////////////////////信号///////////////////////////////////////

_SERVICE_IMPLEMENT VOID_T MutexInit_f(HMUTEX_D* hMutex)
{
#ifdef _WIN32
	InitializeCriticalSection(hMutex);
#else
	pthread_mutexattr_t mutex_attr;
	if ( pthread_mutexattr_init(&mutex_attr) != 0 )
	{
		PRINT_F("\n pthread_mutexattr_init error");
		return;
	}

	if ( pthread_mutex_init(hMutex, &mutex_attr) != 0 )
	{
		PRINT_F("\n pthread_mutexattr_init error");
		return;
	}
#endif
}

_SERVICE_IMPLEMENT VOID_T MutexUnInit_f(HMUTEX_D* hMutex)
{
#ifdef _WIN32
	DeleteCriticalSection(hMutex);
#else
	if ( pthread_mutex_destroy(hMutex) != 0 )
	{
		PRINT_F("\n pthread_mutex_destroy error");
	};
#endif
}

_SERVICE_IMPLEMENT VOID_T MutexLock_f(HMUTEX_D* hMutex)
{
#ifdef _WIN32
	EnterCriticalSection(hMutex);
#else
	pthread_mutex_lock(hMutex);
#endif
}

_SERVICE_IMPLEMENT VOID_T MutexUnLock_f(HMUTEX_D* hMutex)
{
#ifdef _WIN32
	LeaveCriticalSection(hMutex);
#else
	pthread_mutex_unlock(hMutex);
#endif
}

CLock::CLock()
{
	MutexInit_f(&m_hLock);
}

CLock::~CLock()
{
	MutexUnInit_f(&m_hLock);
}

VOID_T CLock::Lock()
{
	MutexLock_f(&m_hLock);
}

VOID_T CLock::UnLock()
{
	MutexUnLock_f(&m_hLock);
}

CAutoLock::CAutoLock(CLock* pLock)
	: m_pLock(pLock)
{
	m_pLock->Lock();
}

CAutoLock::~CAutoLock()
{
	m_pLock->UnLock();
}


///////////////////////////////////线程///////////////////////////////////////

#define THREAD_SLEEP_TIME 1

typedef  void* (*PTaskFunc_T)(void*);
typedef void (* PTaskFunc2_T)(void*);

_SERVICE_IMPLEMENT HTHREAD_D ThreadCreate_f(PTaskFunc_T pFunc, VOID_T* pArgList)
{
#ifdef _WIN32
	HTHREAD_D hThread;
	hThread = _beginthread((PTaskFunc2_T)pFunc, 0, pArgList);
	if ( INVALID_HANDLE_VALUE == (HANDLE)hThread )
	{
		LOGERR_F("ThreadCreate_f() fail");
		return (HTHREAD_D)0L;
	}
	else
	{
		return hThread;
	}
#else
	HTHREAD_D hThread;

	if ( pthread_create(&hThread, NULL, pFunc, pArgList) != 0 )
	{
		LOGERR_F("ThreadCreate_f() fail");
		return (HTHREAD_D)0L;
	}
	else
	{
		return hThread;
	}
#endif
}

_SERVICE_IMPLEMENT VOID_T ThreadExit_f(HTHREAD_D hThread)
{
#ifdef _WIN32
#else
	pthread_join(hThread, NULL);
#endif
}

_SERVICE_IMPLEMENT VOID_T ThreadTerminal_f(HTHREAD_D hThread)
{
#ifdef _WIN32
	if ( false == TerminateThread( (HANDLE)hThread, 1L) )
		LOGERR_F("ThreadCancel_x(), TerminateThread() fail, hThread = %d ", hThread);
	//BOOL_T bRet = (BOOL_T)TerminateThread( (HANDLE)hThread, 1L);
#else
#endif
}

////////////////////////////////////CRoundThread//////////////////////////////////////

CRoundThread::CRoundThread()
	: m_hThread(0L)
	, m_bRunEnable(FALSE)
	, m_bThreadOpen(FALSE)
	, m_sThreadName(NULL)
{
	;
};

CRoundThread::~CRoundThread()
{
	TClose();

	if (m_sThreadName)
		delete[] m_sThreadName;

}

BOOL_T CRoundThread::SetThreadName(const CHAR_T* sName)
{
	if (m_sThreadName)
		return FALSE;

	int nLen = strlen(sName);

	CHAR_T* p = new CHAR_T[nLen+1];
	memset(p, 0, nLen+1);
	memcpy(p, sName, nLen);

	m_sThreadName = p;
	return TRUE;
}

BOOL_T CRoundThread::TStartup()
{
	m_bRunEnable = TRUE;

	if ( m_hThread )
		return FALSE;

	m_bThreadOpen = TRUE;

	HTHREAD_D hThread = ThreadCreate_f((PTaskFunc_T)ThreadProc, this);

	if ( 0L == hThread)
	{
		m_bThreadOpen = FALSE;
		return FALSE;
	}

	LOGD_F("CRoundThread::TStartup(), Thread(%d) %s start! ", (int)hThread, m_sThreadName);

	//ResumeThread( hThread );
	m_hThread = hThread;

	return TRUE;
}

BOOL_T CRoundThread::TClose()
{
	if( 0L == m_hThread )
		return FALSE;

	Sleep_f(50);
	m_bRunEnable = FALSE;
	Sleep_f(50);

	if ( FALSE == WaitForThreadClose(3000) )
	{
		LOGW_F("CRoundThread::TClose(), out of time, Thread(%d) %s will be cancel ! ", (int)m_hThread, m_sThreadName);
		ThreadTerminal_f(m_hThread);
		Sleep_f(50);
	}

	ThreadExit_f(m_hThread);

	LOGD_F("CRoundThread::TClose(), Thread(%d) %s Exit ! ", (int)m_hThread, m_sThreadName);
	m_hThread = 0L;
	return TRUE;
}

VOID_T* CRoundThread::ThreadProc(VOID_T* hThis)
{
	CRoundThread* pThis=(CRoundThread*)hThis;
	//_ASSERTE( pThis != NULL );

	pThis->ProRun();

	return NULL;
}

VOID_T CRoundThread::ProRun()
{
	//	DWORD thread_id = GetCurrentThreadId();
	//	LOG_F("\n thread_id = %d", thread_id);
	Sleep_f(10);
	LOG_F("CRoundThread::ProRun(), Thread(%d) %s is runing... ! ", (int)m_hThread, m_sThreadName);

	if ( !DoOncePro() ) // 只需执行一次线程 在这里返回
	{
		m_bThreadOpen = FALSE;
		return;
	}
	while (m_bRunEnable)
	{
		DoRoundPro();

		Sleep_f(THREAD_SLEEP_TIME);
	}
	DoEndPro();

	LOG_F("CRoundThread::ProRun()， Thread(%d) %s is going to end... ! ", (int)m_hThread, m_sThreadName);

	m_bThreadOpen = FALSE;
	return;
}

BOOL_T CRoundThread::DoOncePro()
{
	return TRUE;
}


VOID_T CRoundThread::DoEndPro()
{
}

BOOL_T CRoundThread::WaitForThreadClose(TIME_T lMs)
{
	int time_base = 50;  // ms
	int time_ticks = (int)lMs / time_base;

	for ( int n=0; n<time_ticks; n++ )
	{
		Sleep_f(time_base);
		if ( FALSE == m_bThreadOpen )
			return TRUE;
	}

	return FALSE;
}

////////////////////////////////////CThreadCreater//////////////////////////////////////

CThreadCreater::CThreadCreater()
	: m_bThreadOpen(FALSE)
	, m_sThreadName(NULL)
	, m_pTaskFunc(NULL)
	, m_hTaskFuncUser(NULL)
{
	;
};

CThreadCreater::~CThreadCreater()
{
	WaitForDestroy();
	delete m_sThreadName;
}


VOID_T CThreadCreater::SetThreadName(const CHAR_T* sName)
{
	if (sName)
	{
		delete m_sThreadName;
		int nLen = strlen(sName);

		m_sThreadName = new char[nLen+1];
		memcpy(m_sThreadName, sName, nLen);
		m_sThreadName[nLen] = '\0';
	}
}

ERR_T CThreadCreater::CreateThread(TaskFunc_T* pTaskFunc, VOID_T* hTaskFuncUser)
{
	if ( TRUE == m_bThreadOpen )  // 线程已经打开， 返回1
	{
		return 1;
	}

	if ( NULL == pTaskFunc || NULL == hTaskFuncUser )
	{
		return -1;
	}
	m_pTaskFunc = pTaskFunc;
	m_hTaskFuncUser = hTaskFuncUser;
	m_bThreadOpen = TRUE;

	m_hThread = ThreadCreate_f(ThreadProc, this);  // m_hThread 可能不为0， 依然正确

	if ( 0L == m_hThread )
	{
		LOGERR_F("MRoundThread::TStartup(), ThreadCreate_f() fail ");

		m_bThreadOpen = FALSE;
		return -1;
	}

	LOG_F("MRoundThread::TStartup(), Thread(%d) %s start! ", (int)m_hThread, m_sThreadName);

	//ResumeThread( hThread );
	return 0;
}

VOID_T* CThreadCreater::ThreadProc(VOID_T* hThis)
{
	CThreadCreater* pThis=(CThreadCreater*)hThis;
	//_ASSERTE( pThis != NULL );

	pThis->ThreadProc2();

	return NULL;
}

VOID_T CThreadCreater::ThreadProc2()
{
	//ASSERT(m_pTaskFunc);
	//ASSERT(m_hTaskFuncUser);

	(*m_pTaskFunc)(m_hTaskFuncUser);

	m_bThreadOpen = false;
}

VOID_T CThreadCreater::WaitForDestroy(/*long lOutOfTime*/)
{
	U32_T uTimeDiff = 50;  // ms
	//int nTimeCount = nTimeCount / nTimeDiff;

	// 	for ( int n=0; n<nTimeCount; n++ )
	// 	{
	// 		SleepMs_f(nTimeDiff);
	// 		if ( false == m_bThreadOpen )
	// 			break;
	// 	}

	do 
	{
		if ( FALSE == m_bThreadOpen )
		{
			break;
		}

		Sleep_f(uTimeDiff);

	} while (true);

}

//测试

CLock g_cTesLock;

static S32_T s_nTesCount = 0;

class CTestThraed1: public CRoundThread
{
public:
	VOID_T DoRoundPro()
	{
		CAutoLock cLock(&g_cTesLock); // 不加锁时，数字会重复打印

		LOGD_F("CTestThraed1::DoRoundPro(), nCount = %d", s_nTesCount);

		s_nTesCount++;
		Sleep_f(50);
	}

};

class CTestThraed2: public CRoundThread
{
public:
	CTestThraed2()
	{
		SetThreadName("CTestThraed2");
	}

public:
	VOID_T DoRoundPro()
	{
		CAutoLock cLock(&g_cTesLock);  // 不加锁时，数字会重复打印

		LOGD_F("CTestThraed2::DoRoundPro(), nCount = %d", s_nTesCount);

		s_nTesCount++;
		Sleep_f(50);
	}

};

_SERVICE_IMPLEMENT ERR_T ThreadTest()
{
	LOGD_F("ThreadTest()");

	CTestThraed1 m_cThead1;
	CTestThraed2 m_cThead2;

	m_cThead1.TStartup();
	m_cThead2.TStartup();

	Sleep_f(3*1000);

	m_cThead1.TClose();
	m_cThead2.TClose();

	return 0;
}

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////
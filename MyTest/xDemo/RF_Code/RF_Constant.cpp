#include "StdAfx.h"

#include "RF_Constant.h"

int g_ThreadSn = 0;

int GetThreadSn()
{
	return ++g_ThreadSn;
}

/************************************************************************/
/*                           CSingelThread_M                            */
/*                               Public                                 */
/************************************************************************/

MSingelThread::MSingelThread()
{
	m_hThread=NULL;
	m_bThread_Enable=false;
	m_nThreadSn = 0;
	m_bThreadRunStatus = false;
};

MSingelThread::~MSingelThread()
{
	TClose();
}

bool MSingelThread::TStartup()
{
	if (m_hThread)
		return FALSE;

	HANDLE hThread;
	UINT uiThreadId = 0;

	m_bThread_Enable = true;
	hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
		0,    // stack
		ThreadProc,   // Thread proc
		this,   // Thread param
		CREATE_SUSPENDED,   // creation mode
		&uiThreadId);   // Thread ID

	if ( NULL == hThread )
		return FALSE;

	m_nThreadSn = GetThreadSn();
	TRACE(_T("\n CSingelThread_BSM_Thread: m_nThreadSn = %d, uiThreadId = %08x start! \r\n"),m_nThreadSn, uiThreadId);

	ResumeThread( hThread );
	m_hThread = hThread;

	return TRUE;
}

void MSingelThread::TClose()
{
	if( NULL == m_hThread )
		return;

	m_bThread_Enable = false;
	if (WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
	{
		TRACE("\n TerminateThread m_hThread = %d", m_hThread);
		TerminateThread(m_hThread, 1L);
	}
	CloseHandle(m_hThread);
	m_hThread = NULL;

	TRACE(_T("\n CSingelThread_BSM_Thread: m_nThreadSn = %d close \r\n"),m_nThreadSn);
	m_nThreadSn = 0;
	m_bThreadRunStatus = false;

	return ;
}

/************************************************************************/
/*                              Protected                               */
/************************************************************************/

UINT MSingelThread::ThreadProc(LPVOID hThis)
{
	MSingelThread* pThis=(MSingelThread*)hThis;
	_ASSERTE( pThis != NULL );

	pThis->ProRun();

	return 1L;
}

void MSingelThread::ProRun()
{
	if ( !brPro() )
	{
		return;
	}
	m_bThreadRunStatus = true;
	while (m_bThread_Enable)
	{
		roundPro();
		Sleep(10);
	}
	arPro();
	m_bThreadRunStatus = false;
}

bool MSingelThread::brPro()
{
	return TRUE;
}

void MSingelThread::roundPro()
{
	;
}

void MSingelThread::arPro()
{
	;
}











/************************************************************************/
/*                           CDoubleThread_M                            */
/*                               Public                                 */
/************************************************************************/


MDoubleThread::MDoubleThread()
{
	m_hThread1 = NULL;
	m_bThread_Enable1 = false;
	m_nThreadSn1 = 0;
	m_bThreadRunStatus1 = false;

	m_hThread2 = NULL;
	m_bThread_Enable2 = false;
	m_nThreadSn2 = 0;
	m_bThreadRunStatus2 = false;
};

MDoubleThread::~MDoubleThread()
{
	TClose1();
	TClose2();
}

bool MDoubleThread::TStartup1()
{
	if (m_hThread1)
		return FALSE;

	HANDLE hThread;
	UINT uiThreadId = 0;

	m_bThread_Enable1 = true;
	hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
		0,    // stack
		ThreadProc1,   // Thread proc
		this,   // Thread param
		CREATE_SUSPENDED,   // creation mode
		&uiThreadId);   // Thread ID

	if ( NULL == hThread )
		return FALSE;

	m_nThreadSn1 = GetThreadSn();
	TRACE(_T("\n CDoublleThread_BSM thread1: m_nThreadSn = %d, uiThreadId= %08x start! \r\n"), m_nThreadSn1, uiThreadId);

	ResumeThread( hThread );
	m_hThread1 = hThread;


	return TRUE;
}

void MDoubleThread::TClose1()
{
	if( NULL == m_hThread1 )
		return;

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);//win32的函数
	long lTmpTime = sysTime.wSecond*1000 + sysTime.wMilliseconds;

	m_bThread_Enable1 = false;
	if (WaitForSingleObject(m_hThread1, 5000) == WAIT_TIMEOUT)
	{
		TRACE("\n TerminateThread m_hThread = %d", m_hThread1);
		TerminateThread(m_hThread1, 1L);
	}
	CloseHandle(m_hThread1);
	m_hThread1 = NULL;

	GetSystemTime(&sysTime);//win32的函数
	lTmpTime = sysTime.wSecond*1000 + sysTime.wMilliseconds - lTmpTime;

	TRACE("\n CDoublleThread_BSM thead1: m_nThreadSn = %d close, m_hThread1 = %d, lCostTime = %d \r\n",m_nThreadSn1, m_hThread1, lTmpTime);
	m_nThreadSn1 = 0;
	m_bThreadRunStatus1 = false;

	return ;
}

bool MDoubleThread::TStartup2()
{
	if (m_hThread2)
		return FALSE;

	HANDLE hThread;
	UINT uiThreadId = 0;

	m_bThread_Enable2 = true;
	hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
		0,    // stack
		ThreadProc2,   // Thread proc
		this,   // Thread param
		CREATE_SUSPENDED,   // creation mode
		&uiThreadId);   // Thread ID

	if ( NULL == hThread )
		return FALSE;

	m_nThreadSn2 = GetThreadSn();
	TRACE(_T("\n CDoublleThread_BSM thread2: m_nThreadSn = %d, uiThreadId= %08x start! \r\n"), m_nThreadSn2, uiThreadId);


	ResumeThread( hThread );
	m_hThread2 = hThread;

	return TRUE;
}

void MDoubleThread::TClose2()
{
	if( NULL == m_hThread2 )
		return;

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);//win32的函数
	long lTmpTime = sysTime.wSecond*1000 + sysTime.wMilliseconds;
	//long lTmpTime2 = time(NULL);

	m_bThread_Enable2 = false;
	if (WaitForSingleObject(m_hThread2, 5000) == WAIT_TIMEOUT)
	{
		TRACE("\n TerminateThread m_hThread = %d", m_hThread2);
		TerminateThread(m_hThread2, 1L);
	}
	CloseHandle(m_hThread2);
	m_hThread2 = NULL;

	GetSystemTime(&sysTime);//win32的函数
	lTmpTime = sysTime.wSecond*1000 + sysTime.wMilliseconds - lTmpTime;
	//lTmpTime2 = time(NULL) - lTmpTime2;

	TRACE("\n CDoublleThread_BSM thead2: m_nThreadSn = %d close, m_hThread2 = %d, lCostTime = %d, lCostTime2 \r\n",m_nThreadSn2, m_hThread2, lTmpTime);
	m_nThreadSn2 = 0;
	m_bThreadRunStatus2 = false;

	return ;
}


/************************************************************************/
/*                              Protected                               */
/************************************************************************/

UINT MDoubleThread::ThreadProc1(LPVOID hThis)
{
	MDoubleThread* pThis=(MDoubleThread*)hThis;
	_ASSERTE( pThis != NULL );

	pThis->ProRun1();

	return 1L;
}

void MDoubleThread::ProRun1()
{
	m_bThreadRunStatus1 = true;
	while (m_bThread_Enable1)
	{
		roundPro1();
		Sleep(10);
	}
	m_bThreadRunStatus2 = false;
}


void MDoubleThread::roundPro1()
{
	;
}

////////////////////////////////////////////////

UINT MDoubleThread::ThreadProc2(LPVOID hThis)
{
	MDoubleThread* pThis=(MDoubleThread*)hThis;
	_ASSERTE( pThis != NULL );

	pThis->ProRun2();

	return 1L;
}

void MDoubleThread::ProRun2()
{
	m_bThreadRunStatus2 = true;
	while (m_bThread_Enable2)
	{
		roundPro2();
		Sleep(10);
	}
	m_bThreadRunStatus2 = false;
}


void MDoubleThread::roundPro2()
{
	;
}


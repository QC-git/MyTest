#pragma once

#include "StdAfx.h"

class MSingelThread  // base model 
{
public:
	MSingelThread();
	~MSingelThread();

	bool TStartup();
	void TClose();

protected:
	static UINT WINAPI ThreadProc(LPVOID hThis);
	void ProRun();

	virtual bool brPro();    //before round (if false ProRun return)
	virtual void roundPro(); //Ñ­»·
	virtual void arPro();    //after  round

private:
	HANDLE m_hThread;
	bool   m_bThread_Enable;
	int m_nThreadSn;

	bool   m_bThreadRunStatus;
};

class MDoubleThread  // base model 
{
public:
	MDoubleThread();
	~MDoubleThread();

	bool TStartup1();
	void TClose1();

	bool TStartup2();
	void TClose2();

protected:
	static UINT WINAPI ThreadProc1(LPVOID hThis);
	void ProRun1();
	virtual void roundPro1(); //Ñ­»·

	static UINT WINAPI ThreadProc2(LPVOID hThis);
	void ProRun2();
	virtual void roundPro2();

private:
	HANDLE m_hThread1;
	bool   m_bThread_Enable1;
	int m_nThreadSn1;
	bool m_bThreadRunStatus1;

	HANDLE m_hThread2;
	bool   m_bThread_Enable2;
	int m_nThreadSn2;
	bool m_bThreadRunStatus2;
};

class RF_CLock
{
public:
	RF_CLock(CRITICAL_SECTION& cs) : m_cs(cs) { EnterCriticalSection(&m_cs); }
	~RF_CLock() { LeaveCriticalSection(&m_cs);; }

private:
	CRITICAL_SECTION& m_cs;

};

#define _RF_LOCK(cs) RF_CLock cLock(cs); 
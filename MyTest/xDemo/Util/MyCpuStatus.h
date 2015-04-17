#pragma once

#include <conio.h>
#include <stdio.h>
#include <windows.h> 

#include "x_thread.h"

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

class MyCpuStatus  
{
public:
	//memory
	MEMORYSTATUS statex;

	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION        SysTimeInfo;
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;
	double                         dbIdleTime;
	double                         dbSystemTime;

	LARGE_INTEGER                  liOldIdleTime;
	LARGE_INTEGER                  liOldSystemTime;
public:
	MyCpuStatus();
	virtual ~MyCpuStatus();
	int GetMemoryStatue(long& lValue);
	int GetCpuStatue(long& lValue);
};


class MyCpuStatusEx : public X::CRoundThread
{
public:
	MyCpuStatusEx();
	~MyCpuStatusEx();

	int Init();

	void SetWnd(HWND hWnd) { m_hWnd = hWnd; }

	int Start();

	void Stop();

	static unsigned __stdcall KeepQyerying(void* hUser) { ((MyCpuStatusEx*)hUser)->KeepQyerying2(); return 1; }

	void KeepQyerying2();

	//Override
	void DoRoundPro();

public:
	int GetStatus(double& dwValue);

	int GetStatusValue() { return m_nValue; }

private:
	void* m_hCpu;

	HWND m_hWnd;

	wchar_t  ListItem[256];

	bool m_bContinueLog;
	bool m_bIsActivity;

	int m_nValue;

	
};

#include "stdafx.h"

#include "MyCpuStatus.h"

//////////////////////////////////CompAbilityState////////////////////////////////////////

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
PROCNTQSI NtQuerySystemInformation;

MyCpuStatus::MyCpuStatus()
{
	memset(&statex,0,sizeof(statex));
	memset(&liOldIdleTime,0,sizeof(liOldIdleTime));
	memset(&liOldSystemTime,0,sizeof(liOldSystemTime));
}

MyCpuStatus::~MyCpuStatus()
{

}
int MyCpuStatus::GetMemoryStatue(long& lValue)
{
	lValue = 0;

	statex.dwLength = sizeof (statex);
	GlobalMemoryStatus(&statex);	
	lValue = statex.dwMemoryLoad;	

	return 0;
}

int MyCpuStatus::GetCpuStatue(long& lValue)
{
	int nStatus = 0;

	LONG ret;
	lValue = 0;

	// 	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
	// 		GetModuleHandle("ntdll"),
	// 		"NtQuerySystemInformation"
	// 		);

	if (!NtQuerySystemInformation)
	{
		NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
			GetModuleHandle(_T("ntdll")),
			(LPCSTR)("NtQuerySystemInformation")
			);
	}

	do 
	{
		nStatus--;
		if (!NtQuerySystemInformation) break;

		nStatus--;
		if (_kbhit()) break;

		// get number of processors in the system
		nStatus--;
		ret = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
		if (ret != NO_ERROR) break;

		// get new system time
		nStatus--;
		ret = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
		if (ret!=NO_ERROR) break;

		// get new CPU's idle time
		nStatus--;
		ret = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
		if (ret != NO_ERROR) break;

		if (liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
		}

		// store new CPU's idle and system time
		liOldIdleTime = SysPerfInfo.liIdleTime;
		liOldSystemTime = SysTimeInfo.liKeSystemTime;

		lValue = (long)dbIdleTime;		
		return 0;
	} while (false);


	return nStatus;
}


////////////////////////////////////MyCpuStatusEx//////////////////////////////////////

#include "CpuUsage/CpuQuery.h"

MyCpuStatusEx::MyCpuStatusEx()
	: m_nValue(0)
	, m_bIsActivity(false)
{
	m_hCpu = new CpuQueryEx;

	this->SetThreadName("MyCpuStatusEx");
}

MyCpuStatusEx::~MyCpuStatusEx()
{
	TRACE("\n ~MyCpuStatusEx()");

	delete (CpuQueryEx*)m_hCpu;

	TRACE("\n ~MyCpuStatusEx() end");
}

int MyCpuStatusEx::Start()
{
//	m_bContinueLog = true;;
// 	unsigned int threadID;
// 	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &MyCpuStatusEx::KeepQyerying, this, 0, &threadID );

	return TStartup() ? 0 : -1;
}

void MyCpuStatusEx::Stop()
{
	//m_bContinueLog = false;

	TClose();
}

int MyCpuStatusEx::Init()
{
	if (m_hCpu)
	{
		CpuQueryEx* pCpu = (CpuQueryEx*)m_hCpu;
		pCpu->Init();

		//const wchar_t str1[] = L"\\Processor(_Total)\\% Processor Time";
		const char str1[] = "\\Processor(_Total)\\% Processor Time";
		pCpu->AddCounterInfo((PCWSTR)str1);
		
		if ( 1 == pCpu->CounterSize() )
		{
			return 0;
		}
	}

	return -1;
}

void MyCpuStatusEx::KeepQyerying2()
{
	//m_bContinueLog = true;

	while(m_bContinueLog)
	{
		if (m_hCpu)
		{
			CpuQueryEx* pCpu = (CpuQueryEx*)m_hCpu;

			double dwValue;
			if ( true == pCpu->GetFirstCpuValue(dwValue) )
			{
				//TRACE("\n dwValue = %.2f", dwValue);
				m_nValue = (int)dwValue;
			}
		}
	}
	

	//m_bContinueLog = false;
}

void MyCpuStatusEx::DoRoundPro()
{
	CpuQueryEx* pCpu = (CpuQueryEx*)m_hCpu;

	double dwValue;
	if ( true == pCpu->GetFirstCpuValue(dwValue) )
	{
		//TRACE("\n dwValue = %.2f", dwValue);
		m_nValue = (int)dwValue;
	}
}

int MyCpuStatusEx::GetStatus(double& dwValue)
{
	if (m_hCpu)
	{
		CpuQueryEx* pCpu = (CpuQueryEx*)m_hCpu;

		return pCpu->GetFirstCpuValue(dwValue) == TRUE ? 0 : -1;
	}

	return -1;
}
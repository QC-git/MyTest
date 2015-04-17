/****************************** Module Header ******************************\
Module Name:  QueryHelper.cpp
Project:      CppCpuUsage
Copyright (c) Microsoft Corporation.

Source Code Query Class which contains functions for Logging CPU Usage

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/en-us/openness/licenses.aspx#MPL
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/
#include "CpuQuery.h"
#include "CpuLog.h"

#pragma comment(lib,"pdh.lib")

CpuQuery::CpuQuery()
{
}

void CpuQuery::Init()
{
    fIsWorking = false;

    time = 0;

    PDH_STATUS status;

    status = PdhOpenQuery(NULL, 0, &query);

    if(status != ERROR_SUCCESS)
    {
        return;
    }

    //Event = CreateEvent(NULL, FALSE, FALSE, L"MyEvent");
	Event = CreateEvent(NULL, FALSE, FALSE, "MyEvent");

    if(Event == NULL)
    {
        return;
    }

    fIsWorking = true;
}

void CpuQuery::AddCounterInfo(PCWSTR name)
{
    if(fIsWorking)
    {
        PDH_STATUS status;
        CpuCounterInfo ci;
        ci.counterName = name;
        status = PdhAddCounter(query, (LPCSTR)ci.counterName, 0 , &ci.counter);

        if(status != ERROR_SUCCESS)
        {
            return;
        }

        vciSelectedCounters.push_back(ci);
    }
}

void CpuQuery::Record()
{
    PDH_STATUS status;
    ULONG CounterType;
    ULONG WaitResult;
    PDH_FMT_COUNTERVALUE DisplayValue;	

    status = PdhCollectQueryData(query);

    if(status != ERROR_SUCCESS)
    {
        return;
    }

    status = PdhCollectQueryDataEx(query, SAMPLE_INTERVAL, Event);

    if(status != ERROR_SUCCESS)
    {
        return;
    }

    WaitResult = WaitForSingleObject(Event, INFINITE);

    if (WaitResult == WAIT_OBJECT_0) 
    {
        for(auto it = vciSelectedCounters.begin(); it < vciSelectedCounters.end(); it++)
        {
            status = PdhGetFormattedCounterValue(it->counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);			

            if(status != ERROR_SUCCESS)
            {
                continue;
            }

            CpuLog log;
            log.time = time;
            log.value = DisplayValue.doubleValue;
            it->logs.push_back(log);				
        }
    }

    time++;
}

bool CpuQueryEx::GetFirstCpuValue(double& dwValue)
{
	PDH_STATUS status;
	ULONG CounterType;
	ULONG WaitResult;
	PDH_FMT_COUNTERVALUE DisplayValue;	

	if ( vciSelectedCounters.size() <= 0 )
	{
		return FALSE;
	}

 	status = PdhCollectQueryData(query);

	if(status != ERROR_SUCCESS)
	{
		return FALSE;
	}

	status = PdhCollectQueryDataEx(query, SAMPLE_INTERVAL, Event);  // 内部会创建线程， 小心使用

	if(status != ERROR_SUCCESS)
	{
		return FALSE;
	}

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);//win32的函数
	long lCosttime = sysTime.wSecond*1000 + sysTime.wMilliseconds;

	WaitResult = WaitForSingleObject(Event, INFINITE);

	GetSystemTime(&sysTime);//win32的函数
	lCosttime = sysTime.wSecond*1000 + sysTime.wMilliseconds - lCosttime;

	if (WaitResult == WAIT_OBJECT_0) 
	{
		auto it = vciSelectedCounters.begin();
		if ( it == vciSelectedCounters.end() )
		{
			return FALSE;
		}

		status = PdhGetFormattedCounterValue(it->counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);	

		if(status != ERROR_SUCCESS)
		{
			return FALSE;
		}

		dwValue = DisplayValue.doubleValue;
		return TRUE;	
	}

	return FALSE;
}
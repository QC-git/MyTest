/*
============================================================================
 Name		 : RKGUI_System.c
 Author	     : 
 Copyright   : Your copyright notice
 Description : 系统相关函数
 Version	 : V1.0
 Date		 : 2009-09-01
 Remark		 :
============================================================================
*/
#include "RKGUI_System.h"
#include <wingdi.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <windows.h>
#include <WinBase.h>

#include "DBWindow.h"

#pragma comment(lib, "Shlwapi")

RKGUI_tchar g_pLogFileName[RKGUI_MAX_PATH] ={0};
RKGUI_tchar g_pAPPFile[RKGUI_MAX_PATH] ={0};

//////////////////////////////////////////////////////////////////////////
//cpu
#define SystemBasicInformation 0 
#define SystemPerformanceInformation 2 
#define SystemTimeInformation 3
#define SystemProcessorPerformanceInformation 8
 
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
 
typedef struct 
{ 
 DWORD dwUnknown1; 
 ULONG uKeMaximumIncrement; 
 ULONG uPageSize; 
 ULONG uMmNumberOfPhysicalPages; 
 ULONG uMmLowestPhysicalPage; 
 ULONG uMmHighestPhysicalPage; 
 ULONG uAllocationGranularity; 
 PVOID pLowestUserAddress; 
 PVOID pMmHighestUserAddress; 
 ULONG uKeActiveProcessors; 
 BYTE bKeNumberProcessors; 
 BYTE bUnknown2; 
 WORD wUnknown3; 
} SYSTEM_BASIC_INFORMATION;
 
typedef struct 
{ 
 LARGE_INTEGER liIdleTime; 
 DWORD dwSpare[76]; 
} SYSTEM_PERFORMANCE_INFORMATION;
 
typedef struct 
{ 
 LARGE_INTEGER liKeBootTime; 
 LARGE_INTEGER liKeSystemTime; 
 LARGE_INTEGER liExpTimeZoneBias; 
 ULONG uCurrentTimeZoneId; 
 DWORD dwReserved; 
} SYSTEM_TIME_INFORMATION;
 
typedef struct
_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
 
typedef struct
_m_PROCESSORS_USE_TIME {
    double dbOldIdleTime;	// save old total time
	double dbOldCurrentTime;
	double dbIdleTime;		// save time after calc
	double dbCurrentTime;
	float fUse;
}m_PROCESSORS_USE_TIME;
 
typedef long (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
PROCNTQSI g_NtQuerySystemInformation = NULL;
m_PROCESSORS_USE_TIME g_mProUserTime[RKGUI_PROCESS_MAXNUMS] ={0};
SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION g_pSPPI[RKGUI_PROCESS_MAXNUMS] ={0};
int g_nProcessNums =0;

/*
============================================================================
 @Description	: 获取系统CPU核数
 @param1		: 
 @Return		:
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetCPUNumbers(int* pProNums)
{
	if (g_nProcessNums > 0)
	{
		return g_nProcessNums;
	}

	if (g_NtQuerySystemInformation == NULL)
	{
		g_NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
		if (g_NtQuerySystemInformation == NULL)
		{
			return -1;
		}
	}

	{
		long status =0; 
		SYSTEM_BASIC_INFORMATION SysBaseInfo;
		// get number of processors in the system 
		status = g_NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL); 
		if (status != NO_ERROR) 
			return -1;
		
		g_nProcessNums = SysBaseInfo.bKeNumberProcessors;
	}

	*pProNums = g_nProcessNums;
	return g_nProcessNums;
}

/*
============================================================================
 @Description	: 获取系统CPU使用率
 @param1		: [out]  pUsePer: cpu using rate
 @Return		: cpu using rate 0-100% 
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetCPU_UseRate( int *pUsePer)
{ 
	static LARGE_INTEGER liOldIdleTime = {0,0}; 
	static LARGE_INTEGER liOldSystemTime = {0,0};

	SYSTEM_BASIC_INFORMATION SysBaseInfo;
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo; 
	SYSTEM_TIME_INFORMATION SysTimeInfo; 
		
	int i=0;
	long lStatus = NO_ERROR;
	double dbIdleTime = 0; 
	double dbSystemTime = 0; 
	double alldbIdleTime = 0;
	int iProcNums = 0;
	 
	if (g_NtQuerySystemInformation == NULL)
	{
		g_NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
		if (g_NtQuerySystemInformation == NULL)
		{
			return -1;
		}
	}
 
	// get number of processors in the system 
	lStatus = g_NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL); 
	if (lStatus != NO_ERROR) 
		return -1;

	iProcNums = SysBaseInfo.bKeNumberProcessors;
	if (iProcNums > RKGUI_PROCESS_MAXNUMS && iProcNums <= 0)
	{
		return -1;
	}

	// get new system time 
	lStatus = g_NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0); 
	if (lStatus!=NO_ERROR) 
		return -1;
 
	// get new CPU's idle time 
	lStatus =g_NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL); 
	if (lStatus != NO_ERROR) 
		return -1;
 
 
	// get ProcessorPer time 
	lStatus =g_NtQuerySystemInformation(SystemProcessorPerformanceInformation, g_pSPPI,
		                                sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * iProcNums, NULL); 
	if (lStatus != NO_ERROR) 
		return -1;
 
	// if it's a first call - skip it 
	if (liOldIdleTime.QuadPart != 0) 
	{ 
		// CurrentValue = NewValue - OldValue 
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime); 
 
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);
 
		// CurrentCpuIdle = IdleTime / SystemTime 
		dbIdleTime = dbIdleTime / dbSystemTime;
 
		// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
 
		//calc Processors
		for ( i = 0; i < iProcNums; i++)
		{
			g_mProUserTime[i].dbCurrentTime = Li2Double(g_pSPPI[i].KernelTime) + Li2Double(g_pSPPI[i].UserTime) + 
										      Li2Double(g_pSPPI[i].DpcTime) + Li2Double(g_pSPPI[i].InterruptTime) -
                                              g_mProUserTime[i].dbOldCurrentTime;
			g_mProUserTime[i].dbIdleTime = Li2Double(g_pSPPI[i].IdleTime) - g_mProUserTime[i].dbOldIdleTime;
 
			// CurrentCpuIdle = IdleTime / SystemTime 
			g_mProUserTime[i].dbIdleTime = g_mProUserTime[i].dbIdleTime / g_mProUserTime[i].dbCurrentTime;
 
			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
			g_mProUserTime[i].dbIdleTime = 100.0 - g_mProUserTime[i].dbIdleTime * 100.0 + 0.5;
 
		}
	}
 
	// store new CPU's idle and system time 
	liOldIdleTime = SysPerfInfo.liIdleTime; 
	liOldSystemTime = SysTimeInfo.liKeSystemTime;
 
	for ( i = 0; i < iProcNums; i++)
	{
		g_mProUserTime[i].dbOldCurrentTime = Li2Double(g_pSPPI[i].KernelTime) + Li2Double(g_pSPPI[i].UserTime) + 
										Li2Double(g_pSPPI[i].DpcTime) + Li2Double(g_pSPPI[i].InterruptTime);
 
		g_mProUserTime[i].dbOldIdleTime = Li2Double(g_pSPPI[i].IdleTime);
	}

	*pUsePer = (int)dbIdleTime;
	return (int)dbIdleTime;
}


/*
============================================================================
 @Description	: 获取系统时间
 @param1		: 返回系统时间
 @Return		:
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: DateTime[0]: 年, DateTime[1]:月...以此类推
============================================================================
*/
RKGUI_void RKGUI_GetSystemTime(int DateTime[6])
{
	SYSTEMTIME systime;
	//GetSystemTime(&systime);
	GetLocalTime(&systime);
	DateTime[0] = systime.wYear;
	DateTime[1] = systime.wMonth;
	DateTime[2] = systime.wDay;
	DateTime[3] = systime.wHour;
	DateTime[4] = systime.wMinute;
	DateTime[5] = systime.wSecond;
}

/*
============================================================================
 @Description	: 获取系统时间(包括星期数)
 @param1		: 返回系统时间
 @Return		:
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: DateTime[0]: 年, DateTime[1]:月...以此类推 DateTime[6]为星期
============================================================================
*/
RKGUI_void RKGUI_GetSystemWeekTime(int DateTime[7])
{
	SYSTEMTIME systime;
	//GetSystemTime(&systime);
	GetLocalTime(&systime);
	DateTime[0] = systime.wYear;
	DateTime[1] = systime.wMonth;
	DateTime[2] = systime.wDay;
	DateTime[3] = systime.wHour;
	DateTime[4] = systime.wMinute;
	DateTime[5] = systime.wSecond;
	DateTime[6] = systime.wDayOfWeek;
}

/*
============================================================================
 @Description	: 设置系统时间
 @param1		: 系统时间
 @Return		:
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: DateTime[0]: 年, DateTime[1]:月...以此类推
============================================================================
*/
RKGUI_void RKGUI_SetSystemTime(int DateTime[6])
{
	SYSTEMTIME systime;
	RKGUI_ulong dw = 0;
	
	RKGUI_memset(&systime, 0, sizeof(SYSTEMTIME));
	systime.wYear = DateTime[0];
	systime.wMonth = DateTime[1];
	systime.wDay = DateTime[2];
	systime.wHour = DateTime[3];
	systime.wMinute = DateTime[4];
	systime.wSecond = DateTime[5];
	
	SetLocalTime(&systime);

}


/*
============================================================================
 @Description	: 获取当前系统时间的星期
 @Return		: 当前是这个星期的第几天 从星期天为0开始
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: DateTime[0]: 年, DateTime[1]:月...以此类推
============================================================================
*/
RKGUI_int RKGUI_GetDayOfWeek(RKGUI_int *pDayOfWeek)
{
	RKGUI_int nDay =0;
	SYSTEMTIME systime;
	//GetSystemTime(&systime);
	GetLocalTime(&systime);
	nDay = (RKGUI_int)systime.wDayOfWeek;
	if (pDayOfWeek)
	{
		*pDayOfWeek = nDay;
	}

	return nDay;
}

/*
============================================================================
 @Description	: 获取当前系统内存状态
 @param1		: 返回总内存
 @param2		: 返回可用物理内存
 @param3		: 返回可用虚拟内存
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_void RKGUI_GetSystemMemoryStatus(int *iTotalMem, int *iAvailPhysMem, int *iAvailVirtualMem)
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memStatus);
	*iTotalMem = memStatus.dwTotalPhys;
	*iAvailPhysMem = memStatus.dwAvailPhys;
	*iAvailVirtualMem = memStatus.dwAvailVirtual;
}

/*
============================================================================
 @Description	: 显示当前系统内存状态
 @param1		: 客户端显存
 @param2		: 显示位置
 @param3		: 
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_void RKGUI_DrawMemoryStatus(HDC hDc, RECT rcShow)
{
	char szShow[256] = {0};
	int iTotalMem,iAvailPhysMem, iAvailVirtualMem;
	RKGUI_GetSystemMemoryStatus(&iTotalMem, &iAvailPhysMem, &iAvailVirtualMem);
	sprintf(szShow, "Total Mem:%d, Avail PhyMem:%d, Avail VirMem:%d",
		    iTotalMem, iAvailPhysMem, iAvailVirtualMem);

	TextOut(hDc, 10, 10, szShow, strlen(szShow));
}



/*
============================================================================
@Description	: 写当前系统内存状态MemoeyStatus.txt
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_PrintSystemMemoryStatus()
{
	FILE *fpCount =RKGUI_NULL;
	RKGUI_tchar cBlockTest[MAX_PATH] = {0};
	SYSTEMTIME m_SysTime;
	MEMORYSTATUS status;
	RKGUI_int nResult = RKGUI_GetModuleAppPath(cBlockTest);
	RKGUI_strcat(cBlockTest, RKGUI_TEXT("MemoeyStatus.txt"));
	fpCount  = RKGUI_fopen(cBlockTest, RKGUI_TEXT("ab+"));
	if (fpCount  != RKGUI_NULL)
	{
		memset(&m_SysTime, 0, sizeof(SYSTEMTIME));
		GetLocalTime(&m_SysTime);
		
		memset(&status, 0, sizeof(MEMORYSTATUS));
		GlobalMemoryStatus(&status);
		
		fprintf(fpCount,"Time: H%d : M%d : S%d 总:%ld, 总物理:%ld, 可用物理:%ld, 总虚拟:%ld, 可用虚拟:%ld\r\n", 
			m_SysTime.wHour, m_SysTime.wMinute, m_SysTime.wSecond, 
			status.dwMemoryLoad, status.dwTotalPhys, status.dwAvailPhys, status.dwTotalVirtual, status.dwAvailVirtual);
		
		fclose(fpCount);
		fpCount = RKGUI_NULL;
	}
	
	return ;
}


/*
============================================================================
 @Description	: 获取当前磁盘剩余空间
 @param1		: 磁盘根目录
 @Return		: 剩余空间
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetDiskFreeSpace(RKGUI_char *pRootPath)
{
	ULARGE_INTEGER uFreeBytesAvailableToCaller;
	ULARGE_INTEGER uTotalNumberOfBytes ; 
	ULARGE_INTEGER uTotalNumberOfFreeBytes;   
	RKGUI_bool bRet = FALSE;
	RKGUI_wchar_t szRootPath[RKGUI_MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pRootPath, -1, szRootPath, RKGUI_MAX_PATH);
	GetDiskFreeSpaceExW(szRootPath, &uFreeBytesAvailableToCaller, &uTotalNumberOfBytes, &uTotalNumberOfFreeBytes);
	return ((int)(uTotalNumberOfFreeBytes.QuadPart/1024/1024));
}

/*
============================================================================
 @Description	: 获取当前硬盘数量
 @param1		: 输出参数 记录返回的硬盘数量
 @Return		: 硬盘数量
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetDiskNums(int *pDiskNum)
{
	int nNums =0;
	int nBuffsize =0;
	char* szDiskName =0;
	if (  0 == pDiskNum )
	{
		return nNums;
	}

	nBuffsize = GetLogicalDriveStrings(0, 0);
	szDiskName = (char *)malloc(nBuffsize+1);
	if (szDiskName)
	{
		memset(szDiskName, 0, nBuffsize+1);
		if (GetLogicalDriveStrings(nBuffsize, szDiskName) > 0)
		{
			int i,nType;
			char *pBuffer = szDiskName;
			for (i=0; i<nBuffsize/4; i++)
			{
				nType = GetDriveType(pBuffer);
				pBuffer += 4;
				if (nType == DRIVE_FIXED || nType == DRIVE_REMOVABLE )
				{
					nNums++;
				}//end if nType		

			}// end for

		}

		free(szDiskName);
		szDiskName=0;
	}

	*pDiskNum = nNums;
	return nNums;
}

/*
============================================================================
 @Description	: 获取当前所有硬盘信息
 @param1		: 输出参数： 记录硬盘信息，内存由外部分配
                  输入参数结构体中RKGUI_AllDisk_st.nDiskNums输入前为所分配内存信息的大小
				  输出后变成获取到的硬盘硬盘数量(如果硬盘数量大于零)
				  nDiskNums 可由RKGUI_GetDiskNums接口先获取                  
 @Return		: 硬盘数量
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetLocalDiskInfo(RKGUI_AllDisk_st* pDiskAllInfo)
{
	int nDiskNums =0;
	int nBuffsize =0;
	char* szDiskName =0;
	if ( 0 == pDiskAllInfo )
	{
		return 0;
	}

	if (pDiskAllInfo->nDiskNums < 1 &&
		pDiskAllInfo->pDiskInfo == 0 )
	{
		return 0;
	}
	
	//通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度。
	nBuffsize = GetLogicalDriveStrings(0, 0);
	if (nBuffsize <1)
	{
		return 0;
	}

	szDiskName = (char *)malloc(nBuffsize+1);
	if (szDiskName)
	{
		//通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
		if (GetLogicalDriveStrings(nBuffsize, szDiskName) > 0)
		{
			int i,nType;
			unsigned _int64 i64FreeBytesToCaller;
			unsigned _int64 i64TotalBytes;
			unsigned _int64 i64FreeBytes;
			char *pBuffer = szDiskName;
			BOOL bStatus = FALSE;

			//为了显示每个驱动器的状态，则通过循环输出实现
			//由于DStr内部保存的数据是A:\NULLB:\NULLC:\NULL，
			//这样的信息，所以DSLength/4可以获得具体大循环范围
			for (i=0; i<nBuffsize/4; i++)
			{
				nType = GetDriveType(pBuffer);
				if (nType == DRIVE_FIXED || nType == DRIVE_REMOVABLE )
				{

					bStatus =GetDiskFreeSpaceEx(pBuffer,(PULARGE_INTEGER)&i64FreeBytesToCaller,
                                                (PULARGE_INTEGER)&i64TotalBytes,(PULARGE_INTEGER)&i64FreeBytes);
					if ( bStatus )
					{
						pDiskAllInfo->pDiskInfo[nDiskNums].uDiskType = nType;
						pDiskAllInfo->pDiskInfo[nDiskNums].dwTotalMBSize = (unsigned long)( (i64TotalBytes / 1024) /1024);
						pDiskAllInfo->pDiskInfo[nDiskNums].dwFreeMBSize  = (unsigned long)( (i64FreeBytesToCaller / 1024) /1024);

						strcpy(pDiskAllInfo->pDiskInfo[nDiskNums].szDiskSign, pBuffer);
						nDiskNums++;
					}

					if (nDiskNums > pDiskAllInfo->nDiskNums)
					{
						break;
					}

				}//end if nType	
				
				pBuffer += 4;
				
			}// end for
		}// end if GetLogicalDriveStrings

		free(szDiskName);
		szDiskName =0;
	}


	pDiskAllInfo->nDiskNums = nDiskNums;
	return nDiskNums;
}

/*
============================================================================
 @Description	: 获取当前系统运行的时间
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_int RKGUI_GetTickCount()
{
	return GetTickCount();
}

/*
============================================================================
@Description	: 获取程序路径
@param1		    : RKGUI_tchar *tcAppPath 返回程序路径,不能为空
@return         : int 路径字节长度
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_GetGlobalModuleAppPath(RKGUI_tchar *pAppFile)
{
	//待返回程序路径不能为空
	int len =0;
	if ( RKGUI_NULL == pAppFile )
	{
		return 0;
	}

	if (g_pAPPFile[0] != 0)
	{
		RKGUI_strcpy(pAppFile, g_pAPPFile);
		return RKGUI_strlen(g_pAPPFile);
	}	

	if(GetModuleFileName(RKGUI_NULL, g_pAPPFile,RKGUI_MAX_PATH) <1)
	{
		return 0;
	}
	
	len = RKGUI_strlen(g_pAPPFile);
	while(g_pAPPFile[len-1] != RKGUI_TEXT('\\') && len>0)
	{
		len--;
	}
	g_pAPPFile[len] = RKGUI_TEXT('\0');

	RKGUI_strcpy(pAppFile, g_pAPPFile);
	return len;
}

/*
============================================================================
@Description	: 获取程序路径
@param1		    : RKGUI_tchar *tcAppPath 返回程序路径,不能为空
@return         : int 路径字节长度
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_GetModuleAppPath(RKGUI_tchar *tcAppPath)
{
	//获取程序运行路径
	RKGUI_tchar tcFilePathName[RKGUI_MAX_PATH] = {0};
	RKGUI_long dwLenth =0;
	RKGUI_int len =0;

	//待返回程序路径不能为空
	if (tcAppPath == RKGUI_NULL)
	{
		return 0;
	}	

	dwLenth = GetModuleFileName(RKGUI_NULL, tcFilePathName,RKGUI_MAX_PATH);
	if(dwLenth <1)
	{
		return 0;
	}
	
	len = RKGUI_strlen(tcFilePathName);
	while(tcFilePathName[len-1] != RKGUI_TEXT('\\') && len>0)
	{
		len--;
	}
	tcFilePathName[len] = RKGUI_TEXT('\0');
	
	RKGUI_strcpy(tcAppPath, tcFilePathName);
	return len;
}

/*
============================================================================
@Description	: 获取程序名称(不带路径)
@param1		    : RKGUI_tchar *tcAppName 返回程序名称,不能为空
@return         : int 名称字节数
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_GetModuleAppName(RKGUI_tchar *tcAppName)
{
	//获取程序运行路径
	RKGUI_tchar tcFilePathName[RKGUI_MAX_PATH] = {0};
	RKGUI_long dwLenth =0;
	RKGUI_int len =0;

	//待返回程序路径不能为空
	if (tcAppName == RKGUI_NULL)
	{
		return 0;
	}	

	dwLenth = GetModuleFileName(RKGUI_NULL, tcFilePathName,RKGUI_MAX_PATH);
	if(dwLenth <1)
	{
		return 0;
	}

	len = RKGUI_strlen(tcFilePathName);
	while(tcFilePathName[len-1] != RKGUI_TEXT('\\') && len>0)
	{
		len--;
	}

	if (len < RKGUI_strlen(tcFilePathName))
	{
		strcpy(tcAppName, &tcFilePathName[len]);
		len = RKGUI_strlen(tcFilePathName)-len;
		tcAppName[len] = RKGUI_TEXT('\0');
	}
	else
	{
		len =0;
	}

	return len;
}

/*
============================================================================
@Description	: 获取文件所处的目录
@param1		    : [in]  文件名(全路径+文件名)
@param2         : [out] 文件所处目录
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_GetFilePath(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFilePath)
{
	int iLen =0;
	if ( 0 == pFileFullPath || 0 == pFilePath )
	{
		return -1;
	}
	{
		iLen = strlen(pFileFullPath);
		strcpy(pFilePath, pFileFullPath);
		while(iLen > 0)
		{
			if ( pFilePath[iLen-1] == '\\' )
			{
				pFilePath[iLen] =0;
				break;
			}
			iLen--;
			pFilePath[iLen] =0;
		}
	}

	return iLen;
}

/*
============================================================================
@Description	: 根据全路径获取文件名称
@param1		    : [in]  文件名(全路径+文件名)
@param2         : [out] 文件名
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_GetFileName(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFileName)
{
	int iLen =0;
	if ( 0 == pFileFullPath || 0 == pFileName )
	{
		return -1;
	}
	{
		iLen = strlen(pFileFullPath);
		while(iLen > 0)
		{
			if ( pFileFullPath[iLen-1] == '\\' )
			{
				break;
			}
			iLen--;
		}

		if (iLen > 1)
		{
			sprintf(pFileName, "%s", pFileFullPath+iLen );
		}
		
	}

	return iLen;
}

/*
============================================================================
@Description	: 用explorer打开文件所在目录
@param1		    : 文件名(全路径+文件名)
@param2		    : 打开方式
@return         : 文件所处目录
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_ExplorerDirectory(const RKGUI_tchar *pFileName, int  nType)
{
	if (pFileName)
	{
		char pDir[RKGUI_MAX_PATH] ={0};
		if (RKGUI_GetFilePath(pFileName, pDir) > 0)
		{
			ShellExecute(NULL, "open",  "explorer.exe",   pDir,  NULL, nType);
			return 1;
		}
	}
	return 0;
}

//获取程序路径宽字符
RKGUI_int RKGUI_GetModuleAppPath_W(RKGUI_wchar_t *wcAppPath)
{
	//获取程序运行路径
	RKGUI_wchar_t wcFilePathName[RKGUI_MAX_PATH] = {0};
	RKGUI_long dwLenth =0;
	RKGUI_int len =0;
	
	//待返回程序路径不能为空
	if (wcAppPath == RKGUI_NULL)
	{
		return 0;
	}	
	
	dwLenth = GetModuleFileNameW(RKGUI_NULL, wcFilePathName,RKGUI_MAX_PATH);
	if(dwLenth <1)
	{
		return 0;
	}
	
	len = wcslen(wcFilePathName);
	while(wcFilePathName[len-1] != RKGUI_TEXTW('\\') && len>0)
	{
		len--;
	}
	wcFilePathName[len] = RKGUI_TEXTW('\0');
	
	wcscpy(wcAppPath, wcFilePathName);
	return len;
}

/*
============================================================================
@Description	: 设置log路径
@param1		    : RKGUI_tchar *tcAppPath 返回程序路径,不能为空
@return         : int 路径字节长度
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_SetLogFilePath()
{
	RKGUI_tchar tcRecordFileName[RKGUI_MAX_PATH] = {0};
	RKGUI_int len =0;

	if (g_pLogFileName[0] != 0)
	{
		return 1;
	}
	len = RKGUI_GetModuleAppPath(tcRecordFileName);
	if (len<4)
	{
		return 0;
	}
	RKGUI_strcat(tcRecordFileName,  RKGUI_TEXT("RKGUI_Log.txt") );
	RKGUI_strcpy(g_pLogFileName, tcRecordFileName);

	return 1;
}

/*
============================================================================
@Description	: 获取程序路径
@param1		    : RKGUI_tchar *tcAppPath 返回程序路径,不能为空
@return         : int 路径字节长度
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_int RKGUI_SaveLogData(RKGUI_uchar *pData, RKGUI_int dwLength, RKGUI_tchar* pFileName)
{
	FILE* fp = NULL;
	RKGUI_tchar tcRecordFileName[RKGUI_MAX_PATH] = {0};
	RKGUI_GetModuleAppPath(tcRecordFileName);
	if (pFileName == NULL)
	{
		RKGUI_strcat(tcRecordFileName, RKGUI_TEXT("\\RKGUI.dat"));
	}
	else
	{
		RKGUI_strcat(tcRecordFileName, pFileName);
	}

	fp = RKGUI_fopen(tcRecordFileName, RKGUI_TEXT("ab+"));
	if (fp)
	{
		fwrite(pData, dwLength, 1, fp);
		fclose(fp);
		fp = NULL;
		return 1;
	}

	return 0;
}

/*
============================================================================
 @Description	: 写log文件
 @param1		: log日志
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_void RKGUI_PutStringToLog(RKGUI_tchar* pLogBuffer, ...)
{
	RKGUI_tchar szLogBuffer[RKGUI_MAX_PATH];
	RKGUI_va_list maker;
	RKGUI_file *fp = RKGUI_NULL;
	RKGUI_tchar tcLogFileName[RKGUI_MAX_PATH] = {0};
	RKGUI_SetLogFilePath();
	RKGUI_strcpy(tcLogFileName, g_pLogFileName);
	RKGUI_va_start(maker, pLogBuffer);
	fp = RKGUI_fopen(tcLogFileName, RKGUI_TEXT("a"));
	if (fp != RKGUI_NULL)
	{
		RKGUI_vsprintf(szLogBuffer, pLogBuffer, maker);
		RKGUI_fputs(szLogBuffer, fp);	
		RKGUI_fputs(RKGUI_TEXT("\n"), fp);
		RKGUI_fclose(fp);
	}
}

/*
============================================================================
@Description	: 写log文件(log时间)
@param1		    : log日志
@Author		    : renjie.li
@Date			: 2009-09-01
@Version		: V1.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_PutStringToLogWidthTime(RKGUI_tchar* pLogBuffer, ...)
{
	RKGUI_tchar szLogBuffer[RKGUI_MAX_PATH];
	RKGUI_va_list maker;
	RKGUI_file *fp = RKGUI_NULL;
	RKGUI_SetLogFilePath();
	RKGUI_va_start(maker, pLogBuffer);
	fp = RKGUI_fopen(g_pLogFileName, RKGUI_TEXT("a"));
	if (fp != RKGUI_NULL)
	{
		int nDateTime[6]={0};
		RKGUI_tchar szTime[32]={0};
		RKGUI_tchar szLog[RKGUI_MAX_PATH]={0};
		RKGUI_GetSystemTime(nDateTime);
		sprintf(szTime, "%02d-%02d:%02d%:%02d",nDateTime[2],nDateTime[3], nDateTime[4], nDateTime[5]);
		RKGUI_vsprintf(szLog, pLogBuffer, maker);
		RKGUI_sprintf(szLogBuffer, "%s  %s", szTime, szLog);
		RKGUI_fputs(szLogBuffer, fp);	
		RKGUI_fputs(RKGUI_TEXT("\n"), fp);
		RKGUI_fclose(fp);
	}
	
	RKGUI_va_end(maker);
}


/*
============================================================================
 @Description	: Utf-8转多字节字符
 @param1		: utf-8字符串
 @param2		: 转换后的多字节字符串（调用者开辟空间）
 @param3		: 多字节字符串长度
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
int RKGUI_UtfToMultiByte(const RKGUI_uchar* utf8, RKGUI_char *ascii, int asciilen)
{
	int len = 0;
	 //UTF-8转UNICODE
	RKGUI_wchar_t *pWChar = RKGUI_NULL;
 	len = MultiByteToWideChar(CP_UTF8, 0, (RKGUI_char*)utf8, -1, RKGUI_NULL,0);
	pWChar = (RKGUI_wchar_t*)RKGUI_malloc(len  * sizeof(RKGUI_wchar_t));
	if (pWChar == RKGUI_NULL)
	{
		return 0;
	}
	RKGUI_memset(pWChar, 0, len  * sizeof(RKGUI_wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, (RKGUI_char*)utf8, -1, pWChar, len);
	len = WideCharToMultiByte(CP_ACP, 0, pWChar, -1, RKGUI_NULL, 0, RKGUI_NULL, RKGUI_NULL); 
	if (len > asciilen)
	{
		len = asciilen;
	}
	WideCharToMultiByte(CP_ACP, 0, pWChar, -1, ascii, len, RKGUI_NULL, RKGUI_NULL);
	if (pWChar)
	{
		RKGUI_free(pWChar);
		pWChar = RKGUI_NULL;
	}
	return len;
}


/*
============================================================================
@Description	: 获取当前文件大小
@param1         : RKGUI_char* pCFileName 指定文件名
@return         : RKGUI_long 当前文件大小 -1表示当前文件不存在 
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_ulong RKGUI_GetFileSize(RKGUI_tchar *pFileName)
{
	RKGUI_ulong lSize = 0;
	FILE *fp = RKGUI_NULL;
	if (pFileName == RKGUI_NULL)
	{
		return 0;
	}
	
	fp = RKGUI_fopen(pFileName, RKGUI_TEXT("rb"));
	if (fp != RKGUI_NULL)
	{
		fseek(fp, 0, SEEK_END);
		lSize = ftell(fp);
		
		fclose(fp);
		fp =NULL;
	}
	else
	{
		return 0;
	}

	return lSize;
}


/*
============================================================================
@Description	: 遍历指定路径和指定扩展名的文件
@param1		    : 指定的路径
@param2		    : 指定扩展名
@param3		    : 搜索的最大文件个数
@param4		    : 返回的搜索到的文件名称
@param5		    : 返回的搜索到的文件个数
@Author		    : renjie.li
@Date			: 2009-12-03
@Version		: V2.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_FindTCFileInDirEx(const RKGUI_tchar* pDirectory, const RKGUI_tchar* pSuffix,
								   RKGUI_int iMaxNum, DDB_FileConfig *pRetPath, RKGUI_int *iRetNum)
{
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	
	RKGUI_tchar filePathName[RKGUI_MAX_PATH] = {0};
    RKGUI_tchar tmpPath[RKGUI_MAX_PATH]={0};
	RKGUI_tchar fdfilename[RKGUI_MAX_PATH]={0};
	RKGUI_int nPathLen =0;
	RKGUI_int nPathNum = *iRetNum;
	RKGUI_bool bSearchFinished = FALSE;

	if (pDirectory == RKGUI_NULL || pRetPath == RKGUI_NULL)
	{
		iRetNum =0;
		return;
	}

	if ( (*iRetNum) >= (iMaxNum))
	{
		return;
	}
	
    RKGUI_strcpy(filePathName, pDirectory);
	nPathLen = RKGUI_strlen(filePathName);
	if (nPathLen<1)
	{
		iRetNum =0;
		return;
	}

	if(filePathName[nPathLen -1] != RKGUI_TEXT('\\'))
    {
		RKGUI_strcat(filePathName, RKGUI_TEXT("\\"));	
	}

	RKGUI_memset(fdfilename, 0, RKGUI_MAX_PATH * sizeof(RKGUI_tchar));
	RKGUI_strcpy(fdfilename, filePathName);
    RKGUI_strcat(fdfilename, RKGUI_TEXT("*"));	//*.*
	RKGUI_memset(&fd, 0, sizeof(WIN32_FIND_DATA));
    hSearch = FindFirstFile(fdfilename, &fd);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

    if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		&& RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
    {
		RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		RKGUI_strcpy(tmpPath, filePathName);
		RKGUI_strcat(tmpPath, fd.cFileName);
		*iRetNum =nPathNum;
		RKGUI_FindTCFileInDirEx(tmpPath, pSuffix, iMaxNum, pRetPath, iRetNum);	// 递归查找所有目录
		nPathNum = *iRetNum;
    }
    else  if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
	{
	   if (pSuffix == RKGUI_NULL || RKGUI_strstr(fd.cFileName, pSuffix) != RKGUI_NULL)	// pSuffix为空时指定为所有文件
	   {
		   if (nPathNum >= (iMaxNum))
		   {
			   bSearchFinished = RKGUI_TRUE;
		   }

		   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		   RKGUI_strcpy(tmpPath, filePathName);
		   RKGUI_strcat(tmpPath, fd.cFileName);
		   RKGUI_strcpy(pRetPath[nPathNum].tcFileName, tmpPath);
		   pRetPath[nPathNum].dwFileLen = fd.nFileSizeLow;
		   nPathNum++;

	   }
	}

	//
    while( !bSearchFinished)
    {
       if( FindNextFile(hSearch, &fd) )
       {
           if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)	// 目录
			   && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
		   {
			   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
			   RKGUI_strcpy(tmpPath, filePathName);
			   RKGUI_strcat(tmpPath, fd.cFileName);
			   *iRetNum =nPathNum;
			   RKGUI_FindTCFileInDirEx(tmpPath, pSuffix, iMaxNum, pRetPath, iRetNum);	// 递归查找所有目录
			   nPathNum = *iRetNum;
           }
		   else if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
		   {
			   if (pSuffix == RKGUI_NULL || RKGUI_strstr(fd.cFileName, pSuffix) != RKGUI_NULL)	// pSuffix为空时指定为所有文件
			   {
				   if (nPathNum >= (iMaxNum))
				   {
					   bSearchFinished = RKGUI_TRUE;
				   }

				   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
				   RKGUI_strcpy(tmpPath, filePathName);
				   RKGUI_strcat(tmpPath, fd.cFileName);
				   RKGUI_strcpy(pRetPath[nPathNum].tcFileName, tmpPath);
				   pRetPath[nPathNum].dwFileLen = fd.nFileSizeLow;
				   nPathNum++;
				}
		   }//
       }
       else
       {
		   if( GetLastError() == ERROR_NO_MORE_FILES )   // 正常结束
		   {
              bSearchFinished = RKGUI_TRUE;
           }
           else
		   {
				bSearchFinished = RKGUI_TRUE;     // 终止查找
		   }
       }
    }
	
	FindClose(hSearch);
	*iRetNum = nPathNum;
}

/*
============================================================================
@Description	: 删除指定路径内所有空文件夹
@param1		    : 指定的路径
@Author		    : renjie.li
@Date			: 2009-12-03
@Version		: V2.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_DeleteEmptyDirectoryEx(const RKGUI_tchar* pDirectory)
{
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	
	RKGUI_tchar filePathName[RKGUI_MAX_PATH] = {0};
    RKGUI_tchar tmpPath[RKGUI_MAX_PATH]={0};
	RKGUI_tchar fdfilename[RKGUI_MAX_PATH]={0};
	
	RKGUI_bool bSearchFinished = FALSE;
	RKGUI_int nPathLen =0;

	if (pDirectory == RKGUI_NULL )
	{
		return;
	}
	
    RKGUI_strcpy(filePathName, pDirectory);

	nPathLen = RKGUI_strlen(filePathName);
	if (nPathLen<1)
	{
		return;
	}

	if(filePathName[nPathLen -1] != RKGUI_TEXT('\\'))
    {
		RKGUI_strcat(filePathName, RKGUI_TEXT("\\"));	
	}

	RKGUI_memset(fdfilename, 0, RKGUI_MAX_PATH * sizeof(RKGUI_tchar));
	RKGUI_strcpy(fdfilename, filePathName);
    RKGUI_strcat(fdfilename, RKGUI_TEXT("*"));	//*.*
	RKGUI_memset(&fd, 0, sizeof(WIN32_FIND_DATA));
    hSearch = FindFirstFile(fdfilename, &fd);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

    if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		&& RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
    {
		RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		RKGUI_strcpy(tmpPath, filePathName);
		RKGUI_strcat(tmpPath, fd.cFileName);
		//递归调用
		RKGUI_DeleteEmptyDirectoryEx(tmpPath);

		//删除文件夹
		RemoveDirectory(tmpPath);
    }

    while( !bSearchFinished)
    {
       if( FindNextFile(hSearch, &fd) )
       {
		   if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			   && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
		   {
			   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
			   RKGUI_strcpy(tmpPath, filePathName);
			   RKGUI_strcat(tmpPath, fd.cFileName);

			   //递归调用
			   RKGUI_DeleteEmptyDirectoryEx(tmpPath);

			   //删除文件夹
			   RemoveDirectory(tmpPath);
		   }
	   }
	   else
	   {
		   bSearchFinished = RKGUI_TRUE;
	   }
	}
	   	
	FindClose(hSearch);
}


/*
============================================================================
@Description	: 删除指定路径内指定扩展名的文件或文件夹
@param1		    : 指定的路径
@param2		    : 指定扩展名
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_ClearFileSuffixInDirEx(const RKGUI_tchar* pDirectory,const RKGUI_tchar* pSuffix)
{
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	
	RKGUI_tchar filePathName[RKGUI_MAX_PATH] = {0};
    RKGUI_tchar tmpPath[RKGUI_MAX_PATH]={0};
	RKGUI_tchar fdfilename[RKGUI_MAX_PATH]={0};
	
	RKGUI_bool bSearchFinished = FALSE;
	RKGUI_int nPathLen =0;
	if (pDirectory == RKGUI_NULL)
	{
		return;
	}

    RKGUI_strcpy(filePathName, pDirectory);

	nPathLen = RKGUI_strlen(filePathName);
	if (nPathLen<4)     //字节小于4是根目录或则
	{
		return;
	}

	if(filePathName[nPathLen -1] != RKGUI_TEXT('\\'))
    {
		RKGUI_strcat(filePathName, RKGUI_TEXT("\\"));	
	}

	RKGUI_memset(fdfilename, 0, RKGUI_MAX_PATH * sizeof(RKGUI_tchar));
	RKGUI_strcpy(fdfilename, filePathName);
    RKGUI_strcat(fdfilename, RKGUI_TEXT("*"));	//*.*
	RKGUI_memset(&fd, 0, sizeof(WIN32_FIND_DATA));
    hSearch = FindFirstFile(fdfilename, &fd);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

    if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		&& RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
    {
		RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		RKGUI_strcpy(tmpPath, filePathName);
		RKGUI_strcat(tmpPath, fd.cFileName);
//		RKGUI_ClearFileSuffixInDirEx(tmpPath,pSuffix);	// 递归查找所有目录

		if (pSuffix == RKGUI_NULL )
		{
			//删除文件夹
			RemoveDirectory(tmpPath);
		}

    }
    else  if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
	{
	   if (pSuffix == RKGUI_NULL || RKGUI_strstr(fd.cFileName, pSuffix) != RKGUI_NULL)	// pSuffix为空时指定为所有文件
	   {
		   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		   RKGUI_strcpy(tmpPath, filePathName);
		   RKGUI_strcat(tmpPath, fd.cFileName);
		   DeleteFile(tmpPath);
	   }
	}

    while( !bSearchFinished)
    {
       if( FindNextFile(hSearch, &fd) )
       {
		   if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			   && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
		   {
			   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
			   RKGUI_strcpy(tmpPath, filePathName);
			   RKGUI_strcat(tmpPath, fd.cFileName);
//			   RKGUI_ClearFileSuffixInDirEx(tmpPath,pSuffix);	// 递归查找所有目录
			   
			   if (pSuffix == RKGUI_NULL )
			   {
				   //删除文件夹
				   RemoveDirectory(tmpPath);
			   }

		   }
		   else  if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
		   {
			   if (pSuffix == RKGUI_NULL || RKGUI_strstr(fd.cFileName, pSuffix) != RKGUI_NULL)	// pSuffix为空时指定为所有文件
			   {
				   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
				   RKGUI_strcpy(tmpPath, filePathName);
				   RKGUI_strcat(tmpPath, fd.cFileName);
				   DeleteFile(tmpPath);
			   }
		   }
       }
       else
       {
		   if( GetLastError() == ERROR_NO_MORE_FILES )   // 正常结束
		   {
              bSearchFinished = RKGUI_TRUE;
           }
           else
		   {
				bSearchFinished = RKGUI_TRUE;     // 终止查找
		   }
       }
    }
	
	FindClose(hSearch);
}


/*
============================================================================
@Description	: 删除指定路径内所有文件或文件夹(不包括目录本身)
@param1		    : 指定的路径
@param2		    : 指定扩展名
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version	    : V2.0
@Remark		    : 
============================================================================
*/
RKGUI_void RKGUI_ClearDirectoryEx(const RKGUI_tchar* pDirectory)
{
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	
	RKGUI_tchar filePathName[RKGUI_MAX_PATH] = {0};
    RKGUI_tchar tmpPath[RKGUI_MAX_PATH]={0};
	RKGUI_tchar fdfilename[RKGUI_MAX_PATH]={0};
	
	RKGUI_bool bSearchFinished = FALSE;
	RKGUI_int nPathLen =0;

	if (pDirectory == RKGUI_NULL)
	{
		return;
	}
	
    RKGUI_strcpy(filePathName, pDirectory);

	nPathLen = RKGUI_strlen(filePathName);
	if (nPathLen<4)     //字节小于4是根目录或则
	{
		return;
	}

	if(filePathName[nPathLen -1] != RKGUI_TEXT('\\'))
    {
		RKGUI_strcat(filePathName, RKGUI_TEXT("\\"));	
	}

	RKGUI_memset(fdfilename, 0, RKGUI_MAX_PATH * sizeof(RKGUI_tchar));
	RKGUI_strcpy(fdfilename, filePathName);
    RKGUI_strcat(fdfilename, RKGUI_TEXT("*"));	//*.*
	RKGUI_memset(&fd, 0, sizeof(WIN32_FIND_DATA));
    hSearch = FindFirstFile(fdfilename, &fd);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

    if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		&& RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
    {
		RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		RKGUI_strcpy(tmpPath, filePathName);
		RKGUI_strcat(tmpPath, fd.cFileName);
		RKGUI_ClearDirectoryEx(tmpPath);	// 递归查找所有目录

		//删除文件夹
		RemoveDirectory(tmpPath);
    }
    else  if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
	{
		//删除文件
		RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
		RKGUI_strcpy(tmpPath, filePathName);
		RKGUI_strcat(tmpPath, fd.cFileName);

		SetFileAttributes(tmpPath,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(tmpPath);
	
	}

    while( !bSearchFinished)
    {
       if( FindNextFile(hSearch, &fd) )
       {
		   if( (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			   && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))    // 目录   
		   {
			   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
			   RKGUI_strcpy(tmpPath, filePathName);
			   RKGUI_strcat(tmpPath, fd.cFileName);
			   RKGUI_ClearDirectoryEx(tmpPath);	// 递归查找所有目录

			   //删除文件夹
			   RemoveDirectory(tmpPath);
		   }
		   else  if( RKGUI_strcmp(fd.cFileName, RKGUI_TEXT(".")) && RKGUI_strcmp(fd.cFileName, RKGUI_TEXT("..")))
		   {
			   //删除文件
			   RKGUI_memset(tmpPath, 0, RKGUI_MAX_PATH*sizeof(RKGUI_tchar));
			   RKGUI_strcpy(tmpPath, filePathName);
			   RKGUI_strcat(tmpPath, fd.cFileName);
			   SetFileAttributes(tmpPath,FILE_ATTRIBUTE_NORMAL);
			   DeleteFile(tmpPath);   
		   }
       }
       else
       {
		   if( GetLastError() == ERROR_NO_MORE_FILES )   // 正常结束
		   {
              bSearchFinished = RKGUI_TRUE;
           }
           else
		   {
				bSearchFinished = RKGUI_TRUE;     // 终止查找
		   }
       }
    }
	
	FindClose(hSearch);
}

/*
============================================================================
 @Description	: 删除指定文件名的文件
 @param1		: 指定的文件名(完整路径)
 @Return		: 返回1:成功, 0:失败
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_DeleteFile(const RKGUI_char *pFileName)
{
	RKGUI_wchar_t FileName[RKGUI_MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pFileName, -1, FileName, RKGUI_MAX_PATH);
	return DeleteFileW(FileName);
}


/*
============================================================================
 @Description	: 确认文件夹目录是否存在
 @param1		: 指定的文件夹名或目录
 @Return		: 返回1:成功, 0:失败
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_CheckDirectory(const RKGUI_tchar *pDirectory)
{
	RKGUI_wchar_t pDirW[RKGUI_MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP, 0, pDirectory, -1, pDirW, RKGUI_MAX_PATH);
	if (PathFileExistsW(pDirW))
	{
		return RKGUI_TRUE;
	}
	return RKGUI_FALSE;
}


/*
============================================================================
 @Description	: 创建文件夹
 @param1		: 指定的文件夹名
 @Return		: 返回1:成功, 0:失败
 @Author		: renjie.li
 @Date			: 2009-09-01
 @Version		: V1.0
 @Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_CreateDirectory(const RKGUI_char *pszFileName)
{
	RKGUI_wchar_t FileName[RKGUI_MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, FileName, RKGUI_MAX_PATH);
	return CreateDirectoryW(FileName, RKGUI_NULL);
}

/*
============================================================================
@Description	: 创建文件夹
@param1		    : 指定的文件夹名
@Return		    : 返回1:成功, 0:失败
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version		: V1.0
@Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_CreateDirectoryT(const RKGUI_tchar* pDirectory)
{
	return CreateDirectory(pDirectory, NULL);
}

/*
============================================================================
@Description	: 创建文件夹目录扩展
@param1		    : 指定的文件夹名
@Return		    : 返回1:成功, 0:失败
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version		: V1.0
@Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_CreateDirectoryEx(const RKGUI_tchar* pDirectory,const char* pDivide)
{
	RKGUI_tchar szFilePath[RKGUI_MAX_PATH]={0};
	RKGUI_tchar *pTmp=pDirectory;
	int nPost =0;
	if ( pDirectory == 0|| pDivide == 0)
	{
		return RKGUI_TRUE;
	}

	while(RKGUI_TRUE)
	{
		pTmp = strstr(pTmp, pDivide);
		if (pTmp == NULL)
		{
			break;
		}
		strcpy(szFilePath, pDirectory);
		pTmp++;
		nPost = pTmp -pDirectory;
		if (nPost >= RKGUI_MAX_PATH)
		{
			break;
		}
		szFilePath[nPost]='\0';
		CreateDirectory(szFilePath, NULL);
		
	}

	return RKGUI_TRUE;

}

/*
============================================================================
@Description	: 弹出路径选择对话框，返回所选路径
@param1		    : 【out】返回选择的文件夹路径 pFilePath
@param2		    : 【in】选择框名称
@Return		    : 【in】返回1:成功, 0:失败
@Author		    : renjie.li
@Date		    : 2009-12-03
@Version		: V1.0
@Remark		: 
============================================================================
*/
RKGUI_bool RKGUI_SelectFilePath(char *pFilePath, char *pTitle, void *lpifq)
{
	BROWSEINFO bi={0};//保存用户选择的目录信息
    LPITEMIDLIST  p = (LPITEMIDLIST)lpifq;
    char path[RKGUI_MAX_PATH] ={0};
	if (pFilePath == RKGUI_NULL)
	{
		return RKGUI_FALSE;
	}
 
    bi.pidlRoot = 0;//对话框的根目录
    bi.pszDisplayName = path;
	//对话框的显示名称
	if (pTitle)
	{
		bi.lpszTitle = pTitle;
	}
	else
	{
		bi.lpszTitle = "请选择路径";
	}
   
    bi.ulFlags = BIF_RETURNONLYFSDIRS | 0x0040 ;//0x0040 BIF_NEWDIALOGSTYLE
    bi.lpfn = 0;
    bi.lParam = 0;
    bi.iImage = 0;
	if (lpifq)
	{
		bi.pidlRoot = p;
	}
  
    p=SHBrowseForFolder(&bi); //弹出选择路径对话框
    if(p)
    {
        if(SHGetPathFromIDList(p,path))
        {
			strcat(path, "\\");
			strcpy(pFilePath,path);
			return RKGUI_TRUE;
        }
	}

	return RKGUI_FALSE;
}

/*
============================================================================
@Description	: 只启动一个应用程序
@Return		    : 返回1:成功, 0:失败
@Author		    : renjie.li
@Date			: 2009-09-01
@Version		: V1.0
@Remark		    : 
============================================================================
*/
HANDLE RKGUI_hmutex;
RKGUI_bool RKGUI_CreateOnlyOneApp(RKGUI_wchar_t *pMutexName)
{
	HWND hWnd = NULL;
	int iReturn = -1;
	RKGUI_hmutex = CreateMutexW(NULL, TRUE, pMutexName);	// 创建互斥对象
	if (RKGUI_hmutex == INVALID_HANDLE_VALUE)
	{// 创建失败
		return RKGUI_FALSE;
	}
	else
	{// 创建成功
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{
			return RKGUI_FALSE;
		}
		else
		{
			ReleaseMutex(RKGUI_hmutex);
			RKGUI_hmutex =0;
			return RKGUI_TRUE;
		}
	}

	return RKGUI_FALSE;
}


// 启动指定程序
RKGUI_bool RKGUI_OpenProcess(RKGUI_tchar *pProcName)
{
	RKGUI_bool bRet = RKGUI_FALSE;
	if (RKGUI_NULL == pProcName || RKGUI_strlen(pProcName) < 1)
	{
		return RKGUI_FALSE;
	}

	{
		int nStrlen = RKGUI_strlen(pProcName);
		RKGUI_wchar_t *pwtProName = (RKGUI_wchar_t *)malloc((nStrlen+1) * sizeof(RKGUI_wchar_t));
		if (pProcName)
		{
			SHELLEXECUTEINFOW stShellInfo ={0};
			MultiByteToWideChar(CP_ACP, 0, pProcName, -1, pwtProName, nStrlen+1);
			stShellInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
			stShellInfo.hwnd = 0; 
			stShellInfo.lpVerb = 0; 
			stShellInfo.lpFile = pwtProName; 
			stShellInfo.nShow = SW_SHOWNORMAL; 
			stShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS; 
			if (ShellExecuteExW(&stShellInfo))
			{
				bRet = RKGUI_TRUE;
			}
			
 			free(pwtProName);
 			pwtProName =RKGUI_NULL;
		}
	}



	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// 注册服务器监测
typedef struct tagTPData
{
	RKGUI_int nType;
	RKGUI_uint nProcessId;
	char pFile[MAX_PATH];
}TPData, *PTPData;


/*
============================================================================
@Description	: 注册和反注册服务到指定监测程序
@param1		    : 【in】bRegister true：注册    false：反注册
@param2		    : 【in】pServerName  监测管道名称
@param3		    : 【in】pWatchName   待监测程序全路径
@Return		    : 返回1:成功, 0:失败
@Author		    : renjie.li
@Date			: 2009-09-01
@Version		: V1.0
@Remark		    : 
============================================================================
*/
RKGUI_bool RKGUI_RegisterServer(RKGUI_bool bRegister, RKGUI_tchar *pServerName, RKGUI_tchar *pWatchName)
{

	//DwLog("RKGUI_RegisterServer()");

	RKGUI_bool bRet = RKGUI_FALSE;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	RKGUI_uint bytesWrite = sizeof(TPData);
	TPData stTPData = {0};
	if (RKGUI_NULL == pServerName || RKGUI_NULL == pWatchName)
	{

		return bRet;
	}

	if ( !(WaitNamedPipe(pServerName, NMPWAIT_WAIT_FOREVER)) )
	{
		return bRet;
	}

	hPipe = CreateFile(pServerName, GENERIC_WRITE, FILE_SHARE_WRITE,
		                      0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		return bRet;
	}
	
	if (bRegister)
	{
		stTPData.nType = 110;
	}
	else
	{
		stTPData.nType = 111;
	}

	//DwLog("    stTPData.nType = %d", stTPData.nType);
	stTPData.nProcessId = 0;
	RKGUI_strcpy(stTPData.pFile, pWatchName);
	if ( WriteFile(hPipe, &stTPData, sizeof(TPData), &bytesWrite, NULL) )
	{
		bRet = (bytesWrite == sizeof(TPData));
	}
	else
	{
		DWORD dwRet = GetLastError();
		printf("GetLastError:%d", dwRet);
	}

	CloseHandle(hPipe);
	hPipe = INVALID_HANDLE_VALUE;		
	return bRet;

}

//How to use:
//YUV_TO_RGB24(pY,width,pU,pV,width>>1,pRGBBuf,width,(int)0-height,width*3);
static uint64_t mmw_mult_Y    = 0x2568256825682568;
static uint64_t mmw_mult_U_G  = 0xf36ef36ef36ef36e;
static uint64_t mmw_mult_U_B  = 0x40cf40cf40cf40cf;
static uint64_t mmw_mult_V_R  = 0x3343334333433343;
static uint64_t mmw_mult_V_G  = 0xe5e2e5e2e5e2e5e2;

static uint64_t mmb_0x10      = 0x1010101010101010;
static uint64_t mmw_0x0080    = 0x0080008000800080;
static uint64_t mmw_0x00ff    = 0x00ff00ff00ff00ff;

static uint64_t mmw_cut_red   = 0x7c007c007c007c00;
static uint64_t mmw_cut_green = 0x03e003e003e003e0;
static uint64_t mmw_cut_blue  = 0x001f001f001f001f;

/*
============================================================================
 @Description	: YUV411转RGB24
 @param1	: 
 @Return	:
 @Author	: renjie.li
 @Date		: 2009-09-01
 @Version	: V1.0
 @Remark	: 
============================================================================
*/
void YUV_TO_RGB24(uint8_t *pYUV_Y, int nLines_Y, uint8_t *pYUV_U,
                  uint8_t *pYUV_V, int nLines_UV, uint8_t *pRGB,
                  int nWidth, int nHeight,int nLineBytes) 
{
	int y, horiz_count;
    
	if (nHeight < 0) {
		//we are flipping our output upside-down
		nHeight  = -nHeight;
		pYUV_Y     += (nHeight   - 1) * nLines_Y ;
		pYUV_U     += (nHeight/2 - 1) * nLines_UV;
		pYUV_V     += (nHeight/2 - 1) * nLines_UV;
		nLines_Y  = -nLines_Y;
		nLines_UV = -nLines_UV;
	}

	horiz_count = -(nWidth >> 3);

	for (y=0; y<nHeight; y++) {
		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, pRGB       
			mov ebx, pYUV_Y       
			mov ecx, pYUV_U       
			mov edx, pYUV_V
			mov edi, horiz_count

			horiz_loop:

			movd mm2, [ecx]
			pxor mm7, mm7

			movd mm3, [edx]
			punpcklbw mm2, mm7       

			movq mm0, [ebx]          
			punpcklbw mm3, mm7       

			movq mm1, mmw_0x00ff     

			psubusb mm0, mmb_0x10    

			psubw mm2, mmw_0x0080    
			pand mm1, mm0            

			psubw mm3, mmw_0x0080    
			psllw mm1, 3             

			psrlw mm0, 8             
			psllw mm2, 3             

			pmulhw mm1, mmw_mult_Y   
			psllw mm0, 3             

			psllw mm3, 3             
			movq mm5, mm3            

			pmulhw mm5, mmw_mult_V_R 
			movq mm4, mm2            

			pmulhw mm0, mmw_mult_Y   
			movq mm7, mm1            

			pmulhw mm2, mmw_mult_U_G 
			paddsw mm7, mm5

			pmulhw mm3, mmw_mult_V_G
			packuswb mm7, mm7

			pmulhw mm4, mmw_mult_U_B
			paddsw mm5, mm0      

			packuswb mm5, mm5
			paddsw mm2, mm3          

			movq mm3, mm1            
			movq mm6, mm1            

			paddsw mm3, mm4
			paddsw mm6, mm2

			punpcklbw mm7, mm5
			paddsw mm2, mm0

			packuswb mm6, mm6
			packuswb mm2, mm2

			packuswb mm3, mm3
			paddsw mm4, mm0

			packuswb mm4, mm4
			punpcklbw mm6, mm2

			punpcklbw mm3, mm4

			// 32-bit shuffle.
			pxor mm0, mm0

			movq mm1, mm6
			punpcklbw mm1, mm0

			movq mm0, mm3
			punpcklbw mm0, mm7

			movq mm2, mm0

			punpcklbw mm0, mm1
			punpckhbw mm2, mm1

			// 24-bit shuffle and sav
			movd   [eax], mm0
			psrlq mm0, 32

			movd  3[eax], mm0

			movd  6[eax], mm2


			psrlq mm2, 32            

			movd  9[eax], mm2        

			// 32-bit shuffle.
			pxor mm0, mm0            

			movq mm1, mm6            
			punpckhbw mm1, mm0       

			movq mm0, mm3            
			punpckhbw mm0, mm7       

			movq mm2, mm0            

			punpcklbw mm0, mm1       
			punpckhbw mm2, mm1       

			// 24-bit shuffle and sav
			movd 12[eax], mm0        
			psrlq mm0, 32            

			movd 15[eax], mm0        
			add ebx, 8               

			movd 18[eax], mm2        
			psrlq mm2, 32            

			add ecx, 4               
			add edx, 4               

			movd 21[eax], mm2        
			add eax, 24              

			inc edi
			jne horiz_loop

			pop edi
			pop edx
			pop ecx
			pop ebx
			pop eax

			emms
		}

		pYUV_Y   += nLines_Y;
		if (y%2)
		{
			pYUV_U   += nLines_UV;
			pYUV_V   += nLines_UV;
		}
		pRGB += nLineBytes; 
	}
}

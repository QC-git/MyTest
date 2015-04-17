
#include "x_util.h"

#ifdef _WIN32

#include <tchar.h>
#include <stdarg.h>
#include <time.h>

#else
#endif

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

#ifdef _WIN32

#define MAX_BUF_LEN 4096
#define LOG_OUT_FILEPATH _T(".\\Log")  // 使用.\ 文件前面会多一个小点

_SERVICE_IMPLEMENT VOID_T Print_f()
{
	printf("\n no implement");
}

_SERVICE_IMPLEMENT BOOL_T GetDataTimeString_f(CHAR_T *pStrDT)
{
	//BOOL bRet = FALSE;
	struct tm *newtime = NULL;
	time_t long_time;

	if (pStrDT == NULL) 
		return FALSE;

	time( &long_time );                /* Get time as long integer. */
	newtime = localtime( &long_time ); /* Convert to local time. */

	if (newtime == NULL)
		return FALSE;

	sprintf_s(pStrDT, 36, "%02d-%02d-%02d-%02d%02d%02d", \
		newtime->tm_year+1900, newtime->tm_mon, newtime->tm_mday,\
		newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
	//
	return TRUE_D;
}

#if X_SERVICE_HAVE_LOGFILE > 0

class CLogFile
{
public:
	CLogFile(LPCTSTR lpPath)
	{
		strcpy_s (m_strPath, _T(""));
		m_hLogFile = NULL;
		//
		if (lpPath != NULL)		
			strcpy_s (m_strPath, lpPath);
		else
		{
			TCHAR   sdaytime[36] = {0};
			GetDataTimeString_f(sdaytime);
			sdaytime[35] = '\n';

			strcpy_s(m_strPath, LOG_OUT_FILEPATH);
			strcat_s(m_strPath, _T(" "));
			strcat_s(m_strPath, sdaytime);
			strcat_s(m_strPath, _T(".log"));
		}
		Open();
	}
	~CLogFile()
	{
		Close();
	}
	BOOL WriteString(LPCTSTR lpString)
	{
		Open();
		if (lpString != NULL && m_hLogFile != NULL)
		{
			fputs(lpString, m_hLogFile);			
			fflush(m_hLogFile);
		}
		Close();
		return TRUE;
	}
private:
	BOOL Open()
	{
		if ((m_hLogFile == NULL) && (strlen(m_strPath)>0))
		{
			m_hLogFile = fopen(m_strPath, "a+");
		}	
		return (m_hLogFile == NULL)? FALSE : TRUE;
	}
	BOOL Close()
	{
		if (m_hLogFile != NULL)
		{
			fclose(m_hLogFile);
			m_hLogFile = NULL;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
private:
	FILE *m_hLogFile;
public:
	TCHAR m_strPath[MAX_PATH];
};

static CLogFile s_cLogFile(NULL);

_SERVICE_EXPORT VOID_T Log_f(E_LOG_LEVEL emLevel, const CHAR_T* sFmt, ...)
{
	//printf("\n no implement");
	
	

	// 翻译可变参数区

	//LPCTSTR debugFmt = (LPCTSTR)sFmt;
	TCHAR	sTmp[MAX_BUF_LEN] = {0};
	va_list vaArg;
	va_start(vaArg, (LPCTSTR)sFmt);  // 采用 debugFmt 会产生乱码 ？ // (LPCTSTR)sFmt 和 sFmt 都正常
	_vsntprintf(sTmp, MAX_BUF_LEN, (LPCTSTR)sFmt, vaArg);
	va_end(vaArg);

	// 获取等级字符串

	TCHAR   sLevel[36] = {0};
	switch (emLevel)
	{
	case E_LOG_LEVEL_NORMAL:
		sprintf_s(sLevel, 36, "[DB-LOG]");
		break;
	case E_LOG_LEVEL_DEBUG:
		sprintf_s(sLevel, 36, "[DB-DEBUG]");
		break;
	case E_LOG_LEVEL_WARNING:
		sprintf_s(sLevel, 36, "[DB-WANING]");
		break;
	case E_LOG_LEVEL_ERROR:
		sprintf_s(sLevel, 36, "[DB-ERROR]");
		break;
	default:
		sprintf_s(sLevel, 36, "[DB-LOG]");
		break;
	}

	PRINT_F("\n\n%s %s", sLevel, sTmp);

	if ( emLevel > E_LOG_LEVEL_NORMAL )
	{
		// 获取日期
		TCHAR   sDataTime[36] = {0};
		GetDataTimeString_f(sDataTime);

		TCHAR   sMsg[MAX_BUF_LEN] = {0};
		sprintf_s(sMsg, MAX_BUF_LEN, "\n\n%s %s %s", sDataTime, sLevel, sTmp);

		s_cLogFile.WriteString(sMsg);
	}
	
}

VOID_T UtilTest1()
{
	for (int n=0; n<50; n++)
	{
		_SERVICE_LOG_F(E_LOG_LEVEL_NORMAL, "%d", n+1, n+1);
	}

	for (int n=0; n<50; n++)
	{
		_SERVICE_LOG_F(E_LOG_LEVEL_DEBUG, "%d", n);
	}

	for (int n=0; n<50; n++)
	{
		_SERVICE_LOG_F(E_LOG_LEVEL_WARNING, "123");
	}

	const CHAR_T* s = "hello";
	for (int n=0; n<50; n++)
	{
		printf("\n%s-%s", s, "qqq");
		_SERVICE_LOG_F(E_LOG_LEVEL_ERROR, "%s-%s", s, "ddd");
	}
}
#endif // X_SERVICE_HAVE_LOGFILE > 0

VOID_T UtilTest2()
{
	for (int n=0; n<50; n++)
	{
		LOG_F("%d", n+1, n+1);
	}

	for (int n=0; n<50; n++)
	{
		LOGD_F("%d", n);
	}

	for (int n=0; n<50; n++)
	{
		LOGW_F("123");
	}

	const CHAR_T* s = "hello";
	for (int n=0; n<50; n++)
	{
		//printf("\n%s-%s", s, "qqq");
		LOGERR_F("%s-%s", s, "ddd");
	}
}

class CPrintImpl: public CPrint
{
public:
	virtual CPrint& operator<<(const char* str)
	{
		if ( NULL == str ) 
			str = "(NULL)"; // sanity check
		PRINT_F("%s", str);
		return *this;
	}

	virtual CPrint& operator<<(int i)
	{
		PRINT_F("%d", i);
		return *this;
	}

	virtual CPrint& operator<<(unsigned u)
	{
		PRINT_F("%u", u);
		return *this;
	}

	virtual CPrint& operator<<(double d)
	{
		PRINT_F("%f", d);
		return *this;
	}

	virtual CPrint& operator<<(void* p)
	{
		PRINT_F("%p", p);
		return *this;
	}

};

static CPrintImpl s_cPrintImpl;

CPrint& g_cPrint(s_cPrintImpl);

_SERVICE_IMPLEMENT CPrint& GetPrint_f()
{
	return g_cPrint;
}

VOID_T UtilTest3()
{
	s_cPrintImpl<<1<<2<<"hello"<<11.23<<'a';

	s_cPrintImpl<<"\n";

	s_cPrintImpl<<1+10000<<2<<"hello"<<11.23<<"a";

	s_cPrintImpl<<"\n";

	//

	g_cPrint<<1<<2<<"hello"<<11.23<<'a';

	g_cPrint<<"\n";

	g_cPrint<<1+10000<<2<<"hello"<<11.23<<"a";

	g_cPrint<<"\n";
}

_SERVICE_IMPLEMENT ERR_T UtilTest()
{
#if X_SERVICE_HAVE_LOGFILE > 0
	//UtilTest1();
#endif

	UtilTest2();

	//UtilTest3();

	return 0;
}

#else

_SERVICE_IMPLEMENT VOID_T Print_f()
{
	printf("\n no implement");
}

_SERVICE_IMPLEMENT VOID_T Log_f()
{
	printf("\n no implement");
}

#endif

_SERVICE_IMPLEMENT TIME_T GetTime_f()
{
#ifdef _WIN32
	return (TIME_T)time(NULL);
#else
	timeval sysTime;
	gettimeofday(&sysTime, NULL);
	return (TIME_T)sysTime.tv_sec;
#endif
}

_SERVICE_IMPLEMENT TIME_T GetMsTime_f()
{
#ifdef _WIN32
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);//win32的函数
	return (TIME_T)(sysTime.wSecond*1000 + sysTime.wMilliseconds);
#else
	timeval sysTime;
	gettimeofday(&sysTime, NULL);
	return (TIME_T)(sysTime.tv_sec * 1000 + sysTime.tv_usec / 1000);
#endif
}

_SERVICE_IMPLEMENT VOID_T Sleep_f(TIME_T lMs)
{
#ifdef _WIN32
	Sleep((DWORD)lMs);
#else
	usleep(uiTime_ms*1000);
#endif
}

_SERVICE_IMPLEMENT CHAR_T* StrDup_f(const CHAR_T* sSrc)
{
	if ( NULL == sSrc ) 
		return NULL;

	size_t nLen = strlen(sSrc) + 1;
	char* sDst = new char[nLen];

	if ( NULL != sDst) 
	{
		memcpy(sDst, sSrc, nLen);
	}
	return sDst;
}

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////
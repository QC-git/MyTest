#include "stdafx.h"
#include "DBWindow.h"

//#ifdef _DEBUG

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>



#define MAX_BUF_LEN 4096
// 灰化
#define DARK_WRITE		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
#define DARK_GREEN		FOREGROUND_GREEN
#define DARK_BLUE		FOREGROUND_BLUE
#define DARK_RED		FOREGROUND_RED
// 高亮
#define INTENSITY_WRITE		FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
#define INTENSITY_GREEN		FOREGROUND_INTENSITY|FOREGROUND_GREEN
#define INTENSITY_BLUE		FOREGROUND_INTENSITY|FOREGROUND_BLUE
#define INTENSITY_RED		FOREGROUND_INTENSITY|FOREGROUND_RED

char gLevelInfo[][32] = {"Normal", "Warning", "Error"};

BOOL GetDataTimeString(char *pStrDT)
{
	BOOL bRet = FALSE;
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
	return TRUE;
}

BOOL GetDataTimeStr(char *pStrDT)
{
	BOOL bRet = FALSE;
	struct tm *newtime = NULL;
	time_t long_time;

	if (pStrDT == NULL) 
		return FALSE;

	time( &long_time );                /* Get time as long integer. */
	newtime = localtime( &long_time ); /* Convert to local time. */

	if (newtime == NULL)
		return FALSE;

	sprintf_s(pStrDT, 36, "%02d-%02d-%02d %02d:%02d:%02d", \
		newtime->tm_year+1900, newtime->tm_mon, newtime->tm_mday,\
		newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
	//
	return TRUE;
}

class ConsoleWindow 
{
public:
	ConsoleWindow();
	virtual ~ConsoleWindow();

	BOOL SetTile(LPCTSTR lpTile);
	BOOL WriteString(LPCTSTR lpString);
	BOOL WriteString(WORD Attrs,LPCTSTR lpString);
private:
	HANDLE m_hConsole;
	BOOL   m_bCreate;
	BOOL   m_bAttrs;
	WORD   m_OldColorAttrs; 
};

ConsoleWindow::ConsoleWindow()
{
	m_hConsole=NULL;
	m_bCreate=FALSE;
	if(AllocConsole())
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle(CONSOLE_TILE);
		SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT);
		m_bCreate=TRUE;
	}
	else
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		if(m_hConsole==INVALID_HANDLE_VALUE)
			m_hConsole=NULL;
	}
	if(m_hConsole)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
		if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
		{
			m_bAttrs=TRUE;
			m_OldColorAttrs = csbiInfo.wAttributes;      
		}
		else{
			m_bAttrs=FALSE;
			m_OldColorAttrs = 0;
		}

		//直接设置高亮红
		SetConsoleTextAttribute(m_hConsole,INTENSITY_RED);
		LPCTSTR lpString = 
			"|----------------------------------------------------------|\n"
			"|                  欢迎使用RFID定位服务端                  |\n"
			"|----------------------------------------------------------|\n\n";
		WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
	}
}

ConsoleWindow::~ConsoleWindow()
{
	if(m_bCreate)
		FreeConsole();
}

BOOL ConsoleWindow::SetTile(LPCTSTR lpTile)
{
	return SetConsoleTitle(lpTile);
}

BOOL ConsoleWindow::WriteString(LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
	}
	return ret;
}

BOOL ConsoleWindow::WriteString(WORD Attrs,LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,Attrs);
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
	}
	return ret; 
}


class CLogToFile
{
public:
	CLogToFile(LPCTSTR lpPath)
	{
		strcpy_s (m_strPath, _T(""));
		m_hLogFile = NULL;
		//
		if (lpPath != NULL)		
			strcpy_s (m_strPath, lpPath);
		else
		{
			TCHAR   sdaytime[36] = {0};
			GetDataTimeString(sdaytime);
			sdaytime[35] = '\n';
			
			strcpy_s(m_strPath, LOG_OUT_FILEPATH);
			strcat_s(m_strPath, _T(" "));
			strcat_s(m_strPath, sdaytime);
			strcat_s(m_strPath, _T(".log"));
		}
		Open();
	}
	~CLogToFile()
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

#ifdef ENABLE_LOG_WIN
ConsoleWindow ConWindow;
#endif
#ifdef ENABLE_LOG_FILE
CLogToFile logFile(NULL);
#endif

const TCHAR* __cdecl  DBGetLogFilePath()
{
	return logFile.m_strPath;
}

#ifdef ENABLE_LOG
BOOL DBWindowTile(LPCTSTR tile)
{
	BOOL bRet = FALSE;

#ifdef ENABLE_LOG_WIN
	bRet = ConWindow.SetTile(tile);
#endif

#ifdef ENABLE_LOG_FILE
#endif	
	return bRet;
}

BOOL DBGWrite(LPCTSTR fmt,...)
{
	BOOL bRet = FALSE;
	TCHAR   sdaytime[36] = {0};
	TCHAR   message[MAX_BUF_LEN] = {0};
	TCHAR	strTemp[MAX_BUF_LEN] = {0};
	va_list cur_arg;

	va_start(cur_arg,fmt);
	_vsntprintf(strTemp,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);

	GetDataTimeStr(sdaytime);
	sprintf_s(message, MAX_BUF_LEN, "%s %s", sdaytime, strTemp);
	
#ifdef ENABLE_LOG_WIN
	bRet = ConWindow.WriteString(message);
#endif

#ifdef ENABLE_LOG_FILE_ALL
	bRet = logFile.WriteString(message);
#endif
	
	return bRet;
}

BOOL DBGWrite(int nLevel,LPCTSTR fmt,...)
{
	BOOL bRet = FALSE;
	WORD Attrs = 0;
	TCHAR   sdaytime[36] = {0};
	TCHAR   message[MAX_BUF_LEN] = {0};
	TCHAR   strTemp[MAX_BUF_LEN] = {0};
	va_list cur_arg;

	if (nLevel < LOG_2OUTPUT_LEVEL)
		return FALSE;

	va_start(cur_arg,fmt);
	_vsntprintf(strTemp,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);

	GetDataTimeStr(sdaytime);
	if (nLevel < LOG_LEVEL_MAX)
		sprintf_s(message, MAX_BUF_LEN, "%s %s %s", sdaytime, gLevelInfo[nLevel], strTemp);
	else
		sprintf_s(message, MAX_BUF_LEN, "%s %s", sdaytime, strTemp);			

	switch (nLevel)
	{
		case LOG_LEVEL_NORMAL:
			Attrs = DARK_WRITE;
			break;
		case LOG_LEVEL_WARNING:
			Attrs = DARK_GREEN;
			break;
		case LOG_LEVEL_ERROR:
			Attrs = INTENSITY_RED;
			break;
	}	

#ifdef ENABLE_LOG_WIN
	bRet = ConWindow.WriteString(Attrs,message); 
#endif//ENABLE_LOG_WIN

#ifdef ENABLE_LOG_FILE
	if (nLevel >= LOG_WRITE_FILE_LEVEL)
		bRet = logFile.WriteString(message);
#endif//ENABLE_LOG_FILE

	return bRet;	
}
#endif//ENABLE_LOG

//#endif // DEBUG


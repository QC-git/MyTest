#ifndef _DBWINDOW_
#define _DBWINDOW_

#include <windows.h>
#ifdef _DEBUG
#define ENABLE_LOG
#endif//_DEBUG
#ifdef ENABLE_LOG
#define ENABLE_LOG_WIN
#define ENABLE_LOG_FILE
#define ENABLE_LOG_FILE_ALL
#endif


// LOG LEVEL
enum LOG_LEVEL
{
	LOG_LEVEL_NORMAL	= 0,
	LOG_LEVEL_WARNING  = 1,
	LOG_LEVEL_ERROR		= 2,
	LOG_LEVEL_MAX,
};

// If current log's level above the defined value could be wrote into file.
#define LOG_2OUTPUT_LEVEL  LOG_LEVEL_NORMAL
#define LOG_WRITE_FILE_LEVEL LOG_LEVEL_ERROR

#define CONSOLE_TILE _T("DBWindow")
#define LOG_OUT_FILEPATH _T(".\\ServerLog")  // ʹ��.\ �ļ�ǰ����һ��С��

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_LOG

//���ÿ���̨������ڱ���
BOOL __cdecl  DBWindowTile(LPCTSTR tile);
//��ʽ���ı����
BOOL __cdecl  DBGWrite(LPCTSTR fmt,...);

const TCHAR* __cdecl  DBGetLogFilePath();

//����ɫ��ʽ���ı����
//BOOL __cdecl  DBGWrite(int nLevel,LPCTSTR fmt,...);

#define DB_LOG_F(fmt, ...) DBGWrite("[DB-LOG] "fmt" \n\n", __VA_ARGS__)

#else

#define DBWindowTile
#define DBGWrite
#define DB_LOG_F

#endif

#ifdef __cplusplus
};
#endif

#endif
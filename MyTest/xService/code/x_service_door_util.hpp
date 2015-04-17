
#ifdef X_SERVICE_USE_PRAGMAONCE
#pragma once
#endif

#ifndef _SERVOCE_DOOR_UTIL_HEADER
#define _SERVOCE_DOOR_UTIL_HEADER

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

#if X_SERVICE_HAVE_SPEPRINT > 0
_SERVICE_EXPORT VOID_T Print_f();
#	define PRINT_F _SERVICE_NAME::Print_f
#else
#	ifdef _WIN32
#		ifdef _MFC_VERSION
#			define PRINT_F TRACE
#		else
#			define PRINT_F printf
#		endif
#	else // linux
#		define PRINT_F printf
#	endif
#endif

#if X_SERVICE_HAVE_LOGFILE > 0
enum E_LOG_LEVEL
{
	E_LOG_LEVEL_NORMAL	= 0,
	E_LOG_LEVEL_DEBUG    = 1,
	E_LOG_LEVEL_WARNING  = 2,
	E_LOG_LEVEL_ERROR    = 3
};
_SERVICE_EXPORT VOID_T Log_f(E_LOG_LEVEL emLevel, const CHAR_T* sFmt, ...);
#	define _SERVICE_LOG_F _SERVICE_NAME::Log_f
#	define LOG_F(fmt, ...)    _SERVICE_LOG_F(_SERVICE_NAME::E_LOG_LEVEL_NORMAL,  fmt, __VA_ARGS__)
#	define LOGD_F(fmt, ...)   _SERVICE_LOG_F(_SERVICE_NAME::E_LOG_LEVEL_DEBUG,   fmt, __VA_ARGS__)
#	define LOGW_F(fmt, ...)   _SERVICE_LOG_F(_SERVICE_NAME::E_LOG_LEVEL_WARNING, fmt, __VA_ARGS__)
#	define LOGERR_F(fmt, ...) _SERVICE_LOG_F(_SERVICE_NAME::E_LOG_LEVEL_ERROR,   fmt, __VA_ARGS__)
#else
#	define LOG_F(fmt, ...)    PRINT_F("\n\n[DB-LOG] "fmt,     __VA_ARGS__)
#	define LOGD_F(fmt, ...)   PRINT_F("\n\n[DB-DEBUG] "fmt,   __VA_ARGS__)
#	define LOGW_F(fmt, ...)   PRINT_F("\n\n[DB-WARNING] "fmt, __VA_ARGS__)
#	define LOGERR_F(fmt, ...) PRINT_F("\n\n[DB-ERROR] "fmt,   __VA_ARGS__)
#endif

#if X_SERVICE_HAVE_CMDWIN > 0
#	ifdef _MFC_VERSION
#		undef  X_SERVICE_HAVE_CMDWIN
#		define X_SERVICE_HAVE_CMDWIN 0
#	endif
#endif

class CPrint
{
public:
	virtual CPrint& operator<<(const char* str) = 0;
	virtual CPrint& operator<<(int i) = 0;
	virtual CPrint& operator<<(unsigned u) = 0;
	virtual CPrint& operator<<(double d) = 0;
	virtual CPrint& operator<<(void* p) = 0;
};

_SERVICE_EXPORT CPrint& GetPrint_f();
#define _SERVICE_OUT_F _SERVICE_NAME::GetPrint_f()
#define OUT_F _SERVICE_OUT_F

/*
************************************************************************************************************************
*                                                    GetDataTimeString_f
*
* Description: ��ȡ�ַ�����ʱ��
*
* Arguments  : �ڴ�
*
* Returns    : �ɹ�����true
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT BOOL_T GetDataTimeString_f(CHAR_T *pDataBuff); 

/*
************************************************************************************************************************
*                                                    GetTime_f
*
* Description: ��ȡ�뼶ʱ��
*
* Arguments  : ��
*
* Returns    : ����ʱ��
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT TIME_T GetTime_f();

/*
************************************************************************************************************************
*                                                    GetMsTime_f
*
* Description: ��ȡ���뼶ʱ��
*
* Arguments  : ��
*
* Returns    : ����ʱ��
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT TIME_T GetMsTime_f();

/*
************************************************************************************************************************
*                                                    Sleep_f
*
* Description: ����
*
* Arguments  : ������뼴ʱ���
*
* Returns    : ��
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT VOID_T Sleep_f(TIME_T lMs); 

/*
************************************************************************************************************************
*                                                    StrDup_f
*
* Description: �ַ�������
*
* Arguments  : ԭ�ַ���
*
* Returns    : �����µ��ַ�������Ҫ�ֶ��ͷ�
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT CHAR_T* StrDup_f(const CHAR_T* sSrc);

/*
************************************************************************************************************************
*                                                    UtilTest
*
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT ERR_T UtilTest();

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif
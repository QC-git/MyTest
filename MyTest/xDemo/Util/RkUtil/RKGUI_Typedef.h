#ifndef __RKGUI_DDBTYPEDEF_H_
#define __RKGUI_DDBTYPEDEF_H_
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


#define  RKGUI_DEBUG				0				// 调试版本
#define	 RKGUI_MEMORY_MENAGE		0				// 内存管理
#define  RKGUI_UNICODE_STRING		1				// 0:不支持UINICODE函数，1支持UINICODE函数


#ifdef _UNICODE
#define _RKGUI_DABTS_PLAMFORM_TYPE_
#endif

#if RKGUI_MEMORY_MENAGE == 0
#define RKGUI_malloc		malloc
#define RKGUI_realloc		realloc
#define RKGUI_free			free
#define RKGUI_calloc		calloc
#elif RKGUI_MEMORY_MENAGE == 1
#define RKGUI_sys_malloc	malloc
#define RKGUI_sys_free		free
#define RKGUI_malloc		autonavi_malloc
#define RKGUI_realloc		autonavi_realloc
#define RKGUI_free			autonavi_free
#define RKGUI_calloc		autonavi_calloc
#endif

#define RKGUI_memcpy		memcpy
#define RKGUI_memset		 memset
#define RKGUI_file			FILE
#define RKGUI_fread			fread
#define RKGUI_fclose		fclose
#define RKGUI_fwrite		fwrite
#define RKGUI_fseek			fseek
#define RKGUI_ftell			ftell
#define RKGUI_va_start		va_start
#define RKGUI_va_end		va_end
#define RKGUI_va_arg		va_arg



#ifdef _RKGUI_DABTS_PLAMFORM_TYPE_
//CE
#define RKGUI_TEXT(LS)		(L##LS)
#define RKGUI_strcpy		wcscpy
#define RKGUI_strstr		wcsstr				
#define RKGUI_strcmp		wcscmp				
#define RKGUI_strcat		wcscat	
#define RKGUI_strlen		wcslen
#define RKGUI_sprintf		wsprintfW
#define RKGUI_strupr		_wcsupr	
#define RKGUI_strncpy		wcsncpy
#define RKGUI_strchr		wcschr
#define RKGUI_strncmp		wcsncmp
#define RKGUI_fopen			_wfopen				
#define RKGUI_fputs			fputws
#define RKGUI_fprintf		fwprintf
#define RKGUI_vsprintf		vswprintf			
#else
//PC
#define RKGUI_TEXT(LS)		LS
#define RKGUI_strcpy		strcpy				
#define RKGUI_strstr		strstr				
#define RKGUI_strcmp		strcmp				
#define RKGUI_strcat		strcat
#define RKGUI_strlen		strlen	
#define RKGUI_sprintf		sprintf
#define RKGUI_strncpy		strncpy
#define RKGUI_strchr		strchr
#define RKGUI_strncmp		strncmp
#define RKGUI_fopen			fopen		
#define RKGUI_fputs			fputs
#define RKGUI_fprintf		fprintf
#define RKGUI_vsprintf		vsprintf	
#endif

#define RKGUI_TEXTW(LS)		(L##LS) 
#if RKGUI_UNICODE_STRING == 1
#define RKGUI_strcpyW		wcscpy				
#define RKGUI_strstrW		wcsstr				
#define RKGUI_strcmpW		wcscmp				
#define RKGUI_strcatW		wcscat
#define RKGUI_strlenW		wcslen	
#define RKGUI_sprintfW		wsprintfW
#define RKGUI_struprW		_strlwr	
#define RKGUI_strncpyW		wcsncpy
#define RKGUI_strchrW		wcschr
#define RKGUI_strncmpW		wcsncmp
#define RKGUI_fopenW		_wfopen		
#define RKGUI_fputsW		fputws
#define RKGUI_fprintfW		wprintf
#define RKGUI_vsprintfW		vswprintf
#endif
// 
// #if RKGUI_DEBUG == 1
// #include "RKGUI_Debug.h"
// #define RKGUI_LOG(s)						RKGUI_PutStringToLog s
// #else
// #define RKGUI_LOG(s)						RKGUI_NULL
// #endif

//////////////////////////////////////////////////////////////////////////
// 调试记录出错位置的宏
// #define RKGUI_STMT_START do
// #define RKGUI_STMT_END   while(0)
// 
// #if RKGUI_DEBUG == 1
// #define RKGUI_return_if_fail(expr)			RKGUI_STMT_START{		\
// 	if (expr) { } else												\
// 	{																\
// 	RKGUI_LOG(														\
// 	("file %s: line %d: assertion `%s' failed",						\
// 	__FILE__,														\
// 	__LINE__,														\
// #expr));															\
// 	return;															\
// };	}RKGUI_STMT_END
// 
// #define RKGUI_return_val_if_fail(expr, val)	RKGUI_STMT_START{		\
// 	if (expr) { } else												\
// 	{																\
// 	RKGUI_LOG(														\
// 	("file %s: line %d: assertion `%s' failed",						\
// 	__FILE__,														\
// 	__LINE__,														\
// #expr));															\
// 	return (val);													\
// };	}RKGUI_STMT_END
// 
// #define RKGUI_return_if_reached()		RKGUI_STMT_START{			\
// 	RKGUI_LOG(														\
// 	("file %s: line %d: should not be reached",						\
// 	__FILE__,														\
// 	__LINE__));														\
// 	return;}RKGUI_STMT_END
// 	
// #define RKGUI_return_val_if_reached(val)	RKGUI_STMT_START{		\
// 	RKGUI_LOG(														\
// 	("file %s: line %d: should not be reached",						\
// 	__FILE__,														\
// 	__LINE__));														\
//      return (val);}RKGUI_STMT_END
// #else
// #define RKGUI_return_if_fail(expr)			RKGUI_STMT_START{ if (expr){}else return; }RKGUI_STMT_END
// #define RKGUI_return_val_if_fail(expr, val)	RKGUI_STMT_START{ if (expr){}else return(val); }RKGUI_STMT_END
// #define RKGUI_return_if_reached()			RKGUI_STMT_START{ return; }RKGUI_STMT_END
// #define RKGUI_return_val_if_reached(val)	RKGUI_STMT_START{ return (val); }RKGUI_STMT_END
// #endif

	

	
#define  RKGUI_MAX_PATH 256
	
#define RKGUI_TRUE					1
#define RKGUI_FALSE					0
typedef unsigned char				RKGUI_bool;
typedef void						RKGUI_void;
typedef char						RKGUI_char;
typedef unsigned char				RKGUI_uchar;
typedef signed char					RKGUI_schar;
typedef short						RKGUI_short;
typedef unsigned short				RKGUI_ushort;
typedef long						RKGUI_long;
typedef unsigned long				RKGUI_ulong;
typedef int							RKGUI_int;
typedef unsigned int				RKGUI_uint;
typedef float						RKGUI_float;
typedef double						RKGUI_double;
typedef char*						RKGUI_va_list;
#define RKGUI_const					const
	
#ifdef __cplusplus
#define RKGUI_NULL					0
#else
#define RKGUI_NULL					((RKGUI_void *)0)
#endif
	
typedef RKGUI_ushort				RKGUI_wchar_t;
#ifdef _RKGUI_DABTS_PLAMFORM_TYPE_ 
	typedef RKGUI_ushort				RKGUI_tchar;
#else
	typedef RKGUI_char					RKGUI_tchar;
#endif
	
#define RKGUI_MIN( a, b )		    ( (a) < (b) ? (a) : (b) )
#define RKGUI_MAX( a, b )			( (a) > (b) ? (a) : (b) )


	
//字节定义
//定义位数(字节数)为8(1)，16(2)，32(4)的数据类型
typedef  char                bits8;
typedef  unsigned char       ubits8;	
typedef  short int           bits16;
typedef  unsigned short int  ubits16;
typedef  int                 bits32;
typedef  unsigned int        ubits32;
	

#ifdef __cplusplus
}
#endif
#endif

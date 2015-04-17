#include "x_config.h"

#ifndef _SERVICE_DOOR_LOCK
#define _SERVICE_DOOR_LOCK
#endif

// #ifdef X_SERVICE_USE_PRAGMAONCE
// #pragma once
// #endif

#ifndef _SERVOCE_DOOR_HEADER
#define _SERVOCE_DOOR_HEADER


//////////////////////////////////////////////////////////////////////////
//规则
// *   defined   :  普通 大写 + _D   函数 大写 + _F
// *   typedef   :  普通 大写 + _T   结构体或类 大小写 + _t
// *   enum      :  EM_ + 大写
// *   struct    :  S + 大小写
// *   class     :  C + 大小写   类中类，大小写
// *   全局函数  :  大小写 + _f

// *   逻辑文件头文件包含参照下述标准：
// *   头文件：线性包含，同类door文件建议只能包含一个
// *   源文件：核文件(mfc)，本头文件，其它文件

// *   门文件头文件包含参照下述标准：
// *   头文件：线性包含，工具文件
// *   源文件：门文件，本头文件，其它文件

// *   驱动文件头文件包含参照下述标准：
// *   头文件：线性包含，核文件，继承文件，工具文件，其它文件
// *   源文件：本头文件，工具文件，其它文件

//////////////////////////////////////////////////////////////////////////


// 命名空间

#define X_SERVICE_VERSION              1.0
#define X_SERVICE_VERSION_STR          "1.0"
#define X_SERVICE_VERSION_NUM          100
#define X_SERVICE_VERSION_COUNT_F(x,y) (x*100+y)

#ifndef X_SERVICE_NAME 
#define X_SERVICE_NAME X
#endif

#define _SERVICE_NAME X_SERVICE_NAME

#define _SERVOCE_DOOR_BEGIN namespace _SERVICE_NAME{
#define _SERVOCE_DOOR_END   }

#define _SERVOCE_CLASS       //namespace X_SERVICE_NAME    // 头文件用
#define _SERVOCE_CLASS_BEGIN //namespace X_SERVICE_NAME{   // 源文件用
#define _SERVOCE_CLASS_END   //}

#ifdef _WIN32 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stdafx.h"
#	if 0
#		include <windows.h>
#	endif
#else
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#ifdef _WIN32 
typedef signed char             S8_T;
typedef unsigned char           U8_T;
typedef signed short           S16_T;
typedef unsigned short         U16_T;
typedef signed int             S32_T;
typedef unsigned int           U32_T;
typedef signed __int64         S64_T;
typedef unsigned __int64       U64_T;
//typedef unsigned char       CHAR_T;
typedef char                  CHAR_T;
typedef int                   BOOL_T;
typedef long                  TIME_T;
typedef void                  VOID_T;
typedef int                    ERR_T;
#define TRUE_D   1
#define FALSE_D  0
#else // Linux
typedef signed char             S8_T;
typedef unsigned char           U8_T;
typedef signed short           S16_T;
typedef unsigned short         U16_T;
typedef signed int             S32_T;
typedef unsigned int           U64_T;
typedef char                  CHAR_T;
typedef int                   BOOL_T;
typedef long                  TIME_T;
typedef void                  VOID_T;
typedef int                    ERR_T;
#	if _MIPS_SZLONG == 64
#		define _PTR_IS_64_BIT  1
#		define _LONG_IS_64_BIT 1
typedef signed long            S64_T;
typedef unsigned long          U64_T;
#	else
typedef signed long long       S64_T;
typedef unsigned long long     U64_T;
#	endif
#endif

#ifdef X_SERVICE_HAVE_EXPORT
#	ifdef _WIN32
#		define _SERVICE_EXPORT          extern "C" __declspec(dllexport)
#		define _SERVICE_CLASS           __declspec(dllexport)
#		define _SERVICE_TAMPLATE_CLASS
#		define _SERVICE_IMPLEMENT 
#	else
#		define _SERVICE_EXPORT
#		define _SERVICE_CLASS
#		define _SERVICE_TAMPLATE_CLASS
#		define _SERVICE_IMPLEMENT
#	endif
#else
#	define _SERVICE_EXPORT
#	define _SERVICE_CLASS
#	define _SERVICE_TAMPLATE_CLASS
#	define _SERVICE_IMPLEMENT
#endif

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN
_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////

#ifndef X_CONFIG
#error  "no config file"
#endif

#if X_SERVICE_VERSION_NUM < X_SERVICE_VERSION_COUNT_F(0,1)
#error  "version too old"
#endif

#endif
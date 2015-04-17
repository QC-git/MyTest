/*
//   Global_Define.h
//   Globe define for project
*/

#ifndef __Global_Define_H__
#define __Global_Define_H__
#include "SQLAPI.h"

//#define APP_TEST_VERSION
#define DATEOUT_YEAR		2015
#define DATEOUT_MONTH		8
#define DATEOUT_DAY			30

#define VERSIONLOG		"V 1.0.707"
#define VERSIONDATE		"20140825"

#ifdef _DEBUG
#ifdef APP_TEST_VERSION
#define CLIENT_VERSION			_T(VERSIONLOG"DT @ "VERSIONDATE)
#else
#define CLIENT_VERSION			_T(VERSIONLOG"D @ "VERSIONDATE)
#endif // APP_TEST_VERSION
#else
#ifdef APP_TEST_VERSION
#define CLIENT_VERSION			_T(VERSIONLOG"T @ "VERSIONDATE)
#else
#define CLIENT_VERSION			_T(VERSIONLOG" @ "VERSIONDATE)
#endif //APP_TEST_VERSION
#endif// _DEBUG
#define CLIENT_COPYRIGHT		_T("Copyright (C) 2012 Ropeok")
#define CLIENT_TITLE				_T("湄公河联合巡逻执法信息管理——智能空间定位系统服务端")

/*======================================================
//常数定义区
======================================================*/
//for CClassView
#define  LOG_LIST_MAXNUM  200

//for CFileView
#define LAYER_IMGINDEX_DEVICEALL					0
#define LAYER_IMGINDEX_DEVICE_DISABLE			1
#define LAYER_IMGINDEX_DEVICE_DETECT		    2
#define LAYER_IMGINDEX_DEVICE_UNDETECT		3
#define LAYER_IMGINDEX_DEVICE_NORMAL		4
#define LAYER_IMGINDEX_DEVICE_PLAY				5
#define LAYER_IMGINDEX_NUM							6  //图片索引总数


/*======================================================
//消息值定义区
======================================================*/
//device list message
#define  ID_DEVLIST_STARTDETECT  (WM_USER + 41)
#define  ID_DEVLIST_ENDDETECT    (WM_USER + 42)
#define  ID_DEVLIST_ONSHOW       (WM_USER + 43)
#define  ID_DEVLIST_OFFSHOW      (WM_USER + 44)
#define  ID_DEVLIST_TAGSINFO      (WM_USER + 45)
#define  ID_DEVLIST_SELECTDETECTMETHOD  (WM_USER + 46)

#define ID_OUTPUTWND_LIST_ALARMLOG WM_USER+10000

#define WM_DEVICE_OFFLINE  WM_USER+11000

/*======================================================
//全局变量声明
======================================================*/
extern CRITICAL_SECTION g_csReConDevice;

//extern SAConnection g_cSaConnection;

#endif
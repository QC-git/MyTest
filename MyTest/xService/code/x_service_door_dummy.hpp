
//#include "x_service_door.hpp"  // 用于测试直接添加该文件是否会出错

#ifdef X_SERVICE_USE_PRAGMAONCE
#pragma once
#endif

#ifndef _SERVOCE_DOOR_DUMMY_HEADER
#define _SERVOCE_DOOR_DUMMY_HEADER

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

/*
************************************************************************************************************************
*                                                    DummyTest
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT ERR_T DummyTest();

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif
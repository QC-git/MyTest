
//#include "x_service_door.hpp"  // ���ڲ���ֱ����Ӹ��ļ��Ƿ�����

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
_SERVICE_EXPORT ERR_T DummyTest();

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif

#ifdef X_SERVICE_USE_PRAGMAONCE
#pragma once
#endif

#ifndef _SERVOCE_DOOR_BOOST_HEADER
#define _SERVOCE_DOOR_BOSST_HEADER

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN

	/*
	************************************************************************************************************************
	*                                                    ContainerTest
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
	_SERVICE_EXPORT ERR_T BoostTest();

_SERVOCE_DOOR_END
	//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif
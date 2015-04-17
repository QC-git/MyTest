
//#include "x_service_door_dummy.hpp"  // 用于测试直接添加该文件是否会出错
#include "x_dummy.h"

#include "x_util.h"
#include "x_thread.h"
#include "x_container.h"
#include "x_boost.h"

//#include <map>

_SERVOCE_DOOR_BEGIN

_SERVICE_IMPLEMENT ERR_T DummyTest()
{
	LOG_F("DummyTest()");

	//qqqqqqq();

	return 0;
}

_SERVOCE_DOOR_END